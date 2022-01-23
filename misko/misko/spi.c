#include "spi.h"

#include <avr/io.h>

typedef struct																	// spi_t actual
{
	// TODO - write semaphore code
	uint8_t __semaphore;														// private var - semaphore for critical section

	spi_t public;																// public struct
} __spi_t;

extern __spi_t __SPI __attribute__ ((section (".data")));						// preallocate __spi object in .data

static inline uint8_t _TransferByte(const uint8_t in_val)						// sends a byte into the circular buffer and gets something back
{
	SPDR = in_val;																// first, initiate the transfer by writing data into the data register
																				//	then
	while( !(SPSR & _BV(SPIF)) )												// wait for the transfer to complete & reset the SPI interrupt by reading (DS. p. 198)
		;
	return SPDR;																// read shift register receive buffer & return data
};

__spi_t __SPI =																	// instantiate __spi_t object & set values
{
	.public.TransferByte = &_TransferByte										// set function pointer
};

spi_t *spi_ctor(void)															// object constructor
{
// TODO - write SPI mode code
	__SPI.__semaphore = 0;														// semaphore for critical sections

	__SPI.public.TransferByte = &_TransferByte;									// set function pointer


	return &__SPI.public;														// return address of public part; calling code accesses it via pointer
};