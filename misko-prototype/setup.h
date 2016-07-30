/* basic container for setup items

  the container is the EEPROM object (offered by EEPROM.h) 
    Arduino offers the phyiscal EEPROM to be accessed in an array-like manner, which is 
    for our needs at the moment simple enough

  the idea is to initialize the EEPROM exactly once and not upon every boot
    
  kind of like "key - value", in a linear fashion
    a totally primitive approach but really good enough

  in order to prevent unnecessary EEPROM writes,  CONFIG_VERSION is checked

   CONFIG_VERSION MUST BE CHANGED IF ANY CHANGES ARE MADE BELOW
*/

if (EEPROM[0] != CONFIG_VERSION) // only if the version did change we will write into the EERPOM
{
  EEPROM[0] = CONFIG_VERSION; // indicates changes in structure, increment by one in case of any change
  EEPROM[1] = 'a'; // lcd power - on(1), off(0), auto(a), default auto
  EEPROM[2] = 'a'; // bluetooth power - on(1), off(0), auto(a), default auto
  EEPROM[3] = 2; // lcd auto timeout - auto timeout in seconds, 0, 1, ..., 254, default 2s
  EEPROM[4] = 10; // bluetooth auto timeout - auto timeout in seconds, 0, 1, ..., 254, default 60s
  EEPROM[5] = 2; // UTC timezone, -12, ..., +12, default +2
  EEPROM[6] = 1; // LOG_RMC - default on, RMC-Recommended Minimum Specific GNSS Data
  EEPROM[7] = 0; // LOG_GGA - default off, GGA-Global Positioning System Fixed Data
  EEPROM[8] = 0; // LOG_GLL - default off, GLL-Geographic Position-Latitude/Longitude
  EEPROM[9] = 1; // LOG_GSA - default on, GSA-GNSS DOP and Active Satellites 
  EEPROM[10] = 1; // LOG_GSV - default on, GSV-GNSS Satellites in View
  EEPROM[11] = 0; // LOG_VTG - default off, VTG-Course Over Ground and Ground Speed
  EEPROM[12] = 1; // USE_WAAS - default on
}
