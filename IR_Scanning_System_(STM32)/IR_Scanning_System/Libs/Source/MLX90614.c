
#include "MLX90614.h"



void MLX9061x_ReadReg(unsigned char slave_addr, unsigned char RamAddr, float *pTemp)
{
	unsigned char ReadBuff[3];

	I2C2_Start(slave_addr, 0);
	
	I2C2_SendBuff(&RamAddr, 1);
	
	I2C2_Start(slave_addr, 1);
		
	I2C2_ReadBuffAndStop(ReadBuff, 3);
	

	unsigned int Temp16 = ReadBuff[0] | (((unsigned int)ReadBuff[1]) << 8);
	
	*pTemp = (float)(Temp16 - 13658) / 50;         // 273.16 * 50 = 13658
}

