#ifndef INC_ADXL345_ADXL345_H_
#define INC_ADXL345_ADXL345_H_

#include "main.h"

#if defined(USE_ADXL345)	// if this particular device is active

// config items for act./inact. detection
#define	DATA_FORMAT_CFG	0x0B	// 16g range, full resolution, active high int.
#define	INT_MAP_CFG	0x00	// route all to INT1
#define	INT_ENABLE_INACT_CFG	0x08	// enable interrupt only for inact. detection
#define	INT_ENABLE_ACT_CFG	0x10	// enable interrupts only for act. detection
//#define	TIME_INACT_CFG	0x02	// LSB meaning: 0x05 (dec 5) => 5s , 0x3C (dec 60) => 60s
//#define	THRESH_INACT_CFG	0x02	// 62.5mg / LSB
//#define	THRESH_ACT_CFG	0x02	// 62.5mg / LSB
#define	ACT_INACT_CTL_CFG	0xFF	// enable activity and inactivity detection on x,y,z using AC
#define POWER_CTL_CFG_OFF	0x00	// all off
#define	POWER_CTL_CFG	0x2A	// 2Hz sampling on sleep, in measure mode, link mode, no auto sleep
#define	BW_RATE_CFG	0x16	// low power, output data rate 6.25Hz

typedef enum adxl345_reg_t  // register name to array index translation
{
	  DEVID = 0,  //  fixed device ID code of 0xE5, DS. p. 24
	  THRESH_TAP = 1,  // threshold	value for tap interrupts, DS. p. 24
	  OFSX = 2,  // user-set offset adjustments in twos complement format, DS. p. 24
	  OFSY = 3,  // ditto, DS. p. 24
	  OFSZ = 4,  // ditto, DS. p. 24
	  DUR = 5,	// event must be above the THRESH_TAP threshold to qualify as a tap event, DS. p. 24
	  LATENT = 6,  // wait time from the detection of a tap event to the start of the time window, DS. p. 24
	  WINDOW = 7,  // time after the expiration of the latency time, DS. p. 24
	  THRESH_ACT = 8,  // threshold value for detecting activity, DS. p. 24
	  THRESH_INACT = 9,  // threshold value for detecting inactivity, DS. p. 24
	  TIME_INACT = 10,  // time that acceleration must be less than THRESH_INACT for inactivity to be declared, DS. p. 24
	  ACT_INACT_CTL = 11,  // act/inact configuration, DS. p. 24
	  THRESH_FF = 12,  // threshold value for free-fall detection, DS. p. 25
	  TIME_FF = 13,  // time that the value of all axes must be less than THRESH_FF to generate a free-fall interrupt, DS. p. 25
	  TAP_AXES = 14,  // tape axes configuration, DS. p. 25
	  ACT_TAP_STATUS = 15,  // act/tap status bits, DS. p. 25
	  BW_RATE = 16,  // bandwidth/low power config, DS. p. 25
	  POWER_CTL = 17,  // power control config, DS. p. 26
	  INT_ENABLE = 18,  // interrupt enable configuration, DS. p. 26
	  INT_MAP = 19,  // interrupt map configuration, DS. p. 27
	  INT_SOURCE = 20,  // interrupt source configuration, DS. p. 27
	  DATA_FORMAT = 21,  // data format config for registers below, DS. p. 27
	  DATAX0 = 22,  // acceleration data for axis, DS. p. 27
	  DATAX1 = 23,  // ditto
	  DATAY0 = 24,  // ditto
	  DATAY1 = 25,  // ditto
	  DATAZ0 = 26,  // ditto
	  DATAZ1 = 27,  // ditto
	  FIFO_CTL = 28,  // FIFO configuration, DS. p. 28
	  FIFO_STATUS = 29  // FIFO status bits, DS. p. 28
} adxl345_reg_t;

typedef struct	// struct describing the Accelerometer functionality
{
	void (*ISR)(void);	// device ISR
	uint8_t (*ReadByte)(const adxl345_reg_t in_register);  // reads a byte from device
	void (*WriteByte)(const adxl345_reg_t in_register, const uint8_t data);  // writes a byte to device
} adxl345_t;

adxl345_t* adxl345_ctor(SPI_HandleTypeDef *in_hspi, GPIO_TypeDef *_SPI_CS_Port, const uint16_t _SPI_CS_Pin);  // the ADXL345 constructor

extern adxl345_t *const ADXL345;  // declare pointer to public struct part

#endif

#endif /* INC_ADXL345_ADXL345_H_ */
