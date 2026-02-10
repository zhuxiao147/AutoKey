
#include "key.h"

#include "sysData.h"

#include <stdio.h>


static void Key_Delay(__IO uint32_t nCount) {
    for (; nCount != 0; nCount--)
        ;
}

void Key_GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStructure.Pin = GPIO_PIN_0;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == KEY_ON) {
        Key_Delay(500);
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == KEY_ON) {
            while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == KEY_ON)
                ;
            return KEY_ON;
        } else
            return KEY_OFF;
    } else
        return KEY_OFF;
}

void EXTI_Config(void) {
    // 设置中断优先级
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);

    // 启用中断
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {
        uint8_t keyStatus = Key_Scan(GPIOA, GPIO_PIN_0);
        if (keyStatus == KEY_ON) {
            g_runStatus = g_runStatus == STATUS_SETTING ? STATUS_RUNING : STATUS_SETTING;
            printf("g_runStatus: %d\n", g_runStatus);
        }
    }
}
