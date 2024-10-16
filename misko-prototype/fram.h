/*
 * fram.h
 *	opcode commands for FeRAM operation
 *		refer to the FeRAM datasheet
 *
 *	cell addressing #defines for config settings
 *
 * Created: 25.9.2016. 22:37:51
 *  Author: hivanic
 */

 // Set Write Enable Latch; disabled on power on (i.e. the module is read only)
#define CMD_WREN	0x06		//0000 0110

// Clear Write Enable Latch; puts module in read-only mode
#define CMD_WRDI	0x04		//0000 0100

// Read Status Register; reads write protection features, returns one uint8_t
#define CMD_RDSR	0x05		//0000 0101

// Write Status Register; sets write protection features (see datasheet p. 7)
#define CMD_WRSR	0x01		//0000 0001

// Read Memory Data; reads one byte from a 2 byte address (p. 8);
#define CMD_READ	0x03		//0000 0011

// Write Memory Data; writes one byte to a 2 byte address (p. 8)
#define CMD_WRITE 0x02		//0000 0010

// the addressing scheme & bit/byte assignment is described below

/*
 *	FERAM_CONFIG_VERSION
 *		fully byte, unsigned integer
 *
 *		represents the current config version
 *			to be changed if the code stucture changes
 */
 #define FERAM_CONFIG_VERSION												0x00	// 1 byte, unsigned integer

/*
 *	FERAM_DEVICE_TIMEZONE
 *		full byte, signed integer from -12 to +12 in 1 incrememnts
 *
 *		timezone value
 */
 #define FERAM_DEVICE_TIMEZONE											0x01	// 1 byte (-12 ... +12) -- hours

/*
 *	FERAM_OLED_AUTO_TIMEOUT
 *		full byte, unsigned integer from 1 to 255
 *
 *		OLED device auto power-off timeout in seconds
 */
#define FERAM_OLED_AUTO_TIMEOUT											0x02 // 1 byte (1 ... 255) -- seconds

/*
 *	FERAM_BLUETOOTH_AUTO_TIMEOUT
 *		full byte, unsigned integer from 1 to 255
 *
 *		Bluetooth device auto power-off timeout in seconds
 */
#define FERAM_BLUETOOTH_AUTO_TIMEOUT								0x03 // 1 byte (1 ... 255) -- seconds

/*
 *	FERAM_ADXL345_MOVEMENT_TIMEOUT
 *		full byte, unsigned integer from 1 to 255
 *
 *		ADXL345 timeout in seconds until sleep after last movement is detected
 */
#define FERAM_ADXL345_MOVEMENT_TIMEOUT							0x04 // 1 byte (1 ... 255) -- seconds

/*
 *	FERAM_GPS_GPRMC_GPGGA_FREQ
 *		2 nibbles, unsigned integer from 0 to 15
 *
 *			b0-b3: GPRMC frequency in seconds
 *			b4-b7: GPGGA frequency in seconds
 */
#define FERAM_GPS_GPRMC_GGA_GSA_FREQ								0x05 // 1 byte, holds 2 nibbles (0 ... 15) -- seconds
#define FERAM_GPS_GPRMC_GGA_FREQ										0
#define FERAM_GPS_GPGSA_FREQ												4

/*
 *	FERAM_GPS_GPRMC_GPGGA_FREQ
 *		2 nibbles, unsigned integer from 0 to 15
 *
 *			b0-b3: GPGSA frequency in seconds
 *			b4-b7: GPGSV frequency in seconds
 */
#define FERAM_GPS_GPZDA_GSV_FREQ										0x06 // 1 byte, holds 2 nibbles (0 ... 15) -- seconds
#define FERAM_GPS_GPZDA_FREQ												0
#define FERAM_GPS_GPGSV_FREQ												4

/*
 *	FERAM_GPS_MISC_CFG
 *		individual bits, 0 (off), 1 (on)
 *
 *		miscellaneous boolean GPS settings, mostly flag-type things
 *
 *
 *		for reading/setting individual bits see :
 *			http://www.catonmat.net/blog/low-level-bit-hacks-you-absolutely-must-know/
 *			http://www.avrfreaks.net/forum/tut-c-bit-manipulation-aka-programming-101?page=all
 *			and many others
 */
#define FERAM_GPS_MISC_CFG													0x07 // 1 byte, holds 8 bits (0, 1) -- flags
#define FERAM_GPS_MISC_CFG_POWER_CTL								0 // GPS power - on(1) or off(0)
#define FERAM_GPS_MISC_CFG_NMEA_PRINT_TO_SERIAL			1 // print NMEA sentences to serial - yes(1) or no(0)
#define FERAM_GPS_MISC_CFG_bit3											2 // unassigned - on(1) or off(0)
#define FERAM_GPS_MISC_CFG_bit4											3 // unassigned - on(1) or off(0)
#define FERAM_GPS_MISC_CFG_bit5											4 // unassigned - on(1) or off(0)
#define FERAM_GPS_MISC_CFG_bit6											5 // unassigned - on(1) or off(0)
#define FERAM_GPS_MISC_CFG_bit7											6 // unassigned - on(1) or off(0)
#define FERAM_GPS_MISC_CFG_bit8											7 // unassigned - on(1) or off(0)

/*
 *	FERAM_DEVICE_MISC_CFG
 *		individual bits, 0 (off), 1 (on)
 *
 *		miscellaneous boolean device settings
 */
