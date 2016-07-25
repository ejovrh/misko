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

#define lcd_mosfet_gate_pin 49 // LCD power control via mosfet
  #define weird_behaviour_pin_50 50 // FIXME: exibits weird behaviour - high/low dont change state?
  #define weird_behaviour_pin_51 51 // FIXME: exibits weird behaviour - high/low dont change state?
  #define weird_behaviour_pin_52 52 // FIXME: exibits weird behaviour - high/low dont change state?
#define bluetooth_mosfet_gate_pin 53 // bluetooth power control via mosfet

