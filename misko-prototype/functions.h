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

// ready a byte from the ADXL345
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

// writes a byte into the ADXL345
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

// sets an eeprom value at a certain index
inline void eeprom_set(int in_val, int in_index) // sets EEPROM[in_index to val
{
  EEPROM[in_index] = in_val;  
}

// gets an eeprom value from a certain index
inline int8_t eeprom_get(int in_index)
{
  return EEPROM[in_index];
}

// returns processor-determined Vcc in volts
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
 
 // returns the external voltage reference in volts
inline float read_Varef() // the external reference voltage is set to 4.30V via a zener diode
{
	return AREF_VOLTAGE;
}
 
 // returns the voltage on a given pin
float calculate_voltage(int pin) // calculates the voltage on a given pin by considering read_aref_V()
{
	uint16_t reading = analogRead(pin); // read raw sensor data (voltage) - 10bit resolution -> values form 0-1023
	return (float) (reading * read_Varef() / 1024.0); // converting reading to voltage, based on AREF;
}

// calculates the temperature by reading voltage from the TMP36
inline int8_t calculate_temperature(void) // executed from loop()
{
	return (calculate_voltage(TMP36_Vsense_pin) - 0.5) * 100.0 ;  // 10 mv per C, 500 mV offset
}

// calculates the average temperature after in_count readings
void avg_temperature(int8_t in_temp, uint8_t in_count)
{
	avg_temp += in_temp;
	
	if (scheduler_run_count % in_count == 0)
	{
		temperature = avg_temp / in_count;
		avg_temp = 0;
	}
}

// callback for Vcc
const char *fn_cb_get_Vcc(m2_rom_void_p element)
{
 	//  sprintf(vcc + 3*sizeof(char), "%.2f", readVcc()); // dont work - on arduinos the %f is not supported
	dtostrf(readVcc(), 3, 2, vcc+3*sizeof(char)); // instead, this works
		// http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__avr__stdlib_1ga060c998e77fb5fc0d3168b3ce8771d42.html
	strcat(vcc, "V");
	return vcc;
} 

// callback for battery percentage
const char *fn_cb_get_bat_pct(m2_rom_void_p element)
{
	/* the percentage calculation
		union battery datasheet: charge cutoff voltage: Vbat 4.20V, discharge cutoff voltage: Vbat 2.75V
			over the voltage divider this gives 2.10V and 1.375V
			our voltage divider gives 0.5 Vbat
	
	 percentage calculation see https://racelogic.support/02VBOX_Motorsport/Video_Data_Loggers/Video_VBOX_Range/Video_VBOX_-_User_manual/24_-_Calculating_Scale_and_Offset
	
		dX is 2.1 - 1.375 = 0.725
		dY is 100 - 0 = 100
	 the gradient is dX/dY = 137.93
	
	 Y = percent = 0, X = Voltage = 1.375V
	 	0 = ((dX/dY)* voltage) + c
		0 = (137.93 * 1.375) + c <=> 0 = 189.66 + c <=> c = -189.66
		our equation is: y = 137.93 * x - 189.66
	
	elementary, dr. watson!
 	*/
	dtostrf((138 * calculate_voltage(bat_A_pin)) - 190, 3, 0, bat_a_pct + 4*sizeof(char)); // instead, this works
		// http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__avr__stdlib_1ga060c998e77fb5fc0d3168b3ce8771d42.html
	strcat(bat_a_pct, "%");
	return bat_a_pct;
} 

// primitive BT button-activated printout
void poor_mans_debugging(void)
{
 	  // poor man's debugging
			
		// EEPROM fields
		Serial.println("EERPOM fields");
    for (uint8_t i=0; i< 7; i++)
    {
      Serial.print(i); Serial.print(" - ");Serial.println(EEPROM[i]);
    }
		Serial.println("EERPOM fields");

		//SPI voodoo
		SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE3));
		adxl345_readByte(0x00);
		Serial.print("INT_SOURCE -");Serial.print(adxl345_readByte(0x30), BIN);Serial.println("-");
		Serial.print("INT_MAP -");Serial.print(adxl345_readByte(0x2F), BIN);Serial.println("-");
		Serial.print("INT_ENABLE -");Serial.print(adxl345_readByte(0x2E), BIN);Serial.println("-");
		Serial.print("vbatt -");Serial.print(calculate_voltage(bat_A_pin));Serial.println("-");
		SPI.endTransaction(); 
		
		
		
}

