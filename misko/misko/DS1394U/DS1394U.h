#ifndef DS1394U_H_
#define DS1394U_H_

#include <inttypes.h>

typedef struct																	// public part of struct describing devices on ADXL345
{
	uint8_t	fooval;																//

	uint8_t (*TransferByte)(uint8_t in_val);									// foo

} ds1394_t;

ds1394_t *ds1394_ctor(void);													// declare constructor for concrete class

extern ds1394_t * const ds1394;													// declare pointer to public struct part

#endif /* DS1394U_H_ */