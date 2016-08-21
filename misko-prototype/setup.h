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


analogReference(EXTERNAL); // AREF_VOLTAGE - 4.30 via zener diode

attachInterrupt(digitalPinToInterrupt(interrupt_adxl345_int1_pin), isr_adxl345_int1, CHANGE); // ADXL345 INT1 pin connects to here, fires IRQ on act/inact

Serial.begin(SERIALRATE); // connect to the serial terminal
Serial.println(F("serial set"));

Serial2.begin(SERIALRATE); // set up the terminal for the SIM800L
delay(10);
Serial.write("AT"); // 1st AT
Serial.write("ATE0"); // turn off command echo

// initialize GPS
Serial3.begin(GPSRATE);   // connect to the GPS at the desired rate
Serial3.print(SERIAL_SET); // set gps serial comm. parameter
Serial3.print(WAAS_ON); // will become obsolete via EEPROM and setup
Serial3.print(RMC_ON); // will become obsolete via EEPROM and setup
Serial3.print(GSA_ON); // will become obsolete via EEPROM and setup
//Serial3.print(GSV_ON); // will become obsolete via EEPROM and setup

delay(50);
m2_SetU8g(OLED.getU8g(), m2_u8g_box_icon); // connect u8glib with m2tklib
m2.setFont(0, u8g_font_6x10); // assign u8g_font_6x10 font to index 0
m2.setPin(M2_KEY_SELECT, menu_right_buttton); // 33
m2.setPin(M2_KEY_PREV, menu_up_buttton); // 32
m2.setPin(M2_KEY_NEXT, menu_down_buttton); // 31
m2.setPin(M2_KEY_EXIT, menu_left_buttton); // 30

// we'll use the initialization code from the utility libraries
// since we're just testing if the card is working!
if (!card.init(SPI_HALF_SPEED, SPI_SS_SD_card_pin)) 
{
  Serial.println(F("SD card initialization failed"));
}
else 
{
    Serial.println(F("Wiring is correct and a card is present."));
}


// ADXL345 config start
SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE3));
delay(10);

adxl345_readByte(DEVID); // weird issue: the 1st read returns 0, subsequent reads return proper values

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

	adxl345_readByte(0x30);
	adxl345_readByte(0x2F);
	adxl345_readByte(0x2E);
	
SPI.endTransaction();
// ADXL345 config end
