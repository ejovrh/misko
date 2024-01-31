/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file         stm32h5xx_hal_msp.c
 * @brief        This file provides code for the MSP Initialization
 *               and de-Initialization codes.
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

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
extern DMA_NodeTypeDef Node_GPDMA1_Channel5;

extern DMA_QListTypeDef List_GPDMA1_Channel5;

extern DMA_HandleTypeDef handle_GPDMA1_Channel5;

extern DMA_HandleTypeDef handle_GPDMA1_Channel3;

extern DMA_HandleTypeDef handle_GPDMA1_Channel2;

extern DMA_HandleTypeDef handle_GPDMA1_Channel1;

extern DMA_HandleTypeDef handle_GPDMA1_Channel0;

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Define */

/* USER CODE END Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Macro */

/* USER CODE END Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* External functions --------------------------------------------------------*/
/* USER CODE BEGIN ExternalFunctions */

/* USER CODE END ExternalFunctions */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void)
{
	/* USER CODE BEGIN MspInit 0 */

	/* USER CODE END MspInit 0 */

	/* System interrupt init*/

	/* USER CODE BEGIN MspInit 1 */

	/* USER CODE END MspInit 1 */
}

/**
 * @brief ADC MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef GPIO_InitStruct =
		{0};
	DMA_NodeConfTypeDef NodeConfig;
	if(hadc->Instance == ADC1)
		{
			/* USER CODE BEGIN ADC1_MspInit 0 */

			/* USER CODE END ADC1_MspInit 0 */
			/* Peripheral clock enable */
			__HAL_RCC_ADC_CLK_ENABLE();

			__HAL_RCC_GPIOA_CLK_ENABLE();
			/**ADC1 GPIO Configuration
			 PA0     ------> ADC1_INP0
			 */
			GPIO_InitStruct.Pin = ADC_VGPS_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(ADC_VGPS_GPIO_Port, &GPIO_InitStruct);

			/* ADC1 DMA Init */
			/* GPDMA1_REQUEST_ADC1 Init */
			NodeConfig.NodeType = DMA_GPDMA_LINEAR_NODE;
			NodeConfig.Init.Request = GPDMA1_REQUEST_ADC1;
			NodeConfig.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
			NodeConfig.Init.Direction = DMA_PERIPH_TO_MEMORY;
			NodeConfig.Init.SrcInc = DMA_SINC_FIXED;
			NodeConfig.Init.DestInc = DMA_DINC_INCREMENTED;
			NodeConfig.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
			NodeConfig.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
			NodeConfig.Init.SrcBurstLength = 1;
			NodeConfig.Init.DestBurstLength = 1;
			NodeConfig.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
			NodeConfig.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
			NodeConfig.Init.Mode = DMA_NORMAL;
			NodeConfig.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
			NodeConfig.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
			NodeConfig.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
			if(HAL_DMAEx_List_BuildNode(&NodeConfig, &Node_GPDMA1_Channel5) != HAL_OK)
				{
					Error_Handler();
				}

			if(HAL_DMAEx_List_InsertNode(&List_GPDMA1_Channel5, NULL, &Node_GPDMA1_Channel5) != HAL_OK)
				{
					Error_Handler();
				}

			if(HAL_DMAEx_List_SetCircularMode(&List_GPDMA1_Channel5) != HAL_OK)
				{
					Error_Handler();
				}

			handle_GPDMA1_Channel5.Instance = GPDMA1_Channel5;
			handle_GPDMA1_Channel5.InitLinkedList.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
			handle_GPDMA1_Channel5.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
			handle_GPDMA1_Channel5.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
			handle_GPDMA1_Channel5.InitLinkedList.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
			handle_GPDMA1_Channel5.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;
			if(HAL_DMAEx_List_Init(&handle_GPDMA1_Channel5) != HAL_OK)
				{
					Error_Handler();
				}

			if(HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel5, &List_GPDMA1_Channel5) != HAL_OK)
				{
					Error_Handler();
				}

			__HAL_LINKDMA(hadc, DMA_Handle, handle_GPDMA1_Channel5);

			if(HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel5, DMA_CHANNEL_NPRIV) != HAL_OK)
				{
					Error_Handler();
				}

			/* ADC1 interrupt Init */
			HAL_NVIC_SetPriority(ADC1_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(ADC1_IRQn);
			/* USER CODE BEGIN ADC1_MspInit 1 */

			/* USER CODE END ADC1_MspInit 1 */
		}

}

