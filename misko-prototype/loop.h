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
  

