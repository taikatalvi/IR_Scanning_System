
#include "USART_Lib.h"

void USART1_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	GPIOA->CRH |= GPIO_CRH_MODE9;    // TX
	GPIOA->CRH &= ~GPIO_CRH_CNF9;
	GPIOA->CRH |= GPIO_CRH_CNF9_1;
	
	GPIOA->CRH &= ~GPIO_CRH_MODE10;  // RX
	GPIOA->CRH |= GPIO_CRH_CNF10_0;
	
	USART1->BRR = 0x48; // 9600 (7500) - 72 MHz | 1 000 000 (72)
	
	USART1->CR1 |= USART_CR1_UE;
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	
	USART1->CR1 |= USART_CR1_RXNEIE;
	
	NVIC_EnableIRQ(USART1_IRQn);
}


void USART1_SendByte(const unsigned char* data)
{
	while(!(USART1->SR & USART_SR_TXE));
	
	USART1->DR = (*data);
}

void USART1_SendChar(const char* data)
{
	while (!(USART1->SR & USART_SR_TXE));
	
	USART1->DR = *data;
}

void USART1_SendShort(const uint16_t* data)
{
	while (!(USART1->SR & USART_SR_TXE)) ;
	USART1->DR = (*data) >> 8;
	
	while (!(USART1->SR & USART_SR_TXE)) ;
	USART1->DR = *data;
}

void USART1_SendString(const char* str)
{
	size_t str_len = strlen(str);
	
	for (size_t i = 0; i < str_len; i++)
		USART1_SendChar(&str[i]);
}

unsigned char USART1_GetByte()
{
	while (!(USART1->SR & USART_SR_RXNE)) ;
	return USART1->DR;
}

char USART1_GetChar()
{
	while(!(USART1->SR & USART_SR_RXNE));
	
	return (char)USART1->DR;
}


void USART1_IRQHandler()
{
	static uint8_t counter = 0;
	
	if (USART1->SR & USART_SR_RXNE)
	{		
		char data = (char)USART1->DR;
				
		if (data != '\n')
		{
			str_buffer[counter] = data;
			counter++;
		}
		else
		{
			str_buffer[counter] = data;
		
			Command_Handler();
					
			counter = 0;
			memset(str_buffer, 0, sizeof(unsigned char) * STR_BUFFER_SIZE);
		}
	}
}


