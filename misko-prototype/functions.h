// forward declaration of this shitty function (it's at the very end of this file)
void poor_mans_debugging(void);

/* auto_timeout() - returns 1 if enough time has passed
 *  returns:
 *    1 if time is up
 *    0 otherwise
 *
 * input arguments:
 *    uint32_t some_button_press_time - time of button press
 *    uint8_t timeout - timeout in seconds
 *
 * times are in seconds !!
 */
uint8_t inline auto_timeout(uint32_t in_button_press_time, uint8_t timeout_val)
{
  return (abs( in_button_press_time - millis()) / 1000 > timeout_val ?  1 :  0);
}

// reads a byte from the ADXL345
byte adxl345_readByte(byte registerAddress) // reads one byte at registerAddress
{
		digitalWrite(SPI_SS_ADXL345_pin, LOW); // reserve the slave
   	SPI.transfer(0x80 | registerAddress); // set the MSB to 1 (the read command), then send address to read from
		byte retval = SPI.transfer(0x00); // send one byte (0x00) into the circular FIFO buffer, get one byte back
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

// writes one byte data into the FeRAM at address addr
uint8_t FeRAMWriteByte(uint16_t addr, byte data)
{
	if (addr > 0x7FFF) // if we try to go past the max. RAM address
		return 1;	// return something

	uint8_t addr_msb = (addr >> 8) & 0xFF;
	uint8_t addr_lsb = addr & 0xFF;

	// put the FeRAM module into RW mode
	digitalWrite(SPI_FRAM_SS_pin, LOW); // select the FeRAM module
	SPI.transfer(CMD_WREN);	// issue WREN command
	digitalWrite(SPI_FRAM_SS_pin, HIGH); // de-select the FeRAM module

	// put into write mode, send address, then write data
	digitalWrite(SPI_FRAM_SS_pin, LOW); // select the FeRAM module
	SPI.transfer(CMD_WRITE);	// issue WRITE command
	SPI.transfer(addr_msb); // send address most significant byte
	SPI.transfer(addr_lsb); // send address least significant byte

	SPI.transfer(data); // write data into module

	digitalWrite(SPI_FRAM_SS_pin, HIGH); // de-select the FeRAM module

	return 0;
}

// returns one one byte from the FeRAM at addr
byte FeRAMReadByte(uint16_t addr)
{
	byte retval;
	if (addr > 0x7FFF) // if we try to go past the max. RAM address
	return -1;	// return something

	uint8_t addr_msb = (addr >> 8) & 0xFF;
	uint8_t addr_lsb = addr & 0xFF;

	digitalWrite(SPI_FRAM_SS_pin, LOW); // select the FeRAM module
	SPI.transfer(CMD_READ); // send READ command
	SPI.transfer(addr_msb);	// send address most significant byte
	SPI.transfer(addr_lsb); // send address least significant byte

	retval = SPI.transfer(0xFF); // send on byte of crap into the circular buffer, get one byte back

	digitalWrite(SPI_FRAM_SS_pin, HIGH); // de-select the FeRAM module

	return retval; // returns read out data
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

 // returns the external voltage reference in volts
inline float read_Varef() // the external reference voltage is set to 2.50V via a zener diode
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
inline float calculate_temperature(void) // executed from loop()
{
	return (calculate_voltage(TMP36_Vsense_pin) - 0.5) * 100.0 ;  // 10 mv per C, 500 mV offset
}

// callback for Vcc
const char *fn_cb_get_Vcc(m2_rom_void_p element)
{
	static char retval[9]; // will return something like Vcc3.23V
	static char tmp_vcc[5]; // holds e.g "3.23" as an intermediate conversion to string

	dtostrf(val_Vcc, 3, 2, tmp_vcc); // convert the float into a string
	sprintf(retval, "Vcc%sV", tmp_vcc ); // print the string into the retval string
	return retval;
}

// callback for battery percentage
const char *fn_cb_get_bat_pct(m2_rom_void_p element)
{
	dtostrf(val_batA_pct, 3, 0, bat_a_pct + 4*sizeof(char)); // instead, this works
		// http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__avr__stdlib_1ga060c998e77fb5fc0d3168b3ce8771d42.html
	strcat(bat_a_pct, "% ");
	return bat_a_pct;
}

// handler for the bluetooth power toggle button
void handle_bluetooth_button(void)
{
	/* purpose: control BT power based on feram settings

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
		if(digitalRead(menu_bluetooth_power_toggle_pin) == HIGH) // if the button gets pressed
		{
			flag_bluetooth_power_toggle_pressed = 1; // flag the button as pressed, prevents multiple calls
			poor_mans_debugging(); // execute poor mans debugging

	if ( ( (FeRAMReadByte(FERAM_DEVICE_MISC_CFG2) >> FERAM_DEVICE_MISC_CFG2_BLUETOOTH_POWER) & 0x03 ) != 2 )
				return; // do nothing

			if (flag_bluetooth_power_keep_on) // prevent the timer from starting when we want to turn off manually toggled power
				return; // do nothing

			bluetooth_button_press_time = millis(); // record time of button press; this is used in a bit down to keep bt on on auto
			digitalWrite(Bluetooth_wakeup_pin, LOW); // turn on the device
			flag_bluetooth_is_on = 1; // set flag to on
		}
	}
	else // the button is already flagged as pressed == is held pressed
	{
	if ( ( (FeRAMReadByte(FERAM_DEVICE_MISC_CFG2) >> FERAM_DEVICE_MISC_CFG2_BLUETOOTH_POWER) & 0x03 ) != 2 )
			return;

		if ( digitalRead(menu_bluetooth_power_toggle_pin) == LOW) // the button is released
		{
			bluetooth_button_release_time = millis(); // record time of button release
			flag_bluetooth_power_toggle_pressed = 0; // mark button as released

			if (abs(bluetooth_button_release_time - bluetooth_button_press_time) > 500) // if button is held long enough
						flag_bluetooth_power_keep_on = !flag_bluetooth_power_keep_on; // toggle the power state on -> off and vice versa
		}
	}

	if ( ( (FeRAMReadByte(FERAM_DEVICE_MISC_CFG2) >> FERAM_DEVICE_MISC_CFG2_BLUETOOTH_POWER) & 0x03 ) != 2 )
		return; // do nothing

	if (flag_bluetooth_power_keep_on) // if the BT device is marked to be kept on
		return;	// do nothing

	if ( flag_bluetooth_is_on && auto_timeout(bluetooth_button_press_time, FeRAMReadByte(FERAM_BLUETOOTH_AUTO_TIMEOUT)) ) // if the device is on and enough time has passed
	{
			digitalWrite(Bluetooth_wakeup_pin, HIGH); // turn off the device
			flag_bluetooth_is_on = 0; // set flag to off
			// flag_bluetooth_power_keep_on = 0;
	}
}

// puts the oled to sleep accrding to config setting
void handle_lcd_sleep(void)
{
	if (flag_oled_sleep || ( (FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) >> FERAM_DEVICE_MISC_CFG1_OLED_AUTO_POWER) & 0x01 ) ) // dont do anything on "on" setting or in sleep mode
		return;

	if (auto_timeout(lcd_button_press_time, FeRAMReadByte(FERAM_OLED_AUTO_TIMEOUT) )) // true if time is up
	{
		flag_oled_sleep = 1;
		OLED.sleepOn();
	}
}

// calculates NMEA checksum
uint8_t getCheckSum(char *string)
{
  int XOR = 0;

  // for (int i = 1; i < 20; i++)
  for (uint8_t i = 1; i < strlen(string) -1; i++)
    XOR = XOR ^ *(string+i);

  return XOR;
}

// NMEA checksum calculator
int16_t parseHex(char g)
{
  if (g >= '0' && g <= '9')
    return (g - '0');
  else
    if (g >= 'A' && g <= 'F')
      return (g + 10 - 'A');
    else
      if (g >= 'a' && g <= 'f')
        return (g + 10 - 'a');

  return (-1);
}

// sets the RMC/GGA sentence frequency by sending the EM406A module an appropriate NMEA config sentence
void gps_adjust_log_freq(uint8_t in_msg, uint8_t in_val) // example call for GPGGA: gps_adjust_log_freq(04, val) for GPRMC
{
      /* EXAMPLE EM406A

      $PSRF103,<msg>,<mode>,<rate>,<cksumEnable>*CKSUM<CR><LF>
      <msg> 00=GGA,01=GLL,02=GSA,03=GSV,04=RMC,05=VTG
      <mode> 00=SetRate,01=Query
      <rate> Output every <rate>seconds, off=00,max=255
      <cksumEnable> 00=disable Checksum,01=Enable checksum for specified message
      Note: checksum is required

      Example 1: Query the GGA message with checksum enabled
      $PSRF103,00,01,00,01*25

      Example 2: Enable VTG message for a 1Hz constant output with checksum enabled
      $PSRF103,05,00,01,01*20

      Example 3: Disable VTG message
      $PSRF103,05,00,00,01*21

      NMEA checksum:
      http://www.hhhh.org/wiml/proj/nmeaxor.html
      */

			/* EXAMPLE MTK3339
			$PMTK220,1000*1F<CR><LF> -- adjust log frequency

			*/
			if (in_val > 10) // some bug in the menu
				in_val -= 10;

			#ifdef GPS_MTK3339_CHIP
			// here we can set the lof freq globally for all NMEA sentences - a nice feature!
			sprintf(gps_command_buffer, "$PMTK220,%.d000*", in_val); // adjust log freq
			#endif

			sprintf(gps_command_buffer + strlen(gps_command_buffer), "%02X\r\n", getCheckSum(gps_command_buffer));
			gps.write(gps_command_buffer);
	return;
}

// callback for ok button
void fn_ok(m2_el_fnarg_p fnarg)
{
  m2_SetRoot(&top_el_expandable_menu);
}

// callback for UTC display
const char *fn_cb_get_utc(m2_rom_void_p element)
{
	static char retval[4] = ""; // e.g. -12 or  +3
	int8_t tz = (int8_t) FeRAMReadByte(FERAM_DEVICE_TIMEZONE); // fetch the setting from FeRAM

	if (tz < 0 )
		itoa(tz, retval, 10); // will have the "-" automatically
	else
	{
		retval[0] = '+';	// manually set the "+" sign
		retval[1] = '\0'; // terminate
		itoa(tz, retval+sizeof(char), 10);
	}
	return retval;
}

// callback for temperature
const char *fn_cb_get_temperature(m2_rom_void_p element)
{
	static char retval[6]; // holds retval of e.g. "T+24C" of "T-10C"
	sprintf(retval, "T%+02dC", (int8_t) round(val_temperature) ); // round & print the temperature as an integer into the retval string
	return retval;
}

// callback for bluetooth power setting
const char *fn_cb_bluetooth_power_setting(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	uint8_t val;
	// see https://github.com/olikraus/m2tklib/wiki/elref#combofn
	switch(msg) // msg can be one of: M2_COMBOFN_MSG_GET_VALUE, M2_COMBOFN_MSG_SET_VALUE, M2_COMBOFN_MSG_GET_STRING
  {
		case M2_COMBOFN_MSG_GET_VALUE: // we get the vaue from feram
			*valptr = (FeRAMReadByte(FERAM_DEVICE_MISC_CFG2) >> FERAM_DEVICE_MISC_CFG2_BLUETOOTH_POWER) & 0x03; // see if bit 1 is set
      break;

    case M2_COMBOFN_MSG_SET_VALUE: // we set the value into feram
			val = FeRAMReadByte(FERAM_DEVICE_MISC_CFG2); // retrieve byte
			// TODO: these two lines can probably be written more beautiful
			val &= ~ (1 << FERAM_DEVICE_MISC_CFG2_BLUETOOTH_POWER); // hopefully set these two bits to 0 (so that they dont poison the subsequent write)
			val &= ~ (1 << (FERAM_DEVICE_MISC_CFG2_BLUETOOTH_POWER +1) );
			FeRAMWriteByte( FERAM_DEVICE_MISC_CFG2, val ^ (*valptr << FERAM_DEVICE_MISC_CFG2_BLUETOOTH_POWER) );	// now save the new bit values & write the crap
      break;

    case M2_COMBOFN_MSG_GET_STRING: // we get the string _and_ set it (implicitly via M2_COMBOFN_MSG_SET_VALUE) via *valptr
      if (*valptr == 0) // values are coded in fram.h
			{
				digitalWrite(Bluetooth_wakeup_pin, HIGH);
        return "off";
			}

      if (*valptr == 1)
			{
				digitalWrite(Bluetooth_wakeup_pin, LOW);
        return "on";
			}

      if (*valptr == 2)
			{
				digitalWrite(Bluetooth_wakeup_pin, HIGH);
        return "auto";
			}
  }

  return NULL;
}

// callback for OLED power setting
const char *fn_cb_lcd_power_setting(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	// see fn_cb_bluetooth_power_setting for comments
	switch(msg)
  {
		case M2_COMBOFN_MSG_GET_VALUE:
			*valptr = (FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) >> FERAM_DEVICE_MISC_CFG1_OLED_AUTO_POWER) & 0x01; // see if bit 1 is set
			break;

    case M2_COMBOFN_MSG_SET_VALUE:
			FeRAMWriteByte(FERAM_DEVICE_MISC_CFG1, FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) ^ (1<<FERAM_DEVICE_MISC_CFG1_OLED_AUTO_POWER)); // set it 1
			break;

    case M2_COMBOFN_MSG_GET_STRING:
      if (*valptr == 0)
        return "auto";

      if (*valptr == 1)
        return "on";
  }

  return NULL;
}

