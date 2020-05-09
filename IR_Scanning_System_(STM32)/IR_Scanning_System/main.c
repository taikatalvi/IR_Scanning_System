
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
	I2C2_Init();
	
	// Configuring USB
	USB_Interrupts_Config();
	Set_USBClock();
	USB_Init();
		
	
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
			else if (strcmp("Temperature", strcom_buffer) == 0)
			{
				float Amb_Temp = 0;
				float Obj_1_Temp = 0, Obj_2_Temp = 0;
	
				int8_t ErrTa = MLX9061x_ReadTa(MLX9061X_ADDR, &Amb_Temp);
				int8_t ErrTo1 = MLX9061x_ReadTo1(MLX9061X_ADDR, &Obj_1_Temp);	
		
				if (ErrTa)
					sprintf(str_buffer, "Ambient temperature error - %d\n", ErrTa);
				else
					sprintf(str_buffer, "Ambient temperature is %3.1f\n", (double)Amb_Temp);
		
				USART1_SendString(str_buffer);
    
				if (ErrTo1)
					sprintf(str_buffer, "First object temperature error - %d\n", ErrTo1);
				else
					sprintf(str_buffer, "First object temperature is %3.1f\n", (double)Obj_1_Temp);
				
				USART1_SendString(str_buffer);
			}
			else USART1_SendString("Unknown command!\r\n");
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


