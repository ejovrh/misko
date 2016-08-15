// libraries
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <SPI.h> // SPI library
#include <EEPROM.h> // EEPROM library
#include <U8glib.h> // https://github.com/olikraus/u8glib/wiki/userreference
#include "M2tk.h" // https://code.google.com/archive/p/m2tklib/
#include "utility/m2ghu8g.h" // the u8g graphics handler
#include <avr/sleep.h> // AVR sleep management
#include <avr/power.h> // AVR power saving

// local files
#include "adxl345.h" // definition of ADXL345 register addresses
#include "gps_config.h" // gps stuff
#include "pin_definitions.h" // pin layout
#include "global_variables.h" // global variables et al.
#include "functions.h" // useful functions
#include "display.h"

void setup() 
{
	#include "eeprom_config.h" // definition of EEPROM fields
	#include "pin_modes.h" // definiton of pin modes
  #include "setup.h" // the actual setup [ without () ]
}

#include "gps_functions.h" // gps functions

void loop() 
{
  #include "loop.h" // the actual loop [ without () ] 
} 

void isr_adxl345_int1(void) //ISR for the ADXL345 accelerometer
{
	adxl345_int1 = 1; // simply (for now) set a flag to true (is false by default)
}