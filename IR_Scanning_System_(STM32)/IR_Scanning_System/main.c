
#include "main.h"

__IO uint8_t PrevXferComplete = 1;

#define SERVO_180 125
#define SERVO_0 25


void delay() 
{
	volatile uint32_t i;
	for (i = 0; i != 1500000; i++) ; // ~ 1 s
}

int main()
{
	RCC_Init();
	GPIO_Init();
	USART1_Init();
	Timer4_Init();
	
	// Configuring USB
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
	
	float Amb_Data = 0;
	float Object_Data = 0;
	
	
	//mlx9061x_ReadReg(0xB4, 7, &Object_Data);
	
	while(1)
	{
		
				
		/*if (bDeviceState == CONFIGURED)
		{
			if (PrevXferComplete)
			{
				if ((GPIOC->IDR & GPIO_IDR_IDR13) != 0)
					Custom_HID_Send(2, 0);
				else 
					Custom_HID_Send(2, 1);
			}
		}*/
	}
}


void Command_Handler()
{
	int num = 0;
	
	switch (sscanf(str_buffer, "%s %d", strcom_buffer, &num))
	{
		case 1:
		{
			if (strcmp("LED", strcom_buffer) == 0)
				GPIOC->ODR ^= GPIO_ODR_ODR13;
			else 
				USART1_SendString("Unknown command!\r\n");
			
			break;
		}
		
		case 2:
		{
			if (strcmp("PWM", strcom_buffer) == 0)
				TIM4->CCR1 = num;
			
			break;
		}
		
		default:
			break;
	}
}


