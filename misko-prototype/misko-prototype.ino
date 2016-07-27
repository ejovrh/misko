// libraries
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <EEPROMex.h> // extended EEPROM library

// local files
#include "gps_config.h" // gps stuff
#include "pin_definitions.h" // pin layout

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

void setup() 
{
#include "pin_modes.h"

  Serial.begin(SERIALRATE); // connect to the serial terminal
  Serial.println("start");

  // initialize GPS
  Serial3.begin(GPSRATE);   // connect to the GPS at the desired rate
  Serial3.print(SERIAL_SET); // set gps serial comm. parameter
  Serial3.print(WAAS_ON);
  Serial3.print(RMC_ON);
  Serial3.print(GSA_ON);
  Serial3.print(GSV_ON);
    
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
  get_nmea_sentences();
} 
