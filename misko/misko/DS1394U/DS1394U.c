#include "DS1394U/DS1394U.h"
// FIXME - untested code !!
#include <avr/io.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "gpio.h"
#include "gpio_definitions.h"

typedef struct																	// ds1394_t actual
{
	spi_t *_spi;																// tie in SPI object

	ds1394_t public;															// public struct
} __ds1394_t;

static __ds1394_t __DS1394 __attribute__ ((section (".data")));					// preallocate __fm25w256 object in .data

static inline unsigned char __BINToBCD (unsigned char binValue)					//
{
	return ( ((binValue / 10) << 4) | (binValue % 10) );						//
}

static inline unsigned char __BCDToBIN (unsigned char bcdValue)					//
{
	return ( (((bcdValue >> 4) & 0x0F) * 10) + (bcdValue & 0x0F) );				//
}

static uint8_t _Read(const uint8_t in_addr)										// reads one byte from address
{
	uint8_t retval;

	if (in_addr >= 0xF)															// check if we got the last address - data sheet p. 8
		return -1;																// quit, return something, don't continue...

	cli();
	gpio_clr(SPI_SS_RTC_pin);													// drive device CS low
	__DS1394._spi->TransferByte(in_addr);										// send address
	retval = __DS1394._spi->TransferByte(0x00);									// hopefully get something back
	gpio_set(SPI_SS_RTC_pin);													// drive device CS high
	sei();

	return retval;
};

static uint8_t _Write(const uint8_t in_addr, const uint8_t in_val)				// writes one byte to address
{
	if (in_addr >= 0xF)															// check if we got the last address - data sheet p. 8
		return -1;																// quit, return something, don't continue...

	cli();
	gpio_clr(SPI_SS_RTC_pin);													// drive device CS low
	__DS1394._spi->TransferByte(in_addr | 0x80);								// send address & mark as write command
	__DS1394._spi->TransferByte(in_val);										// send data
	gpio_set(SPI_SS_RTC_pin);													// drive device CS high
	sei();

	return 0;																	// error-free exit
};

void _SetDate(const unsigned char day, const unsigned char day_date, const unsigned char month, const unsigned char year)
{
	_Write(DS1394_REG_DAY, __BINToBCD(day));
	_Write(DS1394_REG_DATE, __BINToBCD(day_date));
	_Write(DS1394_REG_YEAR, __BINToBCD(year));
	_Write(DS1394_REG_MONTH_CENT, __BINToBCD(month));
};

void _SetTime(const unsigned char hours, const unsigned char minutes, const unsigned char seconds)
{
	_Write(DS1394_REG_HOURS, __BINToBCD(hours));
	_Write(DS1394_REG_MINUTES, __BINToBCD(minutes));
	_Write(DS1394_REG_SECONDS, __BINToBCD(seconds));
};



ds1394_t *ds1394_ctor(void)														//
{
	__DS1394._spi = spi_ctor();													// tie in SPI

	__DS1394.public.Read = &_Read;												// set function pointer
	__DS1394.public.Write = &_Write;											//	ditto
	__DS1394.public.SetDate = &_SetDate;										//	ditto
	__DS1394.public.SetTime = &_SetTime;										//	ditto

	// accelerometer device initialization & configuration
	{
		_Write(hours_write, _Read(hours_read) & ~0x40);							// set 24h mode for clock
		_Write(alarm_hours_write, _Read(hours_read) & ~0x40);					// set 24h mode for alarm
		_Write(control_write, intcn);											//
	}

	return &__DS1394.public;													// return address of public part; calling code accesses it via pointer
};