/**
 * @brief ADC MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
	if(hadc->Instance == ADC1)
		{
			/* USER CODE BEGIN ADC1_MspDeInit 0 */

			/* USER CODE END ADC1_MspDeInit 0 */
			/* Peripheral clock disable */
			__HAL_RCC_ADC_CLK_DISABLE();

			/**ADC1 GPIO Configuration
			 PA0     ------> ADC1_INP0
			 */
			HAL_GPIO_DeInit(ADC_VGPS_GPIO_Port, ADC_VGPS_Pin);

			/* ADC1 DMA DeInit */
			HAL_DMA_DeInit(hadc->DMA_Handle);

			/* ADC1 interrupt DeInit */
			HAL_NVIC_DisableIRQ(ADC1_IRQn);
			/* USER CODE BEGIN ADC1_MspDeInit 1 */

			/* USER CODE END ADC1_MspDeInit 1 */
		}

}

/**
 * @brief FDCAN MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hfdcan: FDCAN handle pointer
 * @retval None
 */
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hfdcan)
{
	GPIO_InitTypeDef GPIO_InitStruct =
		{0};
	if(hfdcan->Instance == FDCAN1)
		{
			/* USER CODE BEGIN FDCAN1_MspInit 0 */

			/* USER CODE END FDCAN1_MspInit 0 */
			/* Peripheral clock enable */
			__HAL_RCC_FDCAN_CLK_ENABLE();

			__HAL_RCC_GPIOC_CLK_ENABLE();
			/**FDCAN1 GPIO Configuration
			 PC6     ------> FDCAN1_RX
			 PC7     ------> FDCAN1_TX
			 */
			GPIO_InitStruct.Pin = CAN_RX_Pin | CAN_TX_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

			/* FDCAN1 interrupt Init */
			HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
			HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
			/* USER CODE BEGIN FDCAN1_MspInit 1 */

			/* USER CODE END FDCAN1_MspInit 1 */
		}

}

/**
 * @brief FDCAN MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hfdcan: FDCAN handle pointer
 * @retval None
 */
void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *hfdcan)
{
	if(hfdcan->Instance == FDCAN1)
		{
			/* USER CODE BEGIN FDCAN1_MspDeInit 0 */

			/* USER CODE END FDCAN1_MspDeInit 0 */
			/* Peripheral clock disable */
			__HAL_RCC_FDCAN_CLK_DISABLE();

			/**FDCAN1 GPIO Configuration
			 PC6     ------> FDCAN1_RX
			 PC7     ------> FDCAN1_TX
			 */
			HAL_GPIO_DeInit(GPIOC, CAN_RX_Pin | CAN_TX_Pin);

			/* FDCAN1 interrupt DeInit */
			HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
			HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
			/* USER CODE BEGIN FDCAN1_MspDeInit 1 */

			/* USER CODE END FDCAN1_MspDeInit 1 */
		}

}

/**
 * @brief SPI MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hspi: SPI handle pointer
 * @retval None
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct =
		{0};
	if(hspi->Instance == SPI1)
		{
			/* USER CODE BEGIN SPI1_MspInit 0 */

			/* USER CODE END SPI1_MspInit 0 */
			/* Peripheral clock enable */
			__HAL_RCC_SPI1_CLK_ENABLE();

			__HAL_RCC_GPIOA_CLK_ENABLE();
			__HAL_RCC_GPIOB_CLK_ENABLE();
			/**SPI1 GPIO Configuration
			 PA8     ------> SPI1_SCK
			 PB4(NJTRST)     ------> SPI1_MISO
			 PB5     ------> SPI1_MOSI
			 */
			GPIO_InitStruct.Pin = GPIO_PIN_8;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF12_SPI1;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

			/* SPI1 interrupt Init */
			HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(SPI1_IRQn);
			/* USER CODE BEGIN SPI1_MspInit 1 */

			/* USER CODE END SPI1_MspInit 1 */
		}

}

/**
 * @brief SPI MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hspi: SPI handle pointer
 * @retval None
 */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)
		{
			/* USER CODE BEGIN SPI1_MspDeInit 0 */

			/* USER CODE END SPI1_MspDeInit 0 */
			/* Peripheral clock disable */
			__HAL_RCC_SPI1_CLK_DISABLE();

			/**SPI1 GPIO Configuration
			 PA8     ------> SPI1_SCK
			 PB4(NJTRST)     ------> SPI1_MISO
			 PB5     ------> SPI1_MOSI
			 */
			HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);

			HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4 | GPIO_PIN_5);

			/* SPI1 interrupt DeInit */
			HAL_NVIC_DisableIRQ(SPI1_IRQn);
			/* USER CODE BEGIN SPI1_MspDeInit 1 */

			/* USER CODE END SPI1_MspDeInit 1 */
		}

}

