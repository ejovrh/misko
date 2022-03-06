#include "misko.h"

#include "main.h"

extern void SystemClock_Config(void);
extern UART_HandleTypeDef huart2;

//#include "SDCard/SDCard.h"
//
//#include "EROLED0152W/EROLED0152W.h"
//#include "RN4678/RN4678.h"
//#include "ORG1510-MK4/ORG1510-MK4.h"
//#include "LED/LED.h"

typedef struct  // adxl345_t actual
{
	misko_t public;  // public struct
} __misko_t;

static __misko_t  __misko  __attribute__ ((section (".data")));  // preallocate __misko_t object in .data

void _StopMode(const uint8_t in_flag)  // stop mode powersave function
{
	if(in_flag)  // if flag is set
		{
			__misko.public.adxl345->FlagStop = 0;  // unflag accelerator stop mode - evaluated in main()

			HAL_UART_Transmit_IT(&huart2, (uint8_t*) "accel. sleep\r\n", 14);  // indicate via UART message
			while(HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY)
				;

			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);  // turn LED off
			HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);  // ditto

			HAL_SuspendTick();  // suspend systick
			HAL_PWR_EnableSleepOnExit();  // enable sleep mode
			HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);  // enter stop mode and wait for any interrupt
		}
	else  // if flag is not set
		{
			SystemClock_Config();  // reconfigure clocks
			HAL_ResumeTick();  // resume systick
			HAL_PWR_DisableSleepOnExit();  // disable stop mode
		}
}

misko_t* misko_ctor(void)
{
	__misko.public.StopMode = &_StopMode;  //

	__misko.public.flag_print = 0;  // flag if in main() a debug print shall happen

	__misko.public.adxl345 = adxl345_ctor();  // initialize accelerometer
	__misko.public.fm25w256 = fm25w256_ctor();  // initialize RAM storage
//	__misko.public.ds1394 = ds1394_ctor();// initialize RTC

	return &__misko.public;  // return address of public part; calling code accesses it via pointer
}
