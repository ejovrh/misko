#include "org1510mk4/org1510mk4.h"

#if defined(USE_ORG1510MK4)	// if this particular device is active
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lwrb\lwrb.h"	// Lightweight RingBuffer - https://docs.majerle.eu/projects/lwrb/en/latest/index.html

#define DIRTY_POWER_MODE_CHANGE 0	// circumvents power mode change safeguards to e.g. deliberately drain the capacitor
#define DEBUG_LWRB_FREE 0 // data member indicating LwRB free space

#define GPS_DMA_INPUT_BUFFER_LEN 32	// officially, a NMEA sentence (from $ to \n) is 80 characters long. 2 more to account for \r\n
#define LWRB_BUFFER_LEN 256	// LwRB buffer size

extern ADC_HandleTypeDef hadc1;  // TODO - move out of here
extern volatile uint32_t __adc_dma_buffer[ADC_CHANNELS];  // TODO - move out of here - store for ADC readout
extern volatile uint32_t __adc_results[ADC_CHANNELS];  // TODO - move out of here - store ADC average data

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

	org1510mk4_t public;  // public struct
} __org1510mk4_t;

static __org1510mk4_t __ORG1510MK4 __attribute__ ((section (".data")));  // preallocate __ORG1510MK4 object in .data

static lwrb_t lwrb;  // 2nd circular buffer for data processing
static uint8_t lwrb_buffer[LWRB_BUFFER_LEN];	//
static char _UTCtimestr[7];  // container for ZDA-derived UTC time
static char _GGAfix_date[7];  // container for GGA-derived fix date
static gnzda_t _zda;  // object for GNZDA data
static gngga_t _gga;  // object for GNGGA data
static uint8_t parse_complete;	// semaphore for parsing <-> ringbuffer load control
static uint8_t gps_dma_input_buffer[GPS_DMA_INPUT_BUFFER_LEN];  // 1st circular buffer: incoming GPS UART DMA data

// all NMEA off: 	PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
// NMEA RMC 5s: 	PMTK314,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
// normal NMEA: PMTK314,1,1,1,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0
// firmware info: PMTK605*31
//	output: $PMTK705,AXN_3.8_3333_16042118,0000,V3.8.1 GP+GL,*6F

// $PMTK414*33 !!! will give $PMTK514,0,0,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0*36\r\n (which was set in _init()

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

			*ptr++;  // @suppress("Statement has no effect")
		}

	return (checksum == checksumInSentence) ? 0 : 1;	// compare the two sums
}

// init function for GPS module
static void _init(void)
{
//	__ORG1510MK4.public.Power(wakeup);	// power up & bring into normal mode
	__ORG1510MK4.public.Write("PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");  // shut the damn thing off first

	__ORG1510MK4.public.Write("PMTK330,0");  // set WGS84 datum
	__ORG1510MK4.public.Write("PMTK185,1");  // stop LOCUS logging
	__ORG1510MK4.public.Write("PMTK355"); 	// enable SBAS
	__ORG1510MK4.public.Write("PMTK301,2");  // set DGPS to SBAS
	__ORG1510MK4.public.Write("PMTK286,1");  // enable active interference cancellation
	__ORG1510MK4.public.Write("PMTK356,0");  // disable HDOP theshold
	__ORG1510MK4.public.Write("PMTK386,0");  // disable speed threshold for static navigation
	__ORG1510MK4.public.Write("PMTK255,0"); 	// disable 1PPS
	__ORG1510MK4.public.Write("PMTK285,0,0"); 	// 	also disable 1PPS

	__ORG1510MK4.public.Write("PMTK886,1");  // pedestrian mode  (slower than 5m/s)
//	__ORG1510MK4.public.Write("PMTK886,0"); // vehicle mode (faster than 5m/s)
	__ORG1510MK4.public.Write("PMTK353,1,1,0,0,1");  // use gps, glonass, not galileo, not galileo_full, beidou (action failed - test)
	__ORG1510MK4.public.Write("PMTK353,1,1,1,0,1");  // use gps, glonass, galileo, not galileo_full, beidou (action failed - test)

	/* NMEA sentences:
	 * 0 GLL - Geographical Position-Latitude/Longitude
	 * 0 RMC - Recommended Minimum Specific GNSS Data
	 * 1 VTG - Course over Ground and Ground Speed
	 * 1 GGA - Global Positioning System Fix Data
	 * 10 GSA - GNSS DOP and Active Satellites
	 * 10 GSV - GNSS Satellites in View
	 * ...
	 * 1 ZDA - UTC Date/Time and Local Time Zone Offset
	 * 0 MCHN - ???
	 */
	// instruct module to spit out NMEA sentences as above
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

							while(__adc_results[Vgps] < 3000)  // wait until the supply voltage is high enough (or the module is awake)
								{
									break;  // FIXME - blocks ADC somehow
									;
								}

							__ORG1510MK4.public.PowerMode = state;	// save the current power mode
							return;
						}
				}
