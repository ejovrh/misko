#include <avr/io.h>
#include <stdint-gcc.h>
#include <avr/interrupt.h>

#include "uart/uart.h"
#include "adxl345.h"
#include "DS1394.h"
#include "FM25W256.h"

// local files
#include "gpio.h"
#include "gpio_definitions.h"

#define UART0_BAUD_RATE 9600
#define UART1_BAUD_RATE 9600

volatile uint8_t adxl345_irq_src;

int main(void)
{
	#include "gpio_modes.h"

	cli();										// globally disable interrupts

	EIMSK |= (1 << INT7);				// enable INT7 (lives on pin PE7)
	EICRB |= (1 << ISC70);			// set to register
	EICRB |= (0 << ISC71);			//	...	any logic level change

	TCCR5A  = 0x0;						// clear the register (A and B)
	TCCR5B  = 0x0;
	OCR5A = 3900;							// set compare match register to desired timer count
	TCCR5B |= (1 << WGM12);		// turn on CTC mode
	TCCR5B |= (1 << CS10);		// Set CS10 and CS12 bits for 1024 prescaler
	TCCR5B |= (1 << CS12);
	TIMSK5 |= (1 << OCIE5A);	// enable timer compare interrupt

	// hardware UART initialization
	uart0_init(UART_BAUD_SELECT(UART0_BAUD_RATE,F_CPU)); // USB
	uart0_puts("UART0 up\r\n");

	uart1_init(UART_BAUD_SELECT(UART1_BAUD_RATE,F_CPU)); // SIM800
	uart1_puts("UART1 up\r\n");

	sei(); // globally enable interrupts

	adxl345_init();
	fm25w256_init();
	ds1394_init();



	if (gpio_rd(PIN, SD_card_detect_pin) == LOW)
		uart0_puts("SD inserted\r\n");
	else
		uart0_puts("SD removed\r\n");

	while (1)
	{
		if (gpio_tst(menu_left_button_pin) == LOW)
			uart0_puts("left\r\n");

		if (gpio_tst(menu_right_button_pin) == LOW)
		{
			uart0_puts("right\r\n");
			uart0_putc(40+ds1394_read(0x03)); // very crude way of verifying that i can read from the rtc (minutes, in this example)
		}

		if (gpio_tst(menu_up_button_pin) == LOW)
			uart0_puts("up\r\n");

		if (gpio_tst(menu_down_button_pin) == LOW)
			uart0_puts("down\r\n");

		if (gpio_tst(menu_center_button_pin) == LOW)
		{
			uart0_puts("center\r\n");
		}

		if (gpio_tst(menu_gprs_power_button_pin) == LOW)
			uart0_puts("GPRS\r\n");

		if (gpio_tst(menu_bluetooth_power_button_pin) == LOW)
		{
				//uint8_t foo;
				//fm25w256_write_byte(0x0002, 70);
				//foo = fm25w256_read_byte(0x0002);
				//uart0_putc('.');
				//uart0_putc(foo);
				//uart0_putc('.');
				//uart0_puts("\r\n");
				uart0_puts("BT\r\n");

				char string[12] = "zello world";
				fm25w256_write_string(0x3123, &string, 11); //FIXME



				uart0_puts("readback\r\n");
				char retstring[12] = "";
				fm25w256_read_string(0x4123, retstring, 11);


uart0_puts("readback string\r\n");
				uart0_putc('.');
				//uart0_puts(retstring);
				for(int i=0; i<12; i++)
					uart0_putc(*retstring+i);
				uart0_putc('.');


				uart0_puts("\r\n-");
				uart0_putc(fm25w256_read_byte(0x4123));

		}

	} // while(1)
} // main()

// ISR for the 500ms timer
ISR(TIMER5_COMPA_vect)
{
	gpio_toggle(gps_red_led_pin);
};

// ISR for the ADXL345 accelerometer
ISR(INT7_vect)
{
	cli();
	adxl345_irq_src = adxl345_read(INT_SOURCE);
	uart1_puts("isr 7\r\n");

	// inactivity
	if( (adxl345_irq_src >> 3) & 0x01)  // if the inact bit is set
	{
		uart0_puts("Inactivity");
		adxl345_write(BW_RATE, 0x17 ); // 0001 0111 (0x1A) - set to low power mode, bit 5 (was 0x0A, becomes 0x1A)
	}

	// activity
	if( (adxl345_irq_src >> 4) & 0x01) // if the act bit is set
	{
		uart0_puts("Activity");

		// set the device back in measurement mode
		// as suggested on the datasheet, we put it in standby then in measurement mode
		//adxl345_write(POWER_CTL_CFG, powerCTL & 11110011);
		//adxl345_write(POWER_CTL, 0x04); // first standby
		//adxl345_write(POWER_CTL_CFG, powerCTL & 11111011);
		//adxl345_write(POWER_CTL, POWER_CTL_CFG); // then full measurement mode

		adxl345_write(BW_RATE, 0x07 ); // 0000 0111 (0x0A) get back to full accuracy measurement (we will consume more power)
	}
	sei();
};