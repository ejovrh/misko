#ifndef ADXL345_H_
#define ADXL345_H_

#include <inttypes.h>

// ADXL345 Register Addresses
#define DEVID   				0x00											// Device ID, DS. p. 24
#define THRESH_TAP  			0x1D											// Tap Threshold, DS. p. 24
#define OFFSET_X    			0x1E											// X-axis offset, DS. p. 24
#define OFSFSET_Y    			0x1F											// Y-axis offset, DS. p. 24
#define OFSFSET_Z    			0x20											// Z-axis offset, DS. p. 24
#define DURATION  				0x21											// Tap Duration, DS. p. 24
#define LATENT    				0x22											// Tap latency, DS. p. 24
#define WINDOW    				0x23											// Tap window, DS. p. 24
#define THRESH_ACT  			0x24											// Activity Threshold, DS. p. 24
#define THRESH_INACT  			0x25											// Inactivity Threshold, DS. p. 24
#define TIME_INACT  			0x26											// Inactivity Time, DS. p. 24
#define ACT_INACT_CTL 			0x27											// act./inact. AC/DC bits, DS. p. 24
#define THRESH_FF 				0x28											// free-fall threshold, DS. p. 25
#define TIME_FF   				0x29											// Free-Fall Time, DS. p. 25
#define TAP_AXES  				0x2A											// Axis control for tap/double tap, DS. p. 25
#define ACT_TAP_STATUS			0x2B											// Source of tap/double tap, DS. p. 25
#define BW_RATE   				0x2C											// Data rate and power mode control, DS. p. 25
#define POWER_CTL 				0x2D											// Power Control Register, DS. p. 25
#define INT_ENABLE  			0x2E											// Interrupt Enable Control, DS. p. 26
#define INT_MAP   				0x2F											// Interrupt Mapping Control, DS. p. 26
#define INT_SOURCE  			0x30											// Source of interrupts, DS. p. 26
#define DATA_FORMAT 			0x31											// Data format control, DS. p. 26
#define DATAX0    				0x32											// X-Axis Data 0, DS. p. 27
#define DATAX1    				0x33											// X-Axis Data 1, DS. p. 27
#define DATAY0    				0x34											// Y-Axis Data 0, DS. p. 27
#define DATAY1    				0x35											// Y-Axis Data 1, DS. p. 27
#define DATAZ0    				0x36											// Z-Axis Data 0, DS. p. 27
#define DATAZ1    				0x37											// Z-Axis Data 1, DS. p. 27
#define FIFO_CTL  				0x38											// FIFO control, DS. p. 27
#define FIFO_STATUS 			0x39											// FIFO status, DS. p. 27

#define DEVID_VAL				0xE5											// hardcoded device ID

//config items for act./inact. detection
#define	DATA_FORMAT_CFG			0x0B											// 16g range, full resolution, active high int.
#define	INT_MAP_CFG				0x00											// route all to INT1
#define	INT_ENABLE_CFG			0x18											// enable interrupts only for act./inact. detection
//set via FeRAM readout #define	TIME_INACT_CFG		0x02	// LSB meaning: 0x05 (dec 5) => 5s , 0x3C (dec 60) => 60s
//set via FeRAM readout #define	THRESH_INACT_CFG	0x02	// 62.5mg / LSB
//set via FeRAM readout #define	THRESH_ACT_CFG		0x02	// 62.5mg / LSB
#define	ACT_INACT_CTL_CFG		0xFF											// enable activity and inactivity detection on x,y,z using AC
#define POWER_CTL_CFG_OFF		0x00											// all off
#define	POWER_CTL_CFG			0x2A											// 2Hz sampling on sleep, in measure mode, link mode, no auto sleep
#define	BW_RATE_CFG				0x16											// low power, output data rate 6.25Hz

typedef struct																	// public part of struct describing devices on ADXL345
{
	uint8_t (*ReadByte)(const uint8_t in_addr);									// reads one byte of data from address
	void (*WriteByte)(const uint8_t in_addr, const uint8_t in_data);			// writes one byte of data to address
} adxl345_t;

adxl345_t *adxl345_ctor(void);													// declare constructor for concrete class

extern adxl345_t * const adxl345;												// declare pointer to public struct part

#endif /* ADXL345_H_ */