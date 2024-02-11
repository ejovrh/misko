#include "org1510mk4/org1510mk4.h"

#if defined(USE_ORG1510MK4)	// if this particular device is active
#include <stdio.h>
#include <string.h>

#include "lwrb\lwrb.h"

extern ADC_HandleTypeDef hadc1;  // TODO - move out of here
extern volatile uint32_t __adc_dma_buffer[ADC_CHANNELS];  // store for ADC readout
extern volatile uint32_t __adc_results[ADC_CHANNELS];  // store ADC average data

typedef struct	// org1510mk4c_t actual
{
	UART_HandleTypeDef *uart_gps;  // HAL UART instance over which to communicate with the GPS module
	UART_HandleTypeDef *uart_sys;  // HAL UART instance over which the whole device communicates with a host computer/VCP
	volatile org1510mk4_power_t currentPowerMode;  // current power mode of the GPS module

	org1510mk4_t public;  // public struct
} __org1510mk4_t;

static __org1510mk4_t __ORG1510MK4 __attribute__ ((section (".data")));  // preallocate __ORG1510MK4 object in .data

#define DIRTY_POWER_MODE_CHANGE 0	// circumvents power mode change safeguards to e.g. deliberately drain the capacitor
#define NMEA_BUFFER_LEN 64	// officially, a NMEA sentence (from $ to \n) is 80 characters long. 2 more to account for \r\n

static uint8_t _NMEA[NMEA_BUFFER_LEN];  // NMEA incoming buffer

#define LWRB_BUFFER_LEN 1024
lwrb_t lwrb;
uint8_t lwrb_buffer[LWRB_BUFFER_LEN];

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
	 * 5 GSA - GNSS DOP and Active Satellites
	 * 0 GSV - GNSS Satellites in View
	 * ...
	 * 1 ZDA - UTC Date/Time and Local Time Zone Offset
	 * 0 MCHN - ???
	 */
	__ORG1510MK4.public.Write("PMTK314,0,0,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,1,0");  //
//	__ORG1510MK4.public.Write("PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0");  //

	//
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

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.currentPowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.currentPowerMode > backup)	// if the module is in some operating mode
				__ORG1510MK4.public.Power(backup);	// first go into backup mode

			if(__ORG1510MK4.currentPowerMode <= backup)  // if the module is in backup mode (or less)
				HAL_GPIO_WritePin(SUPERCAP_EN_GPIO_Port, SUPERCAP_EN_Pin, GPIO_PIN_RESET);	// then turn off supercap charger

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == on)  // supply power on & module (possibly) in backup mode
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_GPIO_WritePin(SUPERCAP_EN_GPIO_Port, SUPERCAP_EN_Pin, GPIO_PIN_SET);	// power on the supercap charger

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.currentPowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.currentPowerMode < wakeup)  // if the module was powered off
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

							__ORG1510MK4.currentPowerMode = state;	// save the current power mode
							return;
						}
				}
