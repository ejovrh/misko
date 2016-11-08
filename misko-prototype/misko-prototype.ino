// libraries
#include <SoftwareSerial.h> // software serial library for GPS/Bluetooth
#include <SD.h> // SD card library
#include <SPI.h> // SPI library
#include <EEPROM.h> // EEPROM library for configuration data
#include <U8glib.h> // OLED chip driver - https://github.com/olikraus/u8glib/wiki/userreference
#include <M2tk.h> // user menu library - https://code.google.com/archive/p/m2tklib/
#include "utility/m2ghu8g.h" // U8glib graphics handler (one of many)
#include "utility/mas.h" // mass storage handler for the SD card
#include <gprs.h> // GPRS library for SIM800C
#include <avr/sleep.h> // AVR sleep management
#include <avr/power.h> // AVR power saving

// local files
#include "adxl345.h" // definition of ADXL345 register addresses
#include "fram.h" // definition of FRAM register addresses & addressing scheme for config settins
#include "pin_definitions.h" // pin layout
#include "global_variables.h" // global variables et al.
#include "functions.h" // useful functions
#include "display.h" // display (menu) code

void setup()
{
	#include "eeprom_config.h" // definition of EEPROM fields
	#include "pin_modes.h" // definiton of pin modes
  #include "setup.h" // the actual setup [ without () ]
}

#include "gps_functions.h" // gps functions - kludge alert!

void loop()
{
  #include "loop.h" // the actual loop [ without () ]
}

//ISR for the ADXL345 accelerometer
ISR(__vectorPCINT6_vect)
{
	flag_adxl345_int1 = 1; // simply (for now) set a flag to true (is false by default)
}

// ISR for timer5 events - generated every second
//	stuff that needs to run periodically goes here
ISR(TIMER5_COMPA_vect)
{
	//avg_temperature(calculate_temperature(), 10); // calculates the temperature via a TMP36 sensor over 10 iterations
	scheduler_run_count++;
}