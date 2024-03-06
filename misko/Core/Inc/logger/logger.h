#ifndef INC_LOGGER_LOGGER_H_
#define INC_LOGGER_LOGGER_H_

#include "main.h"

#if defined(USE_LOGGER)	// if this particular device is active

typedef struct logger_t  // struct describing the GPS module functionality
{
	uint8_t foo;	//
} logger_t;

logger_t* logger_ctor(void);  // the Logger constructor

extern logger_t *const Logger;  // declare pointer to public struct part

#endif

#endif /* INC_LOGGER_LOGGER_H_ */
