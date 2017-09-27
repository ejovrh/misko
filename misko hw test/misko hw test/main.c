#include <avr/io.h>
#include <avr/interrupt.h>

// local files
#include "gpio.h"

#include "gpio_definitions.h"


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
	sei(); // globally enable interrupts


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