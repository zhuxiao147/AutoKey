#include "keyboard.h"

#include "usbd_def.h"
#include "usbd_hid.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t Keyboard_Send(HID_Keyboard_Data *data) {
    if (data == NULL) {
        return HID_KEYBOARD_ERROR;
    }

    while (USBD_HID_SendKeyboardReport(&hUsbDeviceFS, (uint8_t *)data, sizeof(HID_Keyboard_Data)) !=
           USBD_OK) {
        HAL_Delay(10);
    }

    return HID_KEYBOARD_OK;
}

void Keyboard_SendEmpty(void) {
    HID_Keyboard_Data empty_keyboard_data;
    memset(&empty_keyboard_data, 0, sizeof(HID_Keyboard_Data));
    for (uint32_t i = 0; i < 6; i++) {
        while (USBD_HID_SendKeyboardReport(&hUsbDeviceFS, (uint8_t *)&empty_keyboard_data,
                                           sizeof(HID_Keyboard_Data)) != USBD_OK) {
            HAL_Delay(5);
        }
    }
}
