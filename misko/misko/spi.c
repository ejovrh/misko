#include "spi.h"

#include <avr/io.h>
#include <avr/interrupt.h>

typedef struct																	// fm25w256_t actual
{
	// TODO - write semaphore code
	uint8_t __semaphore;														// private var - semaphore for critical section

	spi_t public;																// public struct
} __spi_t;

extern __spi_t __SPI __attribute__ ((section (".data")));						// preallocate __fm25w256 object in .data

/*
static inline void _SPIBusOn(void)												// SPI init - MSB first, mode 3, 4MHz
{
	SPCR = (
				(1<<SPE) |														// SPI Enable
				(0<<SPIE) |														// SPI Interrupt disable
				(0<<DORD) |														// Data Order (0:MSB first / 1:LSB first)
				(1<<MSTR) |														// configure as master
				(0<<SPR1) |														// CLK shall be Fosc/(2*2) = 4MHz
				(0<<SPR0) |														//
				(1<<CPOL) |														// SPI mode 3
				(1<<CPHA)														//
			);

	SPSR = (1<<SPI2X);															// Double Clock Rate

	PRR0 &= ~_BV(PRSPI);														// unset bit - SPI bus power on
};

static inline void _SPIBusOff(void)												// SPI reset & power off
{
	PRR0 |= _BV(PRSPI);															// set bit - SPI bus power off
	SPCR = 0x00;																// set the SPI control register to all 0
};
*/

static inline uint8_t __SPITransferByte(const uint8_t in_val)					//
{
	SPDR = in_val;																// first, initiate the transfer by writing data into the data register
																				//	then
	while( !(SPSR & _BV(SPIF)) )												// wait for the transfer to complete & reset the SPI interrupt by reading (DS. p. 198)
		;

	return SPDR;																// read shift register receive buffer & return data
};

static inline uint8_t _ReadByte(const uint8_t in_addr)							// reads one byte of data from address
{
	uint8_t retval;

	cli();																		// atomic section start
	__SPITransferByte(in_addr);													// pass address to device
	retval = __SPITransferByte(0x00);											// write something to device and retrieve requested address contents
	sei();																		// atomic section end

	return retval;
};

static inline void _WriteByte(const uint8_t in_addr, const uint8_t in_data)		// writes one byte of data to address
{
	cli();																		// atomic section start
	__SPITransferByte(in_addr);													// pass address to device
	__SPITransferByte(in_data);													// further on, pass data to device
	sei();																		// atomic section end
};

__spi_t __SPI =																	// instantiate __spi_t object & set values
{
	.public.SPIReadByte = &_ReadByte,
	.public.SPIWriteByte = &_WriteByte
};

spi_t *spi_ctor(void)															// object constructor
{
	__SPI.__semaphore = 0;

	__SPI.public.SPIReadByte = &_ReadByte;
	__SPI.public.SPIWriteByte = &_WriteByte;

	return &__SPI.public;														// return address of public part; calling code accesses it via pointer
};