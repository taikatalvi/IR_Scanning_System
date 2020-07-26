
#ifndef USART_LIB_H_
#define USART_LIB_H_

#include "stm32f1xx.h"
#include <string.h>

#define STR_BUFFER_SIZE 64
#define STR_COMMAND_SIZE 10
char str_buffer[STR_BUFFER_SIZE];
char strcom_buffer[STR_COMMAND_SIZE];

void USART1_Init();

void USART1_SendByte(const unsigned char*);
void USART1_SendChar(const char*);
void USART1_SendShort(const uint16_t*);
void USART1_SendString(const char*);

unsigned char USART1_GetByte();
char USART1_GetChar();

void Command_Handler();



#endif /* USART_LIB_H_ */
