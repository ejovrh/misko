#include <avr/io.h>
#include <stdlib.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#include "main.h"																//
#include "uart/uart.h"															// for now debug output only

int main(void)
{
	uart0_init(UART_BAUD_SELECT(UART0_BAUD_RATE,F_CPU));						// UART for USB uplink
	uart0_puts("UART0 up\r\n");													//

	misko_ctor();																// initialize device

	char c[1];																	// buffer for integer to ascii char conversion

    while (1)
    {
		cli();																	// sleep sequence atomic start
		if (misko->mcu->flag_sleep)
		{
			SMCR = _BV(SM1);													// set sleep mode to "power down"
			gpio_set(gps_red_led_pin);											// indicate inact. by turning red LED on
			gpio_set(gps_green_led_pin);										// turn green LED off
			misko->adxl345->ReadByte(INT_SOURCE);								// clear any pending interrupts
			misko->adxl345->WriteByte(INT_ENABLE, 0x00);						// disable interrupts
			misko->adxl345->WriteByte(INT_ENABLE, INT_ENABLE_ACT_CFG);			// configure for activity detection
			SMCR |= _BV(SE);													// enable sleep
			sei();
			sleep_cpu();														// sleep
			// wakeup code is executed in INT0 ISR
		}
		sei();																	// sleep sequence atomic end

		if (misko->flag_print)													// foo debug print
		{
			itoa(misko->mcu->foocounter, c, 10);								// convert integer to ascii char
			uart0_puts(c);														// send out
			uart0_puts("\r\n");													// also send out newline & carriage return
			misko->flag_print = 0;												// reset
		}
    }
};