
#ifndef MLX90614_H_
#define MLX90614_H_

#include "I2C_Lib.h"

#define MLX9061X_ADDR       0xB4
#define MLX9061X_TimeOut    1000

// MLX90614 RAM read error constants
#define MLX9061X_ERR_Ok         I2C_ERR_Ok
#define MLX9061X_ERR_NotConnect I2C_ERR_NotConnect
#define MLX9061X_ERR_BadChksum  I2C_ERR_BadChksum
#define MLX9061X_ERR_HWerr      I2C_ERR_HWerr
#define MLX9061X_ERR_MeasErr    -10    // Reading temperature error


uint8_t MLX90614_CRC8_Byte(uint8_t Data, uint8_t Crc);
uint8_t MLX90614_CRC8_Buffer(uint8_t *p, uint8_t len);

int8_t MLX9061x_ReadReg(uint8_t slave_addr, uint8_t RamAddr, float *pTemp);

int8_t MLX9061x_ReadTa(uint8_t slave_addr, float *pTemp);
int8_t MLX9061x_ReadTo1(uint8_t slave_addr, float *pTemp);
int8_t MLX9061x_ReadTo2(uint8_t slave_addr, float *pTemp);




#endif