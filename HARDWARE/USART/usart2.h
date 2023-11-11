#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 


#define USART_DEBUG		USART1		//调试打印所使用的串口组


void Usart2_Init(unsigned int baud);

void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);

void Usart_SendString2( USART_TypeDef * pUSARTx, char *str);

void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);


#endif



