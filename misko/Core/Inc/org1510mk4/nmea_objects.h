#ifndef INC_ORG1510MK4_NMEA_OBJECTS_H_
#define INC_ORG1510MK4_NMEA_OBJECTS_H_

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
	char *time;  // age of GPS fix data
	coord_dd_t *lat;  // latitude
	cardinal_dir_t *lat_dir;  // cardinal direction of latitude
	coord_dd_t *lon;  // longitude
	cardinal_dir_t *lon_dir;  // cardinal direction of longitude
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

typedef enum bool_t
{
	false = 0,
	true = 1,
} bool_t;

typedef struct spacevehicle_t  // space vehicle data
{
	bool_t alm;	// is an almanac for a SV available?
	bool_t eph;	// is an ephemeris for a SV available?
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
	float PDOP;  // Position Dilution Of Position
	float HDOP;  // Horizontal Dilution Of Position
	float VDOP;  // Vertical Dilution Of Position
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

#if PARSE_RMC || PARSE_GLL
typedef enum rmc_status_t  // RMC fix status
{  // ASCII character interpreted as integer
	  Active = 65,  // Active
	  Void = 86,  // Void
} rmc_status_t;
#endif

#if PARSE_RMC
typedef struct rmc_t	// RMC sentence struct
{
	char *time;  //	position fix time in hhmmss time in UTC
	rmc_status_t status;	// GPS status indicator
	coord_dd_t *lat;  // latitude
	cardinal_dir_t *lat_dir;  // direction of latitude
	coord_dd_t *lon;  // longitude
	cardinal_dir_t *lon_dir;  // direction of longitude
	float knots;	// speed in knots
	uint16_t azimut;	// track angle in degrees true north
	char *date;  // date in ddmmyy format
	uint16_t mag_var;  // magnetic variation in degrees
	cardinal_dir_t *var_dir;  // magnetic variation cardinal direction
	faa_mode_t mode;	// FAA mode indicator
} rmc_t;
#endif

#if PARSE_GLL
typedef struct gll_t	// GLL sentence struct
{
	coord_dd_t *lat;  // latitude
	cardinal_dir_t *lat_dir;  // direction of latitude
	coord_dd_t *lon;  // longitude
	cardinal_dir_t *lon_dir;  // direction of longitude
	char *time;  //	position fix time in hhmmss time in UTC
	rmc_status_t status;	// GPS status indicator
	faa_mode_t mode;	// FAA mode indicator

} gll_t;
#endif

#if PARSE_PMTK

typedef enum pmtk_sys_msg_t
{	// ASCII character interpreted as integer
	unknown = 48,	// unknown state
	startup = 49,	// system startup
	noteEPO = 50, //	notification for host aiding EPO
	normal_mode = 51,	// transition to normal mode is successful
} pmtk_sys_msg_t;

typedef enum pmtk_ack_t
{	// ASCII character interpreted as integer
	invald = 48,	// invalid command
	unsupported = 49,	// unsupported command
	failed = 50,	// valid command but action failed
	success = 51,	// valid command and action succeeded
} pmtk_ack_t;

typedef struct pmtk_t	// PMTK message struct
{
		pmtk_sys_msg_t status;	//
		uint16_t cmd;	// reply to command
		pmtk_ack_t flag;	// flag for reply command
		char *buff;	// output buffer for longer stuff
} pmtk_t ;
#endif

#endif /* INC_ORG1510MK4_NMEA_OBJECTS_H_ */