/**
 * @brief TIM_Base MSP Initialization
 * This function configures the hardware resources used in this example
 * @param htim_base: TIM_Base handle pointer
 * @retval None
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim_base)
{
	if(htim_base->Instance == TIM1)
		{
			/* USER CODE BEGIN TIM1_MspInit 0 */

			/* USER CODE END TIM1_MspInit 0 */
			/* Peripheral clock enable */
			__HAL_RCC_TIM1_CLK_ENABLE();
			/* TIM1 interrupt Init */
			HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
			/* USER CODE BEGIN TIM1_MspInit 1 */

			/* USER CODE END TIM1_MspInit 1 */
		}

}

/**
 * @brief TIM_Base MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param htim_base: TIM_Base handle pointer
 * @retval None
 */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim_base)
{
	if(htim_base->Instance == TIM1)
		{
			/* USER CODE BEGIN TIM1_MspDeInit 0 */

			/* USER CODE END TIM1_MspDeInit 0 */
			/* Peripheral clock disable */
			__HAL_RCC_TIM1_CLK_DISABLE();

			/* TIM1 interrupt DeInit */
			HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
			/* USER CODE BEGIN TIM1_MspDeInit 1 */

			/* USER CODE END TIM1_MspDeInit 1 */
		}

}

/**
 * @brief UART MSP Initialization
 * This function configures the hardware resources used in this example
 * @param huart: UART handle pointer
 * @retval None
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStruct =
		{0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct =
		{0};
	if(huart->Instance == USART1)
		{
			/* USER CODE BEGIN USART1_MspInit 0 */

			/* USER CODE END USART1_MspInit 0 */

			/** Initializes the peripherals clock
			 */
			PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
			PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
			if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
				{
					Error_Handler();
				}

			/* Peripheral clock enable */
			__HAL_RCC_USART1_CLK_ENABLE();

			__HAL_RCC_GPIOB_CLK_ENABLE();
			/**USART1 GPIO Configuration
			 PB14     ------> USART1_TX
			 PB15     ------> USART1_RX
			 */
			GPIO_InitStruct.Pin = UART1_TX_Pin | UART1_RX_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

			/* USART1 DMA Init */
			/* GPDMA1_REQUEST_USART1_RX Init */
			handle_GPDMA1_Channel3.Instance = GPDMA1_Channel3;
			handle_GPDMA1_Channel3.Init.Request = GPDMA1_REQUEST_USART1_RX;
			handle_GPDMA1_Channel3.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
			handle_GPDMA1_Channel3.Init.Direction = DMA_PERIPH_TO_MEMORY;
			handle_GPDMA1_Channel3.Init.SrcInc = DMA_SINC_FIXED;
			handle_GPDMA1_Channel3.Init.DestInc = DMA_DINC_INCREMENTED;
			handle_GPDMA1_Channel3.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
			handle_GPDMA1_Channel3.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
			handle_GPDMA1_Channel3.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
			handle_GPDMA1_Channel3.Init.SrcBurstLength = 1;
			handle_GPDMA1_Channel3.Init.DestBurstLength = 1;
			handle_GPDMA1_Channel3.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
			handle_GPDMA1_Channel3.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
			handle_GPDMA1_Channel3.Init.Mode = DMA_NORMAL;
			if(HAL_DMA_Init(&handle_GPDMA1_Channel3) != HAL_OK)
				{
					Error_Handler();
				}

			__HAL_LINKDMA(huart, hdmarx, handle_GPDMA1_Channel3);

			if(HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel3, DMA_CHANNEL_NPRIV) != HAL_OK)
				{
					Error_Handler();
				}

			/* GPDMA1_REQUEST_USART1_TX Init */
			handle_GPDMA1_Channel2.Instance = GPDMA1_Channel2;
			handle_GPDMA1_Channel2.Init.Request = GPDMA1_REQUEST_USART1_TX;
			handle_GPDMA1_Channel2.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
			handle_GPDMA1_Channel2.Init.Direction = DMA_PERIPH_TO_MEMORY;
			handle_GPDMA1_Channel2.Init.SrcInc = DMA_SINC_INCREMENTED;
			handle_GPDMA1_Channel2.Init.DestInc = DMA_DINC_FIXED;
			handle_GPDMA1_Channel2.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
			handle_GPDMA1_Channel2.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
			handle_GPDMA1_Channel2.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
			handle_GPDMA1_Channel2.Init.SrcBurstLength = 1;
			handle_GPDMA1_Channel2.Init.DestBurstLength = 1;
			handle_GPDMA1_Channel2.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
			handle_GPDMA1_Channel2.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
			handle_GPDMA1_Channel2.Init.Mode = DMA_NORMAL;
			if(HAL_DMA_Init(&handle_GPDMA1_Channel2) != HAL_OK)
				{
					Error_Handler();
				}

			__HAL_LINKDMA(huart, hdmatx, handle_GPDMA1_Channel2);

			if(HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel2, DMA_CHANNEL_NPRIV) != HAL_OK)
				{
					Error_Handler();
				}

			/* USART1 interrupt Init */
			HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(USART1_IRQn);
			/* USER CODE BEGIN USART1_MspInit 1 */

			/* USER CODE END USART1_MspInit 1 */
		}
	else if(huart->Instance == USART3)
		{
			/* USER CODE BEGIN USART3_MspInit 0 */

			/* USER CODE END USART3_MspInit 0 */

			/** Initializes the peripherals clock
			 */
			PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
			PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_HSI;
			if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
				{
					Error_Handler();
				}

			/* Peripheral clock enable */
			__HAL_RCC_USART3_CLK_ENABLE();

			__HAL_RCC_GPIOA_CLK_ENABLE();
			/**USART3 GPIO Configuration
			 PA3     ------> USART3_RX
			 PA4     ------> USART3_TX
			 */
			GPIO_InitStruct.Pin = UART3_VCP_RX_Pin | UART3_VCP_TX_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF13_USART3;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			/* USART3 DMA Init */
			/* GPDMA1_REQUEST_USART3_RX Init */
			handle_GPDMA1_Channel1.Instance = GPDMA1_Channel1;
			handle_GPDMA1_Channel1.Init.Request = GPDMA1_REQUEST_USART3_RX;
			handle_GPDMA1_Channel1.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
			handle_GPDMA1_Channel1.Init.Direction = DMA_PERIPH_TO_MEMORY;
			handle_GPDMA1_Channel1.Init.SrcInc = DMA_SINC_FIXED;
			handle_GPDMA1_Channel1.Init.DestInc = DMA_DINC_INCREMENTED;
			handle_GPDMA1_Channel1.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
			handle_GPDMA1_Channel1.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
			handle_GPDMA1_Channel1.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
			handle_GPDMA1_Channel1.Init.SrcBurstLength = 1;
			handle_GPDMA1_Channel1.Init.DestBurstLength = 1;
			handle_GPDMA1_Channel1.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
			handle_GPDMA1_Channel1.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
			handle_GPDMA1_Channel1.Init.Mode = DMA_NORMAL;
			if(HAL_DMA_Init(&handle_GPDMA1_Channel1) != HAL_OK)
				{
					Error_Handler();
				}

			__HAL_LINKDMA(huart, hdmarx, handle_GPDMA1_Channel1);

			if(HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel1, DMA_CHANNEL_NPRIV) != HAL_OK)
				{
					Error_Handler();
				}

			/* GPDMA1_REQUEST_USART3_TX Init */
			handle_GPDMA1_Channel0.Instance = GPDMA1_Channel0;
			handle_GPDMA1_Channel0.Init.Request = GPDMA1_REQUEST_USART3_TX;
			handle_GPDMA1_Channel0.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
			handle_GPDMA1_Channel0.Init.Direction = DMA_PERIPH_TO_MEMORY;
			handle_GPDMA1_Channel0.Init.SrcInc = DMA_SINC_INCREMENTED;
			handle_GPDMA1_Channel0.Init.DestInc = DMA_DINC_FIXED;
			handle_GPDMA1_Channel0.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
			handle_GPDMA1_Channel0.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
			handle_GPDMA1_Channel0.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
			handle_GPDMA1_Channel0.Init.SrcBurstLength = 1;
			handle_GPDMA1_Channel0.Init.DestBurstLength = 1;
			handle_GPDMA1_Channel0.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
			handle_GPDMA1_Channel0.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
			handle_GPDMA1_Channel0.Init.Mode = DMA_NORMAL;
			if(HAL_DMA_Init(&handle_GPDMA1_Channel0) != HAL_OK)
				{
					Error_Handler();
				}

			__HAL_LINKDMA(huart, hdmatx, handle_GPDMA1_Channel0);

			if(HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel0, DMA_CHANNEL_NPRIV) != HAL_OK)
				{
					Error_Handler();
				}

			/* USART3 interrupt Init */
			HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
			HAL_NVIC_EnableIRQ(USART3_IRQn);
			/* USER CODE BEGIN USART3_MspInit 1 */

			/* USER CODE END USART3_MspInit 1 */
		}

}

