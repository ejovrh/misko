// PIN MODES

//port A
//PA0 - MCP73871 power good indicator
	pinMode(MCP73871_charge_status_1_pin, INPUT);			// see MCP73871 datasheet pp. 21 for function
	digitalWrite(MCP73871_charge_status_1_pin, HIGH); //
//PA1 - MCP73871 charge status indicator 2
	pinMode(MCP73871_charge_status_2_pin, INPUT);			// see MCP73871 datasheet pp. 21 for function
	digitalWrite(MCP73871_charge_status_2_pin, HIGH); //
//PA2 - MCP73871 charge status indicator 1
	pinMode(MCP73871_power_good_pin, INPUT);					// see MCP73871 datasheet pp. 21 for function
	digitalWrite(MCP73871_power_good_pin, HIGH);			//
//PA3 - write cycle to SD card indicator (blinks on write), on by default until GPS gets fix, then off
	pinMode(gps_green_led_pin, OUTPUT);								// off by befault until GPS gets fix
	digitalWrite(gps_green_led_pin, HIGH);						// high - off; low - on
//PA4 - GPS fix indiator: on - has fix, off - has no fix
	pinMode(gps_red_led_pin, OUTPUT);									//
	digitalWrite(gps_red_led_pin, HIGH);							// on(high) by default until gps gets fix, then off, blinks on SD card write
//PA5 - GPS power switch pin, high(on), low(off)
	pinMode(GPS_wakeup_pin, OUTPUT);								// signals a wakeup to the GPS device; the sleep command is issued in software
	digitalWrite(GPS_wakeup_pin, LOW);							// off(low), on (high); the wakeup is triggered by pulling the pin high
//PA6 - GPS power status indicator - high(on), low(off)
	pinMode(GPS_power_ctl_pin, INPUT);								// checks if the device is powered on
	digitalWrite(GPS_power_ctl_pin, HIGH);							// off(low), on (high); the wakeup is triggered by pulling the GPS_power_crl_pin high
//PA7 - low - position B1 ( Bluetooth programming mode), high - position B2 (normal operation)
	pinMode(analog_sw_ctrl_pin, OUTPUT);
	digitalWrite(analog_sw_ctrl_pin, HIGH);						//

//port B
//PB0
	pinMode(SPI_FRAM_SS_pin, OUTPUT);
	digitalWrite(SPI_FRAM_SS_pin, HIGH);
//PB1 - SPI_SCK_pin
//PB2 - SPI_MOSI_pin
//PB3 - SPI_MISO_pin
//PB4 - GPS_sw_serial_rx
//PB5 - GPS_sw_serial_tx
//PB6 - SPI_SS_ADXL345_pin
	pinMode(SPI_SS_ADXL345_pin, OUTPUT);							// SPI SS for ADXL345
	digitalWrite(SPI_SS_ADXL345_pin, HIGH);						// have the ADXL released by default
//PB7 - OLED D/C pin
	pinMode(SPI_OLED_a0_pin, OUTPUT);									//
	digitalWrite(SPI_OLED_a0_pin, HIGH);							//

// port C
//PC0 - user buttons - right, on(high), low(off)
	pinMode(menu_right_button_pin, INPUT);							//
	digitalWrite(menu_right_button_pin, LOW);					//
//PC1 - user buttons - left, on(high), low(off)
	pinMode(menu_left_button_pin, INPUT);	//
	digitalWrite(menu_left_button_pin, LOW); //
//PC2 - user buttons - center, on(high), low(off)
	pinMode(menu_center_button_pin, INPUT);							//
	digitalWrite(menu_center_button_pin, LOW);					//
//PC3 - user buttons - up, on(high), low(off)
	pinMode(menu_up_button_pin, INPUT);								//
	digitalWrite(menu_up_button_pin, LOW);						//
//PC4 - user buttons - Bluetooth power button, on(high), low(off)
	pinMode(menu_bluetooth_power_button_pin, INPUT);				//
	digitalWrite(menu_bluetooth_power_button_pin, LOW);			//
//PC5 - user buttons - GPRS power button, on(high), low(off)
	pinMode(menu_gprs_power_button_pin, INPUT);						//
	digitalWrite(menu_gprs_power_button_pin, LOW);					//
