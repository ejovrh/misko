#include "timehelper/timehelper.h"

#if defined(USE_TIMEHELPER)	// if this particular feature is active

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct	// timehelper_t actual
{
	org1510mk4_t *gps;	// placeholder for GPS object
	RTC_HandleTypeDef *rtc;  // placeholder for RTC object

	timehelper_t public;  // public struct
} __timehelper_t;

static __timehelper_t __TimeHelper __attribute__ ((section (".data")));  // preallocate __TimeHelper object in .data

static RTC_TimeTypeDef sTime =
	{0};
static RTC_DateTypeDef sDate =
	{0};

// sets RTC time based on GPS module time
static void _SetRTCfromGPS(org1510mk4_t *gps, RTC_HandleTypeDef *rtc)
{
	sDate.Year = (uint8_t) (gps->zda->year - 2000);  // the year is only the 2 last digits!
	sDate.Month = gps->zda->month;
	sDate.Date = gps->zda->day;

	if(HAL_RTC_SetDate(rtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}

	char temp[3] = "\0";  // will contain e.g. 154726
	char zda_time[7] = "\0";	// will contain a copy of ZDA's time
	memcpy(&zda_time, gps->zda->time, 7);  // copy...

	strncpy(temp, &zda_time[0], 2);  // copy the hour'stwo digits
	sTime.Hours = (uint8_t) atoi(temp);  // convert to integer

	strncpy(temp, &zda_time[2], 2);  // copy the minute's two digits
	sTime.Minutes = (uint8_t) atoi(temp);

	strncpy(temp, &zda_time[4], 2);  // copy the second's two digits
	sTime.Seconds = (uint8_t) atoi(temp);

	if(HAL_RTC_SetTime(rtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
}

// sets GPS time based on RTC time
static void _SetGPSfromRTC(org1510mk4_t *gps, RTC_HandleTypeDef *rtc)
{
	if(HAL_RTC_GetTime(rtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_RTC_GetDate(rtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}

	char outstr[34] = "\0";  // buffer for command string
	// assemble PMTK740 - set UTC date/time command
	// e.g. $PMTK740,2024,02,24,08,27,00*3C
	sprintf(outstr, "PMTK740,%u,%u,%u,%u,%u,%u", sDate.Year + 2000, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);
	gps->Write(outstr);  // send the command out
}

// the one actor
void _TimeHelper(const uint8_t GPS_flag)
{
	__TimeHelper.public.flagGPScorrect = GPS_flag;

	if(!__TimeHelper.public.flagGPScorrect && !__TimeHelper.public.flagRTCcorrect)  // no time is correct
		return;  // get out

	if(__TimeHelper.public.flagGPScorrect && !__TimeHelper.public.flagRTCcorrect)  // GPS has correct time -> set RTC
		{
			_SetRTCfromGPS(__TimeHelper.gps, __TimeHelper.rtc);  // set RTC based off GPS' time
			__TimeHelper.public.flagRTCcorrect = 1;  // mark
			return;
		}

	if(!__TimeHelper.public.flagGPScorrect && __TimeHelper.public.flagRTCcorrect)  // RTC has correct time -> set GPS
		{
			_SetGPSfromRTC(__TimeHelper.gps, __TimeHelper.rtc);  // set GPS based off RTC's time
			__TimeHelper.public.flagGPScorrect = 1;  // mark
			return;
		}

	if(__TimeHelper.public.flagGPScorrect && __TimeHelper.public.flagRTCcorrect)  // periodically update RTC from GPS
		{
			if(__TimeHelper.gps->zda->time[5] == '0')
				__TimeHelper.public.flagRTCcorrect = 0;  // un-mark so that it will be set on the next iteration
			return;
		}
}

// TODO - POC code - seeds the current location
void _LocationHelper(const uint8_t force)
{
	if(HAL_RTC_GetTime(__TimeHelper.rtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}
	if(HAL_RTC_GetDate(__TimeHelper.rtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
		{
			Error_Handler();
		}

	// TODO - POC code
	if((!__TimeHelper.gps->flag_location_seeded && *__TimeHelper.gps->AlmanacFlags) || force)  // if location needs to be seeded
		{
			char outstr[60] = "\0";  // buffer for command string
			// assemble PMTK741 command - set location and date
			// e.g. PMTK741,4547.8113,01554.8408,134.1,2024,02,25,20,44,00
			// $PMTK741,4547.8113,01554.8408,134.1,2024,03,03,06,06,27*0C\r\n
			sprintf(outstr, "PMTK741,45.797515,15.914649,134.1,%u,%02u,%02u,%02u,%02u,%02u", sDate.Year + 2000, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);

			__TimeHelper.gps->Write(outstr);  // send the command out

			__TimeHelper.gps->flag_location_seeded = 1;  // mark location as seeded
		}
}

//
timehelper_t* timehelper_ctor(org1510mk4_t *gps, RTC_HandleTypeDef *rtc)
{
	__TimeHelper.gps = gps;  // store GPS module object
	__TimeHelper.rtc = rtc;  // store RTC object
	__TimeHelper.public.TimeHelper = &_TimeHelper;	// the one actor
	__TimeHelper.public.LocationHelper = &_LocationHelper;  // the one actor

	__TimeHelper.public.flagGPScorrect = gps->flag_time_accurate;  // set GPS time state
	__TimeHelper.public.flagRTCcorrect = 0;  // set RTC time state

	return &__TimeHelper.public;
}

timehelper_t *const TimeHelper = &__TimeHelper.public;  // set pointer to TimeHelper public part

#endif
