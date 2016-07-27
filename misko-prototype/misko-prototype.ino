// libraries
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <EEPROM.h> // EEPROM library

// local files
#include "gps_config.h" // gps stuff
#include "pin_definitions.h" // pin layout

//   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE IN setup.h
#define CONFIG_VERSION 2 // protection against excessive EEPROM writes
//   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE IN setup.h

#define GPSRATE 4800
#define SERIALRATE 9600
//BLUETOOTHSERIALRATE is hardcoded in device
#define NMEA_BUFFERSIZE 80 // plenty big

// global variables
char NMEA_buffer[NMEA_BUFFERSIZE] = "";        // string buffer for the NMEA sentence
unsigned int bufferid = 0; // holds the current position in the NMEA_buffer array, used fo walk through the buffer

char gps_date[9] = "20XXXXXX"; // 0-7 + 1 for '\0'
char gps_time[7] = "XXXXXX"; // 0-5 + 1 for '\0'
char gps_logfile[13] = "";

char debug_out[80] = "";

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
   int printed = 0;
void loop() 
{
  get_nmea_sentences();

  // poor mans debugging

  if (digitalRead(bluetooth_power_button_pin) == HIGH && printed == 0)
  {
    Serial.println(" eeprom debug output: ");
    for (int i = 0; i < 13; i++)
    {
      int val = EEPROM[i];
      sprintf(debug_out, "%d - %d \n", i, val);
      Serial.print(debug_out);
    }
    printed = 1;
  }
  
} 
