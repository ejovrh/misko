#include "FM25W256.h"
#include "gpio.h"
#include "gpio_definitions.h"
#include "uart.h"
#include <avr/io.h>
#include <stdint-gcc.h>

static void spi_bus_on(void)
{
	/* ADXL345 specs:
	 *	- SCK <20MHz
	 *	- SPI mode 0 and 3
	 *	- MSB first
	 */

	PRR0 = (0<<PRSPI); // power on the SPI bus
	SPCR = ((1<<SPE)|     // SPI Enable
					(0<<SPIE)|    // SPI Interupt Enable
					(0<<DORD)|    // Data Order (0:MSB first / 1:LSB first)
					(1<<MSTR)|    // configure as master
					(0<<SPR1)|		// CLK shall be 4MHz
					(0<<SPR0)|    //
					(1<<CPOL)|    // SPI mode 3
					(1<<CPHA));   //

	SPSR = (0<<SPI2X);    // Double Clock Rate
};

static void spi_bus_off(void)
{
	SPCR = 0x00;	// set the SPI control register to all 0
	PRR0 = (1<<PRSPI); // power off the whole bus
};

// device initialization on power-on
void fm25w256_init(void)
{
	spi_bus_on(); // turn on the bus
	gpio_clr(SPI_SS_FRAM_pin); // select the slave

	// issue WREN opcode to generally enable writes on the device
	SPDR = CMD_WREN;	// put it into the register
	while((SPSR & (1<<SPIF))==0); // and let it be written to the buffer

	gpio_set(SPI_SS_FRAM_pin); // deselect the slave
	spi_bus_off(); // turn off the bus
};

// returns one byte read at addr
uint8_t fm25w256_read_byte(const uint16_t addr)
{
	uint8_t retval;

	if (addr >= 0x7fff) // check if we got the last valid address - datasheet p. 8
		return -1; // quit, return something, dont continue...

	uint8_t upper_byte = (addr >> 8) & 0xff; // first split 2 byte into 2 x 1byte
	uint8_t lower_byte = addr & 0xff;

	spi_bus_on(); // turn on the bus
	gpio_clr(SPI_SS_FRAM_pin); // select the slave

	SPDR = CMD_READ; // issue READ opcode to set device into read mode
	while((SPSR & (1<<SPIF))==0);

	SPDR = upper_byte; // ...put into register
	while((SPSR & (1<<SPIF))==0); // ...write it into the buffer
	SPDR = lower_byte; // ditto
	while((SPSR & (1<<SPIF))==0); // ditto

	// get the data from our adress by...
	SPDR = 0x00; // ...excanging some crap...
	while((SPSR & (1<<SPIF))==0); // ...for something useful.
	retval = SPDR;

	gpio_set(SPI_SS_FRAM_pin); // deselect the slave
	spi_bus_off(); // turn off the bus

	return retval;
};

// writes one byte (val) to addr
uint8_t fm25w256_write_byte(const uint16_t addr, const uint8_t val)
{
	if (addr >= 0x7fff) // check if we got the valid last address - datasheet p. 8
		return -1; // quit, return something, dont continue...

	uint8_t upper_byte = (addr >> 8) & 0xff; // first split 2 byte into 2 x 1byte
	uint8_t lower_byte = addr & 0xff;

	spi_bus_on(); // turn on the bus
	gpio_clr(SPI_SS_FRAM_pin); // select the slave

	// issue WRITE opcode to enable writing
	SPDR = CMD_WRITE;	// put it into the register
	while((SPSR & (1<<SPIF))==0); // and let it be written to the buffer

	// send out the addresses
	SPDR = upper_byte; // ...put into register
	while((SPSR & (1<<SPIF))==0); // ...write it into the buffer
	SPDR = lower_byte; // ditto
	while((SPSR & (1<<SPIF))==0); // ditto

	SPDR = val; // send out the actual data to be written into the fram
	while((SPSR & (1<<SPIF))==0);

	gpio_set(SPI_SS_FRAM_pin); // deselect the slave
	spi_bus_off(); // turn off the bus

	return 0; // error-free exit
};

uint8_t fm25w256_write_string(const uint16_t addr, char *in, const uint8_t len)
{
	uint8_t i = 0;

	if (addr + len >= 0x7fff) // check if we got the valid last address - datasheet p. 8
		return -1; // quit, return something, dont continue...

	uint8_t upper_byte = (addr >> 8) & 0xff; // first split 2 byte into 2 x 1byte
	uint8_t lower_byte = addr & 0xff;

	spi_bus_on(); // turn on the bus
	gpio_clr(SPI_SS_FRAM_pin); // select the slave

	// issue WRITE opcode to enable writing
	SPDR = CMD_WRITE;	// put it into the register
	while((SPSR & (1<<SPIF))==0); // and let it be written to the buffer

	// send out the addresses
	SPDR = upper_byte; // ...put into register
	while((SPSR & (1<<SPIF))==0); // ...write it into the buffer
	SPDR = lower_byte; // ditto
	while((SPSR & (1<<SPIF))==0); // ditto

	for(i=0; i<len; i++)
	{
		uart0_putc(*(in+i));
		uart0_puts("-write\r\n");
		SPDR = *(in+i); // send out the actual data to be written into the fram
		while((SPSR & (1<<SPIF))==0);
	}

	gpio_set(SPI_SS_FRAM_pin); // deselect the slave
	spi_bus_off(); // turn off the bus

	return 0;
};

uint8_t fm25w256_read_string(const uint16_t addr, char *out, const uint8_t len)
{
	uint8_t i=0;
	uint8_t retval;

	if (addr + len >= 0x7fff) // check if we got the last valid address - datasheet p. 8
		return -1; // quit, return something, dont continue...

	uint8_t upper_byte = (addr >> 8) & 0xff; // first split 2 byte into 2 x 1byte
	uint8_t lower_byte = addr & 0xff;

	spi_bus_on(); // turn on the bus
	gpio_clr(SPI_SS_FRAM_pin); // select the slave

	SPDR = CMD_READ; // issue READ opcode to set device into read mode
	while((SPSR & (1<<SPIF))==0);

	SPDR = upper_byte; // ...put into register
	while((SPSR & (1<<SPIF))==0); // ...write it into the buffer
	SPDR = lower_byte; // ditto
	while((SPSR & (1<<SPIF))==0); // ditto

	for(i=0; i<len; i++)
	{
		// get the data from our adress by...
		SPDR = 0x00; // ...excanging some crap...
		while((SPSR & (1<<SPIF))==0); // ...for something useful.
		//char f = SPDR; //FIXME - inside it "works", outside the returned values are not visible
		*(out+i) = SPDR;
		//*(out+i) = f;
		//uart0_putc(f);
		uart0_puts("-f\r\n");
		uart0_putc(*(out+i));
		uart0_puts("-out\r\n");
	}

	gpio_set(SPI_SS_FRAM_pin); // deselect the slave
	spi_bus_off(); // turn off the bus

	return retval;
};