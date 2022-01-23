#include "ADXL345\ADXL345.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "spi.h"
#include "gpio.h"
#include "gpio_definitions.h"

#include "misko.h"

typedef struct																	// adxl345_t actual
{
	volatile uint8_t adxl345_irq_src;											// holds the interrupt source bitmask

	spi_t *_spi;																// tie in SPI object

	adxl345_t public;															// public struct
} __adxl345_t;

static __adxl345_t __ADXL345 __attribute__ ((section (".data")));				// preallocate __adxl345 object in .data

 static uint8_t _ReadByte(const uint8_t in_addr)								// reads one byte of data from address
{
	uint8_t retval;

	cli();
	gpio_clr(SPI_SS_ADXL345_pin);												// drive device CS low
	__ADXL345._spi->TransferByte(in_addr | 0x80);								// send address
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
	__ADXL345._spi = spi_ctor();												// tie in SPI

	__ADXL345.public.ReadByte = &_ReadByte;										// set function pointer
	__ADXL345.public.WriteByte = &_WriteByte;									//	ditto

	// accelerometer device initialization & configuration
	{
		_ReadByte(INT_SOURCE);													// clear any pending interrupts

		_WriteByte(INT_ENABLE, 0x00);
		_WriteByte(POWER_CTL, POWER_CTL_CFG_OFF);								// all off
		_WriteByte(DATA_FORMAT, DATA_FORMAT_CFG);								// 16g range, full resolution, active high int.
		_WriteByte(BW_RATE, BW_RATE_CFG);										// low power, output data rate 6.25Hz
		_WriteByte(THRESH_ACT, 0x4);											// 62.5mg/LSB
		_WriteByte(THRESH_INACT, 0x4);											// 62.5mg/LSB
		_WriteByte(TIME_INACT, 0x02);											// 1s/LSB
		_WriteByte(ACT_INACT_CTL, ACT_INACT_CTL_CFG);							// all axes in AC mode
		_WriteByte(INT_ENABLE, INT_ENABLE_CFG);									// enable interrupts only for act./inact. detection
		_WriteByte(INT_MAP, 0x00);												// route all to INT1

		_WriteByte(POWER_CTL, POWER_CTL_CFG);									// power on and start
	}

	return &__ADXL345.public;													// return address of public part; calling code accesses it via pointer
};


ISR(INT0_vect)																	// ISR for the ADXL345 accelerometer
{
	// wakeup event from ADXL345 inact. starts here
	if (misko->mcu->flag_sleep)													// if we were sleeping
	{
		SMCR &= ~_BV(SE);														// disable sleep
		misko->mcu->flag_sleep = 0;												// clear flag
		gpio_clr(gps_red_led_pin);												// indicate act. by turning red LED off
		__ADXL345.public.ReadByte(INT_SOURCE);									// clear any pending interrupts
		__ADXL345.public.WriteByte(INT_ENABLE, 0x00);							// disable interrupts
		__ADXL345.public.WriteByte(INT_ENABLE, INT_ENABLE_CFG);					// reconfigure for act./inact. detection
		__ADXL345.public.ReadByte(INT_SOURCE);									// enable interrupts
		sei();
		return;
	}

	cli();

	__ADXL345.adxl345_irq_src = _ReadByte(INT_SOURCE);							// read interrupt source & thereby clear pending interrupts

	if( (__ADXL345.adxl345_irq_src >> 3) & 0x01)								// if the inact. bit is set
		misko->mcu->flag_sleep = 1;												// flag for execution in main()

	if( (__ADXL345.adxl345_irq_src >> 4) & 0x01)								// if the act bit is set
		misko->mcu->flag_sleep = 0;												// clear flag

	sei();
};