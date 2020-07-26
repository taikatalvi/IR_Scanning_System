
#include "MLX90614.h"


uint8_t MLX90614_CRC8_Byte(uint8_t Data, uint8_t Crc)
{
	Crc ^= Data;
	for (uint8_t i = 0; i < 8; i++) 
		Crc = Crc & 0x80 ? Crc << 1 ^ 0x07 : Crc << 1;
  
	return Crc;
}


uint8_t MLX90614_CRC8_Buffer(uint8_t *p, uint8_t len)
{
	uint8_t crc = 0;

	while (len--)
		crc = MLX90614_CRC8_Byte(*(p++), crc);

	return crc;
}


int8_t MLX9061x_ReadReg(uint8_t slave_addr, uint8_t RamAddr, float *pTemp)
{
	int8_t err;
	uint8_t crc = 0;
	uint8_t ReadBuff[3];

	err = I2C2_Start(MLX9061X_ADDR, 0, MLX9061X_TimeOut);
	crc = MLX90614_CRC8_Byte(MLX9061X_ADDR, crc);
	if (err) return err;
	
	
	err = I2C2_SendBuff(&RamAddr, 1, MLX9061X_TimeOut);
	crc = MLX90614_CRC8_Byte(RamAddr, crc);
	if (err) return err;

	
	err = I2C2_Start(MLX9061X_ADDR, 1, MLX9061X_TimeOut);
	crc = MLX90614_CRC8_Byte(MLX9061X_ADDR | 1, crc);
	if (err) return err;
  
	
	err = I2C2_ReadBuffAndStop(ReadBuff, 3, MLX9061X_TimeOut);
	if (err) return err;
  
	for(uint8_t i = 0 ; i < 3 ; i++)
	  crc = MLX90614_CRC8_Byte(ReadBuff[i], crc);
	
	if (crc) return MLX9061X_ERR_BadChksum;
  
	uint16_t Temp16 = ReadBuff[0] | (((uint16_t)ReadBuff[1]) << 8);
  
	if (Temp16 & 0x8000) return MLX9061X_ERR_MeasErr;
  
	*pTemp = (float)(Temp16 - 13658) / 50;        // 273.16 * 50 = 13658
  
	return MLX9061X_ERR_Ok;
}

int8_t MLX9061x_ReadTa(uint8_t slave_addr, float *pTemp)
{
	return MLX9061x_ReadReg(slave_addr, 6, pTemp);	
}


int8_t MLX9061x_ReadTo1(uint8_t slave_addr, float *pTemp)
{
	return MLX9061x_ReadReg(slave_addr, 7, pTemp);	
}


int8_t MLX9061x_ReadTo2(uint8_t slave_addr, float *pTemp)
{
	return MLX9061x_ReadReg(slave_addr, 8, pTemp);
}