// handler for the bluetooth power toggle button
void handle_bluetooth_button(void)
{
	/* purpose: control BT power based on eeprom settings
		
		the idea is: via one button control BT power according to EERPOM settings.

		the button functions are, if auto mode is enabled:
			- press and the device turns on. after EERPOM_BLUETOOTH_AUTO_TIMEOUT_INDEX is over, turn off
			- hold pressed to keep the device on until manually turned off by again holding pressed (timeout then doesnt count)
			
		if auto mode is not enabled, the button does nothing.
		
		in order to accomplish this, we need to track in what mode we are in (EERPOM_BLUETOOTH_POWER_INDEX),
			how long the device is supposed to be turned on (EERPOM_BLUETOOTH_AUTO_TIMEOUT_INDEX) and how long the BT button
			was held pressed (bluetooth_button_press_time and bluetooth_button_release_time).
	
		additionally, we need to make sure that one button press equals one event - we need to know if the button is 
			held pressed or not (flag_bluetooth_power_toggle_pressed)
	
	*/
	
	if (!flag_bluetooth_power_toggle_pressed) // if the button is not flagged as pressed
	{
		if(digitalRead(bluetooth_power_toggle_pin) == HIGH) // if the button gets pressed
		{
			flag_bluetooth_power_toggle_pressed = 1; // flag the button as pressed, prevents multiple calls
			poor_mans_debugging(); // execute poor mans debugging
			
			if (EEPROM[EERPOM_BLUETOOTH_POWER_INDEX] != 2) // if bt setting is not auto (meaning is set to on or off)
				return; // do nothing
			
			if (flag_bluetooth_power_keep_on) // prevent the timer from starting when we want to turn off manually toggled power
				return; // do nothing
			
			bluetooth_button_press_time = millis(); // record time of button press; this is used in a bit down to keep bt on on auto
			digitalWrite(bluetooth_mosfet_gate_pin, HIGH); // turn on the device
			flag_bluetooth_is_on = 1; // set flag to on
		}
	}
	else // the button is already flagged as pressed == is held pressed
	{
		if (EEPROM[EERPOM_BLUETOOTH_POWER_INDEX] != 2) // if bt setting is auto
			return;
				
		if ( digitalRead(bluetooth_power_toggle_pin) == LOW) // the button is released
		{ 
			bluetooth_button_release_time = millis(); // record time of button release
			flag_bluetooth_power_toggle_pressed = 0; // mark button as released

			if (abs(bluetooth_button_release_time - bluetooth_button_press_time) > 500) // if button is held long enough
						flag_bluetooth_power_keep_on = !flag_bluetooth_power_keep_on; // toggle the power state on -> off and vice versa
		}
	}

	if (EEPROM[EERPOM_BLUETOOTH_POWER_INDEX] != 2) // if bt setting is not auto
		return; // do nothing
	
	if (flag_bluetooth_power_keep_on) // if the BT device is marked to be kept on
		return;	// do nothing 
			
	if ( flag_bluetooth_is_on && eeprom_timer(bluetooth_button_press_time, EERPOM_BLUETOOTH_AUTO_TIMEOUT_INDEX)) // if the device is on and enough time has passed
	{ 
			digitalWrite(bluetooth_mosfet_gate_pin, LOW); // turn off the device
			flag_bluetooth_is_on = 0; // set flag to off
			// flag_bluetooth_power_keep_on = 0; 
	}
}

// puts the oled to sleep accrding to the eeprom setting
void handle_lcd_sleep(void)
{
	if (oled_sleep || EEPROM[EERPOM_LCD_POWER_INDEX] == 1) // dont do anything on "on" setting or in sleep mode
		return;
	
	if (eeprom_timer(lcd_button_press_time, EEPROM[EERPOM_LCD_AUTO_TIMEOUT_INDEX])) // true if tiem is up
	{
		oled_sleep = 1;
		OLED.sleepOn();
	}
}

// TODO
void gps_adjust_log_freq(uint8_t in_val) // adjusts the NMEA frequency by sending the EM406A module an appropriate config sentence
{ 
	Serial.print(in_val); Serial.println(" TODO GPS LOG FREQ");
}

// callback for ok button
void fn_ok(m2_el_fnarg_p fnarg)
{
  m2_SetRoot(&top_el_expandable_menu);
}

// callback for temperature 
const char *fn_cb_get_temperature(m2_rom_void_p element)
{
	sprintf(temp + sizeof(char), "%+.2d", temperature); // THE way to print
	strncat( temp + 4*sizeof(char), "C", sizeof(char)); // append C and a null terminator
	return temp;
}

