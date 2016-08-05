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

// initalize u8g object
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // Display which does not send AC
U8GLIB_SSD1306_128X64 OLED(U8G_I2C_OPT_FAST);

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
  
  bool u8g_redraw_required = 1; // flags the U8glib picture loop to execute
  
  uint32_t number = 1234;
  M2_U32NUM(el_num, "a0c4", &number);

  //M2_LABEL(hello_world_label, NULL, "foXXXo World"); // https://code.google.com/archive/p/m2tklib/wikis/t01u8g.wiki 
  M2tk m2(&el_num, m2_es_arduino, m2_eh_4bs, m2_gh_u8g_fb); // https://code.google.com/archive/p/m2tklib/wikis/t01u8g.wiki
  


  