#endif
		}

	if(state == backup)  // "backup mode", DS. ch. 4.3.15
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK225,4*2F\r\n", 15);  // send backup mode command

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.currentPowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.currentPowerMode > backup)  // if the module is in some operating mode
				{
					__ORG1510MK4.public.Write("PMTK225,4");  // send backup mode command

					// FIXME - sometimes this is blocking
					while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin))
						;						// wait until the wakeup pin goes low
				}

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == wakeup)  // return to full power mode
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_GPIO_WritePin(GPS_PWR_CTRL_GPIO_Port, GPS_PWR_CTRL_Pin, GPIO_PIN_SET);	// set high
			_wait(1000);	 // wait 1s
			HAL_GPIO_WritePin(GPS_PWR_CTRL_GPIO_Port, GPS_PWR_CTRL_Pin, GPIO_PIN_RESET);	// set low

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.currentPowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.currentPowerMode == off)  // if the module is powered off
				__ORG1510MK4.public.Power(on);	// first power on,

			if(__ORG1510MK4.currentPowerMode == on)  // if the module is powered on
				__ORG1510MK4.currentPowerMode = backup;  // then cheat the mode into backup

			if(__ORG1510MK4.currentPowerMode == backup)  // if in backup mode
				{
					// get out of backup: ("off" state can also be a backup state)
					HAL_GPIO_WritePin(GPS_PWR_CTRL_GPIO_Port, GPS_PWR_CTRL_Pin, GPIO_PIN_SET);	// set high
					_wait(1000);	 // wait 1s
					HAL_GPIO_WritePin(GPS_PWR_CTRL_GPIO_Port, GPS_PWR_CTRL_Pin, GPIO_PIN_RESET);	// set low

					// FIXME - sometimes this is blocking
					while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin) == GPIO_PIN_RESET)
						;						// wait until the wakeup pin goes high
				}

			if(__ORG1510MK4.currentPowerMode > wakeup)  // if the module is in a lighter sleep state
				{
					while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin) == GPIO_PIN_RESET)
						;						// wait until the wakeup pin goes high

					__ORG1510MK4.public.Write("PMTK225,0");  // wakeup command - transit into full power mode
				}

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == standby)  // standby mode, DS. ch. 4.3.12
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK161,0*28\r\n", 15);  // then go into standby

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.currentPowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.currentPowerMode < wakeup)  // if the module is not awake
				return;  // do nothing

			__ORG1510MK4.public.Write("PMTK161,0");  // send standby sleep command

			// FIXME - sometimes this is blocking
			while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin))
				;						// wait until the wakeup pin goes low

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == periodic)  // periodic mode, DS. ch. 4.3.13
		{
#if DIRTY_POWER_MODE_CHANGE
			// TODO - implement periodic mode

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.currentPowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.currentPowerMode < wakeup)  // if the module is not awake
				return;  // do nothing

			// TODO - implement periodic mode

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == alwayslocate)  // alwaysLocate mode, DS. ch. 4.3.14
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK225,9*22\r\n", 15);  // DS. ch. 4.3.14

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#else
			if(__ORG1510MK4.currentPowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.currentPowerMode < wakeup)  // if the module is not awake
				return;  // do nothing

			__ORG1510MK4.public.Write("PMTK225,9");  // send command for AlwaysLocate backup mode

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
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
			if(__ORG1510MK4.currentPowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.currentPowerMode == backup)  // if the module is in backup
				__ORG1510MK4.public.Power(wakeup);	// first, wake up

			if(__ORG1510MK4.currentPowerMode == off)  // if the module is powered off
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
				;						// wait until the wakeup pin goes high

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
void rx_start(void)
{
	if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(__ORG1510MK4.uart_gps, _NMEA, NMEA_BUFFER_LEN))  // start reception)
		Error_Handler();
}

// transfer data from circular DMA RX buffer into ringbuffer
// 	double buffering
void _Parse(uint16_t pos)
{
	static uint16_t old_pos;

	if(pos != old_pos)
		{
			if(pos > old_pos)
				{
					lwrb_write(&lwrb, &_NMEA[old_pos], pos - old_pos);
				}
			else
				{
					lwrb_write(&lwrb, &_NMEA[old_pos], NMEA_BUFFER_LEN - old_pos);

					if(pos > 0)
						{
							lwrb_write(&lwrb, &_NMEA[0], pos);
						}
				}
			old_pos = pos;
		}

	HAL_UART_Transmit_DMA(__ORG1510MK4.uart_sys, _NMEA, (uint16_t) strlen((const char*) _NMEA));  // send GPS to VCP
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
	__ORG1510MK4.public.NMEA = _NMEA;  // tie in NMEA sentence buffer
	__ORG1510MK4.currentPowerMode = 0;  // TODO - read from FeRAM, for now set to off by default

	lwrb_init(&lwrb, lwrb_buffer, sizeof(lwrb_buffer));

	rx_start();  // start DMA reception

	//	_init();  // initialize the module
	__ORG1510MK4.public.Power(wakeup);  // wake the module

	return &__ORG1510MK4.public;  // set pointer to ORG1510MK4 public part
}

org1510mk4_t *const ORG1510MK4 = &__ORG1510MK4.public;  // set pointer to ORG1510MK4 public part

#endif
