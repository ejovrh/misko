#ifndef ATMEGA2561_H_
#define ATMEGA2561_H_

typedef struct
{
	volatile uint8_t foocounter;												// counter variable for timer-based printout
	volatile uint8_t flag_sleep;												// flag for MCU sleep control
} ATMega2561_t;

ATMega2561_t *atmega_ctor(void);												// declare constructor for concrete class, does hardware initialization

#endif /* ATMEGA2561_H_ */