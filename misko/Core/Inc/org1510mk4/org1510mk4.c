#include "org1510mk4/org1510mk4.h"

#if defined(USE_ORG1510MK4)	// if this particular device is active
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lwrb\lwrb.h"	// Lightweight RingBuffer - https://docs.majerle.eu/projects/lwrb/en/latest/index.html

#define DIRTY_POWER_MODE_CHANGE 0	// circumvents power mode change safeguards to e.g. deliberately drain the capacitor
#define DEBUG_GPS_RX_RB_FREE 0 // data member indicating LwRB free space

#define UART1_GPS_RX_DMA_BUFFER_LEN 32	// circular DMA RX buffer length for incoming GPS UART DMA data
#define UART1_GPS_RX_RINGBUFFER_LEN 256	// ringbuffer size
#define GPS_OUT_BUFFER_LEN 256	// _GPS_out buffer length

extern ADC_HandleTypeDef hadc1;  // TODO - hadc1 - move out of here
extern volatile uint32_t __adc_dma_buffer[ADC_CHANNELS];  // TODO - __adc_dma_buffer[] - move out of here - store for ADC readout
extern volatile uint32_t __adc_results[ADC_CHANNELS];  // TODO - __adc_results[] - move out of here - store ADC average data

typedef struct	// org1510mk4c_t actual
{
	UART_HandleTypeDef *uart_gps;  // HAL UART instance over which to communicate with the GPS module
	UART_HandleTypeDef *uart_sys;  // HAL UART instance over which the whole device communicates with a host computer/VCP

#if DEBUG_LWRB_FREE
	uint16_t lwrb_free;  // ringbuffer free memory
	uint32_t char_written;	// characters written
	lwrb_sz_t ovrflowlen;  // amount of data written in overflow mode
	lwrb_sz_t linearlen;  // amount of data written in linear mode
#endif
#if PARSE_PMTK
	pmtk_t *pmtk;  // container for PMTK messages
#endif
	org1510mk4_t public;  // public struct
} __org1510mk4_t;

static __org1510mk4_t __ORG1510MK4 __attribute__ ((section (".data")));  // preallocate __ORG1510MK4 object in .data

static lwrb_t uart1_gps_rx_rb;  // 2nd circular buffer for data processing
static uint8_t uart1_gps_rx_rb_buffer[UART1_GPS_RX_RINGBUFFER_LEN];  //

#if PARSE_GGA || PARSE_ZDA || PARSE_RMC || PARSE_GLL
static date_t _date;
static nmea_time_t _time;
#endif
#if PARSE_GGA || PARSE_RMC || PARSE_GLL
static coord_dd_t _lat;  // object for GGA latitude
static coord_dd_t _lon;  // object for GGA longitude
static cardinal_dir_t _cd;	// cardinal direction - NSEW
#endif
#if PARSE_ZDA
static zda_t _zda;  // object for ZDA sentence
#endif
#if PARSE_GGA
static gga_t _gga;  // object for GGA sentence
#endif
#if PARSE_VTG
static vtg_t _vtg;	// object for VTG sentence
#endif
#if PARSE_GSA && !PARSE_GSV
static gsa_t _gpgsa;	// object for GPS GSA sentence
static gsa_t _glgsa;	// object for GLANOSS GSA sentence
#endif
#if PARSE_GSV
uint32_t _pmtk661;	 // SV PRN flags with valid alamanac (PRN 1 is leftmost)
static gsv_t _gpgsv;	// object for GPS GSV sentence
static gsv_t _glgsv;	// object for GLANOSS GSV sentence
#endif
#if PARSE_GSA
static gsa_t _gsa;	// object for GSA sentences
#endif
# if PARSE_RMC
static rmc_t _rmc;	// object for RMC sentence
#endif
#if PARSE_GLL
static gll_t _gll;	// object for GLL sentences
#endif
#if PARSE_PMTK
static pmtk_t _pmtk;  // container for PMTK messages
static char _pmtk_buff[255];	// packet buffer
#endif

static uint8_t uart1_gps_rx_dma_buffer[UART1_GPS_RX_DMA_BUFFER_LEN] = "\0";  // 1st circular buffer: incoming GPS UART DMA data
static uint8_t _GPS_out[GPS_OUT_BUFFER_LEN] = "\0";  // output box for NMEA sentences fished out of the ringbuffer

// wait time in ms
static inline void _wait(const uint16_t ms)
{
	uint32_t i = (HAL_RCC_GetSysClockFreq() / 1000) * (ms / 5);
	while(i--)
		;
}

// calculates NMEA checksum
// https://nmeachecksum.eqth.net
// XOR of all the bytes between the $ and the * (not including the delimiters themselves)
static uint8_t calculate_checksum(const char *str, const uint8_t len)
{
	uint8_t retval = 0;

	for(uint8_t i = 0; i < len + 1; i++)	// go over the whole input string
		retval ^= *(str + i);  // XOR the characters

	return retval;	// return the XOR
}

// do we have a checksum mismatch? 1 - true, 0 - false
static uint8_t checksumMismatch(const char *sentence, const uint8_t len)
{
	const char *ptr = sentence;
	uint8_t checksum = calculate_checksum(sentence, len - 1);  // calculate the input sentence as a whole first
	uint8_t checksumInSentence = 0;

	// convert the NMEA-reported checksum (in ASCII) into a number
	while(*ptr != '\0' && *ptr != '\r' && *ptr != '\n')
		{
			if(*ptr >= '0' && *ptr <= '9')
				{
					checksumInSentence = (uint8_t) (checksumInSentence * 16 + (*ptr - '0'));
				}
			else if(*ptr >= 'A' && *ptr <= 'F')
				{
					checksumInSentence = (uint8_t) (checksumInSentence * 16 + (*ptr - 'A' + 10));
				}
			else if(*ptr >= 'a' && *ptr <= 'f')
				{
					checksumInSentence = (uint8_t) (checksumInSentence * 16 + (*ptr - 'a' + 10));
				}

			*ptr++;
		}

	return (checksum == checksumInSentence) ? 0 : 1;	// compare the two sums
}

// init function for GPS module
static void _init(void)
{
	/*
	 * module default settings:
	 * firmware AXN_3.8_3333_16042118,0000,V3.8.1 GP+GL
	 *
	 * 	WGS84 datum (330/430/530)
	 *	SBAS enabled (313/413/
	 *	DGPS source is WAAS (301/401/501)
	 *	search for GPS & GLANOSS only (353/355)
	 *	EASY enabled (869)
	 *	LOCUS enabled (183: $PMTKLOG,0,0,b,31,15,0,0,1,0,0*13)
	 *	HDOP threshold disabled (356/357)
	 *	dead reckoning tunnel scenario: report 0 fixes (308/408/508)
	 *	satellite elevation mask: 5 deg. (311/411/511)
	 *	solution priority: precision (257)
	 *	static nav. speed threshold: disabled (386)
	 *	AIC (jamming rejection): enabled (286)
	 *
	 *
	 */
//	__ORG1510MK4.public.Power(wakeup);	// power up & bring into normal mode
	__ORG1510MK4.public.Write("PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");  // shut the damn thing off first

	__ORG1510MK4.public.Write("PMTK185,1");  // stop LOCUS logging
	__ORG1510MK4.public.Write("PMTK256, 0");  // disable 1PPS
	__ORG1510MK4.public.Write("PMTK286,1");  // enable active interference cancellation
	__ORG1510MK4.public.Write("PMTK285,0,0"); 	// disable 1PPS
	__ORG1510MK4.public.Write("PMTK869,1,1");  // enable EASY
	//__ORG1510MK4.public.Write("PMTK257,0");  // enable fast TtFF when exiting tunnel/garage
	// TODO - periodically switch modes depending on speed
	__ORG1510MK4.public.Write("PMTK886,1");  // pedestrian mode  (slower than 5m/s)
//	__ORG1510MK4.public.Write("PMTK886,0"); // vehicle mode (faster than 5m/s)

	/* NMEA sentences:
	 * 0 GLL - Geographical Position-Latitude/Longitude
	 * 0 RMC - Recommended Minimum Specific GNSS Data
	 * 1 VTG - Course over Ground and Ground Speed
	 * 1 GGA - Global Positioning System Fix Data
	 * 10 GSA - GNSS DOP and Active Satellites
	 * 10 GSV - GNSS Satellites in View
	 * 0 GRS - GNSS Range Residuals
	 * 0 GST - GNSS Range Statistics
	 * ...
	 * 1 ZDA - UTC Date/Time and Local Time Zone Offset
	 * 0 MCHN - GNSS channel status
	 * DTM - Datum reference
	 *
	 */
	__ORG1510MK4.public.Write("PMTK314,0,0,1,1,10,10,0,0,0,0,0,0,0,0,0,0,0,1,0");

//	__ORG1510MK4.public.Power(off);  // power off
}