#endif
		}

	if(state == backup)  // "backup mode", DS. ch. 4.3.15
		{  // FIXME - module seems to reset when entering/exiting backup mode
#if DIRTY_POWER_MODE_CHANGE
			HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK225,4*2F\r\n", 15);  // send backup mode command

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode > backup)  // if the module is in some operating mode
				{
					__ORG1510MK4.public.Write("PMTK225,4");  // send backup mode command

					// FIXME - sometimes this is blocking
					while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin))
						;  // wait until the wakeup pin goes low
				}

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == wakeup)  // return to full power mode
		{  // FIXME - module seems to be reset when transitioning into wakeup
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

					// FIXME - sometimes this is blocking
					while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin) == GPIO_PIN_RESET)
						;  // wait until the wakeup pin goes high
				}

			if(__ORG1510MK4.public.PowerMode > wakeup)  // if the module is in a lighter sleep state
				{
					// FIXME - blocks when transitioning from standby to wake
					while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin) == GPIO_PIN_RESET)
						;  // wait until the wakeup pin goes high

					__ORG1510MK4.public.Write("PMTK225,0");  // wakeup command - transit into full power mode
				}

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == standby)  // standby mode, DS. ch. 4.3.12
		{  // FIXME - entering standby seems to reset the module
#if DIRTY_POWER_MODE_CHANGE
			HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK161,0*28\r\n", 15);  // then go into standby

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode < wakeup)  // if the module is not awake
				return;  // do nothing

			__ORG1510MK4.public.Write("PMTK161,0");  // send standby sleep command

			// FIXME - sometimes this is blocking
			while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin))
				;  // wait until the wakeup pin goes low

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == periodic)  // periodic mode, DS. ch. 4.3.13
		{
#if DIRTY_POWER_MODE_CHANGE
			// TODO - implement periodic mode

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.public.PowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.public.PowerMode < wakeup)  // if the module is not awake
				return;  // do nothing

			// TODO - implement periodic mode

			__ORG1510MK4.public.PowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == alwayslocate)  // alwaysLocate mode, DS. ch. 4.3.14
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK225,9*22\r\n", 15);  // DS. ch. 4.3.14

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

			// FIXME - sometimes this is blocking
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
}

//
static void _Read(void)
{
	__ORG1510MK4.public.Write("PMTK414");

}

//
static void rx_start(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(__ORG1510MK4.uart_gps, gps_dma_input_buffer, GPS_DMA_INPUT_BUFFER_LEN);

//	if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(__ORG1510MK4.uart_gps, gps_dma_input_buffer, GPS_DMA_INPUT_BUFFER_LEN))  // start reception)
//		Error_Handler();
}

// parses NMEA str for ZDA data - time & date
static void parse_zda(const __org1510mk4_t* device, const char *str)
{
	char *zda = strstr(str, "ZDA");  // first, check if ZDA exists in the input string

	if(zda == NULL)  // if not, get out
		return;

	char temp[82] = "\0";  // create a temporary buffer

	strncpy(temp, str, strlen(str));	// copy str into temp

	// $GNZDA,163207.000,15,02,2024,,*4B
	char *token = strtok(temp, ",");	// start to tokenize

	token = strtok(NULL, ",");  // UTC time - 163207.000
	device->public.zda->time = strncpy(device->public.zda->time, token, 6);

	token = strtok(NULL, ",");  // day - 15
	device->public.zda->day = (uint8_t) atoi(token);

	token = strtok(NULL, ",");  // month - 02
	device->public.zda->month = (uint8_t) atoi(token);

	token = strtok(NULL, ",");  // year - 2024
	device->public.zda->year = (uint16_t) atoi(token);

	token = strtok(NULL, ",");  // local timezone offset
	if(token)
		device->public.zda->tz = (uint8_t) atoi(token);
}

