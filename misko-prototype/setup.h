// basic structure for setup items
typedef struct setup
{
  unsigned char version = CONFIG_VERSION; // 0, 1, ..., 254
  unsigned char lcd_power = 'a';  // on(1), off(0), auto(a), default auto
  unsigned char bluetooth_power = 'a';  // on(1), off(0), auto(a), default auto
  unsigned char lcd_auto_timeout = 2; // auto timeout in seconds, 0, 1, ..., 254
  unsigned char bluetooth_timeout = 60; // bluetooth timeout in seconds, 0, 1, ..., 254
  char timezone = +2; // UTC timezone -12, ..., 12
  // gps_settings byte order: leftmost: 0, rightmost 7; value 0(off), value 1(on)
  //  bit assignment
  //    0 - reserved - do not use
  //    1 - LOG_RMC - default on, RMC-Recommended Minimum Specific GNSS Data
  //    2 - LOG_GGA - default off, GGA-Global Positioning System Fixed Data
  //    3 - LOG_GLL - default off, GLL-Geographic Position-Latitude/Longitude
  //    4 - LOG_GSA - default on, GSA-GNSS DOP and Active Satellites
  //    5 - LOG_GSV - default on, GSV-GNSS Satellites in View
  //    6 - LOG_VTG - default off, VTG-Course Over Ground and Ground Speed
  //    7 - USE_WAAS - default on, WAAS
  byte gps_settings = B01001101;
} Setup;

