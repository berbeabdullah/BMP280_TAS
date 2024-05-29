/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "BMP280.h"
#include "ssd1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define INTERVAL 200
bool BUZ = false;
uint32_t last_time;

float Temp1, Press1, Alt1;
float con1 = 0.1902632;
float con2 = 44330.77;
float sea_level = 1013.26;

float vel,xN,xB;
float Calculate_Height (float prs);
char temp[100];
char press[100];
char alt[100];
char velo[100];

float Preh = 0;
float Curh = 0;
float dish = 0;
float vel, tvel;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start_IT(&htim3);
  BMP280_Config(OSRS_2, OSRS_16, MODE_NORMAL, T_SB_0p5, IRR_16);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Temp1 = BMP280_Get_Temp();
	  Press1 = BMP280_Get_Press();
	  Alt1 = Calculate_Height(Press1);
	  sprintf(temp,"sicaklik= %.2f",Temp1);
	  sprintf(press,"basinc = %.2f",Press1);
	  sprintf(alt,"yukseklik = %.5f",Alt1);
	  sprintf(velo,"hiz = %.2f",vel);
	  ssd1306_Fill(Black);
	  ssd1306_SetCursor(2,8);
	  ssd1306_WriteString(temp, Font_7x10, White);
	  ssd1306_SetCursor(2,20);
	  ssd1306_WriteString(press, Font_7x10, White);
	  ssd1306_SetCursor(2,32);
	  ssd1306_WriteString(alt, Font_7x10, White);
	  ssd1306_SetCursor(2,44);
	  ssd1306_WriteString(velo, Font_7x10, White);
	  ssd1306_UpdateScreen();
	  //ssd1306_TestAll();
	  HAL_UART_Transmit(&huart2, temp, strlen(temp), 10000);
	  HAL_UART_Transmit(&huart2, press, strlen(press), 10000);
	  HAL_UART_Transmit(&huart2, alt, strlen(press), 10000);
	  HAL_UART_Transmit(&huart2, velo, strlen(velo), 10000);
	  if (vel < -1) {
		  HAL_GPIO_WritePin(BUZZ_GPIO_Port, BUZZ_Pin, 1);
		  BUZ = false;
	  }
	  else if (vel >=0.6) {
		  BUZ = true;
	  }
	  else {
		  HAL_GPIO_WritePin(BUZZ_GPIO_Port, BUZZ_Pin, 0);
		  BUZ = false;
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
float Calculate_Height (float prs){
	float tmp = pow((prs/sea_level),con1);
	return  con2*(1-tmp);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
		if (htim->Instance==TIM2) {
			Curh = Alt1;
			dish = Curh - Preh;
			vel = dish/0.25;
			Preh = Curh;
		}
		if (htim->Instance==TIM3) {
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
			if (BUZ) {
				HAL_GPIO_TogglePin(BUZZ_GPIO_Port, BUZZ_Pin);
			}

		}
}
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
