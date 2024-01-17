/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "linked_list.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern DMA_QListTypeDef Queue_tx;
extern DMA_QListTypeDef Queue_rx;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Blue_User_Button_Pin GPIO_PIN_13
#define Blue_User_Button_GPIO_Port GPIOC
#define Blue_User_Button_EXTI_IRQn EXTI13_IRQn
#define ADC_VGPS_Pin GPIO_PIN_0
#define ADC_VGPS_GPIO_Port GPIOA
#define UART3_VCP_RX_Pin GPIO_PIN_3
#define UART3_VCP_RX_GPIO_Port GPIOA
#define UART3_VCP_TX_Pin GPIO_PIN_4
#define UART3_VCP_TX_GPIO_Port GPIOA
#define User_LED_Pin GPIO_PIN_5
#define User_LED_GPIO_Port GPIOA
#define SPI1_FRAM_CS_Pin GPIO_PIN_6
#define SPI1_FRAM_CS_GPIO_Port GPIOA
#define SPI1_ADXL345_CS_Pin GPIO_PIN_7
#define SPI1_ADXL345_CS_GPIO_Port GPIOA
#define GPS_PWR_CTRL_Pin GPIO_PIN_4
#define GPS_PWR_CTRL_GPIO_Port GPIOC
#define UART1_TX_Pin GPIO_PIN_14
#define UART1_TX_GPIO_Port GPIOB
#define UART1_RX_Pin GPIO_PIN_15
#define UART1_RX_GPIO_Port GPIOB
#define CAN_RX_Pin GPIO_PIN_6
#define CAN_RX_GPIO_Port GPIOC
#define CAN_TX_Pin GPIO_PIN_7
#define CAN_TX_GPIO_Port GPIOC
#define Debug_Out_Pin GPIO_PIN_8
#define Debug_Out_GPIO_Port GPIOC
#define ADXL345_INT1_Pin GPIO_PIN_9
#define ADXL345_INT1_GPIO_Port GPIOC
#define ADXL345_INT1_EXTI_IRQn EXTI9_IRQn
#define USB_FS_VBUS_Pin GPIO_PIN_9
#define USB_FS_VBUS_GPIO_Port GPIOA
#define GPS_WKUP_Pin GPIO_PIN_10
#define GPS_WKUP_GPIO_Port GPIOA
#define USB_FS_DN_Pin GPIO_PIN_11
#define USB_FS_DN_GPIO_Port GPIOA
#define USB_FS_DP_Pin GPIO_PIN_12
#define USB_FS_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define JTDI_Pin GPIO_PIN_15
#define JTDI_GPIO_Port GPIOA
#define USB_FS_PWR_EN_Pin GPIO_PIN_10
#define USB_FS_PWR_EN_GPIO_Port GPIOC
#define GPS_Green_LED_Pin GPIO_PIN_11
#define GPS_Green_LED_GPIO_Port GPIOC
#define GPS_Red_LED_Pin GPIO_PIN_12
#define GPS_Red_LED_GPIO_Port GPIOC
#define USB_FS_OVCR_Pin GPIO_PIN_2
#define USB_FS_OVCR_GPIO_Port GPIOD
#define USB_FS_OVCR_EXTI_IRQn EXTI2_IRQn
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define Anal_SW_CTRL_Pin GPIO_PIN_8
#define Anal_SW_CTRL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define USE_ADXL345 1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */