#include "sysdata.h"



void SYS_Receive_Data(SYS_Data *data, unsigned char *buffer, unsigned int size)
{
    if(data == NULL || buffer == NULL || size == 0)
    {
        return -1;
    }
    unsigned char report_id = buffer[0];
    unsigned char index = buffer[1];
    unsigned char length = buffer[2];

    if(index == 0){
       // 第一个数据包
       memcpy(data->data, buffer + 3, length);
        data->dataSize = *(unsigned int *)(buffer + 3);
       data->tailIndex = index;
       data->dataOffset = length;
    }

    if(index == data->tail_index + 1){

    }
}
