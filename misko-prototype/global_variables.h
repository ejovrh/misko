//   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE IN setup.h
#define CONFIG_VERSION 3 // protection against excessive EEPROM writes
//   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE IN setup.h

#define TEMPERATURE_SAMPLE_PERIOD 10 // temperature measure interval in seconds
#define GPSRATE 4800
#define SERIALRATE 9600
//BLUETOOTHSERIALRATE is hardcoded in device
#define NMEA_BUFFERSIZE 80 // plenty big
#define SD_BUFFERSIZE 4096 // huge buffer for NMEA sentences to be written to SD card

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
char temperature[6] = "T+xxC"; // temperature, "T-12C" or "T+56C"
int8_t timezone;
char sd_buffer[SD_BUFFERSIZE];

// bluetooth flags
uint32_t bluetooth_button_press_time = millis(); // time of button press
uint32_t bluetooth_button_release_time = 0; // time of button release
bool flag_bluetooth_is_on = 0; // flag is BT device is powered on or off
bool flag_bluetooth_power_toggle_pressed = 0; // flag marks bluetooth button pressed or not - used to recognize button state change for proper high/low handling
bool flag_bluetooth_power_keep_on = 0;

// LCD flags
uint32_t lcd_button_press_time = millis(); // time of button press
bool flag_lcd_is_on = 0; // flag is BT device is powered on or off
bool flag_lcd_button_down_pressed = 0; // flag marks button pressed or not
bool flag_lcd_button_up_pressed = 0; // flag marks button pressed or not
bool flag_lcd_button_left_pressed = 0; // flag marks button pressed or not
bool flag_lcd_button_right_pressed = 0; // flag marks button pressed or not

// device initializations
U8GLIB_SH1106_128X64_2X OLED(SPI_SS_OLED_pin,  SPI_OLED_a0_pin,  SPI_OLED_reset_pin); // HW SPI - look in the library source for precise info

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

uint32_t temperature_last_reading = 0;
