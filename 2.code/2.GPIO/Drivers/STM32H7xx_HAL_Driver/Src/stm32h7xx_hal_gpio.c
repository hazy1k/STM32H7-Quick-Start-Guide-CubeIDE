#include "stm32h7xx_hal.h"

#ifdef HAL_GPIO_MODULE_ENABLED

/* 私有定义 */
#define GPIO_NUMBER           (16U)

#if defined(DUAL_CORE)
#define EXTI_CPU1             (0x01000000U)
#define EXTI_CPU2             (0x02000000U)
#endif

/* 私有变量和函数声明（省略） */
/* 导出函数实现 */

/**
  * @brief  初始化 GPIO 引脚
  * @param  GPIOx: 指定 GPIO 端口（A-K）
  * @param  GPIO_Init: 指向 GPIO 初始化结构体，包含引脚配置信息
  * @retval 无
  */
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, const GPIO_InitTypeDef *GPIO_Init)
{
    uint32_t position = 0x00U;
    uint32_t iocurrent;
    uint32_t temp;
    EXTI_Core_TypeDef *EXTI_CurrentCPU;

#if defined(DUAL_CORE) && defined(CORE_CM4)
    EXTI_CurrentCPU = EXTI_D2;  // CM4 CPU 的 EXTI
#else
    EXTI_CurrentCPU = EXTI_D1;  // CM7 CPU 的 EXTI
#endif

    // 检查参数合法性
    assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
    assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
    assert_param(IS_GPIO_MODE(GPIO_Init->Mode));

    // 遍历所有被选中的引脚
    while (((GPIO_Init->Pin) >> position) != 0x00U)
    {
        iocurrent = GPIO_Init->Pin & (1UL << position);

        if (iocurrent != 0x00U)
        {
            // 输出或复用功能模式下配置速度和输出类型
            if (((GPIO_Init->Mode & GPIO_MODE) == MODE_OUTPUT) ||
                ((GPIO_Init->Mode & GPIO_MODE) == MODE_AF))
            {
                assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));

                // 配置输出速度
                temp = GPIOx->OSPEEDR;
                temp &= ~(GPIO_OSPEEDR_OSPEED0 << (position * 2U));
                temp |= (GPIO_Init->Speed << (position * 2U));
                GPIOx->OSPEEDR = temp;

                // 配置输出类型（推挽/开漏）
                temp = GPIOx->OTYPER;
                temp &= ~(GPIO_OTYPER_OT0 << position);
                temp |= (((GPIO_Init->Mode & OUTPUT_TYPE) >> OUTPUT_TYPE_Pos) << position);
                GPIOx->OTYPER = temp;
            }

            // 非模拟模式下配置上下拉
            if ((GPIO_Init->Mode & GPIO_MODE) != MODE_ANALOG)
            {
                assert_param(IS_GPIO_PULL(GPIO_Init->Pull));

                // 配置上拉或下拉电阻
                temp = GPIOx->PUPDR;
                temp &= ~(GPIO_PUPDR_PUPD0 << (position * 2U));
                temp |= (GPIO_Init->Pull << (position * 2U));
                GPIOx->PUPDR = temp;
            }

            // 复用功能模式下配置 AF 寄存器
            if ((GPIO_Init->Mode & GPIO_MODE) == MODE_AF)
            {
                assert_param(IS_GPIO_AF_INSTANCE(GPIOx));
                assert_param(IS_GPIO_AF(GPIO_Init->Alternate));

                // 配置复用功能（AFRL/AFRH）
                temp = GPIOx->AFR[position >> 3U];  // 选择低/高寄存器
                temp &= ~(0xFU << ((position & 0x07U) * 4U));
                temp |= (GPIO_Init->Alternate << ((position & 0x07U) * 4U));
                GPIOx->AFR[position >> 3U] = temp;
            }

            // 配置引脚方向模式（输入、输出、复用、模拟）
            temp = GPIOx->MODER;
            temp &= ~(GPIO_MODER_MODE0 << (position * 2U));
            temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * 2U));
            GPIOx->MODER = temp;

            // 外部中断/事件模式配置
            if ((GPIO_Init->Mode & EXTI_MODE) != 0x00U)
            {
                // 使能 SYSCFG 时钟以访问 EXTI 配置
                __HAL_RCC_SYSCFG_CLK_ENABLE();

                // 将 GPIO 引脚映射到 EXTI 线
                temp = SYSCFG->EXTICR[position >> 2U];
                temp &= ~(0x0FUL << (4U * (position & 0x03U)));
                temp |= (GPIO_GET_INDEX(GPIOx) << (4U * (position & 0x03U)));
                SYSCFG->EXTICR[position >> 2U] = temp;

                // 配置上升沿触发
                temp = EXTI->RTSR1;
                temp &= ~iocurrent;
                if ((GPIO_Init->Mode & TRIGGER_RISING) != 0x00U)
                {
                    temp |= iocurrent;
                }
                EXTI->RTSR1 = temp;

                // 配置下降沿触发
                temp = EXTI->FTSR1;
                temp &= ~iocurrent;
                if ((GPIO_Init->Mode & TRIGGER_FALLING) != 0x00U)
                {
                    temp |= iocurrent;
                }
                EXTI->FTSR1 = temp;

                // 配置事件模式（是否产生事件）
                temp = EXTI_CurrentCPU->EMR1;
                temp &= ~iocurrent;
                if ((GPIO_Init->Mode & EXTI_EVT) != 0x00U)
                {
                    temp |= iocurrent;
                }
                EXTI_CurrentCPU->EMR1 = temp;

                // 配置中断模式（是否使能中断）
                temp = EXTI_CurrentCPU->IMR1;
                temp &= ~iocurrent;
                if ((GPIO_Init->Mode & EXTI_IT) != 0x00U)
                {
                    temp |= iocurrent;
                }
                EXTI_CurrentCPU->IMR1 = temp;
            }
        }

        position++;  // 处理下一个引脚
    }
}