// GPS module power mode change control function
static void _Power(const org1510mk4_power_t state)
{
	/* power / current consumption & equivalent series resistance at 3.3V supply
	 * as per datasheet:
	 * 	full power - acquisition: 92mW - 27.88 mA - 118.37 Ohm ESR
	 * 	full power - tracking: 79mW - 23.94 mA - 137.85 Ohm ESR
	 * 	periodic: 16mW - 4.85mA - 680.63 Ohm ESR
	 * 	standby: 1.65mW - 0.5mA - 6.6 kOhm ESR
	 * 	backup: 50uW - 15uA - 217.8 kOhm ESR
	 *
	 * 	pin states:
	 * 		GPS_WKUP_Pin: stm32 gpio input
	 * 			low: standby or backup mode; low while in periodic/alwayslocate sleep mode, high when in wake periods
	 * 			high: module is in full power
	 *
	 * 		GPS_RESET_Pin: stm32 gpio output, active low
	 * 			low: reset
	 * 			high: normal operation
	 *
	 * 		GPS_PWR_CTRL_Pin: stm32 gpio output (aka. force on)
	 * 			low: module can enter backup mode
	 * 			high: module can not enter backup mode
	 *
	 * 		timing:
	 * 			after reset:
	 * 				not ideal conditions:
	 * 					- get correct date/time: TODO - measure acquisition time
	 * 					- see first satellite: TODO - measure acquisition time
	 * 					- get a fix: TODO - measure acquisition time
	 * 				ideal conditions:
	 * 					- get correct date/time: TODO - measure acquisition time
	 * 					- see first satellite: TODO - measure acquisition time
	 * 					- get a fix: TODO - measure acquisition time
	 */

	if(state == off)	// supply power off & module (possibly) in backup mode
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_GPIO_WritePin(SUPERCAP_EN_GPIO_Port, SUPERCAP_EN_Pin, GPIO_PIN_RESET);	// turn off supercap charger

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode > backup)	// if the module is in some operating mode
				__ORG1510MK4.public.Power(backup);	// first go into backup mode

			if(__ORG1510MK4.public.PowerMode <= backup)  // if the module is in backup mode (or less)
				HAL_GPIO_WritePin(SUPERCAP_EN_GPIO_Port, SUPERCAP_EN_Pin, GPIO_PIN_RESET);	// then turn off supercap charger

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == on)  // supply power on & module (possibly) in backup mode
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_GPIO_WritePin(SUPERCAP_EN_GPIO_Port, SUPERCAP_EN_Pin, GPIO_PIN_SET);	// power on the supercap charger

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode < wakeup)  // if the module was powered off
				{
					if(HAL_ADC_GetState(&hadc1) == HAL_ADC_STATE_RESET)  // if the ADC is down
						HAL_ADC_Start_DMA(&hadc1, (uint32_t*) __adc_dma_buffer, ADC_CHANNELS);	// start it

					if(HAL_ADC_GetError(&hadc1) == 0)  // if the ADC is not in some error state
						{
							HAL_GPIO_WritePin(SUPERCAP_EN_GPIO_Port, SUPERCAP_EN_Pin, GPIO_PIN_SET);	// power on the supercap charger

							while(__adc_results[Vgps] < 3000)
								// wait until the supply voltage is high enough (or the module is awake)
								;//break;  // FIXME - blocks ADC somehow

							__ORG1510MK4.public.PowerMode = state;	// save the current power mode
							return;
						}
				}
#endif
		}

	if(state == backup)  // "backup mode", DS. ch. 4.3.15
		{
#if DIRTY_POWER_MODE_CHANGE
			__ORG1510MK4.public.Write("PMTK225,4");  // send backup mode command

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode > backup)  // if the module is in some operating mode
				{
					HAL_GPIO_WritePin(GPS_PWR_CTRL_GPIO_Port, GPS_PWR_CTRL_Pin, GPIO_PIN_RESET);	// bring Force-On down
					__ORG1510MK4.public.Write("PMTK225,4");  // send backup mode command

					while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin))
						;  // wait until the wakeup pin goes low
				}

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == wakeup)  // return to full power mode
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_GPIO_WritePin(GPS_PWR_CTRL_GPIO_Port, GPS_PWR_CTRL_Pin, GPIO_PIN_SET);	// set high
			_wait(1000);	 // wait 1s
			HAL_GPIO_WritePin(GPS_PWR_CTRL_GPIO_Port, GPS_PWR_CTRL_Pin, GPIO_PIN_RESET);	// set low

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode == off)  // if the module is powered off
				__ORG1510MK4.public.Power(on);	// first power on,

			if(__ORG1510MK4.public.PowerMode == on)  // if the module is powered on
				__ORG1510MK4.public.PowerMode = backup;  // then cheat the mode into backup

			if(__ORG1510MK4.public.PowerMode == backup)  // if in backup mode
				{
					// get out of backup: ("off" state can also be a backup state)
					HAL_GPIO_WritePin(GPS_PWR_CTRL_GPIO_Port, GPS_PWR_CTRL_Pin, GPIO_PIN_SET);	// set high
					_wait(1000);	 // wait 1s
					HAL_GPIO_WritePin(GPS_PWR_CTRL_GPIO_Port, GPS_PWR_CTRL_Pin, GPIO_PIN_RESET);	// set low

					while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin) == GPIO_PIN_RESET)
						;  // wait until the wakeup pin goes high
				}

			if(__ORG1510MK4.public.PowerMode == standby)  // if the module is in standby
				__ORG1510MK4.public.Write("PMTK225,0");  // wakeup command - transit into full power mode

			if(__ORG1510MK4.public.PowerMode > standby)  // if the module is in periodic/alwayslocate
				{
					while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin) == GPIO_PIN_RESET)
						;  // wait until the wakeup pin goes high

					__ORG1510MK4.public.Write("PMTK225,0");  // then send wakeup command - transit into full power mode
				}

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == standby)  // standby mode, DS. ch. 4.3.12
		{
#if DIRTY_POWER_MODE_CHANGE
			__ORG1510MK4.public.Write("PMTK161,0");  // then go into standby

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode < wakeup)  // if the module is not awake
				return;  // do nothing

			__ORG1510MK4.public.Write("PMTK161,0");  // send standby sleep command

			while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin))
				;  // wait until the wakeup pin goes low

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == periodic)  // periodic mode, DS. ch. 4.3.13
		{
#if DIRTY_POWER_MODE_CHANGE
			// TODO - implement periodic power mode

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode < wakeup)  // if the module is not awake
				return;  // do nothing

			// periodic mode settings are left at default values

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == alwayslocate)  // alwaysLocate mode, DS. ch. 4.3.14
		{
#if DIRTY_POWER_MODE_CHANGE
			__ORG1510MK4.public.Write("PMTK225,9");  // DS. ch. 4.3.14

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode < wakeup)  // if the module is not awake
				return;  // do nothing

			__ORG1510MK4.public.Write("PMTK225,9");  // send command for AlwaysLocate backup mode

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == reset)  // reset the module
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_RESET);  // take GPS module into reset
			_wait(200);  // wait 200ms
			HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_SET);  // take GPS module out of reset
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode == backup)  // if the module is in backup
				__ORG1510MK4.public.Power(wakeup);	// first, wake up

			if(__ORG1510MK4.public.PowerMode == off)  // if the module is powered off
				{
					__ORG1510MK4.public.Power(on);	// first, turn on
					__ORG1510MK4.public.Power(wakeup);	// then, wake up
				}

			__ORG1510MK4.public.Write("PMTK104");  // send reset command
			_wait(100);

			// finally: reset
			HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_RESET);  // take GPS module into reset
			_wait(200);  // wait 200ms
			HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_SET);  // take GPS module out of reset

			while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin) == GPIO_PIN_RESET)
				;						// wait until the wakeup pin goes high

			__ORG1510MK4.public.Power(wakeup);	// then, wake up

			while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin) == GPIO_PIN_RESET)
				;  // wait until the wakeup pin goes high

			_wait(450);  // first wait for module power-up
			_init();  // then re-initialize the module

			return;
