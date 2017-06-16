//   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE IN setup.h
#define CONFIG_VERSION 5 // protection against excessive EEPROM writes
//   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE IN setup.h

#define BUFFER_DEBUG_PRINT 0 // SD write debug printout
#define AREF_VOLTAGE 2.50
#define GPSRATE 4800
#define SERIALRATE 9600
#define NMEA_BUFFERSIZE 82 // officially, NMEA sentences are at maximum 82 characters long (80 readable characters + \r\n)
#define SD_BUFFERSIZE 1024 // cyclical buffer for NMEA sentences to be written to SD card
#define GPS_FITNESS_MODE_THRESHOLD 10 // theshold in knots/h for transition from fitness mode <-> normal mode

// GPS variables
char NMEA_buffer[NMEA_BUFFERSIZE] = "";	// string buffer for the NMEA sentence
char FERAM_GPS_LAST_GOOD_POSITION[82] = ""; // stores last good position
uint8_t bufferid = 0; // holds the current position in the NMEA_buffer array, used for walk through the buffer
char gps_command_buffer[24];
char gps_date[9] = "20------"; // YYYYMMDD derived from NMEA sentence
char gps_time[7] = "------"; // 0-5 + 1 for '\0'
char gps_logfile[22] = "";
static char gps_latitude[16] = "lat ----.----  "; // N or S, memcpy needs to start to write at pos 4 ( populated in gps_functions.h:gps_parse_gprmc() )
static char gps_longtitude[17] = "lon -----.----  "; // W or E, memcpy needs to start to write at pos 4 ( populated in gps_functions.h:gps_parse_gprmc() )
static char gps_altitude[5]; // GPS altitude: [xxxx or -xxx], populated in gps_functions.h:gps_parse_gpgga()
static char gps_hdop[5]; // GPS horizontal dilution of position [0.99 - 99.99], populated in gps_functions.h:gps_parse_gprmc()
static char gps_satellites_in_view[3]; // GPS satellites in view [00 - 99]
static char gps_position_fix_indicator; // indicates the type of fix
static uint8_t gps_speed = 0; // gps speed in knots (only the interger part of the speed is relevant)
bool flag_gps_fitness_is_set = 1; // is the fitness mode set or not?
bool flag_gps_fix = 0; // do we have a fix or not?
bool flag_gps_on = 1; // is the gps powered on or off?
uint8_t gps_gnrmc_age = 0; // age of a GNRMC relative to uptime
uint8_t gps_gngga_age = 0; // age of a GNGGA relative to uptime
int8_t timezone;

// SIM800C variables
bool flag_gprs_push_pressed = 0; // flag marks GSM device button pressed or not - used to recognize button state change for proper high/low handling
char gsm_apn[64] = "web.htgprs"; // APN for the GPRS connection (per protocol max 63 octets)
char gsm_http_server[64] = ""; // HTTP server for log upload (arbitrary lenght)
char gsm_http_cred[11] = ""; // HTTP server credentials (arbitrary lenght)

// device variables
char bat_a_pct[9] = "batAxxx%";
char bat_b_pct[9] = "batBxxx%";
char sd_buffer[SD_BUFFERSIZE]; // buffer holding 2x 512byte blocks of NMEA sentences for buffered write of 512byte blocks
char statistics_buffer[SD_BUFFERSIZE] = ""; // buffer for statistical data which ends up written to SD
byte adxl345_irq_src; // holds INT_SRC - a register in the ADXL345 via which it is determined which interrupt was triggered

// Bluetooth flags
uint32_t bluetooth_button_press_time = millis(); // time of button press
uint32_t bluetooth_button_release_time = 0; // time of button release
bool flag_bluetooth_is_on = 0; // flag is BT device is powered on or off
bool flag_bluetooth_power_toggle_pressed = 0; // flag marks Bluetooth button pressed or not - used to recognize button state change for proper high/low handling
bool flag_bluetooth_power_keep_on = 0; // flag if the BT device shall be kept on and not power off on timeout

// display variables
uint32_t lcd_button_press_time = millis(); // time of button press
bool flag_lcd_is_on = 0; // flag is BT device is powered on or off
bool flag_oled_sleep = 0; // flag if the OLED shall sleep or not
M2_EXTERN_ALIGN(top_el_expandable_menu); // Forward declaration of the top level element
M2_EXTERN_ALIGN(el_top_sd_content_menu); // Forward declaration of the top level element
const char format_rf0[] = "rf0"; // menu format string
const char format_1W64H64[] = "-0|1W64H64"; //

// display device initializations
//U8GLIB_SSD1306_128X64 OLED(U8G_I2C_OPT_FAST);
U8GLIB_SH1106_128X64_2X OLED(SPI_SS_OLED_pin,  SPI_OLED_a0_pin,  SPI_OLED_reset_pin); // HW SPI - look in the library source for precise info

// software serial for the GPS device
SoftwareSerial gps(GPS_sw_serial_rx, GPS_sw_serial_tx);

// set up variables using the SD utility library functions:
File gpslogfile; // file object for the log file
bool flag_sd_write_enable = 0; // flag if a write shall be allowed or not - is controlled by log file name initialization
uint8_t fs_m2tk_first = 0; // helper variable for the strlist element
uint8_t fs_m2tk_cnt = 0; // helper variable for the strlist element

uint16_t uptime = 0; // uptime in seconds, incrementer via timer5
volatile bool flag_adxl345_int1 = 0; // flag for active interrupt
bool flag_gsm_on = 0; // flag if the gsm modem shall be powered on or not, controlled via menu combo, inspected in loop()
uint8_t flag_cb_gsm_power = 0; // used in GSM power callback
bool flag_run_once = 0; // runs exactly once, triggered by timer5

float val_Vcc; // measured Vcc, updated in timer5 ISR
float val_temperature; // measured temperature, updated in timer5 ISR
uint8_t val_batA_pct; // battA percentage, updated in timer5 ISR
uint8_t val_batB_pct; // battB percentage, updated in timer5 ISR
