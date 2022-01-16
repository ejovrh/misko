#ifndef MISKO_H_
#define MISKO_H_

#include <inttypes.h>

#include "gpio.h"
#include "gpio_definitions.h"

#include "ATMega2561/ATMega2561.h"
#include "ADXL345/ADXL345.h"

typedef struct																	// public part of struct describing devices on misko
{
	ATMega2561_t *mcu;															//
	adxl345_t *adxl345;															//

	uint8_t	fooval;																//
} misko_t;

void misko_ctor(void);															// declare constructor for concrete class

extern misko_t * const misko;													// declare pointer to public struct part

#endif /* MISKO_H_ */