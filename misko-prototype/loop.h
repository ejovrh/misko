 //Serial.println(millis());
 m2.checkKey();

 if (m2.handleKey() != 0)
 {
	 // picture loop - https://github.com/olikraus/u8glib/wiki/tpictureloop
     OLED.firstPage();  // https://github.com/olikraus/u8glib/wiki/userreference#firstpage
     do 
     {
				m2.draw(); // defined in functions.h
     }
     while( OLED.nextPage() ); // https://github.com/olikraus/u8glib/wiki/userreference#nextpage
 } 
 //Serial.println(millis());
    
 handle_bluetooth_button(); // handles the bluetooth power button
  
 //Serial.println(millis());
 get_nmea_sentences(); // gets NMEA sentences out of the GPS and deals with them
 //Serial.println(millis());
  
calculate_temperature(); // calculates the temperature via a TMP36 sensor

if (adxl345_int1) // if the ADXL345 INT1 flag is set
	handle_adx_intl(); // execute the function
	
//Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
if(Serial2.available())
	Serial.write(Serial2.read());

//Read Arduino IDE Serial Monitor inputs (if available) and send them to SIM800
if(Serial.available())     
    Serial2.write(Serial.read()); 