#define FERAM_DEVICE_MISC_CFG1											0x08 // 1 byte, holds 8 bits (0, 1) -- flags
#define FERAM_DEVICE_MISC_CFG1_OLED_AUTO_POWER			0 // enable OLED auto-poweroff - auto(0), constant on(1)
#define FERAM_DEVICE_MISC_CFG1_ADXL345_AUTO_POWER		1	// allow the accelerometer to trigger a device sleep - yes(1), no(0)
#define FERAM_DEVICE_MISC_CFG1_SD_WRITE							2 // write NMEA to SD card on valid fix - yes(1), no(0)
#define FERAM_DEVICE_MISC_CFG1_STAT_WRITE						3 // write device statistics to SD card - yes(1) or no(0)
#define FERAM_DEVICE_MISC_CFG1_bit4									4 // unassigned - on(1) or off(0)
#define FERAM_DEVICE_MISC_CFG1_bit5									5 // unassigned - on(1) or off(0)
#define FERAM_DEVICE_MISC_CFG1_bit6									6 // unassigned - on(1) or off(0)
#define FERAM_DEVICE_MISC_CFG1_bit7									7 // unassigned - on(1) or off(0)

/*
 *	FERAM_DEVICE_MISC_CFG2
 *		two bits, 0 (off), 1 (on), 2(auto) 3(if defined)
 *
 *		miscellaneous quad_state device settings, e.g. on, off, something and something else
 */
#define FERAM_DEVICE_MISC_CFG2											0x09 // 1 byte - holds 4 bit-pairs, values 0-3
#define FERAM_DEVICE_MISC_CFG2_SYSTEM_SERIAL				0 // connection to serial console - GPS(0), SIM800(1), system(2)
#define FERAM_DEVICE_MISC_CFG2_BLUETOOTH_POWER			2 // Bluetooth power scheme - off(0), on(1), auto(2)
#define FERAM_DEVICE_MISC_CFG2_GPRS_FUNCTION				4 // GPRS module behaviour - nothing(0), push on demand(1), upload log(2), foo(3)
#define FERAM_DEVICE_MISC_CFG2_pair3								6 // unassigned

/*
 *	FERAM_ADXL345_ACTIVITY_THRESHOLD
 *		whole byte range
 *
 *		represents the activity threshold
 */
#define FERAM_ADXL345_ACTIVITY_THRESHOLD						0x0A	// 1 byte

/*
 *	FERAM_ADXL345_INACTIVITY_THRESHOLD
 *		whole byte range
 *
 *		represents the inactivity threshold
 */
#define FERAM_ADXL345_INACTIVITY_THRESHOLD					0x0B	// 1 byte

/*
 *
 *
 */
#define FERAM_ADXL345_FREE													0x0C	// 1 byte

/*
 *	FERAM_GSM_MISC_CFG
 *		individual bits, 0 (off), 1 (on)
 *
 *		miscellaneous boolean GSM settings, mostly flag-type things
 *
 */
#define FERAM_GSM_MISC_CFG													0x0D // 1 byte, holds 8 bits (0, 1) -- flags
#define FERAM_GSM_MISC_CFG_POWER_CTL								0 // GSM  power - on(1) or off(0)
#define FERAM_GSM_MISC_CFG_bit2											1 // unassigned - on(1) or off(0)
#define FERAM_GSM_MISC_CFG_bit3											2 // unassigned - on(1) or off(0)
#define FERAM_GSM_MISC_CFG_bit4											3 // unassigned - on(1) or off(0)
#define FERAM_GSM_MISC_CFG_bit5											4 // unassigned - on(1) or off(0)
#define FERAM_GSM_MISC_CFG_bit6											5 // unassigned - on(1) or off(0)
#define FERAM_GSM_MISC_CFG_bit7											6 // unassigned - on(1) or off(0)
#define FERAM_GSM_MISC_CFG_bit8											7 // unassigned - on(1) or off(0)

	/*
	 *	FERAM_GPS_LAST_GOOD_GNRMC
	 *		83-byte field
	 *
	 *		contains last good RMC NMEA sentece, which can be max. 82 + '\0' chars long
	 */
#define FERAM_GPS_LAST_GOOD_GNRMC										0x0E	// 83 bytes, holds a string (ends at 0x61)

	/*
	 *	#define FERAM_GPS_LAST_GOOD_GNGGA
	 *		83-byte field
	 *
	 *		contains last good RMC NMEA sentece, which can be max. 82 + '\0' chars long
	 */
#define FERAM_GPS_LAST_GOOD_GNGGA										0x62	// 83 bytes, holds a string (ends at 0xB5)


	/*
	 *	FERAM_GSM_APN
	 *		64-byte field
	 *
	 *		contains the APN, by protocol max 63 octets + '\0' long
	 */
#define FERAM_GSM_APN																0xB6	// 64 bytes, holds a string (end at 0xF6)

	/*
	 *	FERAM_GSM_HTTP_SERVER
	 *		64-byte field
	 *
	 *		contains the remote HTTP server URL, by design decision max 63 octets + '\0' long
	 */
#define	FERAM_GSM_HTTP_SERVER												0xF7	// 64 bytes, holds a string (ends at 0x137)

	/*
	 *	FERAM_
	 *		foo-byte field
	 *
	 *		contains the APN, by protocol max 63 octets + '\0' long
	 */
#define FERAM_FREE																	0x138 //
