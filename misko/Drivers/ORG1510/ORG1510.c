#include "ORG1510.h"
#include "main.h"
#include "misko.h"

#include "ORG1510/lwrb/lwrb.h"

extern UART_HandleTypeDef huart1;  // UART towards GPS / BT
extern UART_HandleTypeDef huart2;  // UART towards debugger
extern DMA_HandleTypeDef hdma_usart1_rx;  //
extern DMA_HandleTypeDef hdma_usart1_tx;

#define ARRAY_LEN(x) ( sizeof(x) / sizeof((x)[0]) )

typedef struct  // adxl345_t actual
{
	org1510_t public;  // public struct
} __org1510_t;

uint8_t usart_rx_dma_buffer[UART1_DMA_RX_BUFFERSIZE];
uint8_t usart_tx_dma_buffer[UART1_DMA_RX_BUFFERSIZE];
uint8_t usart_rx_rb_data[UART1_RB_BUFFERSIZE];
uint8_t usart_tx_rb_data[UART1_RB_BUFFERSIZE];
lwrb_t usart_rx_rb;
lwrb_t usart_tx_rb;
volatile size_t usart_tx_dma_current_len;

static __org1510_t                                             __ORG1510                                              __attribute__ ((section (".data")));  // preallocate __org1510 object in .data

uint8_t usart_start_tx_dma_transfer(void)
{
	uint32_t primask;
	uint8_t started = 0;

	/*
	 * First check if transfer is currently in-active,
	 * by examining the value of usart_tx_dma_current_len variable.
	 *
	 * This variable is set before DMA transfer is started and cleared in DMA TX complete interrupt.
	 *
	 * It is not necessary to disable the interrupts before checking the variable:
	 *
	 * When usart_tx_dma_current_len == 0
	 *    - This function is called by either application or TX DMA interrupt
	 *    - When called from interrupt, it was just reset before the call,
	 *         indicating transfer just completed and ready for more
	 *    - When called from an application, transfer was previously already in-active
	 *         and immediate call from interrupt cannot happen at this moment
	 *
	 * When usart_tx_dma_current_len != 0
	 *    - This function is called only by an application.
	 *    - It will never be called from interrupt with usart_tx_dma_current_len != 0 condition
	 *
	 * Disabling interrupts before checking for next transfer is advised
	 * only if multiple operating system threads can access to this function w/o
	 * exclusive access protection (mutex) configured,
	 * or if application calls this function from multiple interrupts.
	 *
	 * This example assumes worst use case scenario,
	 * hence interrupts are disabled prior every check
	 */
	primask = __get_PRIMASK();
	__disable_irq();
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) &usart_tx_rb, usart_tx_dma_current_len);
	if(usart_tx_dma_current_len == 0 && (usart_tx_dma_current_len = lwrb_get_linear_block_read_length(&usart_tx_rb)) > 0)
		{
			/* Disable channel if enabled */
			__HAL_DMA_DISABLE(&hdma_usart1_tx);
			//LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);

			/* Clear all flags */
			__HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_FLAG_TC3);
			//LL_DMA_ClearFlag_TC3(DMA1);

			__HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_FLAG_HT3);
			//LL_DMA_ClearFlag_HT3(DMA1);

			__HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_FLAG_GL3);
			//LL_DMA_ClearFlag_GI3(DMA1);

			__HAL_DMA_CLEAR_FLAG(&hdma_usart1_tx, DMA_FLAG_TE3);
			//LL_DMA_ClearFlag_TE3(DMA1);

			/* Prepare DMA data and length */
//			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, usart_tx_dma_current_len);
//			LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_3, (uint32_t) lwrb_get_linear_block_read_address(&usart_tx_rb));
			/* Start transfer */
			__HAL_DMA_ENABLE(&hdma_usart1_tx);
			//LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
			started = 1;
		}
	__set_PRIMASK(primask);
	return started;
}

