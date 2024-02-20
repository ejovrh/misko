#ifndef INC_ORG1510MK4_ORG1510MK4_H_
#define INC_ORG1510MK4_ORG1510MK4_H_

#include "main.h"

#if defined(USE_ORG1510MK4)	// if this particular device is active

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
 *
 * see init() and adapt accordingly
 */

#define PARSE_GLL 0	// TODO - implement GLL parsing
#define PARSE_RMC 0 // parse RMC sentences
#define PARSE_VTG 1 // parse VTG sentences
#define PARSE_GGA 1	// parse GGA sentences
#define GSA_COUNT 2	// number of GSA talkers - GP, GL, ...
#define PARSE_GSV 1 // parse GSV sentences
#define EXPOSE_GSV 1	// expose normally hidden GSV data struct
#define PARSE_GSA 1 // parse GSA sentences
#define PARSE_ZDA 1	// parse ZDA sentences

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

#if PARSE_ZDA
typedef struct zda_t	// ZDA message struct
{
	char *time;  //	hhmmss time in UTC
	uint8_t day;	// day of the month
	uint8_t month;  // month of the year
	uint16_t year;	// year
	uint8_t tz;  // local time-zone offset from GMT
} zda_t;
#endif

#if PARSE_GGA
typedef enum gga_fix_t	// GGA sentence struct
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
#endif

#if PARSE_RMC || PARSE_GGA
typedef enum cardinal_dir_t  // directions on the hemisphere - NSEW
{  // ASCII character interpreted as integer
	  na = 0,  // no fix
	  N = 78,  // north
	  S = 83,  // south
	  E = 69,  // east
	  W = 87,  // west
} cardinal_dir_t;
#endif

#if PARSE_RMC || PARSE_GGA
typedef struct coord_dd_t  // NMEA decimal degree
{
	uint8_t deg;  // degrees	45 47.8623
	float s;  // seconds 015 54.9327
} coord_dd_t;
#endif

#if PARSE_GGA
typedef struct gga_t  // GGA sentence struct
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
#endif

#if PARSE_VTG || PARSE_RMC
typedef enum faa_mode_t  // FAA mode indicator
{  // ASCII character interpreted as integer
	  auton = 65,  // A - autonomous mode
	  diff = 68,  // D - differential mode
	  est = 69,  // E - estimated (dead reckoning) mode
	  man = 77,  // M - manual mode
	  simu = 83,  // S - simulation mode
	  invalid = 78,  // N - data not valid
} faa_mode_t;
#endif

#if PARSE_VTG
typedef struct vtg_t	// VTG sentence struct
{
	float track_tn;  // heading of track made good in true north
	float track_mn;  // heading of track made good in magnetic north
	float knots;  // speed in knots
	float kph;  // speed in kilometres per hour
	faa_mode_t mode;	// FAA mode indicator
} vtg_t;
#endif

#if PARSE_GSA
typedef enum gsa_selectionmode_t  // GSA selection mode
{  // ASCII character interpreted as integer
	  Auto = 65,  // Automatic, 2D or 3D
	  Manu = 77,  // Manual, forced to operate in 2D/3D
} gsa_selectionmode_t;
#endif

#if PARSE_GSA
typedef enum gsa_fixmode_t  // GSA fix mode
{
	  no = 1,  //	no fix
	  twoD = 2,  //	2D fix
	  threeD = 3,  // 3D fix

} gsa_fixmode_t;
#endif

#if PARSE_GSA  || PARSE_GSV
typedef struct spacevehicle_t  // space vehicle data
{
	uint8_t prn;	// Pseudo-Random Number of space vehicle
	uint8_t elev;  // elevation in degrees, 0-90
	uint16_t azim;  // azimuth in degrees from true north, 0 - 359
	uint8_t snr;  // Signal to Noise Ratio, 00 - 99, NULL when not tracking
} spacevehicle_t;
#endif

#if PARSE_GSA
typedef struct gsa_t  // GSA sentence struct
{
	gsa_selectionmode_t sel_mode;  // GSA selection mode
	gsa_fixmode_t fixmode;  // fix mode
#if PARSE_GSV
	spacevehicle_t *sv[12];  // array of pointers to GSV objects
#else
	uint8_t sv01;  //	space vehicle 01 PRN
	uint8_t sv02;  // space vehicle 02 PRN
	uint8_t sv03;  // ...
	uint8_t sv04;  //
	uint8_t sv05;  //
	uint8_t sv06;  //
	uint8_t sv07;  //
	uint8_t sv08;  //
	uint8_t sv09;  //
	uint8_t sv10;  //
	uint8_t sv11;  //
	uint8_t sv12;  //
#endif
	float pdop;  // Position Dilution Of Position
	float hdop;  // Horizontal Dilution Of Position
	float vdop;  // Vertical Dilution Of Position
} gsa_t;
#endif

#if PARSE_GSV
typedef struct gsv_t	// GSV sentence struct
{
	uint8_t msg_count;	// total number of GSA messages in this cycle
	uint8_t sv_visible;  // total number of space vehicles visible
	spacevehicle_t sv[12];  // container for 12 space vehicles
} gsv_t;
#endif

#if PARSE_RMC
typedef enum rmc_status_t  // RMC fix status
{  // ASCII character interpreted as integer
	  Active = 65,  // Active
	  Void = 86,  // Void
} rmc_status_t;

typedef struct rmc_t	// RMC sentence struct
{
	char *time;  //	p[osition fix time in hhmmss time in UTC
	rmc_status_t status;	// GPS status indicator
	coord_dd_t *lat;  // latitude
	cardinal_dir_t lat_dir;  // direction of latitude
	coord_dd_t *lon;  // longitude
	cardinal_dir_t lon_dir;  // direction of longitude
	float knots;	// speed in knots
	uint16_t azimut;	// track angle in degrees true north
	char *date;  // date in ddmmyy format
	uint16_t mag_var;  // magnetic variation in degrees
	cardinal_dir_t var_dir;  // magnetic variation cardinal direction
	faa_mode_t mode;	// FAA mode indicator
} rmc_t;
#endif

typedef struct org1510mk4_t  // struct describing the GPS module functionality
{
#if PARSE_ZDA	// expose zda_t
	zda_t *zda;  // ZDA-derived data
#endif
#if PARSE_GGA
	gga_t *gga;  // GGA-derived data
#endif
#if PARSE_VTG	// expose vtg_t // expose vtg_t
	vtg_t *vtg;  // VTG-derived data
#endif
#if PARSE_GSA && !PARSE_GSV
	gsa_t *gpgsa;  // GSA-derived data
	gsa_t *glgsa;  // GSA-derived data
#endif
#if PARSE_GSA // expose gsa_t
	gsa_t *gsa;  // GSA-derived data
#endif
#if PARSE_GSV && EXPOSE_GSV // expose normally hidden gsv_t
	gsv_t *gpgsv;  //	 GSV-derived data
	gsv_t *glgsv;  //	 GSV-derived data
#endif
#if PARSE_RMC // expose rmc_t
	rmc_t *rmc;  // RMC-derived data
#endif

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
