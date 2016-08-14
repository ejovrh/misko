/* eeprom_timer() - returns 1 if enough time has passed
 *  returns: 
 *    1 if time is up
 *    0 otherwise
 *    
 * input arguments: 
 *    unsigned int in_bluetooth_button_press_time - time of button press
 *    char in_eeprom_index - index of time paramater in EEPROM[]
 * 
 * times are in milliseconds !!
 */
int eeprom_timer(unsigned int in_button_press_time, unsigned int in_eeprom_index)
{
  return (abs( in_button_press_time - millis()) / 1000 > EEPROM[in_eeprom_index] ?  1 :  0); 
}

inline void eeprom_set(int in_val, int in_index) // sets EEPROM[in_index to val
{
  EEPROM[in_index] = in_val;  
}

inline int8_t eeprom_get(int in_index)
{
  return EEPROM[in_index];
}

void handle_bluetooth_button(void)
{
    // bluetooth power on
  if (!flag_bluetooth_power_toggle_pressed && digitalRead(bluetooth_power_toggle_pin) == HIGH) // if button was not pressed and now gets pressed
  //  flag_bluetooth_power_toggle_pressed prevents mutiple executions of this if() block
  { 
    bluetooth_button_press_time = millis(); // record time of button press; this is used in eeprom_timer()
    digitalWrite(bluetooth_mosfet_gate_pin, HIGH); // turn on the device
    flag_bluetooth_is_on = 1; // set flag to on
    flag_bluetooth_power_toggle_pressed = 1; // mark button as pressed

    // poor man's debugging
    for (uint8_t i=0; i< 7; i++)
    {
      Serial.print(i); Serial.print(" - ");Serial.println(EEPROM[i]);
    }
    //int8_t tz = EEPROM[5];
    //Serial.print("tz - ");Serial.println(tz);
  }

  // bluetooth power off
  if (flag_bluetooth_power_toggle_pressed && digitalRead(bluetooth_power_toggle_pin) == LOW) // if button was  pressed and now gets released
  // flag_bluetooth_power_toggle_pressed prevents mutiple executions of this if() block
  { 
    bluetooth_button_release_time = millis(); // record time of button press; this is used in 
    flag_bluetooth_power_toggle_pressed = 0; // mark button as released

    if (abs(bluetooth_button_release_time - bluetooth_button_press_time) > 500) // if button is held long enough
    {
      if (flag_bluetooth_power_keep_on) // if the BT device was on
        digitalWrite(bluetooth_mosfet_gate_pin, LOW); // turn off the BT device

      flag_bluetooth_power_keep_on = !flag_bluetooth_power_keep_on; // invert the flag (on -> off or off -> on)
    }
  }

  // bluetooth timed power off
  if ( (!flag_bluetooth_power_keep_on && flag_bluetooth_is_on) && eeprom_timer(bluetooth_button_press_time, EERPOM_BLUETOOTH_AUTO_TIMEOUT_INDEX)) // if the device is on and enough time has passed
  // flag_bluetooth_power_keep_on prevents the timer from kicking in if we want the BT device to stay on
  // flag_bluetooth_is_on prevents code execution on every loop
  { 
      digitalWrite(bluetooth_mosfet_gate_pin, LOW); // turn off the device
      flag_bluetooth_is_on = 0; // set flag to off
  }
}

float readVcc() // http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
{
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
	ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  float retval = 1125.300 / result; // Calculate Vcc in V; 1125.300 = 1.1*1023
  return retval; // Vcc in V
}

void calculate_temperature(void) // calculates temperature by reading the TMP36 analog data
{
	// the temperature field is defined as:
	// 		char temperature[6] = "T+30C"; // temperature, "T-12C" or "T+56C"
	
	if ( abs(millis() -  temperature_last_reading) / 1000 > TEMPERATURE_SAMPLE_PERIOD  || temperature_last_reading == 0)
	{
		int tempReading = analogRead(tmp36_pin);  // read raw sensor data (voltage) - 10bit resolution -> values form 0-1023

		float voltage = ( tempReading * readVcc()) / 1024.0 ; // converting reading to voltage, based on AREF
 
		int8_t temperatureC = (voltage - 0.5) * 100 ;  // 10 mv per C, 500 mV offset

		sprintf(temperature + sizeof(char), "%+.2d", temperatureC); // THE way to print
		strncat( temperature + 4*sizeof(char), "C", sizeof(char)); // append C and a null terminator
/*
		int reading3v3 = analogRead(A1);
		float voltage3v3 = reading3v3 * readVcc();
		voltage3v3 /= 1024.0;
		
		int readingvcc = analogRead(A2);
		float voltagevcc = readingvcc * readVcc();
		voltagevcc /= 1024.0;
		
		Serial.print(F("Vref - ")); Serial.print(readVcc()); 
		Serial.print(F(" Vcc - ")); Serial.print(voltagevcc); 
		Serial.print(F(" 3V3 - ")); Serial.print(voltage3v3); 
*/
		Serial.print(F(" V temp - ")); Serial.print(voltage);
		Serial.print(F(" temp - "));Serial.println(temperature);

		temperature_last_reading = millis(); // update last read time of value
	}	
}
