
#include "MLX90614.h"


unsigned char mlx90614_crc8_byte(unsigned char Data, unsigned char Crc)
{
	Crc ^= Data;
	for (unsigned char i = 0; i < 8; i++)
		Crc = Crc & 0x80 ? Crc << 1 ^ 0x07 : Crc << 1;
	
	return Crc;
}

unsigned char mlx90614_crc8_buff(unsigned char *p, unsigned char len)
{
	unsigned char crc = 0;

	while (len--)
		crc = mlx90614_crc8_byte(*(p++), crc);

	return crc;
}

void mlx9061x_ReadReg(unsigned char slave_addr, unsigned char RamAddr, float *pTemp)
{
	unsigned char ReadBuff[3];

	// ����� ������ START �� ����
	i2c_Start(slave_addr, 0);
	
	// ����� ������� (���������� �����) � mlx9061x
	i2c_WriteBuff(&RamAddr, 1);
	
	// ����� ��������� START ����� ������� ������
	i2c_Start(slave_addr, 1);
	
	// ������ 2 ����� � CRC (PEC)
	i2c_ReadBuffAndStop(ReadBuff, 3);
	
	
	// ������������� � ���. ��������
	unsigned int Temp16 = ReadBuff[0] | (((unsigned int)ReadBuff[1]) << 8);
	
	*pTemp = (float)(Temp16 - 13658) / 50;         // 273.16 * 50 = 13658
}