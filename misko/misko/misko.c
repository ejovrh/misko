#include <avr/io.h>
#include <avr/interrupt.h>

#include "misko.h"

// #include "SDCard/SDCard.h"
// #include "EROLED0152W/EROLED0152W.h"
// #include "RN4678/RN4678.h"
// #include "ORG1510-MK4/ORG1510-MK4.h"
// #include "LED/LED.h"

typedef struct																	// adxl345_t actual
{
	misko_t public;																// public struct
} __misko_t;

static __misko_t __misko __attribute__ ((section (".data")));					// preallocate __misko_t object in .data



void misko_ctor(void)															// object constructor
{
	sei();
	#include "gpio_modes.h"														// set pin states
	cli();

	__misko.public.mcu = atmega_ctor();											// tie in MCU object
	__misko.public.adxl345 = adxl345_ctor();									// tie on accelerometer object
	__misko.public.fm25w256 = fm25w256_ctor();									// tie in FeRAM object
	__misko.public.ds1394 = ds1394_ctor();										// tie in RTC object
};

misko_t * const misko = &__misko.public;										// return address of public part; calling code accesses it via pointer