#include "fm25w256/fm25w256.h"

#if defined(USE_FM25W256)	// if this particular device is active

typedef struct	// fm25w256c_t actual
{
	fm25w256_t public;  // public struct

	SPI_HandleTypeDef *_hspi;  // HAL SPI instance
	GPIO_TypeDef *_CS_Port;  // SPI chip select GPIO port
	uint16_t _CS_Pin;  // SPI chip select GPIO pin
} __fm25w256_t;

static __fm25w256_t  __FM25W256  __attribute__ ((section (".data")));  // preallocate __FM25W256 object in .data

#define CMD_WRSR	0x01	// Write Status Register; sets write protection features (p. 7)
#define CMD_WRITE	0x02	// Write Memory Data; writes one byte to a 2 byte address (p. 8)
#define CMD_READ	0x03	// Read Memory Data; reads one byte from a 2 byte address (p. 8)
#define CMD_WRDI	0x04	// Clear Write Enable Latch; puts module in read-only mode (p. 6)
#define CMD_RDSR	0x05	// Read Status Register; reads write protection features (p. 7)
#define CMD_WREN	0x06	// Set Write Enable Latch; disabled on power on (p. 6)

// reads a length of string from address
static void _ReadBytes(const uint16_t in_addr, uint8_t *out_data, const uint8_t in_len)
{
	if(in_addr >= 0x7FFF)  // check if we got the last valid address - data sheet p. 8
		return;  // quit, return something, don't continue...

	uint8_t cmd_read = CMD_READ;  // read enable opcode
	uint8_t upper_byte = (uint8_t) ((in_addr >> 8) & 0xFF);  // first split 2 byte into 2 x 1byte
	uint8_t lower_byte = (uint8_t) (in_addr & 0xFF);

	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // guard for bus-ready condition

	HAL_GPIO_WritePin(__FM25W256._CS_Port, __FM25W256._CS_Pin, GPIO_PIN_RESET);  // drive device CS low

	HAL_SPI_Transmit_DMA(__FM25W256._hspi, &cmd_read, 1);  // send read command
	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // the bloody thing needs its time...

	HAL_SPI_Transmit_DMA(__FM25W256._hspi, &upper_byte, 1);  // send upper address byte
	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // same as above

	HAL_SPI_Transmit_DMA(__FM25W256._hspi, &lower_byte, 1);  // send lower address byte
	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // same as above

	HAL_SPI_Receive_DMA(__FM25W256._hspi, out_data, in_len);  // get the data
	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // same as above

	HAL_GPIO_WritePin(__FM25W256._CS_Port, __FM25W256._CS_Pin, GPIO_PIN_SET);  // drive device CS high
}

// writes a length of string from address
static void _WriteBytes(const uint16_t in_addr, uint8_t *in_data, const uint8_t in_len)
{
	if(in_addr >= 0x7FFF)  // check if we got the last valid address - data sheet p. 8
		return;  // quit, return something, don't continue...

	uint8_t cmd_wren = CMD_WREN;  // write enable opcode
	uint8_t cmd_write = CMD_WRITE;  // write opcode
	uint8_t upper_byte = (uint8_t) ((in_addr >> 8) & 0xFF);  // first split 2 byte into 2 x 1byte
	uint8_t lower_byte = (uint8_t) (in_addr & 0xFF);

	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // guard for bus-ready condition

	HAL_GPIO_WritePin(__FM25W256._CS_Port, __FM25W256._CS_Pin, GPIO_PIN_RESET);  // drive device CS low

	HAL_SPI_Transmit_DMA(__FM25W256._hspi, &cmd_wren, 1);  // issue WREN opcode to enable writes on the device
	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // the bloody thing needs its time...

	HAL_GPIO_WritePin(__FM25W256._CS_Port, __FM25W256._CS_Pin, GPIO_PIN_SET);  // drive device CS high

	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // guard for bus-ready condition
	HAL_GPIO_WritePin(__FM25W256._CS_Port, __FM25W256._CS_Pin, GPIO_PIN_RESET);  // drive device CS low

	HAL_SPI_Transmit_DMA(__FM25W256._hspi, &cmd_write, 1);  // issue WRITE opcode to enable writes on the device
	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // the bloody thing needs its time...

	HAL_SPI_Transmit_DMA(__FM25W256._hspi, &upper_byte, 1);  // send upper address byte
	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // same as above

	HAL_SPI_Transmit_DMA(__FM25W256._hspi, &lower_byte, 1);  // send lower address byte
	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // same as above

	HAL_SPI_Transmit_DMA(__FM25W256._hspi, in_data, in_len);  // write data
	while(HAL_SPI_GetState(__FM25W256._hspi) != HAL_SPI_STATE_READY)
		;  // same as above

	HAL_GPIO_WritePin(__FM25W256._CS_Port, __FM25W256._CS_Pin, GPIO_PIN_SET);  // drive device CS high
}

static __fm25w256_t  __FM25W256 =  // instantiate fm25w256c_t actual and set function pointers
	{  //
	.public.ReadBytes = &_ReadBytes,	// reads n bytes from device
	.public.WriteBytes = &_WriteBytes  // writes n bytes to device
	};

fm25w256_t* fm25w256_ctor(SPI_HandleTypeDef *in_hspi, GPIO_TypeDef *_SPI_CS_Port, const uint16_t _SPI_CS_Pin)  //
{
	__FM25W256._hspi = in_hspi;  // store SPI object
	__FM25W256._CS_Port = _SPI_CS_Port;  // store CS port
	__FM25W256._CS_Pin = _SPI_CS_Pin;  // store CS pin
	HAL_GPIO_WritePin(__FM25W256._CS_Port, __FM25W256._CS_Pin, GPIO_PIN_SET);  // chip select is active low

	return &__FM25W256.public;  // set pointer to FM25W256 public part
}

fm25w256_t *const FM25W256 = &__FM25W256.public;  // set pointer to FM25W256 public part

#endif
