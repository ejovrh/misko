
// determines fix or not, parses coordinates, datetime
void gps_parse_gprmc() // KLUDGE
{
  // sample NMEA GPRMC sentence
  //		$GNRMC,214329.000,A,4547.9089,N,01555.1553,E,1.33,121.79,121116,,,A*7A
  //    $GPRMC,221939.869,V,,,,,,,060816,,,N*41
  //    $GNRMC,214325.073,V,,,,,1.48,112.99,121116,,,N*5F
  //
	//
	// fields of interest (0-indexed) are marked with an X:
	//
	// #0 - talker ID
	// #1 - UTC time
	// #2 - fix indicator: A - valid, V - invalid
	// #3 - latitude; "4547.9089" in above example
	// #4 - latitude indicator; "N" in above example
	// #5 - longtitude; "01555.1553" in above example
	// #6 - longtitude indicator; "E" in above exmaple
	// #7 - speed over ground in knots; "1.33" in above example
	// #8 - course, ; "121.79" in above example
	// #9 - date in DDMMYY format; "121116" in above example
	// #10 - magnetic variation
	// #11 - magnetic variation indicator

  // real programmers would probably do this in a more elegant way..
  // strtok would be a much cooler way but i dont want to have a loop within a loop (nema parser) (within a loop (loop()) )
  //  p = strchr(p, ',')+1, on the other hand, lets me seek the next comma in a non destructive way
  char *p;  // pointer for parsing

  // field 2 - hhmmss time data: 170942.000,A,4547.9094,N,01555.1254,E,0.13,142.38,050816,,,A*63
  p = NMEA_buffer+7;  // set pointer to proper position: 154951.285,A,4547.8814,N,01555.2455,E,0.92,115.67,020814,,,A*69
  memcpy(gps_time, p, 6* sizeof(char)); // fill char gps_time[7] = "XXXXXX" by copying from one array into another

  // field 3 - fix indicator: A,4547.9094,N,01555.1254,E,0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1; // finds position of next comma and puts the cursor one position further

	flag_gps_fix = ( *p == 'A' ? 1 : 0 ); // sets flag_gps_fix to 1 if there is a fix

  // field 4 - latitude: 4547.9094,N,01555.1254,E,0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1;
  if (flag_gps_fix)
    memcpy(gps_latitude+(4*sizeof(char)), p, 9 * sizeof(char)); // fill up gps_latitude[] , part 1

  // field 5 - latitude indicator: N,01555.1254,E,0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1;
  if (flag_gps_fix)
    memcpy(gps_latitude+(13*sizeof(char)), p, sizeof(char)); // fill up gps_latitude[] , part 2

  // field 6 - longtitude: 01555.1254,E,0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1;
  if (flag_gps_fix)
    memcpy(gps_longtitude+(4*sizeof(char)), p, 10 * sizeof(char)); // fill up gps_longtitude[] , part 1

  // field 7 - longtitude indicator: E,0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1;
  if (flag_gps_fix)
    memcpy(gps_longtitude+(14*sizeof(char)), p, sizeof(char)); // fill up gps_longtitude[] , part 2, appends letter

  // field 8 - speed over ground: 0.13,142.38,050816,,,A*63
	p = strchr(p, ',')+1;

  if (flag_gps_fix)
	// TODO: this needs to be verified under real world conditions (i.e. real movement with varying speeds)
		gps_speed = atoi( strtok(p, ".") ); // tokenize and convert what's left of the dot to an integer

  // [not needed] field 9 - course over ground: 142.38,050816,,,A*63
  p = strchr(p, ',')+1;

  // field 10 - date: 050816,,,A*63
  p = strchr(p, ',')+1;

  // fill gps_date
      *(gps_date+2) = *(p+4);  // Y - 1
      *(gps_date+3) = *(p+5);  // Y - 6
      *(gps_date+4) = *(p+2);  // M - 0
      *(gps_date+5) = *(p+3);  // M - 8
      *(gps_date+6) = *(p+0);  // D - 0
      *(gps_date+7) = *(p+1);  // D - 5
    //Serial.print("gps_date: ");  Serial.println(gps_date);
    //Serial.print("gps_time: ");  Serial.println(gps_time);
}

// parses out satellites used, HDOP, MSL altitude
void gps_parse_gpgga(char *in_str)
{
	// $GNGGA,214323.073,,,,,0,0,,,M,,M,,*57
	// $GNGGA,214326.073,4547.9072,N,01555.1584,E,1,5,1.59,140.9,M,42.5,M,,*43
	//
	// fields of interest (0-indexed) are marked with an X:
	//
	//  #0 - talker ID
	//  #1 - UTC time
	//  #2 - latitude
	//  #3 - north/south
	//  #4 - longtitude
	//  #5 - east/west
	//X	#6 - position fix indicator: "1" after the "E," in the above example)
	//		0 - invalid
	//		1 - GPS (SPS)
	//		2 - DGPS
	//		3 - PPS
	//		4 - real time kinetic
	//		5 - float real time kinetic
	//		6 - estimated
	//		7 - manual input
	//		8 - simulation mode
	//X	#7 - satellites used: "5" in the above example
	//X	#8 - HDOP: "1.59" in the above example
	//X	#9 - MSL altitude: "140.9" in the above example
	//	#10 - unit: "M"
	//	#11 - height above geoid: "42.5" in the above example
	//	#12 - unit: "M"

	char *p = strtok(in_str, ","); // char pointer for strtok
	uint8_t i = 0; // counter for the string tokenizer
	uint8_t len = 0; // how long is a piece of string?

	while (*p) // for as long as there is something to tokenize with the given delimiter...
	{
		if (i == 6) // position fix indicator
			gps_position_fix_indicator = *p;

		if (i == 7) // satellites used
		{
			len = strcspn (p, ",");
			memcpy( gps_satellites_in_view, p, len * sizeof(char) );
			*(gps_satellites_in_view+len+1) = '\0';
		}

		if (i == 8) // hdop
		{
			len = strcspn (p, ",");
			memcpy( gps_hdop, p, len * sizeof(char) ); // copy the value
			*(gps_hdop+len+1) = '\0'; // terminate the string
		}

		if (i == 9) // altitude
		{
			len = strcspn (p, ".,");
			memcpy( gps_altitude, p, len * sizeof(char) ); // copy the numbers...
			*(p+3+len+1) = '\0'; // at the next position, terminate the string
		}

		p = strtok(NULL, ",");
		i++;
	}
}