#endif
		}

	if(state == discharge)	// discharge the supercap
		{
			__ORG1510MK4.public.PowerMode = discharge;
			HAL_GPIO_WritePin(SUPERCAP_EN_GPIO_Port, SUPERCAP_EN_Pin, GPIO_PIN_RESET);	// turn off supercap charger

			HAL_GPIO_WritePin(SC_DISCHARGE_GPIO_Port, SC_DISCHARGE_Pin, GPIO_PIN_SET);	// start to discharge

			while(__adc_results[Vgps] > 50)
				// wait until the supply voltage drops below 50mV
				;

			_wait(5000);	// wait 5 seconds

			HAL_GPIO_WritePin(SC_DISCHARGE_GPIO_Port, SC_DISCHARGE_Pin, GPIO_PIN_RESET);	// stop the discharge
			__ORG1510MK4.public.PowerMode = off;
		}
}

//
static void rx_start(void)
{
	if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(__ORG1510MK4.uart_gps, uart1_gps_rx_dma_buffer, UART1_GPS_RX_DMA_BUFFER_LEN))  // start reception)
		Error_Handler();
}

static char* strtok_fr(char *s, char delim, char **save_ptr)
{
	char *tail;
	char c;

	if(s == NULL)
		{
			s = *save_ptr;
		}
	tail = s;
	if((c = *tail) == '\0')
		{
			s = NULL;
		}
	else
		{
			do
				{
					if(c == delim)
						{
							*tail++ = '\0';
							break;
						}
				}
			while((c = *++tail) != '\0');
		}
	*save_ptr = tail;
	return s;
}

static char* strtok_f(char *s, char delim)
{
	static char *save_ptr;

	return strtok_fr(s, delim, &save_ptr);
}

#if PARSE_GGA || PARSE_RMC || PARSE_GLL
// converts NMEA decimal degrees to coord_dd_t object
static void NMEA_DecimalDegree_to_coord_dd_t(char *str, coord_dd_t *coord)
{
	coord->deg = (uint8_t) (atoi(str) / 100);
	coord->s = (float) ((atof(str) / 100.0) - coord->deg) * 100;
}
#endif

#if PARSE_ZDA || PARSE_RMC
// converts time format "163207" into time_t object
void _ParseTime(char *timestr, nmea_time_t *timeobj)
{
	char temptemp[2];
	memcpy(temptemp, &timestr[0], 2);  // hours
	timeobj->hh = (uint8_t) atoi(temptemp);
	memcpy(temptemp, &timestr[2], 2);  // minutes
	timeobj->mm = (uint8_t) atoi(temptemp);
	memcpy(temptemp, &timestr[4], 2);  // seconds
	timeobj->ss = (uint8_t) atoi(temptemp);
}
#endif

#if PARSE_ZDA
// parses NMEA str for ZDA data
static void ParseZDA(zda_t *sentence, const char *str)
{
	char *msg = strstr(str, "ZDA");  // first, check if we have the correct message type

	if(msg == NULL)  // if not, get out
		return;

	char temp[GPS_OUT_BUFFER_LEN] = "\0";  // create a temporary buffer

	strncpy(temp, str, strlen(str));	// copy str into temp

	// $GNZDA,163207.000,15,02,2024,,*4B
	strtok_f(temp, ',');	// start to tokenize

	char tempdt[6];
	memcpy(tempdt, strtok_f(NULL, ','), 6);  // UTC time - 163207.000

	_ParseTime(tempdt, sentence->time);  // convert UTC time 163207 to time_t

	sentence->date->dd = (uint8_t) atoi(strtok_f(NULL, ','));  // day - 15
	sentence->date->mm = (uint8_t) atoi(strtok_f(NULL, ','));  // month - 02
	sentence->date->yyyy = (uint16_t) atoi(strtok_f(NULL, ','));  // year 2024
	sentence->tz = (uint8_t) atoi(strtok_f(NULL, ','));  // local timezone offset

	if(sentence->date->yyyy > 1980)
		__ORG1510MK4.public.FlagTimeAccurate = 1;
	else
		__ORG1510MK4.public.FlagTimeAccurate = 0;

	if(__ORG1510MK4.public.print->zda)
		HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP
}
#endif

#if PARSE_GGA
// parses str for GGA data
static void ParseGGA(gga_t *sentence, const char *str)
{
	char *msg = strstr(str, "GGA");  // first, check if we have the correct message type

	if(msg == NULL)  // if not, get out
		return;

	char temp[GPS_OUT_BUFFER_LEN] = "\0";  // create a temporary buffer

	strncpy(temp, str, strlen(str));  // copy str into temp

	// $GNGGA,161439.000,4547.8623,N,01554.9327,E,1,5,2.05,104.7,M,42.5,M,,*4E
	char *tok = strtok_f(temp, ',');	// start to tokenize

	char tempdt[6];
	memcpy(tempdt, strtok_f(NULL, ','), 6);  // UTC of this position report - 161439.000

//	_ParseTime(tempdt, sentence->time);

	tok = strtok_f(NULL, ',');
	NMEA_DecimalDegree_to_coord_dd_t(tok, sentence->lat);  // 4547.8623 to 45 and 47.8623 in coord_dd_t

	tok = strtok_f(NULL, ',');
	sentence->lat_dir = (cardinal_dir_t*) tok;  // north - N

	tok = strtok_f(NULL, ',');
	NMEA_DecimalDegree_to_coord_dd_t(tok, sentence->lon);  // 01554.9327 to 15 and 54.9327 in coord_dd_t

	tok = strtok_f(NULL, ',');
	sentence->lon_dir = (cardinal_dir_t*) tok;  // east - E
	sentence->fix = (gga_fix_t) atoi(strtok_f(NULL, ','));  // GPS fix indicator - 1
	sentence->sat_used = (uint8_t) atoi(strtok_f(NULL, ','));  // satellites used for solution - 5
	sentence->HDOP = (float) atof(strtok_f(NULL, ','));  // horizontal dilution of position - 2.05
	sentence->alt = (float) atof(strtok_f(NULL, ','));  // alt - 104.7
	strtok_f(NULL, ',');  // M
	sentence->geoid_sep = (float) atof(strtok_f(NULL, ','));  // geoid seperation - 42.5
	strtok_f(NULL, ',');  // M
	sentence->dgps_age = (float) atof(strtok_f(NULL, ','));  // DGPS age

	if(__ORG1510MK4.public.print->gga)
		HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP
}
#endif