void usart_process_data(const void *data, size_t len)
{
	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	memset(__ORG1510.public.NMEA, 0, NMEA_BUFFERSIZE);

	misko->free1 = lwrb_get_free(&usart_rx_rb);
	lwrb_write(&usart_rx_rb, data, len);

	misko->free2 = lwrb_get_free(&usart_rx_rb);
	len = lwrb_seek_char(&usart_rx_rb, '$');  // find first/next start of NMEA sentence

	lwrb_advance_r(&usart_rx_rb, len);  // advance read pointer to that location
	len = lwrb_seek_char(&usart_rx_rb, '\r');  // determine length of NMEA sentence (from $ until first '\r'
	lwrb_read(&usart_rx_rb, __ORG1510.public.NMEA, len);  // read it out

	__ORG1510.public.NMEA[len] = '\r';
	__ORG1510.public.NMEA[len + 1] = '\n';
	misko->free3 = lwrb_get_free(&usart_rx_rb);
	misko->FlagPrint = 1;

}

void _RX_Check(void)
{
	static size_t OldPos = 0;
	size_t NewPos = ARRAY_LEN(usart_rx_dma_buffer) - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

	if(NewPos != OldPos)  // new data
		{
			if(NewPos > OldPos)  // no overflow
				{
					usart_process_data(&usart_rx_dma_buffer[OldPos], NewPos - OldPos);
				}
			else  // overflow
				{
					usart_process_data(&usart_rx_dma_buffer[OldPos], ARRAY_LEN(usart_rx_dma_buffer) - OldPos);

					if(NewPos > 0)  // run up
						{
							usart_process_data(&usart_rx_dma_buffer[0], (NewPos - (ARRAY_LEN(usart_rx_dma_buffer) - OldPos)));
						}
				}

			OldPos = NewPos;
		}
}

void usart_send_string(const char *str)
{
	lwrb_write(&usart_tx_rb, str, strlen(str)); /* Write data to transmit buffer */
	usart_start_tx_dma_transfer();
}

//void _RX_Check(void)
//{
//	static uint16_t start_pos = 0;  // start at 0 or the previous old position (static!!)
//	uint16_t end_pos = start_pos + __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);  // figure out end position (aka. length)
//	uint8_t len;
//
//	if(end_pos > ARRAY_LEN(_rxbuffer))  // overflow
//		{
//			// two part read:
//			// 1. read from current pos. to end of buffer
//			// 2. read remainder from start of buffer
//			len = (ARRAY_LEN(_rxbuffer) - start_pos) - 1;  // length is from current pos to end of buffer minus one (zero indexed buffer)
//			lwrb_write(&usart_rx_rb, &_rxbuffer[start_pos], len);  // 1. read from start position to end of buffer
//
//			len = end_pos - len;  // subtract old (already read) length from overflowed length
//			lwrb_write(&usart_rx_rb, &_rxbuffer[0], len);  // 2. read remainder from start of buffer onwards
//
//			start_pos = len;  // set new start position to current end of read
//			return;
//		}
//	else  // no overflow
//		{
//			len = end_pos - start_pos;  // figure out length
//			lwrb_write(&usart_rx_rb, &_rxbuffer[start_pos], len);
//			start_pos = len;  // set new start pos
//			return;
//		}
//}

//void _parse(void)
//{
////	__HAL_DMA_GET_COUNTER(&hdma_usart1_rx);  // ((&hdma_usart1_rx)->Instance->CNDTR)
//
//	misko->FlagParseIncomingNMEA = 0;
//	misko->FlagPrint = 1;
//	HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);  // indicate it
//	HAL_UART_Transmit_DMA(&huart2, __ORG1510._rxbuffer, UART1RXBUFFERSIZE);  // printout
//}
org1510_t* org1510_ctor(void)  // object constructor
{
	lwrb_init(&usart_tx_rb, usart_tx_rb_data, sizeof(usart_tx_rb_data));
	lwrb_init(&usart_rx_rb, usart_rx_rb_data, sizeof(usart_rx_rb_data));

	__ORG1510.public.RX_Check = &_RX_Check;  // NMEA sentence parser
//	usart_send_string("USART DMA example: DMA HT & TC + USART IDLE LINE interrupts\r\n");
	HAL_UART_Receive_DMA(&huart1, usart_rx_dma_buffer, ARRAY_LEN(usart_rx_dma_buffer));  // start receiving NMEA sentences

	return &__ORG1510.public;  // return address of public part; calling code accesses it via pointer
}
