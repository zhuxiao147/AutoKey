#ifndef __SYS_DATA_H__
#define __SYS_DATA_H__
#include <stdbool.h>

#define MAX_SYS_DATA_SIZE (1024*2)

/*  
总数据格式：
    length 4
    crc32 4
    data 
*/
#define DATASIZE_LENGTH (4)
#define CRC32_LENGTH (4)

/*
单次数据包格式：（65字节）
    report_id 1
    index 1
    length 1
    data 
*/
#define PACKET_SIZE (0x40+1)
#define PACKET_HEAD_SIZE (3) // 包含 report_id , index , length

typedef enum
{
    SYS_RECEIVE_NONE = 0x00,
    SYS_RECEIVE_CONTINUE = 0x01,
    SYS_RECEIVE_FINISHED = 0x02,
} SYS_Status;

typedef struct
{
    unsigned int  dataSize;
    unsigned char data[MAX_SYS_DATA_SIZE];
    unsigned int  crc32;

    unsigned char tailIndex;
    unsigned char dataOffset;
    SYS_Status status;
} SYS_Data;

extern SYS_Data global_sysdata;

void SYS_Clear_Data(SYS_Data *sysdata);
bool SYS_Update_Status(SYS_Data *sysdata);
bool SYS_Receive_Data(SYS_Data *sysdata, unsigned char *buffer, unsigned int size);
int SYS_Pharse_Data(SYS_Data *sysdata);
#endif