void get_nmea_sentences() {
// reads one char at a time from the gps device;
//  due to iteration from the main loop, eventually the whole NMEA sentence accumulates in the global buffer variable

  uint8_t sum; // variable for the NMEA checksum of each sentence

  if (gps.available()) // if gps is availiable
  {
    *(NMEA_buffer+bufferid) = gps.read();      // put byte by byte into the array "buffer"

    if ( *(NMEA_buffer+bufferid) == '\n' )           // if we have recieved a newline -- read  http://www.gammon.com.au/forum/?id=11425
               //it means we are at the end of the NMEA sentence and we can start to parse it (the GPS reciever will terminate each NMEA sentece with at '\n' )
    {
      *(NMEA_buffer+(bufferid+1) ) = '\0'; // terminate it with a NULL terminator

      if ( *(NMEA_buffer+(bufferid-4)) != '*' ) // no "*" indicating no checksum field
      {
        bufferid = 0;
        return;  // break out of the loop and read next iterations
      }

      sum = parseHex(NMEA_buffer[bufferid-3]) * 16; // sum the last fields for the checksum
      sum += parseHex(NMEA_buffer[bufferid-2]);

      for (uint8_t i=1; i < (bufferid-4); i++)
        sum ^= *(NMEA_buffer+i); // check checksum

      if (sum != 0) // checksum bad
      {
        bufferid = 0; // set pointer back to the beginning
        return;
      }

			//NMEA sentence printout
				if ( (FeRAMReadByte(FERAM_GPS_MISC_CFG) >> FERAM_GPS_MISC_CFG_NMEA_PRINT_TO_SERIAL) & 0x01 ) // if the 2nd bit is set
					Serial.print(NMEA_buffer);

      // check for GPRMC sentence
      if (strncmp(NMEA_buffer, "$GNRMC", 6) == 0) // if we have a GPRMC sentence (compare the NMEA buffer with its sentence to gprmc[])
			{
        gps_parse_gprmc(); // parse the GPRMC sentence and get datetime and other values

        if (flag_gps_fix) // valid fix - indicate it by lighting up the red LED
          digitalWrite(gps_green_led_pin, LOW);
        else
          digitalWrite(gps_green_led_pin, HIGH);
      }

      // check for GPGGA sentence
      if (flag_gps_fix && strncmp(NMEA_buffer, "$GNGGA",  6) == 0) // if we have a GPRMC sentence
          gps_parse_gpgga(NMEA_buffer); // get HDOP, altitude and satellites in view

			bufferid++; // ?!? needed??

			// start the write cycle
      #if BUFFER_DEBUG_PRINT
			if ( ( (FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) >> FERAM_DEVICE_MISC_CFG1_SD_WRITE ) & 0x01) && flag_sd_write_enable) // if we are set up to write - i.e. the logfile name is set
			#else
			if ( ( (FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) >> FERAM_DEVICE_MISC_CFG1_SD_WRITE ) & 0x01) && flag_sd_write_enable && flag_gps_fix) // if we are set up to write - i.e. the logfile name is set
			#endif
      {
				// logfile name generation - should run only once a day
				if (strlen(gps_date) != 2 && strstr(gps_logfile, gps_date) == NULL ) // if "gps_date is initialized" and "gps_logfile does not contain the current datetime (e.g. on startup or on date change)"
				{
					//FIXME some weird behaviour
/*
					strncat(gps_logfile, gps_date, 4*sizeof(char));//Serial.println(gps_logfile);
					strcat(gps_logfile, "/");//Serial.println(gps_logfile);
					strncat(gps_logfile, gps_date+4*sizeof(char), 2*sizeof(char));//Serial.println(gps_logfile);

					if (!SD.exists(gps_logfile))
						SD.mkdir(gps_logfile);

					strcat(gps_logfile, "/");//Serial.println(gps_logfile);
		 */
					strcat(gps_logfile,gps_date); // constrcut the logfile
					strcat(gps_logfile, ".log");

					Serial.println(gps_logfile);
				}
				// open file in write mode - once! (not on every iteration)
				if (!gpslogfile) // run only on initialization, not on every loop iteration
						gpslogfile = SD.open(gps_logfile, FILE_WRITE);

				sd_buffer_write(NMEA_buffer, bufferid); // write NMEA data into buffer
      }

			if (strlen(gps_logfile) != 1) // check if the gps_logfile is of proper lenght (== likely to be initialized)
				flag_sd_write_enable = 1; // flag the sd card as writeable because now we have a valid datetime set (needed for logfile name)

      bufferid = 0;    //reset buffer pointer
      return;
    } // if (c == '\n')

    bufferid++;

    if (bufferid == NMEA_BUFFERSIZE-1) // if we get to the end of the buffer, right before the '\0' terminator
    {
      bufferid = 0; // we want to put the "cursor" to the beginning of the buffer array
      return;
    }

  } // if (gps.available())
} // void get_nmea_sentence()
