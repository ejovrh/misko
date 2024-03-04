#ifndef INC_TIMEHELPER_TIMEHELPER_H_
#define INC_TIMEHELPER_TIMEHELPER_H_

#include "org1510mk4/org1510mk4.h"

#if defined(USE_TIMEHELPER)	// if this particular feature is active

typedef struct timehelper_t  // struct describing the GPS module functionality
{
	uint8_t flagRTCcorrect :1;  // flag indicating that the RTC has the correct time
	uint8_t flagGPScorrect :1;  // flag indicate that the GPS module has the correct time

	void (*TimeHelper)(const uint8_t GPS_flag);  // the one actor
	void (*LocationHelper)(const uint8_t force);  // POC code - seeds the current location
} timehelper_t;

timehelper_t* timehelper_ctor(org1510mk4_t *gps, RTC_HandleTypeDef *rtc);  // the timehelper constructor

extern timehelper_t *const TimeHelper;  // declare pointer to public struct part

#endif

#endif /* INC_TIMEHELPER_TIMEHELPER_H_ */