#if PARSE_VTG
// parses str for VTG data
static void ParseVTG(vtg_t *sentence, const char *str)
{
	char *msg = strstr(str, "VTG");  // first, check if we have the correct message type

	if(msg == NULL)  // if not, get out
		return;

	char temp[GPS_OUT_BUFFER_LEN] = "\0";  // create a temporary buffer

	strncpy(temp, str, strlen(str));  // copy str into temp

	// $GNVTG,20.11,T,,M,2.50,N,4.63,K,A*17
	char *tok = strtok_f(temp, ',');	// start to tokenize
	sentence->track_tn = (float) atof(strtok_f(NULL, ','));  // heading of track made good in true north - 20.11
	strtok_f(NULL, ',');  // T
	sentence->track_mn = (float) atof(strtok_f(NULL, ','));  // heading of track made good in magnetic north
	strtok_f(NULL, ',');  // M
	sentence->knots = (float) atof(strtok_f(NULL, ','));  // speed in knots - 2.5
	strtok_f(NULL, ',');  // N
	sentence->kph = (float) atof(strtok_f(NULL, ','));  // speed in kilometres per hour - 4.63
	strtok_f(NULL, ',');  // K
	tok = strtok_f(NULL, ',');
	sentence->mode = (faa_mode_t) *tok;  // FAA mode indicator

	if(__ORG1510MK4.public.print->vtg)
		HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP
}
#endif

/* GPS satellite NMEA PRN assignment
 * 	01 - 32 - GPS
 * 	65 - 88 - GLONASS
 * 	33 - 51 - SBAS
 * 	52 - 71 - SBAS
 */

#if PARSE_GSA && PARSE_GSV
// searches for GSA PRN in priv_gsv and returns object address, NULL otherwise
static spacevehicle_t* linkSV(const uint8_t gsa_prn, gsv_t *priv_gsv)
{
	for(uint8_t i = 0; i < 12; i++)  // loop over all GSV's
		{
			if(priv_gsv->sv[i].prn == gsa_prn)	// and compare GSV PRN with GSA PRN
				return &priv_gsv->sv[i];	// if found return it
		}

	return NULL;	// otherwise return NULL
}
#endif

#if PARSE_GSA
// parses NMEA str for GSA data and link "satellites used" with GSV SVs
#if PARSE_GSV
static void ParseGNGSA(const char *talker, const char *str, gsa_t *pub_gsa, gsv_t *priv_view)
#else
static void ParseGNGSA(const char *talker, const char *str, gsa_t *pub_gsa)
#endif
{
	char *msg = strstr(str, talker);  // first, check if we have the correct message type

	if(msg == NULL)  // if not, get out
		return;

#if PARSE_GGA
	if(__ORG1510MK4.public.gga->fix == 0)  // if no SVs are used for a solution
		{
			for(uint8_t i = 0; i < 12; i++)  // zero-out previously linked SVs
				pub_gsa->sv[i] = NULL;

			pub_gsa->HDOP = 0;	// zero fields
			pub_gsa->VDOP = 0;
			pub_gsa->PDOP = 0;

			if(__ORG1510MK4.public.print->gsa)
				HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP

			return;  // and get out
		}
#endif

#if PARSE_GSV
	if(priv_view->sv_visible == 0)  // if no SVs are used for a solution
		return;
#endif

	static uint8_t iter;	// GSA loop (this function) iteration counter

	char temp[GPS_OUT_BUFFER_LEN] = "\0";  // create a temporary buffer
#if PARSE_GSV
	uint8_t prn = 0;	// SV PRN
#endif
	static uint8_t ins;  // successful insertions

	strncpy(temp, str, strlen(str));	// copy str into temp

//	$GPGSA,A,3,13,24,14,15,22,,,,,,,,1.56,1.22,0.97*0A
//	$GLGSA,A,3,79,68,,,,,,,,,,,1.56,1.22,0.97*13
	char *tok = strtok_f(temp, ',');	// start to tokenize

	tok = strtok_f(NULL, ',');
	pub_gsa->sel_mode = (gsa_selectionmode_t) *tok;  // GSA selection mode - A

	pub_gsa->fixmode = (gsa_fixmode_t) atoi(strtok_f(NULL, ','));  // GSA fix mode - 3

#if PARSE_GSV
	// now tokenize SVs and - if found - link it to GSA's SV list
	for(uint8_t i = 0; i < 12; i++)
		{
			if(ins == 12)
				{
					ins = 0;
					break;
				}

			prn = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of space vehicle

			if(prn)  // if it is non-zero
				{
					spacevehicle_t *svptr = linkSV(prn, priv_view);  // find PRN in gsv_t and get pointer to it
					pub_gsa->sv[ins] = svptr;  // link that pointer (it can be NULL!!)

					if(svptr != NULL)  // match found
						ins++;	// count the reference insertions - i.e. advance position by one
				}
		}
#else
	pub_gsa->sv01 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv02 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv03 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv04 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv05 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv06 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv07 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv08 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv09 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv10 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv11 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
	pub_gsa->sv12 = (uint8_t) atoi(strtok_f(NULL, ','));  // PRN of SV01
#endif

	// continue with normal tokenizing
	pub_gsa->PDOP = (float) atof(strtok_f(NULL, ','));  // Positional Dilution of Position - 4.75
	pub_gsa->HDOP = (float) atof(strtok_f(NULL, ','));  // Horizontal Dilution of Position - 4.64
	pub_gsa->VDOP = (float) atof(strtok_f(NULL, ','));  // VErtical Dilution of Position - 0.98

	iter++;  // advance index one position

	if(iter == GSA_COUNT)  // we reached the number of possible GSA iterations
		{
#if PARSE_GSV
			do
				{
					pub_gsa->sv[ins++] = NULL;	// zero out rest
				}
			while(ins < 12);
#endif
			iter = 0;  // start over
			ins = 0;
		}

	if(__ORG1510MK4.public.print->gsa)
		HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP
}
#endif

