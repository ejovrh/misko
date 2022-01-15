#include "DS1394U/DS1394U.h"

#include "spi.h"

typedef struct																	// ds1394_t actual
{
	//uint8_t __privatefoo;														// private var - foo

	ds1394_t public;															// public struct
} __ds1394_t;

static __ds1394_t __ds1394 __attribute__ ((section (".data")));					// preallocate __fm25w256 object in .data

static uint8_t _TransferByte(uint8_t in_val)									//
{
	SPITransferByte(0x02);														//

	return 0xFF;
};



ds1394_t *ds1394_ctor(void)														//
{
	__ds1394.public.TransferByte = &_TransferByte;								//

	return &__ds1394.public;													// return address of public part; calling code accesses it via pointer
};