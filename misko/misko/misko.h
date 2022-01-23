#ifndef MISKO_H_
#define MISKO_H_

#include <inttypes.h>

#include "gpio.h"
#include "gpio_definitions.h"

#include "ATMega2561/ATMega2561.h"												// core MCU
#include "FM25W256/FM25W256.h"													// FeRAM
#include "ADXL345/ADXL345.h"													// accelerometer
#include "DS1394U/DS1394U.h"													// RTC

typedef struct																	// public part of struct describing devices on misko
{
	uint8_t flag_print;															// flag for timer-based print control

	ATMega2561_t *mcu;															// MCU object
	fm25w256_t *fm25w256;														// FeRAM object
	adxl345_t *adxl345;															// accelerometer object
	ds1394_t *ds1394;															// RTC object
} misko_t;

void misko_ctor(void);															// declare constructor for concrete class

extern misko_t * const misko;													// declare pointer to public struct part

#endif /* MISKO_H_ */