#if PARSE_GSV
// parses NMEA str for GSV data - all talkers
static void ParseGNGSV(gsv_t *sentence, const char *talker, const char *str)
{
	char *msg = strstr(str, talker);  // first, check if we have the correct message type

	if(msg == NULL)  // if not, get out
		return;

	char temp[GPS_OUT_BUFFER_LEN] = "\0";  // create a temporary buffer

	strncpy(temp, str, strlen(str));	// copy str into temp

//	$GPGSV,3,1,11,24,66,305,21,15,53,205,26,19,46,104,,17,38,064,*79
//	$GPGSV,3,2,11,22,36,055,17,13,36,157,26,12,30,235,28,23,19,278,*71
//	$GPGSV,3,3,11,14,15,057,,10,14,315,14,02,01,020,*4D
//	$GLGSV,3,1,10,78,59,032,,69,54,288,,79,50,283,18,68,41,206,18*65
//	$GLGSV,3,2,10,87,23,045,,88,21,103,,70,07,339,,77,07,067,*64
//	$GLGSV,3,3,10,80,03,259,,67,01,177,*60
	char *tok = strtok_f(temp, ',');	// start to tokenize

	sentence->msg_count = (uint8_t) atoi(strtok_f(NULL, ','));  // 3
	uint8_t num = (uint8_t) atoi(strtok_f(NULL, ','));  // 1,2,3.. only needed for internal computation
	sentence->sv_visible = (uint8_t) atoi(strtok_f(NULL, ','));  //	09

	if(sentence->sv_visible == 0)  // no space vehicles visible, no point to start tokenizing
		{
			for(uint8_t i = 0; i < 12; i++)  // in case there are values, zero them out
				{
					sentence->sv[i].alm = 0;
					sentence->sv[i].eph = 0;
					sentence->sv[i].prn = 0;
					sentence->sv[i].elev = 0;
					sentence->sv[i].azim = 0;
					sentence->sv[i].snr = 0;
				}

			_pmtk661 = 0;  // CHECKME - zero-put flags, maybe move elsewhere

			return;
		}

	if(sentence->sv_visible > 12)  // the tokenizer tokenized crap
		sentence->sv_visible = 12;

	static uint8_t n;  // SV array index depending on message count (1-3)
	static uint8_t lastn;  // previous SV array iterator

	// iterator setup depending on the message count (1-3)
	if(num == 1)	// message number 1
		n = 0;	// SV array index 0 to 3
	if(num == 2)  // message number 2
		n = 4;	// SV array index 4 to 7
	if(num == 3)  // message number 3
		n = 8;	// SV array index 8 to 11

	if(lastn > sentence->sv_visible)	// if the last GSV had more SVs than the current
		{
			lastn = sentence->sv_visible;  // start from the previous index
			do	// and zero out the rest ...
				{
					sentence->sv[lastn].alm = 0;
					sentence->sv[lastn].eph = 0;
					sentence->sv[lastn].prn = 0;
					sentence->sv[lastn].elev = 0;
					sentence->sv[lastn].azim = 0;
					sentence->sv[lastn].snr = 0;
					lastn++;  // move to the next field
				}
			while(lastn < 12);	// ...until the end
		}

	for(uint8_t i = n; i <= (4 * num); i++)  // loop over the up to 4 SVs per message
		{
			if(i == 12)  // safeguard against crap
				break;

			tok = strtok_f(NULL, ',');	// tokenize PRN
			if(tok)  // if there is one
				sentence->sv[i].prn = (uint8_t) atoi(tok);	// set the PRN decimal number
			else
				sentence->sv[i].prn = 0;	// else set 0

			tok = strtok_f(NULL, ',');
			if(tok)
				sentence->sv[i].elev = (uint8_t) atoi(tok);
			else
				sentence->sv[i].elev = 0;

			tok = strtok_f(NULL, ',');
			if(tok)
				sentence->sv[i].azim = (uint16_t) atoi(tok);
			else
				sentence->sv[i].azim = 0;

			tok = strtok_f(NULL, ',');
			if(tok)
				sentence->sv[i].snr = (uint8_t) atoi(tok);
			else
				sentence->sv[i].snr = 0;

			n = i;	// store current loop iterator for next message

			if(n > lastn)  // on the last GSV message, flag for ephemeris & almanac query
				__ORG1510MK4.public.FlagQueryAlmandEph = 1;  // CHECKME - flag_alm_eph_query - move to RTC ISR

			lastn = n;	// store last iterator of last message for next parser call
		}

	if(__ORG1510MK4.public.print->gsv)
		HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP
}
#endif

#if PARSE_GSA
// parses NMEA str for GSV and GSA data
static void ParseGSA(gsa_t *sentence, const char *str)
{
#if PARSE_GSV
	// first, fill private GSV struct with SVs - essentially arrays of spacevehicle_t's
	ParseGNGSV(&_gpgsv, "GPGSV", str);  // parse GPGSV
	ParseGNGSV(&_glgsv, "GLGSV", str);  // parse GLGSV
#endif

	// then, parse GSA sentences and populate "satellites used" with pointers to GSV's SVs
#if PARSE_GSV
	ParseGNGSA("GPGSA", str, sentence, &_gpgsv);  // GP
	ParseGNGSA("GLGSA", str, sentence, &_glgsv);  // GL
#else
	ParseGNGSA("GPGSA", str, sentence);  // GP
	ParseGNGSA("GLGSA", str, sentence);  // GL
#endif
}
#endif

#if PARSE_RMC
// parses NMEA str for RMC data
static void ParseRMC(rmc_t *sentence, const char *str)
{
	char *msg = strstr(str, "RMC");  // first, check if we have the correct message type

	if(msg == NULL)  // if not, get out
		return;

	char temp[GPS_OUT_BUFFER_LEN] = "\0";  // create a temporary buffer

	strncpy(temp, str, strlen(str));	// copy str into temp

	// $GNRMC,145342.000,A,4547.8104,N,01554.8789,E,0.55,352.46,180224,,,A*7A
	char *tok = strtok_f(temp, ',');	// start to tokenize

	char tempdt[6];
	memcpy(tempdt, strtok_f(NULL, ','), 6);  // 145342.000

	_ParseTime(tempdt, sentence->time);  // convert UTC time 145342 to time_t

	tok = strtok_f(NULL, ',');	// A
	sentence->status = (rmc_status_t) *tok;

	tok = strtok_f(NULL, ',');  // 4547.8104
	NMEA_DecimalDegree_to_coord_dd_t(tok, sentence->lat);  // 4547.8104 to 45 and 47.8104 in coord_dd_t

	tok = strtok_f(NULL, ',');
	sentence->lat_dir = (cardinal_dir_t*) tok;  // north - N

	tok = strtok_f(NULL, ',');
	NMEA_DecimalDegree_to_coord_dd_t(tok, sentence->lon);  // 01554.8789 to 15 and 54.8789 in coord_dd_t

	tok = strtok_f(NULL, ',');
	sentence->lat_dir = (cardinal_dir_t*) tok;  // north - E

	sentence->knots = (float) atof(strtok_f(NULL, ','));  // 0.55
	sentence->azimut = (uint16_t) atoi(strtok_f(NULL, ','));  // 352.46

	memcpy(tempdt, strtok_f(NULL, ','), 6);  // 180224

	memcpy(temptemp, &tempdt[0], 2);	// years
	sentence->date->dd = (uint8_t) atoi(temptemp);
	memcpy(temptemp, &tempdt[2], 2);	// months
	sentence->date->mm = (uint8_t) atoi(temptemp);
	memcpy(temptemp, &tempdt[4], 2);	// days
	sentence->date->yyyy = (uint16_t) atoi(temptemp) + 2000;

	if(sentence->date->yyyy > 1980)
		__ORG1510MK4.public.FlagTimeAccurate = 1;
	else
		__ORG1510MK4.public.FlagTimeAccurate = 0;

	sentence->mag_var = (uint16_t) atoi(strtok_f(NULL, ','));  // empty

	tok = strtok_f(NULL, ',');
	sentence->var_dir = (cardinal_dir_t*) tok;    //

	if(__ORG1510MK4.public.print->rmc)
		HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP
}
#endif

#if PARSE_GLL
// parses NMEA str for GLL data
static void ParseGLL(gll_t *sentence, const char *str)
{
	char *msg = strstr(str, "GLL");  // first, check if we have the correct message type

	if(msg == NULL)  // if not, get out
		return;

	char temp[GPS_OUT_BUFFER_LEN] = "\0";  // create a temporary buffer

	strncpy(temp, str, strlen(str));	// copy str into temp

	// $GNGLL,4547.8136,N,01554.8884,E,080019.000,A,A*40
	char *tok = strtok_f(temp, ',');	// start to tokenize

	tok = strtok_f(NULL, ',');
	NMEA_DecimalDegree_to_coord_dd_t(tok, sentence->lat);  // 4547.8136 to 45 and 47.8136 in coord_dd_t

	tok = strtok_f(NULL, ',');
	sentence->lat_dir = (cardinal_dir_t*) tok;  // north - N

	tok = strtok_f(NULL, ',');
	NMEA_DecimalDegree_to_coord_dd_t(tok, sentence->lon);  // 080019.000 to 80 and 19.000 in coord_dd_t

	tok = strtok_f(NULL, ',');
	sentence->lon_dir = (cardinal_dir_t*) tok;  // east - E

	memcpy(sentence->time, strtok_f(NULL, ','), 6);  // 080019.000

	tok = strtok_f(NULL, ',');	// A
	sentence->status = (rmc_status_t) *tok;

	tok = strtok_f(NULL, ',');	// A
	sentence->mode = (faa_mode_t) *tok;  // FAA mode indicator

	if(__ORG1510MK4.public.print->gll)
		HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP
}
#endif

