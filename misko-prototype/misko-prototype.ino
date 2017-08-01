// libraries
#include <SoftwareSerial.h> // software serial library for GPS/Bluetooth
#include <SD.h> // SD card library
#include <SPI.h> // SPI library
#include <U8glib.h> // OLED chip driver - https://github.com/olikraus/u8glib/wiki/userreference
#include <M2tk.h> // user menu library - https://code.google.com/archive/p/m2tklib/
#include "utility/m2ghu8g.h" // U8glib graphics handler (one of many)
#include "utility/mas.h" // mass storage handler for the SD card
#include <gprs.h> // GPRS library for SIM800C
#include <avr/sleep.h> // AVR sleep management
#include <avr/power.h> // AVR power saving

// local files
#include "adxl345.h" // definition of ADXL345 register addresses
#include "fram.h" // definition of FRAM register addresses & addressing scheme for config settins
#include "pin_definitions.h" // pin layout
#include "global_variables.h" // global variables et al.
#include "functions.h" // useful functions
#include "display.h" // display (menu) code

void setup()
{
	#include "pin_modes.h" // definiton of pin modes
  #include "setup.h" // the actual setup [ without () ]
}

#include "gps_functions.h" // gps functions - kludge alert!

void loop()
{
  #include "loop.h" // the actual loop [ without () ]
}

// ISR names
//	http://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html

//ISR for the ADXL345 accelerometer
ISR(INT7_vect)
{
	cli();
	adxl345_irq_src = adxl345_readByte(INT_SOURCE);

	// inactivity
	if( (adxl345_irq_src >> 3) & 0x01)  // if the inact bit is set
	{
		Serial.println("Inactivity");
		adxl345_writeByte(BW_RATE, 0x17 ); // 0001 0111 (0x1A) - set to low power mode, bit 5 (was 0x0A, becomes 0x1A)
	}

	// activity
	if( (adxl345_irq_src >> 4) & 0x01) // if the act bit is set
	{
		Serial.println("Activity");

		// set the device back in measurement mode
		// as suggested on the datasheet, we put it in standby then in measurement mode
		//adxl345_writeByte(POWER_CTL_CFG, powerCTL & 11110011);
		//adxl345_writeByte(POWER_CTL, 0x04); // first standby
		//adxl345_writeByte(POWER_CTL_CFG, powerCTL & 11111011);
		//adxl345_writeByte(POWER_CTL, POWER_CTL_CFG); // then full measurement mode

		adxl345_writeByte(BW_RATE, 0x07 ); // 0000 0111 (0x0A) get back to full accuracy measurement (we will consume more power)
	}
	sei();
}

// ISR for timer5 events - generated every second
//	stuff that needs to run periodically goes here
ISR(TIMER5_COMPA_vect)
{
	uptime++; // increment every second

	flag_timer5_handler_execute = 1;

// construct which runs only once at power-up
	if(!flag_run_once)
	{
		flag_run_once = 1;

		//SPI voodoo - for some reason it is necessary for the ADXL345 to "work"
		SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE3));
		adxl345_readByte(DEVID); // reads INT SRC to clear the act/inact bits
		adxl345_readByte(INT_SOURCE); // reads INT SRC to clear the act/inact bits
		adxl345_writeByte(BW_RATE, 0x07 );
		SPI.endTransaction();
	}
}

// catchall interrupt handler
ISR(BADISR_vect)
{
	Serial.println("!!! BADISR_vect !!!");
}