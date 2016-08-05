#include "pin_modes.h"
#include "eeprom_config.h"


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
  
/*
FIXME: seems to crash the graphics library
  // initialize SD card  
  if (!SD.begin(sd_ss_pin)) // see if the card is present and can be initialized
  {  
    Serial.println("SD Card failed, or not present");
    return;
  }  
*/

m2_SetU8g(OLED.getU8g(), m2_u8g_box_icon); // connect u8glib with m2tklib
m2.setFont(0, u8g_font_6x10); // assign u8g_font_6x10 font to index 0
m2.setPin(M2_KEY_SELECT, menu_up_buttton); // up
m2.setPin(M2_KEY_PREV, menu_left_buttton); // left
m2.setPin(M2_KEY_NEXT, menu_right_buttton); // right
m2.setPin(M2_KEY_HOME, menu_down_buttton); // down

