#ifndef MISKO_H_
#define MISKO_H_

#include <inttypes.h>
#include "stm32l1xx_hal.h"

#include "ADXL345/ADXL345.h"	// accelerometer
#include "FM25W256/FM25W256.h"	// FeRAM
//#include "DS1394U/DS1394U.h"	// RTC

typedef struct  // public part of struct describing devices on misko
{
	uint8_t flag_print;  // flag for timer-based print control

	void (*StopMode)(const uint8_t in_flag);  // stop mode powersave function

	adxl345_t *adxl345;  // accelerometer object
	fm25w256_t *fm25w256;  // FeRAM object
//	ds1394_t *ds1394;	// RTC object
} misko_t;

misko_t* misko_ctor(void);  // declare constructor for concrete class

extern misko_t *misko;  // declare pointer to public struct part

#endif /* MISKO_H_ */
