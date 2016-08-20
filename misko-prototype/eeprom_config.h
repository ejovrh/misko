/* basic container for setup items

  the container is the EEPROM object (offered by EEPROM.h) 
    Arduino offers the phyiscal EEPROM to be accessed in an array-like manner, which is 
    for our needs at the moment simple enough

  the idea is to initialize the EEPROM exactly once and not upon every boot
    
  kind of like "key - value", in a linear fashion
    a totally primitive approach but really good enough

  in order to prevent unnecessary EEPROM writes,  CONFIG_VERSION is checked

   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE BELOW

   EEPROM values are stored as uint8_t -- CAST PROPERLY BEFORE USE
   
*/

if (EEPROM[0] != CONFIG_VERSION) // only if the version did change we will write into the EERPOM
{
  EEPROM[0] = CONFIG_VERSION; // indicates changes in structure, increment by one in case of any change
  EEPROM[EERPOM_LCD_POWER_INDEX] = 0; // lcd power - on(1), auto(0), default auto
  EEPROM[EERPOM_BLUETOOTH_POWER_INDEX] = 0; // bluetooth power - on(1), off(0), auto(2), default auto
  EEPROM[EERPOM_LCD_AUTO_TIMEOUT_INDEX] = 2; // lcd auto timeout - auto timeout in seconds, 0, 1, ..., 254, default 2s
  EEPROM[EERPOM_BLUETOOTH_AUTO_TIMEOUT_INDEX] = 5; // bluetooth auto timeout - auto timeout in seconds, 0, 1, ..., 254, default 60s
  EEPROM[EERPOM_TIMEZONE_INDEX] = 2; // UTC timezone, -12, ..., +12, default +2
  EEPROM[EEPROM_GPS_GPRMC_GGA_FREQ_INDEX] = 1; // frequency of RMC and GGA messages from the em406 module, 1 - 20
  EEPROM[EEPROM_GPS_USE_WAAS_INDEX] = 1; // USE_WAAS - default on
}
