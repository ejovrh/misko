/*
 * here the display is defined
 * 
 * references for the libraries used are:
 * https://github.com/olikraus/u8glib/wiki
 * https://github.com/olikraus/m2tklib/wiki/ghref
 * https://github.com/olikraus/m2tklib/wiki
 */

// display construction start

M2_BUTTON(el_ok, "f4", "ok", fn_ok); // an ok button
M2_SPACE(el_whitespace, "W6h8");
// data output start

// datetime and timezone
M2_LABEL(el_gps_date, "rf0", gps_date); // label for gps_date
M2_LABEL(el_gps_time, "rf0", gps_time); // label for gps_time
M2_LIST(el_list_datetime) = {&el_gps_date, &el_whitespace, &el_gps_time};
M2_HLIST(el_hlist_datetime, "rf0", el_list_datetime);
M2_LABEL(el_utc, "rf0", "UTC");
M2_LABELFN(el_gps_utc, "rf0", fn_cb_utc); // timezone string
M2_LIST(el_list_utc) = {&el_utc, &el_whitespace, &el_gps_utc};
M2_HLIST(el_hlist_utc, "rf0", el_list_utc);
M2_LIST(el_datetime_list) { &el_hlist_datetime, &el_hlist_utc, &el_ok} ;
M2_GRIDLIST(el_datetime_grid, "c1", el_datetime_list);
M2_ALIGN(el_top_datetime_menu, "-0|1W64H64", &el_datetime_grid);
// datetime and timezone

// GPS position
M2_LABELFN(el_gps_latitude, "rf0", fn_cb_gps_latitude); 
M2_LABELFN(el_gps_longtitude, "rf0", fn_cb_gps_longtitude);
M2_LABELFN(el_altitude, "rf0", fn_cb_gps_altitude);
M2_LABELFN(el_gps_sat_in_view, "rf0", fn_cb_gps_satellites_in_view);
M2_LABELFN(el_gps_hdop, "rf0", fn_cb_gps_hdop);
M2_LIST(el_sat_dop_list) = {&el_gps_sat_in_view, &el_whitespace, & el_gps_hdop};
M2_HLIST(el_hlist_sat_dop, "rf0", el_sat_dop_list);
M2_LIST(el_position_list) = {&el_gps_latitude, &el_gps_longtitude, &el_altitude, &el_hlist_sat_dop, &el_ok};
M2_GRIDLIST(el_position_grid, "c1", el_position_list);
M2_ALIGN(el_top_position_menu, "-0|1W64H64", &el_position_grid);
// GPS position

// device status
M2_LABELFN(el_power_good, "fr0", fn_cb_get_power_good_status);
M2_LABELFN(el_batt_charge, "rf0", fn_cb_get_batt_charge_status);
M2_LIST(el_power_list) = {&el_power_good, &el_whitespace, &el_batt_charge};
M2_HLIST(el_power_hlist, "rf0", el_power_list);
M2_LABELFN(el_batt_a, "rf0", fn_cb_get_bat_pct);
M2_LABEL(el_batt_b, "rf0", "batB 0%");
M2_LABELFN(el_vcc, "rf0", fn_cb_get_Vcc);
M2_LIST(el_list_batt) = {&el_batt_a, &el_whitespace, &el_batt_b};
M2_HLIST(el_hlist_batt, "rf0", el_list_batt);
M2_LABELFN(el_temperature, "rf0", fn_cb_get_temperature);
M2_LIST(el_list_vcc_temp) = {&el_vcc, &el_whitespace, &el_temperature};
M2_HLIST(el_hlist_vcc_temp, "rf0", el_list_vcc_temp);
M2_LIST(el_device_misc_list) = {&el_power_hlist, &el_hlist_batt, &el_hlist_vcc_temp, &el_ok};
M2_GRIDLIST(el_device_misc_grid, "c1", el_device_misc_list);
M2_ALIGN(el_top_device_misc_menu, "-0|1W64H64", &el_device_misc_grid);
// device status

// SD card contents
M2_STRLIST(el_fs_strlist, "l5f0W55", &fs_m2tk_first, &fs_m2tk_cnt, fs_strlist_getstr);
// M2_SPACE(el_fs_space, "W1h1");
// M2_VSB(el_fs_strlist_vsb, "l5W4r1", &fs_m2tk_first, &fs_m2tk_cnt);
M2_LIST(list_fs_strlist) = { &el_fs_strlist};
// M2_HLIST(el_fs_hlist, "rf0", list_fs_strlist);
M2_GRIDLIST(el_fs_grid, "c2", list_fs_strlist);
M2_ALIGN(el_top_sd_content_menu, "-1|1W64H64", &el_fs_grid);
// SD card contents

// data output end

// SD card start el_top_sd_menu
M2_LABEL(el_sd_write, "rf0", "SD card write");
M2_COMBOFN(el_sd_write_setting, "rf0", 2, fn_cb_sd_write);
M2_LIST(el_sd_list) = { &el_sd_write, &el_sd_write_setting, &el_ok };
M2_GRIDLIST(el_sd_grid, "c2", el_sd_list);
M2_ALIGN(el_top_sd_menu, "-1|1W64H64", &el_sd_grid);
// SD card end

// serial setup
M2_LABEL(el_serial, "rf0", "serial port");
M2_COMBOFN(el_serial_setting, "rf0", 3, fn_cb_serial_setting);
M2_LIST(el_serial_list) = { &el_serial, &el_serial_setting, &el_ok };
M2_GRIDLIST(el_serial_grid, "c2", el_serial_list);
M2_ALIGN(el_top_serial_menu, "-1|1W64H64", &el_serial_grid);
// serial setup