//PC6 - Bluetooth switch pin - high(on), low(off)
	pinMode(Bluetooth_switch_pin, OUTPUT);
	digitalWrite(Bluetooth_switch_pin, LOW);
//PC7 - Bluetooth wakeup pin - high(wake), low(sleep)
	pinMode(Bluetooth_wakeup_pin, OUTPUT);
	digitalWrite(Bluetooth_wakeup_pin, LOW);

//portD
//PD0 - I2C_SCL_pin
//PD1 - I2C_SDA_pin
//PD2 - Serial1 - SIM800C_rx_pin
//PD3 - Serial1 - SIM800C_tx_pin
//PD4 - SIM800C_cts_pin
//PD5 - SIM800C_power_ctrl_pin
	pinMode(SIM800C_power_ctrl_pin, INPUT);
	digitalWrite(SIM800C_power_ctrl_pin, HIGH);
//PD6 - SIM800C_power_pin, high(on), low(off)
	pinMode(SIM800C_power_pin, OUTPUT);								// SIM800L power control, connected directly to battery ( due to up 2A current )
	digitalWrite(SIM800C_power_pin, LOW);							// power control for SIM800L module, on(high), off(low)
//PD7 - WiFi module enable pin
	pinMode(WIFI_wake_pin, OUTPUT);
	digitalWrite(WIFI_wake_pin, LOW);

//port E
//PE0 - PDI_UART0_RX_pin
//PE1 - PDI_UART0_TX_pin
//PE2 - SD Card detect pin
	pinMode(SD_card_detect_pin, INPUT);
	digitalWrite(SD_card_detect_pin, HIGH);
//PE3 - SPI_SS_SD_card_pin
	pinMode(SPI_SS_SD_card_pin, OUTPUT);							// SPI SS for the SD card
	digitalWrite(SPI_SS_SD_card_pin, HIGH);						// have the CD card released by default
//PE4 - Wifi Module IRQ pin
	pinMode(WIFI_INT_interrupt_pin, INPUT);
	digitalWrite(WIFI_INT_interrupt_pin, HIGH);
//PE5 - SPI_SS_RTC_pin
	pinMode(SPI_SS_RTC_pin, OUTPUT);								// SPI SS for the RTC module
	digitalWrite(SPI_SS_RTC_pin, HIGH);							// have the RTC released by default
//PE6 - RTC IRQ pin
	pinMode(RTC_INT_interrupt_pin, INPUT);
	digitalWrite(RTC_INT_interrupt_pin, HIGH);							//
//PE7 - ADXL345 Interrupt1 pin
	pinMode(ADXL345_INT1_interrupt_pin, INPUT);
	digitalWrite(ADXL345_INT1_interrupt_pin, HIGH);

// port F
//PF0 - WiFi SPI SS pin
  pinMode(SPI_WIFI_SS_pin, OUTPUT);
	digitalWrite(SPI_WIFI_SS_pin, HIGH);
//PF1 - ADC battery A voltage - reads Vcc via a voltage divider
	pinMode(Vcc_sense_pin, INPUT);
//PF2 - battery A voltage sense
	pinMode(bat_A_pin, INPUT);
//PF3 - ADC TMP36 temperature sensor voltage out
	pinMode(TMP36_Vsense_pin, INPUT);
//PF4 - JTAG_TCL_pin
//PF5 - JTAG_TMS_pin
//PF6 - JTAG_TDO_pin
//PF7 - JTAG_TDI_pin

// port G
//PG0 - WiFi wakeup pin
	pinMode(WIFI_wake_pin, OUTPUT);
	digitalWrite(WIFI_wake_pin, LOW);
//PG1 - user buttons - down, on(high), low(off)
	pinMode(menu_down_button_pin, INPUT);							//
	digitalWrite(menu_down_button_pin, LOW);
//PG2 - Bluetooth profile indicator pin
	pinMode(Bluetooth_profile_indicator_pin, INPUT);
	digitalWrite(Bluetooth_profile_indicator_pin, HIGH);
//PG3 - OLED reset pin
	pinMode(SPI_OLED_reset_pin, OUTPUT);
	digitalWrite(SPI_OLED_reset_pin, HIGH);
//PG4 - SPI OLED SS pin
	pinMode(SPI_SS_OLED_pin, OUTPUT);										//
	digitalWrite(SPI_SS_OLED_pin, HIGH);								//
//PG5 - PDI_UART0_CTS_pin
