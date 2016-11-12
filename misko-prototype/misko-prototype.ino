// libraries
#include <SoftwareSerial.h> // software serial library for GPS/Bluetooth
#include <SD.h> // SD card library
#include <SPI.h> // SPI library
#include <EEPROM.h> // EEPROM library for configuration data
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
	#include "eeprom_config.h" // definition of EEPROM fields
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
	adxl345_irq_src = adxl345_readByte(INT_SOURCE);
}

// ISR for timer5 events - generated every second
//	stuff that needs to run periodically goes here
ISR(TIMER5_COMPA_vect)
{
	scheduler_run_count++;

	val_vcc = 2 * calculate_voltage(Vcc_sense_pin); // measures Vcc across a voltage divider
	val_temp = calculate_temperature(); // reads out temperature-dependant voltage
		/* the percentage calculation
		union battery datasheet: charge cutoff voltage: Vbat 4.20V, discharge cutoff voltage: Vbat 2.70V
			over the voltage divider this gives 2.10V and 1.3V
			our voltage divider gives 0.5 Vbat

	 percentage calculation see https://racelogic.support/02VBOX_Motorsport/Video_Data_Loggers/Video_VBOX_Range/Video_VBOX_-_User_manual/24_-_Calculating_Scale_and_Offset

		dX is 2.1 - 1.35 = 0.75
		dY is 100 - 0 = 100
	 the gradient is dX/dY = 133

	 Y = percent = 0, X = Voltage = 1.35V
	 	0 = ((dX/dY)* voltage) + c
		0 = (133 * 1.35) + c <=> 0 = 180 + c <=> c = -180
		our equation is: y = 133 * x - 180

	elementary, dr. watson!
 	*/
	val_batA_pct = (133 * calculate_voltage(bat_A_pin)) - 180; // calculus...
	val_batB_pct = 000;

	if(!flag_run_once)
	{
		flag_run_once = 1;

		//SPI voodoo - for some reason it is necessary for the ADXL345 to "work"
		SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE3));
		adxl345_readByte(INT_SOURCE); // reads INT SRC to clear the act/inact bits
		SPI.endTransaction();

	}
}