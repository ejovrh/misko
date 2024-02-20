/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32h5xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "stm32h5xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "org1510mk4/org1510mk4.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
extern volatile uint32_t __adc_dma_buffer[ADC_CHANNELS];  // store for ADC readout
extern volatile uint32_t __adc_results[ADC_CHANNELS];  // store ADC average data
extern double _VddaConversionConstant;	// constant values pre-computed in constructor
extern org1510mk4_t *const ORG1510MK4;

static volatile uint8_t i;  // iterator for average calculation
static uint32_t tempvbat;  // temporary variable for average calculation
static uint32_t tempvrefint;	// ditto
static uint32_t temptemp;  // ditto
static uint32_t tempvgps;  // ditto
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_NodeTypeDef Node_GPDMA1_Channel5;
extern DMA_QListTypeDef List_GPDMA1_Channel5;
extern DMA_HandleTypeDef handle_GPDMA1_Channel5;
extern ADC_HandleTypeDef hadc1;
extern FDCAN_HandleTypeDef hfdcan1;
extern DMA_HandleTypeDef handle_GPDMA1_Channel7;
extern DMA_HandleTypeDef handle_GPDMA1_Channel6;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;
extern DMA_NodeTypeDef Node_GPDMA1_Channel3;
extern DMA_QListTypeDef List_GPDMA1_Channel3;
extern DMA_HandleTypeDef handle_GPDMA1_Channel3;
extern DMA_HandleTypeDef handle_GPDMA1_Channel1;
extern DMA_HandleTypeDef handle_GPDMA1_Channel0;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while(1)
		{
		}
	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
	/* USER CODE BEGIN HardFault_IRQn 0 */

	/* USER CODE END HardFault_IRQn 0 */
	while(1)
		{
			/* USER CODE BEGIN W1_HardFault_IRQn 0 */
			/* USER CODE END W1_HardFault_IRQn 0 */
		}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
	/* USER CODE BEGIN MemoryManagement_IRQn 0 */

	/* USER CODE END MemoryManagement_IRQn 0 */
	while(1)
		{
			/* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
			/* USER CODE END W1_MemoryManagement_IRQn 0 */
		}
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
	/* USER CODE BEGIN BusFault_IRQn 0 */

	/* USER CODE END BusFault_IRQn 0 */
	while(1)
		{
			/* USER CODE BEGIN W1_BusFault_IRQn 0 */
			/* USER CODE END W1_BusFault_IRQn 0 */
		}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
	/* USER CODE BEGIN UsageFault_IRQn 0 */

	/* USER CODE END UsageFault_IRQn 0 */
	while(1)
		{
			/* USER CODE BEGIN W1_UsageFault_IRQn 0 */
			/* USER CODE END W1_UsageFault_IRQn 0 */
		}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
	/* USER CODE BEGIN SVCall_IRQn 0 */

	/* USER CODE END SVCall_IRQn 0 */
	/* USER CODE BEGIN SVCall_IRQn 1 */

	/* USER CODE END SVCall_IRQn 1 */
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
	/* USER CODE BEGIN DebugMonitor_IRQn 0 */

	/* USER CODE END DebugMonitor_IRQn 0 */
	/* USER CODE BEGIN DebugMonitor_IRQn 1 */

	/* USER CODE END DebugMonitor_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
	/* USER CODE BEGIN PendSV_IRQn 0 */

	/* USER CODE END PendSV_IRQn 0 */
	/* USER CODE BEGIN PendSV_IRQn 1 */

	/* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	/* USER CODE BEGIN SysTick_IRQn 0 */

	/* USER CODE END SysTick_IRQn 0 */
	HAL_IncTick();
	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H5xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h5xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles EXTI Line2 interrupt.
 */
void EXTI2_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI2_IRQn 0 */

	/* USER CODE END EXTI2_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(USB_FS_OVCR_Pin);
	/* USER CODE BEGIN EXTI2_IRQn 1 */

	/* USER CODE END EXTI2_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line9 interrupt.
 */
void EXTI9_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI9_IRQn 0 */

	/* USER CODE END EXTI9_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(ADXL345_INT1_Pin);
	/* USER CODE BEGIN EXTI9_IRQn 1 */

	/* USER CODE END EXTI9_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line13 interrupt.
 */
