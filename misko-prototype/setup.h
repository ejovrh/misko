// 1Hz timer interrupt setup
/*
	a good read: http://www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/

	according to the atmega256X/128X datasheet, the following timers are available:
		OC0:	8bit
		OC1: 16bit
		OC2:  8bit
		OC3: 16bit
		OC4: 16bit
		OC5: 16bit, i dont need PWM, this is a 16bit counter which means i'll use this one
*/
	cli(); // globally disable interrupts
	TCCR5A  = 0x0; // clear the register (A and B)
	TCCR5B  = 0x0;
	OCR5A = 15624; // set compare match register to desired timer count
	TCCR5B |= (1 << WGM12); // turn on CTC mode
	TCCR5B |= (1 << CS10); // Set CS10 and CS12 bits for 1024 prescaler
	TCCR5B |= (1 << CS12);
	TIMSK5 |= (1 << OCIE5A); // enable timer compare interrupt
	sei(); // globally enable interrupts

// INT7 interrupt setup - ADXL345 INT1 pin connects to here, fires IRQ on act/inact
	cli(); // globally disable interrupts
	EIMSK |= (1<<INT7); // enable INT7 (lives on pin PE7)
	EICRB |= (1<<ISC70); // set to register
	EICRB |= (1<<ISC71); //		a rising edge
	sei(); // globally enable interrupts

	analogReference(EXTERNAL); // set the reference voltage source (needed for all voltage measurements via the ADC)

// connect to the PDI serial terminal
	// 230400 == 28.125 kB/s, as large as possible since we will be transferring files of up to 20 MB
	Serial.begin(SERIALRATE); // NOTE: the baud rate must be compatible with the SIM800L max baud rate
	Serial.println(F("PDI/PDO HW serial set"));

	Serial1.begin(9600); // set up the terminal for the SIM800L
	Serial.println(F("SIM800C HW serial set"));

// Bluetooth device initialization start
	digitalWrite(Bluetooth_wakeup_pin, LOW); // turn the device on
	digitalWrite(analog_sw_ctrl_pin, LOW); // set into Bluetooth programming mode (state B1)
	delay(10);

	gps.begin(115200); // Bluetooth default baud rate
	gps.write("$$$\r\n"); // enter command mode
	gps.write("su,4800\r\n"); // set 4800 baud
	gps.write("sp,0000\r\n"); // set 0000 as pin code
	gps.write("s-,Mi�ko\r\n"); // give the device its name
	gps.write("sn,Mi�ko\r\n"); // give the device its name
	gps.write("S|,0101"); // on/off duty cycle in seconds when not connected (1s off, 1s on)
	gps.write("S%,1000"); // set the GPIO's to inputs, thereby disabling them all
	gps.write("SY,0000"); // set TX power to 0dBM
	gps.write("sa,2\r\n"); // set authentication to "SPP just works"
	gps.write("sw,0800\r\n"); // 500ms for sniff mode (needed???)
	gps.write("r,1\r\n"); // reboot it
	gps.end();

	digitalWrite(analog_sw_ctrl_pin, HIGH); // set back to normal operation mode (state B2)
	digitalWrite(Bluetooth_wakeup_pin, HIGH); // turn the device off
// Bluetooth device initialization end

