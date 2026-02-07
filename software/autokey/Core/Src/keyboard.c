#include "keyboard.h"

#include "usbd_def.h"
#include "usbd_hid.h"


extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t HID_Keyboard_Send(HID_Keyboard_Data *data) {
    if (data == NULL) {
        return HID_KEYBOARD_ERROR;
    }

    while (USBD_HID_SendKeyboardReport(&hUsbDeviceFS, (uint8_t *)data, sizeof(HID_Keyboard_Data)) !=
           USBD_OK) {
        HAL_Delay(10);
    }

    return HID_KEYBOARD_OK;
}
