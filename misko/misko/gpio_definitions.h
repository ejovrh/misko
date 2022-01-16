#ifndef GPIO_DEFINITIONS_H_
#define GPIO_DEFINITIONS_H_

#define    MCP73871_charge_status_1_pin		A,   0,   0							// MCP73871_CHARGE_STATUS_1_PIN - see MCP73871 data sheet pp. 21 for function
#define    MCP73871_charge_status_2_pin		A,   1,   1							// MCP73871_CHARGE_STATUS_2_PIN - see MCP73871 data sheet pp. 21 for function
#define    MCP73871_power_good_pin			A,   2,   2							// MCP73871_POWER_GOOD_PIN - see MCP73871 data sheet pp. 21 for function
#define    gps_green_led_pin   				A,   3,   3 						// GPS_GREEN_LED_PIN - LED, GPS operation indicator
#define    gps_red_led_pin   				A,   4,   4 						// GPS_RED_LED_PIN - LED, SD card operation indicator
#define    GPS_wakeup_pin   				A,   5,   5							// GPS_WAKEUP_PIN - senses device power state
#define    GPS_power_ctl_pin   				A,   6,   6							// GPS_POWER_CTL_PIN - signals a wakeup to the GPS device via the voltage supervisor; the sleep command is issued in software
#define    analog_sw_ctrl_pin   			A,   7,   7							// ANALOG_SW_CTRL_PIN - controls analog switches for BT/GPS <-> micro comm

#define    SPI_SS_FRAM_pin   				B,   0,   0							// SPI_S_FRAM_PIN - no direct pin operations advised
#define    SPI_SCK_pin   					B,   1,   1							// SPI_SCK_PIN - no direct pin operations advised
#define    SPI_MOSI_pin   					B,   2,   2							// SPI_MOSI_PIN - no direct pin operations advised
#define    SPI_MISO_pin   					B,   3,   3							// SPI_MISO_PIN - no direct pin operations advised
#define    GPS_sw_serial_rx   				B,   4,   4							// GPS_SW_SERIAL_RX - no direct pin operations advised
#define    GPS_sw_serial_tx   				B,   5,   5							// GPS_SW_SERIAL_TX - no direct pin operations advised
#define    SPI_SS_ADXL345_pin   			B,   6,   6							// SPI_SS_ADXL345_PIN - SPI ADXL345 SS
#define    SPI_OLED_a0_pin   				B,   7,   7							// SPI_OLED_D/C_PIN - OLED data/command pin (aka. a0)

#define    menu_right_button_pin			C,   0,   0							// MENU_RIGHT_BUTTON_PIN - joystick
#define    menu_left_button_pin				C,   1,   1							// MENU_LEFT_BUTTON_PIN -joystick
#define    menu_center_button_pin			C,   2,   2							// MENU_CENTER_BUTTON_PIN - joystick
#define    menu_up_button_pin				C,   3,   3							// MENU_UP_BUTTON_PIN - joystick
#define    menu_bluetooth_power_button_pin	C,   4,   4							// MENU_BLUETOOTH_POWER_BUTTON_PIN - external button
#define    menu_gprs_power_button_pin		C,   5,   5							// MENU_GPRS_POWER_BUTTON_PIN - external button, LED equipped
#define    Bluetooth_switch_pin   			C,   6,   6							// BLUETOOTH_SWITCH_PIN - BT unit main power switch
#define    Bluetooth_wakeup_pin   			C,   7,   7							// BLUETOOTH_WAKEUP_PIN - BT wake from sleep

#define    I2C_SCL_pin   					D,   0,   0							// I2C_SCL_PIN - no direct pin operations advised
#define    I2C_SDA_pin   					D,   1,   1							// I2C_SDA_PIN - no direct pin operations advised
#define    SIM800C_rx_pin   				D,   2,   2							// SIM800C_RX_PIN - UART from GPRS modem, no direct pin operations advised
#define    SIM800C_tx_pin   				D,   3,   3							// SIM800C_TX_PIN - UART to GPRS mode, no direct pin operations advised
#define    SIM800C_cts_pin   				D,   4,   4							// SIM800C_CTS_PIN - UART CTS for GPRS modem, no direct pin operations advised
#define    SIM800C_power_ctrl_pin   		D,   5,   5							//TODO SIM800C_POWER_CTRL_PIN -
#define    SIM800C_power_pin   				D,   6,   6							//TODO SIM800C_POWER_PIN -
#define    WIFI_enable_pin   				D,   7,   7							//TODO WIFI_EN_PIN -

#define    PDI_UART0_RX_pin   				E,   0,   0							// PDI_UART0_RX_PIN -
#define    PDI_UART0_TX_pin   				E,   1,   1							// PDI_UART0_TX_PIN -
#define    SD_card_detect_pin				E,   2,   2							// SD_CARD_DETECT_PIN - SD card detect
#define    SPI_SS_SD_card_pin				E,   3,   3							// SPI_SS_SD_CARD_PIN - SPI SD SS
#define    WIFI_INT_interrupt_pin			E,   4,   4							// WIFI_IRQ_PIN - interrupt sense for WiFi events
#define    SPI_SS_RTC_pin					E,   5,   5							// SPI_SS_RTC_PIN - SPI RTC SS
#define    RTC_INT_interrupt_pin			E,   6,   6							// RTC_INT_INTERRUPT_PIN - interrupt sense for RTC events
#define    ADXL345_INT1_interrupt_pin		E,   7,   7							// ADXL345_INT1_INTERRUPT_PIN - interrupt sense for accelerometer events

#define    SPI_SS_WIFI_pin					F,   0,   0							// SPI_SS_WIFI_MODULE - SPI SS for WiFi
#define    Vcc_sense_pin					F,   1,   1							// VCC_SENSE_PIN - ADC, Vcc measurement
#define    bat_A_pin						F,   2,   2							// BAT_A_PIN - ADC, Vbat measurement
#define    TMP36_Vsense_pin					F,   3,   3							// TMP36_VSENSE_PIN - ADC, temperature measurement
#define    JTAG_TCL_pin						F,   4,   4							// JTAG_TCL_PIN - no direct pin operations advised
#define    JTAG_TMS_pin						F,   5,   5							// JTAG_TMS_PIN - no direct pin operations advised
#define    JTAG_TDO_pin						F,   6,   6							// JTAG_TDO_PIN - no direct pin operations advised
#define    JTAG_TDI_pin						F,   7,   7							// JTAG_TDI_PIN - no direct pin operations advised

#define    WIFI_wake_pin					G,   0,   0							//TODO WIFI_WAKE_PIN -
#define    menu_down_button_pin				G,   1,   1							// MENU_DOWN_BUTTON_PIN - joystick
#define    Bluetooth_profile_indicator_pin	G,   2,   2							// BLUETOOTH_PROF_IND_PIN - indicates currently used bluetooth profile (BT classic vs. BLE)
#define    SPI_OLED_reset_pin				G,   3,   3							// SPI_OLED_RESET_PIN - OLED reset
#define    SPI_SS_OLED_pin					G,   4,   4							// SPI_SS_OLED_PIN - SPI OLED SS
#define    PDI_UART0_CTS_pin				G,   5,   5							// PDI_UART0_CTS_PIN -

#endif /* GPIO_DEFINITIONS_H_ */