
#ifndef MLX90614_H_
#define MLX90614_H_

#include "I2C_Lib.h"




void MLX9061x_ReadReg(unsigned char slave_addr, unsigned char RamAddr, float *pTemp);






#endif