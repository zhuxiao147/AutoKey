#ifndef __SYS_DATA_H__
#define __SYS_DATA_H__

#define MAX_SYS_DATA_SIZE 1024*2

#define SYS_DATA_RECEIVED_FLAG 0x01

typedef struct
{
    unsigned int  dataSize;
    unsigned char data[MAX_SYS_DATA_SIZE];
    unsigned int  crc32;

    unsigned char tailIndex;
    unsigned char dataOffset;
} SYS_Data;


void SYS_Receive_Data(SYS_Data *data, unsigned char *buffer, unsigned int size);
int SYS_Pharse_Data(SYS_Data *data);
#endif
