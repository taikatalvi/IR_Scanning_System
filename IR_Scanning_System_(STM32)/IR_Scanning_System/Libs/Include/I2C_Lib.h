
#ifndef I2C_LIB_H_
#define I2C_LIB_H_

#include "stm32f1xx.h"

void I2C2_Init();


void i2c_Start(unsigned char slave_addr, unsigned char IsRead)
{
	// ����� ������� START
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))) ;//�������� ��������� ���� TWIN
	
	// ����� ����� ������
	TWDR = IsRead ? slave_addr | 1 : slave_addr;
	
	// �������� �������� ������
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))) ;//�������� ��������� ���� TWIN
}
void i2c_Stop()
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void i2c_WriteBuff(unsigned char *pbuf, unsigned int len)
{
	while (len--)
	{
		// ��������� ��������� ���� � ������� TWDR
		TWDR = *(pbuf++);
		// �������� ��������
		TWCR = (1 << TWINT) | (1 << TWEN);
		// ��� ��������� ��������
		
		while(!(TWCR & (1 << TWINT))); //�������� ��������� ���� TWIN
	}
}

unsigned char i2c_ReadbyteACK()                           // ������� ������ � ���� 1 ���� � ������� ACK
{
	// �������� TWI � �������������� ACK
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	
	// ��� ��������� ����� �����
	while(!(TWCR & (1 << TWINT))); //�������� ��������� ���� TWIN
	
	// ���������� ���������� �������� TWDR
	return TWDR;
}
unsigned char i2c_ReadbyteNACK()                          // ������� ������ � ���� 1 ���� � ������� NACK
{
	// �������� TWI ��� ������������� ACK
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	// ��� ��������� ����� �����
	while(!(TWCR & (1 << TWINT))); //�������� ��������� ���� TWIN
	
	// ���������� ���������� �������� TWDR
	return TWDR;
}

void i2c_ReadBuffAndStop(unsigned char *pbuf, unsigned int len)
{
	while (len--)
	{
		if (len)
			*(pbuf++) = i2c_ReadbyteACK();
		else
			*(pbuf++) = i2c_ReadbyteNACK();
	}
	
	i2c_Stop();
}


#endif
