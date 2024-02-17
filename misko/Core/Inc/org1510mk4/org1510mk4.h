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

typedef struct zda_t	// ZDA message struct
{
	char *time;  //	hhmmss time in UTC
	uint8_t day;	// day of the month
	uint8_t month;  // month of the year
	uint16_t year;	// year
	uint8_t tz;  // local time-zone offset from GMT
} zda_t;

typedef enum gga_fix_t	// GGA message struct
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

typedef enum cardinal_dir_t  // directions on the hemisphere - NSEW
{  // ASCII character interpreted as integer
	  na = 0,  // no ifx
	  N = 78,  // north
	  S = 83,  // south
	  E = 69,  // east
	  W = 87,  // west
} cardinal_dir_t;

typedef struct coord_dd_t  // NMEA decimal degree
{
	uint8_t deg;  // degrees	45 47.8623
	float s;  // seconds 015 54.9327
} coord_dd_t;

typedef struct gga_t  // GGA message
{
	char *fix_date;  // age of GPS fix data
	coord_dd_t *lat;  // latitude
	cardinal_dir_t lat_dir;  // direction of latitude
	coord_dd_t *lon;  // longitude
	cardinal_dir_t lon_dir;  // direction of longitude
	gga_fix_t fix;	// type of GPS fix
	uint8_t sat_used;  // satellites used for solution
	float HDOP;  // Horizontal Dilution Of Precision
	float alt;  // orthometric height (MSL reference)
	float geoid_sep;  // geoid separation in meters
	float dgps_age;  // age of DGPS record
} gga_t;

typedef enum faa_mode_t  // FAA mode indicator
{  // ASCII character interpreted as integer
	  auton = 65,  // A - autonomous mode
	  diff = 68,  // D - differential mode
	  est = 69,  // E - estimated (dead reckoning) mode
	  man = 77,  // M - manual mode
	  simu = 83,  // S - simulation mode
	  invalid = 78,  // N - data not valid
} faa_mode_t;

typedef struct vtg_t	// VTG sentence struct
{
	float track_tn;  // heading of track made good in true north
	float track_mn;  // heading of track made good in magnetic north
	float knots;  // speed in knots
	float kph;  // speed in kilometres per hour
	faa_mode_t mode;	// FAA mode indicator
} vtg_t;

typedef enum gsa_selectionmode_t  // GSA selection mode
{  // ASCII character interpreted as integer
	  A = 65,  // Automatic, 2D or 3D
	  M = 77,  // Manual, forced to operate in 2D/3D
} gsa_selectionmode_t;

typedef enum gsa_fixmode_t  // GSA fix mode
{
	  no = 1,  //	no fix
	  twoD = 2,  //	2D fix
	  threeD = 3,  // 3D fix

} gsa_fixmode_t;

typedef struct gsa_t  // GSA sentence struct
{
	gsa_selectionmode_t sel_mode;  // GSA selection mode
	gsa_fixmode_t fixmode;  // fix mode
	uint8_t sv01;  //	space vehicle 01 ID
	uint8_t sv02;  // ...
	uint8_t sv03;  // 01 - 32 is GPS, 33 - 64 SBAS, 64+ GLONASS
	uint8_t sv04;  //
	uint8_t sv05;  //
	uint8_t sv06;  //
	uint8_t sv07;  //
	uint8_t sv08;  //
	uint8_t sv09;  //
	uint8_t sv10;  //
	uint8_t sv11;  //
	uint8_t sv12;  //
	float pdop;  // Position Dilution Of Position
	float hdop;  // Horizontal Dilution Of Position
	float vdop;  // Vertical Dilution Of Position
} gsa_t;

typedef struct spacevehicle_t
{
	uint8_t prn;	// Pseudo-Random Number of space vehicle
	uint8_t elev;  // elevation in degrees, 0-90
	uint16_t azim;  // azimuth in degrees from true north, 0 - 359
	uint8_t snr;  // Signal to Noise Ratio, 00 - 99, NULL when not tracking
} spacevehicle_t;

typedef struct gsv_t
{
	uint8_t msg_count;	// total number of GSA messages in this cycle
	uint8_t sv_visible;  // total number of space vehicles visible
	spacevehicle_t sv[12];  // container for 12 space vehicles
} gsv_t;

typedef struct org1510mk4_t  // struct describing the GPS module functionality
{
	zda_t *zda;  // ZDA-derived data
	gga_t *gga;  // GGA-derived data
	vtg_t *vtg;  // VTG-derived data
	gsa_t *gsa;  // GSA-derived data
	gsv_t *gsv;  //	 GSV-derived data
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
