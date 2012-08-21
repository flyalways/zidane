#include "..\header\SPDA2K.h"
#include "..\header\variables.h"

void PowerOn_Init_USB(void) 
{
	/*------------- Power On USB Init --------------*/
	XBYTE[0xB019] = 0x00;                        // USB Device mode
	XBYTE[0xB011]|= 0x10;                        // Enable USB CLK 
	//   Enale  each clocks and Reset USB Phy       //
	//   FM + DMA + DM                              //
	//----------------------------------------------//
	XBYTE[0xB016]=0x04;                       // power on default is 0x04
	XBYTE[0xB0E9]&=0xFE;                       // Enable usb pll clk 
	USER_Delay(5);                              // 使用Delay 方式不用polling方式
	XBYTE[0xB01D]=0x00;                        // 090510 Jay USB SRAM USE USB Phy clk
	XBYTE[0xB002]|=0x04;                       // bit 3   turn on USB_SW_RESET
	XBYTE[0xB002]&=0xFB;                       // bit 3   turn off USB_SW_RESET
}


void USBInitialize(void) 
{
	XBYTE[0xB017]=0x00;   //USB_READY_DISABLE;
	XBYTE[0xB5A0]=0x00;		
	XBYTE[0xB5A1]=0x00;		
	XBYTE[0xB5A2]=0x00;
	XBYTE[0xB530]=0x0F;	
	//Enable all module clk.
	XBYTE[0xB0F0]=0xFF;
	XBYTE[0xB0F1]=0xFF;
	XBYTE[0xB0F2]=0x00;
	
	XBYTE[0xB5D3]=0xFF;		//Enable SetFeature INT.
	XBYTE[0xB5D2]=0x02;		//Enable Bulk Out	
	XBYTE[0xB5C0]=0x00;
	XBYTE[0xB5D0]=0xFF;  
	XBYTE[0xB5D3]=0x10;
	//081023 Jay add for DMA register reset
	XBYTE[0xB340]=0x00;		// re-store default 
	XBYTE[0xB341]=0x00;
	XBYTE[0xB390]=0x00;
   	gb_XferNormalResponseData_ModeSense=NormalResponseData;
   	gc_CX_Status_flag=0;
	gb_USBRST_int_flag=0;
	gb_USB_CMD_IN=0;
	gc_USB_ADDR=1;
	gb_USB_Connect_Flag=0;
	gb_USER_Remove_Flag=0;
	gb_MediaChanged=0;
	gb_Start_Stop_Flag=0; 
	gc_USBStage=CBWStage;
}

