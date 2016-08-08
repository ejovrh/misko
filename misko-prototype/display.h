// display construction start
M2_EXTERN_ALIGN(top_el_expandable_menu); // Forward declaration of the toplevel element

uint8_t select_color = 0;

void fn_ok(m2_el_fnarg_p fnarg) // accept selection
{
  m2_SetRoot(&top_el_expandable_menu);
}

const char *fn_idx_to_bluetooth_power_value(uint8_t idx)
{
  if ( idx == 0 )
    return "off";
  else if (idx == 1 )
    return "on";
  return "auto";
}

const char *fn_idx_to_lcd_power_value(uint8_t idx)
{
  if ( idx == 1 )
    return "on";
  else if (idx == 2 )
    return "auto";
}

int8_t fn_set_eerpom_tz(m2_rom_void_p element, uint8_t msg, int8_t val) // callback for EEPROM timezone setting
{
  if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
    return eeprom_set(EERPOM_TIMEZONE_INDEX);
  
  if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
    eeprom_get(val, EERPOM_TIMEZONE_INDEX);
}

fn_set_eerpom_bluetooth_timeout(m2_rom_void_p element, uint8_t msg, uint8_t val)
{
  if ( msg == M2_U8_MSG_GET_VALUE ) // if we get a GET message
    return eeprom_get(EERPOM_BLUETOOTH_POWER_INDEX); // set val to the EEPROM value at that index
  
  if ( msg == M2_U8_MSG_SET_VALUE ) // if we get a SET message
    eeprom_set(EERPOM_BLUETOOTH_POWER_INDEX); // set the EEPROM value at that index to val
}

// timezone start 
M2_LABEL(el_timezone_utc, NULL, "UTC");
M2_S8NUMFN(el_timezone_utc_value, "+1c2", -12, 12, fn_set_eerpom_tz);
M2_ROOT(el_timezone_ok, "f4", "OK", &top_el_expandable_menu);
M2_LIST(el_timezone_list) = { &el_timezone_utc, &el_timezone_utc_value, &el_timezone_ok };
M2_GRIDLIST(el_timezone_grid, "c2", el_timezone_list);
M2_ALIGN(el_top_timezone_menu, "-1|1W64H64", &el_timezone_grid);
// timezone end

// bluetooth start
M2_LABEL(el_bluetooth_power, NULL, "Power");
M2_COMBO(el_bluetooth_power_value, NULL, &select_color, 3, fn_idx_to_bluetooth_power_value);
M2_S8NUMFN(el_bluetooth_power_timeout, "+0c1", 1, 5, fn_set_eerpom_bluetooth_timeout);
M2_BUTTON(el_ok, "f4", "ok", fn_ok);
M2_LIST(el_bluetooth_list) = { &el_bluetooth_power, &el_bluetooth_power_value, &el_bluetooth_power_timeout, &el_ok };
M2_GRIDLIST(el_bluetooth_grid, "c2", el_bluetooth_list);
M2_ALIGN(el_top_bluetooth_menu, "-1|1W64H64", &el_bluetooth_grid);
// bluetooth end

// lcd start
M2_LABEL(el_lcd_power, NULL, "Power");
M2_COMBO(el_lcd_power_value, NULL, &select_color, 3, fn_idx_to_lcd_power_value);
M2_LIST(el_lcd_list) = { &el_lcd_power, &el_lcd_power_value, &el_ok };
M2_GRIDLIST(el_lcd_grid, "c2", el_lcd_list);
M2_ALIGN(el_top_lcd_menu, "-1|1W64H64", &el_lcd_grid);
// lcd end

// gps start
uint8_t gps = 2;
M2_LABEL(el_gps_freq, NULL, "Frequency");
M2_U8NUM(el_gps_freq_value, "+0c2", 1, 30, &gps);
M2_ROOT(el_gps_ok, "f4", "OK", &top_el_expandable_menu);
M2_LIST(el_gps_list) = { &el_gps_freq, &el_gps_freq_value, &el_gps_ok };
M2_GRIDLIST(el_gps_grid, "c2", el_gps_list);
M2_ALIGN(el_top_gps_menu, "-1|1W64H64", &el_gps_grid);
// gps end

m2_menu_entry m2_2lmenu_data[] = 
{
  { "Data", NULL },
  { ". GPS", NULL },
  { ". Device", NULL },
  { "Settings", NULL },
  { ". Timezone", &el_top_timezone_menu },
  { ". Bluetooth", &el_top_bluetooth_menu },
  { ". Display", &el_top_lcd_menu },
  { ". GPS", &el_top_gps_menu },
  { NULL, NULL },
};

uint8_t m2_2lmenu_first; // first visible line
uint8_t m2_2lmenu_cnt; // total visible lines

M2_2LMENU(el_2lmenu,"l4F3e15W43",&m2_2lmenu_first,&m2_2lmenu_cnt, m2_2lmenu_data,65,102,'\0');
M2_SPACE(el_space, "W1h1");
M2_VSB(el_vsb, "l4W2r1", &m2_2lmenu_first, &m2_2lmenu_cnt);
M2_LIST(list_2lmenu) = { &el_2lmenu, &el_space, &el_vsb };
M2_HLIST(el_hlist, NULL, list_2lmenu);
M2_ALIGN(top_el_expandable_menu, "-1|1W64H64", &el_hlist);

M2tk m2(&top_el_expandable_menu, m2_es_arduino, m2_eh_4bs, m2_gh_u8g_ffs); // m2 object and constructor

// display construction end
