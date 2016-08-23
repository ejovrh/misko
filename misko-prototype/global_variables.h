//   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE IN setup.h
#define CONFIG_VERSION 4 // protection against excessive EEPROM writes
//   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE IN setup.h

#define AREF_VOLTAGE 4.27
#define NMEA_DEBUG_PRINT 1
#define TEMPERATURE_SAMPLE_PERIOD 10 // temperature measure interval in seconds
#define GPSRATE 4800
#define SERIALRATE 9600
#define NMEA_BUFFERSIZE 82 // officially, NMEA sentences are at maximum 82 characters long (80 readable characters + \r\n)
#define SD_BUFFERSIZE 512 // huge buffer for NMEA sentences to be written to SD card

// EERPOM indices
#define EERPOM_LCD_POWER_INDEX 1
#define EERPOM_BLUETOOTH_POWER_INDEX 2
#define EERPOM_LCD_AUTO_TIMEOUT_INDEX 3
#define EERPOM_BLUETOOTH_AUTO_TIMEOUT_INDEX 4
#define EERPOM_TIMEZONE_INDEX 5
#define EEPROM_GPS_GPRMC_GGA_FREQ_INDEX 6
#define EEPROM_GPS_USE_WAAS_INDEX 7

// GPS variuables
char NMEA_buffer[NMEA_BUFFERSIZE] = "";        // string buffer for the NMEA sentence
uint8_t bufferid = 0; // holds the current position in the NMEA_buffer array, used fo walk through the buffer
char gps_command_buffer[24];
bool gps_fix = 0;
char gps_date[9] = "20XXXXXX"; // 0-7 + 1 for '\0' -- YEAR 2100-BUG, HERE WE COME!!!
char gps_time[7] = "XXXXXX"; // 0-5 + 1 for '\0'
char gps_utc[7] = "UTC+2"; // timezone string
char gps_logfile[13] = "";
char gps_latitude[16] = "lat hhmm.ssss  "; // N or S, memcpy needs to start to write at pos 4 ( populated in gps_functions.h:gps_parse_gprmc() )
char gps_longtitude[17] = "lon hhhmm.ssss  "; // W or E, memcpy needs to start to write at pos 4 ( populated in gps_functions.h:gps_parse_gprmc() )
char gps_altitude[9] = "alt    m"; // GPS altitude: "altxxxxm" or "alt-xxxm", populated in gps_functions.h:gps_parse_gpgga()
char gps_hdop[8] = "dop____"; // GPS horizontal dilution of position: "dop12.5" , populated in gps_functions.h:gps_parse_gprmc()
char gps_satellites_in_view[6] = "sat__"; // GPS satellites in view
char gps_wk[5] = "xxxx";
bool flag_gps_wk_set = 0;
char gps_tow[7] = "xxxxxx";
bool flag_gps_tow_set = 0;
int8_t timezone;

// device variables
int8_t temperature = 0; // temperature in degrees Celsius
char temp[6] = "T+xxC";
uint16_t avg_temp = 0;
char vcc[9] = "Vccx.xxV";
char bat_a_pct[9] = "batAxxx%";
char sd_buffer1[SD_BUFFERSIZE];
char sd_buffer2[SD_BUFFERSIZE];

// bluetooth flags
uint32_t bluetooth_button_press_time = millis(); // time of button press
uint32_t bluetooth_button_release_time = 0; // time of button release
bool flag_bluetooth_is_on = 0; // flag is BT device is powered on or off
bool flag_bluetooth_power_toggle_pressed = 0; // flag marks bluetooth button pressed or not - used to recognize button state change for proper high/low handling
bool flag_bluetooth_power_keep_on = 0;

// display variables
uint32_t lcd_button_press_time = millis(); // time of button press
bool flag_lcd_is_on = 0; // flag is BT device is powered on or off
bool oled_sleep = 0;
M2_EXTERN_ALIGN(top_el_expandable_menu); // Forward declaration of the toplevel element

// display device initializations
//U8GLIB_SSD1306_128X64 OLED(U8G_I2C_OPT_FAST);
U8GLIB_SH1106_128X64_2X OLED(SPI_SS_OLED_pin,  SPI_OLED_a0_pin,  SPI_OLED_reset_pin); // HW SPI - look in the library source for precise info

// set up variables using the SD utility library functions:
File gpslogfile;
bool sd_write_enable = 0;

uint16_t scheduler_run_count = 0;
volatile bool adxl345_int1 = 0;
bool flag_gsm_on = 0;
uint8_t flag_cb_gsm_power = 0; // used in gsm power callback
