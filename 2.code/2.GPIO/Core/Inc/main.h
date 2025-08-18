#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32h7xx_hal.h"

void Error_Handler(void);

#define BEEP_Pin GPIO_PIN_4
#define BEEP_GPIO_Port GPIOE
#define LED_GREEN_Pin GPIO_PIN_5
#define LED_GREEN_GPIO_Port GPIOE
#define LED_RED_Pin GPIO_PIN_6
#define LED_RED_GPIO_Port GPIOE
#define WK_UP_Pin GPIO_PIN_0
#define WK_UP_GPIO_Port GPIOA
#define KEY0_Pin GPIO_PIN_1
#define KEY0_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_15
#define KEY1_GPIO_Port GPIOA
#define LED_BLUE_Pin GPIO_PIN_4
#define LED_BLUE_GPIO_Port GPIOB

// LED低电平点亮
#define LED_ON(port, pin)      HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)
#define LED_OFF(port, pin)     HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)
#define LED_TOGGLE(port, pin)  HAL_GPIO_TogglePin(port, pin)

// 按键状态定义
typedef enum {
    KEY_NONE = 0,
    KEY_WK_UP,
    KEY0,
    KEY1
} KEY_TypeDef;

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
