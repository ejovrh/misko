//#include "gps_config.h"

void gps_parse_gprmc() // determines fix or not, parses coordinates, datetime
{ 
  // sample NMEA GPRMC sentence
  //    $GPRMC,170942.000,A,4547.9094,N,01555.1254,E,0.13,142.38,050816,,,A*63
  //    $GPRMC,221939.869,V,,,,,,,060816,,,N*41

  // real programmers would probably do this in a more elegant way..
  // strtok would be a much cooler way but i done want to have a loop within a loop (nema parser) (within a loop (loop()) )
  //  p = strchr(p, ',')+1, on the other hand, lets me seek the next comma in a non destructive way
  char *p;  // pointer for parsing

  // field 2 - hhmmss time data: 170942.000,A,4547.9094,N,01555.1254,E,0.13,142.38,050816,,,A*63
  p = NMEA_buffer+7;  // set pointer to proper position: 154951.285,A,4547.8814,N,01555.2455,E,0.92,115.67,020814,,,A*69
  memcpy(gps_time, p, 6* sizeof(char)); // fill char gps_time[7] = "XXXXXX" by copying from one array into another

  // field 3 - fix indicator: A,4547.9094,N,01555.1254,E,0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1; // finds position of next comma and puts the cursor one position further
    if (*p == 'A') // good fix
      gps_fix = 1; // flag good fix
            
    if (*p == 'V') // invaid fix
      gps_fix = 0; // flag bad fix
     
  // field 4 - latitude: 4547.9094,N,01555.1254,E,0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1; 
  if (gps_fix)
     memcpy(gps_latitude+(4*sizeof(char)), p, 9 * sizeof(char)); // fill up gps_latitude[] , part 1

  // field 5 - latitude indicator: N,01555.1254,E,0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1;
  if (gps_fix)
    memcpy(gps_latitude+(13*sizeof(char)), p, sizeof(char)); // fill up gps_latitude[] , part 2

  // field 6 - longtitude: 01555.1254,E,0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1;
  if (gps_fix)
    memcpy(gps_longtitude+(4*sizeof(char)), p, 10 * sizeof(char)); // fill up gps_longtitude[] , part 1

  // field 7 - longtitude indicator: E,0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1;
  if (gps_fix)
    memcpy(gps_longtitude+(14*sizeof(char)), p, sizeof(char)); // fill up gps_longtitude[] , part 2, appends letter

  // [not needed] field 8 - speed over ground: 0.13,142.38,050816,,,A*63
  p = strchr(p, ',')+1;
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

void gps_parse_gpgga() // parses out sattelites used and HDOP 
{
	if (!gps_fix)
		return;
	
  char *p; // char pointer for string tokenizer
	// $GPGGA,185447.258,4547.8986,N,01555.1525,E,1,04,4.3,126.8,M,42.5,M,,0000*5F

  p = NMEA_buffer+7; // set the pointer to position 185447.258,4547.8986,N,01555.1525,E,1,04,4.3,126.8,M,42.5,M,,0000*5F
	p = strchr(p, ',')+1; // 4547.8986,N,01555.1525,E,1,04,4.3,126.8,M,42.5,M,,0000*5F
	p = strchr(p, ',')+1; // N,01555.1525,E,1,04,4.3,126.8,M,42.5,M,,0000*5F
	p = strchr(p, ',')+1; // 01555.1525,E,1,04,4.3,126.8,M,42.5,M,,0000*5F
	p = strchr(p, ',')+1; // E,1,04,4.3,126.8,M,42.5,M,,0000*5F
	p = strchr(p, ',')+1; // 1,04,4.3,126.8,M,42.5,M,,0000*5F
	
	// satellites in view
	p = strchr(p, ',')+1; // 04,4.3,126.8,M,42.5,M,,0000*5F
	memcpy( gps_satellites_in_view + 3 * sizeof(char), p, 2 * sizeof(char) ); // copy the value 
  *(gps_satellites_in_view+5) = '\0'; // terminate the string
	
	// HDOP
	p = strchr(p, ',')+1; // 4.3,126.8,M,42.5,M,,0000*5F
  memcpy( gps_hdop + 3 * sizeof(char), p, strcspn (p, ",") * sizeof(char) ); // copy the value 
  *(gps_hdop+7) = '\0'; // terminate the string
	
	// altitude
	p = strchr(p, ',')+1; // 126.8,M,42.5,M,,0000*5F
  memcpy( gps_altitude + ( 4*sizeof(char) ), p, strcspn (p, ".") * sizeof(char) ); // copy the numbers...
  *(p+7) = 'm'; // at this position, put the meter symbol
  *(p+8) = '\0'; // at the next position, terminate the string
}

void get_nmea_sentences() {
// reads one char at a time from the gps device; 
//  due to iteration from the main loop, eventually the whole NMEA sentence accumulates in the global buffer variable

  uint8_t sum; // variable for the NMEA checksum of each sentence
  bool gotGPRMC = false;    // flag that indicates GPRMC or GPGGA strings
 
  if (Serial3.available()) // if serial3 is availiable
  {  
    *(NMEA_buffer+bufferid) = Serial3.read();      // put that char into the array "buffer"

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

      //debug print
      //Serial.println("debug print of buffer:");      
      #if NMEA_DEBUG_PRINT
			Serial.print(NMEA_buffer);
			#endif
      // check for GPRMC sentence
      if (memcmp(NMEA_buffer, gprmc, 6*sizeof(char)) == 0) // if we have a GPRMC sentence (compare the NMEA buffer with its sentence to gprmc[])
      { 
        gps_parse_gprmc();

        if (gps_fix) // valid fix - indicate it by lighting up the reed LED
          digitalWrite(gps_green_led_pin, HIGH);
        else 
          digitalWrite(gps_green_led_pin, LOW);
      }

      // check for GPGGA sentence
      if (memcmp(NMEA_buffer, gpgga, 6*sizeof(char)) == 0) // if we have a GPRMC sentence
      { 
        if (gps_fix) // if we dont have a fix we get garbage (leads to too compliated code)
          gps_parse_gpgga(); // get HDOP and satellites used
      }
      
        strcat(strcpy(gps_logfile,gps_date), ".gps"); 

        bufferid++; // ?!? needed??

        digitalWrite(gps_red_led_pin, HIGH);      // Turn on red LED, indicates begin of write to SD
        File gpslogfile = SD.open(gps_logfile, FILE_WRITE);
              
        if (! strstr(gps_logfile, "XXX") )
        {
          //Serial.print("debug write into outfile: "); Serial.println(gps_logfile);

            gpslogfile.write(NMEA_buffer);
            gpslogfile.close();
        } 

        digitalWrite(gps_red_led_pin, LOW);    //turn off red LED, indicates write to SD is finished
        bufferid = 0;    //reset buffer pointer
        return;
    } // if (c == '\n')
    
    bufferid++;

    if (bufferid == NMEA_BUFFERSIZE-1) // if we get to the end of the buffer, right before the '\0' terminator
    { 
      bufferid = 0; // we want to put the "cursor" to the beginning of the buffer array
      return;
    }
    
  } // if (gSerial3.available())
} // void get_nmea_sentence()
