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

byte adxl345_readByte(byte registerAddress) // reads one byte at registerAddress
{
			//Serial.print("registerAddress -");Serial.print(registerAddress, BIN); Serial.println("- registerAddress");
			//Serial.print("0x80 registerAddress -");Serial.print(0x80 | registerAddress, BIN); Serial.println("- 0x80 registerAddress");
		digitalWrite(SPI_SS_ADXL345_pin, LOW); // reserve the slave
    
		SPI.transfer(0x80 | registerAddress); // set the MSB to 1 (the read command), then send address to read from
		
		byte retval = SPI.transfer(0x00); // send one byte (0xff) into the circular fifo buffer, get one byte back
			//Serial.print("retval -");Serial.print(retval, BIN); Serial.println("- retval");
		
		digitalWrite(SPI_SS_ADXL345_pin, HIGH); // release the slave
    return retval;  // return value
}

bool adxl345_writeByte(byte registerAddress, byte value)
{
	digitalWrite(SPI_SS_ADXL345_pin, LOW); // signal the slave
	SPI.transfer(registerAddress); // send address to write to
	SPI.transfer(value); // send the byte
	digitalWrite(SPI_SS_ADXL345_pin, HIGH); // release the slave
	
	if (adxl345_readByte(registerAddress) != value) // verify correct setting
	{
		Serial.print(F("registerAddress was not set:"));Serial.print(registerAddress);Serial.print("-");Serial.println(value);
		return 1;
	}
		
	return 0;
}

void poor_mans_debugging(void)
{
/* 	    // poor man's debugging
    for (uint8_t i=0; i< 7; i++)
    {
      Serial.print(i); Serial.print(" - ");Serial.println(EEPROM[i]);
    }
    //int8_t tz = EEPROM[5];
    //Serial.print("tz - ");Serial.println(tz);
		SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE3));
		adxl345_readByte(0x00);
		Serial.print("INT_SOURCE -");Serial.print(adxl345_readByte(0x30), BIN);Serial.println("-");
		Serial.print("INT_MAP -");Serial.print(adxl345_readByte(0x2F), BIN);Serial.println("-");
		Serial.print("INT_ENABLE -");Serial.print(adxl345_readByte(0x2E), BIN);Serial.println("-");
		
		SPI.endTransaction(); */
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

		poor_mans_debugging();
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
		uint16_t tempReading = analogRead(tmp36_pin);  // read raw sensor data (voltage) - 10bit resolution -> values form 0-1023

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

void handle_adx_intl(void)
{
	byte adxl345_irq_src = adxl345_readByte(INT_SOURCE);
	//Serial.print("adxl345_irq_src");Serial.print(adxl345_irq_src,BIN);Serial.println("adxl345_irq_src");
	
    
		// inactivity
    if(adxl345_irq_src & 0x08) // if the inact bit is set
		{
      Serial.println("Inactivity");

      byte bwRate = adxl345_readByte(BW_RATE); // get current config
      adxl345_writeByte(BW_RATE, (bwRate | 0x10) ); // set to low power mode, bit 5 (was 0x0A, becomes 0x1A)
    }
		
		// activity
    if(adxl345_irq_src & 0x10) // if the act bit is set
		{
      Serial.println("Activity");
      
      byte powerCTL = adxl345_readByte(POWER_CTL); // get current config
      // set the device back in measurement mode
      // as suggested on the datasheet, we put it in standby then in measurement mode
      //writeTo(DEVICE, R_POWER_CTL, powerCTL & B8(11110011));
      adxl345_writeByte(POWER_CTL, 0x04); // first standby
      delay(10); // let's give it some time (not sure if this is needed)
      //writeTo(DEVICE, R_POWER_CTL, powerCTL & B8(11111011));
      adxl345_writeByte(POWER_CTL, POWER_CTL_CFG); // then full measurement mode
      
      // set the LOW_POWER bit to 0 in R_BW_RATE: get back to full accuracy measurement (we will consume more power)
      byte bwRate = adxl345_readByte(BW_RATE);
      adxl345_writeByte(BW_RATE, (BW_RATE & 0x08) );
    }
    adxl345_int1 = 0;
}
