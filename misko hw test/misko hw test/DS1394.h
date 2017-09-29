#ifndef DS1394_H_
#define DS1394_H_
#include <avr/io.h>

/*
 * DS1394.h
 *	register addresses for the RTC
 *		refer to RTC datasheet p. 14
 *
 *
 *
 */

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
	#define bitmask_24h_format 0x40									// 1 = 24 format
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
	#define bitmask_alarm_hours_24h_format 0x40			// 1 = 24h format
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
	#define tcs3 0x80																//
	#define tcs2 0x40																//
	#define tcs1 0x20																//
	#define tcs0 0x10																//
	#define ds1 0x08																//
	#define ds0 0x04																//
	#define rout1 0x02															//
	#define rout0 0x01															//

uint8_t ds1394_read(uint8_t addr);

uint8_t ds1394_write(uint8_t addr, const uint8_t val);

#endif /* DS1394_H_ */