// callback for GPS power setting
const char *fn_cb_gps_power_setting(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	// see fn_cb_bluetooth_power_setting for comments
	switch(msg)
  {
		case M2_COMBOFN_MSG_GET_VALUE:
			*valptr = (FeRAMReadByte(FERAM_GPS_MISC_CFG) >> FERAM_GPS_MISC_CFG_POWER_CTL) & 0x01;
      break;

    case M2_COMBOFN_MSG_SET_VALUE:
			FeRAMWriteByte(FERAM_GPS_MISC_CFG, FeRAMReadByte(FERAM_GPS_MISC_CFG) ^ (1<<FERAM_GPS_MISC_CFG_POWER_CTL)); // set it 1
			break;

    case M2_COMBOFN_MSG_GET_STRING:
      if (*valptr == 0)
        return "off";
		//if (*valptr == 1)
		else
        return "on";
  }

  return NULL;
}

// callback for NMEA sentence printout setting
const char *fn_cb_nmea_printout_setting(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	// see fn_cb_bluetooth_power_setting for comments
	switch(msg)
	{
		case M2_COMBOFN_MSG_GET_VALUE:
		*valptr = (FeRAMReadByte(FERAM_GPS_MISC_CFG) >> FERAM_GPS_MISC_CFG_NMEA_PRINT_TO_SERIAL) & 0x01;
		break;

		case M2_COMBOFN_MSG_SET_VALUE:
		FeRAMWriteByte(FERAM_GPS_MISC_CFG, FeRAMReadByte(FERAM_GPS_MISC_CFG) ^ (1<<FERAM_GPS_MISC_CFG_NMEA_PRINT_TO_SERIAL)); // set it 1
		break;

		case M2_COMBOFN_MSG_GET_STRING:
		if (*valptr == 0)
			return "off";
		//if (*valptr == 1)
		else
			return "on";
	}

	return NULL;
}

