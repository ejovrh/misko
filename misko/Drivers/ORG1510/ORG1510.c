#include "ORG1510.h"

#include "main.h"
#include "misko.h"

extern UART_HandleTypeDef huart1;  // UART towards GPS / BT
extern UART_HandleTypeDef huart2;  // UART towards debugger

typedef struct  // adxl345_t actual
{
	uint8_t _rxbuffer[UART1RXBUFFERSIZE];  // receive buffer for raw GPS NMEA sentences

	org1510_t public;  // public struct
} __org1510_t;

static __org1510_t   __ORG1510   __attribute__ ((section (".data")));  // preallocate __org1510 object in .data

void _parse(void)
{
	misko->FlagPrint = 1;
	HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);  // indicate it
	HAL_UART_Transmit_DMA(&huart2, __ORG1510._rxbuffer, UART1RXBUFFERSIZE);  // printout
}

org1510_t* org1510_ctor(void)  // object constructor
{
	__ORG1510.public.parse = &_parse;  // NMEA sentence parser

	HAL_UART_Receive_DMA(&huart1, __ORG1510._rxbuffer, UART1RXBUFFERSIZE);  // start receiving NMEA sentences

	return &__ORG1510.public;  // return address of public part; calling code accesses it via pointer
}
