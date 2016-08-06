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


void handle_bluetooth_button(void)
{
    // bluetooth power on
  if (!flag_bluetooth_power_toggle_pressed && digitalRead(bluetooth_power_toggle_pin) == HIGH) // if button was not pressed and now gets pressed
  //  flag_bluetooth_power_toggle_pressed prevents mutiple executions of this if() block
  { 
    bluetooth_button_press_time = millis(); // record time of button press; this is used in eeprom_timer()
    digitalWrite(bluetooth_mosfet_gate_pin, HIGH); // turn on the device
    flag_bluetooth_is_on = 1; // set flag to on
    flag_bluetooth_power_toggle_pressed = 1; // mark button as pressed
  }

  // bluetooth power off
  if (flag_bluetooth_power_toggle_pressed && digitalRead(bluetooth_power_toggle_pin) == LOW) // if button was  pressed and now gets released
  // flag_bluetooth_power_toggle_pressed prevents mutiple executions of this if() block
  { 
    bluetooth_button_release_time = millis(); // record time of button press; this is used in 
    flag_bluetooth_power_toggle_pressed = 0; // mark button as released

    if (abs(bluetooth_button_release_time - bluetooth_button_press_time) > 500) // if button is held long enough
    {
      if (flag_bluetooth_power_keep_on) // if the BT device was on
        digitalWrite(bluetooth_mosfet_gate_pin, LOW); // turn off the BT device

      flag_bluetooth_power_keep_on = !flag_bluetooth_power_keep_on; // invert the flag (on -> off or off -> on)
    }
  }

  // bluetooth timed power off
  if ( (!flag_bluetooth_power_keep_on && flag_bluetooth_is_on) && eeprom_timer(bluetooth_button_press_time, 4)) // if the device is on and enough time has passed
  // flag_bluetooth_power_keep_on prevents the timer from kicking in if we want the BT device to stay on
  // flag_bluetooth_is_on prevents code execution on every loop
  { 
      digitalWrite(bluetooth_mosfet_gate_pin, LOW); // turn off the device
      flag_bluetooth_is_on = 0; // set flag to off
  }
}