// callback for serial port setting
const char *fn_cb_serial_setting(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	// see fn_cb_bluetooth_power_setting for comments
	switch(msg)
  {
		uint8_t val;
		case M2_COMBOFN_MSG_GET_VALUE:
			*valptr = (FeRAMReadByte(FERAM_DEVICE_MISC_CFG2) >> FERAM_DEVICE_MISC_CFG2_SYSTEM_SERIAL) & 0x03; // see if bit 1 is set
			break;

    case M2_COMBOFN_MSG_SET_VALUE:
			val = FeRAMReadByte(FERAM_DEVICE_MISC_CFG2); // retrieve byte
			// TODO: these two lines can probably be written more beautiful
			val &= ~ (1 << FERAM_DEVICE_MISC_CFG2_SYSTEM_SERIAL); // hopefully set these two bits to 0 (so that they dont poison the subsequent write)
			val &= ~ (1 << (FERAM_DEVICE_MISC_CFG2_SYSTEM_SERIAL +1) );
			FeRAMWriteByte( FERAM_DEVICE_MISC_CFG2, val ^ (*valptr << FERAM_DEVICE_MISC_CFG2_SYSTEM_SERIAL) );	// now save the new bit values & write the crap
			break;

    case M2_COMBOFN_MSG_GET_STRING:
      if (*valptr == 0)
        return "GPS";

      if (*valptr == 1)
        return "GSM";

			if (*valptr == 2)
        return "system";

	}

  return NULL;
}

