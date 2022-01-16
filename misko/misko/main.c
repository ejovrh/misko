#include <avr/io.h>

#include "main.h"																//
#include "uart/uart.h"

volatile uint8_t i = 0;

int main(void)
{
	misko_ctor();

	uart0_init(UART_BAUD_SELECT(UART0_BAUD_RATE,F_CPU)); // USB
	uart0_puts("UART0 up\r\n");

    while (1)
    {
		// FIXME: ADXL345 code works kinda in polling mode (something has to constantly read the INT_SOURCE register)
 		i = misko->adxl345->ReadByte(INT_SOURCE);
    }
};