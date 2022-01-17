#include <avr/io.h>

#include "main.h"																//
#include "uart/uart.h"															// for now debug output only

int main(void)
{
	uart0_init(UART_BAUD_SELECT(UART0_BAUD_RATE,F_CPU)); // USB
	uart0_puts("UART0 up\r\n");

	misko_ctor();

	uart0_puts("ping\r\n");

    while (1)
    {
		;
    }
};