// callback for OLED power setting
const char *fn_cb_gsm_power(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	// see fn_cb_bluetooth_power_setting for comments
	switch(msg)
  {
		case M2_COMBOFN_MSG_GET_VALUE:
			*valptr = flag_cb_gsm_power;
			break;

    case M2_COMBOFN_MSG_SET_VALUE:
			flag_cb_gsm_power = *valptr;
			break;

    case M2_COMBOFN_MSG_GET_STRING:
      if (*valptr == 0)
			{
        return "off";
			}

      if (*valptr == 1)
			{
        return "on";
			}
  }

  return NULL;
}

// callback for SD write setting
const char *fn_cb_sd_write(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	// see fn_cb_bluetooth_power_setting for comments
	switch(msg)
  {
		case M2_COMBOFN_MSG_GET_VALUE:
			*valptr = (FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) >> FERAM_DEVICE_MISC_CFG1_SD_WRITE) & 0x01;
			break;

    case M2_COMBOFN_MSG_SET_VALUE:
			FeRAMWriteByte(FERAM_DEVICE_MISC_CFG1, FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) ^  (1<<FERAM_DEVICE_MISC_CFG1_SD_WRITE) ); // set it 1
			break;

    case M2_COMBOFN_MSG_GET_STRING:
      if (*valptr == 0)
        return "off";
		if (*valptr == 1)
        return "on";
  }

  return NULL;
}

