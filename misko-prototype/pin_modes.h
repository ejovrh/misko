// PIN MODES
//pinMode(interrupt_0_pin, INPUT); // meant to power the whole thing off via acceleroeter -- FIXME: not in use
pinMode(gps_power_pin, OUTPUT); // powers the gps (PWM) -- FIXME: direct power through the atmega2560; better is using a mosfet, draws ~4mA regardless of fix
pinMode(gps_red_led_pin, OUTPUT); // write cycle to SD card indicator (blinks on write), on by default until GPS gets fix, then off
pinMode(gps_green_led_pin, OUTPUT); // GPS fix indiator: on - has fix, off - has no fix
pinMode(sd_ss_pin, OUTPUT); // FIXME
pinMode(bluetooth_power_pin, OUTPUT); // FIXME: supplies power via PWM (100% duty cycle) to bluetooth module FIXME: move to digital pin and drive via mosfet, draws ~40mA wen not connected, 20-30 on TX in connected state
//pinMode(bluetooth_mosfet_gate, OUTPUT);  //switches power for bluetooth transmitter -- FIXME: not in use
//pinMode(bluetooth_activate_button, INPUT); // FIXME: not in use
//pinMode(temperature_pin, INPUT); // FIXME: not in use

// PIN STATES
//digitalWrite(bluetooth_mosfet_gate, HIGH);  //bluetooth gate low = off by default, FIXME: not in use
//digitalWrite(bluetooth_activate_button, LOW); // button press brings it HIGH => bluetooth_mosfet_gate goes low (by software), counts 30s, then goes HIGH again, FIXME: not in use
digitalWrite(gps_power_pin, LOW); // low - on, high - off 
digitalWrite(bluetooth_power_pin, HIGH); //FIXME: move to digital pin and drive via mosfet
digitalWrite(gps_red_led_pin, HIGH); // on(high) by default until gps gets fix, then off, blinks on SD card write
digitalWrite(gps_green_led_pin, LOW); // off(low) by befault until GPS gets fix
//digitalWrite(temperature_pin, HIGH); // FIXME: not in use
