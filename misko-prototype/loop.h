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

	handle_bluetooth_button(); // handles the "bluetooth power toggle" button
	handle_gprs_button(); // handles the "gprs push" button

 	handle_lcd_sleep(); // checks if it is time for the display to go to sleep

	//Serial.println(millis());
	if (flag_gps_on)	// if the device is turned on
		get_nmea_sentences(); // gets NMEA sentences out of the GPS and deals with them
	//Serial.println(millis());

// GPS device power control
	if( !flag_gps_on && ( (FeRAMReadByte(FERAM_GPS_MISC_CFG) >> FERAM_GPS_MISC_CFG_POWER_CTL) & 0x01 ) ) // user powers GPS on
	{
		digitalWrite(GPS_power_ctl_pin, HIGH); // pull high to wake up
		flag_gps_on = 1;
	}

	if( flag_gps_on && !( (FeRAMReadByte(FERAM_GPS_MISC_CFG) >> FERAM_GPS_MISC_CFG_POWER_CTL) & 0x01 ) ) // user powers GPS off
	{
		digitalWrite(GPS_power_ctl_pin, LOW); // keep low on sleep
		gps.println("$PMTK225,4*2F"); //puts the GPS receiver into backup mode
		flag_gps_on = 0;
		flag_gps_fix = 0;
	}

// GSM power control
// TODO: needs testing
	if( !flag_gsm_on && ( (FeRAMReadByte(FERAM_GSM_MISC_CFG) >> FERAM_GSM_MISC_CFG_POWER_CTL) & 0x01 ) ) // user powers GSM on
	{
		//TODO: must be an impulse of at least 1s

		digitalWrite(SIM800C_power_pin, HIGH); // pull high to wake up
		flag_gsm_on = 1;

		Serial.println(F("gsm on"));

		Serial1.print(F("AT")); // 1st AT
		Serial1.print(F("ATE1")); // turn off command echo
	}

	if( flag_gsm_on && !( (FeRAMReadByte(FERAM_GSM_MISC_CFG) >> FERAM_GSM_MISC_CFG_POWER_CTL) & 0x01 ) ) // user powers GSM off
	{
		//TODO: must be an impulse of at least 1s
		digitalWrite(SIM800C_power_pin, LOW); // keep low on sleep
		flag_gsm_on = 0;

		Serial.println(F("gsm on"));
	}

//FIXME - serial redirection makes garbage
	if ( ( ( FeRAMReadByte(FERAM_DEVICE_MISC_CFG2) >> FERAM_DEVICE_MISC_CFG2_SYSTEM_SERIAL) & 0x03 ) == 0 ) // user selects GPS in serial menu
	{
		// serial redirection for GPS testing
		if(gps.available()) //read GPS output (if available) and print it in arduino IDE serial monitor
			Serial.write(gps.read()); // NL & CR need to be enabled
		if(Serial.available()) //read arduino IDE serial monitor inputs (if available) and send them to GPS
			gps.write(Serial.read()); // NL & CR need to be enabled
	}

//FIXME - serial redirection makes garbage
	if ( ( ( FeRAMReadByte(FERAM_DEVICE_MISC_CFG2) >> FERAM_DEVICE_MISC_CFG2_SYSTEM_SERIAL) & 0x03 ) == 1 ) // user selects GSM in user menu
	{
		// serial redirection for GSM modem testing
		if(Serial1.available()) //read SIM800 output (if available) and print it in arduino IDE serial monitor
			Serial.write(Serial1.read()); // NL & CR need to be enabled

		if(Serial.available()) //read arduino IDE serial monitor inputs (if available) and send them to SIM800
			Serial1.write(Serial.read()); // NL & CR need to be enabled
	}

	if (flag_timer5_handler_execute)
	{
		flag_timer5_handler_execute = 0;

		if (uptime % DEVICE_STATISTICS_FREQUENCY == 0)
			flag_device_do_write_stats = 1;

			// statistical data (voltages, temperatures, etc.)
		val_Vcc = 2 * calculate_voltage(Vcc_sense_pin); // measures Vcc across a voltage divider
		val_temperature = calculate_temperature(); // reads out temperature-dependant voltage

		/* the percentage calculation
			union battery datasheet: charge cutoff voltage: Vbat 4.20V, discharge cutoff voltage: Vbat 2.70V
				over the voltage divider this gives 2.10V and 1.3V
				our voltage divider gives 0.5 Vbat

		 percentage calculation see https://racelogic.support/02VBOX_Motorsport/Video_Data_Loggers/Video_VBOX_Range/Video_VBOX_-_User_manual/24_-_Calculating_Scale_and_Offset

			dX is 2.1 - 1.35 = 0.75
			dY is 100 - 0 = 100
		 the gradient is dX/dY = 133

		 Y = percent = 0, X = Voltage = 1.35V
	 		0 = ((dX/dY)* voltage) + c
			0 = (133 * 1.35) + c <=> 0 = 180 + c <=> c = -180
			our equation is: y = 133 * x - 180

		elementary, dr. watson!
 		*/
		val_batA_pct = (133 * calculate_voltage(bat_A_pin)) - 180; // calculus...
		val_batB_pct = 000;

		// set and unset of the fitness mode ( a MTK3333 chipset feature which depends on velocity of the GPS receiver)
		if (flag_gps_fitness_is_set && gps_speed >= GPS_FITNESS_MODE_THRESHOLD ) // 10 knots == 5.1m/s ( 18,.5km/h ) or faster - 5m/s is the threshold reported in the datasheet
		{
			gps.println("$PMTK886,0*28"); // set to normal mode
			flag_gps_fitness_is_set = 0; // flag fitness mode off
		}

		if (!flag_gps_fitness_is_set && gps_speed < GPS_FITNESS_MODE_THRESHOLD) // lower speed
		{
			gps.println("$PMTK886,1*29"); // enable fitness mode (good for speeds up to 5m/s (== 9.72 knots), for faster speeds normal mode is better)
			flag_gps_fitness_is_set = 1; // flag fitness mode on
		}
	}

	if ( flag_device_do_write_stats && (FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) >> FERAM_DEVICE_MISC_CFG1_STAT_WRITE) & 0x01)
	{
		flag_device_do_write_stats = 0;

		static char tmp_vcc[5] = "";
		static char tmp_tz[4] = "";

		itoa((int8_t) FeRAMReadByte(FERAM_DEVICE_TIMEZONE), tmp_tz, 10);
		dtostrf(val_Vcc, 3, 2, tmp_vcc); // convert the float into a string
		sprintf(statistics_buffer, "%sUTC+%s,%s,%+02d", gps_time, tmp_tz, tmp_vcc, (int8_t) round(val_temperature) );
		Serial.println(statistics_buffer);
	}