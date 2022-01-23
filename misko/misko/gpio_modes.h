#ifndef GPIO_MODES_H_
#define GPIO_MODES_H_

gpio_conf(ADXL345_INT1_interrupt_pin, INPUT, LOW);								// hardware interrupt for motion-based events, active high
gpio_conf(analog_sw_ctrl_pin, OUTPUT, LOW);										//TODO low (), high ()
gpio_conf(bat_A_pin, INPUT, PULLUP);											// ADC
gpio_conf(Bluetooth_profile_indicator_pin, OUTPUT, LOW);						// sense pin: high (BT classic), low (BLE)
gpio_conf(Bluetooth_switch_pin, OUTPUT, LOW);									// see RN4678 data sheet p.4 & 9 for function
gpio_conf(Bluetooth_wakeup_pin, OUTPUT, HIGH);									// see RN4678 data sheet p. 11 for function
gpio_conf(gps_green_led_pin, OUTPUT, HIGH);										// high (off), low (on)
gpio_conf(GPS_power_ctl_pin, OUTPUT, LOW);										// control pin: low (off), high (on)
gpio_conf(gps_red_led_pin, OUTPUT, HIGH);										// high (off), low (on)
gpio_conf(GPS_wakeup_pin, INPUT, LOW);											// sense pin: high (device on), low (device off)
gpio_conf(MCP73871_charge_status_1_pin, INPUT, HIGH);							// see MCP73871 data sheet pp. 21 for function
gpio_conf(MCP73871_charge_status_2_pin, INPUT, HIGH);							// see MCP73871 data sheet pp. 21 for function
gpio_conf(MCP73871_power_good_pin, INPUT, HIGH);								// see MCP73871 data sheet pp. 21 for function
gpio_conf(menu_bluetooth_power_button_pin, INPUT, HIGH);						// high (idle), low (pressed)
gpio_conf(menu_center_button_pin, INPUT, HIGH);									// high (idle), low (pressed)
gpio_conf(menu_down_button_pin, INPUT, HIGH);									// high (idle), low (pressed)
gpio_conf(menu_gprs_power_button_pin, INPUT, HIGH);								// high (idle), low (pressed)
gpio_conf(menu_left_button_pin, INPUT, HIGH);									// high (idle), low (pressed)
gpio_conf(menu_right_button_pin, INPUT, HIGH);									// high (idle), low (pressed)
gpio_conf(menu_up_button_pin, INPUT, HIGH);										// high (idle), low (pressed)
gpio_conf(RTC_INT_interrupt_pin, INPUT, LOW);									// TODO
gpio_conf(SD_card_detect_pin, INPUT, HIGH);										// TODO low (card inserted), high (card removed)
gpio_conf(SPI_MISO_pin, INPUT, LOW);
gpio_conf(SPI_MOSI_pin, OUTPUT, LOW);
gpio_conf(SPI_SCK_pin, OUTPUT, HIGH);
gpio_conf(SPI_OLED_a0_pin, OUTPUT, LOW);										//TODO
gpio_conf(SPI_OLED_reset_pin, OUTPUT, LOW);										//TODO
gpio_conf(SPI_SS_ADXL345_pin, OUTPUT, HIGH);									// high (device inert), low (device selected)
gpio_conf(SPI_SS_FRAM_pin, OUTPUT, HIGH);										// high (device inert), low (device selected)
gpio_conf(SPI_SS_OLED_pin, OUTPUT, HIGH);										// high (device inert), low (device selected)
gpio_conf(SPI_SS_RTC_pin, OUTPUT, HIGH);										// high (device inert), low (device selected)
gpio_conf(SPI_SS_SD_card_pin, OUTPUT, HIGH);									// high (device inert), low (device selected)
gpio_conf(TMP36_Vsense_pin, INPUT, PULLUP);										// ADC
gpio_conf(Vcc_sense_pin, INPUT, PULLUP);										// ADC

#endif /* GPIO_MODES_H_ */