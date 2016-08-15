// libraries
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <SPI.h> // SPI library
#include <EEPROM.h> // EEPROM library
#include <U8glib.h> // https://github.com/olikraus/u8glib/wiki/userreference
#include "M2tk.h" // https://code.google.com/archive/p/m2tklib/
#include "utility/m2ghu8g.h" // the u8g graphics handler
#include <avr/sleep.h>
#include <avr/power.h>

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
	#include "pin_modes.h"
  #include "setup.h"
}

#include "gps_functions.h" // gps functions

void loop() 
{
  #include "loop.h"
} 

//ISR for the ADXL345 accelerometer
void isr_adxl345_int1(void) 
{
	adxl345_int1 = 1;
}