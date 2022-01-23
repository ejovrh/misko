#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "ATMega2561/ATMega2561.h"
#include "misko.h"

#include "gpio.h"
#include "gpio_definitions.h"

typedef struct																	// ATMega2561_t actual
{
	ATMega2561_t public;														// public struct
} __ATMega2561_t;

static __ATMega2561_t __MCU __attribute__ ((section (".data")));				// preallocate __ATMega2561_t object in .data



ATMega2561_t *atmega_ctor(void)
{
	cli();																		// clear interrupts globally

	// MCU peripherals power supply on/off
	{
		PRR0 = 0xFF;															// all peripherals off
		PRR1 = 0xFF;															// ditto

		PRR0 &= ~_BV(PRUSART0);													// unset bit - USART0 power on
		PRR1 &= ~_BV(PRTIM5);													// unset bit - timer5 power on
		PRR1 &= ~_BV(PRTIM4);													// unset bit - timer4 power on
		PRR1 &= ~_BV(PRTIM3);													// unset bit - timer3 power on
	}

	// SPI bus config
	{
		PRR0 &= ~_BV(PRSPI);													// unset bit - SPI bus power on

		SPCR = (
					_BV(SPE) |	_BV(MSTR) |										// SPI Enable, master, MSB first, fosc./4
					_BV(CPOL) |	_BV(CPHA)										// SPI mode 3
			   );

		SPSR = (1<<SPI2X);														// Double Clock Rate -> 4MHz
	}

	// interrupt config
	{
		EICRA = (
					_BV(ISC01) | _BV(ISC00) |									// ADXL345 interrupt line - register rising edge
					_BV(ISC11) | _BV(ISC10)										// RTC interrupt line - register rising edge
				);

		EIMSK = (
					_BV(INT1) |													// enable INT6 - RTC interrupts
					_BV(INT0)													// enable INT7 - ADXL345 act./inact. interrupts
				);
	}

	// timer config
	{
		// timer5
		{
/*			OCR5A = 7800;														// 500ms
 			OCR5A = 3900;														// 500ms
			OCR5A = 1950;														// 250ms
			OCR5A = 975;														// 125ms
			OCR5A = 488;														// 62.5ms
			OCR5A = 242;														// 31.2ms
			OCR5A = 117;														// 15ms (invisible blinking)
*/

			OCR5A = 3900;														// 500ms
			TCCR5B = (
						_BV(WGM52) |											// CTC mode
						_BV(CS52) | _BV(CS50)									// set prescaler to 1024
					 );

			TIMSK5 |= _BV(OCIE5A);												// enable timer compare interrupt
		}

		// timer 4
		{
			OCR4A = 975;														// see timer5

			TCCR4B = (
						_BV(WGM42) |											// CTC mode
						_BV(CS42) | _BV(CS40)									// set prescaler to 1024
					 );

			TIMSK4 |= _BV(OCIE4A);												// enable timer compare interrupt
		}

		// timer 3
		{
			OCR3A = 7800;														// see timer5

			TCCR3B = (
						_BV(WGM32) |											// CTC mode
						_BV(CS32) | _BV(CS30)									// set prescaler to 1024
					);

			TIMSK3 |= _BV(OCIE3A);												// enable timer compare interrupt
		}
	}

	sei();																		// enable interrupts globally

	return &__MCU.public;														// return address of public part; calling code accesses it via pointer
};


ISR(TIMER5_COMPA_vect)															// timer5 ISR - 500ms periodic
{
	gpio_toggle(gps_green_led_pin);												// FooIndicator
};

ISR(TIMER4_COMPA_vect)															// timer4 ISR - 125ms periodic
{
// 	gpio_toggle(gps_green_led_pin);												// FooIndicator
};

ISR(TIMER3_COMPA_vect)															// timer3 ISR - 1s periodic
{
	misko->flag_print = 1;
	++misko->mcu->foocounter;
// 	gpio_toggle(gps_green_led_pin);												// FooIndicator
};