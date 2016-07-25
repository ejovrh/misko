// define pin modes and states, order by pin name

// PIN MODES
pinMode(bluetooth_mosfet_gate_pin, OUTPUT); // bluetooth power control, bluetooth module draws ~40mA when not connected, 20-30 in connected state
pinMode(gps_green_led_pin, OUTPUT); // GPS fix indiator: on - has fix, off - has no fix
pinMode(gps_power_pin, OUTPUT); // powers the gps (PWM) -- FIXME: direct power through the atmega2560; better is using a mosfet, draws ~4mA regardless of fix
pinMode(gps_red_led_pin, OUTPUT); // write cycle to SD card indicator (blinks on write), on by default until GPS gets fix, then off
pinMode(led_mosfet_gate_pin, OUTPUT); // led module power control, FIXME: how much current??
pinMode(sd_ss_pin, OUTPUT); // FIXME - add description

// PIN STATES
digitalWrite(bluetooth_mosfet_gate_pin, HIGH); // mosfet gate for bluetooth module, on(high), off(low)
digitalWrite(gps_green_led_pin, LOW); // off(low) by befault until GPS gets fix
digitalWrite(gps_power_pin, LOW); // low(on), high(off) 
digitalWrite(gps_red_led_pin, HIGH); // on(high) by default until gps gets fix, then off, blinks on SD card write
digitalWrite(led_mosfet_gate_pin, HIGH); // on(high), low(off)

