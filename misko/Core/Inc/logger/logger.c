#include "logger/logger.h"

#if defined(USE_LOGGER)	// if this particular device is active

typedef struct	// loggerc_t actual
{
	logger_t public;  // public struct
} __logger_t;

static __logger_t __Logger __attribute__ ((section (".data")));  // preallocate __Logger object in .data

logger_t* logger_ctor(void)  //
{
	return &__Logger.public;  // set pointer to Logger public part
}

logger_t *const Logger = &__Logger.public;  // set pointer to Logger public part

#endif
