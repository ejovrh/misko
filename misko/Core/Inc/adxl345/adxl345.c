#include "adxl345/adxl345.h"

#if defined(USE_ADXL345)	// if this particular device is active

typedef struct	// adxl345c_t actual
{
	adxl345_t public;  // public struct

	uint8_t _irq_src;  //
	SPI_HandleTypeDef *_hspi;  // HAL SPI instance
	GPIO_TypeDef *_CS_Port;  // SPI chip select GPIO port
	uint16_t _CS_Pin;  // SPI chip select GPIO pin
} __adxl345_t;

static __adxl345_t __ADXL345 __attribute__ ((section (".data")));  // preallocate __ADXL345 object in .data

#define SPI_TIMEOUT 10	// SPI timeout in SysTicks
#define REG_CNT 30	// 64 registers
#define COMMAND_READ 0x80	// SPI read command, DS. p. 15, figure 37

static const uint8_t _RegisterAddress[REG_CNT] =  // address of each register address, indexed by adxl345_reg_t
	{  //
	0x00,  // DEVID
	// mind the address gap for bulk reads!
	0x1D,// THRESH_TAP
	0x1E,  // OFSX
	0x1F,  // OFSY
	0x20,  // OFSZ
	0x21,  // DUR
	0x22,  // LATENT
	0x23,  // WINDOW
	0x24,  // THRESH_ACT
	0x25,  // THRESH_INACT
	0x26,  // TIME_INACT
	0x27,  // ACT_INACT_CTL
	0x28,  // THRESH_FF
	0x29,  // TIME_FF
	0x2A,  // TAP_AXES
	0x2B,  // ACT_TAP_STATUS
	0x2C,  // BW_RATE
	0x2D,  // POWER_CTL
	0x2E,  // INT_ENABLE
	0x2F,  // INT_MAP
	0x30,  // INT_SOURCE
	0x31,  // DATA_FORMAT
	0x32,  // DATAX0
	0x33,  // DATAX1
	0x34,  // DATAY0
	0x35,  // DATAY1
	0x36,  // DATAZ0
	0x37,  // DATAZ1
	0x38,  // FIFO_CTL
	0x39,  // FIFO_STATUS
	};

// reads a byte from device
static uint8_t _ReadByte(const adxl345_reg_t in_register)
{
	uint8_t address = (_RegisterAddress[in_register] | COMMAND_READ);  // flag address with read command

	while(HAL_SPI_GetState(__ADXL345._hspi) != HAL_SPI_STATE_READY)
		;  // guard for bus-ready condition

	HAL_GPIO_WritePin(__ADXL345._CS_Port, __ADXL345._CS_Pin, GPIO_PIN_RESET);  // select the slave

	HAL_SPI_Transmit_DMA(__ADXL345._hspi, &address, 1);  // first, supply the address to read from
	while(HAL_SPI_GetState(__ADXL345._hspi) != HAL_SPI_STATE_READY)
		;  // the bloody thing needs its time...

	HAL_SPI_Receive_DMA(__ADXL345._hspi, &address, 1);	// second, (invisibly) clock-in 0x00 into the bus and get data back from the circular buffer
	while(HAL_SPI_GetState(__ADXL345._hspi) != HAL_SPI_STATE_READY)
		;  // same as above

	HAL_GPIO_WritePin(__ADXL345._CS_Port, __ADXL345._CS_Pin, GPIO_PIN_SET);  // de-select the slave

	return address;  // return received data
}

// writes a byte to device
static void _WriteByte(const adxl345_reg_t in_register, const uint8_t data)
{
	uint8_t address = _RegisterAddress[in_register];

	while(HAL_SPI_GetState(__ADXL345._hspi) != HAL_SPI_STATE_READY)
		;  // guard for bus-ready condition

	HAL_GPIO_WritePin(__ADXL345._CS_Port, __ADXL345._CS_Pin, GPIO_PIN_RESET);  // select the slave

	HAL_SPI_Transmit_DMA(__ADXL345._hspi, &address, 1);  // send register address to write to
	while(HAL_SPI_GetState(__ADXL345._hspi) != HAL_SPI_STATE_READY)
		;  // the bloody thing needs its time...

	HAL_SPI_Transmit_DMA(__ADXL345._hspi, &data, 1);  // send data
	while(HAL_SPI_GetState(__ADXL345._hspi) != HAL_SPI_STATE_READY)
		;  // same as above

	HAL_GPIO_WritePin(__ADXL345._CS_Port, __ADXL345._CS_Pin, GPIO_PIN_SET);  // de-select the slave
}

