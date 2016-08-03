// libraries
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <EEPROM.h> // EEPROM library

// local files
#include "gps_config.h" // gps stuff
#include "pin_definitions.h" // pin layout
#include "global_stuff.h" // global variables et al.
#include "functions.h" // useful functions

void setup() 
{
#include "pin_modes.h"
#include "setup.h"

  Serial.begin(SERIALRATE); // connect to the serial terminal
  Serial.println("start");

  // initialize GPS
  Serial3.begin(GPSRATE);   // connect to the GPS at the desired rate
  Serial3.print(SERIAL_SET); // set gps serial comm. parameter
  Serial3.print(WAAS_ON); // will become obsolete via EEPROM and setup
  Serial3.print(RMC_ON); // will become obsolete via EEPROM and setup
  Serial3.print(GSA_ON); // will become obsolete via EEPROM and setup
  Serial3.print(GSV_ON); // will become obsolete via EEPROM and setup
    
  delay(50);

  // initialize SD card  
  if (!SD.begin(sd_ss_pin)) // see if the card is present and can be initialized
  {  
    Serial.println("SD Card failed, or not present");
    return;
  }  
} // setup end 

#include "gps_functions.h" // gps functions

void loop() 
{
  handle_bluetooth_button(); // handles the bluetooth power button
  
  handle_lcd_buttons(); // handles the lcd buttons
  
  get_nmea_sentences(); // gets NMEA sentences out of the GPS and deals with them
} 
