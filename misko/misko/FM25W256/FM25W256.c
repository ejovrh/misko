#include "FM25W256/FM25W256.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "gpio.h"
#include "gpio_definitions.h"

typedef struct																	// fm25w256_t actual
{
	spi_t *_spi;																// tie in SPI object

	fm25w256_t public;															// public struct
} __fm25w256_t;

static __fm25w256_t __FM25W256 __attribute__ ((section (".data")));				// preallocate __fm25w256 object in .data

// TODO - reduce functions to _ReadString() and _WriteString() only
static uint8_t _ReadByte(const uint16_t in_addr)								// reads one byte of data from address
{
	uint8_t retval;

	if (in_addr >= 0x7FFF)														// check if we got the last valid address - data sheet p. 8
		return -1;																// quit, return something, don't continue...

	uint8_t upper_byte = (in_addr >> 8) & 0xFF;									// first split 2 byte into 2 x 1byte
	uint8_t lower_byte = in_addr & 0xFF;

	cli();
	gpio_clr(SPI_SS_FRAM_pin);													// drive device CS low
	__FM25W256._spi->TransferByte(CMD_READ);									// send read command
	__FM25W256._spi->TransferByte(upper_byte);									// send upper address byte
	__FM25W256._spi->TransferByte(lower_byte);									// send lower address byte
	retval = __FM25W256._spi->TransferByte(0xFF);								// read data back
	gpio_set(SPI_SS_FRAM_pin);													// drive device CS high
	sei();

	return retval;																// return data
};

static uint8_t _WriteByte(const uint16_t in_addr, const uint8_t in_data)		// writes one byte of data to address
{
	if (in_addr >= 0x7FFF)														// check if we got the last valid address - data sheet p. 8
		return -1;																// quit, return something, don't continue...

	uint8_t upper_byte = (in_addr >> 8) & 0xFF;									// first split 2 byte into 2 x 1byte
	uint8_t lower_byte = in_addr & 0xFF;

	cli();
	gpio_clr(SPI_SS_FRAM_pin);													// drive device CS low
	__FM25W256._spi->TransferByte(CMD_WREN);									// issue WREN opcode to enable writes on the device
	gpio_set(SPI_SS_FRAM_pin);													// drive device CS high
	gpio_clr(SPI_SS_FRAM_pin);													// drive device CS low
	__FM25W256._spi->TransferByte(CMD_WRITE);									// send write command
	__FM25W256._spi->TransferByte(upper_byte);									// send upper address byte
	__FM25W256._spi->TransferByte(lower_byte);									// send lower address byte
	__FM25W256._spi->TransferByte(in_data);										// send data
	gpio_set(SPI_SS_FRAM_pin);													// drive device CS high
	sei();

	return 0;
};

static uint8_t _ReadString(const uint16_t in_addr, char *out, const uint8_t in_len)		// reads a length of string from from address
{
	uint8_t i = 0;

	if (in_addr >= 0x7FFF)														// check if we got the last valid address - data sheet p. 8
		return -1;																// quit, return something, don't continue...

	uint8_t upper_byte = (in_addr >> 8) & 0xFF;									// first split 2 byte into 2 x 1byte
	uint8_t lower_byte = in_addr & 0xFF;

	cli();
	gpio_clr(SPI_SS_FRAM_pin);													// drive device CS low
	__FM25W256._spi->TransferByte(CMD_READ);									// send read command
	__FM25W256._spi->TransferByte(upper_byte);									// send upper address byte
	__FM25W256._spi->TransferByte(lower_byte);									// send lower address byte

	for(i=0; i<in_len; ++i)
		*(out+i) = __FM25W256._spi->TransferByte(0xFF);							// get data

	gpio_set(SPI_SS_FRAM_pin);													// drive device CS high
	sei();

	return 0;
};

static uint8_t _WriteString(const uint16_t in_addr, char *in, const uint8_t in_len)		// writes a length of string from from address
{
	uint8_t i = 0;

	if (in_addr >= 0x7FFF)														// check if we got the last valid address - data sheet p. 8
		return -1;																// quit, return something, don't continue...

	uint8_t upper_byte = (in_addr >> 8) & 0xFF;									// first split 2 byte into 2 x 1byte
	uint8_t lower_byte = in_addr & 0xFF;

	cli();
	gpio_clr(SPI_SS_FRAM_pin);													// drive device CS low
	__FM25W256._spi->TransferByte(CMD_WREN);									// issue WREN opcode to enable writes on the device
	gpio_set(SPI_SS_FRAM_pin);													// drive device CS high
	gpio_clr(SPI_SS_FRAM_pin);													// drive device CS low
	__FM25W256._spi->TransferByte(CMD_WRITE);									// send read command
	__FM25W256._spi->TransferByte(upper_byte);									// send upper address byte
	__FM25W256._spi->TransferByte(lower_byte);									// send lower address byte

	for(i=0; i<in_len; ++i)
		__FM25W256._spi->TransferByte(*(in+i));									// write data

	gpio_set(SPI_SS_FRAM_pin);													// drive device CS high
	sei();

	return 0;
};



fm25w256_t *fm25w256_ctor(void)													// object constructor
{
	__FM25W256._spi = spi_ctor();												// tie in SPI

	__FM25W256.public.ReadByte = &_ReadByte;									// set function pointer
	__FM25W256.public.WriteByte = &_WriteByte;									//	ditto
	__FM25W256.public.ReadString = &_ReadString;								//	ditto
	__FM25W256.public.WriteString = &_WriteString;								//	ditto

	return &__FM25W256.public;													// return address of public part; calling code accesses it via pointer
};