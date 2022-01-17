#include "ADXL345\ADXL345.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "spi.h"
#include "gpio.h"
#include "gpio_definitions.h"


typedef struct																	// adxl345_t actual
{
	volatile uint8_t adxl345_irq_src;											//

	spi_t *_spi;																// tie in SPI object

	adxl345_t public;															// public struct
} __adxl345_t;

static __adxl345_t __ADXL345 __attribute__ ((section (".data")));				// preallocate __adxl345 object in .data

 static uint8_t _ReadByte(const uint8_t in_addr)								// reads one byte of data from address
{
	uint8_t retval;

	cli();
	gpio_clr(SPI_SS_ADXL345_pin);												// drive device CS low
	__ADXL345._spi->TransferByte(in_addr | 0x80);								// send data
	retval = __ADXL345._spi->TransferByte(0x00);								// hopefully get something back
	gpio_set(SPI_SS_ADXL345_pin);												// drive device CS high
	sei();

	return retval;																// return data
};

static void _WriteByte(const uint8_t in_addr, const uint8_t in_data)			// writes one byte of data to address
{
	cli();
	gpio_clr(SPI_SS_ADXL345_pin);												// drive device CS low
	__ADXL345._spi->TransferByte(in_addr);										// write address
	__ADXL345._spi->TransferByte(in_data);										// write data
	gpio_set(SPI_SS_ADXL345_pin);												// drive device CS high
	sei();
};

adxl345_t *adxl345_ctor(void)													// object constructor
{
	__ADXL345._spi = spi_ctor();

	__ADXL345.public.ReadByte = &_ReadByte;										// set function pointer
	__ADXL345.public.WriteByte = &_WriteByte;									//	ditto

	// accelerometer device initialization & configuration
	{
		_WriteByte(POWER_CTL, POWER_CTL_CFG_OFF);								// all off
		_WriteByte(DATA_FORMAT, DATA_FORMAT_CFG);								// 16g range, full resolution, active high int.
		_WriteByte(BW_RATE, BW_RATE_CFG);										// low power, output data rate 6.25Hz
		_WriteByte(THRESH_ACT, 0x4);											// 62.5mg/LSB
		_WriteByte(THRESH_INACT, 0x4);											// 62.5mg/LSB
		_WriteByte(TIME_INACT, 0x01);											// 1s/LSB
		_WriteByte(ACT_INACT_CTL, ACT_INACT_CTL_CFG);							// all axes in AC mode
		_WriteByte(INT_ENABLE, INT_ENABLE_CFG);									// enable interrupts only for act./inact. detection
		_WriteByte(INT_MAP, 0x00);												// route all to INT1

		_WriteByte(POWER_CTL, POWER_CTL_CFG);									// power on and start
	}

	return &__ADXL345.public;													// return address of public part; calling code accesses it via pointer
};

// ISR for the ADXL345 accelerometer
ISR(INT7_vect)
{
	cli();

	__ADXL345.adxl345_irq_src = _ReadByte(INT_SOURCE);							// clear pending interrupts

	// inactivity
	if( (__ADXL345.adxl345_irq_src >> 3) & 0x01)								// if the inact. bit is set
	{
		__ADXL345.adxl345_irq_src = _ReadByte(INT_SOURCE);						// clear pending interrupts
		gpio_set(gps_red_led_pin);												// indicate by turning red LED off
	}

	// activity
	if( (__ADXL345.adxl345_irq_src >> 4) & 0x01)								// if the act bit is set
	{
		__ADXL345.adxl345_irq_src = _ReadByte(INT_SOURCE);						// clear pending interrupts
		gpio_clr(gps_red_led_pin);												// indicate by turning red LED on
	}

	sei();
};