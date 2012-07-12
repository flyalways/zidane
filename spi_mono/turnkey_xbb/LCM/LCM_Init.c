#include "..\Header\SPDA2K.h"
#include "..\header\UI_config.h"
#include "..\header\variables.h"
#include "LCM_BMP.h"


void LCM_Init(void)
{
	U8	i;
	U8	code LCM_InitCommand[]={0xe2,0xa2,0xa0,0xc8,0xa6,0xa4,0x24,0x81,32,0x2f,0x40,0xaf};//ST7565

	XBYTE[0xB421]=0x13;
	XBYTE[0xB102]|=0x07;
	XBYTE[0xB400]=0x00;		// FMGPIO use as GPIO
	XBYTE[0xB405]|=0x02;	// FM[1] as output
	LCM_RESET_LO;
	USER_Delay(20);
	LCM_RESET_HI;
	XBYTE[0xB400]=0x01;	// FMGPIO use as NAND

	for(i=0;i<sizeof(LCM_InitCommand);i++)  //fill LCD parameter
	{
		LCM_write_command(LCM_InitCommand[i]);
	}
	LCM_BACKLIGHT=0;
	LCM_ShowCompanyLogo();
	LCM_Clear_L0_L7();
}



