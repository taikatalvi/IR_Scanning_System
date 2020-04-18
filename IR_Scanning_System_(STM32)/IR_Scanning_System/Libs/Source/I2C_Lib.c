
#include "I2C_Lib.h"

void I2C2_Init()
{
	// SDL -> PB10 
	// SDA -> PB11
	
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	
	GPIOB->CRH |= GPIO_CRH_MODE10; // Output, speed 50 MHz
	GPIOB->CRH |= GPIO_CRH_MODE11;
	
	GPIOB->CRH |= GPIO_CRH_CNF10; // Alternate function output Open-drain
	GPIOB->CRH |= GPIO_CRH_CNF11;
	
	I2C2->OAR2 &= ~I2C_OAR2_ENDUAL;
	I2C2->CR1 &= ~I2C_CR1_ENGC;
	
	I2C2->CR1 &= ~I2C_CR1_PE;    // Disable I2C peripheral
	I2C2->CR1 &= ~I2C_CR1_SMBUS; // I2C mode
	
	
	
}
