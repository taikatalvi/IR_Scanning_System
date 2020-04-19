
#include "I2C_Lib.h"

void I2C2_Init()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;  // SDL -> PB10, SDA -> PB11
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	
	GPIOB->CRH |= GPIO_CRH_MODE10; // Output, speed 50 MHz
	GPIOB->CRH |= GPIO_CRH_MODE11;
	
	GPIOB->CRH |= GPIO_CRH_CNF10; // Alternate function output Open-drain
	GPIOB->CRH |= GPIO_CRH_CNF11;
	
	I2C2->OAR2 &= ~I2C_OAR2_ENDUAL;
	I2C2->CR1 &= ~I2C_CR1_ENGC;
	
	I2C2->CR1 &= ~I2C_CR1_NOSTRETCH;
	
	I2C2->CR1 &= ~I2C_CR1_PE;     // Disable I2C peripheral
	
	// Setting clock speed
	I2C2->CR2 |= (I2C_CR2_FREQ_2 | I2C_CR2_FREQ_5); // Set 36 MHz -> Tpclck = 28 ns
	I2C2->TRISE &= ~I2C_TRISE_TRISE;
	I2C2->TRISE |= 0x25; // 36 + 1
	
	I2C2->CCR &= ~I2C_CCR_FS;
	I2C2->CCR |= 180; // 5000 ns / 28 ns
	
	I2C2->OAR1 &= ~I2C_OAR1_ADDMODE;
	
	I2C2->CR1 &= ~(I2C_CR1_SMBUS | I2C_CR1_SMBTYPE | I2C_CR1_ENARP);    // I2C mode
	
	I2C2->CR1 |= I2C_CR1_PE;      // Enable I2C peripheral
	
	I2C2->CR1 |= I2C_CR1_ACK;
	
	I2C2->OAR2 &= I2C_OAR2_ADD2; 
}


void I2C2_SendByte(const unsigned char* data)
{
	I2C2->DR = *data;
	while (!(I2C2->SR1 & I2C_SR1_TXE)) ;
}


void I2C2_Start(unsigned char slave_addr, unsigned char IsRead)
{
	I2C2->CR1 &= ~I2C_CR1_POS;
	
	I2C2->CR1 |= I2C_CR1_START;
	
	while (!(I2C2->SR1 & I2C_SR1_SB)); // Wait until START is set
	
	(void)I2C2->SR1;
	
	I2C2->DR = IsRead ? slave_addr | 1 : slave_addr; // Sending slave address
	
	while (!(I2C2->SR1 & I2C_SR1_ADDR));
	
	(void)I2C2->SR1;
	(void)I2C2->SR2;	
}


void I2C2_Stop()
{
	I2C2->CR1 |= I2C_CR1_STOP;
}

void I2C2_SendBuff(unsigned char *pbuf, unsigned int len)
{
	while (len--) I2C2_SendByte(pbuf++);
}

unsigned char I2C2_ReadByteACK()
{
	I2C2->CR1 |= I2C_CR1_ACK;
	
	while(!(I2C2->SR1 & I2C_SR1_RXNE));
	
	return I2C2->DR;	
}

unsigned char I2C2_ReadByteNACK()
{
	I2C2->CR1 &= ~I2C_CR1_ACK;
	
	while (!(I2C2->SR1 & I2C_SR1_RXNE));
	
	return I2C2->DR;
}

void I2C2_ReadBuffAndStop(unsigned char *pbuf, unsigned int len)
{
	while (len--)
	{
		if (len)
			*(pbuf++) = I2C2_ReadByteACK();
		else
			*(pbuf++) = I2C2_ReadByteNACK();
	}
	
	I2C2_Stop();
}