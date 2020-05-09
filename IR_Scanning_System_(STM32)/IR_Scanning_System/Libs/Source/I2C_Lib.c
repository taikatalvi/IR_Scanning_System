
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


int8_t I2C2_Start(uint8_t slave_addr, uint8_t IsRead, uint16_t TimeOut)
{
	uint16_t timeout = TimeOut;
		
	I2C2->CR1 |= I2C_CR1_START;
	
	// Wait until I2C_EVENT_MASTER_MODE_SELECT
	while (!((I2C2->SR1 & I2C_SR1_SB) && (I2C2->SR2 & I2C_SR2_BUSY) && (I2C2->SR2 & I2C_SR2_MSL)) && timeout--) ;
	
	(void)I2C2->SR1;
	
	if (!timeout) return I2C_ERR_HWerr;
	
	timeout = TimeOut;
	
	if (IsRead)
	{
		I2C2->DR = slave_addr | 1;
		
		// Wait until I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED
		while (!((I2C2->SR1 & I2C_SR1_ADDR) && (I2C2->SR2 & I2C_SR2_BUSY) && (I2C2->SR2 & I2C_SR2_MSL)) && timeout--) ;
	}
	else
	{
		I2C2->DR = slave_addr;
		
		// Wait until I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED		
		while (!((I2C2->SR1 & I2C_SR1_ADDR) && (I2C2->SR1 & I2C_SR1_TXE) && (I2C2->SR1 & I2C_SR2_TRA) && (I2C2->SR2 & I2C_SR2_BUSY) && (I2C2->SR2 & I2C_SR2_MSL)) && timeout--) ;
	}
  
	(void)I2C2->SR1;
	(void)I2C2->SR2;
	
	if (!timeout) return I2C_ERR_NotConnect;
	
	return I2C_ERR_Ok;
}


int8_t I2C2_SendByte(const uint8_t* data, uint16_t TimeOut)
{
	uint16_t timeout = TimeOut;
	
	I2C2->DR = *data;
	
	while ((!(I2C2->SR1 & I2C_SR1_BTF)) && timeout)
		timeout--;
	
	if (!timeout) return I2C_ERR_NotConnect;
	
	return I2C_ERR_Ok;
}

int8_t I2C2_SendBuff(const uint8_t* pbuf, uint32_t len, uint16_t TimeOut)
{
	int8_t err = 0;
	
	while (len--) 
	{	
		err = I2C2_SendByte(pbuf++, TimeOut);
		
		if (err) return I2C_ERR_NotConnect;
	}
	
	return I2C_ERR_Ok;
}

uint8_t I2C2_ReadByteACK(uint16_t* TimeOut)
{
	I2C2->CR1 |= I2C_CR1_ACK;
	
	while (!(I2C2->SR1 & I2C_SR1_BTF)) ;
	
	return I2C2->DR;	
}

uint8_t I2C2_ReadByteNACK(uint16_t* TimeOut)
{
	I2C2->CR1 &= ~I2C_CR1_ACK;
	
	while (!(I2C2->SR1 & I2C_SR1_BTF)) ;
	
	return I2C2->DR;
}

int8_t I2C2_ReadBuffAndStop(uint8_t *pbuf, uint32_t len, uint16_t TimeOut)
{
	uint16_t timeout = TimeOut;
	I2C2->CR1 |= I2C_CR1_ACK;
	
	if (1 == len)
	{
		I2C2->CR1 &= ~I2C_CR1_ACK;
		
		__disable_irq();
		(void) I2C2->SR2;                                                  
		I2C2->CR1 |= I2C_CR1_STOP;
		__enable_irq();

		timeout = TimeOut;
		
		while ((!(I2C2->SR1 & I2C_SR1_RXNE)) && timeout--);
		
		*pbuf++ = (uint8_t)I2C2->DR;
	}
	else if (2 == len)
	{
		I2C2->CR1 |= I2C_CR1_POS;
		
		__disable_irq();
		(void) I2C2->SR2;  // Clear ADDR flag
		I2C2->CR1 &= ~I2C_CR1_ACK;  
		__enable_irq();

		timeout = TimeOut;
		
		while ((!(I2C2->SR1 & I2C_SR1_BTF)) && timeout--);
		
		__disable_irq();
		I2C2->CR1 |= I2C_CR1_STOP;
		*pbuf++ = (uint8_t)I2C2->DR;
		__enable_irq();

		*pbuf++ = (uint8_t)I2C2->DR;		
	}
	else 
	{
		(void) I2C2->SR2;
		
		while(len-- != 3)
		{
			timeout = TimeOut;
			while ((!(I2C2->SR1 & I2C_SR1_BTF)) && timeout--);
			*pbuf++ = (uint8_t)I2C2->DR;
		}

		timeout = TimeOut;
		while ((!(I2C2->SR1 & I2C_SR1_BTF)) && timeout--);

		__disable_irq();
		*pbuf++ = (uint8_t)I2C2->DR;
		I2C2->CR1 |= I2C_CR1_STOP;
		__enable_irq();

		*pbuf++ = (uint8_t)I2C2->DR;

		I2C2->CR1 &= ~I2C_CR1_ACK; 

		timeout = TimeOut;
		
		while ( !( (I2C2->SR1 & I2C_SR1_RXNE) && (I2C2->SR2 & I2C_SR2_BUSY) && (I2C2->SR2 & I2C_SR2_MSL) ) && timeout--) ;
		
		*pbuf++ = (uint8_t)I2C2->DR; // Receive PEC

		len = 0;
	}
	
	timeout = TimeOut;
	
	while ((!(I2C2->SR1 & I2C_SR1_STOPF)) && timeout--);
	
	if (!timeout) return I2C_ERR_HWerr;
	
  
	return I2C_ERR_Ok;
}