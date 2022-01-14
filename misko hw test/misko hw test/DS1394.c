#include "DS1394.h"
#include "gpio.h"
#include "gpio_definitions.h"
#include "uart.h"
#include <avr/io.h>
#include <stdint-gcc.h>

static unsigned char binTobcd (unsigned char binValue)
{
	return ( ((binValue / 10) << 4) | (binValue % 10) );
}

static unsigned char bcdTobin (unsigned char bcdValue)
{
	return ( (((bcdValue >> 4) & 0x0F) * 10) + (bcdValue & 0x0F) );
}

static void spi_bus_on(void)
{
	/* DS1394-U33+ specs:
	 *	- SCK <4MHz
	 *	- SPI mode 0 or 2
	 *	- MSB first
	 */

 	PRR0 = (0<<PRSPI); // power on the SPI bus
	SPCR = ((1<<SPE)|     // SPI Enable
					(0<<SPIE)|    // SPI Interupt Enable
					(0<<DORD)|    // Data Order (0:MSB first / 1:LSB first)
					(1<<MSTR)|    // configure as master
					(0<<SPR1)|		// CLK shall be 4MHz
					(0<<SPR0)|    //
					(0<<CPOL)|    // SPI mode 0
					(0<<CPHA));   //

	SPSR = (1<<SPI2X);    // Double Clock Rate
};

static void spi_bus_off(void)
{
	SPCR = 0x00;	// set the SPI control register to all 0
	PRR0 = (1<<PRSPI); // power off the whole bus};
}

void ds1394_init(void)
{
	ds1394_write(hours_write, ds1394_read(hours_read) & ~0x40);	// set 24h mode for clock
	ds1394_write(alarm_hours_write, ds1394_read(hours_read) & ~0x40);	// set 24h mode for alarm
	ds1394_write(control_write, intcn);
}

uint8_t ds1394_read(const uint8_t addr)
{
	uint8_t retval;

	if (addr >= 0xf) // check if we got the last address - datasheet p. 8
		return -1; // quit, return something, dont continue...

	spi_bus_on(); // turn on the bus
	gpio_clr(SPI_SS_RTC_pin); // select the slave

	SPDR = addr; // ...put into register
	while((SPSR & (1<<SPIF))==0); // ...write it into the buffer

		// get the data from our adress by...
	SPDR = 0x00; // ...excanging some crap...
	while((SPSR & (1<<SPIF))==0); // ...for something useful.

	retval = SPDR; // save for later...

	gpio_set(SPI_SS_RTC_pin); // deselect the slave
	spi_bus_off(); // turn off the bus

	return retval;
};

uint8_t ds1394_write(const uint8_t addr, const uint8_t val)
{
	if (addr >= 0xf) // check if we got the last address - datasheet p. 8
		return -1; // quit, return something, dont continue...

	spi_bus_on(); // turn on the bus
	gpio_clr(SPI_SS_RTC_pin); // select the slave

	// send out the addresses
	SPDR = (0x80 | addr); // ...put into register
	while((SPSR & (1<<SPIF))==0); // ...write it into the buffer

	SPDR = val; // send out the actual data to be written into the fram
	while((SPSR & (1<<SPIF))==0);

	gpio_set(SPI_SS_RTC_pin); // deselect the slave
	spi_bus_off(); // turn off the bus

	return 0; // error-free exit
};

void ds1394_set_date(unsigned char day, unsigned char day_date, unsigned char month, unsigned char year)
{
	ds1394_write(DS1394_REG_DAY, binTobcd(day));
	ds1394_write(DS1394_REG_DATE, binTobcd(day_date));
	ds1394_write(DS1394_REG_YEAR, binTobcd(year));
	ds1394_write(DS1394_REG_MONTH_CENT, binTobcd(month));
};

void ds1394_set_time(unsigned char hours, unsigned char minutes, unsigned char seconds)
{
	ds1394_write(DS1394_REG_HOURS, binTobcd(hours));
	ds1394_write(DS1394_REG_MINUTES, binTobcd(minutes));
	ds1394_write(DS1394_REG_SECONDS, binTobcd(seconds));
};