#include "main.h"
#include "gpio.h"

/* 函数声明 */
void SystemClock_Config(void);
static void MPU_Config(void);
KEY_TypeDef Key_Scan(void);

/* 全局变量 */
uint8_t beep_status = 0; // 蜂鸣器状态

/* 主函数 */
int main(void)
{
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  while (1)
  {
	  KEY_TypeDef key = Key_Scan(); // 扫描按键
	  switch(key)
	  {
	  	  case KEY_WK_UP:
	  		  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET); // 打开
	          beep_status = 1;
	          break;
	      case KEY0:
	          HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); // 关闭
	          beep_status = 0;
	          break;
	      case KEY1:
	          beep_status = !beep_status;
	          HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, beep_status ? GPIO_PIN_SET : GPIO_PIN_RESET); // 翻转
	          break;
	      default:
	          break;
	  }
	  /* 流水灯 */
	  LED_OFF(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
	  LED_ON(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	  HAL_Delay(100);
	  LED_OFF(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	  LED_ON(LED_RED_GPIO_Port, LED_RED_Pin);
	  HAL_Delay(100);
	  LED_OFF(LED_RED_GPIO_Port, LED_RED_Pin);
	  LED_ON(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
	  HAL_Delay(100);
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}


void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

// 按键扫描函数，返回按下键值（一次只响应一次）
KEY_TypeDef Key_Scan(void)
{
    static uint8_t key_up_wkup = 1; // 松手标志
    static uint8_t key_up_key0 = 1;
    static uint8_t key_up_key1 = 1;

    // WKUP为高电平按下，其余为低电平按下
    if (HAL_GPIO_ReadPin(WK_UP_GPIO_Port, WK_UP_Pin) == GPIO_PIN_SET)
    {
        if (key_up_wkup)  // 上一个循环没有按下，当前为按下
        {
            key_up_wkup = 0;
            return KEY_WK_UP;
        }
    }
    else key_up_wkup = 1;

    if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET)
    {
        if (key_up_key0)
        {
            key_up_key0 = 0;
            return KEY0;
        }
    }
    else key_up_key0 = 1;

    if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
    {
        if (key_up_key1)
        {
            key_up_key1 = 0;
            return KEY1;
        }
    }
    else key_up_key1 = 1;

    return KEY_NONE;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
