//   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE IN setup.h
  #define CONFIG_VERSION 2 // protection against excessive EEPROM writes
//   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE IN setup.h

#define GPSRATE 4800
#define SERIALRATE 9600
//BLUETOOTHSERIALRATE is hardcoded in device
#define NMEA_BUFFERSIZE 80 // plenty big

// GPS variuables
  char NMEA_buffer[NMEA_BUFFERSIZE] = "";        // string buffer for the NMEA sentence
  unsigned int bufferid = 0; // holds the current position in the NMEA_buffer array, used fo walk through the buffer
  char gps_date[9] = "20XXXXXX"; // 0-7 + 1 for '\0'
  char gps_time[7] = "XXXXXX"; // 0-5 + 1 for '\0'
  char gps_logfile[13] = "";

// bluetooth flags
  unsigned long bluetooth_button_press_time = millis(); // time of button press
  unsigned long bluetooth_button_release_time = 0; // time of button release
  bool flag_bluetooth_is_on = 0; // flag is BT device is powered on or off
  bool flag_bluetooth_power_toggle_pressed = 0; // flag marks bluetooth button pressed or not - used to recognize button state change for proper high/low handling
  bool flag_bluetooth_power_keep_on = 0;

// LCD flags
  unsigned long lcd_button_press_time = millis(); // time of button press
  bool flag_lcd_is_on = 0; // flag is BT device is powered on or off
  bool flag_lcd_button_down_pressed = 0; // flag marks button pressed or not
  bool flag_lcd_button_up_pressed = 0; // flag marks button pressed or not
  bool flag_lcd_button_left_pressed = 0; // flag marks button pressed or not
  bool flag_lcd_button_right_pressed = 0; // flag marks button pressed or not

