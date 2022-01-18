#ifndef SDCARD_H_
#define SDCARD_H_

#include <inttypes.h>

typedef struct																	// public part of struct describing the SD card
{
	void (*Read)(void);	//
	void (*Write)(void);	//
} sd_t;

sd_t *sd_ctor(void);															// declare constructor for concrete class

extern sd_t * const sd;															// declare pointer to public struct part

#endif /* SDCARD_H_ */