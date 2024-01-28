#ifndef INC_SD_SD_H_
#define INC_SD_SD_H_

#include "main.h"

#if defined(USE_SD)	// if this particular device is active
#include <string.h>

#define CARD_INSERTED GPIO_PIN_RESET	// if a SD card is inserted, the Card Detect GPIO will read high; otherwise low

typedef struct	// struct describing the SD Card functionality
{
	uint32_t (*Free)(void);  // returns free space in KiB
	unsigned int (*Read)(const char *filename, const void *data_in, const unsigned int len);  // reads n bytes from file into data buffer
	unsigned int (*Write)(const char *filename, const void *data_out, const unsigned int len);  // writes n bytes from file into data buffer
} sd_t;

sd_t* sd_ctor(SPI_HandleTypeDef *in_hspi, GPIO_TypeDef *_SPI_CS_Port, const uint16_t _SPI_CS_Pin, GPIO_TypeDef *_SD_CD_Port, const uint16_t _SD_CD_Pin);  // the SD constructor

extern sd_t *const SD;  // declare pointer to public struct part

#endif

#endif /* INC_SD_SD_H_ */