/**
  * @brief  反初始化 GPIO 引脚（恢复默认状态）
  * @param  GPIOx: 指定 GPIO 端口（A-K）
  * @param  GPIO_Pin: 指定要反初始化的引脚（如 GPIO_PIN_5）
  * @retval 无
  */
void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
    uint32_t position = 0x00U;
    uint32_t iocurrent;
    uint32_t tmp;
    EXTI_Core_TypeDef *EXTI_CurrentCPU;

#if defined(DUAL_CORE) && defined(CORE_CM4)
    EXTI_CurrentCPU = EXTI_D2;
#else
    EXTI_CurrentCPU = EXTI_D1;
#endif

    assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
    assert_param(IS_GPIO_PIN(GPIO_Pin));

    while ((GPIO_Pin >> position) != 0x00U)
    {
        iocurrent = GPIO_Pin & (1UL << position);

        if (iocurrent != 0x00U)
        {
            // 清除 EXTI 配置
            tmp = SYSCFG->EXTICR[position >> 2U];
            tmp &= (0x0FUL << (4U * (position & 0x03U)));

            if (tmp == (GPIO_GET_INDEX(GPIOx) << (4U * (position & 0x03U))))
            {
                EXTI_CurrentCPU->IMR1 &= ~iocurrent;  // 禁用中断
                EXTI_CurrentCPU->EMR1 &= ~iocurrent;  // 禁用事件
                EXTI->FTSR1 &= ~iocurrent;            // 清除下降沿触发
                EXTI->RTSR1 &= ~iocurrent;            // 清除上升沿触发

                tmp = 0x0FUL << (4U * (position & 0x03U));
                SYSCFG->EXTICR[position >> 2U] &= ~tmp;  // 清除端口映射
            }

            // 恢复引脚为模拟输入模式（默认低功耗状态）
            GPIOx->MODER |= (GPIO_MODER_MODE0 << (position * 2U));

            // 清除复用功能
            GPIOx->AFR[position >> 3U] &= ~(0xFU << ((position & 0x07U) * 4U));

            // 清除上下拉
            GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPD0 << (position * 2U));

            // 恢复默认输出类型（推挽）
            GPIOx->OTYPER &= ~(GPIO_OTYPER_OT0 << position);

            // 恢复默认输出速度（低速）
            GPIOx->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED0 << (position * 2U));
        }

        position++;
    }
}

