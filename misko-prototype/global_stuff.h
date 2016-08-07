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
const char gpgga[] = "$GPGGA"; // beginning of GPGGA sentence, used to fish out datetime via memcmp in gps_functions.h 
bool gps_fix = 0;
char gps_date[9] = "20XXXXXX"; // 0-7 + 1 for '\0' -- YEAR 2100-BUG, HERE WE COME!!!
char gps_time[7] = "XXXXXX"; // 0-5 + 1 for '\0'
char gps_utc[7] = "UTC+2"; // timezone string
char gps_logfile[13] = "";
char gps_latitude[16] = "lat hhmm.ssss  "; // N or S, memcpy needs to start to write at pos 4 ( populated in gps_functions.h:gps_parse_gprmc() )
char gps_longtitude[17] = "lon hhhmm.ssss  "; // W or E, memcpy needs to start to write at pos 4 ( populated in gps_functions.h:gps_parse_gprmc() )
char gps_altitude[10] = "alt "; // GPS altitude: "alt xxxxm" or "alt -xxxm", populated in gps_functions.h:gps_parse_gpgga()
char gps_hdop[6] = "D"; // GPS horizontal dilution of position: "D12.5" , populated in gps_functions.h:gps_parse_gprmc()
char gps_satellites_in_view[4] = "S"; // GPS satellites in view
char temperature[6] = "T+30C"; // temperature, "T-12C" or "T+56C"

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



// display construction
M2_LABEL(el_space, "f0", ' '); // label for space character

// 1st line begin
M2_LABEL(el_gps_date, "rf0", gps_date); // label for gps_date
M2_LABEL(el_gps_time, "rf0", gps_time); // label for gps_time
M2_LABEL(el_gps_utc, "rf0", gps_utc); // timezone string
M2_LIST(datetime) { &el_gps_date, &el_space, &el_gps_time, &el_gps_utc} ; // create a list of gps date, time, timezone
M2_HLIST(el_1st_line, NULL, datetime); // 1st line
// 1st line end

// 2nd line begin
M2_LABEL(el_gps_latitude, "rf0", gps_latitude); 
M2_LABEL(el_gps_hdop, "rf0", gps_hdop);
M2_LIST(el_lat_hdop) = {&el_gps_latitude, &el_space, &el_gps_hdop};
M2_HLIST(el_2nd_line, NULL, el_lat_hdop); // 2nd line
// 2nd line end

// 3rd line begin
M2_LABEL(el_gps_longtitude, "rf0", gps_longtitude);
M2_LABEL(el_gps_sat_in_view, "rf0", gps_satellites_in_view);
M2_LIST(el_lon_sat) = {&el_gps_longtitude, &el_space, &el_gps_sat_in_view};
M2_HLIST(el_3rd_line, NULL, el_lon_sat); // 3rd line
// 3rd line begin

// 4th line begin
M2_LABEL(el_altitude, "rf0", gps_altitude);
M2_LABEL(el_temperature, "rf0", temperature);
M2_LIST(el_alt_temp) = {&el_altitude, &el_space, &el_temperature};
M2_HLIST(el_4th_line, NULL, el_alt_temp); // 4th line
// 4th line begin

// 5th line begin
M2_LABEL(el_batt_a, "rfo", "batA 100%");
M2_LABEL(el_batt_b, "rfo", "batB 100%");
M2_LIST(el_battery) = {&el_batt_a, &el_space, &el_batt_b};
M2_HLIST(el_5th_line, NULL, el_battery); // 5th line
// 5th line begin

// 6th line begin
M2_LABEL(el_bluetooth, "rf0", "bt");
M2_LABEL(el_accel, "rf0", "accel");
M2_LIST(el_misc) = {&el_bluetooth, &el_space, &el_accel};
M2_HLIST(el_6th_line, NULL, el_misc); // 6th line
// 6th line begin

M2_LIST(el_lines) = {&el_1st_line, &el_2nd_line, &el_3rd_line, &el_4th_line, &el_5th_line, &el_6th_line}; // line up elements

M2_VLIST(el_dispay, NULL, el_lines); // list the lineup into a vertical list
M2_ALIGN(top_el_display, "-0|2", &el_dispay); // align it top&left

M2tk m2(&top_el_display, m2_es_arduino, m2_eh_2bs, m2_gh_u8g_fb); // push it into the display object

