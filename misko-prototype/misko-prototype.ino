// libraries
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <EEPROM.h> // EEPROM library
#include <U8glib.h> // https://github.com/olikraus/u8glib/wiki/userreference
#include "M2tk.h" // https://code.google.com/archive/p/m2tklib/
#include "utility/m2ghu8g.h"

// local files
#include "gps_config.h" // gps stuff
#include "pin_definitions.h" // pin layout
#include "global_stuff.h" // global variables et al.
#include "functions.h" // useful functions

void setup() 
{
  #include "setup.h"
} // setup end 

#include "gps_functions.h" // gps functions

void loop() 
{
     m2.checkKey();

    if (m2.handleKey() != 0)
    {
      // picture loop - https://github.com/olikraus/u8glib/wiki/tpictureloop
      OLED.firstPage();  // https://github.com/olikraus/u8glib/wiki/userreference#firstpage
      do 
      {
        draw(); // defined in functions.h
      }
      while( OLED.nextPage() ); // https://github.com/olikraus/u8glib/wiki/userreference#nextpage
    } 
    
  handle_bluetooth_button(); // handles the bluetooth power button
  
  handle_lcd_buttons(); // handles the lcd buttons
  
  get_nmea_sentences(); // gets NMEA sentences out of the GPS and deals with them

} 
