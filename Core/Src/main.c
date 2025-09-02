#include "stm32f1xx_hal.h"
#include "os.h"        // ton OS minimal
#include "os_private.h"

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void task1(void);
void task2(void);

// Variables globales utilisées par OS
uint32_t cpu_hz = 72000000;  // fréquence CPU STM32F103
uint8_t  use_ahb_div = 0;    // ou 1 si tu veux diviser l'AHB par 8

// ----------------- TÂCHES -----------------
void task1(void) {
    while (1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // toggle LED
        os_delay(1000);  // 1 sec
    }
}

void task2(void) {
    while (1) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // toggle LED
        os_delay(500);   // 0.5 sec
    }
}

// ----------------- MAIN -----------------
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    // Initialisation OS
    os_init();

    // Création des tâches
    os_create_task(task1, 1);
    os_create_task(task2, 2);

    // Démarrage OS (lance le scheduler)
    os_start();

    while (1) {
        // Normalement jamais atteint
        __WFI();  // attend interruption pour économiser CPU
    }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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
