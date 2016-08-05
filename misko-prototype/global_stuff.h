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
  const char gprmc[] = "$GPRMC"; // beginning of GPRMC sentence, used to fish out datetime via memcmp in gps_functions.h
  bool gps_fix = 0;
  char gps_date[9] = "20XXXXXX"; // 0-7 + 1 for '\0'
  char gps_time[7] = "XXXXXX"; // 0-5 + 1 for '\0'
  char gps_logfile[13] = "";
  char gps_latitude[12] = "Y hhmm.ssss"; // N or S
  char gps_longtitude[13] = "X hhhmm.ssss"; // W or E
  char gps_altitude[6] = "0000m";
  
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
  
// menu construction  
  M2_LABEL(el_space, "f0", " ");
  M2_LABEL(el_colon, "f0", "");
  
  M2_LABEL(el_gps_date, "f0", gps_date);
  M2_LABEL(el_gps_time, "f0", gps_time);
  M2_LIST(datetime) {&el_gps_time, &el_space, &el_gps_date} ;

  
  M2_HLIST(el_1st_line, NULL, datetime); //1st line
  M2_LABEL(el_longtitude, "f0", gps_longtitude); // 3rd line
  M2_LABEL(el_latitude, "f0", gps_latitude); // 2nd line

  M2_LIST(el_lines) = {&el_1st_line, &el_longtitude, &el_latitude};

  M2_VLIST(el_dispay, NULL, el_lines);
  M2_ALIGN(top_el_display, "-0|2", &el_dispay);

  M2tk m2(&top_el_display, m2_es_arduino, m2_eh_2bs, m2_gh_u8g_fb);

