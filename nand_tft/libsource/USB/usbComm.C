#include "SPDA2K.h"

void PowerOn_Init_USB(void) 
{
	/*------------- Power On USB Init --------------*/
	GLOBAL_REG[0x19] = 0x00;                        // USB Device mode
	GLOBAL_REG[0x11]|= 0x10;                        // Enable USB CLK 
	//   Enale  each clocks and Reset USB Phy       //
	//   FM + DMA + DM                              //
	//----------------------------------------------//
	GLOBAL_REG[0x16]  = 0x04;                       // power on default is 0x04
	GLOBAL_REG[0xE9] &= 0xFE;                       // Enable usb pll clk 
	USER_DelayDTms(5);                              // 使用Delay 方式不用polling方式
	GLOBAL_REG[0x1D] = 0x00;                        // 090510 Jay USB SRAM USE USB Phy clk
	GLOBAL_REG[0x02] |= 0x04;                       // bit 3   turn on USB_SW_RESET
	GLOBAL_REG[0x02] &= 0xFB;                       // bit 3   turn off USB_SW_RESET
}


void USBInitialize(void) 
{
	GLOBAL_REG[0x17]  = 0x00;   //USB_READY_DISABLE;
	USB_REG[0xA0] 	  = 0x00;		
	USB_REG[0xA1] 	  = 0x00;		
	USB_REG[0xA2] 	  = 0x00;
	USB_REG[0x30]     = 0x0F;	
	//Enable all module clk.
	GLOBAL_REG[0xF0] = 0xFF;
	GLOBAL_REG[0xF1] = 0xFF;
	GLOBAL_REG[0xF2] = 0x00;
	
	USB_REG[0xD3] = 0xFF;		//Enable SetFeature INT.
	USB_REG[0xD2] = 0x02;		//Enable Bulk Out	
	USB_REG[0xC0] = 0x00;
	USB_REG[0xD0] = 0xFF;  
	USB_REG[0xD3] = 0x10;
	//081023 Jay add for DMA register reset
	DMA_REG[0x40] = 0x00;		// re-store default 
	DMA_REG[0x41] = 0x00;
	DMA_REG[0x90] = 0x00;
   	gbt_XferNormalResponseData_or_ModeSense = NormalResponseData;
   	gbt_Have_set_configuration = 0;
   	gc_CX_Status_flag       = 0;
	gbt_EP0SETUP_int_flag   = 0;
	gbt_EP0RX_int_flag      = 0;
	gbt_EP0TX_int_flag      = 0;
	gbt_USBRST_int_flag     = 0;
	gbt_SUSPEND_int_flag    = 0;
	gbt_RESUME_int_flag     = 0;
	gbt_USB_CMD_IN	    = 0;
	gc_USB_ADDR             = 1;
	gbt_USB_Connect_Flag    = 0;
	gbt_USER_Remove_Flag    = 0;
	
	gbt_USB_CONFIG_Chg_int_flag = 0;
	gbt_StorageMedia_has_changed =0;
	gc_Start_Stop_Flag      = 0; 
	gc_USBStage = CBWStage;
}


