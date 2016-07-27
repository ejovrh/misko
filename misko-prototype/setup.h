// basic structure for setup items
typedef struct setup
{
  unsigned int version = 0; // 
  char lcd_power = 'a';  // on(1), off(0), auto(a), default auto
  char bluetooth_power = 'a';  // on(1), off(0), auto(a), default auto
  int lcd_auto_timeout = 2; // auto timeout in seconds
  int bluetooth_timeout = 60; // bluetooth timeout in seconds
  int timezone = 2; // UTC timezone
  // byte order: leftmost: 0, rightmost 7; value 0(off), value 1(on)
  //  bit assignment
  //    0 - reserved - do not use
  //    1 - LOG_RMC - default on
  //    2 - LOG_GGA - default off
  //    3 - LOG_GLL - default off
  //    4 - LOG_GSA - default on
  //    5 - LOG_GSV - default on
  //    6 - LOG_VTG - default off
  //    7 - USE_WAAS - default on
        
  byte gps_settings = B01001101;
} Setup;

