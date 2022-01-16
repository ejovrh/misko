#include "FM25W256/FM25W256.h"

#include "spi.h"

typedef struct																	// fm25w256_t actual
{
	//uint8_t __privatefoo;														// private var - foo

	fm25w256_t public;															// public struct
} __fm25w256_t;

static __fm25w256_t __fm25w256 __attribute__ ((section (".data")));				// preallocate __fm25w256 object in .data

//static uint8_t _TransferByte(uint8_t in_val)									//
//{
	//SPITransferByte(0x02);														//
//
	//return 0xFF;
//};



fm25w256_t *fm25w256_ctor(void)													//
{
	//__fm25w256.public.TransferByte = &_TransferByte;							//

	return &__fm25w256.public;													// return address of public part; calling code accesses it via pointer
};