#if PARSE_PMTK
// parses NMEA str for PMTK messages
static void ParsePMTK(pmtk_t *message, const char *str)
{
	char *msg = strstr(str, "PMTK");  // first, check if we have the correct message type

	if(msg == NULL)  // if not, get out
		return;

	char temp[GPS_OUT_BUFFER_LEN] = "\0";  // create a temporary buffer
	uint8_t len = (uint8_t) strlen(str);

	strncpy(temp, str, len);	// copy str into temp

	msg = strstr(temp, "PMTK001");	// acknowledgement for PMTK command
	if(msg)
		{
			// $PMTK001,161,3*36
			char *tok = strtok_f(temp, ',');  // start to tokenize

			message->cmd = (uint16_t) atoi(strtok_f(NULL, ','));  //	161

			tok = strtok_f(NULL, ',');
			message->flag = (pmtk_ack_t) *tok;  // 3

			// $PMTK001,449,3,0*25
			// $PMTK001,660,3,6008200*20
			tok = strtok_f(NULL, ',');	// tokenize some more
			if(tok)  // if there is stuff after the comma
				{
					memset(message->buff, '\0', 255);
					memcpy(message->buff, &str[1], strlen(str) - 6);  // put it all into the out buffer and remove $ and checksum

#if PARSE_GSV
					if(message->cmd == 661)  // reply to PMTK661,1 - query ephemeris of a GPS PRN SV
						{
							// war message: $PMTK001,661,3,0*2D -- no data or $PMTK001,661,3,f87ffff0*44 -- success
							// tokenizer position is this: f87ffff0*44
							char temp1[8] = "\0";
							strncpy(temp1, tok, 8);  // copy str into temp
							_pmtk661 = (uint32_t) strtol(temp1, NULL, 16);  // convert hex to integer and store
						}
#endif
				}

			if(__ORG1510MK4.public.print->pmtk_001)
				HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP

			return;
		}

	msg = strstr(temp, "PMTK010");	// system startup message
	if(msg)
		{
			// $PMTK010,002*2D
			char *tok = strtok_f(temp, ',');	// start to tokenize

			tok = strtok_f(NULL, ',');
			message->status = (pmtk_sys_msg_t) tok[2];  // 2

			if(__ORG1510MK4.public.print->pmtk_010)
				HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP

			return;
		}

	msg = strstr(temp, "PMTK011");	// system startup message
	if(msg)
		{
			// $PMTK011,MTKGPS*08
			if(__ORG1510MK4.public.print->pmtk_011)
				HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP

			return;
		}

	msg = strstr(temp, "PMTK710");	// reply to PMTK473 - query ephemeris of a GPS PRN SV
	if(msg)
		{
			// $PMTK001,474,2*36 -- fail
			// $PMTK710,19,3FD000,2F7BE7,F7F547,04E7EA,BE5C0C,1B49D4,00000A,4036BE,1B0909,366822,C0DA88,080406,0F5F58,0CEDA1,0CDBA0,49D47D,FFA73E,ADB69D,002A26,BB4E1B,200A2B,F1C06B,FFA286,1B0500*6F -- success
			char *tok = strtok_f(temp, ',');  // start to tokenize

			tok = strtok_f(NULL, ',');	// tokenize PRN - 02
			uint8_t prn = (uint8_t) strtol(tok, NULL, 16);	// covert NMEA message hex PRN number into decimal integer

#if PARSE_GSV
			for(uint8_t i = 0; i < 12; i++)  // loop over all visible SVs
				{
					if(_gpgsv.sv[i].prn)	// if we had a non-zero PRN in the NMEA sentence
						{
							if(_gpgsv.sv[i].prn == prn)  // compare the SV PRN with the 711 reply and see if they match
								_gpgsv.sv[i].eph = true;	// set to true
						}
					else
						_gpgsv.sv[i].eph = false;  // PRN is/became/was zero -> set to false
				}
#endif

			if(__ORG1510MK4.public.print->pmtk_710)
				HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP
		}

	msg = strstr(temp, "PMTK711");	// reply to PMTK474 - query almanac of a GPS PRN SV
	if(msg)
		{
			// 	command: PMTK474,1	-- fail
			// 		$PMTK001,474,2*36	-- didnt get here because of earlier return
			// 	command: PMTK474,2	-- success
			// 		$PMTK711,02,08FF,428454,4E1083,FD5D00,A10D59,9146C9,CDDBF7,29655B,C1004B*4F
			char *tok = strtok_f(temp, ',');  // start to tokenize

			tok = strtok_f(NULL, ',');	// see comments above for command PMTK710
			uint8_t prn = (uint8_t) strtol(tok, NULL, 16);

#if PARSE_GSV
			for(uint8_t i = 0; i < 12; i++)  //
				{
					if(_gpgsv.sv[i].prn)
						{
							if(_gpgsv.sv[i].prn == prn)
								_gpgsv.sv[i].alm = true;
						}
					else
						_gpgsv.sv[i].alm = false;
				}
#endif

			if(__ORG1510MK4.public.print->pmtk_711)
				HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP

			return;
		}

	msg = strstr(temp, "PMTK668");	// ephemeris parameter of GPS
	if(msg)
		{
			// TODO - implement PMTK668

			if(__ORG1510MK4.public.print->pmtk_668)
				HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, (const uint8_t*) str, (uint16_t) strlen((const char*) str));  // send GPS to VCP

			return;
		}
}
#endif

// buffer NMEA sentences from DMA circular buffer (1st buffer) into ringbuffer (2nd buffer)
static void LoadRingBuffer(lwrb_t *rb, const uint16_t high_pos)
{
	static uint16_t low_pos;

	// safeguard against an RX event interrupt where no new data has come in
	if(high_pos == low_pos)  // no new data
		return;

	// 1st buffering - load into ringbuffer
	if(high_pos > low_pos)  // linear region
// read in a linear fashion from DMA rcpt. buffer from beginning to end of new data
		lwrb_write(rb, &uart1_gps_rx_dma_buffer[low_pos], high_pos - low_pos);  // (high_pos - low_pos) is length of new data
	else	// overflow region
		{
			// read new data from current position until end of DMA rcpt. buffer
			lwrb_write(&uart1_gps_rx_rb, &uart1_gps_rx_dma_buffer[low_pos], UART1_GPS_RX_DMA_BUFFER_LEN - low_pos);

			// then, if there is more after the rollover
			if(high_pos > 0)
// read from beginning of circular DMA buffer until the end position of new data
				lwrb_write(rb, &uart1_gps_rx_dma_buffer[0], high_pos);
		}

	low_pos = high_pos;  // save position until data has been read

#if DEBUG_LWRB_FREE
	__ORG1510MK4.lwrb_free = (uint16_t) lwrb_get_free(&uart1_gps_rx_rb);  // have free memory value visible

	if(__ORG1510MK4.lwrb_free == 0)  // LwRB memory used up: hang here
		Error_Handler();
#endif
}

