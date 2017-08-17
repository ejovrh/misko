// define pin function, order by port pin in comment

// port A
#define MCP73871_charge_status_1_pin 24			// PA0 - MCP73871_CHARGE_STATUS_1_PIN
#define MCP73871_charge_status_2_pin 25			// PA1 - MCP73871_CHARGE_STATUS_2_PIN
#define MCP73871_power_good_pin 26					// PA2 - MCP73871_POWER_GOOD_PIN
#define gps_green_led_pin  27								// PA3 - GPS_GREEN_LED_PIN
#define gps_red_led_pin 28									// PA4 - GPS_RED_LED_PIN
#define GPS_wakeup_pin 29										// PA5 - GPS_WAKEUP_PIN
#define GPS_power_ctl_pin	30								// PA6 - GPS_POWER_CTL_PIN
#define analog_sw_ctrl_pin 31 							// PA7 - ANALOG_SW_CTRL_PIN

// port B
#define SPI_FRAM_SS_pin 8										// PB0 - SPI_FRAM_SS_PIN
#define SPI_SCK_pin 9 											// PB1 - SPI_SCK_PIN
#define SPI_MOSI_pin 10 										// PB2 - SPI_MOSI_PIN
#define SPI_MISO_pin 11 										// PB3 - SPI_MISO_PIN
#define GPS_sw_serial_rx 12 								// PB4 - GPS_SW_SERIAL_RX
#define GPS_sw_serial_tx 13 								// PB5 - GPS_SW_SERIAL_TX
#define SPI_SS_ADXL345_pin 14								// PB6 - SPI_SS_ADXL345_PIN
#define SPI_OLED_a0_pin 15									// PB7 - SPI_OLED_D/C_PIN

// port C
#define menu_right_button_pin  32						// PC0 - MENU_RIGHT_BUTTON_PIN
#define menu_left_button_pin	33 						// PC1 - MENU_LEFT_BUTTON_PIN
#define menu_center_button_pin 34						// PC2 - MENU_CENTER_BUTTON_PIN
#define menu_up_button_pin 35								// PC3 - MENU_UP_BUTTON_PIN
#define menu_bluetooth_power_button_pin 36	// PC4 - MENU_BLUETOOTH_POWER_BUTTON_PIN
#define menu_gprs_power_button_pin 37 			// PC5 - MENU_GPRS_POWER_BUTTON_PIN
#define Bluetooth_switch_pin 38 						// PC6 - BLUETOOTH_SWITCH_PIN
#define Bluetooth_wakeup_pin  39 						// PC7 - BLUETOOTH_WAKEUP_PIN

// port D
#define I2C_SCL_pin	16											// PD0 - I2C_SCL_PIN
#define I2C_SDA_pin	17											// PD1 - I2C_SDA_PIN
#define SIM800C_rx_pin 19  									// PD2 - SIM800C_RX_PIN
#define SIM800C_tx_pin 18  									// PD3 - SIM800C_TX_PIN
#define SIM800C_cts_pin 20									// PD4 - SIM800C_CTS_PIN
#define SIM800C_power_ctrl_pin 21						// PD5 - SIM800C_POWER_CTRL_PIN
#define SIM800C_power_pin	22 								// PD6 - SIM800C_POWER_PIN
#define WIFI_enable_pin 23									// PD7 - WIFI_EN_PIN

// port E
#define PDI_UART0_RX_pin 0									// PE0 - PDI_UART0_RX_PIN
#define PDI_UART0_TX_pin 1									// PE1 - PDI_UART0_TX_PIN
#define SD_card_detect_pin 2								// PE2 - SD_CARD_DETECT_PIN
#define SPI_SS_SD_card_pin 3								// PE3 - SPI_SS_SD_CARD_PIN
#define WIFI_INT_interrupt_pin  4 	 				// PE4 - WIFI_IRQ_PIN
#define SPI_SS_RTC_pin 5 										// PE5 - SPI_SS_RTC_PIN
#define RTC_INT_interrupt_pin 6 						// PE6 - RTC_INT_INTERRUPT_PIN
#define ADXL345_INT1_interrupt_pin 7				// PE7 - ADXL345_INT1_INTERRUPT_PIN

// port F
#define SPI_WIFI_SS_pin 46 									// PF0 - SPI_SS_WIFI_MODULE
#define Vcc_sense_pin 47										// PF1 - VCC_SENSE_PIN
#define bat_A_pin 48 												// PF2 - BAT_A_PIN
#define TMP36_Vsense_pin 49									// PF3 - TMP36_VSENSE_PIN
#define JTAG_TCL_pin 50											// PF4 - JTAG_TCL_PIN
#define JTAG_TMS_pin 51											// PF5 - JTAG_TMS_PIN
#define JTAG_TDO_pin 52											// PF6 - JTAG_TDO_PIN
#define JTAG_TDI_pin 53											// PF7 - JTAG_TDI_PIN

// port G
#define WIFI_wake_pin 40										// PG0 - WIFI_WAKE_PIN
#define menu_down_button_pin 41							// PG1 - MENU_DOWN_BUTTON_PIN
#define Bluetooth_profile_indicator_pin 42  // PG2 - BLUETOOTH_PROF_IND_PIN
#define SPI_OLED_reset_pin 43								// PG3 - SPI_OLED_RESET_PIN
#define SPI_SS_OLED_pin 44									// PG4 - SPI_SS_OLED_PIN
#define PDI_UART0_CTS_pin	45								// PG5 - PDI_UART0_CTS_PIN