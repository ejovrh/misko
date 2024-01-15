#ifndef MISKO_H_
#define MISKO_H_

#include <inttypes.h>
#include "stm32l1xx_hal.h"

#include "ADXL345/ADXL345.h"	// accelerometer
#include "FM25W256/FM25W256.h"	// FeRAM
#include "ORG1510/ORG1510.h"// GPS receiver

#define STOPMODE 0 // use MCU power save mode or not

typedef struct  // public part of struct describing devices on misko
{
	volatile uint8_t FlagPrint;  // flag for timer-based print control
	volatile uint8_t free1;
	volatile uint8_t free2;
	volatile uint8_t free3;

	void (*StopMode)(const uint8_t in_flag);  // stop mode powersave function

	adxl345_t *adxl345;  // accelerometer object
	fm25w256_t *fm25w256;  // FeRAM object
	org1510_t *org1510;  // GPS receiver object
} misko_t;

misko_t* misko_ctor(void);  // declare constructor for concrete class

extern misko_t *misko;  // declare pointer to public struct part

#endif /* MISKO_H_ */
