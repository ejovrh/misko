// define pins by function, order by pin number on board
#define TMP36_Vsense_pin A0 // TMP36 temoerature sensor 97
#define bat_A_pin A1 // battery A voltage
//#define bat_B_pin A2 // battery B voltage
#define interrupt_adxl345_int1_pin 2 // goes to ADXL345 INT1 pin
#define gps_power_pin 4 // supplies power via PWM (100% duty cycle) to em406a
#define gps_green_led_pin 5 // LED1, blinks on write onto SD card
#define gps_red_led_pin 6 // LED2, on - GPS has fix, off - GPS has no fix
#define sleep_indicator_pin 7 // LED which lights up when the controller is sleeping

// UART Serial 1 (pins 18 and 19)
// free

// UART Serial 2 - (pins 16 and 17)
// used for SIM800L module

// UART Serial 3 (pins 14 and 15)
// used for bluetooth and GPS modules

// Pins used were SS 10, MOSI 11, MISO 12, and SCK 13.
#define DO_NOT_USE_pin_10 10 // SD/SPI SS -> SPI_SS_SD_card_pin; reason: see Sd2Card.h:47
#define DO_NOT_USE_pin_11 11 // SD/SPI SCK -> SPI_SCK_pin
#define DO_NOT_USE_pin_12 12 // SD/SPI MISO -> SPI_MISO_pin
#define DO_NOT_USE_pin_13 13 // SD/SPI MOSI -> SPI_MOSI_pin

//#define OLED_scl_pin 21 // OLED SCL
//#define OLED_sda_pin 20 // OLED SDA - MOSI
#define unassigned_6th_button_pin 29 // not yet implemented
#define menu_left_buttton 30 // user menu - left button, the buttons draw each 0.5 mA (we have 5 of them)
#define menu_down_buttton 31 // user menu - down button
#define menu_up_buttton 32 // user menu - up button
#define menu_right_buttton 33 // user menu - right button
#define MCP73871_battery_low_pin 35 // battery low indicator
#define MCP73871_charge_status_1_pin 36 // charge status indicator 1
#define MCP73871_charge_status_2_pin 37 // charge status indicator 1
#define MCP73871_power_good_indicator_pin 38 // supply power indicator
#define TMP36_shutdown_pin 39 // not yet implemented
#define bluetooth_power_toggle_pin 40 // user menu - bluetooth power toggle
#define SIM800L_mosfet_gate_pin 44 // SIM800L power control via mosfet
#define SPI_SS_SD_card_pin 45 // SD card SPI SS pin
#define SPI_SS_ADXL345_pin 46 // slave select for ADXL345(CS)
#define SPI_OLED_a0_pin 47 // OLED address0 pin
#define bluetooth_mosfet_gate_pin 48 // bluetooth power control via mosfet
#define SPI_OLED_reset_pin 49 // OLED reset pin (yellow)
#define SPI_MISO_pin 50 // SPI - MISO - blue - connected are: ADXL345(SDO)
#define SPI_MOSI_pin 51 // SPI - MOSI - orange - connected are: OLED, ADXL345(SDA)
#define SPI_SCK_pin 52 // SPI - SCK - green - connected are: OLED, ADXL345(SCL)
#define SPI_SS_OLED_pin 53 // SPI - SS - white - connected are: OLED(CS)
