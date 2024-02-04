#include "org1510mk4/org1510mk4.h"

#if defined(USE_ORG1510MK4)	// if this particular device is active

extern ADC_HandleTypeDef hadc1;  // TODO - move out of here
extern volatile uint32_t __adc_dma_buffer[ADC_CHANNELS];  // store for ADC readout
extern volatile uint32_t __adc_results[ADC_CHANNELS];  // store ADC average data

typedef struct	// org1510mk4c_t actual
{
	volatile org1510mk4_power_t currentPowerMode;  // current power mode of the GPS module

	org1510mk4_t public;  // public struct
} __org1510mk4_t;

static __org1510mk4_t __ORG1510MK4 __attribute__ ((section (".data")));  // preallocate __ORG1510MK4 object in .data

extern UART_HandleTypeDef huart1;  //

#define DIRTY_POWER_MODE_CHANGE 0	// circumvents power mode change safeguards to e.g. deliberately drain the capacitor
#define NMEA_BUFFER_LEN 82	// officially, a NMEA sentence is 80 characters long. 2 more to account for \r\n

static uint8_t _NMEA[NMEA_BUFFER_LEN];  // NMEA incoming buffer

// all NMEA off: 	$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28
// NMEA RMC 5s: 	$PMTK314,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2D
// normal NMEA: $PMTK314,1,1,1,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0*2C
// firmware info: $PMTK605*31
//	output: $PMTK705,AXN_3.8_3333_16042118,0000,V3.8.1 GP+GL,*6F

// disable 1PPS: $PMTK255,1*2D
// 	also: $PMTK285,0,0*3D
// enable active interference cancellation: $PMTK286,1*23
// pedestrian mode: $PMTK886,1*29 (slower than 5m/s)
// vehicle mode: $PMTK886,0*28 (faster than 5m/s)
// set WGS84 datum: $PMTK330,0*2E
// use gps, glonass, galileo, not galileo_full, beidou: $PMTK353,1,1,1,0,1*2B (action failed - test)
// stop LOCUS logging: $PMTK185,1*23
// enable SBAS: $PMTK355*31

// workaround for a blocking issue
static inline void _wait(uint32_t i)
{
	while(i--)
		;
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
					HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK225,4*2F\r\n", 15);  // send backup mode command

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
			_wait(8000000);	 // wait 1s
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
					_wait(8000000);	 // wait 1s
					HAL_GPIO_WritePin(GPS_PWR_CTRL_GPIO_Port, GPS_PWR_CTRL_Pin, GPIO_PIN_RESET);	// set low

					while(HAL_GPIO_ReadPin(GPS_WKUP_GPIO_Port, GPS_WKUP_Pin) == GPIO_PIN_RESET)
						;						// wait until the wakeup pin goes high
				}

			if(__ORG1510MK4.currentPowerMode > wakeup)  // if the module is in a lighter sleep state
				HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK225,0*2B\r\n", 15);  // wakeup command - transit into full power mode

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

			HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK161,0*28\r\n", 15);  // then go into standby

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

			HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK225,9*22\r\n", 15);  // DS. ch. 4.3.14

			__ORG1510MK4.currentPowerMode = state;	// save the current power mode
			return;
#endif
		}

	if(state == reset)  // reset the module
		{
#if DIRTY_POWER_MODE_CHANGE
			HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_RESET);  // take GPS module into reset
			_wait(4000000);  // wait 500ms
			HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_SET);  // take GPS module out of reset
#else
			if(__ORG1510MK4.currentPowerMode == state)  // if the module is already in this state
				return;  // do nothing

			if(__ORG1510MK4.currentPowerMode == backup)  // if the module is in backup
				__ORG1510MK4.public.Power(wakeup);	// first, wake up

			if(__ORG1510MK4.currentPowerMode == off)  // if the module is in backup
				{
					__ORG1510MK4.public.Power(on);	// first, turn on
					__ORG1510MK4.public.Power(wakeup);	// then, wake up
				}

			// finally: reset
			HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_RESET);  // take GPS module into reset
			_wait(4000000);  // wait 500ms
			HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_SET);  // take GPS module out of reset

			__ORG1510MK4.public.Power(wakeup);	// then, wake up
			_wait(4000000);  // wait 500ms
			return;
#endif
		}
}

//
static void _Read(void)
{
	;
}

//
static void _Write(void)
{
	;
}

static __org1510mk4_t __ORG1510MK4 =  // instantiate org1510mk4_t actual and set function pointers
	{  //
	.public.Power = &_Power,	// GPS module power mode change control function
	.public.Read = &_Read,	//
	.public.Write = &_Write  //
	};

org1510mk4_t* org1510mk4_ctor(void)  //
{
	__ORG1510MK4.public.NMEA = _NMEA;  // tie in NMEA sentence buffer
	__ORG1510MK4.currentPowerMode = 0;	// set to off by default

//	__ORG1510MK4.public.Power(wakeup);
//	__ORG1510MK4.public.Power(reset);
//	HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK314,1,1,1,1,1,5,0,0,0,0,0,0,0,0,0,0,0,0,0*2C\r\n", 51);  // DS. ch. 4.3.14
	HAL_UART_Transmit_DMA(&huart1, (const uint8_t*) "$PMTK314,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*2D", 51);

	HAL_UART_Receive_DMA(&huart1, _NMEA, 82);

	return &__ORG1510MK4.public;  // set pointer to ORG1510MK4 public part
}

org1510mk4_t *const ORG1510MK4 = &__ORG1510MK4.public;  // set pointer to ORG1510MK4 public part

#endif
