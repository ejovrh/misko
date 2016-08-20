/*
 * here the display is defined
 * 
 * references for the libraries used are:
 * https://github.com/olikraus/u8glib/wiki
 * https://github.com/olikraus/m2tklib/wiki/ghref
 * https://github.com/olikraus/m2tklib/wiki
 * 
 * 
 */


// display construction start

M2_BUTTON(el_ok, "f4", "ok", fn_ok); // an ok button
// data output start

// datetime and timezone
M2_LABEL(el_gps_date, "rf0", gps_date); // label for gps_date
M2_LABEL(el_gps_time, "rf0", gps_time); // label for gps_time
M2_LABEL(el_gps_utc, "rf0", (int8_t) eeprom_get(EERPOM_TIMEZONE_INDEX)); // timezone string
M2_LIST(el_datetime_list) { &el_gps_date, &el_gps_time, &el_gps_utc, &el_ok} ; // create a list of gps date, time, timezone
M2_GRIDLIST(el_datetime_grid, "c1", el_datetime_list);
M2_ALIGN(el_top_datetime_menu, "-1|1W64H64", &el_datetime_grid);

// GPS position
M2_LABEL(el_gps_latitude, "rf0", gps_latitude); 
M2_LABEL(el_gps_longtitude, "rf0", gps_longtitude);
M2_LABEL(el_altitude, "rf0", gps_altitude);
M2_LABEL(el_gps_sat_in_view, "rf0", gps_satellites_in_view);
M2_LABEL(el_gps_hdop, "rf0", gps_hdop);
M2_LIST(el_position_list) = {&el_gps_latitude, &el_gps_longtitude, &el_altitude, &el_gps_sat_in_view, &el_gps_hdop, &el_ok};
M2_GRIDLIST(el_position_grid, "c1", el_position_list);
M2_ALIGN(el_top_position_menu, "-1|1W64H64", &el_position_grid);

// misc
M2_LABELFN(el_power_good, "fr0", fn_get_power_good_status);
M2_LABELFN(el_batt_charge, "rf0", fn_get_batt_charge_status);
M2_LIST(el_bat_list) = {&el_power_good, &el_batt_charge};
M2_HLIST(el_batt_hlist, "rf0", el_bat_list);
M2_LABELFN(el_batt_a, "rf0", fn_get_bat_pct);
M2_LABELFN(el_vcc, "rf0", fn_get_Vcc);
M2_LIST(el_bat_a) = {&el_batt_a, &el_vcc};
M2_HLIST(el_batta_hlist, "rf0", el_bat_a);
M2_LABEL(el_temperature, "rf0", temperature);
M2_LIST(el_device_misc_list) = {&el_batt_hlist, &el_batta_hlist, &el_temperature, &el_ok};
M2_GRIDLIST(el_device_misc_grid, "c1", el_device_misc_list);
M2_ALIGN(el_top_device_misc_menu, "-1|1W64H64", &el_device_misc_grid);

// data output end

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
M2_LIST(el_bluetooth_list) = { &el_bluetooth_power, &el_bluetooth_power_value, &el_bluetooth_power_timeout, &el_ok };
M2_GRIDLIST(el_bluetooth_grid, "c2", el_bluetooth_list);
M2_ALIGN(el_top_bluetooth_menu, "-1|1W64H64", &el_bluetooth_grid);
// bluetooth end

// lcd start
M2_LABEL(el_lcd_power, NULL, "Power");
M2_COMBO(el_lcd_power_value, NULL, &select_color, 2, fn_idx_to_lcd_power_value);
M2_S8NUMFN(el_lcd_power_timeout, "+0c1", 1, 5, fn_set_eerpom_lcd_timeout);
M2_LIST(el_lcd_list) = { &el_lcd_power, &el_lcd_power_value, &el_lcd_power_timeout, &el_ok };
M2_GRIDLIST(el_lcd_grid, "c2", el_lcd_list);
M2_ALIGN(el_top_lcd_menu, "-1|1W64H64", &el_lcd_grid);
// lcd end

// gps start
M2_LABEL(el_gps_freq, NULL, "Frequency");
M2_S8NUMFN(el_gps_freq_value, "+0c2", 1, 20, fn_set_eerpom_gps_log_freq);
M2_ROOT(el_gps_ok, "f4", "OK", &top_el_expandable_menu);
M2_LIST(el_gps_list) = { &el_gps_freq, &el_gps_freq_value, &el_gps_ok };
M2_GRIDLIST(el_gps_grid, "c2", el_gps_list);
M2_ALIGN(el_top_gps_menu, "-1|1W64H64", &el_gps_grid);
// gps end

m2_menu_entry m2_2lmenu_data[] = 
{
  { "Data", NULL },
  { ". Datetime", &el_top_datetime_menu },
  { ". Position", &el_top_position_menu },
  { ". Device Status", &el_top_device_misc_menu}, 
  { "Settings", NULL },
  { ". Timezone", &el_top_timezone_menu },
  { ". Bluetooth", &el_top_bluetooth_menu },
  { ". Display", &el_top_lcd_menu },
  { ". GPS", &el_top_gps_menu },
	{ ". GSM", NULL},
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
