#ifndef DS1394U_H_
#define DS1394U_H_

#include <inttypes.h>

#define hundreths_s_read	0x00
#define hundreths_s_write	0x80
#define bitmask_hundreths_s 0x0f								// 00-99 BCD
#define bitmask_tens_s	0xf0										// 00-99 BCD

#define seconds_read 0x01
#define seconds_write 0x81
#define bitmask_seconds 0x7f										// 00-59 BCD

#define minutes_read 0x02
#define minutes_write 0x82
#define bitmask_minutes 0x7f										// 00-59 BCD

#define hours_read	0x03
#define hours_write 0x83
#define bitmask_24h_format 0x40									// 0 = 24 format
#define bitmask_hours 0x3f											// 00-23 BCD

#define days_read 0x04
#define days_write 0x84
#define bitmask_days 0x03												// 1-7 BCD

#define date_read 0x05
#define date_write 0x85
#define bitmask_date 0x3f												// 01-31 BCD

#define month_century_read 0x06
#define month_century_write 0x86
#define bitmask_century 0x80										// 0-8 BCD; year-8999 problem, here we come!
#define bitmask_month 0x1f											// 01-12 BCD

#define year_read 0x07
#define year_write 0x87
#define bitmask_year 0xff												// 00-99 BCD

#define alarm_hundreths_s_read 0x08
#define alarm_hundreths_s_write 0x88
#define bitmask_alarm_hundreths_s 0x0f					// 00-99 BCD
#define bitmask_alarm_tens_s	0xf0							// 00-99 BCD

#define alarm_seconds_read 0x09
#define alarm_seconds_write 0x89
#define bitmask_alarm_seconds_am1 0x80					//
#define bitmask_alarm_seconds_ 0x7f							// 00-59 BCD

#define alarm_minutes_read 0x0a
#define alarm_minutes_write 0x8a
#define bitmask_alarm_minutes_am2 0x80					//
#define bitmask_alarm_minutes_minutes 0x7f			// 00-59 BCD

#define alarm_hours_read 0x0b
#define alarm_hours_write 0x8b
#define bitmask_alarm_hours_am3 0x40
#define bitmask_alarm_hours_24h_format 0x40			// 0 = 24h format
#define bitmask_alarm_hours_hours 0x3f					// 00-23 BCD

#define alarm_day_date_read 0x0c
#define alarm_day_date_write 0x8c
#define bitmask_alarm_day_date_read_am4 0x80		//
#define bitmask_alarm_day_date_read_dy_dt 0x40	// 0 - day, 1 - date
#define bitmask_alarm_day_date_read_date 0x3f		// 00-31 BCD
#define bitmask_alarm_day_date_read_day 0x0f		// 1-7 BCD

#define control_read 0x0d
#define control_write 0x8d											// datasheet p. 17
#define eosc 7																	// enable oscillator on Vbackup; 0 - started, 1 - stopped
#define bbsqi 5																	// battery backed square wave and interrupt enable; 0 - HIGHZ, 1 - enabled
#define rs2 4																		// rate select
#define rs1 3																		// rate select
#define intcn 2																	// interrupt control; 0 - sqw. out, 1 - int. out
#define aie 0																		// alarm interrupt enable; 0 - INT low, 1 - int. on alarm enabled

#define status_read 0x0e
#define status_write 0x8e												// datesheet p. 18
#define osf 7																		// oscillator stop, 0 - osc running (rw), 1 - osc stopped (value 1 can only be read, not written)
#define af 0																		// alarm flag; 0 = flag gets cleared, 1 - time matches alarm registers

#define charger_read 0x0f
#define charger_write 0x8f											// datasheet p. 18, table 5
#define tcs3 7																	//
#define tcs2 6																	//
#define tcs1 5																	//
#define tcs0 4																	//
#define ds1 3																		//
#define ds0 2																		//
#define rout1 1																	//
#define rout0 0																	//

