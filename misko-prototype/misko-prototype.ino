// libraries
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <EEPROM.h> // EEPROM library
#include <IIC_without_ACK.h> // some obscure (chinese) I2C library for the SSD1306 OLED chipset
#include "oledfont.c"   // OLED fonts

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

// global variables start

// initalize OLED display
IIC_without_ACK OLED(oled_sda_pin, oled_scl_pin); // heavy C++ style instantation, declaration and everything...

char NMEA_buffer[NMEA_BUFFERSIZE] = "";        // string buffer for the NMEA sentence
unsigned int bufferid = 0; // holds the current position in the NMEA_buffer array, used fo walk through the buffer

char gps_date[9] = "20XXXXXX"; // 0-7 + 1 for '\0'
char gps_time[7] = "XXXXXX"; // 0-5 + 1 for '\0'
char gps_logfile[13] = "";

unsigned long bluetooth_button_press_time = millis(); // time of button press
unsigned long bluetooth_button_release_time = 0; // time of button release
bool flag_bluetooth_is_on = 0; // flag is BT device is powered on or off
bool flag_bluetooth_power_toggle_pressed = 0; // flag marks bluetooth button pressed or not - used to recognize button state change for proper high/low handling
bool flag_bluetooth_power_keep_on = 0;

unsigned long lcd_button_press_time = millis(); // time of button press
bool flag_lcd_is_on = 0; // flag is BT device is powered on or off
bool flag_lcd_button_down_pressed = 0; // flag marks button pressed or not
bool flag_lcd_button_up_pressed = 0; // flag marks button pressed or not
bool flag_lcd_button_left_pressed = 0; // flag marks button pressed or not
bool flag_lcd_button_right_pressed = 0; // flag marks button pressed or not

char debug_out[80] = "";
// global variables end

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
   int printed = 0;
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

        /*
          // poor mans debugging
          if (digitalRead(bluetooth_power_toggle_pin) == HIGH && printed == 0)
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
         */
} 