// callback for statistics write setting
const char *fn_cb_stat_write(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	// see fn_cb_bluetooth_power_setting for comments
	switch(msg)
	{
		case M2_COMBOFN_MSG_GET_VALUE:
		*valptr = (FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) >> FERAM_DEVICE_MISC_CFG1_STAT_WRITE) & 0x01;
		break;

		case M2_COMBOFN_MSG_SET_VALUE:
		FeRAMWriteByte(FERAM_DEVICE_MISC_CFG1, FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) ^  (1<<FERAM_DEVICE_MISC_CFG1_STAT_WRITE) ); // set it 1
		break;

		case M2_COMBOFN_MSG_GET_STRING:
		if (*valptr == 0)
		return "off";
		if (*valptr == 1)
		return "on";
	}

	return NULL;
}

// callback for timezone setting
int8_t fn_cb_set_tz(m2_rom_void_p element, uint8_t msg, int8_t val)
{
  if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
    return (uint8_t) FeRAMReadByte(FERAM_DEVICE_TIMEZONE);

  if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
		FeRAMWriteByte(FERAM_DEVICE_TIMEZONE, val);

	return FeRAMReadByte(FERAM_DEVICE_TIMEZONE);
}

// callback for ADXL345 activity power setting
const char *fn_cb_accel_enable(m2_rom_void_p element, uint8_t msg, uint8_t *valptr)
{
	// see fn_cb_bluetooth_power_setting for comments
	switch(msg)
	{
		case M2_COMBOFN_MSG_GET_VALUE:
			*valptr = ( FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) >> FERAM_DEVICE_MISC_CFG1_ADXL345_AUTO_POWER ) & 0x01;
			break;

		case M2_COMBOFN_MSG_SET_VALUE:
			FeRAMWriteByte(FERAM_DEVICE_MISC_CFG1, FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) ^ (1<<FERAM_DEVICE_MISC_CFG1_ADXL345_AUTO_POWER));
			break;

		case M2_COMBOFN_MSG_GET_STRING:
		if (*valptr == 0)
		{
			adxl345_writeByte(INT_ENABLE, INT_DISABLE_CFG);
			return "off";
		}
		else
		{
			adxl345_writeByte(INT_ENABLE, INT_ENABLE_CFG);
			return "on";
		}
	}

	return NULL;
}

// callback for accelerometer timeout value setting
int8_t fn_cb_set_accel_timeout(m2_rom_void_p element, uint8_t msg, int8_t val)
{
	if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
	return (uint8_t) FeRAMReadByte(FERAM_ADXL345_MOVEMENT_TIMEOUT);

	if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
	{
	 FeRAMWriteByte(FERAM_ADXL345_MOVEMENT_TIMEOUT, val);
	 adxl345_writeByte(TIME_INACT, val);
	}
	return FeRAMReadByte(FERAM_ADXL345_MOVEMENT_TIMEOUT);
}

// callback for accelerometer inativity threshold setting
int8_t fn_cb_set_inact_threshold(m2_rom_void_p element, uint8_t msg, int8_t val)
{
	if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
	return (uint8_t) FeRAMReadByte(FERAM_ADXL345_INACTIVITY_THRESHOLD);

	if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
	{
		FeRAMWriteByte(FERAM_ADXL345_INACTIVITY_THRESHOLD, val);
		adxl345_writeByte(TIME_INACT, val);
	}
	return FeRAMReadByte(FERAM_ADXL345_INACTIVITY_THRESHOLD);
}

// callback for accelerometer ativity threshold setting
int8_t fn_cb_set_act_threshold(m2_rom_void_p element, uint8_t msg, int8_t val)
{
	if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
	return (uint8_t) FeRAMReadByte(FERAM_ADXL345_ACTIVITY_THRESHOLD);

	if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
	{
		FeRAMWriteByte(FERAM_ADXL345_ACTIVITY_THRESHOLD, val);
		adxl345_writeByte(TIME_INACT, val);
	}
	return FeRAMReadByte(FERAM_ADXL345_ACTIVITY_THRESHOLD);
}

// callback for bluetooth timeout
uint8_t fn_cb_set_bluetooth_timeout(m2_rom_void_p element, uint8_t msg, uint8_t val)
{
  if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
    return (uint8_t) FeRAMReadByte(FERAM_BLUETOOTH_AUTO_TIMEOUT); // get value

  if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
		FeRAMWriteByte(FERAM_BLUETOOTH_AUTO_TIMEOUT, val);

	return (uint8_t) FeRAMReadByte(FERAM_BLUETOOTH_AUTO_TIMEOUT);
}

