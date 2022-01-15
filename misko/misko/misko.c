#include "misko.h"

#include "ADXL345/adxl345.h"
#include "FM25W256/FM25W256.h"
#include "SDCard/SDCard.h"
#include "DS1394U/DS1394U.h"
#include "EROLED0152W/EROLED0152W.h"
#include "RN4678/RN4678.h"
#include "ORG1510-MK4/ORG1510-MK4.h"
#include "LED/LED.h"

typedef struct																	// adxl345_t actual
{
	uint8_t __privatefoo;														// private var - foo

	adxl345_t *adxl345;

	misko_t public;																// public struct
} __misko_t;

static __misko_t __misko __attribute__ ((section (".data")));					// preallocate __misko_t object in .data

static void _barfunc(uint8_t in_val)											//
{
	__misko.__privatefoo = __misko.adxl345->TransferByte(0xFF);					//
};



void misko_ctor(void)
{
	__misko.adxl345 = adxl345_ctor();
	__misko.public.mcu = atmega_ctor();

	__misko.public.fooFunc = &_barfunc;

};

misko_t * const misko = &__misko.public;										// return address of public part; calling code accesses it via pointer