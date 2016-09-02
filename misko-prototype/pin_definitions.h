// define pins by function, order by pin number on board
#define GPS_PPS_pin 3 // GPS PPS signal
#define sleep_indicator_pin 7 // LED which lights up when the controller is sleeping

// port A
																				// PA0 - FREE (digital 22)
																				// PA1 - FREE (digital 23)
																				// PA2 - FREE (digital 24)
#define TMP36_shutdown_pin	25					// PA3 - shutdown pin on TMP36
#define gps_power_pin 28 								// PA6 - supplies power via PWM (100% duty cycle) to em406a
#define SIM800L_mosfet_gate_pin 29 			// PA7 - SIM800L power control via mosfet

// port B
#define SPI_SS_SD_card_pin 13 					// PB7 - SPI - SS - SD card
#define SPI_SS_OLED_pin 12 							// PB6 - SPI - SS - white - connected are: OLED(CS)
#define SIM800L_sw_serial_tx 11 			 	// PB5 - software serial TX for SIM800L
#define SIM800L_sw_serial_rx 10 				// PB4 - software serial RX for SIM800L
#define SPI_MISO_pin 50 								// PB3 - SPI - MISO - blue - connected are: ADXL345(SDO)
#define SPI_MOSI_pin 51 								// PB2 - SPI - MOSI - orange - connected are: OLED, ADXL345(SDA)
#define SPI_SCK_pin 52 									// PB1 - SPI - SCK - green - connected are: OLED, ADXL345(SCL)
#define SPI_SS_ADXL345_pin 53						// PB0 - slave select for ADXL345(CS)

// port C
#define bluetooth_power_toggle_pin 37 	// PC0 - user menu - bluetooth power toggle
#define menu_right_buttton 36 					// PC1 - user menu - right button
#define menu_up_buttton 35 							// PC2 - user menu - up button
#define menu_down_buttton 34 						// PC3 - user menu - down button
#define menu_left_buttton 33 						// PC4 - user menu - left button, the buttons draw each 0.5 mA (we have 5 of them)
#define unassigned_6th_button_pin 32 		// PC5 - not yet implemented
#define gps_red_led_pin 31 							// PC6 - LED2, on - GPS has fix, off - GPS has no fix
#define gps_green_led_pin 30 						// PC7 - LED1, blinks on write onto SD card

// port D
#define TWI_SCL_pin	21									// PD0 - TWI SCL
#define TWI_SDA_pin	20									// PD1 - TWI SDA
#define USART1_RX 19  									// PD2 - Serial1 RX
#define USART1_TX 18  									// PD3 - Serial1 TX
#define GPS_power_ctl_pin	45						// PL4 AS A STANDIN ON ATMEGA2560// PD4 - FREE (!mega)
#define GPS_reset_pin										// PD5 - GPS reset pin (!mega)
#define MCP73871_power_good_pin 47 			// PL2 AS A STANDIN ON ATMEGA2560 // PD6 - supply power indicator
#define MCP73871_charge_status_2_pin 38 // PL3 AS A STANDIN ON ATMEGA2560 // PD7 - charge status indicator 1 (charged)

// port E
#define PDI_USART0_RX										// PE0 - Programming Data Interface - Serial RX
#define PDI_USART0_TX										// PE1 - Programming Data Interface - Serial TX
																				// PE2 - ? (!mega)
																				// PE3 - FREE (digital 5)
#define SD_card_detect_pin 2 						// PE4 - SD card detect
#define FRAM_hold_pin	3									// PE5 - FRAM hold
#define FRAM_write_protect_pin					// PE6 - FRAM write protect
#define SPI_SS_FRAM_pin									// PE7 - SPI - SS - FRAM chip

// port F
#define TMP36_Vsense_pin A0 						// PF0 - TMP36 temoerature sensor 97
#define bat_A_pin A1 										// PF1 - battery A voltage
#define interrupt_adxl345_int1_pin 2 		// PF2 - ADXL345 INT1 pin
																				// PF3 - FREE (analog 3)
#define JTAG_TCL_pin 4									// PF4 - JTAG TCL
#define JTAG_TMS_pin 5									// PF5 - JTAG TMS
#define JTAG_TDO_pin 6									// PF6 - JTAG TDO
#define JTAG_TDI_pin 7									// PF7 - JTAG TDI

// port G
#define MCP73871_charge_status_1_pin 41 // PG0 - charge status indicator 1 (charging / LBO)
																				// PG1 - FREE (digital 40)
#define bluetooth_mosfet_gate_pin 39 		// PG2 - bluetooth power control via mosfet
#define SPI_OLED_reset_pin 49 					// PL0 AS A STANDIN ON ATMEGA2560			// PG3 (not routed on mega2560 !! ) - OLED reset pin (yellow)
#define SPI_OLED_a0_pin 48 	 						// PL1 AS A STANDIN ON ATMEGA2560			// PG4 (not routed on mega2560 !! ) - OLED a0 (D/C) pin (white) 
// PG5 - FREE digital 4
