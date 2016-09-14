// define pins by function, order by port pin in comment

// port A - ok
#define MCP73871_power_good_pin 22			// PA0 - MCP73871 supply power indicator
#define MCP73871_charge_status_2_pin 23	// PA1 - MCP73871 charge status indicator 2
#define MCP73871_charge_status_1_pin 24	// PA2 - charge status indicator 1 
																				// PA3 - not used
																				// PA3 - not used
																				// PA4 - not used
																				// PA5 - not used

// port B - ok
#define SPI_FRAM_SS_pin  								// PB0 - SPI - SS FRAM
#define SPI_SCK_pin 52 									// PB1 - SPI - SCK - green - connected are: OLED, ADXL345(SCL)
#define SPI_MOSI_pin 51 								// PB2 - SPI - MOSI - orange - connected are: OLED, ADXL345(SDA)
#define SPI_MISO_pin 50 								// PB3 - SPI - MISO - blue - connected are: ADXL345(SDO)
#define GPS_sw_serial_rx 10 						// PB4 - software serial RX for GPS
#define GPS_sw_serial_tx 11 			 			// PB5 - software serial TX for GPS
#define interrupt_adxl345_int1_pin 12 	// PB6 - ADXL345 INT1 pin // FIXME !!!
#define Bluetooth_wakeup_pin 13					// PB7 - wkaeup signal for Bluetooth device

// port C - ok
#define menu_left_button_pin 37						// PC0 - user menu - left button, the buttons draw each 0.5 mA (we have 5 of them)
#define menu_bluetooth_power_toggle_pin 36 	// PC1 - user menu - bluetooth power toggle
#define menu_down_button_pin 35						// PC2 - user menu - down button
#define menu_up_button_pin 34							// PC3 - user menu - up button
#define menu_gprs_push_button_pin 33 							// PC4 - user menu - GPRS push position
#define menu_right_button_pin 32 					// PC5 - user menu - right button
#define gps_red_led_pin 30 							// PC6 - LED2, on - GPS has fix, off - GPS has no fix
#define gps_green_led_pin 31 						// PC7 - LED1, blinks on write onto SD card

// port D - (! mega)
#define I2C_SCL_pin	21									// PD0 - I2C SCL
#define I2C_SDA_pin	20									// PD1 - I2C SDA
#define SIM800C_rx_pin 19  							// PD2 - Serial1 RX
#define SIM800C_tx_pin 18  							// PD3 - Serial1 TX
// (!mega) #define SIM800C_cts_pin 			// PD4 - SIM800C CTS 
// (!mega) #define SIM800C_status_pin		// PD5 - SIM800C status pin SIM800C
#define SIM800C_power_pin	29 						// PA7 - AS A STANDIN ON ATMEGA2560 // PD6 - SIM800C power on/off (! mega)
#define GPS_power_ctl_pin 38						// PD7 - GPS power on/off

// port E - (! mega)
#define PDI_UART0_RX_pin								// PE0 - Programming Data Interface - Serial RX
#define PDI_UART0_TX_pin								// PE1 - Programming Data Interface - Serial TX
#define SPI_SS_ADXL345_pin 44						// (! mega) PL5 (44) AS A STANDIN ON ATMEGA2560 // PE2 - ADXL345 Slave Select (! mega)
#define SPI_OLED_reset_pin 5						// PE3 - OLED reset pin (yellow)
#define SPI_OLED_a0_pin 2 	 						// PE4 - OLED a0 (D/C) pin (white) 
#define SPI_SS_OLED_pin 3 							// PE5 - SPI - SS - white - connected are: OLED(CS)
#define SPI_SS_SD_card_pin 46 					// (! mega) PL6 AS A STANDIN ON ATMEGA2560 // PE6 - SPI - SS - SD card (! mega)
#define SD_card_detect_pin 							// PE7 - SD card detect

// port F - ok
#define TMP36_Vsense_pin A0 						// PF0 - TMP36 temoerature sensor voltage out
#define TMP36_shutdown_pin A1						// PF1 - TMP36 shutdown pin
#define bat_A_pin A2 										// PF2 - battery A voltage
																				// PF3 - not used
#define JTAG_TCL_pin 4									// PF4 - JTAG TCK
#define JTAG_TMS_pin 5									// PF5 - JTAG TMS
#define JTAG_TDO_pin 6									// PF6 - JTAG TDO
#define JTAG_TDI_pin 7									// PF7 - JTAG TDI

// port G - (! mega)

																				// PG0 - not used
																				// PG1 - not used
																				// PG2 - not used
// (! mega) #define analog_cw_ctrl_pin	// PG3 - analog switch control (! mega)
// (! mega) #define GPS_wakeup_pin			// PG4 - GPS wakeup (! mega)
#define PDI_UART0_CTS_pin	4							// PG5 - PDI CTS
