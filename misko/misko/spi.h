#ifndef SPI_H_
#define SPI_H_

#include <inttypes.h>

typedef struct																	// public part of struct describing devices on ADXL345
{
	uint8_t (*SPIReadByte)(const uint8_t in_addr);								// reads one byte of data from address
	void (*SPIWriteByte)(const uint8_t in_addr, const uint8_t in_data);			// writes one byte of data to address
} spi_t;

spi_t *spi_ctor(void);															// object constructor

extern spi_t * const spi;														// declare pointer to public struct part

#endif /* SPI_H_ */