//
void _ISR(void)  // ISR for the ADXL345 accelerometer
{
	__ADXL345._irq_src = _ReadByte(INT_SOURCE);  // read interrupt source & thereby clear pending interrupts

	if(((__ADXL345._irq_src >> 3) & 0x01) == 0x01)  // if the inactivity bit is set
		{
			_ReadByte(INT_SOURCE);  // clear any pending interrupts
			_WriteByte(INT_ENABLE, 0x00);  // disable interrupts
			_WriteByte(INT_ENABLE, INT_ENABLE_ACT_CFG);  // configure for activity detection

			HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_RESET);  // turn LED off
		}

	if(((__ADXL345._irq_src >> 4) & 0x01) == 0x01)  // if the activity bit is set
		{
			_ReadByte(INT_SOURCE);  // clear any pending interrupts
			_WriteByte(INT_ENABLE, 0x00);  // disable interrupts
			_WriteByte(INT_ENABLE, INT_ENABLE_INACT_CFG);  // reconfigure for inact. detection
			_ReadByte(INT_SOURCE);  // enable interrupts

			HAL_GPIO_WritePin(LED_Green_GPIO_Port, LED_Green_Pin, GPIO_PIN_SET);  // turn LED on
		}
}

static __adxl345_t __ADXL345 =  // instantiate adxl345c_t actual and set function pointers
	{  //
	.public.ISR = &_ISR,	// device ISR
	.public.ReadByte = &_ReadByte,	// reads a byte from device
	.public.WriteByte = &_WriteByte  // writes a byte to device
	};

adxl345_t* adxl345_ctor(SPI_HandleTypeDef *in_hspi, GPIO_TypeDef *_SPI_CS_Port, const uint16_t _SPI_CS_Pin)  //
{
	__ADXL345._hspi = in_hspi;  // store SPI object
	__ADXL345._CS_Port = _SPI_CS_Port;	// store CS port
	__ADXL345._CS_Pin = _SPI_CS_Pin;  // store CS pin
	HAL_GPIO_WritePin(__ADXL345._CS_Port, __ADXL345._CS_Pin, GPIO_PIN_SET);  // chip select is active low

// basic accelerometer device initialisation & configuration
	_ReadByte(INT_SOURCE);	// clear any pending interrupts

	_WriteByte(INT_ENABLE, 0x00);  // disable all interrupts
	_WriteByte(POWER_CTL, POWER_CTL_CFG_OFF);  // all off
	_WriteByte(DATA_FORMAT, DATA_FORMAT_CFG);  // 16g range, full resolution, active high interrupt
	_WriteByte(BW_RATE, BW_RATE_CFG);  // low power, output data rate 6.25Hz
	_WriteByte(THRESH_ACT, 0x4);	// 62.5mg/LSB
	_WriteByte(THRESH_INACT, 0x4);	// 62.5mg/LSB
	_WriteByte(TIME_INACT, 0x02);  // 1s/LSB
	_WriteByte(ACT_INACT_CTL, ACT_INACT_CTL_CFG);  // all axes in AC mode
	_WriteByte(INT_ENABLE, INT_ENABLE_INACT_CFG);  // enable interrupts only for inact. detection
	_WriteByte(INT_MAP, 0x00);	// route all to INT1

	_WriteByte(POWER_CTL, POWER_CTL_CFG);  // power on and start

	HAL_NVIC_EnableIRQ(EXTI9_IRQn);  // turn on EXTI triggered by INT1 for good

	return &__ADXL345.public;  // set pointer to ADXL345 public part
}

adxl345_t *const ADXL345 = &__ADXL345.public;  // set pointer to ADXL345 public part

#endif
