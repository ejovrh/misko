#ifndef MISKO_H_
#define MISKO_H_

#include <inttypes.h>

#include "ATMega2561/ATMega2561.h"

typedef struct																	// public part of struct describing devices on misko
{
	ATMega2561_t *mcu;															//
	uint8_t	fooval;																//

	void (* fooFunc)(uint8_t in_val);											// foo

} misko_t;

void misko_ctor(void);															// declare constructor for concrete class

extern misko_t * const misko;													// declare pointer to public struct part

#endif /* MISKO_H_ */