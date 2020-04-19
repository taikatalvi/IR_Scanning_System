
#ifndef I2C_LIB_H_
#define I2C_LIB_H_

#include "stm32f1xx.h"

void I2C2_Init();


void I2C2_SendByte(const unsigned char* data);


void I2C2_Start(unsigned char slave_addr, unsigned char IsRead);
void I2C2_Stop();

void I2C2_SendBuff(unsigned char *pbuf, unsigned int len);

unsigned char I2C2_ReadByteACK();
unsigned char I2C2_ReadByteNACK(); 

void I2C2_ReadBuffAndStop(unsigned char *pbuf, unsigned int len);


#endif