void EXTI13_IRQHandler(void)
{
	/* USER CODE BEGIN EXTI13_IRQn 0 */

	/* USER CODE END EXTI13_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(Blue_User_Button_Pin);
	/* USER CODE BEGIN EXTI13_IRQn 1 */

	/* USER CODE END EXTI13_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 0 global interrupt.
 */
void GPDMA1_Channel0_IRQHandler(void)
{
	/* USER CODE BEGIN GPDMA1_Channel0_IRQn 0 */

	/* USER CODE END GPDMA1_Channel0_IRQn 0 */
	HAL_DMA_IRQHandler(&handle_GPDMA1_Channel0);
	/* USER CODE BEGIN GPDMA1_Channel0_IRQn 1 */

	/* USER CODE END GPDMA1_Channel0_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 1 global interrupt.
 */
void GPDMA1_Channel1_IRQHandler(void)
{
	/* USER CODE BEGIN GPDMA1_Channel1_IRQn 0 */

	/* USER CODE END GPDMA1_Channel1_IRQn 0 */
	HAL_DMA_IRQHandler(&handle_GPDMA1_Channel1);
	/* USER CODE BEGIN GPDMA1_Channel1_IRQn 1 */

	/* USER CODE END GPDMA1_Channel1_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 3 global interrupt.
 */
void GPDMA1_Channel3_IRQHandler(void)
{
	/* USER CODE BEGIN GPDMA1_Channel3_IRQn 0 */

	/* USER CODE END GPDMA1_Channel3_IRQn 0 */
	HAL_DMA_IRQHandler(&handle_GPDMA1_Channel3);
	/* USER CODE BEGIN GPDMA1_Channel3_IRQn 1 */

	/* USER CODE END GPDMA1_Channel3_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 5 global interrupt.
 */
void GPDMA1_Channel5_IRQHandler(void)
{
	/* USER CODE BEGIN GPDMA1_Channel5_IRQn 0 */
	tempvgps += __adc_dma_buffer[Vgps];  // sum up raw data
	tempvbat += __adc_dma_buffer[Vbat];  // ditto
	temptemp += __adc_dma_buffer[Temperature];  // ...
	tempvrefint += __adc_dma_buffer[Vrefint];  // ...
	++i;	// increment the iterator

	if(i == ADC_MEASURE_ITERATIONS)
		{
			tempvgps /= ADC_MEASURE_ITERATIONS;  // divide over iterations
			tempvbat /= ADC_MEASURE_ITERATIONS;  // ditto
			temptemp /= ADC_MEASURE_ITERATIONS;  // ...
			tempvrefint /= ADC_MEASURE_ITERATIONS;	// ...

			// raw data is enough to determine darkness levels, no need for Lux conversion
			__adc_results[Vgps] = (uint16_t) ((double) tempvgps / tempvrefint * _VddaConversionConstant);  // store computed average in result buffer

			/* ADC channel voltage calculation - see RM 0492, chapter 19.4.32, p. 269
			 *
			 * using the Vrefint reference channel, the formula for calculating the ADC channel voltage is:
			 * 	Vchannel = (Vdda_charact. * Vrefint_cal * ADC_data) / (Vrefint_data * full scale)
			 *
			 * 	of these, only ADC_data and Vrefint_data are variable, the rest are constants which can be computed in advance
			 * 	thus, the formula becomes:
			 *
			 * 	Vchannel = (ADC_data/Vrefint_data) * ( (Vdda_charact. * Vrefint_cal) / full scale )
			 * 		the latter term is computed once in the constructor.
			 * 		in absolute numbers it is 1203.9560439560439
			 *
			 * 	Vchannel becomes (ADC_data/Vrefint_data) * VddaConversionConstant, true battery voltage in mV
			 * 	this is then typecast into uint16_t to have a nice round number
			 */
			__adc_results[Vbat] = (uint16_t) ((double) tempvbat / tempvrefint * _VddaConversionConstant);  // store computed average in result buffer

			/* ADC temperature calculation - see RM0492, chapter 19.4.30, p. 265
			 *
			 *	https://techoverflow.net/2015/01/13/reading-stm32f0-internal-temperature-and-voltage-using-chibios/
			 */
			__adc_results[Temperature] = (((((double) (temptemp * *VREFINT_CAL_ADDR) / tempvrefint) - *TEMPSENSOR_CAL1_ADDR) * 1000.0) / (int16_t) (*TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR)) + 300;
			__adc_results[Vrefint] = tempvrefint;  // store computed average in result buffer

			tempvgps = 0;  // reset
			tempvbat = 0;
			temptemp = 0;
			tempvrefint = 0;
			i = 0;
		}
	/* USER CODE END GPDMA1_Channel5_IRQn 0 */
	HAL_DMA_IRQHandler(&handle_GPDMA1_Channel5);
	/* USER CODE BEGIN GPDMA1_Channel5_IRQn 1 */

	/* USER CODE END GPDMA1_Channel5_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 6 global interrupt.
 */
void GPDMA1_Channel6_IRQHandler(void)
{
	/* USER CODE BEGIN GPDMA1_Channel6_IRQn 0 */

	/* USER CODE END GPDMA1_Channel6_IRQn 0 */
	HAL_DMA_IRQHandler(&handle_GPDMA1_Channel6);
	/* USER CODE BEGIN GPDMA1_Channel6_IRQn 1 */

	/* USER CODE END GPDMA1_Channel6_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 7 global interrupt.
 */
void GPDMA1_Channel7_IRQHandler(void)
{
	/* USER CODE BEGIN GPDMA1_Channel7_IRQn 0 */

	/* USER CODE END GPDMA1_Channel7_IRQn 0 */
	HAL_DMA_IRQHandler(&handle_GPDMA1_Channel7);
	/* USER CODE BEGIN GPDMA1_Channel7_IRQn 1 */

	/* USER CODE END GPDMA1_Channel7_IRQn 1 */
}

/**
 * @brief This function handles ADC1 global interrupt.
 */
void ADC1_IRQHandler(void)
{
	/* USER CODE BEGIN ADC1_IRQn 0 */

	/* USER CODE END ADC1_IRQn 0 */
	HAL_ADC_IRQHandler(&hadc1);
	/* USER CODE BEGIN ADC1_IRQn 1 */

	/* USER CODE END ADC1_IRQn 1 */
}

/**
 * @brief This function handles FDCAN1 interrupt 0.
 */
void FDCAN1_IT0_IRQHandler(void)
{
	/* USER CODE BEGIN FDCAN1_IT0_IRQn 0 */

	/* USER CODE END FDCAN1_IT0_IRQn 0 */
	HAL_FDCAN_IRQHandler(&hfdcan1);
	/* USER CODE BEGIN FDCAN1_IT0_IRQn 1 */

	/* USER CODE END FDCAN1_IT0_IRQn 1 */
}

/**
 * @brief This function handles FDCAN1 interrupt 1.
 */
void FDCAN1_IT1_IRQHandler(void)
{
	/* USER CODE BEGIN FDCAN1_IT1_IRQn 0 */

	/* USER CODE END FDCAN1_IT1_IRQn 0 */
	HAL_FDCAN_IRQHandler(&hfdcan1);
	/* USER CODE BEGIN FDCAN1_IT1_IRQn 1 */

	/* USER CODE END FDCAN1_IT1_IRQn 1 */
}

/**
 * @brief This function handles TIM1 Capture Compare interrupt.
 */
void TIM1_CC_IRQHandler(void)
{
	/* USER CODE BEGIN TIM1_CC_IRQn 0 */
	HAL_GPIO_TogglePin(Debug_Out_GPIO_Port, Debug_Out_Pin);

	/* USER CODE END TIM1_CC_IRQn 0 */
	HAL_TIM_IRQHandler(&htim1);
	/* USER CODE BEGIN TIM1_CC_IRQn 1 */

#if PARSE_GGA
	if(ORG1510MK4->gga->fix == INS)  // check if we have an inertial navigation fix
		{
			HAL_GPIO_TogglePin(LED_Green_GPIO_Port, LED_Green_Pin);  // blink the LED
			return;
		}

	if(ORG1510MK4->gga->fix > 0)	// check if we have a GPS fix
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);	// light up green LED
	else
		HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);	// light down green LED
#endif
	/* USER CODE END TIM1_CC_IRQn 1 */
}

/**
 * @brief This function handles SPI1 global interrupt.
 */
void SPI1_IRQHandler(void)
{
	/* USER CODE BEGIN SPI1_IRQn 0 */

	/* USER CODE END SPI1_IRQn 0 */
	HAL_SPI_IRQHandler(&hspi1);
	/* USER CODE BEGIN SPI1_IRQn 1 */

	/* USER CODE END SPI1_IRQn 1 */
}

/**
 * @brief This function handles USART1 global interrupt.
 */
void USART1_IRQHandler(void)
{
	/* USER CODE BEGIN USART1_IRQn 0 */

	/* USER CODE END USART1_IRQn 0 */
	HAL_UART_IRQHandler(&huart1);
	/* USER CODE BEGIN USART1_IRQn 1 */

	/* USER CODE END USART1_IRQn 1 */
}

/**
 * @brief This function handles USART3 global interrupt.
 */
void USART3_IRQHandler(void)
{
	/* USER CODE BEGIN USART3_IRQn 0 */

	/* USER CODE END USART3_IRQn 0 */
	HAL_UART_IRQHandler(&huart3);
	/* USER CODE BEGIN USART3_IRQn 1 */

	/* USER CODE END USART3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
