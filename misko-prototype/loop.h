	//Serial.println(millis());
	m2.checkKey(); // check for key events

	if (m2.handleKey() != 0) // if there are key events in the queue
	{
		lcd_button_press_time = millis(); // save the button press time

		// oled wakeup control
		if (flag_oled_sleep) // if the sleep flag is set
		{
			flag_oled_sleep = !flag_oled_sleep; // unset the sleep flag
			OLED.sleepOff(); // wake up the oled
		}

		// picture loop - https://github.com/olikraus/u8glib/wiki/tpictureloop
			OLED.firstPage();  // https://github.com/olikraus/u8glib/wiki/userreference#firstpage

			do
			{
				m2.draw(); // draw the display
			}
			while( OLED.nextPage() ); // https://github.com/olikraus/u8glib/wiki/userreference#nextpage
	}
	//Serial.println(millis());

	handle_bluetooth_button(); // handles the bluetooth power button
 	handle_lcd_sleep(); // checks if it is time for the display to go to sleep

	//Serial.println(millis());
	get_nmea_sentences(); // gets NMEA sentences out of the GPS and deals with them
	//Serial.println(millis());

	if (flag_adxl345_int1) // if the ADXL345 INT1 flag is set
		handle_adx_intl(); // execute the function

	if( !flag_gps_on && eeprom_get(EERPOM_GPS_POWER_INDEX)) // user powers GPS on
	{
		digitalWrite(GPS_power_ctl_pin, HIGH); // pull high to wake up
		flag_gps_on = 1;
	}

	if(flag_gps_on && !eeprom_get(EERPOM_GPS_POWER_INDEX)) // user powers GPS off
	{
		digitalWrite(GPS_power_ctl_pin, LOW); // keep low on sleep
		gps.println("$PMTK225,4*2F"); //puts the GPS receiver into backup mode
		flag_gps_on = 0;
		flag_gps_fix = 0;
	}

	//FIXME
	if (eeprom_get(EERPOM_SERIAL_SETTING_INDEX) == 0) // user selects GPS in serial menu
	{
		// serial redirection for GPS testing
		if(gps.available()) //read GPS output (if available) and print it in arduino IDE serial monitor
			Serial.write(gps.read()); // NL & CR need to be enabled

		if(Serial.available()) //read arduino IDE serial monitor inputs (if available) and send them to GPS
			gps.write(Serial.read()); // NL & CR need to be enabled
	}

	//FIXME
	//if (eeprom_get(EERPOM_SERIAL_SETTING_INDEX) == 1) // user selects GSM in user menu
	//{
		//// serial redirection for GSM modem testing
		//if(Serial1.available()) //read SIM800 output (if available) and print it in arduino IDE serial monitor
			//Serial.write(Serial1.read()); // NL & CR need to be enabled
//
		//if(Serial.available()) //read arduino IDE serial monitor inputs (if available) and send them to SIM800
			//Serial1.write(Serial.read()); // NL & CR need to be enabled
	//}

// GSM modem power control
	if (flag_cb_gsm_power && !flag_gsm_on)
		gsm_power(1);
	if (!flag_cb_gsm_power && flag_gsm_on)
		gsm_power(0);