/**
 * @brief UART MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param huart: UART handle pointer
 * @retval None
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
		{
			/* USER CODE BEGIN USART1_MspDeInit 0 */

			/* USER CODE END USART1_MspDeInit 0 */
			/* Peripheral clock disable */
			__HAL_RCC_USART1_CLK_DISABLE();

			/**USART1 GPIO Configuration
			 PB14     ------> USART1_TX
			 PB15     ------> USART1_RX
			 */
			HAL_GPIO_DeInit(GPIOB, UART1_TX_Pin | UART1_RX_Pin);

			/* USART1 DMA DeInit */
			HAL_DMA_DeInit(huart->hdmarx);
			HAL_DMA_DeInit(huart->hdmatx);

			/* USART1 interrupt DeInit */
			HAL_NVIC_DisableIRQ(USART1_IRQn);
			/* USER CODE BEGIN USART1_MspDeInit 1 */

			/* USER CODE END USART1_MspDeInit 1 */
		}
	else if(huart->Instance == USART3)
		{
			/* USER CODE BEGIN USART3_MspDeInit 0 */

			/* USER CODE END USART3_MspDeInit 0 */
			/* Peripheral clock disable */
			__HAL_RCC_USART3_CLK_DISABLE();

			/**USART3 GPIO Configuration
			 PA3     ------> USART3_RX
			 PA4     ------> USART3_TX
			 */
			HAL_GPIO_DeInit(GPIOA, UART3_VCP_RX_Pin | UART3_VCP_TX_Pin);

			/* USART3 DMA DeInit */
			HAL_DMA_DeInit(huart->hdmarx);
			HAL_DMA_DeInit(huart->hdmatx);

			/* USART3 interrupt DeInit */
			HAL_NVIC_DisableIRQ(USART3_IRQn);
			/* USER CODE BEGIN USART3_MspDeInit 1 */

			/* USER CODE END USART3_MspDeInit 1 */
		}

}

