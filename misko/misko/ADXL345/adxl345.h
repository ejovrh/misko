#ifndef ADXL345_H_
#define ADXL345_H_

#include <inttypes.h>

typedef struct																	// public part of struct describing devices on ADXL345
{
	uint8_t	fooval;																//

	uint8_t (*TransferByte)(uint8_t in_val);									// foo

} adxl345_t;

adxl345_t *adxl345_ctor(void);													// declare constructor for concrete class

extern adxl345_t * const adxl345;												// declare pointer to public struct part

#endif /* ADXL345_H_ */