#include "sysdata.h"

#include "keyboard.h"

#include <stdio.h>
#include <string.h>

SYS_Data global_sysdata = {0};

bool SYS_Check_CRC32(SYS_Data *sysdata);
uint32_t CRC32_Calc(const uint8_t *data, uint32_t length);

void SYS_Clear_Data(SYS_Data *sysdata) {
    if (sysdata == NULL) {
        return;
    }
    memset(sysdata, 0, sizeof(SYS_Data));
}

bool SYS_Update_Status(SYS_Data *sysdata) {
    if (sysdata->dataOffset == sysdata->dataSize) {
        // 校验数据
        if (SYS_Check_CRC32(sysdata)) {
            // 设置接收完成标志
            sysdata->status = SYS_RECEIVE_FINISHED;
            return true;
        } else {
            // 校验失败
            sysdata->status = SYS_RECEIVE_ERROR;
            return false;
        }
    } else if (sysdata->dataOffset > sysdata->dataSize) {
        // 接收数据长度超出总数据长度
        return false;
    } else {
        // 设置继续接收标志
        sysdata->status = SYS_RECEIVE_CONTINUE;
    }
    return true;
}

bool SYS_Receive_Data(SYS_Data *sysdata, uint8_t *buffer, uint32_t size) {
    if (sysdata == NULL || buffer == NULL || size != PACKET_SIZE) {
        return false;
    }
    // uint8_t report_id = buffer[0];
    uint8_t index = buffer[1];
    uint8_t length = buffer[2];

    if (length > PACKET_SIZE - PACKET_HEAD_SIZE) {
        // 数据长度错误,不能超过最大长度
        return false;
    }

    /* 第一个数据包处理 */
    if (index == 0) {
        // 第一包数据至少大于数据总长度的字节数 + crc32字节数
        if (length < DATASIZE_LENGTH + CRC32_LENGTH) {
            return false;
        }

        // 清除数据
        SYS_Clear_Data(sysdata);

        // 第一个数据包，包含数据总长度
        sysdata->dataSize = *(uint32_t *)(buffer + PACKET_HEAD_SIZE);
        sysdata->crc32 = *(uint32_t *)(buffer + PACKET_HEAD_SIZE + DATASIZE_LENGTH);
        if (sysdata->dataSize > MAX_SYS_DATA_SIZE) {
            // 数据长度超出缓存最大长度
            return false;
        }

        // 第一个数据包
        memcpy(sysdata->data, buffer + PACKET_HEAD_SIZE + DATASIZE_LENGTH + CRC32_LENGTH,
               length - DATASIZE_LENGTH - CRC32_LENGTH);
        sysdata->tailIndex = index;
        sysdata->dataOffset = length - DATASIZE_LENGTH - CRC32_LENGTH;

        // 更新状态
        if (!SYS_Update_Status(sysdata)) {
            return false;
        }
        return true;
    } else {
        /* 后续数据包处理 */
        if (length + sysdata->dataOffset > MAX_SYS_DATA_SIZE) {
            return false;
        }

        if (index == sysdata->tailIndex + 1 && sysdata->status == SYS_RECEIVE_CONTINUE) {
            memcpy(sysdata->data + sysdata->dataOffset, buffer + PACKET_HEAD_SIZE, length);
            sysdata->tailIndex = index;
            sysdata->dataOffset += length;

            // 更新状态
            if (!SYS_Update_Status(sysdata)) {
                return false;
            }
            return true;
        } else {
            // 非连续的数据包 ,通讯出错
            return false;
        }
    }
}

void SYS_Process_Data(SYS_Data *sysdata) {
    if (sysdata == NULL) {
        return;
    }

    if (sysdata->status != SYS_RECEIVE_FINISHED) {
        return;
    }

    // 解析命令
    SYS_Cmd cmd;
    memset(&cmd, 0, sizeof(SYS_Cmd));
    cmd.cmdType = (SYS_CmdType)sysdata->data[sysdata->iterator];
    sysdata->iterator++;
    cmd.cmdData = sysdata->data + sysdata->iterator;

    // 执行命令
    sysdata->iterator += SYS_Run_Cmd(&cmd);

    if (sysdata->iterator >= sysdata->dataSize) {
        sysdata->iterator = 0;
    }
}

bool SYS_Check_CRC32(SYS_Data *sysdata) {
    if (sysdata == NULL) {
        return false;
    }
    uint32_t crc32 = 0;
    // 计算数据部分的CRC32
    crc32 = CRC32_Calc(sysdata->data, sysdata->dataSize);

    return (crc32 == sysdata->crc32);
}

uint32_t CRC32_Calc(const uint8_t *data, uint32_t length) {
    if (data == NULL) {
        return 0;
    }

    uint32_t crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < length; i++) {
        crc ^= (uint32_t)data[i];
        for (uint32_t j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc ^ 0xFFFFFFFF;
}

uint32_t SYS_Run_Cmd(SYS_Cmd *cmd) {
    if (cmd == NULL) {
        return 0;
    }
    uint32_t iterator = 0;
    switch (cmd->cmdType) {
    case SYS_CMD_KEYBOARD:
        iterator = SYS_CMD_KEYBOARD_SIZE;
        HID_Keyboard_Data empty_keyboard_data;
        memset(&empty_keyboard_data, 0, sizeof(HID_Keyboard_Data));

        HID_Keyboard_Send((HID_Keyboard_Data *)cmd->cmdData);
        SYS_Delay_ms(20);
        HID_Keyboard_Send(&empty_keyboard_data);
        SYS_Delay_ms(50);

        break;
    case SYS_CMD_MOUSE:
        iterator = SYS_CMD_MOUSE_SIZE;
        break;
    case SYS_CMD_DELAY:
        SYS_Delay_ms(*(uint32_t *)cmd->cmdData);
        iterator = SYS_CMD_DELAY_SIZE;
        break;
    default:
        break;
    }
    for (uint32_t i = 0; i < iterator; i++) {
        printf("%d ", cmd->cmdData[i]);
    }
    printf("\n");
    return iterator;
}

void SYS_Delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}
