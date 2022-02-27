#ifndef FM25W256_H_
#define FM25W256_H_

#include <inttypes.h>

#define CMD_WRSR	0x01	// Write Status Register; sets write protection features (p. 7)
#define CMD_WRITE	0x02	// Write Memory Data; writes one byte to a 2 byte address (p. 8)
#define CMD_READ	0x03	// Read Memory Data; reads one byte from a 2 byte address (p. 8)
#define CMD_WRDI	0x04	// Clear Write Enable Latch; puts module in read-only mode (p. 6)
#define CMD_RDSR	0x05	// Read Status Register; reads write protection features (p. 7)
#define CMD_WREN	0x06	// Set Write Enable Latch; disabled on power on (p. 6)

typedef struct  // public part of struct describing devices on FM25W256
{
	uint8_t (*ReadString)(const uint16_t addr, uint8_t *out_data, const uint8_t len);  // reads a length of string from from address
	uint8_t (*WriteString)(const uint16_t addr, uint8_t *in_data, const uint8_t len);  // writes a length of string from from address
} fm25w256_t;

fm25w256_t* fm25w256_ctor(void);  // declare constructor for concrete class

extern fm25w256_t *fm25w256;  // declare pointer to public struct part

#endif /* FM25W256_H_ */
