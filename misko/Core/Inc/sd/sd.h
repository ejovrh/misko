#ifndef INC_SD_SD_H_
#define INC_SD_SD_H_

#include "main.h"

#if defined(USE_SD)	// if this particular device is active

#define CARD_INSERTED GPIO_PIN_RESET	// if a SD card is inserted, the Card Detect GPIO will read high; otherwise low

typedef struct	// struct describing the SD Card functionality
{
	void (*Read)(uint8_t *filename, uint8_t *data_in, const uint16_t len);  // reads n bytes from file into data buffer
	void (*Write)(uint8_t *filename, uint8_t *data_out, const uint16_t len);  // writes n bytes from file into data buffer
} sd_t;

sd_t* sd_ctor(SPI_HandleTypeDef *in_hspi, GPIO_TypeDef *_SPI_CS_Port, const uint16_t _SPI_CS_Pin, GPIO_TypeDef *_SD_CD_Port, const uint16_t _SD_CD_Pin);  // the SD constructor

extern sd_t *const SD;  // declare pointer to public struct part

#endif

#endif /* INC_SD_SD_H_ */
