
#ifndef MLX90614_H_
#define MLX90614_H_

#include "I2C_Lib.h"




unsigned char mlx90614_crc8_byte(unsigned char Data, unsigned char Crc);

unsigned char mlx90614_crc8_buff(unsigned char *p, unsigned char len);

void mlx9061x_ReadReg(unsigned char slave_addr, unsigned char RamAddr, float *pTemp);






#endif