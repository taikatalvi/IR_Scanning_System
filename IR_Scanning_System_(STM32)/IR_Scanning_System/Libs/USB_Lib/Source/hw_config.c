/**
  ******************************************************************************
  * @file    hw_config.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Hardware Configuration & Setup
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

ErrorStatus HSEStartUpStatus;
extern __IO uint8_t PrevXferComplete;
__IO uint16_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0, calibration_value = 0;


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_USBEN;
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;
  
  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else 
  {
    bDeviceState = ATTACHED;
  }
  /*Enable SystemCoreClock*/
  SystemInit();
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
	/* Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value */
	SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup_2;
				
	uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
	uint8_t NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn, NVIC_IRQChannelPreemptionPriority = 2, NVIC_IRQChannelSubPriority = 0;
	
	/* Compute the Corresponding IRQ Priority --------------------------------*/    
	tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700)) >> 0x08;
	tmppre = (0x4 - tmppriority);
	tmpsub = tmpsub >> tmppriority;
	
	tmppriority = (uint32_t)NVIC_IRQChannelPreemptionPriority << tmppre;
	tmppriority |= NVIC_IRQChannelSubPriority & tmpsub;
	tmppriority = tmppriority << 0x04;
	        
	NVIC->IP[NVIC_IRQChannel] = tmppriority;
	    
	/* Enable the Selected IRQ Channels --------------------------------------*/
	NVIC->ISER[NVIC_IRQChannel >> 0x05] = (uint32_t)0x01 << (NVIC_IRQChannel & (uint8_t)0x1F);
		
	NVIC_IRQChannel = USBWakeUp_IRQn;
	NVIC_IRQChannelPreemptionPriority = 1;
			
	/* Compute the Corresponding IRQ Priority --------------------------------*/    
	tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700)) >> 0x08;
	tmppre = (0x4 - tmppriority);
	tmpsub = tmpsub >> tmppriority;
	
	tmppriority = (uint32_t)NVIC_IRQChannelPreemptionPriority << tmppre;
	tmppriority |= NVIC_IRQChannelSubPriority & tmpsub;
	tmppriority = tmppriority << 0x04;
	        
	NVIC->IP[NVIC_IRQChannel] = tmppriority;
	    
	/* Enable the Selected IRQ Channels --------------------------------------*/
	NVIC->ISER[NVIC_IRQChannel >> 0x05] = (uint32_t)0x01 << (NVIC_IRQChannel & (uint8_t)0x1F); 
}

void Custom_HID_Send(uint8_t* data, uint32_t length)
{	
	if (length > wMaxPacketSize) return;
	
	/* Reset the control token to inform upper layer that a transfer is ongoing */
	PrevXferComplete = 0;

	/* Copy buttons data in ENDP1 Tx Packet Memory Area*/
	USB_SIL_Write(EP1_IN, data, length);
	/* Enable endpoint for transmission */
	SetEPTxValid(ENDP1);
}


/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
  
  Device_Serial0 = *(uint32_t*)ID1;
  Device_Serial1 = *(uint32_t*)ID2;
  Device_Serial2 = *(uint32_t*)ID3;
  
  Device_Serial0 += Device_Serial2;
  
  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &CustomHID_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &CustomHID_StringSerial[18], 4);
  }
}

/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
