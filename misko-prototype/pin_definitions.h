// define pins by function, order by pin number on board
#define gps_power_pin 4 // supplies power via PWM (100% duty cycle) to em406a
#define gps_green_led_pin 5 // LED1, blinks on write onto SD card
#define gps_red_led_pin 6 // LED2, on - GPS has fix, off - GPS has no fix

// UART Serial 2
//free

// UART Serial 3
//free

#define sd_ss_pin 10 // SD/SPI slave select
#define sd_sck_pin 11 // SD/SPI serial clock
#define sd_miso_pin 12 // SD/SPI master in slave out
#define sd_mosi_pin 13 // SD/SPI master out slave in

#define bluetooth_power_button_pin 40
#define bluetooth_mosfet_gate_pin 48 // bluetooth power control via mosfet
#define lcd_mosfet_gate_pin 49 // LCD power control via mosfet
#define spi_miso_pin 50 // SPI - Master In Slave Out - not used yet
#define spi_mosi_pin 51 // SPI - Master Out Slave In - not used yet
#define spi_sck_pin 52 // SPI - Serial Clock - not used yet
#define spi_ss_pin 53 // SPI - Slave Select - not used yet

