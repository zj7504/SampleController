#ifndef __USART1_H
#define	__USART1_H

#include "stm32f10x.h"
#include <stdio.h>
#include  <os.h>


#define USART_BUFF_SIZE     128
#define USART2_BUFF_SIZE	128

extern volatile uint8_t	usart1_p;
extern volatile uint8_t	Usart1_Send_Num;
extern volatile uint8_t	Usart1_Received;
extern uint8_t Usart1_Tx_buff[USART_BUFF_SIZE];
extern uint8_t	Usart1_Rx_buff[USART_BUFF_SIZE];
//extern uint8_t Decode_buff[USART_BUFF_SIZE];

extern volatile uint32_t usart2_p;
extern volatile uint8_t	Usart2_Send_Num;
extern volatile uint8_t	Usart2_Received;
extern uint8_t		Usart2_DJI_buff[USART2_BUFF_SIZE];
extern uint8_t		Usart2_Rx_buff[USART2_BUFF_SIZE];

extern volatile uint8_t	usart3_p;
extern volatile uint8_t	Usart3_Send_Num;
extern volatile uint8_t	Usart3_Received;
extern uint8_t Usart3_Tx_buff[USART_BUFF_SIZE];
extern uint8_t	Usart3_Rx_buff[USART_BUFF_SIZE];

void USART1_Config(void);
void USART2_Config(void);
void USART1_Init(void);
void USART2_Init(void);
void USART3_Init(void);

void USART1_DMA_Send(uint32_t addess, uint32_t size);
void USART3_DMA_Send(uint32_t addess, uint32_t size);

void Uart1_PutChar(uint8_t ch);





//int fputc(int ch, FILE *f);
//void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...);

#endif /* __USART1_H */
