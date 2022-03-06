/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <string.h>

#include "misko.h"	// misko hardware object representation
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
misko_t *misko;  // instantiate misko object

static volatile uint8_t FlagPrint;
static volatile uint8_t counter;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char bufferITOA[sizeof(char) + 3];
uint8_t ReturnString[20];
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
	MX_TIM10_Init();  // 500ms periodic
	MX_TIM11_Init();  // 10ms periodic
	MX_DMA_Init();  // mem to mem: DMA1 C4 & C5
	MX_GPIO_Init();
	MX_SPI1_Init();  // IT mode
	MX_ADC_Init();  // DMA1 C1
	MX_USART2_UART_Init();  // TX: DMA1 C7, RX: DMA1 C6
	MX_USART3_UART_Init();  // TX: DMA1 C2, RX: DMA1 C3

	/* Initialize interrupts */
	MX_NVIC_Init();
	/* USER CODE BEGIN 2 */

	/* bug in MX code generation - order or initialisation is wrong (UART before DMA -> no comm.)
	 MX_TIM10_Init();  // 500ms periodic
	 MX_TIM11_Init();  // 10ms periodic
	 MX_DMA_Init();  // mem to mem: DMA1 C4 & C5
	 MX_GPIO_Init();
	 MX_SPI1_Init();  // IT mode
	 MX_ADC_Init();  // DMA1 C1
	 MX_USART2_UART_Init();  // TX: DMA1 C7, RX: DMA1 C6
	 MX_USART3_UART_Init();  // TX: DMA1 C2, RX: DMA1 C3
	 */

	HAL_TIM_Base_Start_IT(&htim10);  // start timer10 - 500ms periodic
	HAL_TIM_Base_Start_IT(&htim11);  // start timer11 - 10ms periodic

	while(HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY)
		;
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) "\r\nUART2 start\r\n", 15);  // transmit test string

	misko = misko_ctor();  // run misko constructor
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while(1)
		{
			if(misko->adxl345->FlagStop)  // if processor stop is flagged
				misko->StopMode(1);  // go into stop mode

			if(FlagPrint)  // if printout is flagged
				{
					FlagPrint = 0;  // unset flag
					itoa(counter, (char*) bufferITOA, 10);  // convert counter to integer

					while(HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY)
						;
					HAL_UART_Transmit_DMA(&huart2, (uint8_t*) bufferITOA, strlen(bufferITOA));  // print converted integer

					while(HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY)
						;
					HAL_UART_Transmit_DMA(&huart2, (uint8_t*) "\r\n", 2);  // add newline and carriage return
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
	RCC_OscInitTypeDef RCC_OscInitStruct =
		{0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
		{0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		{
			Error_Handler();
		}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
		{
			Error_Handler();
		}
}

/**
 * @brief NVIC Configuration.
 * @retval None
 */
static void MX_NVIC_Init(void)
{
	/* TIM10_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(TIM10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM10_IRQn);
	/* TIM11_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(TIM11_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM11_IRQn);
	/* EXTI15_10_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  // ISR callback for timer interrupts
{
	if(htim->Instance == TIM10)  // timer10 - 500ms periodic
		{
			HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
			FlagPrint = 1;  // flag to pint
			++counter;  // increase counter value
		}

	if(htim->Instance == TIM11)  // timer11 - 10ms periodic
		{
			//HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);  // for now jsut toggle the LED
			;
		}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)	// ISR for pin change interrupts
{
	if(GPIO_Pin == EXTI_PushButton_Pin)  // if button is pressed
		misko->StopMode(0);  // go out of stop mode

	if(GPIO_Pin == EXTI_ADXL345_Pin)  // accelerometer interrupt (act. or inact.)
		misko->adxl345->ISR();  // execute the ISR callback
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
	while(1)
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