/* Address registers define */
#define DS1394_REG_100THS               0x00	// Hundredths of Seconds register /0-99 BCD
#define DS1394_REG_SECONDS              0x01	// Seconds register				/00-59 BCD
#define DS1394_REG_MINUTES              0x02	// Minutes register				/00-59 BCD
#define DS1394_REG_HOURS                0x03	// Hours register				/00-23 BCD
#define DS1394_REG_DAY                  0x04	// Day of week register			/1-7 BCD
#define DS1394_REG_DATE                 0x05	// Date register				/01-31 BCD
#define DS1394_REG_MONTH_CENT           0x06	// Month register 				/01-12 BCD + Century bit (bit7)
#define DS1394_REG_YEAR                 0x07	// Year register 				/00-99 BCD

/* Alarm address registers  */
#define DS1394_REG_ALARM_BIT			0x10	// Alarm match bit, can be added to day, hours, minutes and seconds register, see ds,  Table 4
#define DS1394_REG_ALARM_100THS         0x08	// Hundredths of Seconds alarm register	/0-99 BCD
#define DS1394_REG_ALARM_SECONDS        0x09	// Seconds alarm register				/00-59 BCD
#define DS1394_REG_ALARM_MINUTES        0x0A	// Minutes alarm register				/00-59 BCD
#define DS1394_REG_ALARM_HOURS          0x0B	// Hours alarm register				/00-23 BCD
#define DS1394_REG_ALARM_DAY_DATE       0x0C	// Day alarm register				/01-31 BCD

/* Control registers address define */
#define DS1394_REG_CONTROL              0x0D	// control register (specific for the different models of ds13.., see datasheet)
#define DS1394_REG_STATUS               0x0E	// status register can be written to "0" to reset the flags
#define DS1394_REG_TRICKLE              0x0F	// Trickle charge register, see datasheet? Table 5

/* Control register map */						// specific for DS1394, see datasheet for other
#define DS1394_CONTROL_REG_CLEAN		0x00	// control register cleaning, oscillator run
#define DS1394_STOP_OSC 				BIT7	// stop oscillator, can be stopped on battery powered
#define DS1394_BBSQI 					BIT5	// Battery-Backed Square-Wave and Interrupt Enable
#define DS1394_RATE_1HZ 				0x00	// Rate Select frequency of the square-wave output
#define DS1394_RATE_4KHZ 				0x08	// when the square wave has been enabled
#define DS1394_RATE_8KHZ 				0x10	//
#define DS1394_RATE_32KHZ 				0x18	//
#define DS1394_INTERRUPT 				BIT2	// Interrupt Control, activates the SQW/INT provided the alarm is also enabled
#define DS1394_ALARM_ENABLE 			BIT0	// Alarm Interrupt Enable. permits the alarm flag (AF) bit in the status register to assert SQW/INT

/* Status register map */
#define DS1394_STATUS_REG_CLEAN			0x00	// Status register cleaning, clean all flags
#define DS1394_OSCILLATOR_STOP_FLAG 	BIT7	// Oscillator Stop Flag
#define DS1394_ALARM_FLAG 				BIT0	// Alarm Flag

/* Trickle charger register map */
#define DISABLE_TCS 					0x00	// Disable trickle charger
#define ENABLE_TCS 						0xA0	// Enable trickle charger
#define ENABLE_DIODE 					0x08	// Diode switch on
#define RES_250_OHM 					0x01	// resistors are used to select
#define RES_2K_OHM 						0x02	// the appropriate face value or its combination.
#define RES_4K_OHM 						0x03	//

typedef struct																	// public part of struct describing devices on ADXL345
{
	uint8_t (*Read)(const uint8_t in_addr);										// reads one byte from address
	uint8_t (*Write)(const uint8_t in_addr, const uint8_t in_val);				// writes one byte to address
	void (*SetDate)(const unsigned char day, const unsigned char day_date, const unsigned char month, const unsigned char year);	//
	void (*SetTime)(const unsigned char hours, const unsigned char minutes, const unsigned char seconds);							//

} ds1394_t;

ds1394_t *ds1394_ctor(void);													// declare constructor for concrete class

extern ds1394_t * const ds1394;													// declare pointer to public struct part

#endif /* DS1394U_H_ */