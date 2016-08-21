	//Serial.println(millis());
	m2.checkKey(); // check for key events

	if (m2.handleKey() != 0) // if there are key events in the queue
	{
	 lcd_button_press_time = millis(); // save the button press time
	 
	 // oled wakeup control
	 if (oled_sleep) // if the sleep flag is set
	 {
			oled_sleep = !oled_sleep; // unset the sleep flag
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
  
	// poor man's scheduler - runs roughly every 1s
	if (abs(millis() -  scheduler_last_run) / 1000 > 1 || scheduler_last_run == 0)
	{
		Serial.println(scheduler_run_count);
		avg_temperature(calculate_temperature(), 10); // calculates the temperature via a TMP36 sensor over 10 iterations
		scheduler_last_run = millis();
		scheduler_run_count++;
	}
	
	if (adxl345_int1) // if the ADXL345 INT1 flag is set
		handle_adx_intl(); // execute the function
	

	if(Serial2.available()) //read SIM800 output (if available) and print it in arduino IDE serial monitor
		Serial.write(Serial2.read()); // NL & CR need to be enabled


	if(Serial.available()) //read arduino IDE serial monitor inputs (if available) and send them to SIM800   
			Serial2.write(Serial.read()); // NL & CR need to be enabled 