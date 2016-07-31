// libraries
#include <SoftwareSerial.h> // serial library
#include <SD.h> // sd card library
#include <EEPROM.h> // EEPROM library

// local files
#include "functions.h" // useful functions
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

unsigned long bluetooth_button_press_time = millis(); // time of button press
unsigned long bluetooth_button_release_time = 0; // time of button release
bool flag_bluetooth_is_on = 0; // flag is BT device is powered on or off
bool flag_bluetooth_power_toggle_pressed = 0; // flag marks bluetooth button pressed or not - used to recognize button state change for proper high/low handling
bool flag_bluetooth_power_keep_on = 0;

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
  if (digitalRead(menu_left_buttton) == HIGH)
    Serial.println("left button press");

  if (digitalRead(menu_right_buttton) == HIGH)
    Serial.println("right button press");

  if (digitalRead(menu_up_buttton) == HIGH)
    Serial.println("up button press");

  if (digitalRead(menu_down_buttton) == HIGH)
    Serial.println("down button press");


  // bluetooth power on
  if (!flag_bluetooth_power_toggle_pressed && digitalRead(bluetooth_power_toggle_pin) == HIGH) // if button was not pressed and now gets pressed
  //  flag_bluetooth_power_toggle_pressed prevents mutiple executions of this if() block
  { 
    bluetooth_button_press_time = millis(); // record time of button press; this is used in eeprom_timer()
    digitalWrite(bluetooth_mosfet_gate_pin, HIGH); // turn on the device
    flag_bluetooth_is_on = 1; // set flag to on
    flag_bluetooth_power_toggle_pressed = 1; // mark button as pressed
  }

  // bluetooth power off
  if (flag_bluetooth_power_toggle_pressed && digitalRead(bluetooth_power_toggle_pin) == LOW) // if button was  pressed and now gets released
  // flag_bluetooth_power_toggle_pressed prevents mutiple executions of this if() block
  { 
    bluetooth_button_release_time = millis(); // record time of button press; this is used in 
    flag_bluetooth_power_toggle_pressed = 0; // mark button as released

    if (abs(bluetooth_button_release_time - bluetooth_button_press_time) > 500) // if button is held long enough
    {
      if (flag_bluetooth_power_keep_on) // if the BT device was on
        digitalWrite(bluetooth_mosfet_gate_pin, LOW); // turn off the BT device

      flag_bluetooth_power_keep_on = !flag_bluetooth_power_keep_on; // invert the flag (on -> off or off -> on)
    }
  }

  // bluetooth timed power off
  if ( (!flag_bluetooth_power_keep_on && flag_bluetooth_is_on) && eeprom_timer(bluetooth_button_press_time, 4)) // if the device is on and enough time has passed
  // flag_bluetooth_power_keep_on prevents the timer from kicking in if we want the BT device to stay on
  // flag_bluetooth_is_on prevents code execution on every loop
  { 
      digitalWrite(bluetooth_mosfet_gate_pin, LOW); // turn off the device
      flag_bluetooth_is_on = 0; // set flag to off
  }




  get_nmea_sentences();

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
