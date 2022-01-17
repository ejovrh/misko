#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "ATMega2561/ATMega2561.h"

typedef struct																	// ATMega2561_t actual
{
	ATMega2561_t public;														// public struct
	//	uint8_t foo_private;													// private - some data member
} __ATMega2561_t;

static __ATMega2561_t __MCU __attribute__ ((section (".data")));				// preallocate __ATMega2561_t object in .data




ATMega2561_t *atmega_ctor(void)
{
	cli();																		// clear interrupts globally

	// peripherals config
	{
		PRR0 = 0xEF;															// all peripherals off
		PRR1 = 0x3F;															// ditto
		PRR0 &= ~_BV(PRUSART0);													// unset bit - USART0 power on
	}

	// SPI bus config
	{
		PRR0 &= ~_BV(PRSPI);													// unset bit - SPI bus power on

		SPCR = (
					(1<<SPE) |													// SPI Enable
					(1<<SPIE) |													// SPI Interrupt disable
					(0<<DORD) |													// Data Order (0:MSB first / 1:LSB first)
					(1<<MSTR) |													// configure as master
					(0<<SPR1) |													// CLK shall be Fosc/(2*2) = 4MHz
					(0<<SPR0) |													//
					(1<<CPOL) |													// SPI mode 3
					(1<<CPHA)													//
				);

		SPSR = (1<<SPI2X);														// Double Clock Rate
	}

	// interrupt config
	{
		EICRB |= (1 << ISC70);													// set to register
		EICRB |= (0 << ISC71);													//	...	rising edge
		EIMSK |= (1 << INT7);													// enable INT7 (lives on pin PE7)
	}

	sei();																		// enable interrupts globally

	return &__MCU.public;														// return address of public part; calling code accesses it via pointer
};