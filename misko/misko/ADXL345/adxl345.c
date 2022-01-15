#include "ADXL345\ADXL345.h"

#include "spi.h"

typedef struct																	// adxl345_t actual
{
	uint8_t __privatefoo;														// private var - foo

	adxl345_t public;															// public struct
} __adxl345_t;

static __adxl345_t __adxl345 __attribute__ ((section (".data")));				// preallocate __adxl345 object in .data

static uint8_t _TransferByte(uint8_t in_val)									//
{
	SPITransferByte(0x02);														//

	return 0xFF;
};



adxl345_t *adxl345_ctor(void)													//
{
	__adxl345.public.TransferByte = &_TransferByte;								//

	return &__adxl345.public;													// return address of public part; calling code accesses it via pointer
};