// callback for OLED timeout
uint8_t fn_cb_set_oled_timeout(m2_rom_void_p element, uint8_t msg, uint8_t val)
{
  if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
		return (uint8_t) FeRAMReadByte(FERAM_OLED_AUTO_TIMEOUT); // get val

  if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
		FeRAMWriteByte(FERAM_OLED_AUTO_TIMEOUT, val); // set val

	return (uint8_t) FeRAMReadByte(FERAM_OLED_AUTO_TIMEOUT); // ...so that we dont have an non-return for this type of function
}

// callback for gps log frequency
uint8_t fn_cb_set_eerpom_gps_log_freq(m2_rom_void_p element, uint8_t msg, uint8_t val)
{
  if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
    return (uint8_t) eeprom_get(EEPROM_GPS_GPRMC_GGA_FREQ_INDEX); // set val to the EEPROM value at that index

  if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
	{
    eeprom_set(val, EEPROM_GPS_GPRMC_GGA_FREQ_INDEX); // set the EEPROM value at that index to val
		gps_adjust_log_freq(00, val); // GPGGA
		gps_adjust_log_freq(04, val); // GPRMC
	}
}

// callback for MCP73871 battery charge status
const char *fn_cb_get_batt_charge_status(m2_rom_void_p element)
{
	if (digitalRead(MCP73871_power_good_pin) == LOW)
	{
		if (digitalRead(MCP73871_charge_status_1_pin) == HIGH || digitalRead(MCP73871_charge_status_2_pin) == LOW)
			return "charging ";
		if (digitalRead(MCP73871_charge_status_1_pin) == LOW || digitalRead(MCP73871_charge_status_2_pin) == HIGH)
			return "charged ";
	}

	if (digitalRead(MCP73871_power_good_pin) == HIGH)
	{
		if (digitalRead(MCP73871_charge_status_1_pin) == LOW || digitalRead(MCP73871_charge_status_2_pin) == HIGH)
			return "BATT LOW";
	}

	return "foo";
}

// callback for MCP73871 input power status
const char *fn_cb_get_power_good_status(m2_rom_void_p element)
{
	if (digitalRead(MCP73871_power_good_pin) == LOW)
		return "ExtPw ok ";

		return "ExtPw off ";
}

// callback for gps latitude
const char *fn_cb_gps_latitude(m2_rom_void_p element)
{
		return gps_latitude;
}

// callback for gps longtitude
const char *fn_cb_gps_longtitude(m2_rom_void_p element)
{
		return gps_longtitude;
}

// callback for gps altitude
const char *fn_cb_gps_altitude(m2_rom_void_p element)
{
	static char retval[10]; // will return e.g. "-333m" or "+5555m"
	uint8_t len = strlen(gps_altitude);

	if (len == 0) // if no altitude is given
		sprintf(retval, "alt -m"); // print "alt" and a dummy into retval
	else
		sprintf(retval, "alt %sm", gps_altitude); // print "alt" and the actual number-string into retval

	return retval;
}

// callback for gps satellites in view
const char *fn_cb_gps_satellites_in_view(m2_rom_void_p element)
{
	static char retval[7]; // will return something like "0.00" or "99.99"
	uint8_t len = strlen(gps_satellites_in_view); // lenght of HDOP string

	if (len == 0) // empty string (only a null terminator)
		sprintf(retval, "sat --"); // copy "sat" and the actual number-string into retval
	else
		sprintf(retval, "sat %s", gps_satellites_in_view); // copy "sat" and the actual number-string into retval

	return retval; // return that val
}

// callback for the position fix indicator
const char *fn_cb_gps_fix_indicator(m2_rom_void_p element)
{
	if (gps_position_fix_indicator == '0')
		return "inv.";

	if (gps_position_fix_indicator == '1')
		return "SPS";

	if (gps_position_fix_indicator == '2')
		return "DGPS";
}

// callback for gps HDOP
const char *fn_cb_gps_hdop(m2_rom_void_p element)
{
	static char retval[10];
	uint8_t len = strlen(gps_hdop);

	if (len == 0) // empty string or not?
		sprintf(retval, "hdop --.--"); // copy "hdop" and the actual number-string into retval
	else
		sprintf(retval, "hdop %s", gps_hdop); // copy "hdop" and the actual number-string into retval

	return retval; // return that val
}

// send contents of file over serial to host PC
void print_to_serial(const char *in_string)
{
	Serial.println(in_string);
	 File dataFile = SD.open(in_string);

  if (dataFile)
	{
		Serial.println(F("START ---"));
		Serial.print(in_string);Serial.print(F(" size "));Serial.print(dataFile.size()/1024);Serial.println(F("kB"));

    while (dataFile.available())
		{
      Serial.write(dataFile.read());
    }

    dataFile.close();
		Serial.println();
		Serial.print(in_string);Serial.println(F(" END ---"));
		Serial.println();
}
}