// timezone start 
M2_LABEL(el_timezone_utc, "rf0", "UTC");
M2_S8NUMFN(el_timezone_utc_value, "+1c2", -12, 12, fn_cb_set_eerpom_tz);
M2_ROOT(el_timezone_ok, "f4", "OK", &top_el_expandable_menu);
M2_LIST(el_timezone_list) = { &el_timezone_utc, &el_timezone_utc_value, &el_timezone_ok };
M2_GRIDLIST(el_timezone_grid, "c2", el_timezone_list);
M2_ALIGN(el_top_timezone_menu, "-1|1W64H64", &el_timezone_grid);
// timezone end

// Bluetooth start
M2_LABEL(el_bluetooth_power, "rf0", "Power");
M2_COMBOFN(el_bluetooth_power_value, "rf0", 3, fn_cb_bluetooth_power_setting);
M2_S8NUMFN(el_bluetooth_power_timeout, "+0c1f0", 1, 5, fn_cb_set_eerpom_bluetooth_timeout);
M2_LIST(el_bluetooth_list) = { &el_bluetooth_power, &el_bluetooth_power_value, &el_bluetooth_power_timeout, &el_ok };
M2_GRIDLIST(el_bluetooth_grid, "c2", el_bluetooth_list);
M2_ALIGN(el_top_bluetooth_menu, "-1|1W64H64", &el_bluetooth_grid);
// Bluetooth end

// OLED start
M2_LABEL(el_lcd_power, "rf0", "Power");
M2_COMBOFN(el_lcd_power_value, "rf0", 2, fn_cb_lcd_power_setting);
M2_S8NUMFN(el_lcd_power_timeout, "+0c1f0", 1, 5, fn_cb_set_eerpom_lcd_timeout);
M2_LIST(el_lcd_list) = { &el_lcd_power, &el_lcd_power_value, &el_lcd_power_timeout, &el_ok };
M2_GRIDLIST(el_lcd_grid, "c2", el_lcd_list);
M2_ALIGN(el_top_lcd_menu, "-1|1W64H64", &el_lcd_grid);
// OLED end

// GPS start
M2_LABEL(el_gps_power, "rf0", "Power");
M2_COMBOFN(el_gps_power_value, "rf0", 2, fn_cb_gps_power_setting);
M2_LABEL(el_nmea_printout, "rf0", "NMEA printout");
M2_COMBOFN(el_nmea_printout_value, "rf0", 2, fn_cb_nmea_printout_setting);
M2_LABEL(el_gps_freq, "rf0", "Frequency");
M2_S8NUMFN(el_gps_freq_value, "+0c1f0", 1, 20, fn_cb_set_eerpom_gps_log_freq);
M2_ROOT(el_gps_ok, "f0", "OK", &top_el_expandable_menu);
M2_LIST(el_gps_list) = { &el_gps_power, &el_gps_power_value, &el_nmea_printout, &el_nmea_printout_value, &el_gps_freq, &el_gps_freq_value, &el_gps_ok };
M2_GRIDLIST(el_gps_grid, "c2", el_gps_list);
M2_ALIGN(el_top_gps_menu, "-1|1W64H64", &el_gps_grid);
// GPS end

// GSM start
M2_COMBOFN(el_gsm_power, "rf0", 2, fn_cb_gsm_power);
M2_ROOT(el_gsm_ok, "f0", "OK", &top_el_expandable_menu);

M2_LIST(el_gsm_list) = { &el_gsm_power, &el_gsm_ok };
M2_GRIDLIST(el_gsm_grid, "c2", el_gsm_list);
M2_ALIGN(el_top_gsm_menu, "-1|1W64H64", &el_gsm_grid);
// GSM end

m2_menu_entry m2_2lmenu_data[] = 
{
  { "Data", NULL },
  { ". Datetime", &el_top_datetime_menu },
  { ". Position", &el_top_position_menu },
  { ". Device Status", &el_top_device_misc_menu}, 
	{ ". SD card", &el_top_sd_content_menu}, 
  { "Settings", NULL },
	{ ". SD card", &el_top_sd_menu},
	{ ". Serial port", &el_top_serial_menu}, 
  { ". Timezone", &el_top_timezone_menu },
  { ". Bluetooth", &el_top_bluetooth_menu },
  { ". Display", &el_top_lcd_menu },
  { ". GPS", &el_top_gps_menu },
	{ ". GSM", &el_top_gsm_menu },
  { NULL, NULL },
};

uint8_t m2_2lmenu_first; // first visible line
uint8_t m2_2lmenu_cnt; // total visible lines

M2_2LMENU(el_2lmenu,"l5f0e0W55",&m2_2lmenu_first,&m2_2lmenu_cnt, m2_2lmenu_data,NULL,NULL,'\0');
M2_SPACE(el_space, "W1h1");
M2_VSB(el_vsb, "l5w4r1", &m2_2lmenu_first, &m2_2lmenu_cnt);
M2_LIST(list_2lmenu) = { &el_2lmenu, &el_space, &el_vsb };
M2_HLIST(el_hlist, NULL, list_2lmenu);
M2_ALIGN(top_el_expandable_menu, "-1|1W64H64", &el_hlist);

M2tk m2(&top_el_expandable_menu, m2_es_arduino, m2_eh_4bs, m2_gh_u8g_ffs); // m2 object and constructor

// display construction end
