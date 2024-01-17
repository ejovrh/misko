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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#if USE_ADXL345
#include "adxl345/adxl345.h"
#endif
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
ADC_HandleTypeDef hadc1;

FDCAN_HandleTypeDef hfdcan1;

DMA_HandleTypeDef handle_GPDMA1_Channel7;
DMA_HandleTypeDef handle_GPDMA1_Channel6;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef handle_GPDMA1_Channel3;
DMA_HandleTypeDef handle_GPDMA1_Channel2;
DMA_HandleTypeDef handle_GPDMA1_Channel1;
DMA_HandleTypeDef handle_GPDMA1_Channel0;

PCD_HandleTypeDef hpcd_USB_DRD_FS;

/* USER CODE BEGIN PV */
uint8_t aTxBuffer[] = "\rnucleo-h503rb start\r\n";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ICACHE_Init(void);
static void MX_USB_PCD_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_SPI1_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

	/* Configure the peripherals common clocks */
	PeriphCommonClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_GPDMA1_Init();
	MX_USART1_UART_Init();
	MX_ICACHE_Init();
	MX_USB_PCD_Init();
	MX_USART3_UART_Init();
	MX_TIM1_Init();
	MX_SPI1_Init();
	MX_FDCAN1_Init();
	MX_ADC1_Init();
	/* USER CODE BEGIN 2 */
	MX_Queue_tx_Config();
	HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel7, &Queue_tx);
	__HAL_LINKDMA(&hspi1, hdmatx, handle_GPDMA1_Channel7);
	MX_Queue_rx_Config();
	HAL_DMAEx_List_LinkQ(&handle_GPDMA1_Channel6, &Queue_rx);
	__HAL_LINKDMA(&hspi1, hdmarx, handle_GPDMA1_Channel6);

	adxl345_ctor(&hspi1, SPI1_ADXL345_CS_GPIO_Port, SPI1_ADXL345_CS_Pin);  // initialize accelerometer object

	if(HAL_UART_Transmit_DMA(&huart3, (uint8_t*) aTxBuffer, 22) != HAL_OK)	// transmit hello world
		Error_Handler();

	if(HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1) != HAL_OK)	// 125ms time base
		Error_Handler();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while(1)
		{
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
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
		{
		}

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		{
			Error_Handler();
		}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
		{
			Error_Handler();
		}
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void)
{
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct =
		{0};

	/** Initializes the peripherals clock
	 */
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADCDAC | RCC_PERIPHCLK_FDCAN | RCC_PERIPHCLK_SPI1;
	PeriphClkInitStruct.PLL2.PLL2Source = RCC_PLL2_SOURCE_HSI;
	PeriphClkInitStruct.PLL2.PLL2M = 4;
	PeriphClkInitStruct.PLL2.PLL2N = 8;
	PeriphClkInitStruct.PLL2.PLL2P = 16;
	PeriphClkInitStruct.PLL2.PLL2Q = 16;
	PeriphClkInitStruct.PLL2.PLL2R = 16;
	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2_VCIRANGE_3;
	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2_VCORANGE_WIDE;
	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
	PeriphClkInitStruct.PLL2.PLL2ClockOut = RCC_PLL2_DIVP | RCC_PLL2_DIVQ | RCC_PLL2_DIVR;
	PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL2Q;
	PeriphClkInitStruct.AdcDacClockSelection = RCC_ADCDACCLKSOURCE_PLL2R;
	PeriphClkInitStruct.Spi1ClockSelection = RCC_SPI1CLKSOURCE_PLL2P;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
			Error_Handler();
		}
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void)
{

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig =
		{0};

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO2;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.SamplingMode = ADC_SAMPLING_MODE_NORMAL;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.OversamplingMode = DISABLE;
	if(HAL_ADC_Init(&hadc1) != HAL_OK)
		{
			Error_Handler();
		}

	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_VREFINT;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if(HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
		{
			Error_Handler();
		}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief FDCAN1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_FDCAN1_Init(void)
{

	/* USER CODE BEGIN FDCAN1_Init 0 */

	/* USER CODE END FDCAN1_Init 0 */

	/* USER CODE BEGIN FDCAN1_Init 1 */

	/* USER CODE END FDCAN1_Init 1 */
	hfdcan1.Instance = FDCAN1;
	hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
	hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
	hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
	hfdcan1.Init.AutoRetransmission = DISABLE;
	hfdcan1.Init.TransmitPause = DISABLE;
	hfdcan1.Init.ProtocolException = DISABLE;
	hfdcan1.Init.NominalPrescaler = 16;
	hfdcan1.Init.NominalSyncJumpWidth = 1;
	hfdcan1.Init.NominalTimeSeg1 = 2;
	hfdcan1.Init.NominalTimeSeg2 = 2;
	hfdcan1.Init.DataPrescaler = 1;
	hfdcan1.Init.DataSyncJumpWidth = 1;
	hfdcan1.Init.DataTimeSeg1 = 1;
	hfdcan1.Init.DataTimeSeg2 = 1;
	hfdcan1.Init.StdFiltersNbr = 0;
	hfdcan1.Init.ExtFiltersNbr = 0;
	hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
	if(HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
		{
			Error_Handler();
		}
	/* USER CODE BEGIN FDCAN1_Init 2 */

	/* USER CODE END FDCAN1_Init 2 */

}

/**
 * @brief GPDMA1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPDMA1_Init(void)
{

	/* USER CODE BEGIN GPDMA1_Init 0 */

	/* USER CODE END GPDMA1_Init 0 */

	/* Peripheral clock enable */
	__HAL_RCC_GPDMA1_CLK_ENABLE();

	/* GPDMA1 interrupt Init */
	HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
	HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);
	HAL_NVIC_SetPriority(GPDMA1_Channel2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);
	HAL_NVIC_SetPriority(GPDMA1_Channel3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(GPDMA1_Channel3_IRQn);
	HAL_NVIC_SetPriority(GPDMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(GPDMA1_Channel6_IRQn);
	HAL_NVIC_SetPriority(GPDMA1_Channel7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(GPDMA1_Channel7_IRQn);

	/* USER CODE BEGIN GPDMA1_Init 1 */

	/* USER CODE END GPDMA1_Init 1 */
	handle_GPDMA1_Channel7.Instance = GPDMA1_Channel7;
	handle_GPDMA1_Channel7.InitLinkedList.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
	handle_GPDMA1_Channel7.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
	handle_GPDMA1_Channel7.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT1;
	handle_GPDMA1_Channel7.InitLinkedList.TransferEventMode = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
	handle_GPDMA1_Channel7.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_NORMAL;
	if(HAL_DMAEx_List_Init(&handle_GPDMA1_Channel7) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel7, DMA_CHANNEL_NPRIV) != HAL_OK)
		{
			Error_Handler();
		}
	handle_GPDMA1_Channel6.Instance = GPDMA1_Channel6;
	handle_GPDMA1_Channel6.InitLinkedList.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
	handle_GPDMA1_Channel6.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
	handle_GPDMA1_Channel6.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT0;
	handle_GPDMA1_Channel6.InitLinkedList.TransferEventMode = DMA_TCEM_LAST_LL_ITEM_TRANSFER;
	handle_GPDMA1_Channel6.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_NORMAL;
	if(HAL_DMAEx_List_Init(&handle_GPDMA1_Channel6) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel6, DMA_CHANNEL_NPRIV) != HAL_OK)
		{
			Error_Handler();
		}
	/* USER CODE BEGIN GPDMA1_Init 2 */

	/* USER CODE END GPDMA1_Init 2 */

}

/**
 * @brief ICACHE Initialization Function
 * @param None
 * @retval None
 */
static void MX_ICACHE_Init(void)
{

	/* USER CODE BEGIN ICACHE_Init 0 */

	/* USER CODE END ICACHE_Init 0 */

	/* USER CODE BEGIN ICACHE_Init 1 */

	/* USER CODE END ICACHE_Init 1 */

	/** Enable instruction cache (default 2-ways set associative cache)
	 */
	if(HAL_ICACHE_Enable() != HAL_OK)
		{
			Error_Handler();
		}
	/* USER CODE BEGIN ICACHE_Init 2 */

	/* USER CODE END ICACHE_Init 2 */

}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 0x7;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
	hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
	hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
	hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
	hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
	hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
	hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
	hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
	hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
	if(HAL_SPI_Init(&hspi1) != HAL_OK)
		{
			Error_Handler();
		}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void)
{

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig =
		{0};
	TIM_MasterConfigTypeDef sMasterConfig =
		{0};
	TIM_OC_InitTypeDef sConfigOC =
		{0};
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig =
		{0};

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 639;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 6249;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if(HAL_TIM_Base_Init(&htim1) != HAL_OK)
		{
			Error_Handler();
		}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if(HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_TIM_OC_Init(&htim1) != HAL_OK)
		{
			Error_Handler();
		}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if(HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
		{
			Error_Handler();
		}
	sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if(HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
		{
			Error_Handler();
		}
	sConfigOC.OCMode = TIM_OCMODE_TIMING;
	if(HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
		{
			Error_Handler();
		}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if(HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
		{
			Error_Handler();
		}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
{

	/* USER CODE BEGIN USART1_Init 0 */

	/* USER CODE END USART1_Init 0 */

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if(HAL_UART_Init(&huart1) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
		{
			Error_Handler();
		}
	/* USER CODE BEGIN USART1_Init 2 */

	/* USER CODE END USART1_Init 2 */

}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void)
{

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if(HAL_UART_Init(&huart3) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
		{
			Error_Handler();
		}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
 * @brief USB Initialization Function
 * @param None
 * @retval None
 */
static void MX_USB_PCD_Init(void)
{

	/* USER CODE BEGIN USB_Init 0 */

	/* USER CODE END USB_Init 0 */

	/* USER CODE BEGIN USB_Init 1 */

	/* USER CODE END USB_Init 1 */
	hpcd_USB_DRD_FS.Instance = USB_DRD_FS;
	hpcd_USB_DRD_FS.Init.dev_endpoints = 8;
	hpcd_USB_DRD_FS.Init.speed = USBD_FS_SPEED;
	hpcd_USB_DRD_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
	hpcd_USB_DRD_FS.Init.Sof_enable = DISABLE;
	hpcd_USB_DRD_FS.Init.low_power_enable = DISABLE;
	hpcd_USB_DRD_FS.Init.lpm_enable = DISABLE;
	hpcd_USB_DRD_FS.Init.battery_charging_enable = DISABLE;
	hpcd_USB_DRD_FS.Init.vbus_sensing_enable = DISABLE;
	hpcd_USB_DRD_FS.Init.bulk_doublebuffer_enable = DISABLE;
	hpcd_USB_DRD_FS.Init.iso_singlebuffer_enable = DISABLE;
	if(HAL_PCD_Init(&hpcd_USB_DRD_FS) != HAL_OK)
		{
			Error_Handler();
		}
	/* USER CODE BEGIN USB_Init 2 */

	/* USER CODE END USB_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct =
		{0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(User_LED_GPIO_Port, User_LED_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, SPI1_FRAM_CS_Pin | SPI1_ADXL345_CS_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPS_PWR_CTRL_Pin | Debug_Out_Pin | USB_FS_PWR_EN_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPS_Green_LED_Pin | GPS_Red_LED_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(Anal_SW_CTRL_GPIO_Port, Anal_SW_CTRL_Pin, GPIO_PIN_SET);

	/*Configure GPIO pins : Blue_User_Button_Pin ADXL345_INT1_Pin */
	GPIO_InitStruct.Pin = Blue_User_Button_Pin | ADXL345_INT1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PC0 PC1 PC2 PC3
	 PC5 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PA1 PA2 */
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : User_LED_Pin SPI1_FRAM_CS_Pin SPI1_ADXL345_CS_Pin */
	GPIO_InitStruct.Pin = User_LED_Pin | SPI1_FRAM_CS_Pin | SPI1_ADXL345_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : GPS_PWR_CTRL_Pin */
	GPIO_InitStruct.Pin = GPS_PWR_CTRL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPS_PWR_CTRL_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PB0 PB1 PB2 PB10
	 PB12 PB13 PB6 PB7 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : Debug_Out_Pin */
	GPIO_InitStruct.Pin = Debug_Out_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(Debug_Out_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : USB_FS_VBUS_Pin */
	GPIO_InitStruct.Pin = USB_FS_VBUS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(USB_FS_VBUS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : GPS_WKUP_Pin */
	GPIO_InitStruct.Pin = GPS_WKUP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPS_WKUP_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : USB_FS_PWR_EN_Pin GPS_Green_LED_Pin GPS_Red_LED_Pin */
	GPIO_InitStruct.Pin = USB_FS_PWR_EN_Pin | GPS_Green_LED_Pin | GPS_Red_LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : USB_FS_OVCR_Pin */
	GPIO_InitStruct.Pin = USB_FS_OVCR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(USB_FS_OVCR_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : Anal_SW_CTRL_Pin */
	GPIO_InitStruct.Pin = Anal_SW_CTRL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(Anal_SW_CTRL_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	HAL_NVIC_SetPriority(EXTI9_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI9_IRQn);

	HAL_NVIC_SetPriority(EXTI13_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI13_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	HAL_NVIC_DisableIRQ(EXTI9_IRQn);	// disable until accelerometer initialization is complete - see adxl345_ctor()
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// EXTI rising edge callback
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == Blue_User_Button_Pin)  // if button is pressed
		{
			HAL_GPIO_TogglePin(User_LED_GPIO_Port, User_LED_Pin);
			HAL_GPIO_TogglePin(GPS_Green_LED_GPIO_Port, GPS_Green_LED_Pin);
		}

	if(GPIO_Pin == ADXL345_INT1_Pin)  // accelerometer interrupt (act. or inact.)
		ADXL345->ISR();  // execute the ISR callback
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
