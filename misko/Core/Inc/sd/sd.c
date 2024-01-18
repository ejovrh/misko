#include "sd/sd.h"

#if defined(USE_SD)	// if this particular device is active

typedef struct	// sdc_t actual
{
	sd_t public;  // public struct

	SPI_HandleTypeDef *_hspi;  // HAL SPI instance
	GPIO_TypeDef *_CS_Port;  // SPI chip select GPIO port
	uint16_t _CS_Pin;  // SPI chip select GPIO pin
} __sd_t;

static __sd_t __SD __attribute__ ((section (".data")));  // preallocate __SD object in .data

// reads a length of string from address
static void _Read(void)
{
	;
}

// writes a length of string from address
static void _Write(void)
{
	;
}

static __sd_t __SD =  // instantiate sdc_t actual and set function pointers
	{  //
	.public.Read = &_Read,  // reads n bytes from device
	.public.Write = &_Write  // writes n bytes to device
	};

sd_t* sd_ctor(SPI_HandleTypeDef *in_hspi, GPIO_TypeDef *_SPI_CS_Port, const uint16_t _SPI_CS_Pin)  //
{
	__SD._hspi = in_hspi;  // store SPI object
	__SD._CS_Port = _SPI_CS_Port;  // store CS port
	__SD._CS_Pin = _SPI_CS_Pin;  // store CS pin
	HAL_GPIO_WritePin(__SD._CS_Port, __SD._CS_Pin, GPIO_PIN_SET);  // chip select is active low

	return &__SD.public;  // set pointer to SD public part
}

sd_t *const SD = &__SD.public;  // set pointer to SD public part

#endif