// GPS device initialization start
	/* two step approach:
			1. connet via 9600 baud to set to 4800 baud
			2. once at 4800, configure other settings

			if the GPS receiver power-cycles, it loses all its configuration settings
				and reverts to factory defautls (9600 baud)
				a supercap remedies this somewhat
	*/

	digitalWrite(GPS_power_ctl_pin, LOW); // power off
	digitalWrite(GPS_power_ctl_pin, HIGH); // power on
	gps.begin(9600);   // connect to the GPS at the default rate
	gps.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*34");
	gps.println("$PMTK250,3,3,4800*15"); // set in/out data format to NMEA over 4600 baud
	gps.println("$PMTK251,4800*14"); // set baud rate to 4600
	gps.end();

	gps.begin(GPSRATE);
	gps.println("$PMTK185,1*23"); // disable locus logging
	//gps.println("$PMTK220,1000*1F"); // 1Hz update rate per fix
	gps.println("$PMTK225,0*2B"); // no power saving, i.e. normal mode
	gps.println("$PMTK255,1*2D"); // set sync PPS and NMEA
	gps.println("$PMTK257,1*2F"); // tunnel/garage high accurancy function
	gps.println("$PMTK262,3*2B"); // enable GLP (low power mode) mode
	gps.println("$PMTK285,4,50*0C"); // set PPS
	gps.println("$PMTK286,1*23"); // enable active interface cancellation
	gps.println("$PMTK301,2*2E"); // set DGPS mode to SBAS
	gps.println("$PMTK308,3*26"); // ouput 3 fixes after entering tunnel/garage
	gps.println("$PMTK313,1*2E"); // enable SBAS
	gps.println("$PMTK353,1,1,1,0,0*2A"); // look for GPS, GLASNOSS and GALILEO satellites
	gps.println("$PMTK356,0*2E"); // disable HDOP theshold
	gps.println("$PMTK386,0.5*38"); // static nav. threshold 0.5m/s
	gps.println("$PMTK869,1,1*35"); // enable EASY
	gps.println("$PMTK886,1*29"); // enable fitness mode (good for speeds up to 5m/s (== 9.72 knots), for faster speeds normal mode is better)

	// TODO - works, but needs further improvement
	gps_adjust_log_freq(); // operates directly off values stored in FeRAM

	if (flag_gps_on) // see what settings we have
		digitalWrite(GPS_power_ctl_pin, HIGH); // ...and keep on
	else
		digitalWrite(GPS_power_ctl_pin, LOW); // ... or turn off

	// TODO: adjust dynamically for best SNR
	// $PMTK306 - minimum satellite SNR
	// $PMTK311 - mimimum elevation mask

//		from here onwards the GPS is initialized and ready to use
// GPS device initialization end

// set up display elements
	m2_SetU8g(OLED.getU8g(), m2_u8g_box_icon); // connect u8glib with m2tklib
	m2.setFont(0, u8g_font_6x10); // assign u8g_font_6x10 font to index 0
	m2.setPin(M2_KEY_SELECT, menu_right_button_pin); // 37
	m2.setPin(M2_KEY_PREV, menu_up_button_pin); // 35
	m2.setPin(M2_KEY_NEXT, menu_down_button_pin); // 34
	m2.setPin(M2_KEY_EXIT, menu_left_button_pin); // 32

// ADXL345 config start - some settings are read from FeRAM
	SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE3));
	delay(10);

	if (adxl345_readByte(DEVID) != B11100101)
		Serial.println(F("DEVID mismatch - ADXL345 init failed"));

	adxl345_writeByte(DATA_FORMAT, DATA_FORMAT_CFG); // see adxl345.h
	adxl345_writeByte(INT_MAP, INT_MAP_CFG); // see adxl345.h

	if ( ( FeRAMReadByte(FERAM_DEVICE_MISC_CFG1) >> FERAM_DEVICE_MISC_CFG1_ADXL345_AUTO_POWER ) & 0x01 ) // see if the enable bit is set
		adxl345_writeByte(INT_ENABLE, INT_ENABLE_CFG); // enable interrupts on act/inact (i.e. enable the ADXL)
	else
		adxl345_writeByte(INT_ENABLE, INT_DISABLE_CFG); // disable all interrupts (i.e. disable the ADXL)

	adxl345_writeByte(TIME_INACT, FeRAMReadByte(FERAM_ADXL345_MOVEMENT_TIMEOUT)); // fetch settings from FeRAM
	adxl345_writeByte(THRESH_INACT, FeRAMReadByte(FERAM_ADXL345_INACTIVITY_THRESHOLD)); // fetch settings from FeRAM
	adxl345_writeByte(THRESH_ACT, FeRAMReadByte(FERAM_ADXL345_ACTIVITY_THRESHOLD)); // fetch settings from FeRAM
	adxl345_writeByte(ACT_INACT_CTL, ACT_INACT_CTL_CFG); // see adxl345.h
	adxl345_writeByte(POWER_CTL, POWER_CTL_CFG); // see adxl345.h
	adxl345_writeByte(BW_RATE, BW_RATE_CFG); // see adxl345.h, gets changed on act/inact
	SPI.endTransaction();
// ADXL345 config end

// SD card init
	if (digitalRead(SD_card_detect_pin) == LOW) // fist, see if a card is inserted
	{
		Serial.println("card inserted");
		// SD card init
		if (!SD.begin(SPI_SS_SD_card_pin)) // will fail if the card is not present
			Serial.println(F("SD card initialization failed"));
		else
			Serial.println(F("Wiring is correct and a card is present."));
	}
	else
		Serial.println("card not inserted");

	mas_Init(mas_device_sd, NULL);