/**
  * @brief  读取指定 GPIO 引脚的输入电平
  * @param  GPIOx: 端口（A-K）
  * @param  GPIO_Pin: 引脚编号（0-15）
  * @retval GPIO_PIN_SET（高电平）或 GPIO_PIN_RESET（低电平）
  */
GPIO_PinState HAL_GPIO_ReadPin(const GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIO_PinState bitstatus;

    assert_param(IS_GPIO_PIN(GPIO_Pin));

    if ((GPIOx->IDR & GPIO_Pin) != 0x00U)
    {
        bitstatus = GPIO_PIN_SET;
    }
    else
    {
        bitstatus = GPIO_PIN_RESET;
    }

    return bitstatus;
}

/**
  * @brief  设置或清除指定 GPIO 引脚
  * @note   使用 BSRR 寄存器实现原子操作，避免中断干扰
  * @param  GPIOx: 端口（A-K）
  * @param  GPIO_Pin: 引脚
  * @param  PinState: 要设置的状态（GPIO_PIN_SET / RESET）
  * @retval 无
  */
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    assert_param(IS_GPIO_PIN(GPIO_Pin));
    assert_param(IS_GPIO_PIN_ACTION(PinState));

    if (PinState != GPIO_PIN_RESET)
    {
        GPIOx->BSRR = GPIO_Pin;                    // 置位
    }
    else
    {
        GPIOx->BSRR = (uint32_t)GPIO_Pin << 16;    // 复位（BSRR 高16位）
    }
}

/**
  * @brief  翻转指定 GPIO 引脚电平
  * @param  GPIOx: 端口
  * @param  GPIO_Pin: 引脚
  * @retval 无
  */
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    uint32_t odr;

    assert_param(IS_GPIO_PIN(GPIO_Pin));

    odr = GPIOx->ODR;
    // 当前为高时，清除；当前为低时，置位
    GPIOx->BSRR = ((odr & GPIO_Pin) << 16) | (~odr & GPIO_Pin);
}

/**
  * @brief  锁定 GPIO 引脚配置（防止意外修改）
  * @note   锁定后，MODER、OTYPER、OSPEEDR、PUPDR、AFRL/AFRH 不可再修改，直到复位
  * @param  GPIOx: 端口
  * @param  GPIO_Pin: 要锁定的引脚
  * @retval HAL_OK 或 HAL_ERROR
  */
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    __IO uint32_t tmp = GPIO_LCKR_LCKK;

    assert_param(IS_GPIO_LOCK_INSTANCE(GPIOx));
    assert_param(IS_GPIO_PIN(GPIO_Pin));

    // 锁定序列：101 写入 LCKR
    tmp |= GPIO_Pin;               // LCKK=1 + 引脚位
    GPIOx->LCKR = tmp;
    GPIOx->LCKR = GPIO_Pin;        // LCKK=0 + 引脚位
    GPIOx->LCKR = tmp;             // LCKK=1 + 引脚位

    // 读取以确认锁定生效
    tmp = GPIOx->LCKR;
    if ((GPIOx->LCKR & GPIO_LCKR_LCKK) != 0x00U)
    {
        return HAL_OK;
    }
    else
    {
        return HAL_ERROR;
    }
}

/**
  * @brief  处理外部中断请求
  * @param  GPIO_Pin: 触发中断的引脚
  * @retval 无
  */
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin)
{
#if defined(DUAL_CORE) && defined(CORE_CM4)
    if (__HAL_GPIO_EXTID2_GET_IT(GPIO_Pin) != 0x00U)
    {
        __HAL_GPIO_EXTID2_CLEAR_IT(GPIO_Pin);
        HAL_GPIO_EXTI_Callback(GPIO_Pin);
    }
#else
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_Pin) != 0x00U)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
        HAL_GPIO_EXTI_Callback(GPIO_Pin);
    }
#endif
}

/**
  * @brief  外部中断回调函数（用户可重写）
  * @param  GPIO_Pin: 触发中断的引脚
  * @retval 无
  * @note   默认为弱函数，用户可在应用中实现具体逻辑
  */
__weak void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    UNUSED(GPIO_Pin);  // 防止未使用警告
    // 用户可在此添加中断处理代码
}

#endif /* HAL_GPIO_MODULE_ENABLED */
