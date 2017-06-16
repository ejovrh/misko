// takes an incoming NMEA sentence and acts upon it (parse for RMC and GGA, decides to log to file, etc.)
void get_nmea_sentences()
{
	// reads one char at a time from the gps device;
	//  due to iteration from the main loop, eventually the whole NMEA sentence accumulates in the global buffer variable

	uint8_t sum; // variable for the NMEA checksum of each sentence

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

		if (flag_gps_fix) // valid fix
			digitalWrite(gps_green_led_pin, LOW); // indicate it by lighting up the red LED (inverse logic)
		else
			digitalWrite(gps_green_led_pin, HIGH);

		// check for GPRMC sentence
		if ( strncmp(NMEA_buffer, "$GNRMC", 6) == 0 ) // if we have a GPRMC sentence (compare the NMEA buffer with its sentence to gprmc[])
		{
			// TODO: optimize
			if (flag_gps_fix && (abs(uptime - gps_gnrmc_age) > 60 ? 1 : 0) ) // if we have a fix and the age of fix is more than 60s
			{
				gps_gnrmc_age = uptime;
				FeRAMWriteStr(FERAM_GPS_LAST_GOOD_GNRMC, NMEA_buffer, strlen(NMEA_buffer)); // write last good position; 83 because its not worth to calculate the lenght when we know that its max. 83 characters long
			}

			gps_parse_gprmc(NMEA_buffer); // parse the GPRMC sentence and get datetime and other values
		}

		// check for GPGGA sentence
		if ( flag_gps_fix && strncmp(NMEA_buffer, "$GNGGA",  6) == 0 ) // if we have a GPRMC sentence
		{
			// TODO: optimize
			if (flag_gps_fix && (abs(uptime - gps_gngga_age) > 60 ? 1 : 0) ) // if we have a fix and the age of fix is more than 60s
			{
				gps_gngga_age = uptime;
				FeRAMWriteStr(FERAM_GPS_LAST_GOOD_GNGGA, NMEA_buffer, strlen(NMEA_buffer)); // write last good position; 83 because its not worth to calculate the lenght when we know that its max. 83 characters long
			}

			gps_parse_gpgga(NMEA_buffer); // get HDOP, altitude and satellites in view
		}

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
				/* FIXME some weird behaviour
				 * the active FS object is not visible until after a
				 * furthermore, once selected, the FS object is not visible again until after a reset
				 */

				/* log format:
				 *	e.g. /2017/06/20170616.log
				 */

				strncat(gps_logfile, gps_date, 4*sizeof(char)); // construct the year "2017"
				strcat(gps_logfile, "/"); // directory seperator "2017/"
				strncat(gps_logfile, gps_date+4*sizeof(char), 2*sizeof(char)); // construct the month "2017/06"

				if (!SD.exists(gps_logfile)) // if e.g. /2017/06/ doesnt exist...
					SD.mkdir(gps_logfile); // create it

				strcat(gps_logfile, "/"); // add another directory seperator "2017/06/"

				strcat(gps_logfile,gps_date); // constrcut the actual file "2017/06/201706"
				strcat(gps_logfile, ".log"); // "2017/06/201706.log"

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
} // void get_nmea_sentence()