/**
 * @brief PCD MSP Initialization
 * This function configures the hardware resources used in this example
 * @param hpcd: PCD handle pointer
 * @retval None
 */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
	GPIO_InitTypeDef GPIO_InitStruct =
		{0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct =
		{0};
	if(hpcd->Instance == USB_DRD_FS)
		{
			/* USER CODE BEGIN USB_DRD_FS_MspInit 0 */

			/* USER CODE END USB_DRD_FS_MspInit 0 */

			/** Initializes the peripherals clock
			 */
			PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
			PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
			if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
				{
					Error_Handler();
				}

			__HAL_RCC_GPIOA_CLK_ENABLE();
			/**USB GPIO Configuration
			 PA11     ------> USB_DM
			 PA12     ------> USB_DP
			 */
			GPIO_InitStruct.Pin = USB_FS_DN_Pin | USB_FS_DP_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF10_USB;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			/* Peripheral clock enable */
			__HAL_RCC_USB_CLK_ENABLE();
			/* USER CODE BEGIN USB_DRD_FS_MspInit 1 */

			/* USER CODE END USB_DRD_FS_MspInit 1 */
		}

}

/**
 * @brief PCD MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param hpcd: PCD handle pointer
 * @retval None
 */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
	if(hpcd->Instance == USB_DRD_FS)
		{
			/* USER CODE BEGIN USB_DRD_FS_MspDeInit 0 */

			/* USER CODE END USB_DRD_FS_MspDeInit 0 */
			/* Peripheral clock disable */
			__HAL_RCC_USB_CLK_DISABLE();

			/**USB GPIO Configuration
			 PA11     ------> USB_DM
			 PA12     ------> USB_DP
			 */
			HAL_GPIO_DeInit(GPIOA, USB_FS_DN_Pin | USB_FS_DP_Pin);

			/* USER CODE BEGIN USB_DRD_FS_MspDeInit 1 */

			/* USER CODE END USB_DRD_FS_MspDeInit 1 */
		}

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
