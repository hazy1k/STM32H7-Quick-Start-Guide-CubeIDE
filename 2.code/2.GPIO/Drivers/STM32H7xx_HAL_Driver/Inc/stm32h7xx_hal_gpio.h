#ifndef STM32H7xx_HAL_GPIO_H
#define STM32H7xx_HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* 包含公共定义 */
#include "stm32h7xx_hal_def.h"

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup GPIO
  * @{
  */

/* 导出的类型定义 ------------------------------------------------------------*/

/**
  * @brief GPIO 初始化结构体
  */
typedef struct
{
    uint32_t Pin;        /*!< 指定要配置的 GPIO 引脚，可使用 GPIO_PIN_0 到 GPIO_PIN_15 或组合 */

    uint32_t Mode;       /*!< 指定引脚工作模式（输入、输出、复用、模拟、中断等） */

    uint32_t Pull;       /*!< 上下拉配置：无、上拉、下拉 */

    uint32_t Speed;      /*!< 输出速度：低、中、高、超高 */

    uint32_t Alternate;  /*!< 复用功能编号（AF0~AF15），用于选择外设功能 */
} GPIO_InitTypeDef;

/**
  * @brief GPIO 引脚电平状态（置位/复位）
  */
typedef enum
{
    GPIO_PIN_RESET = 0U,  /*!< 引脚为低电平（0） */
    GPIO_PIN_SET        /*!< 引脚为高电平（1） */
} GPIO_PinState;

/* 导出的常量定义 --------------------------------------------------------*/

/** @defgroup GPIO_引脚定义  GPIO 引脚定义
  * @{
  */
#define GPIO_PIN_0                 ((uint16_t)0x0001)  /* 选择引脚 0 */
#define GPIO_PIN_1                 ((uint16_t)0x0002)  /* 选择引脚 1 */
#define GPIO_PIN_2                 ((uint16_t)0x0004)  /* 选择引脚 2 */
#define GPIO_PIN_3                 ((uint16_t)0x0008)  /* 选择引脚 3 */
#define GPIO_PIN_4                 ((uint16_t)0x0010)  /* 选择引脚 4 */
#define GPIO_PIN_5                 ((uint16_t)0x0020)  /* 选择引脚 5 */
#define GPIO_PIN_6                 ((uint16_t)0x0040)  /* 选择引脚 6 */
#define GPIO_PIN_7                 ((uint16_t)0x0080)  /* 选择引脚 7 */
#define GPIO_PIN_8                 ((uint16_t)0x0100)  /* 选择引脚 8 */
#define GPIO_PIN_9                 ((uint16_t)0x0200)  /* 选择引脚 9 */
#define GPIO_PIN_10                ((uint16_t)0x0400)  /* 选择引脚 10 */
#define GPIO_PIN_11                ((uint16_t)0x0800)  /* 选择引脚 11 */
#define GPIO_PIN_12                ((uint16_t)0x1000)  /* 选择引脚 12 */
#define GPIO_PIN_13                ((uint16_t)0x2000)  /* 选择引脚 13 */
#define GPIO_PIN_14                ((uint16_t)0x4000)  /* 选择引脚 14 */
#define GPIO_PIN_15                ((uint16_t)0x8000)  /* 选择引脚 15 */
#define GPIO_PIN_All               ((uint16_t)0xFFFF)  /* 选择所有引脚 */
#define GPIO_PIN_MASK              (0x0000FFFFU)       /* 引脚掩码，用于参数检查 */

/**
  * @}
  */

/** @defgroup GPIO_模式定义  GPIO 工作模式定义
  * @brief GPIO 配置模式
  *        数值格式：0x00WX00YZ
  *           - W: 3 位 EXTI 触发方式（上升沿、下降沿等）
  *           - X: 2 位 EXTI 模式（中断 IT 或事件 EVT）
  *           - Y: 1 位输出类型（0=推挽, 1=开漏）
  *           - Z: 2 位 GPIO 模式（输入、输出、复用、模拟）
  * @{
  */
#define GPIO_MODE_INPUT                 MODE_INPUT                                                  /* 浮空输入模式 */
#define GPIO_MODE_OUTPUT_PP             (MODE_OUTPUT | OUTPUT_PP)                                   /* 推挽输出模式 */
#define GPIO_MODE_OUTPUT_OD             (MODE_OUTPUT | OUTPUT_OD)                                   /* 开漏输出模式 */
#define GPIO_MODE_AF_PP                 (MODE_AF | OUTPUT_PP)                                       /* 复用推挽输出模式 */
#define GPIO_MODE_AF_OD                 (MODE_AF | OUTPUT_OD)                                       /* 复用开漏输出模式 */
#define GPIO_MODE_ANALOG                MODE_ANALOG                                                 /* 模拟输入模式 */
#define GPIO_MODE_IT_RISING             (MODE_INPUT | EXTI_IT | TRIGGER_RISING)                     /* 外部中断：上升沿触发 */
#define GPIO_MODE_IT_FALLING            (MODE_INPUT | EXTI_IT | TRIGGER_FALLING)                    /* 外部中断：下降沿触发 */
#define GPIO_MODE_IT_RISING_FALLING     (MODE_INPUT | EXTI_IT | TRIGGER_RISING | TRIGGER_FALLING)   /* 外部中断：双边沿触发 */

#define GPIO_MODE_EVT_RISING            (MODE_INPUT | EXTI_EVT | TRIGGER_RISING)                    /* 外部事件：上升沿触发 */
#define GPIO_MODE_EVT_FALLING           (MODE_INPUT | EXTI_EVT | TRIGGER_FALLING)                   /* 外部事件：下降沿触发 */
#define GPIO_MODE_EVT_RISING_FALLING    (MODE_INPUT | EXTI_EVT | TRIGGER_RISING | TRIGGER_FALLING)  /* 外部事件：双边沿触发 */

/**
  * @}
  */

/** @defgroup GPIO_速度定义  GPIO 输出速度定义
  * @brief 用于配置输出引脚的切换速度
  * @{
  */
#define GPIO_SPEED_FREQ_LOW         (0x00000000U)  /* 低速 */
#define GPIO_SPEED_FREQ_MEDIUM      (0x00000001U)  /* 中速 */
#define GPIO_SPEED_FREQ_HIGH        (0x00000002U)  /* 高速（快） */
#define GPIO_SPEED_FREQ_VERY_HIGH   (0x00000003U)  /* 超高速 */

/**
  * @}
  */

/** @defgroup GPIO_上下拉定义  GPIO 上下拉配置
  * @brief 用于启用或禁用内部上拉/下拉电阻
  * @{
  */
#define GPIO_NOPULL        (0x00000000U)   /* 无上下拉 */
#define GPIO_PULLUP        (0x00000001U)   /* 启用上拉 */
#define GPIO_PULLDOWN      (0x00000002U)   /* 启用下拉 */

/**
  * @}
  */

/* 导出的宏定义 ------------------------------------------------------------*/

/** @defgroup GPIO_导出宏  GPIO 导出宏
  * @{
  */

/**
  * @brief  检查指定 EXTI 中断标志是否被置位
  * @param  __EXTI_LINE__: 要检查的 EXTI 线（如 GPIO_PIN_5）
  * @retval SET（已触发）或 RESET（未触发）
  */
#define __HAL_GPIO_EXTI_GET_FLAG(__EXTI_LINE__) (EXTI->PR1 & (__EXTI_LINE__))

/**
  * @brief  清除 EXTI 中断标志位
  * @param  __EXTI_LINE__: 要清除的引脚
  * @retval 无
  */
#define __HAL_GPIO_EXTI_CLEAR_FLAG(__EXTI_LINE__) (EXTI->PR1 = (__EXTI_LINE__))

/**
  * @brief  检查 EXTI 中断是否触发（与 GET_FLAG 功能相同）
  * @param  __EXTI_LINE__: 指定引脚
  * @retval SET 或 RESET
  */
#define __HAL_GPIO_EXTI_GET_IT(__EXTI_LINE__) (EXTI->PR1 & (__EXTI_LINE__))

/**
  * @brief  清除 EXTI 中断挂起位
  * @param  __EXTI_LINE__: 要清除的引脚
  * @retval 无
  */
#define __HAL_GPIO_EXTI_CLEAR_IT(__EXTI_LINE__) (EXTI->PR1 = (__EXTI_LINE__))

#if defined(DUAL_CORE)
/**
  * @brief  双核模式下：检查 CPU2 的 EXTI 标志
  */
#define __HAL_GPIO_EXTID2_GET_FLAG(__EXTI_LINE__) (EXTI->C2PR1 & (__EXTI_LINE__))

/**
  * @brief  双核模式下：清除 CPU2 的 EXTI 标志
  */
#define __HAL_GPIO_EXTID2_CLEAR_FLAG(__EXTI_LINE__) (EXTI->C2PR1 = (__EXTI_LINE__))

/**
  * @brief  双核模式下：检查 CPU2 的中断触发
  */
#define __HAL_GPIO_EXTID2_GET_IT(__EXTI_LINE__) (EXTI->C2PR1 & (__EXTI_LINE__))

/**
  * @brief  双核模式下：清除 CPU2 的中断挂起
  */
#define __HAL_GPIO_EXTID2_CLEAR_IT(__EXTI_LINE__) (EXTI->C2PR1 = (__EXTI_LINE__))
#endif

/**
  * @brief  产生软件中断，用于测试 EXTI 逻辑
  * @param  __EXTI_LINE__: 指定要触发的 EXTI 线
  * @retval 无
  */
#define __HAL_GPIO_EXTI_GENERATE_SWIT(__EXTI_LINE__) (EXTI->SWIER1 |= (__EXTI_LINE__))

/**
  * @}
  */

/* 包含 GPIO 扩展功能模块 */
#include "stm32h7xx_hal_gpio_ex.h"

/* 导出的函数声明 --------------------------------------------------------*/

/** @addtogroup GPIO_导出函数
  * @{
  */

/** @addtogroup GPIO_初始化函数组
  * @{
  */
/* 初始化与反初始化函数 */
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, const GPIO_InitTypeDef *GPIO_Init);
void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
/**
  * @}
  */

