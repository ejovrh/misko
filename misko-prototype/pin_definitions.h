#define interrupt_0_pin 2 // adxl345 int 1

#define gps_power_pin 4 // Use pin 4 to control power to the GPS
#define gps_green_led_pin 5 // blinks on write onto SD card
#define gps_red_led_pin 6 // blinks on lock

#define bluetooth_mosfet_gate 7 // goes to gate of mosfet, controlling power to bluetooth transmitter
#define bluetooth_activate_button 52 // button to power up the transmitter

// UART Serial 2
//free

// UART Serial 3
//free
#define sd_ss_pin 10 // SD/SPI slave select
#define sd_sck_pin 11 // SD/SPI serial clock
#define sd_miso_pin 12 // SD/SPI master in slave out
#define sd_mosi_pin 13 // SD/SPI master out slave in
#define battery_pin 14 // battery voltage
 
//#define not_connected_yet 42 
#define temperature_pin 44 // temperature sensor

