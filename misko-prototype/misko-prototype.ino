// libraries
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <EEPROM.h> // EEPROM library
#include <U8glib.h> // https://github.com/olikraus/u8glib/wiki/userreference
#include "M2tk.h" // https://code.google.com/archive/p/m2tklib/
#include "utility/m2ghu8g.h" // the u8g graphics handler

// local files
#include "gps_config.h" // gps stuff
#include "pin_definitions.h" // pin layout
#include "global_stuff.h" // global variables et al.
#include "functions.h" // useful functions

// EERPOM indices
#define EERPOM_LCD_POWER_INDEX 1
#define EERPOM_BLUETOOTH_POWER_INDEX 2
#define EERPOM_LCD_AUTO_TIMEOUT_INDEX 3
#define EERPOM_BLUETOOTH_ATUO_TIMEOUT_INDEX 4
#define EERPOM_TIMEZONE_INDEX 5
#define EEPROM_GPS_GPRMC_INDEX 6
#define EEPROM_GPS_GPGGA_INDEX 7
#define EEPROM_GPS_USE_WAAS_INDEX 8

void setup() 
{
  #include "setup.h"
}

#include "gps_functions.h" // gps functions

void loop() 
{
  #include "loop.h"
} 
