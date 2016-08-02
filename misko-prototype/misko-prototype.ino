// libraries
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <EEPROM.h> // EEPROM library
#include <IIC_without_ACK.h> // some obscure (chinese) I2C library for the SSD1306 OLED chipset
#include "oledfont.c"   // OLED fonts

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

  OLED.Initial(); // initializes the display (values are hardcoded in "IIC_without_ACK.cpp"
} // setup end 

#include "gps_functions.h" // gps functions

void loop() 
{
  /*
  OLED.Fill_Screen(0x00);
  OLED.Char_F6x8(0,0,"line 1");
  OLED.Char_F6x8(0,1,"line 2");
  OLED.Char_F6x8(0,2,"line 3");
  OLED.Char_F6x8(0,3,"line 4");
  OLED.Char_F6x8(0,4,"line 5");
  */
  
  handle_bluetooth_button(); // handles the bluetooth power button
  
  handle_lcd_buttons(); // handles the lcd buttons
  
  get_nmea_sentences(); // gets NMEA sentences out of the GPS and deals with them

} 
