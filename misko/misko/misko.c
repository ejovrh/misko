#include <avr/io.h>

#include "misko.h"


#include "SDCard/SDCard.h"
#include "DS1394U/DS1394U.h"
#include "EROLED0152W/EROLED0152W.h"
#include "RN4678/RN4678.h"
#include "ORG1510-MK4/ORG1510-MK4.h"
#include "LED/LED.h"

typedef struct																	// adxl345_t actual
{
	misko_t public;																// public struct
} __misko_t;

static __misko_t __misko __attribute__ ((section (".data")));					// preallocate __misko_t object in .data



void misko_ctor(void)
{
	#include "gpio_modes.h"

	__misko.public.mcu = atmega_ctor();											//
	__misko.public.adxl345 = adxl345_ctor();									//
	__misko.public.fm25w256 = fm25w256_ctor();									//
};

misko_t * const misko = &__misko.public;										// return address of public part; calling code accesses it via pointer