// load from 2nd buffer into temp[], returns 1 if a complete NMEA sentence was loaded, 0 otherwise
static uint8_t LoadNMEA(lwrb_t *rb, uint8_t *temp)
{
	uint8_t retval = 0;

	// 2nd buffering into LwRB & load NMEA sentence into out[] for parsing
	static lwrb_sz_t nmea_start_pos;  // position store for NMEA sentence start - not needed at all
	if(lwrb_find(rb, "$", 1, 0, &nmea_start_pos))  // starting from the current read pointer location, find the NMEA sentence start marker
		{
			static lwrb_sz_t nmea_terminator_pos;  // position store for NMEA terminator start position
			if(lwrb_find(rb, "\r\n", 2, nmea_start_pos, &nmea_terminator_pos))  // starting from current read pointer location, find the terminator start
				{
					memset(temp, '\0', GPS_OUT_BUFFER_LEN);  // zero out out-container

					// assemble the complete NMEA sentence
					if(nmea_terminator_pos > nmea_start_pos)  // linear region
						{
							nmea_terminator_pos++;  // advance by one to get the \r
							nmea_terminator_pos++;  // advance by one to get the \n

							lwrb_read(rb, temp, nmea_terminator_pos - nmea_start_pos);  // the terminators are from the current read pointer len away

							if(nmea_terminator_pos > UART1_GPS_RX_DMA_BUFFER_LEN)  // if the advance went into overflow
								nmea_start_pos = 0;  // reset to the beginning
							else
								nmea_start_pos = nmea_terminator_pos;  // save position for next iteration

#if DEBUG_LWRB_FREE
							__ORG1510MK4.linearlen = retval;
#endif
						}
					else  // overflow region
						{
							// the terminators are in overflow:
							lwrb_sz_t rest = (lwrb_sz_t) (UART1_GPS_RX_DMA_BUFFER_LEN - nmea_start_pos);  // from the current read pointer to buffer end
							lwrb_sz_t extra = (lwrb_sz_t) (nmea_terminator_pos - rest);  // then from buffer start some more
							extra++;	// advance to get the \r
							extra++;	// advance to get the \n

							if(rest + extra > GPS_OUT_BUFFER_LEN)  // safeguard against temp[] buffer overflow
								return 0;

							lwrb_read(rb, temp, rest);  // read out len characters into out
							lwrb_read(rb, &temp[retval], extra);  // read out len characters into out

#if DEBUG_LWRB_FREE
							__ORG1510MK4.ovrflowlen = retval;
#endif
							nmea_start_pos = extra;  // save position for next iteration
						}
					retval = 1;  // at this point we have the complete NMEA sentence in out[]
				}
		}

	return retval;
}

// check basic NMEA sentence validity and return 1 if true, 0 otherwise
static uint8_t ValidateNMEA(uint8_t *out)
{
	uint16_t len = (uint8_t) strlen((const char*) out);  // first figure out the length
	uint8_t fail = 0;  // failure flag for basic checks below

	// check for valid length
	if(len > GPS_OUT_BUFFER_LEN || len == 0)  // too long/too short - most likely not valid
		fail = 1;

	// check for existing checksum
	if(out[len - 5] != '*')  // no checksum field
		fail = 1;

	// check for correct checksum
	if(checksumMismatch((const char*) &out[1], (uint8_t) len - 6))  // advance start to the first checksum character and pass on
		fail = 1;

	if(fail)
		{
			memset(out, '\0', GPS_OUT_BUFFER_LEN);	// zero the buffer
			return 0;
		}
	else
		{
#if DEBUG_LWRB_FREE
	__ORG1510MK4.char_written += len;
#endif
			return 1;  // if we got until here, the input sentence is valid
		}
}

// UART RX ISR driven - loads incoming data from DMA into a ringbuffer and parses for NMEA sentences
static void _Parse(uint16_t high_pos)
{
	// high_pos indicates the position in the circular DMA reception buffer until which data is available
	// it is NOT the length of new data

	LoadRingBuffer(&uart1_gps_rx_rb, high_pos);  // buffer incoming DMA data into ringbuffer

	if(LoadNMEA(&uart1_gps_rx_rb, _GPS_out))  // if GPS_out has a complete NMEA sentence
		{
			if(ValidateNMEA(_GPS_out))  // check for basic NMEA sentence validity
				{
					// at this point we have a good sentence and we can start parsing NMEA data
#if PARSE_PMTK
					ParsePMTK(__ORG1510MK4.pmtk, (const char*) _GPS_out);  // parse for PMTK messages
#endif
#if PARSE_RMC
					ParseRMC(__ORG1510MK4.public.rmc, (const char*) _GPS_out);  // parse for RMC data
#endif
#if PARSE_GLL
					ParseGLL(__ORG1510MK4.public.gll, (const char*) _GPS_out);  // parse for GLL data
#endif
#if PARSE_VTG
					ParseVTG(__ORG1510MK4.public.vtg, (const char*) _GPS_out);  // parse for VTG data
#endif
#if PARSE_GGA
					ParseGGA(__ORG1510MK4.public.gga, (const char*) _GPS_out);  // parse for GGA data
#endif
#if PARSE_ZDA
					ParseZDA(__ORG1510MK4.public.zda, (const char*) _GPS_out);  // parse for ZDA data
#endif
#if PARSE_GSV && PARSE_GSA
#if EXPOSE_GSV
					ParseGNGSV(__ORG1510MK4.public.gpgsv, "GPGSV", _GPS_out);  // parse GPGSV
					ParseGNGSV(__ORG1510MK4.public.glgsv, "GLGSV", _GPS_out);  // parse GLGSV
#else
					ParseGNGSV(&_gpgsv, "GPGSV", (const char*) _GPS_out);  // parse GPGSV
					ParseGNGSV(&_glgsv, "GLGSV", (const char*) _GPS_out);  // parse GLGSV
#endif
#endif
#if PARSE_GSA
					ParseGSA(__ORG1510MK4.public.gsa, (const char*) _GPS_out);  // parse for GSA data
#endif
				}
		}
}

// writes a NEMA sentence to the GPS module
// 	format is "PMTK313,1" - i.e. no $, checksum and no "\r\n"
static void _Write(const char *str)
{
	char outstr[GPS_OUT_BUFFER_LEN] = "\0";  // buffer for assembling the output string
	uint8_t len = (uint8_t) strlen(str);	// length of incoming string

	if(len > GPS_OUT_BUFFER_LEN - 5)	// invalid length: 82 - 2 (delimiters) - 3 (checksum) = 77
		return;  // invalid length, get out

	if(len < 6)  // (almost) no string: $*00\r\n
		return;  // get out

	sprintf(outstr, "$%s*%02X\r\n", str, calculate_checksum(str, len));  // assemble the raw NEMA command w. prefix, checksum and delimiters

	while(HAL_UART_Transmit_IT(__ORG1510MK4.uart_gps, (const uint8_t*) outstr, (uint16_t) strlen(outstr)) != HAL_OK)
		;  // send assembled string to GPS module & wait for completion

	_wait(50);	// always wait a while. stuff works better that way...
}