// parses str for GGA data - fix indicator, HDOP, satellites used, etc.
static void parse_gga(const __org1510mk4_t* device, const char *str)
{
	char *zda = strstr(str, "GGA");  // first, check if GGA exists in the input string

	if(zda == NULL)  // if not, get out
		return;

	char temp[82] = "\0";  // create a temporary buffer

	strncpy(temp, str, strlen(str));  // copy str into temp

	// $GNGGA,161439.000,4547.8623,N,01554.9327,E,1,5,2.05,104.7,M,42.5,M,,*4E
	char *token = strtok(temp, ",");	// start to tokenize

	token = strtok(NULL, ",");  // UTC of this position report - 161439.000
	device->public.gga->fix_date = strncpy(device->public.gga->fix_date, token, 6);

	token = strtok(NULL, ",");  // latitude - 4547.8623
	token = strtok(NULL, ",");  // north - N
	token = strtok(NULL, ",");  // longitude - 01554.9327
	token = strtok(NULL, ",");  // east - E

	token = strtok(NULL, ",");  // GPS fix indicator - 1
	device->public.gga->fix = atoi(token);

	token = strtok(NULL, ",");  // satellites in use - 5
	if(token)
		device->public.gga->sat_used = (uint8_t) atoi(token);
	else
		device->public.gga->sat_used = 0;

	token = strtok(NULL, ",");  // HDOP - 2.05
	if(token)
		device->public.gga->HDOP = (float) atof(token);
	else
		device->public.gga->HDOP = 0;

	token = strtok(NULL, ",");  // alt - 104.7
	if(token)
		device->public.gga->alt = (float) atof(token);
	else
		device->public.gga->alt = 0;
}

// buffer NMEA sentences into DMA circular buffer
static void load_into_1st_buffer(lwrb_t *rb, const uint16_t high_pos)
{
	static uint16_t low_pos;

	// safeguard against an RX event interrupt where no new data has come in
	if(high_pos == low_pos)  // no new data
		return;

	// 1st buffering - load into ringbuffer
	if(high_pos > low_pos)  // linear region
		// read in a linear fashion from DMA rcpt. buffer from beginning to end of new data
		lwrb_write(rb, &gps_dma_input_buffer[low_pos], high_pos - low_pos);  // (high_pos - low_pos) is length of new data
	else	// overflow region
		{
			// read new data from current position until end of DMA rcpt. buffer
			lwrb_write(&lwrb, &gps_dma_input_buffer[low_pos], GPS_DMA_INPUT_BUFFER_LEN - low_pos);

			// then, if there is more after the rollover
			if(high_pos > 0)
				// read from beginning of circular DMA buffer until the end position of new data
				lwrb_write(rb, &gps_dma_input_buffer[0], high_pos);
		}

	low_pos = high_pos;  // save position until data has been read

#if DEBUG_LWRB_FREE
	__ORG1510MK4.lwrb_free = (uint16_t) lwrb_get_free(&lwrb);  // have free memory value visible

	if(__ORG1510MK4.lwrb_free == 0)  // LwRB memory used up: hang here
		Error_Handler();
#endif
}

// buffer into the 2nd buffer and write out to out[]
static uint8_t load_into_2nd_buffer(lwrb_t *rb, uint8_t *temp, uint8_t *parse_flag)
{
	lwrb_sz_t retval = 0;

	// 2nd buffering into LwRB & load NMEA sentence into out[] for parsing
	static lwrb_sz_t nmea_start_pos;  // position store for NMEA sentence start - not needed at all
	if(lwrb_find(rb, "$", 1, 0, &nmea_start_pos))  // starting from the current read pointer location, find the NMEA sentence start marker
		{
			static lwrb_sz_t nmea_terminator_pos;  // position store for NMEA terminator start position
			if(lwrb_find(rb, "$", 1, nmea_start_pos + 1, &nmea_terminator_pos))  // starting from current read pointer location, find the terminator start
				{
					memset(temp, '\0', 82);  // zero out out-container

					// assemble the complete NMEA sentence
					if(nmea_terminator_pos > nmea_start_pos)  // linear region
						{
							retval = lwrb_read(rb, temp, nmea_terminator_pos - nmea_start_pos);  // the terminators are from the current read pointer len away
							nmea_start_pos = nmea_terminator_pos;  // save position for next iteration

#if DEBUG_LWRB_FREE
							__ORG1510MK4.linearlen = retval;
#endif
						}
					else  // overflow region
						{
							// the terminators are in overflow:
							lwrb_sz_t rest = (lwrb_sz_t) (GPS_DMA_INPUT_BUFFER_LEN - nmea_start_pos);  // from the current read pointer to buffer end
							lwrb_sz_t extra = (lwrb_sz_t) (nmea_terminator_pos - rest);  // then from buffer start some more

							retval += lwrb_read(rb, temp, rest);  // read out len characters into out
							retval += lwrb_read(rb, &temp[retval], extra);  // read out len characters into out
//							temp[retval + 1] = '\0';
#if DEBUG_LWRB_FREE
							__ORG1510MK4.ovrflowlen = retval;
#endif
							nmea_start_pos = extra;  // save position for next iteration
						}
					*parse_flag = 0;  // at this point we have the complete NMEA sentence in out[]
				}
		}

	return (uint8_t) retval;
}

