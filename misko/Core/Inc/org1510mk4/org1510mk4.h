#ifndef INC_ORG1510MK4_ORG1510MK4_H_
#define INC_ORG1510MK4_ORG1510MK4_H_

#include "main.h"

#if defined(USE_ORG1510MK4)	// if this particular device is active

#define PARSE_PMTK 1	// parse MTK NMEA module command messages
#define PRINT_PMTK_001 0	// print PMTK message reply 001
#define PRINT_PMTK_010 1	// ditto
#define PRINT_PMTK_011 1	//
#define PRINT_PMTK_710 0	//
#define PRINT_PMTK_711 0	//
#define PRINT_PMTK_668 0	//

#define PARSE_GLL 0	// parse GLL sentences
#define PARSE_RMC 0 // parse RMC sentences
#define PARSE_VTG 1 // parse VTG sentences
#define PARSE_GGA 1	// parse GGA sentences
#define GSA_COUNT 2	// number of GSA talkers - GP, GL, ...
#define PARSE_GSV 1 // parse GSV sentences
#define EXPOSE_GSV 0	// expose normally hidden GSV data struct
#define PARSE_GSA 1 // parse GSA sentences
#define PARSE_ZDA 1	// parse ZDA sentences

#include "nmea_objects.h"	// NMEA-related structs and enums

typedef struct print_nmea_t  // NMEA sentence printout flags
{
	uint8_t pmtk :1;  // print PMTK messages
	uint8_t pmtk_001 :1;  // print PMTK 001 reply messages
	uint8_t pmtk_010 :1;  //
	uint8_t pmtk_011 :1;  //
	uint8_t pmtk_710 :1;  //
	uint8_t pmtk_711 :1;  //
	uint8_t pmtk_668 :1;  //
	uint8_t gll :1;  // print GLL sentences
	uint8_t rmc :1;  // print RMC sentences
	uint8_t vtg :1;  // ditto
	uint8_t gga :1;  //
	uint8_t gsv :1;  //
	uint8_t gsa :1;  //
	uint8_t zda :1;  //
} print_nmea_t;

typedef enum org1510mk4_power_t  // GPS module power states, DS. ch. 4.3.10, p. 24
{
	  off = 0,  // supply power off & module (possibly) in backup mode
	  on = 1,  // supply power on & module (possibly) in backup mode
	  backup = 2,  // backup mode, DS. ch. 4.3.15
	  wakeup = 3,  // full power continuous mode, DS. ch. 4.3.11
	  standby = 4,  // standby mode, DS. ch. 4.3.12
	  periodic = 5,  // periodic mode, DS. ch. 4.3.13
	  alwayslocate = 6,  // alwaysLocate mode, DS. ch. 4.3.14
	  reset = 7,  // reset the module, DS. ch. 8.3.2
	  discharge = 8,  // supercap discharge FET control
} org1510mk4_power_t;

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
#if PARSE_GLL
	gll_t *gll;  //GLL-parsed data
#endif
	uint8_t flag_alm_eph_query :1;  // flag for running AlmEphQuery()
	print_nmea_t *print;	// flags struct for dynamic NMEA & PMTK printout control
	uint8_t *NMEA;  //	last NMEA sentence
	volatile org1510mk4_power_t PowerMode;  // current power mode of the GPS module
	void (*Power)(const org1510mk4_power_t state);  // GPS module power mode change control function
	void (*Parse)(const uint16_t Size);  // loads incoming NMEA string from DMA into a buffer and parses it
	void (*Read)(void);  //
	void (*Write)(const char *str);  // writes a NEMA sentence to the GPS module
	void (*AlmEphQuery)(void);  // asynchronous flag_alm_eph_query for SV almanac & ephemeris
} org1510mk4_t;

org1510mk4_t* org1510mk4_ctor(UART_HandleTypeDef *gps, UART_HandleTypeDef *sys);  // the ORG1510MK4 constructor

extern org1510mk4_t *const ORG1510MK4;  // declare pointer to public struct part

#endif

#endif /* INC_ORG1510MK4_ORG1510MK4_H_ */
