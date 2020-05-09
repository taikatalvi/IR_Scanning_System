
#ifndef I2C_LIB_H_
#define I2C_LIB_H_

#include "stm32f1xx.h"

#define I2C_ERR_Ok         0
#define I2C_ERR_NotConnect -1
#define I2C_ERR_BadChksum  -2
#define I2C_ERR_HWerr      -3

void I2C2_Init();


int8_t I2C2_SendByte(const uint8_t* data, uint16_t TimeOut);


int8_t I2C2_Start(uint8_t slave_addr, uint8_t IsRead, uint16_t TimeOut);

int8_t I2C2_SendBuff(const uint8_t* pbuf, uint32_t len, uint16_t TimeOut);

uint8_t I2C2_ReadByteACK();
uint8_t I2C2_ReadByteNACK(); 

int8_t I2C2_ReadBuffAndStop(uint8_t *pbuf, uint32_t len, uint16_t TimeOut);


#endif
