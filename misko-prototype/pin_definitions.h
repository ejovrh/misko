// define pins by function, order by pin number on board
#define gps_power_pin 4 // supplies power via PWM (100% duty cycle) to em406a
#define gps_green_led_pin 5 // LED1, blinks on write onto SD card
#define gps_red_led_pin 6 // LED2, on - GPS has fix, off - GPS has no fix

// UART Serial 1
// used for bluetooth and GPS modules

// UART Serial 2
//free

// UART Serial 3
//free

#define DO_NOT_USE_pin_10 10 // was SW SPI SD/SPI slave select - see Sd2Card.h:47
#define DO_NOT_USE_pin_11 11 // SD/SPI serial clock
#define DO_NOT_USE_pin_12 12 // SD/SPI master in slave out
#define DO_NOT_USE_pin_13 13 // SD/SPI master out slave in

//#define oled_scl_pin 21 // OLED SCL
//#define oled_sda_pin 20 // OLED SDA - MOSI
#define menu_left_buttton 30 // user menu - left button, the buttons draw each 0.5 mA (we have 5 of them)
#define menu_down_buttton 31 // user menu - down button
#define menu_up_buttton 32 // user menu - up button
#define menu_right_buttton 33 // user menu - right button
#define bluetooth_power_toggle_pin 40 // user menu - bluetooth power toggle
#define spi_ss_sd_card 45 // SD card SPI SS pin
#define spi_adxl345_ss_pin 46 // slave select for adxl345
#define spi_oled_a0_pin 47 // OLED address0 pin
#define bluetooth_mosfet_gate_pin 48 // bluetooth power control via mosfet
#define spi_oled_reset_pin 49 // OLED reset pin
#define spi_miso_pin 50 // SPI - Master In Slave Out - purple - connected are: ADXL345
#define spi_mosi_pin 51 // SPI - Master Out Slave In - orange - connected are: OLED, ADXL345
#define spi_sck_pin 52 // SPI - Serial Clock - green - connected are: OLED, ADXL345
#define spi_ss_pin 53 // SPI - Slave Select - blue - connected are: OLED, ADXL345
