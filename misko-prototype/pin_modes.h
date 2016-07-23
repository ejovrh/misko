pinMode(interrupt_0_pin, INPUT); 
pinMode(gps_power_pin, OUTPUT); // powers the gps
pinMode(gps_red_led_pin, OUTPUT); // Use the red LED as an indicator for write to SD card
pinMode(gps_green_led_pin, OUTPUT); // Use the green LED as an indicator for gps fix
pinMode(sd_ss_pin, OUTPUT);
pinMode(bluetooth_mosfet_gate, OUTPUT);  //switches power for bluetooth transmitter
pinMode(bluetooth_activate_button, INPUT);

pinMode(temperature_pin, INPUT); // ??

digitalWrite(bluetooth_mosfet_gate, HIGH);  //bluetooth gate low = off by default
digitalWrite(bluetooth_activate_button, LOW); // button press brings it HIGH => bluetooth_mosfet_gate goes low (by software), counts 30s, then goes HIGH again
digitalWrite(gps_power_pin, LOW); // pull low to turn on!
digitalWrite(gps_red_led_pin, HIGH); // on by default, turns off when gps has fix
digitalWrite(gps_green_led_pin, LOW); // off by befault
digitalWrite(temperature_pin, HIGH);