//M2tk function for FAT directory content retrieval
const char *fs_strlist_getstr(uint8_t idx, uint8_t msg)
{
  if (msg == M2_STRLIST_MSG_GET_STR)
	{
    /* Check for the extra button: Return string for this extra button */
    if ( idx == 0 )
      return "..";

		/* Not the extra button: Return file/directory name */

    mas_GetDirEntry(idx - 1);
    return mas_GetFilename();
  }

	if ( msg == M2_STRLIST_MSG_GET_EXTENDED_STR )
	{
		/* Check for the extra button: Return icon for this extra button */
		if ( idx == 0 )
			return "a";       /* arrow left of the m2icon font */

		/* Not the extra button: Return file or directory icon */
		mas_GetDirEntry(idx - 1);

		if ( mas_IsDir() )
			return "A";       /* folder icon of the m2icon font */
		return "B";         /* file icon of the m2icon font */
	}

	if ( msg == M2_STRLIST_MSG_SELECT ) // button right
	{
		/* Check for the extra button: Execute button action */
		if ( idx == 0 )
		{
				if ( mas_GetPath()[0] == '\0' )
					m2_SetRoot(&top_el_expandable_menu);
				else
				{
					mas_ChDirUp();
					m2_SetRoot(m2_GetRoot());  /* reset menu to first element, send NEW_DIALOG and force recount */
				}
					/* Not the extra button: Goto subdir or return (with selected file) */
		}
		else
		{
				mas_GetDirEntry(idx - 1);
				if ( mas_IsDir() )
				{
					mas_ChDir(mas_GetFilename());
					m2_SetRoot(m2_GetRoot());  /* reset menu to first element, send NEW_DIALOG and force recount */
				}
				else
				{
						print_to_serial(mas_GetFilename()); // send the filename to print_to_serial() for TX over serial to e.g. putty
				}
			}
	}

	if ( msg == M2_STRLIST_MSG_NEW_DIALOG )
	{Serial.println(mas_GetDirEntryCnt());
		/* (re-) calculate number of entries, limit no of entries to 250 */
		if ( mas_GetDirEntryCnt() < 250-1 )
			fs_m2tk_cnt = mas_GetDirEntryCnt()+1;
		else
			fs_m2tk_cnt = 250;
	}

	return NULL;
}

// switched GSM on / off, inits on "on"
void gsm_power(bool in_val)
{
	if (in_val)
	{
		digitalWrite(SIM800C_power_pin, HIGH);
		flag_gsm_on = 1;

		// TODO - software serial via SIM800L_sw_serial_tx and SIM800L_sw_serial_rx
		Serial.println(F("gsm on"));

		delay(10);
		Serial1.print(F("AT")); // 1st AT
		Serial1.print(F("ATE0")); // turn off command echo
	}
	else
	{
		digitalWrite(SIM800C_power_pin, LOW);
		flag_gsm_on = 0;
		Serial.println(F("gsm off"));
		//Serial1.end();
	}
}

// buffered write onto SD card utilizing a circular buffer
void sd_buffer_write(char *in_string, uint8_t in_size)
// TODO add 3rd argument: the dst buffer
//void sd_buffer_write(char *dst_buffer, char *in_string, uint8_t in_size)

