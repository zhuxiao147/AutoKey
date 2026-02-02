#include "sysdata.h"
#include <string.h>

SYS_Data global_sysdata = {0};

void SYS_Clear_Data(SYS_Data *sysdata)
{
    if (sysdata == NULL){
        return;
    }
    memset(sysdata, 0, sizeof(SYS_Data));
}

bool SYS_Update_Status(SYS_Data *sysdata){
    if (sysdata->dataOffset == sysdata->dataSize)
    {
        // 设置接收完成标志
        sysdata->status = SYS_RECEIVE_FINISHED;
    }else if (sysdata->dataOffset > sysdata->dataSize)
    {
        // 接收数据长度超出总数据长度
        return false;
    }else{
        // 设置继续接收标志
        sysdata->status = SYS_RECEIVE_CONTINUE;
    }
    return true;
}

bool SYS_Receive_Data(SYS_Data *sysdata, unsigned char *buffer, unsigned int size)
{
    if (sysdata == NULL || buffer == NULL || size != PACKET_SIZE){
        return false;
    }
    unsigned char report_id = buffer[0];
    unsigned char index = buffer[1];
    unsigned char length = buffer[2];

    if(length > PACKET_SIZE - PACKET_HEAD_SIZE){
        // 数据长度错误,不能超过最大长度
        return false;
    }

    /* 第一个数据包处理 */
    if (index == 0){
        //第一包数据至少大于数据总长度的字节数 + crc32字节数
        if(length < DATASIZE_LENGTH + CRC32_LENGTH){
            return false;
        }

        // 清除数据
        SYS_Clear_Data(sysdata);

        // 第一个数据包，包含数据总长度
        sysdata->dataSize = *(unsigned int *)(buffer + PACKET_HEAD_SIZE);
        sysdata->crc32 = *(unsigned int *)(buffer + PACKET_HEAD_SIZE + DATASIZE_LENGTH);
        if (sysdata->dataSize > MAX_SYS_DATA_SIZE){
            // 数据长度超出缓存最大长度
            return false;
        }

        // 第一个数据包
        memcpy(sysdata->data, buffer + PACKET_HEAD_SIZE + DATASIZE_LENGTH + CRC32_LENGTH, length - DATASIZE_LENGTH - CRC32_LENGTH);
        sysdata->tailIndex = index;
        sysdata->dataOffset = length - DATASIZE_LENGTH - CRC32_LENGTH;

        // 更新状态
        if (!SYS_Update_Status(sysdata)){
            return false;
        }
        return true;
    }else{
        /* 后续数据包处理 */
        if (length + sysdata->dataOffset > MAX_SYS_DATA_SIZE){
            return false;
        }

        if (index == sysdata->tailIndex + 1 && sysdata->status == SYS_RECEIVE_CONTINUE){

            memcpy(sysdata->data + sysdata->dataOffset, buffer + PACKET_HEAD_SIZE, length);
            sysdata->tailIndex = index;
            sysdata->dataOffset += length;

            // 更新状态
            if (!SYS_Update_Status(sysdata)){
                return false;
            }
            return true;        
        }else{
            // 非连续的数据包 ,通讯出错
            return false;
        }
    }

}
