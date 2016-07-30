/* eeprom_timer() - returns 1 if enough time has passed
 *  returns: 
 *    1 if time is up
 *    0 otherwise
 *    
 * input arguments: 
 *    unsigned int in_bluetooth_button_press_time - time of button press
 *    char in_eeprom_index - index of time paramater in EEPROM[]
 * 
 * times are in milliseconds !!
 */
int eeprom_timer(unsigned int in_button_press_time, unsigned int in_eeprom_index)
{
  return (abs( in_button_press_time - millis()) / 1000 > EEPROM[in_eeprom_index] ?  1 :  0); 
}
