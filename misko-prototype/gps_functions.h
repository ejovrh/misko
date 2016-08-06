//#include "gps_config.h"

int16_t parseHex(char g) // NMEA checksum calculator
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

void get_gps_datetime() { 
      /*
      $GPRMC,170942.000,A,4547.9094,N,01555.1254,E,0.13,142.38,050816,,,A*63
      $GPGGA,170943.000,4547.9091,N,01555.1251,E,1,06,1.7,139.9,M,42.5,M,,0000*55
      $GPGSA,A,3,08,11,22,03,27,32,,,,,,,3.2,1.7,2.7*3F
      $GPGSV,3,1,11,11,74,284,34,22,53,248,34,08,48,190,39,32,44,068,24*71
      $GPGSV,3,2,11,03,28,245,32,27,19,165,29,10,18,060,,01,58,304,28*78
      $GPGSV,3,3,11,14,51,100,,28,17,299,,17,02,324,*46
      $GPRMC,170943.000,A,4547.9091,N,01555.1251,E,0.19,110.80,050816,,,A*6C
      $GPGGA,170944.000,4547.9089,N,01555.1250,E,1,07,1.3,140.5,M,42.5,M,,0000*5D
      $GPGSA,A,3,08,11,22,03,27,32,10,,,,,,2.3,1.3,1.9*37
      $GPRMC,170944.000,A,4547.9089,N,01555.1250,E,0.22,182.76,050816,,,A*69
      $GPGGA,170945.000,4547.9087,N,01555.1248,E,1,07,1.3,141.2,M,42.5,M,,0000*5D
      $GPGSA,A,3,08,11,22,03,27,32,10,,,,,,2.3,1.3,1.9*37
      $GPRMC,170945.000,A,4547.9087,N,01555.1248,E,0.38,191.61,050816,,,A*60
      */

  char *p;  // pointer for parsing
  int i = 0;

  // hhmmss time data
  p = NMEA_buffer+7;  // set pointer to proper position: 154951.285,A,4547.8814,N,01555.2455,E,0.92,115.67,020814,,,A*69

  memcpy(gps_time, p, 6* sizeof(char)); // fill gps_time by copying from one array into another

  p = strchr(p, ',')+3; // 4547.8814,N,01555.2455,E,0.92,115.67,020814,,,A*69
  
  if (gps_fix)
    memcpy(gps_latitude+(4*sizeof(char)), p, 9 * sizeof(char)); // fill up gps_latitude[] , part 1
  
  p = strchr(p, ',')+1; // N,01555.2455,E,0.92,115.67,020814,,,A*69
  
  if (gps_fix)
    memcpy(gps_latitude+(13*sizeof(char)), p, sizeof(char));
  
  p = strchr(p, ',')+1; // 01555.2455,E,0.92,115.67,020814,,,A*69
  
  if (gps_fix)
    memcpy(gps_longtitude+(4*sizeof(char)), p, 10 * sizeof(char)); // fill up gps_longtitude[] , part 1
    
  p = strchr(p, ',')+1; // E,0.92,115.67,020814,,,A*69
  
  if (gps_fix)
    memcpy(gps_longtitude+(14*sizeof(char)), p, sizeof(char)); // fill up gps_longtitude[] , part 2, appends letter
  
  p = strchr(p, ',')+1; // 0.92,115.67,020814,,,A*69
  p = strchr(p, ',')+1; // 115.67,020814,,,A*69
  p = strchr(p, ',')+1; // 020814,,,A*69
  
  // fill gps_date 
      *(gps_date+2) = *(p+4);  // Y - 2
      *(gps_date+3) = *(p+5);  // Y - 2
      *(gps_date+4) = *(p+2);  // M - 2
      *(gps_date+5) = *(p+3);  // M - 2
      *(gps_date+6) = *(p+0);  // D - 6
      *(gps_date+7) = *(p+1);  // D - 6
    //Serial.print("gps_date: ");  Serial.println(gps_date);
    //Serial.print("gps_time: ");  Serial.println(gps_time);
}

void get_gps_hdop()
{
  ;
}

void get_nmea_sentences() {
// reads one char at a time from the gps device; 
//  due to iteration from the main loop, eventually the whole NMEA sentence accumulates in the global buffer variable

  unsigned int sum; // variable for the NMEA checksum of each sentence
  bool gotGPRMC = false;    // flag that indicates GPRMC or GPGGA strings
  unsigned int i;  // iterator for various loops
  char *p;
 
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

      for (i=1; i < (bufferid-4); i++) 
        sum ^= *(NMEA_buffer+i); // check checksum
      
      if (sum != 0) // checksum bad
      { 
        bufferid = 0; // set pointer back to the beginning
        return;
      }

      //debug print
      //Serial.println("debug print of buffer:");      
      Serial.print(NMEA_buffer);

      // check for GPRMC sentence
      if (memcmp(NMEA_buffer, gprmc, 6*sizeof(char)) == 0) // if we have a GPRMC sentence (compare the NMEA buffer with its sentence to gprmc[])
      { 
        get_gps_datetime();

        p = strchr(strchr(NMEA_buffer, ',')+1, ',')+1; // skip to position after 2nd comma
        
        if (*p == 'A') // valid fix - indicate it by lighting up the reed LED
        { 
          gps_fix = 1;
          digitalWrite(gps_green_led_pin, HIGH);
        } 
        else 
        {
          gps_fix = 0;
          digitalWrite(gps_green_led_pin, LOW);
        }
      }

      // check for GPGGA sentence
      if (memcmp(NMEA_buffer, gpgga, 6*sizeof(char)) == 0) // if we have a GPRMC sentence (compare the NMEA buffer with its sentence to gprmc[])
      { 
        get_gps_hdop();
          
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
