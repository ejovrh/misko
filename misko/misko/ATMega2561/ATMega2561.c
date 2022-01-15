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

	sei();																		// enable interrupts globally

	return &__MCU.public;														// return address of public part; calling code accesses it via pointer
};