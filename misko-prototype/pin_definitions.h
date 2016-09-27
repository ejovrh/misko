// define pin function, order by port pin in comment

// port A
#define MCP73871_power_good_pin 24			// PA0 - MCP73871 supply power indicator
#define MCP73871_charge_status_2_pin 25	// PA1 - MCP73871 charge status indicator 2
#define MCP73871_charge_status_1_pin 26	// PA2 - charge status indicator 1 
											// PA3 - not used
											// PA3 - not used
											// PA4 - not used
											// PA5 - not used

// port B
#define SPI_FRAM_SS_pin 8							// PB0 - SPI - SS FRAM
#define SPI_SCK_pin 9 								// PB1 - SPI - SCK - green - connected are: OLED, ADXL345(SCL)
#define SPI_MOSI_pin 10 							// PB2 - SPI - MOSI - orange - connected are: OLED, ADXL345(SDA)
#define SPI_MISO_pin 11 							// PB3 - SPI - MISO - blue - connected are: ADXL345(SDO)
#define GPS_sw_serial_rx 12 					// PB4 - software serial RX for GPS
#define GPS_sw_serial_tx 13 					// PB5 - software serial TX for GPS
#define interrupt_adxl345_int1_pin 14 // PB6 - ADXL345 INT1 pin // FIXME !!!
#define Bluetooth_wakeup_pin 15				// PB7 - wakeup signal for Bluetooth device

// port C
#define menu_left_button_pin 32				// PC0 - user menu - left button, the buttons draw each 0.5 mA (we have 5 of them)
#define menu_bluetooth_power_toggle_pin 33 	// PC1 - user menu - Bluetooth power toggle
#define menu_down_button_pin 34				// PC2 - user menu - down button
#define menu_up_button_pin 35					// PC3 - user menu - up button
#define menu_gprs_push_button_pin 36	// PC4 - user menu - GPRS push position
#define menu_right_button_pin 37 			// PC5 - user menu - right button
#define gps_red_led_pin 38 						// PC6 - LED2, on - GPS has fix, off - GPS has no fix
#define gps_green_led_pin 39 					// PC7 - LED1, blinks on write onto SD card

// port D
#define I2C_SCL_pin	21								// PD0 - I2C SCL
#define I2C_SDA_pin	20								// PD1 - I2C SDA
#define SIM800C_rx_pin 19  						// PD2 - Serial1 RX
#define SIM800C_tx_pin 18  						// PD3 - Serial1 TX
#define SIM800C_cts_pin 20						// PD4 - SIM800C CTS 
#define SIM800C_status_pin	21				// PD5 - SIM800C status pin SIM800C
#define SIM800C_power_pin	22 					// PD6 - SIM800C power on/off
#define GPS_power_ctl_pin 23					// PD7 - GPS power on/off

// port E
#define PDI_UART0_RX_pin 0						// PE0 - Programming Data Interface - Serial RX
#define PDI_UART0_TX_pin 1						// PE1 - Programming Data Interface - Serial TX
#define SPI_SS_ADXL345_pin 2					// PE2 - ADXL345 Slave Select
#define SPI_OLED_reset_pin 3					// PE3 - OLED reset pin
#define SPI_OLED_a0_pin 4 	 					// PE4 - OLED a0 (D/C)
#define SPI_SS_OLED_pin 5 						// PE5 - SPI - SS - OLED
#define SPI_SS_SD_card_pin 6 					// PE6 - SPI - SS - SD card
#define SD_card_detect_pin 7					// PE7 - SD card detect

// port F
#define TMP36_Vsense_pin 46 					// PF0 - TMP36 temperature sensor voltage out
#define TMP36_shutdown_pin 47					// PF1 - TMP36 shutdown pin
#define bat_A_pin 48 									// PF2 - battery A voltage
									// PF3 - not used
#define JTAG_TCL_pin 50								// PF4 - JTAG TCK
#define JTAG_TMS_pin 51								// PF5 - JTAG TMS
#define JTAG_TDO_pin 52								// PF6 - JTAG TDO
#define JTAG_TDI_pin 53								// PF7 - JTAG TDI

// port G
									// PG0 - not used
									// PG1 - not used
									// PG2 - not used
#define analog_sw_ctrl_pin 43					// PG3 - analog switch control
#define GPS_wakeup_pin 44							// PG4 - GPS wakeup 
#define PDI_UART0_CTS_pin	45					// PG5 - PDI CTS
