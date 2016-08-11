// define pin modes and states, order by pin name

// PIN MODES
pinMode(bluetooth_mosfet_gate_pin, OUTPUT); // bluetooth power control, bluetooth module draws ~40mA when not connected, 20-30 in connected state
pinMode(bluetooth_power_toggle_pin, INPUT);
pinMode(gps_green_led_pin, OUTPUT); // GPS fix indiator: on - has fix, off - has no fix
pinMode(gps_power_pin, OUTPUT); // powers the gps (PWM) -- FIXME: direct power through the atmega2560; better is using a mosfet, draws ~4mA regardless of fix
pinMode(gps_red_led_pin, OUTPUT); // write cycle to SD card indicator (blinks on write), on by default until GPS gets fix, then off
pinMode(menu_down_buttton, INPUT); // user buttons - down
pinMode(menu_left_buttton, INPUT); // user buttons - left 
pinMode(menu_right_buttton, INPUT); //  user buttons - right
pinMode(menu_up_buttton, INPUT); //  user buttons - up
pinMode(menu_left_buttton, INPUT); // 
pinMode(spi_ss_sd_card, OUTPUT); // SPI SS for the SD card

// PIN STATES
digitalWrite(bluetooth_power_toggle_pin, LOW); // 
digitalWrite(bluetooth_mosfet_gate_pin, LOW); // mosfet gate for bluetooth module, on(high), off(low)
digitalWrite(gps_green_led_pin, LOW); // off(low) by befault until GPS gets fix
digitalWrite(gps_power_pin, LOW); // low(on), high(off) 
digitalWrite(gps_red_led_pin, HIGH); // on(high) by default until gps gets fix, then off, blinks on SD card write
digitalWrite(menu_down_buttton, LOW); // on(high), low(off)
digitalWrite(menu_left_buttton, LOW); // on(high), low(off)
digitalWrite(menu_right_buttton, LOW); // on(high), low(off)
digitalWrite(menu_up_buttton, LOW); // on(high), low(off)