// check basic NMEA sentence validity and return 1 if true, 0 otherwise
static uint8_t NMEA_sentence_valid(uint8_t *out, uint8_t *prase_flag)
{
	uint8_t len = (uint8_t) strlen((const char*) out);  // first figure out the length
	uint8_t fail = 0;  // failure flag for basic checks below

	// check for valid length
	if(len > 82 || len == 0)  // too long/too short - most likely not valid
		fail = 1;

	// check for existing checksum
	if(out[len - 5] != '*')  // no checksum field
		fail = 1;

	// check for correct checksum
	if(checksumMismatch((const char*) &out[1], (uint8_t) len - 6))  // advance start to the first checksum character and pass on
		fail = 1;

	if(fail)
		{
			*prase_flag = 1;  // get out
			memset(out, '\0', 82);	// zero the buffer
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

// loads incoming NMEA string from DMA into a buffer and parses it
static void _Parse(uint16_t high_pos)
{
	// high_pos indicates the position in the circular DMA reception buffer until which data is available
	// it is NOT the length of new data

	static uint8_t out[82];  // output box for GPS UART's DMA

	load_into_1st_buffer(&lwrb, high_pos);	// buffer incoming NMEA sentences into circular DMA buffer

	if(parse_complete)
		{
			load_into_2nd_buffer(&lwrb, out, &parse_complete);  // buffer into the 2nd buffer and write out to out[]
		}
	else
		{
			if(NMEA_sentence_valid(out, &parse_complete))  // check basic NMEA sentence validity
				{
					// at this point we have a good sentence and we can start parsing NMEA data
					parse_gga(&__ORG1510MK4, (const char*) out);	// parse for GGA data
					parse_zda(&__ORG1510MK4, (const char*) out);	// parse for ZDA data

					HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, out, (uint16_t) strlen((const char*) out));  // send GPS to VCP

					parse_complete = 1;
				}
		}
}

// writes a NEMA sentence to the GPS module
// 	format is "PMTK313,1" - i.e. no $, checksum and no "\r\n"
static void _Write(const char *str)
{
	char outstr[82] = "\0";  // buffer for assembling the output string
	uint8_t len = (uint8_t) strlen(str);	// length of incoming string

	if(len > 77)	// invalid length: 82 - 2 (delimiters) - 3 (checksum) = 77
		return;  // invalid length, get out

	sprintf(outstr, "$%s*%02X\r\n", str, calculate_checksum(str, len));  // assemble the raw NEMA command w. prefix, checksum and delimiters

	while(HAL_UART_Transmit_IT(__ORG1510MK4.uart_gps, (const uint8_t*) outstr, (uint16_t) strlen(outstr)) != HAL_OK)
		;  // send assembled string to GPS module & wait for completion

	_wait(50);	// always wait a while. stuff works better that way...
}

static __org1510mk4_t __ORG1510MK4 =  // instantiate org1510mk4_t actual and set function pointers
	{  //
	.public.Power = &_Power,	// GPS module power mode change control function
	.public.Parse = &_Parse,	//
	.public.Read = &_Read,	//
	.public.Write = &_Write  // writes a NEMA sentence to the GPS module
	};

org1510mk4_t* org1510mk4_ctor(UART_HandleTypeDef *gps, UART_HandleTypeDef *sys)  //
{
	__ORG1510MK4.uart_gps = gps;  // store GPS module UART object
	__ORG1510MK4.uart_sys = sys;  // store system UART object
//	__ORG1510MK4.public.NMEA = gps_dma_input_buffer;  // tie in NMEA sentence buffer
	__ORG1510MK4.public.PowerMode = 0;  // TODO - read from FeRAM, for now set to off by default

	__ORG1510MK4.public.gga = &_gga;	// tie in GNGGA-derived struct
	__ORG1510MK4.public.gga->fix_date = _GGAfix_date;  // tie in container for GGA-derived fix date

	__ORG1510MK4.public.zda = &_zda;	// tie in GNZDA-derived struct
	__ORG1510MK4.public.zda->time = _UTCtimestr;  // tie in container for ZDA-derived UTC time
	__ORG1510MK4.public.zda->tz = 0;	// initialize to 0

#if DEBUG_LWRB_FREE
	__ORG1510MK4.char_written = 0;	// characters written out to system UART
#endif
	parse_complete = 1;
	lwrb_init(&lwrb, lwrb_buffer, sizeof(lwrb_buffer));

	// TODO - move rx_start() into _Power()
	rx_start();  // start DMA reception

	//	_init();  // initialize the module
	__ORG1510MK4.public.Power(wakeup);  // wake the module

	return &__ORG1510MK4.public;  // set pointer to ORG1510MK4 public part
}

org1510mk4_t *const ORG1510MK4 = &__ORG1510MK4.public;  // set pointer to ORG1510MK4 public part

#endif
