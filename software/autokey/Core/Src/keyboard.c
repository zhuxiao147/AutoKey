#include "keyboard.h"

#include "usbd_def.h"
#include "usbd_hid.h"


extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t HID_Keyboard_Send(HID_Keyboard_Data *data) {
    HID_Keyboard_Data empty_keyboard_data;
    memset(&empty_keyboard_data, 0, sizeof(HID_Keyboard_Data));
    if (data == NULL) {
        return HID_KEYBOARD_ERROR;
    }

    USBD_HID_SendKeyboardReport(&hUsbDeviceFS, (uint8_t *)data, sizeof(HID_Keyboard_Data));

    HAL_Delay(15);

    USBD_HID_SendKeyboardReport(&hUsbDeviceFS, (uint8_t *)&empty_keyboard_data,
                                sizeof(HID_Keyboard_Data));

    return HID_KEYBOARD_OK;
}
