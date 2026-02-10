#ifndef __KEY_H
#define	__KEY_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>
 /*******
 KEY_ON 0
 KEY_OFF 1
 ********/
#define KEY_ON	GPIO_PIN_RESET
#define KEY_OFF	GPIO_PIN_SET

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
void EXTI_Config(void);


#endif /* __LED_H */
