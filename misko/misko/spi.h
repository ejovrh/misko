#ifndef SPI_H_
#define SPI_H_

#include <inttypes.h>

typedef struct																	// public part of struct describing the SPI bus
{
	uint8_t (*TransferByte)(const uint8_t in_addr);								// sends a byte into the circular buffer and gets something back
} spi_t;

spi_t *spi_ctor(void);															// object constructor

extern spi_t * const spi;														// declare pointer to public struct part

#endif /* SPI_H_ */