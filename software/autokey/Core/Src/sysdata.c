#include "sysData.h"

#include "keyboard.h"

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>

#include "ee.h" 

SYS_Data g_sysData = {0};

SYS_Data ee_sysData = {0};

uint8_t g_runStatus = STATUS_SETTING;

bool SYS_Check_CRC32(SYS_Data *sysData);
uint32_t CRC32_Calc(const uint8_t *data, uint32_t length);

void SYS_Init(void) {
    // 初始化EE
    SYS_Clear_Data(&g_sysData);

    ee_init(&ee_sysData, sizeof(SYS_Data));
    ee_read();

    memcpy(&g_sysData, &ee_sysData, sizeof(SYS_Data));
}

void SYS_Save_Data(SYS_Data *sysData) {
    if (sysData == NULL) {
        return;
    }
    memcpy(&ee_sysData, sysData, sizeof(SYS_Data));
    ee_write();
}

void SYS_Clear_Data(SYS_Data *sysData) {
    if (sysData == NULL) {
        return;
    }
    memset(sysData, 0, sizeof(SYS_Data));
}

bool SYS_Update_Status(SYS_Data *sysData) {
    if (sysData->dataOffset == sysData->dataSize) {
        // 校验数据
        if (SYS_Check_CRC32(sysData)) {
            // 设置接收完成标志
            sysData->status = SYS_RECEIVE_FINISHED;
            SYS_Save_Data(sysData);
            return true;
        } else {
            // 校验失败
            sysData->status = SYS_RECEIVE_ERROR;
            return false;
        }
    } else if (sysData->dataOffset > sysData->dataSize) {
        // 接收数据长度超出总数据长度
        return false;
    } else {
        // 设置继续接收标志
        sysData->status = SYS_RECEIVE_CONTINUE;
    }
    return true;
}

bool SYS_Receive_Data(SYS_Data *sysData, uint8_t *buffer, uint32_t size) {
    if (sysData == NULL || buffer == NULL || size != PACKET_SIZE) 
        return false;
    
    // 运行状态下不接收数据
    if(g_runStatus == STATUS_RUNING)
        return false;

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
        SYS_Clear_Data(sysData);

        // 第一个数据包，包含数据总长度
        sysData->dataSize = *(uint32_t *)(buffer + PACKET_HEAD_SIZE);
        sysData->crc32 = *(uint32_t *)(buffer + PACKET_HEAD_SIZE + DATASIZE_LENGTH);
        if (sysData->dataSize > MAX_SYS_DATA_SIZE) {
            // 数据长度超出缓存最大长度
            return false;
        }

        // 第一个数据包
        memcpy(sysData->data, buffer + PACKET_HEAD_SIZE + DATASIZE_LENGTH + CRC32_LENGTH,
               length - DATASIZE_LENGTH - CRC32_LENGTH);
        sysData->tailIndex = index;
        sysData->dataOffset = length - DATASIZE_LENGTH - CRC32_LENGTH;

        // 更新状态
        if (!SYS_Update_Status(sysData)) {
            return false;
        }
        return true;
    } else {
        /* 后续数据包处理 */
        if (length + sysData->dataOffset > MAX_SYS_DATA_SIZE) {
            return false;
        }

        if (index == sysData->tailIndex + 1 && sysData->status == SYS_RECEIVE_CONTINUE) {
            memcpy(sysData->data + sysData->dataOffset, buffer + PACKET_HEAD_SIZE, length);
            sysData->tailIndex = index;
            sysData->dataOffset += length;

            // 更新状态
            if (!SYS_Update_Status(sysData)) {
                return false;
            }
            return true;
        } else {
            // 非连续的数据包 ,通讯出错
            return false;
        }
    }
}

void SYS_Process_Data(SYS_Data *sysData) {
    if (sysData == NULL) {
        return;
    }

    if (sysData->status != SYS_RECEIVE_FINISHED) {
        return;
    }

    if(g_runStatus != STATUS_RUNING){
        //Keyboard_SendEmpty();
        //SYS_Delay_ms(50);
        return;
    }
        
    // 解析命令
    SYS_Cmd cmd;
    memset(&cmd, 0, sizeof(SYS_Cmd));
    cmd.cmdType = (SYS_CmdType)sysData->data[sysData->iterator];
    sysData->iterator++;
    cmd.cmdData = sysData->data + sysData->iterator;

    // 执行命令
    sysData->iterator += SYS_Run_Cmd(&cmd);

    if (sysData->iterator >= sysData->dataSize) {
        sysData->iterator = 0;
    }
}

bool SYS_Check_CRC32(SYS_Data *sysData) {
    if (sysData == NULL) {
        return false;
    }
    uint32_t crc32 = 0;
    // 计算数据部分的CRC32
    crc32 = CRC32_Calc(sysData->data, sysData->dataSize);

    return (crc32 == sysData->crc32);
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
        Keyboard_Send((HID_Keyboard_Data *)cmd->cmdData);
        SYS_Delay_ms(20);
        Keyboard_SendEmpty();
        SYS_Delay_ms(50);
        iterator = SYS_CMD_KEYBOARD_SIZE;
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
    //debug
    for (uint32_t i = 0; i < iterator; i++) {
        printf("%d ", cmd->cmdData[i]);
    }
    printf("\n");
    return iterator;
}

void SYS_Delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}
