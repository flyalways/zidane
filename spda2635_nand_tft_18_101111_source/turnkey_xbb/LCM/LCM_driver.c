#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\LCM\model_define.h"
#include "..\LCM\LCM.h"
#include "..\LCM\LCM_func.h"
#include "..\LCM\TFT_config.h"		// TFT 20090629

extern	xdata U8	gc_strobe_bak;
extern xdata U8  	gc_0xB400_bak;
xdata U8 	gc_LCMStobe=0x64; 

void LCD_Write_Command(U8 tc_Data)
{
	LCM_A0_LO; 
	LCM_CS_LO; 
	XBYTE[0xB420] = tc_Data; 
	LCM_CS_HI; 
}

void LCD_Write_OneData(U8 tc_Data)  
{
	LCM_A0_HI;
	LCM_CS_LO; 
	XBYTE[0xB420] = tc_Data; 
	LCM_CS_HI;
}

void LCD_Write_OneWord(U16 w)
{
	LCM_A0_HI;
	LCM_CS_LO; 
	XBYTE[0xB420] = WORD_HI_BYTE(w); 
	XBYTE[0xB420] = WORD_LO_BYTE(w); 
	LCM_CS_HI;
}

void LCD_Nand2DataMode(void)
{
	//for 023Only
	gc_strobe_bak = XBYTE[0xB421];
	gc_0xB400_bak = XBYTE[0xB400];	
	XBYTE[0xB400] =0x01;		//switch to NAND 
	FLASH_REG[0x00] = 0x00;		//disable nand auto 
	DMA_REG[0x01] = 0x20;		//Sram to Flash		
	XBYTE[0xB421]=gc_LCMStobe;   //XBYTE[0xB421] = D_LCM_Strobe;	//strobe = 1T 1T

	LCM_A0_HI; //OLED_A0=1;
	LCM_CS_LO; //OLED_CS=0;
}

void LCD_Data2NandMode(void)
{
	LCM_CS_HI; //OLED_CS=1;	
	//for 023Only
	XBYTE[0xB421] = gc_strobe_bak;
	FLASH_REG[0x00] = 0x01;
	XBYTE[0xB400]=gc_0xB400_bak;
}



#ifdef LD5414C
#include "..\LCM\LD5414\LD5414_driver.c"	
#elif defined SL9163	
#include "..\LCM\SL9163\SL9163_driver.c"
#elif defined S6D0134
#include "..\LCM\S6D0134\S6D0134_driver.c"
#elif defined TM7735	
#include "..\LCM\TM7735\TM7735_driver.c"	

#elif defined SL8210	
#include "..\LCM\SL8210\SL8210_driver.c"		
#endif

