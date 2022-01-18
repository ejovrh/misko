#include "SDCard/SDCard.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "gpio.h"
#include "gpio_definitions.h"

typedef struct																	// sd_t actual
{
	spi_t *_spi;																// tie in SPI object

	sd_t public;																// public struct
} __sd_t;

static __sd_t __SD __attribute__ ((section (".data")));							// preallocate __sd_t object in .data

static void _Read(void)	//
{
	;
};

static void _Write(void)	//
{
	;
};



sd_t *sd_ctor(void)																// object constructor
{
	__SD._spi = spi_ctor();														// tie in SPI

	__SD.public.Read = &_Read;													// set function pointer
	__SD.public.Write = &_Write;												//	ditto

	return &__SD.public;														// return address of public part; calling code accesses it via pointer
};