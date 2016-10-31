// timer setup
/*
	a good read: http://www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/

	according to the arduino schematic, the following timers are available and tied to the following pins:
		OC0: 4, 13 - 8bit
		OC1: 11, 12 - 16bit
		OC2: 9, 10 - 8bit
		OC3: 2, 3, 5 - 16bit
		OC4: 6, 7, 8 - 16bit
		OC5: 44, 45, 46 - 16bit, i dont need PWM, this is a 16bit counter which means i'll use this one
*/
	noInterrupts(); // globally disable interrupts
	TCCR5A  = 0; // clear the register (A and B)
	TCCR5B  = 0;
	OCR5A = 15624; // set compare match register to desired timer count
	TCCR5B |= (1 << WGM12); // turn on CTC mode
	TCCR5B |= (1 << CS10); // Set CS10 and CS12 bits for 1024 prescaler
	TCCR5B |= (1 << CS12);
	TIMSK5 |= (1 << OCIE5A); // enable timer compare interrupt
	interrupts(); // globally enable interrupts

	analogReference(EXTERNAL);

// ADXL345 INT1 pin connects to here, fires IRQ on act/inact
	attachInterrupt(ADXL345_INT1_interrupt_pin, ISR_ADXL345, CHANGE);
	//cli();
	//SREG |= 0b1000000;
	//PCICR |= 0b00000001;
	//PCMSK0 |= 0b01000000; // PCINT6
	//sei();


// connect to the PDI serial terminal
	// 230400 == 28.125 kB/s, as large as possible since we will be transferring files of up to 20 MB
	Serial.begin(SERIALRATE); // NOTE: the baud rate must be compatible with the SIM800L max baud rate
	Serial.println(F("PDI/PDO HW serial set"));

	Serial1.begin(9600); // set up the terminal for the SIM800L
	Serial.println(F("SIM800C HW serial set"));

// GPS device initialization start
	/* two step approach:
			1. connet via 9600 baud to set to 4800 baud
			2. configure settings

			if the GPS receiver power-cycles, it loses all its configuration settings
				and reverts to factory defautls (9600 baud)
	*/
	gps.begin(9600);   // connect to the GPS at the default rate
	Serial.println(F("GPS SW serial set")); // set gps serial comm. baud rate
	gps.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
	gps.println("$PMTK250,3,3,4800*15"); // set in/out data format to NMEA over 4600 baud
	gps.println("$PMTK251,4800*14"); // set baud rate to 4600
	gps.end();

	gps.begin(4800);
	gps.println("$PMTK185,1*23"); // disable locus logging
	gps.println("$PMTK353,1,1,1,0,0*2A"); // look for GPS, GLASNOSS and GALILEO satellites
	//gps.println("$PMTK299,1*2D"); // output debug messages
	gps.println("$PMTK301,2*2E"); // set DGPS mode to SBAS
	gps.println("$PMTK313,1*2E"); // enable SBAS
	gps.println("$PMTK386,0.5*38"); // static nav. threshold 0.5m/s
	gps.println("$PMTK257,1*2F"); // tunnel/garage high accurancy function
	gps.println("$PMTK308,3*26"); // ouput 3 fixes after entering tunnel/garage
	gps.println("$PMTK286,1*23"); // enable active interface cancellation
	gps.println("$PMTK869,1,1*35"); // enable EASY
	// $PMTK220 - what does it do?
	// $PMTK262 - fitness mode?
	// $PMTK306 - minimum satellite SNR
	// $PMTK311 - mimimum elevation mask

// GPS device initialization end

	gps.begin(GPSRATE); // from here onwards the GPS is initialized and ready to use

// set up display elements
	m2_SetU8g(OLED.getU8g(), m2_u8g_box_icon); // connect u8glib with m2tklib
	m2.setFont(0, u8g_font_6x10); // assign u8g_font_6x10 font to index 0
	m2.setPin(M2_KEY_SELECT, menu_right_button_pin); // 33
	m2.setPin(M2_KEY_PREV, menu_up_button_pin); // 32
	m2.setPin(M2_KEY_NEXT, menu_down_button_pin); // 31
	m2.setPin(M2_KEY_EXIT, menu_left_button_pin); // 30

// ADXL345 config start
	SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE3));
	delay(10);

	if (adxl345_readByte(DEVID) != B11100101)
		Serial.println(F("ADXL345 init failed"));

	adxl345_writeByte(DATA_FORMAT, DATA_FORMAT_CFG); // see adxl345.h
	adxl345_writeByte(INT_MAP, INT_MAP_CFG);
	adxl345_writeByte(INT_ENABLE, INT_ENABLE_CFG);
	adxl345_writeByte(TIME_INACT, TIME_INACT_CFG);
	adxl345_writeByte(THRESH_INACT, THRESH_INACT_CFG);
	adxl345_writeByte(THRESH_ACT, THRESH_ACT_CFG);
	adxl345_writeByte(ACT_INACT_CTL, ACT_INACT_CTL_CFG);
	adxl345_writeByte(POWER_CTL, POWER_CTL_CFG);
	adxl345_writeByte(BW_RATE, BW_RATE_CFG);

	adxl345_readByte(0x00);
	SPI.endTransaction();
// ADXL345 config end

// SD card init
	if (!SD.begin(SPI_SS_SD_card_pin)) // will fail if the card is not present
	{
		Serial.println(F("SD card initialization failed"));
	}
	else
	{
			Serial.println(F("Wiring is correct and a card is present."));
	}
	mas_Init(mas_device_sd, NULL);