// callback for bluetooth power setting
const char *fn_cb_bluetooth_power_setting(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	// see https://github.com/olikraus/m2tklib/wiki/elref#combofn
	switch(msg) // msg can be one of: M2_COMBOFN_MSG_GET_VALUE, M2_COMBOFN_MSG_SET_VALUE, M2_COMBOFN_MSG_GET_STRING
  {
		case M2_COMBOFN_MSG_GET_VALUE: // we get the vaue from eeprom
			*valptr = EEPROM[EERPOM_BLUETOOTH_POWER_INDEX];
      break;
			
    case M2_COMBOFN_MSG_SET_VALUE: // we set the value into eeprom
			EEPROM[EERPOM_BLUETOOTH_POWER_INDEX] = *valptr;
      break;
			
    case M2_COMBOFN_MSG_GET_STRING: // we get the string _and_ set it (implicitly via M2_COMBOFN_MSG_SET_VALUE) via *valptr
      if (*valptr == 0) // values are coded in eeprom.h
			{
				digitalWrite(bluetooth_mosfet_gate_pin, LOW);
        return "off";
			}
			
      if (*valptr == 1)
			{
				digitalWrite(bluetooth_mosfet_gate_pin, HIGH);
        return "on";
			}
			
      if (*valptr == 2)
			{
				digitalWrite(bluetooth_mosfet_gate_pin, LOW);
        return "auto";
			}
  }
				
  return NULL;
}

// callback for lcd power setting
const char *fn_cb_lcd_power_setting(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	// see fn_cb_bluetooth_power_setting for comments
	switch(msg)
  {
		case M2_COMBOFN_MSG_GET_VALUE:
			*valptr = EEPROM[EERPOM_LCD_POWER_INDEX];
      break;
			
    case M2_COMBOFN_MSG_SET_VALUE:
			EEPROM[EERPOM_LCD_POWER_INDEX] = *valptr;
      break;
			
    case M2_COMBOFN_MSG_GET_STRING:
      if (*valptr == 0)
			{
				//digitalWrite(bluetooth_mosfet_gate_pin, LOW);
        return "auto";
			}
			
      if (*valptr == 1)
			{
				//digitalWrite(bluetooth_mosfet_gate_pin, HIGH);
        return "on";
			}
  }
				
  return NULL;
}

// callback for EEPROM timezone setting
int8_t fn_cb_set_eerpom_tz(m2_rom_void_p element, uint8_t msg, int8_t val)
{
  if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
  {
	timezone = eeprom_get(EERPOM_TIMEZONE_INDEX);
    return timezone;
  }
  
  if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
    eeprom_set(val, EERPOM_TIMEZONE_INDEX);
}

// callback for bluetooth timeout
uint8_t fn_cb_set_eerpom_bluetooth_timeout(m2_rom_void_p element, uint8_t msg, uint8_t val) 
{
  if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
    return (uint8_t) eeprom_get(EERPOM_BLUETOOTH_AUTO_TIMEOUT_INDEX); // set val to the EEPROM value at that index
  
  if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
    eeprom_set(val, EERPOM_BLUETOOTH_AUTO_TIMEOUT_INDEX); // set the EEPROM value at that index to val
}

// callback for lcd timeout
uint8_t fn_cb_set_eerpom_lcd_timeout(m2_rom_void_p element, uint8_t msg, uint8_t val)
{
  if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
    return (uint8_t) eeprom_get(EERPOM_LCD_AUTO_TIMEOUT_INDEX); // set val to the EEPROM value at that index
  
  if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
		eeprom_set(val, EERPOM_LCD_AUTO_TIMEOUT_INDEX); // set the EEPROM value at that index to val
}

// callback for gps log frequency
uint8_t fn_cb_set_eerpom_gps_log_freq(m2_rom_void_p element, uint8_t msg, uint8_t val)
{
  if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
    return (uint8_t) eeprom_get(EEPROM_GPS_GPRMC_GGA_FREQ_INDEX); // set val to the EEPROM value at that index
  
  if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
	{
    eeprom_set(val, EEPROM_GPS_GPRMC_GGA_FREQ_INDEX); // set the EEPROM value at that index to val
		gps_adjust_log_freq(val);
	}
}

// callback for MCP73871 battery charge status
const char *fn_cb_get_batt_charge_status(m2_rom_void_p element)
{
	if (digitalRead(MCP73871_power_good_indicator_pin) == LOW)
	{
		if (digitalRead(MCP73871_charge_status_1_pin) == HIGH || digitalRead(MCP73871_charge_status_2_pin) == LOW)
			return "charging ";
		if (digitalRead(MCP73871_charge_status_1_pin) == LOW || digitalRead(MCP73871_charge_status_2_pin) == HIGH)
			return "charged ";
	}
	
	if (digitalRead(MCP73871_power_good_indicator_pin) == LOW)
	{
		if (digitalRead(MCP73871_charge_status_1_pin) == HIGH || digitalRead(MCP73871_charge_status_2_pin) == LOW)
			return "BATT LOW";
	}		
}

// callback for MCP73871 input power status
const char *fn_cb_get_power_good_status(m2_rom_void_p element)
{
	if (digitalRead(MCP73871_power_good_indicator_pin) == LOW)
		return "ExtPw ok ";
	
		return "ExtPw off ";
}

// interrupt handler for ADXL345
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
