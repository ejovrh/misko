// PIN MODES

//port A
//PA0 - MCP73871 power good indicator
	pinMode(MCP73871_power_good_pin, INPUT);					//
	digitalWrite(MCP73871_power_good_pin, HIGH);			//
//PA1 - MCP73871 charge status indicator 2
	pinMode(MCP73871_charge_status_2_pin, INPUT);			//
	digitalWrite(MCP73871_charge_status_2_pin, HIGH); //
//PA2 - MCP73871 charge status indicator 1
	pinMode(MCP73871_charge_status_1_pin, INPUT);			//
	digitalWrite(MCP73871_charge_status_1_pin, HIGH); //
//PA3 - TMP36 shutdown pin
pinMode(TMP36_shutdown_pin, OUTPUT);							//
digitalWrite(TMP36_shutdown_pin, HIGH);						//
//PA4 - not used
//PA5 - not used
//PA6 - not used
//PA7 - not used

//port B
//PB0
	pinMode(SPI_FRAM_SS_pin, OUTPUT);
	digitalWrite(SPI_FRAM_SS_pin, HIGH);
//PB1 - SPI_SCK_pin
//PB2 - SPI_MOSI_pin
//PB3 - SPI_MISO_pin
//PB4 - GPS_sw_serial_rx
//PB5 - GPS_sw_serial_tx
//PB6 - ADXL345 INT1 pin
	pinMode(ADXL345_INT1_interrupt_pin, INPUT);
	digitalWrite(ADXL345_INT1_interrupt_pin, HIGH);
//PB7
	pinMode(Bluetooth_wakeup_pin, OUTPUT);						// bluetooth power control, bluetooth module draws ~40mA when not connected, 20-30 in connected state
	digitalWrite(Bluetooth_wakeup_pin, LOW);					// mosfet gate for bluetooth module, on(high), off(low)

// port C
//PC0 - user buttons - left, on(high), low(off)
	pinMode(menu_left_button_pin, INPUT);							//
	digitalWrite(menu_left_button_pin, LOW);					//
//PC1 - user buttons - bluetooth power toggle
	pinMode(menu_bluetooth_power_toggle_pin, INPUT);	//
	digitalWrite(menu_bluetooth_power_toggle_pin, LOW); //
//PC2 - user buttons - down, on(high), low(off)
	pinMode(menu_down_button_pin, INPUT);							//
	digitalWrite(menu_down_button_pin, LOW);					//
//PC3 - user buttons - up, on(high), low(off)
	pinMode(menu_up_button_pin, INPUT);								//
	digitalWrite(menu_up_button_pin, LOW);						//
//PC4 - user buttons - GPRS push position, on(high), low(off)
	pinMode(menu_gprs_push_button_pin, INPUT);				//
	digitalWrite(menu_gprs_push_button_pin, LOW);			//
//PC5 - user buttons - right, on(high), low(off)
	pinMode(menu_right_button_pin, INPUT);						//
	digitalWrite(menu_right_button_pin, LOW);					//
//PC6  -write cycle to SD card indicator (blinks on write), on by default until GPS gets fix, then off
	pinMode(gps_red_led_pin, OUTPUT);									//
	digitalWrite(gps_red_led_pin, HIGH);							// on(high) by default until gps gets fix, then off, blinks on SD card write
//PC7 - GPS fix indiator: on - has fix, off - has no fix
	pinMode(gps_green_led_pin, OUTPUT);								// off by befault until GPS gets fix
	digitalWrite(gps_green_led_pin, HIGH);							// high - off; low - on

//portD
//PD0 - I2C_SCL_pin
//PD1 - I2C_SDA_pin
//PD2 - Serial1 - SIM800C_rx_pin
//PD3 - Serial1 - SIM800C_tx_pin
//PD4 - SIM800C_cts_pin
//PD5 - SIM800C_status_pin
	pinMode(SIM800C_status_pin, INPUT);
	digitalWrite(SIM800C_status_pin, HIGH);
//PD6 - SIM800C_power_pin
	pinMode(SIM800C_power_pin, OUTPUT);								// SIM800L power control, connected directly to battery ( due to up 2A current )
	digitalWrite(SIM800C_power_pin, LOW);							// mosfet gate for SIM800L module, on(high), off(low)
//PD7 - GPS power control pin, high(on), low(off)
	pinMode(GPS_power_ctl_pin, OUTPUT);								//
	digitalWrite(GPS_power_ctl_pin, LOW);							//

//port E
//PE0 - PDI_UART0_RX_pin
//PE1 - PDI_UART0_TX_pin
//PE2 - SPI_SS_ADXL345_pin
	pinMode(SPI_SS_ADXL345_pin, OUTPUT);							// SPI SS for ADXL345
	digitalWrite(SPI_SS_ADXL345_pin, HIGH);						// have the ADXL released by default
//PE3 - SPI_OLED_reset_pin
	pinMode(SPI_OLED_reset_pin, OUTPUT);
	digitalWrite(SPI_OLED_a0_pin, HIGH);
//PE4 - SPI_OLED_a0_pin
	pinMode(SPI_OLED_a0_pin, OUTPUT);
	digitalWrite(SPI_OLED_a0_pin, HIGH);
//PE5 - SPI_SS_OLED_pin
	pinMode(SPI_SS_OLED_pin, OUTPUT);
	digitalWrite(SPI_SS_OLED_pin, HIGH);							// have the OLED released by default
//PE6 - SPI_SS_SD_card_pin
	pinMode(SPI_SS_SD_card_pin, OUTPUT);							// SPI SS for the SD card
	digitalWrite(SPI_SS_SD_card_pin, HIGH);
//PE7 - SD_card_detect_pin
	pinMode(SD_card_detect_pin, INPUT);
	digitalWrite(SD_card_detect_pin, HIGH);

// port F
//PF0 - ADC TMP36 temperature sensor voltage out
	pinMode(TMP36_Vsense_pin, INPUT);
//PF1 - ADC battery A voltage
	pinMode(Vcc_sense_pin, INPUT);
//PF2 - battery A voltage sense
	pinMode(bat_A_pin, INPUT);
//PF3 - not used
//PF4 - JTAG_TCL_pin
//PF5 - JTAG_TMS_pin
//PF6 - JTAG_TDO_pin
//PF7 - JTAG_TDI_pin

// port G
//PG0 - not used
//PG1 - not used
//PG2 - not used
//PG3 - low - position B1 ( Bluetooth programming mode), high - position B2 (normal operation)
	pinMode(analog_sw_ctrl_pin, OUTPUT);
	digitalWrite(analog_sw_ctrl_pin, HIGH);						//
//PG4 - GPS power state indicator
	pinMode(GPS_wakeup_pin, INPUT);										//
	digitalWrite(GPS_wakeup_pin, HIGH);								//
//PG5 - PDI_UART0_CTS_pin
