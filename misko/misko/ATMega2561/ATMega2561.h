#ifndef ATMEGA2561_H_
#define ATMEGA2561_H_

typedef struct
{
	//volatile uint8_t wdtcr;
} ATMega2561_t;

ATMega2561_t *atmega_ctor(void);												// declare constructor for concrete class, does hardware initialization

#endif /* ATMEGA2561_H_ */