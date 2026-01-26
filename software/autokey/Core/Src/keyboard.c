#include "keyboard.h"
#include "usbd_hid.h"
#include "usbd_def.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t HID_Keyboard_Send(HID_Keyboard_Data *data)
{
    if (USBD_HID_SendKeyboardReport(&hUsbDeviceFS, (uint8_t *)data, sizeof(HID_Keyboard_Data)) == USBD_OK)
    {
        return HID_KEYBOARD_OK;
    }
    else
    {
        return HID_KEYBOARD_ERROR;
    }
}