{
	// pointer arithmetrics, beware!
	static char *end = sd_buffer + SD_BUFFERSIZE; // end address of buffer
	static char *target = sd_buffer; // at init time is start address of sd_buffer, otherwise is current of next free write dst. addr
	static bool flag_flush_1st = 0; // flag 1st half of buffer for flushing
	static bool flag_flush_2nd = 0; // flag 2nd half of buffer for flushing

	#if BUFFER_DEBUG_PRINT
		Serial.print("buffer start:");Serial.println((uint16_t)sd_buffer);
		Serial.print("buffer end:");Serial.println((uint16_t) end);
		Serial.print("target:");Serial.println((uint16_t) target - (uint16_t)sd_buffer );
		Serial.print("space left:");Serial.println((uint16_t) end - (uint16_t)target );
		Serial.print("in_size");Serial.println(in_size);
	#endif

	/* we need to cyclically write into the buffer and make sure we dont write past the buffer end address:
			if we have a sring that would go past buffer end address, we truncate it, fill up the buffer until
			the end addr with as much string that fits and then write the rest of the string at the beginning of
			the buffer.

		every time we do this, we need to check if the 1st half (or the 2nd half) of the buffer are filled
			with fresh data; if so, we need to flush it onto the SD card.

		*in_string points to the global variable NMEA_buffer, in_size bytes in lenght.
			per NMEA standard, the NMEA sentence can be maximally 82 characters long (80 + \r\n)
	*/
	if (target < end - (in_size * sizeof(char)) ) // if one NMEA sentence will fit
	// simply write into the buffer
	{
		#if BUFFER_DEBUG_PRINT
			Serial.println("if");
		#endif

		target = (char *) memcpy(target, in_string, in_size); // copy onto location of c and return position of destination
		target += in_size*sizeof(char); // advance target by ammount of bytes copied to the next free space

		if (!flag_flush_1st && target >= sd_buffer + 512* sizeof(char)) // if we have filled 1st 512 byte buffer
			flag_flush_1st = 1; // mark the 1st buffer for flushing

		if (!flag_flush_2nd && target < sd_buffer + 512* sizeof(char)) // if we have filled 1st 512 byte buffer
			flag_flush_2nd = 1; // mark the 2nd buffer for flushing

	}
	else // the incoming sentence will not fully fit
	// write what fits until the end of sd_buffer and the rest from beginning of sd_buffer
	{
		#if BUFFER_DEBUG_PRINT
			Serial.println("else");
		#endif

		uint16_t len = (uint16_t)end - (uint16_t)target; // how much free space is left? [difference in addresses]
		uint16_t rest = (uint16_t)in_size - len; // how much will not fit? [difference in number of bytes]

		#if BUFFER_DEBUG_PRINT
			Serial.print("len:");Serial.println(len);
			Serial.print("rest:");Serial.println(rest );
		#endif

		// first, flush the 1st half of the buffer
		if (flag_flush_1st)
		{
			digitalWrite(gps_red_led_pin, LOW); // turn on led to make write cycle start visible

			#if BUFFER_DEBUG_PRINT
				Serial.println("flushing 1st half");
			#else
				gpslogfile.write(sd_buffer, 512*sizeof(char)); // write 1st half of the buffer
				gpslogfile.flush(); // flush all to card
			#endif

			flag_flush_1st = 0; // unmark it
		}

		memcpy(target, in_string, len); // write that much into buffer
		target = sd_buffer; // set pointer back to start

		target = (char *) memcpy(target, in_string+len*sizeof(char), rest*sizeof(char)); // write the rest
		target += rest*sizeof(char); // advance target by ammount of bytes copied to the next free space

		// now we can flush the 2nd half of the buffer
		if (flag_flush_2nd)
		{
			#if BUFFER_DEBUG_PRINT
				Serial.println("flushing 2nd half");
			#else
				gpslogfile.write(sd_buffer+512*sizeof(char), 512*sizeof(char)); // write 2nd half of the buffer
				gpslogfile.flush(); // flush all to card
			#endif

			flag_flush_2nd = 0; // unmark it
			digitalWrite(gps_red_led_pin, HIGH); // turn off led to make write cycle end visible
		}
	}
}

// determines baud rate (approximately)
uint32_t detectBaud(int pin)
{
	uint16_t rate = 10000;
	uint16_t x;

	for (uint8_t i = 0; i < 10; i++)
	{
		x = pulseIn(pin,LOW);   // measure the next zero bit width
    rate = x < rate ? x : rate;
	}

	if (rate < 12)
		return 115200;

  if (rate < 20)
		return 57600;

	if (rate < 29)
		return 38400;

	if (rate < 40)
		return 28800;

	if (rate < 60)
		return 19200;

	if (rate < 80)
		return 14400;

	if (rate < 150)
		return 9600;

	if (rate < 300)
		return 4800;

	if (rate < 600)
		return 2400;

	if (rate < 1200)
		return 1200;

  return 0;
 }

// primitive BT button-activated printout
void poor_mans_debugging(void)
{
	// poor man's debugging

		//Serial.print("temp:");Serial.print(temp);Serial.println(":");

	//Serial.print("FERAM_DEVICE_MISC_CFG2 ");
	//Serial.println(FeRAMReadByte(FERAM_DEVICE_MISC_CFG2), BIN);

	Serial.print("bt:");Serial.print(FeRAMReadByte(FERAM_DEVICE_MISC_CFG2), BIN);Serial.println(":bt");

/* 		char buffer[82];
		sprintf(buffer, "$PSRF104,%s,%s,%s,75000,%s,%s,12,1", gps_latitude, gps_longtitude, gps_altitude, gps_time_of_week, gps_week);
		Serial.println(buffer);
		//Serial1.write("$PSRF109,124*34\r\n"); */

}
