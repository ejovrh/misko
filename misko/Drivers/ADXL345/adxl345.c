#include "ADXL345.h"

#include <spi.h>

extern void SystemClock_Config(void);
extern UART_HandleTypeDef huart2;

typedef struct  // adxl345_t actual
{
	volatile uint8_t _adxl345_irq_src;  // holds the interrupt source bitmask

	adxl345_t public;  // public struct
} __adxl345_t;

static __adxl345_t  __ADXL345  __attribute__ ((section (".data")));  // preallocate __adxl345 object in .data

static uint8_t _ReadByte(uint8_t in_addr)  // reads one byte of data from address
{
	in_addr |= 0x80;  // mark as read command

	__disable_irq();  // atomic start

	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;  // configure for SPI mode 3
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	__HAL_SPI_ENABLE(&hspi1);  // enable peripheral

	HAL_GPIO_WritePin(SPI1_SS_ADXL345_GPIO_Port, SPI1_SS_ADXL345_Pin, GPIO_PIN_RESET);  // drive device CS low
	HAL_SPI_Transmit(&hspi1, &in_addr, 1, 100);  // send address
	HAL_SPI_Receive(&hspi1, &in_addr, 1, 100);  // hopefully get something back
	HAL_GPIO_WritePin(SPI1_SS_ADXL345_GPIO_Port, SPI1_SS_ADXL345_Pin, GPIO_PIN_SET);	// drive device CS high

	__HAL_SPI_DISABLE(&hspi1);  // disable peripheral
	__enable_irq();  // atomic end

	return (in_addr);  // return data
}

static void _WriteByte(uint8_t in_addr, uint8_t in_data)  // writes one byte of data to address
{
	__disable_irq();  // atomic start

	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;  // configure for SPI mode 3
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	__HAL_SPI_ENABLE(&hspi1);  // enable peripheral

	HAL_GPIO_WritePin(SPI1_SS_ADXL345_GPIO_Port, SPI1_SS_ADXL345_Pin, GPIO_PIN_RESET);	// drive device CS low
	HAL_SPI_Transmit(&hspi1, &in_addr, 1, 100);  // send address
	HAL_SPI_Transmit(&hspi1, &in_data, 1, 100);  // send data
	HAL_GPIO_WritePin(SPI1_SS_ADXL345_GPIO_Port, SPI1_SS_ADXL345_Pin, GPIO_PIN_SET);	// drive device CS high

	__HAL_SPI_DISABLE(&hspi1);  // disable peripheral
	__enable_irq();  // atomic end
}

void _ISR(void)  // ISR for the ADXL345 accelerometer
{
	__ADXL345._adxl345_irq_src = _ReadByte(INT_SOURCE);  // read interrupt source & thereby clear pending interrupts

	if((__ADXL345._adxl345_irq_src >> 3) & 0x01)  // if the inact. bit is set
		{
			_ReadByte(INT_SOURCE);  // clear any pending interrupts
			_WriteByte(INT_ENABLE, 0x00);  // disable interrupts
			_WriteByte(INT_ENABLE, INT_ENABLE_ACT_CFG);  // configure for activity detection

			__ADXL345.public.FlagStop = 1;  // flag for sleep mode - is evaluated in main()
		}

	if((__ADXL345._adxl345_irq_src >> 4) & 0x01)  // if the act bit is set
		{
			SystemClock_Config();  // restart the clocks
			HAL_ResumeTick();  // resume systick
			HAL_PWR_DisableSleepOnExit();  // disable sleep

			_ReadByte(INT_SOURCE);  // clear any pending interrupts
			_WriteByte(INT_ENABLE, 0x00);  // disable interrupts
			_WriteByte(INT_ENABLE, INT_ENABLE_CFG);  // reconfigure for act./inact. detection
			_ReadByte(INT_SOURCE);  // enable interrupts

			HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);  // turn LED on

			while(HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY)
				;
			HAL_UART_Transmit_IT(&huart2, (uint8_t*) "accel. wakeup\r\n", 15);  // indicate via UART message
		}
}

adxl345_t* adxl345_ctor(void)  // object constructor
{
	__ADXL345.public.ReadByte = &_ReadByte;  // set function pointer
	__ADXL345.public.WriteByte = &_WriteByte;  //	ditto
	__ADXL345.public.ISR = &_ISR;  //	ditto
	__ADXL345.public.FlagStop = 0;  // indicate no stop mode

// accelerometer device initialisation & configuration
	_ReadByte(INT_SOURCE);	// clear any pending interrupts

	_WriteByte(INT_ENABLE, 0x00);  // disable all interrupts
	_WriteByte(POWER_CTL, POWER_CTL_CFG_OFF);  // all off
	_WriteByte(DATA_FORMAT, DATA_FORMAT_CFG);  // 16g range, full resolution, active high interrupt
	_WriteByte(BW_RATE, BW_RATE_CFG);  // low power, output data rate 6.25Hz
	_WriteByte(THRESH_ACT, 0x4);	// 62.5mg/LSB
	_WriteByte(THRESH_INACT, 0x4);	// 62.5mg/LSB
	_WriteByte(TIME_INACT, 0x02);  // 1s/LSB
	_WriteByte(ACT_INACT_CTL, ACT_INACT_CTL_CFG);  // all axes in AC mode
	_WriteByte(INT_ENABLE, INT_ENABLE_CFG);  // enable interrupts only for act./inact. detection
	_WriteByte(INT_MAP, 0x00);	// route all to INT1

	_WriteByte(POWER_CTL, POWER_CTL_CFG);  // power on and start

	return &__ADXL345.public;  // return address of public part; calling code accesses it via pointer
}
