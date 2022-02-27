#include "FM25W256.h"

#include "spi.h"

typedef struct  // fm25w256_t actual
{
	fm25w256_t public;  // public struct
} __fm25w256_t;

static __fm25w256_t   __FM25W256   __attribute__ ((section (".data")));  // preallocate __fm25w256 object in .data

static uint8_t _ReadString(const uint16_t in_addr, uint8_t *out_data, const uint8_t in_len)  // reads a length of string from from address
{
	if(in_addr >= 0x7FFF)  // check if we got the last valid address - data sheet p. 8
		return -1;  // quit, return something, don't continue...

	uint8_t cmd_read = CMD_READ;  // read enable opcode

	uint8_t upper_byte = (in_addr >> 8) & 0xFF;  // first split 2 byte into 2 x 1byte
	uint8_t lower_byte = in_addr & 0xFF;

	__disable_irq();  // atomic start

	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;  // configure for SPI mode 3
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	__HAL_SPI_ENABLE(&hspi1);  // enable peripheral

	HAL_GPIO_WritePin(SPI1_SS_MR25H256_GPIO_Port, SPI1_SS_MR25H256_Pin, GPIO_PIN_RESET);  // drive device CS low
	HAL_SPI_Transmit(&hspi1, &cmd_read, 1, 100);  // send read command
	HAL_SPI_Transmit(&hspi1, &upper_byte, 1, 100);  // send upper address byte
	HAL_SPI_Transmit(&hspi1, &lower_byte, 1, 100);  // send lower address byte
	HAL_SPI_Receive(&hspi1, out_data, in_len, 100);  // get data
	HAL_GPIO_WritePin(SPI1_SS_MR25H256_GPIO_Port, SPI1_SS_MR25H256_Pin, GPIO_PIN_SET);	// drive device CS high

	__HAL_SPI_DISABLE(&hspi1);  // disable peripheral
	__enable_irq();  // atomic end

	return 0;
}

static uint8_t _WriteString(const uint16_t in_addr, uint8_t *in_data, const uint8_t in_len)  // writes a length of string from from address
{
	if(in_addr >= 0x7FFF)  // check if we got the last valid address - data sheet p. 8
		return -1;  // quit, return something, don't continue...

	uint8_t cmd_wren = CMD_WREN;  // write enable opcode
	uint8_t cmd_write = CMD_WRITE;  // write opcode

	uint8_t upper_byte = (in_addr >> 8) & 0xFF;  // first split 2 byte into 2 x 1byte
	uint8_t lower_byte = in_addr & 0xFF;

	__disable_irq();  // atomic start

	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;  // configure for SPI mode 3
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	__HAL_SPI_ENABLE(&hspi1);  // enable peripheral

	HAL_GPIO_WritePin(SPI1_SS_MR25H256_GPIO_Port, SPI1_SS_MR25H256_Pin, GPIO_PIN_RESET);  // drive device CS low
	HAL_SPI_Transmit(&hspi1, &cmd_wren, 1, 100);  // issue WREN opcode to enable writes on the device
	HAL_GPIO_WritePin(SPI1_SS_MR25H256_GPIO_Port, SPI1_SS_MR25H256_Pin, GPIO_PIN_SET);	// drive device CS high

	HAL_GPIO_WritePin(SPI1_SS_MR25H256_GPIO_Port, SPI1_SS_MR25H256_Pin, GPIO_PIN_RESET);  // drive device CS low
	HAL_SPI_Transmit(&hspi1, &cmd_write, 1, 100);  // issue WRITE opcode to enable writes on the device
	HAL_SPI_Transmit(&hspi1, &upper_byte, 1, 100);  // send upper address byte
	HAL_SPI_Transmit(&hspi1, &lower_byte, 1, 100);  // send lower address byte
	HAL_SPI_Transmit(&hspi1, in_data, in_len, 100);  // write data
	HAL_GPIO_WritePin(SPI1_SS_MR25H256_GPIO_Port, SPI1_SS_MR25H256_Pin, GPIO_PIN_SET);	// drive device CS high

	__HAL_SPI_DISABLE(&hspi1);  // disable peripheral
	__enable_irq();  // atomic end

	return 0;
}

fm25w256_t* fm25w256_ctor(void)  // object constructor
{
	__FM25W256.public.ReadString = &_ReadString;  // set function pointer
	__FM25W256.public.WriteString = &_WriteString;  //	ditto

	return &__FM25W256.public;	// return address of public part; calling code accesses it via pointer
}
