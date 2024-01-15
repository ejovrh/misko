#ifndef ORG1510_ORG1510_H_
#define ORG1510_ORG1510_H_

#define UART1_DMA_RX_BUFFERSIZE 256 // incoming NMEA buffer, sized so that when HC IRQ fires, at least one complete sentence is in
#define UART1_RB_BUFFERSIZE	512
#define NMEA_BUFFERSIZE 82	// officially, NMEA sentences are at maximum 82 characters long (80 readable characters + \r\n)

#include <inttypes.h>

typedef struct  // public part of struct describing devices on Origin ORG1510 mk4 GPS receiver
{
	char NMEA[NMEA_BUFFERSIZE];

	void (*RX_Check)(void);  //parse function
} org1510_t;

org1510_t* org1510_ctor(void);  // declare constructor for concrete class

extern org1510_t *org1510;  // declare pointer to public struct part

#endif /* ORG1510_ORG1510_H_ */
