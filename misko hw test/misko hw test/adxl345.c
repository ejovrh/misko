#include "adxl345.h"
#include "gpio.h"
#include "gpio_definitions.h"
#include "uart.h"
#include <avr/io.h>
#include <stdint-gcc.h>

static void spi_bus_on(void)
{
	/* ADXL345 specs:
	 *	- SCK <5MHz
	 *	- SPI mode 3
	 *	MSB first
	 */
	SPCR = ((1<<SPE)|     // SPI Enable
					(0<<SPIE)|    // SPI Interupt Enable
					(0<<DORD)|    // Data Order (0:MSB first / 1:LSB first)
					(1<<MSTR)|    // configure as master
					(0<<SPR1)|		// CLK shall be 4MHz
					(0<<SPR0)|    //
					(1<<CPOL)|    // SPI mode 3
					(1<<CPHA));   //

	SPSR = (1<<SPI2X);    // Double Clock Rate
};

static void spi_bus_off(void)
{
	SPCR = 0x00;
};

void adxl345_init(void)
{
	adxl345_write(DATA_FORMAT, DATA_FORMAT_CFG);
	adxl345_write(INT_MAP, INT_MAP_CFG);
	adxl345_write(INT_ENABLE, INT_ENABLE_CFG);
	adxl345_write(TIME_INACT, 0x01);
	adxl345_write(THRESH_INACT, 0x01);
	adxl345_write(THRESH_ACT, 0x01);
	adxl345_write(ACT_INACT_CTL, ACT_INACT_CTL_CFG);
	adxl345_write(POWER_CTL, POWER_CTL_CFG);
	adxl345_write(BW_RATE, BW_RATE_CFG);

	if (adxl345_read(DEVID) == 0xE5)
		uart0_puts("ADXL345 initialized\r\n");
};

uint8_t adxl345_read(const uint8_t addr)
{
	uint8_t retval;

	spi_bus_on();
	gpio_clr(SPI_SS_ADXL345_pin);

	SPDR = 0x80 | addr;
	while((SPSR & (1<<SPIF))==0);
	SPDR = 0x00;
	while((SPSR & (1<<SPIF))==0);

	retval = SPDR;

	gpio_set(SPI_SS_ADXL345_pin);
	spi_bus_off();

	return retval;
};

void adxl345_write(const uint8_t addr, const uint8_t val)
{
	spi_bus_on();

	gpio_clr(SPI_SS_ADXL345_pin);

	SPDR = addr;
	while((SPSR & (1<<SPIF))==0);
	SPDR = val;
	while((SPSR & (1<<SPIF))==0);

	gpio_set(SPI_SS_ADXL345_pin);

	spi_bus_off();

	if (adxl345_read(addr) != val)
		uart0_puts("write did not work\r\n");
};