#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart/uart.h"

// local files
#include "gpio.h"
#include "gpio_definitions.h"


#define UART0_BAUD_RATE 9600
#define UART1_BAUD_RATE 9600


char str_uart[] = "hello world";

int main(void)
{
	#include "gpio_modes.h"

	cli(); // globally disable interrupts
	TCCR5A  = 0x0; // clear the register (A and B)
	TCCR5B  = 0x0;
	OCR5A = 3900; // set compare match register to desired timer count
	TCCR5B |= (1 << WGM12); // turn on CTC mode
	TCCR5B |= (1 << CS10); // Set CS10 and CS12 bits for 1024 prescaler
	TCCR5B |= (1 << CS12);
	TIMSK5 |= (1 << OCIE5A); // enable timer compare interrupt

	// hardware UART initialization
	uart0_init(UART_BAUD_SELECT(UART0_BAUD_RATE,F_CPU)); // USB
	uart1_init(UART_BAUD_SELECT(UART1_BAUD_RATE,F_CPU)); // SIM800

	sei(); // globally enable interrupts

	uart0_puts("UART0 up");
	uart0_putc(0x0D); /* CR */
	uart0_putc(0x0A); /* LF */

	uart1_puts("UART1 up");
	uart1_putc(0x0D); /* CR */
	uart1_putc(0x0A); /* LF */


	/* Replace with your application code */
	while (1)
	{
		;
	}
}

ISR(TIMER5_COMPA_vect)
{
	gpio_toggle(gps_red_led_pin);
	//gpio_toggle(gps_green_led_pin);
}