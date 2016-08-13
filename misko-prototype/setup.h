#include "pin_modes.h"
#include "eeprom_config.h"

analogReference(EXTERNAL); // external voltage reference - Vcc (measured in functions.h readVcc() 

Serial.begin(SERIALRATE); // connect to the serial terminal
Serial.println(F("start"));

// initialize GPS
Serial3.begin(GPSRATE);   // connect to the GPS at the desired rate
Serial3.print(SERIAL_SET); // set gps serial comm. parameter
Serial3.print(WAAS_ON); // will become obsolete via EEPROM and setup
Serial3.print(RMC_ON); // will become obsolete via EEPROM and setup
Serial3.print(GSA_ON); // will become obsolete via EEPROM and setup
//Serial3.print(GSV_ON); // will become obsolete via EEPROM and setup
    
delay(50);
  
m2_SetU8g(OLED.getU8g(), m2_u8g_box_icon); // connect u8glib with m2tklib
m2.setFont(0, u8g_font_6x10); // assign u8g_font_6x10 font to index 0
m2.setPin(M2_KEY_SELECT, menu_right_buttton); // 33
m2.setPin(M2_KEY_PREV, menu_up_buttton); // 32
m2.setPin(M2_KEY_NEXT, menu_down_buttton); // 31
m2.setPin(M2_KEY_EXIT, menu_left_buttton); // 30

// we'll use the initialization code from the utility libraries
// since we're just testing if the card is working!
if (!card.init(SPI_FULL_SPEED, spi_ss_sd_card)) 
{
  Serial.println(F("initialization failed. Things to check:"));
}
else 
{
    Serial.println(F("Wiring is correct and a card is present."));
}
