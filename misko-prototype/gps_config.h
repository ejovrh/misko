// what to log 
// FIXME - not used in gps_functions
#define LOG_DDM 0
#define LOG_RMC 1 // RMC-Recommended Minimum Specific GNSS Data, message 103,04
#define LOG_GGA 1 // GGA-Global Positioning System Fixed Data, message 103,00
//#define LOG_GLL 0 // GLL-Geographic Position-Latitude/Longitude, message 103,01
//#define LOG_GSA 0 // GSA-GNSS DOP and Active Satellites, message 103,02
//#define LOG_GSV 0 // GSV-GNSS Satellites in View, message 103,03
//#define LOG_VTG 0 // VTG-Course Over Ground and Ground Speed, message 103,05

//SiRF Disable Static Navigation, Message ID 143
//#define DisStatNav "$A0,$A2,$00,$02,$8F,$00,$00,$8F,$B0,$B3"
//#define SirfMSG136 "A0A2000E88000001000000000000000502010091B0B3"

// serial communication parameters - baud, parity, etc.
#define SERIAL_SET  "$PSRF100,01,4800,08,01,00*0E\r\n"

// GGA-Global Positioning System Fixed Data, message 103,00
#define GGA_ON   "$PSRF103,00,00,01,01*25\r\n"
#define GGA_OFF  "$PSRF103,00,00,00,01*24\r\n"

// GLL-Geographic Position-Latitude/Longitude, message 103,01
#define GLL_ON   "$PSRF103,01,00,01,01*24\r\n"
#define GLL_OFF  "$PSRF103,01,00,00,01*25\r\n"

// GSA-GNSS DOP and Active Satellites, message 103,02
#define GSA_ON   "$PSRF103,02,00,01,01*27\r\n"
#define GSA_OFF  "$PSRF103,02,00,00,01*26\r\n"

// GSV-GNSS Satellites in View, message 103,03
#define GSV_ON   "$PSRF103,03,00,01,01*26\r\n"
#define GSV_OFF  "$PSRF103,03,00,00,01*27\r\n"

// RMC-Recommended Minimum Specific GNSS Data, message 103,04
#define RMC_ON   "$PSRF103,04,00,01,01*21\r\n"
#define RMC_OFF  "$PSRF103,04,00,00,01*20\r\n"

// VTG-Course Over Ground and Ground Speed, message 103,05
#define VTG_ON   "$PSRF103,05,00,01,01*20\r\n"
#define VTG_OFF  "$PSRF103,05,00,00,01*21\r\n"

// Switch Development Data Messages On/Off, message 105
#define DDM_ON   "$PSRF105,01*3E\r\n"
#define DDM_OFF  "$PSRF105,00*3F\r\n"

// use WAAS
#define WAAS_ON    "$PSRF151,01*0F\r\n"
#define WAAS_OFF   "$PSRF151,00*0E\r\n"

// use SBAS
#define SBAS_ON 	"$PSRF103,00,8,0,1*1C" //SBAS ON 
#define SBAS_OFF 	"$PSRF103,00,9,0,1*1D" //SABS OFF
