#ifndef PIN_MODES_H_
#define PIN_MODES_H_

pinMode(ADXL345_INT1_interrupt_pin, INPUT);				// hardware interrupt from the accelerometer, triggers wake & sleep events
pinMode(analog_sw_ctrl_pin, OUTPUT);							// controls analog switches for BT/GPS <-> micro comm
pinMode(bat_A_pin, INPUT);												// ADC, Vbat measurement
pinMode(Bluetooth_profile_indicator_pin, INPUT);	// inducates current BT profile (see datasheet ) TODO
pinMode(Bluetooth_switch_pin, OUTPUT);						// TODO
pinMode(Bluetooth_wakeup_pin, OUTPUT);						// TODO
pinMode(gps_green_led_pin, OUTPUT);								// LED, off by befault until GPS gets fix
pinMode(GPS_power_ctl_pin, INPUT);								// checks if the device is powered on
pinMode(gps_red_led_pin, OUTPUT);									// LED, TODO
pinMode(GPS_wakeup_pin, OUTPUT);									// signals a wakeup to the GPS device; the sleep command is issued in software
pinMode(MCP73871_charge_status_1_pin, INPUT);			// see MCP73871 datasheet pp. 21 for function
pinMode(MCP73871_charge_status_2_pin, INPUT);			// see MCP73871 datasheet pp. 21 for function
pinMode(MCP73871_power_good_pin, INPUT);					// see MCP73871 datasheet pp. 21 for function
pinMode(menu_bluetooth_power_button_pin, INPUT);	// external button
pinMode(menu_center_button_pin, INPUT);						// joystick
pinMode(menu_down_button_pin, INPUT);							// joystick
pinMode(menu_gprs_power_button_pin, INPUT);				// external button, LED equipped
pinMode(menu_left_button_pin, INPUT);							// joystick
pinMode(menu_right_button_pin, INPUT);						// joystick
pinMode(menu_up_button_pin, INPUT);								// joystick
pinMode(RTC_INT_interrupt_pin, INPUT);						// hardware interrupt from the RTC, triggers time-based events
pinMode(SD_card_detect_pin, INPUT);								// signals the micro on sd card insertion/removal (not in real time!)
pinMode(SIM800C_power_ctrl_pin, INPUT);						// TODO
pinMode(SIM800C_power_pin, OUTPUT);								// SIM800L power control, connected directly to battery ( due to up 2A current )
pinMode(SPI_FRAM_SS_pin, OUTPUT);									// SPI SS for FERAM
pinMode(SPI_OLED_a0_pin, OUTPUT);									// OLED data/command pin
pinMode(SPI_OLED_reset_pin, OUTPUT);							// OLED reset pin
pinMode(SPI_SS_ADXL345_pin, OUTPUT);							// SPI SS for ADXL345
pinMode(SPI_SS_OLED_pin, OUTPUT);									// SPI SS for OLED
pinMode(SPI_SS_RTC_pin, OUTPUT);									// SPI SS for the RTC module
pinMode(SPI_SS_SD_card_pin, OUTPUT);							// SPI SS for the SD card
pinMode(SPI_WIFI_SS_pin, OUTPUT);									// SPI SS for WiFi module
pinMode(TMP36_Vsense_pin, INPUT);									// ADC, temperature measurement
pinMode(Vcc_sense_pin, INPUT);										// ADC, Vcc measurement
pinMode(WIFI_INT_interrupt_pin, INPUT);						// hardware interrupt from the WiFi module, triggers WiFi action
pinMode(WIFI_wake_pin, OUTPUT);										// FIXME - something is missing here
pinMode(WIFI_wake_pin, OUTPUT);										// FIXME - something is missing here

#endif /* PIN_MODES_H_ */