/** @addtogroup GPIO_操作函数组
  * @{
  */
/* IO 操作函数 */
GPIO_PinState HAL_GPIO_ReadPin(const GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
/**
  * @}
  */

/**
  * @}
  */

/* 私有常量定义 ---------------------------------------------------------*/

/** @defgroup GPIO_私有常量  GPIO 私有常量
  * @{
  */
#define GPIO_MODE_Pos                   0u
#define GPIO_MODE                       (0x3uL << GPIO_MODE_Pos)
#define MODE_INPUT                      (0x0uL << GPIO_MODE_Pos)      /* 输入模式 */
#define MODE_OUTPUT                     (0x1uL << GPIO_MODE_Pos)      /* 输出模式 */
#define MODE_AF                         (0x2uL << GPIO_MODE_Pos)      /* 复用功能模式 */
#define MODE_ANALOG                     (0x3uL << GPIO_MODE_Pos)      /* 模拟模式 */

#define OUTPUT_TYPE_Pos                 4u
#define OUTPUT_TYPE                     (0x1uL << OUTPUT_TYPE_Pos)
#define OUTPUT_PP                       (0x0uL << OUTPUT_TYPE_Pos)    /* 推挽输出 */
#define OUTPUT_OD                       (0x1uL << OUTPUT_TYPE_Pos)    /* 开漏输出 */

#define EXTI_MODE_Pos                   16u
#define EXTI_MODE                       (0x3uL << EXTI_MODE_Pos)
#define EXTI_IT                         (0x1uL << EXTI_MODE_Pos)      /* 中断模式 */
#define EXTI_EVT                        (0x2uL << EXTI_MODE_Pos)      /* 事件模式 */

#define TRIGGER_MODE_Pos                20u
#define TRIGGER_MODE                    (0x7uL << TRIGGER_MODE_Pos)
#define TRIGGER_RISING                  (0x1uL << TRIGGER_MODE_Pos)   /* 上升沿触发 */
#define TRIGGER_FALLING                 (0x2uL << TRIGGER_MODE_Pos)   /* 下降沿触发 */
#define TRIGGER_LEVEL                   (0x4uL << TRIGGER_MODE_Pos)   /* 电平触发（保留） */

/**
  * @}
  */

/* 私有宏定义 ------------------------------------------------------------*/

/** @defgroup GPIO_私有宏  GPIO 私有宏
  * @{
  */
#define IS_GPIO_PIN_ACTION(ACTION) (((ACTION) == GPIO_PIN_RESET) || ((ACTION) == GPIO_PIN_SET))
#define IS_GPIO_PIN(__PIN__)         ((((uint32_t)(__PIN__) & GPIO_PIN_MASK) != 0x00U) && \
                                     (((uint32_t)(__PIN__) & ~GPIO_PIN_MASK) == 0x00U))
