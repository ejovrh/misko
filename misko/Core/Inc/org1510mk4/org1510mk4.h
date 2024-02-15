#ifndef INC_ORG1510MK4_ORG1510MK4_H_
#define INC_ORG1510MK4_ORG1510MK4_H_

#include "main.h"

#if defined(USE_ORG1510MK4)	// if this particular device is active

typedef enum org1510mk4_power_t  // GPS module power states, DS. ch. 4.3.10, p. 24
{
	  off = 0,  // supply power off & module (possibly) in backup mode
	  on = 1,  // supply power on & module (possibly) in backup mode
	  backup = 2,  // backup mode, DS. ch. 4.3.15
	  wakeup = 3,  // full power continuous mode, DS. ch. 4.3.11
	  standby = 4,  // standby mode, DS. ch. 4.3.12
	  periodic = 5,  // periodic mode, DS. ch. 4.3.13
	  alwayslocate = 6,  // alwaysLocate mode, DS. ch. 4.3.14
	  reset = 7  // reset the module, DS. ch. 8.3.2
} org1510mk4_power_t;

typedef struct gnzda_t	// GNZDA sentence struct
{
	char *time;  //	hhmmss time in UTC
	uint8_t day;	// day of the month
	uint8_t month;  // month of the year
	uint16_t year;	// year
	uint8_t tz;  // local time-zone offset from GMT
} gnzda_t;

typedef enum gga_fix_t	// GNGGA GPS fix type
{
	  none = 0,  // no fix
	  GPS = 1,	// GPS fix
	  SBAS = 2,  // SBAS/DGPS
	  PPS = 3,	// PPS fix
	  RTKfixed = 4,  // RealTime Kinematic fixed
	  RTKfloat = 5,  // RealTime Kinematic float
	  INS = 6,	// estimated (dead reckoning)
	  manual = 7,  // manual input
	  simulation = 8,  // simulation
} gga_fix_t;

typedef struct gngga_t  // GNGGA sentence
{
	char *fix_date;  // age of GPS fix data
	gga_fix_t fix;	// type of GPS fix
	uint8_t sat_used;  // satellites used for solution
	float HDOP;  // Horizontal Dilution Of Precision
	float alt;  // altitude in meters above mean sea level
} gngga_t;

typedef struct org1510mk4_t  // struct describing the GPS module functionality
{
	gnzda_t *zda;  // ZDA-derived data
	gngga_t *gga;  // GGA-derived data
	uint8_t *NMEA;  //	last NMEA sentence
	volatile org1510mk4_power_t PowerMode;  // current power mode of the GPS module
	void (*Power)(const org1510mk4_power_t state);  // GPS module power mode change control function
	void (*Parse)(const uint16_t Size);  // loads incoming NMEA string from DMA into a buffer and parses it
	void (*Read)(void);  //
	void (*Write)(const char *str);  // writes a NEMA sentence to the GPS module
} org1510mk4_t;

org1510mk4_t* org1510mk4_ctor(UART_HandleTypeDef *gps, UART_HandleTypeDef *sys);  // the ORG1510MK4 constructor

extern org1510mk4_t *const ORG1510MK4;  // declare pointer to public struct part

#endif

#endif /* INC_ORG1510MK4_ORG1510MK4_H_ */
