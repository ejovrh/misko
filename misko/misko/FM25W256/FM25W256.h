#ifndef FM25W256_H_
#define FM25W256_H_

#include <inttypes.h>

typedef struct																	// public part of struct describing devices on ADXL345
{
	uint8_t	fooval;																//

	uint8_t (*TransferByte)(uint8_t in_val);									// foo

} fm25w256_t;

fm25w256_t *fm25w256_ctor(void);												// declare constructor for concrete class

extern fm25w256_t * const fm25w256;												// declare pointer to public struct part

#endif /* FM25W256_H_ */