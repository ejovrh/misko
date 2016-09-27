// define pin modes and states, order by pin name

// PIN MODES
pinMode(bat_A_pin, INPUT);
pinMode(SPI_SS_ADXL345_pin, OUTPUT); // SPI SS for ADXL345
pinMode(SPI_SS_SD_card_pin, OUTPUT); // SPI SS for the SD card
pinMode(Bluetooth_wakeup_pin, OUTPUT); // bluetooth power control, bluetooth module draws ~40mA when not connected, 20-30 in connected state
pinMode(menu_bluetooth_power_toggle_pin, INPUT); // user buttons - bluetooth power toggle
pinMode(gps_green_led_pin, OUTPUT); // GPS fix indiator: on - has fix, off - has no fix
pinMode(GPS_power_ctl_pin, OUTPUT); // powers the gps (PWM) -- FIXME: direct power through the atmega2560; better is using a mosfet, draws ~4mA regardless of fix
pinMode(gps_red_led_pin, OUTPUT); // write cycle to SD card indicator (blinks on write), on by default until GPS gets fix, then off
pinMode(MCP73871_charge_status_1_pin, INPUT); // MCP73871 charge status indicator 1
pinMode(MCP73871_charge_status_2_pin, INPUT); // MCP73871 charge status indicator 2
pinMode(MCP73871_power_good_pin, INPUT); // MCP73871 power good indicator
pinMode(menu_down_button_pin, INPUT); // user buttons - down
pinMode(menu_gprs_push_button_pin, INPUT); // user buttons - GPRS push position
pinMode(menu_left_button_pin, INPUT); // user buttons - left 
pinMode(menu_right_button_pin, INPUT); //  user buttons - right
pinMode(menu_up_button_pin, INPUT); //  user buttons - up
pinMode(SIM800C_power_pin, OUTPUT); // SIM800L power control, connected directly to battery ( due to up 2A current )
pinMode(TMP36_shutdown_pin, OUTPUT); // TMP36 shutdown control
pinMode(GPS_power_ctl_pin, OUTPUT); // GPS power control pin
pinMode(interrupt_adxl345_int1_pin, INPUT);

// PIN STATES
digitalWrite(SPI_SS_ADXL345_pin, HIGH); // have the ADXL released by default
digitalWrite(SPI_SS_OLED_pin, HIGH); // have the OLED released by default
digitalWrite(Bluetooth_wakeup_pin, LOW); // mosfet gate for bluetooth module, on(high), off(low)
digitalWrite(menu_bluetooth_power_toggle_pin, LOW); // 
digitalWrite(gps_green_led_pin, LOW); // off(low) by befault until GPS gets fix
digitalWrite(GPS_power_ctl_pin, HIGH); // low(on), high(off) 
digitalWrite(gps_red_led_pin, HIGH); // on(high) by default until gps gets fix, then off, blinks on SD card write
digitalWrite(MCP73871_charge_status_1_pin, HIGH); // high impedance state
digitalWrite(MCP73871_charge_status_2_pin, HIGH); // high impedance state
digitalWrite(MCP73871_power_good_pin, HIGH); // high impedance state
digitalWrite(menu_down_button_pin, LOW); // on(high), low(off)
digitalWrite(menu_left_button_pin, LOW); // on(high), low(off)
digitalWrite(menu_gprs_push_button_pin, LOW); // on(high), low(off)
digitalWrite(menu_right_button_pin, LOW); // on(high), low(off)
digitalWrite(menu_up_button_pin, LOW); // on(high), low(off)
digitalWrite(SIM800C_power_pin, LOW);  // mosfet gate for SIM800L module, on(high), off(low)
digitalWrite(TMP36_shutdown_pin, HIGH); // on(high), low(off), on by default
digitalWrite(GPS_power_ctl_pin, HIGH); // // on(high), low(off), on by default
digitalWrite(interrupt_adxl345_int1_pin, HIGH); 
delay(1); // delay for the last command to settle