#ifndef __SYS_DATA_H__
#define __SYS_DATA_H__
#include <stdbool.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"

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
    SYS_RECEIVE_ERROR = 0x00,
    SYS_RECEIVE_CONTINUE = 0x01,
    SYS_RECEIVE_FINISHED = 0x02,
} SYS_Status;

typedef struct
{
    uint32_t  dataSize;  //数据总长度
    uint8_t data[MAX_SYS_DATA_SIZE]; //数据缓存
    uint32_t  crc32;  //crc32校验值

    uint8_t tailIndex;  //尾部索引
    uint8_t dataOffset; //接收数据偏移量
    SYS_Status status;  //接收状态

    uint32_t iterator; // 解析数据迭代器
} SYS_Data;


#define SYS_CMD_KEYBOARD_SIZE (8)
#define SYS_CMD_MOUSE_SIZE (8)
#define SYS_CMD_DELAY_SIZE (4)

typedef enum{
    SYS_CMD_NONE = 0x00,
    SYS_CMD_KEYBOARD = 0x01,
    SYS_CMD_MOUSE = 0x02,
    SYS_CMD_DELAY = 0x03,
}SYS_CmdType;

typedef struct{
    SYS_CmdType cmdType;
    uint8_t *cmdData;
}SYS_Cmd;

extern SYS_Data global_sysdata;

void SYS_Clear_Data(SYS_Data *sysdata);
bool SYS_Update_Status(SYS_Data *sysdata);
bool SYS_Receive_Data(SYS_Data *sysdata, uint8_t *buffer, uint32_t size);
void SYS_Process_Data(SYS_Data *sysdata);
uint32_t SYS_Run_Cmd(SYS_Cmd *cmd);

bool SYS_Check_CRC32(SYS_Data *sysdata);
void SYS_Delay_ms(uint32_t ms);
#endif