#define IS_GPIO_MODE(MODE) (((MODE) == GPIO_MODE_INPUT)              || \
                            ((MODE) == GPIO_MODE_OUTPUT_PP)          || \
                            ((MODE) == GPIO_MODE_OUTPUT_OD)          || \
                            ((MODE) == GPIO_MODE_AF_PP)              || \
                            ((MODE) == GPIO_MODE_AF_OD)              || \
                            ((MODE) == GPIO_MODE_IT_RISING)          || \
                            ((MODE) == GPIO_MODE_IT_FALLING)         || \
                            ((MODE) == GPIO_MODE_IT_RISING_FALLING)  || \
                            ((MODE) == GPIO_MODE_EVT_RISING)         || \
                            ((MODE) == GPIO_MODE_EVT_FALLING)        || \
                            ((MODE) == GPIO_MODE_EVT_RISING_FALLING) || \
                            ((MODE) == GPIO_MODE_ANALOG))

#define IS_GPIO_SPEED(SPEED) (((SPEED) == GPIO_SPEED_FREQ_LOW)  || ((SPEED) == GPIO_SPEED_FREQ_MEDIUM) || \
                              ((SPEED) == GPIO_SPEED_FREQ_HIGH) || ((SPEED) == GPIO_SPEED_FREQ_VERY_HIGH))

#define IS_GPIO_PULL(PULL) (((PULL) == GPIO_NOPULL) || ((PULL) == GPIO_PULLUP) || \
                            ((PULL) == GPIO_PULLDOWN))

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32H7xx_HAL_GPIO_H */
