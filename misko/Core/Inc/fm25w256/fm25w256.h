#ifndef INC_FM25W256_FM25W256_H_
#define INC_FM25W256_FM25W256_H_

#include "main.h"

#if defined(USE_FM25W256)	// if this particular device is active

typedef struct	// struct describing the FeRAM functionality
{
	void (*ReadBytes)(const uint16_t in_addr, uint8_t *out_data, const uint8_t in_len);  // reads a length of string from address
	void (*WriteBytes)(const uint16_t in_addr, uint8_t *in_data, const uint8_t in_len);  // writes a length of string from address
} fm25w256_t;

fm25w256_t* fm25w256_ctor(SPI_HandleTypeDef *in_hspi, GPIO_TypeDef *_SPI_CS_Port, const uint16_t _SPI_CS_Pin);  // the FM25W256 constructor

extern fm25w256_t *const FM25W256;  // declare pointer to public struct part

#endif

#endif /* INC_FM25W256_FM25W256_H_ */
