#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__
#include <stdint.h>

#define HID_KEYBOARD_OK 0
#define HID_KEYBOARD_ERROR 1

#define HID_KEY_A 0x04
#define HID_KEY_B 0x05
#define HID_KEY_C 0x06
#define HID_KEY_D 0x07
#define HID_KEY_E 0x08
#define HID_KEY_F 0x09
#define HID_KEY_G 0x0A
#define HID_KEY_H 0x0B
#define HID_KEY_I 0x0C
#define HID_KEY_J 0x0D
#define HID_KEY_K 0x0E
#define HID_KEY_L 0x0F
#define HID_KEY_M 0x10
#define HID_KEY_N 0x11
#define HID_KEY_O 0x12
#define HID_KEY_P 0x13
#define HID_KEY_Q 0x14
#define HID_KEY_R 0x15
#define HID_KEY_S 0x16
#define HID_KEY_T 0x17
#define HID_KEY_U 0x18
#define HID_KEY_V 0x19
#define HID_KEY_W 0x1A
#define HID_KEY_X 0x1B
#define HID_KEY_Y 0x1C
#define HID_KEY_Z 0x1D

// HID键盘数据结构体
typedef struct {
    uint8_t modifier; // 修饰键状态
    uint8_t reserved; // 保留字节
    uint8_t keycode[6]; // 普通按键的键值
} HID_Keyboard_Data;

uint8_t HID_Keyboard_Send(HID_Keyboard_Data *data);


#endif //__KEYBOARD_H__
