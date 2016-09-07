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
	noInterrupts(); // gobally disable interrupts
	TCCR5A  = 0; // clear the register (A and B)
	TCCR5B  = 0;
	OCR5A = 15624; // set compare match register to desired timer count
	TCCR5B |= (1 << WGM12); // turn on CTC mode
	TCCR5B |= (1 << CS10); // Set CS10 and CS12 bits for 1024 prescaler
	TCCR5B |= (1 << CS12);
	TIMSK5 |= (1 << OCIE5A); // enable timer compare interrupt
	interrupts(); // globally enable interrupts

// AREF_VOLTAGE - 4.30 via zener diode
	analogReference(EXTERNAL); 

// ADXL345 INT1 pin connects to here, fires IRQ on act/inact
	attachInterrupt(digitalPinToInterrupt(interrupt_adxl345_int1_pin), isr_flag_adxl345_int1, CHANGE); 

// connect to the PDI serial terminal
	// 230400 == 28.125 kB/s, as large as possible since we will be trasnferring files of up to 20 MB
	Serial.begin(230400); // NOTE: the baud rate must be compatible with the SIM800L max baud rate
	Serial.println(F("serial set"));
	
	Serial1.begin(9600); // set up the terminal for the SIM800L
	Serial.println(F("sim800l SW set"));

// initialize GPS
	#ifdef GPS_MTK3339_CHIP // pre-init for the MTK3339 - it has 9600 as default
	Serial1.begin(9600);	// connect with 9600
	Serial1.write("$PMTK251,4800*14"); // set to 4800
	Serial1.end(); // terminate and continue below regularly...
	#endif

	gps.begin(GPSRATE);   // connect to the GPS at the desired rate
	Serial.println(F("gps serial set")); // set gps serial comm. baud rate

	#ifdef GPS_EM406A_CHIP
	if (EEPROM[EEPROM_GPS_USE_WAAS_INDEX] == 1)
		Serial1.write("$PSRF151,01*0F\r\n"); // turn on WAAS
	else
		Serial1.write("$PSRF151,00*0E\r\n"); // turn off WAAS

	Serial1.write("$PSRF105,01*3E\r\n"); // gps debug messages on
	//Serial1.write("$PSRF105,00*3F\r\n"); // gps debug messages off
	Serial1.write("$PSRF109,137*36\r\n"); // use SBAS
	Serial1.write("$PSRF109,120*30\r\n"); // SBAS Channel PRN120 #33(EGNOS) Inmarsat 3-F2
	Serial1.write("$PSRF109,122*32\r\n"); // SBAS Channel PRN122 #35(WAAS)
	Serial1.write("$PSRF109,124*34\r\n"); // SBAS Channel PRN124 #37(EGNOS)
	Serial1.write("$PSRF109,126*36\r\n"); // SBAS Channel PRN126 #39(EGNOS)
	Serial1.write("$PSRF109,129*39\r\n"); // SBAS Channel PRN129 #42(MTSAT-1)
	Serial1.write("$PSRF109,131*30\r\n"); // SBAS Channel PRN131 #44(EGNOS)
	Serial1.write("$PSRF109,134*35\r\n"); // SBAS Channel PRN134 #47(WAAS)
	Serial1.write("$PSRF109,136*37\r\n"); // SBAS - Astra 4B
	Serial1.write("$PSRF109,137*36\r\n"); // SBAS Channel PRN137 #50(MTSAT-2)
	#endif
	
	#ifdef GPS_MTK3339_CHIP
	gps.write("$PMTK330,0*2E"); // set WGS84 as the datum
	gps.write("$PMTK301,1*2D"); // set DGPS mode to WAAS
	gps.write("$PMTK313,1*2E"); // enable SBAS satellite search
	gps.write("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");  // report only GPRMC and GPGGA sentences
	gps.write("$PMTK286,1*23"); // enable interference cancellation
	gps.write("$PMTK869,1,1*35"); // enable EASY
	gps.write("$PGCMD,33,0*6D"); // disable antenna messages
	#endif

	gps_adjust_log_freq(00, EEPROM[EEPROM_GPS_GPRMC_GGA_FREQ_INDEX]); // GPGGA
	// gps_adjust_log_freq(01, 0); // GPGLL
	// gps_adjust_log_freq(02, 0); // GPGSA
	// gps_adjust_log_freq(03, 0); // GPGSV
	gps_adjust_log_freq(04, EEPROM[EEPROM_GPS_GPRMC_GGA_FREQ_INDEX]); // GPRMC
	// gps_adjust_log_freq(05, 0); // GPVTG
	delay(50);
	
// set up display elements
	m2_SetU8g(OLED.getU8g(), m2_u8g_box_icon); // connect u8glib with m2tklib
	m2.setFont(0, u8g_font_6x10); // assign u8g_font_6x10 font to index 0
	m2.setPin(M2_KEY_SELECT, menu_right_buttton); // 33
	m2.setPin(M2_KEY_PREV, menu_up_buttton); // 32
	m2.setPin(M2_KEY_NEXT, menu_down_buttton); // 31
	m2.setPin(M2_KEY_EXIT, menu_left_buttton); // 30

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