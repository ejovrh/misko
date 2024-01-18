#ifndef INC_SD_SD_H_
#define INC_SD_SD_H_

#include "main.h"

#if defined(USE_SD)	// if this particular device is active

typedef struct	// struct describing the SD Card functionality
{
	void (*Read)(void);  // reads a length of string from address
	void (*Write)();  // writes a length of string from address
} sd_t;

sd_t* sd_ctor(SPI_HandleTypeDef *in_hspi, GPIO_TypeDef *_SPI_CS_Port, const uint16_t _SPI_CS_Pin);  // the SD constructor

extern sd_t *const SD;  // declare pointer to public struct part

#endif

#endif /* INC_SD_SD_H_ */
