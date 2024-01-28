#include "sd/sd.h"

#if defined(USE_SD)	// if this particular device is active

typedef struct	// sdc_t actual
{
	sd_t public;  // public struct

	SPI_HandleTypeDef *_hspi;  // HAL SPI instance
	GPIO_TypeDef *_CS_Port;  // SPI chip select GPIO port
	uint16_t _CS_Pin;  // SPI chip select GPIO pin
	GPIO_TypeDef *_CD_Port;  // SD Card detect GPIO port
	uint16_t _CD_Pin;  // SD Card detect GPIO pin
} __sd_t;

static __sd_t  __SD  __attribute__ ((section (".data")));  // preallocate __SD object in .data

// reads n bytes from file into data buffer
static void _Read(uint8_t *filename, uint8_t *data_in, const uint16_t len)
{
	;
}

// writes n bytes from file into data buffer
static void _Write(uint8_t *filename, uint8_t *data_out, const uint16_t len)
{
	;
}

static __sd_t  __SD =  // instantiate sdc_t actual and set function pointers
	{  //
	.public.Read = &_Read,  // reads n bytes from device
	.public.Write = &_Write  // writes n bytes to device
	};

sd_t* sd_ctor(SPI_HandleTypeDef *in_hspi, GPIO_TypeDef *_SPI_CS_Port, const uint16_t _SPI_CS_Pin, GPIO_TypeDef *_SD_CD_Port, const uint16_t _SD_CD_Pin)  //
{
	__SD._hspi = in_hspi;  // store SPI object
	__SD._CS_Port = _SPI_CS_Port;  // store SPI Chip select port
	__SD._CS_Pin = _SPI_CS_Pin;  // store SPI Chip select pin
	__SD._CD_Port = _SD_CD_Port;  // store SD Card detect port
	__SD._CD_Pin = _SD_CD_Pin;  // store SD Card detect pin

	HAL_GPIO_WritePin(__SD._CS_Port, __SD._CS_Pin, GPIO_PIN_SET);  // chip select is active low

	return &__SD.public;  // set pointer to SD public part
}

sd_t *const SD = &__SD.public;  // set pointer to SD public part

#endif
