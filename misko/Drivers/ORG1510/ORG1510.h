#ifndef ORG1510_ORG1510_H_
#define ORG1510_ORG1510_H_

#define UART1RXBUFFERSIZE	256// receive buffer of UART1
#define NMEA_BUFFERSIZE 82	// officially, NMEA sentences are at maximum 82 characters long (80 readable characters + \r\n)

#include <inttypes.h>

typedef struct  // public part of struct describing devices on Origin ORG1510 mk4 GPS receiver
{
	void (*parse)(void);  //parse function
} org1510_t;

org1510_t* org1510_ctor(void);  // declare constructor for concrete class

extern org1510_t *org1510;  // declare pointer to public struct part

#endif /* ORG1510_ORG1510_H_ */