#if PARSE_GSV
// asynchronous flag_alm_eph_query for SV almanac & ephemeris
void _Alm_Eph_query(void)
{
	if(__ORG1510MK4.public.FlagQueryAlmandEph)
		{
			char cmnd[13] = "\0";

			for(uint8_t i = 0; i < 12; i++)
				{
					if(_gpgsv.sv[i].prn == 0)
						break;

					sprintf(cmnd, "PMTK474,%u", _gpgsv.sv[i].prn);  // flag_alm_eph_query GPS almanac for PRN
					__ORG1510MK4.public.Write(cmnd);
					sprintf(cmnd, "PMTK473,%u", _gpgsv.sv[i].prn);  // flag_alm_eph_query GPS ephemeris for PRN
					__ORG1510MK4.public.Write(cmnd);
				}

			for(uint8_t i = 0; i < 12; i++)
				{
					if(_gpgsv.sv[i].prn == 0)
						break;

					sprintf(cmnd, "PMTK478,%u", _glgsv.sv[i].prn);  // flag_alm_eph_query GLONASS almanac for PRN
					__ORG1510MK4.public.Write(cmnd);
					sprintf(cmnd, "PMTK477,%u", _glgsv.sv[i].prn);  // flag_alm_eph_query GLONASS ephemeris for PRN
					__ORG1510MK4.public.Write(cmnd);
				}

			__ORG1510MK4.public.Write("PMTK661,1");  // query which SVs will have a valid almanac in 1s

			__ORG1510MK4.public.FlagQueryAlmandEph = 0;
		}
}
#endif

static __org1510mk4_t __ORG1510MK4 =  // instantiate org1510mk4_t actual and set function pointers
	{  //
	.public.Power = &_Power,	// GPS module power mode change control function
	.public.Parse = &_Parse,	// parses incoming NMEA sentences
	.public.Write = &_Write,  // writes a NEMA sentence to the GPS module
#if PARSE_GSV
	.public.AlmEphQuery = &_Alm_Eph_query,  // asynchronous flag_alm_eph_query for SV almanac & ephemeris
#endif
	};

static print_nmea_t _print =	// instantiate & initialize print_nmea_t struct
	{  //
#if PARSE_PMTK
	.pmtk = PARSE_PMTK,  // see #define's in .h
	.pmtk_001 = PRINT_PMTK_001,  //
	.pmtk_010 = PRINT_PMTK_010,  //
	.pmtk_011 = PRINT_PMTK_011,  //
	.pmtk_710 = PRINT_PMTK_710,  //
	.pmtk_711 = PRINT_PMTK_711,  //
	.pmtk_668 = PRINT_PMTK_668,  //
#endif
	.gga = PARSE_GGA,  //
	.gll = PARSE_GLL,  //
	.gsa = PARSE_GSA,  //
	.gsv = PARSE_GSV,  //
	.rmc = PARSE_RMC,  //
	.vtg = PARSE_VTG,  //
	.zda = PARSE_ZDA,  //
	};

org1510mk4_t* org1510mk4_ctor(UART_HandleTypeDef *gps, UART_HandleTypeDef *sys)  //
{
#if PARSE_PMTK
	__ORG1510MK4.public.AlmanacFlags = &_pmtk661;  // SV PRN flags with valid alamanac (PRN 1 is leftmost)
#endif
#if PARSE_GSV
	__ORG1510MK4.public.FlagQueryAlmandEph = 0;  // flag for running AlmEphQuery()
	__ORG1510MK4.public.FlagLocationSeeded = 0;  // flag indicating that PMTK741 was sent
#endif
	__ORG1510MK4.public.FlagTimeAccurate = 0;  // on startup, flag time as inaccurate
	__ORG1510MK4.public.print = &_print;	// tie in printout flag struct
	__ORG1510MK4.uart_gps = gps;  // store GPS module UART object
	__ORG1510MK4.uart_sys = sys;  // store system UART object
	__ORG1510MK4.public.NMEA = _GPS_out;  // tie in NMEA sentence buffer
	__ORG1510MK4.public.PowerMode = 0;  // TODO - read from FeRAM, for now set to off by default

#if PARSE_RMC || PARSE_ZDA || PARSE_GGA
	__ORG1510MK4.public.time = &_time;	// tie in time struct
	__ORG1510MK4.public.date = &_date;	// tie in date struct
#endif
#if PARSE_GGA
	__ORG1510MK4.public.gga = &_gga;	// tie in GGA sentence struct
	__ORG1510MK4.public.gga->lat = &_lat;  // tie in latitude
	__ORG1510MK4.public.gga->lat_dir = &_cd;  // cardinal direction of latitude
	__ORG1510MK4.public.gga->lon = &_lon;  // tie in longitude
	__ORG1510MK4.public.gga->lon_dir = &_cd;  // cardinal direction of longitude
	__ORG1510MK4.public.gga->time = &_time;  // tie in container for GGA-derived fix date
#endif
#if PARSE_ZDA
	__ORG1510MK4.public.zda = &_zda;	// tie in ZDA sentence struct
	__ORG1510MK4.public.zda->time = &_time;  // tie in container for ZDA-derived UTC time
	__ORG1510MK4.public.zda->date = &_date;  // tie in container for ZDA-derived date
	__ORG1510MK4.public.zda->tz = 0;	// initialize to 0
#endif
#if PARSE_VTG
	__ORG1510MK4.public.vtg = &_vtg;	// tie in VTG sentence struct
#endif
#if PARSE_GSA && !PARSE_GSV
	__ORG1510MK4.public.gpgsa = &_gpgsa;	// tie in GSA sentence struct
	__ORG1510MK4.public.glgsa = &_glgsa;	// tie in GSA sentence struct
#endif
#if PARSE_GSV && EXPOSE_GSV
	__ORG1510MK4.public.gpgsv = &_gpgsv;	// tie in GSA sentence struct
	__ORG1510MK4.public.glgsv = &_glgsv;	// tie in GSA sentence struct
#endif
#if PARSE_GSA
	__ORG1510MK4.public.gsa = &_gsa;	// tie in GSA sentence struct
#endif
#if PARSE_RMC
	__ORG1510MK4.public.rmc = &_rmc;	// tie in GSA sentence struct
	__ORG1510MK4.public.rmc->lat = &_lat;  // tie in latitude
	__ORG1510MK4.public.rmc->lat_dir = &_cd;  // cardinal direction of latitude
	__ORG1510MK4.public.rmc->lon = &_lon;  // tie in longitude
	__ORG1510MK4.public.rmc->lon_dir = &_cd;  // cardinal direction of longitude
	__ORG1510MK4.public.rmc->date = &_date;	// tie in date struct
	__ORG1510MK4.public.rmc->time = &_time;	// tie in time struct
#endif
#if PARSE_GLL
	__ORG1510MK4.public.gll = &_gll;	// tie in GLL sentence struct
	__ORG1510MK4.public.gll->lat = &_lat;
	__ORG1510MK4.public.gll->lat_dir = &_cd;  // cardinal direction of latitude
	__ORG1510MK4.public.gll->lon = &_lon;
	__ORG1510MK4.public.gll->lon_dir = &_cd;  // cardinal direction of longitude
	__ORG1510MK4.public.gll->time = &_time;	// tie in time struct
#endif
#if PARSE_PMTK
	__ORG1510MK4.pmtk = &_pmtk;  // tie in PMTKL message struct
	__ORG1510MK4.pmtk->buff = _pmtk_buff;  // tie in PMTK packet buffer container
#endif
#if DEBUG_LWRB_FREE
	__ORG1510MK4.char_written = 0;	// characters written out to system UART
#endif

	lwrb_init(&uart1_gps_rx_rb, uart1_gps_rx_rb_buffer, sizeof(uart1_gps_rx_rb_buffer));

	// TODO - move rx_start() into _Power()
	rx_start();  // start DMA reception

//	_init();  // initialize the module
//	__ORG1510MK4.public.Power(wakeup);  // wake the module

	return &__ORG1510MK4.public;  // set pointer to ORG1510MK4 public part
}

org1510mk4_t *const ORG1510MK4 = &__ORG1510MK4.public;  // set pointer to ORG1510MK4 public part

#endif
