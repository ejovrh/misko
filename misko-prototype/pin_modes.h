pinMode(interrupt_0_pin, INPUT); // meant to power the whole thing off via acceleroeter -- FIXME: not in use
pinMode(gps_power_pin, OUTPUT); // powers the gps -- FIXME: direct power through the atmega2560; better is using a mosfet
pinMode(gps_red_led_pin, OUTPUT); // Use the red LED as an indicator for write to SD card
pinMode(gps_green_led_pin, OUTPUT); // Use the green LED as an indicator for gps fix
pinMode(sd_ss_pin, OUTPUT);
pinMode(bluetooth_mosfet_gate, OUTPUT);  //switches power for bluetooth transmitter -- FIXME: not in use
pinMode(bluetooth_activate_button, INPUT); // FIXME: not in use

pinMode(temperature_pin, INPUT); // FIXME: not in use

digitalWrite(bluetooth_mosfet_gate, HIGH);  //bluetooth gate low = off by default, FIXME: not in use
digitalWrite(bluetooth_activate_button, LOW); // button press brings it HIGH => bluetooth_mosfet_gate goes low (by software), counts 30s, then goes HIGH again, FIXME: not in use
digitalWrite(gps_power_pin, LOW); // pull low to turn on!
digitalWrite(gps_red_led_pin, HIGH); // on by default, turns off when gps has fix
digitalWrite(gps_green_led_pin, LOW); // off by befault
digitalWrite(temperature_pin, HIGH); // FIXME: not in use

