extern void USER_DelayDTms(U16 DT);
#define Default_Conotrast  60

void TFT_init(void)
{
	data	U8 uc_strobe_bak;

	uc_strobe_bak = XBYTE[0xB421];
	XBYTE[0xB800] = 0;	// disable NAND auto mode
	XBYTE[0xB423] = 3;	// NAND /CS(bit0)=1, /WP(bit1)=1, ALE(bit2)=0, CLE(bit3)=0
#ifdef CAR_48
	XBYTE[0xB421] = 0x13;
	XBYTE[0xB039]|=0x04;//  Reset :XFGPIO10 set output enable

	LCM_RESET_LO;
	USER_DelayDTms(100);
	LCM_RESET_HI;
	USER_DelayDTms(100);
#else
	XBYTE[0xB421] = 0x11;
	XBYTE[0xB102]|=0x04;	// P1_2=LCM reset
	LCM_RESET=0;
	USER_DelayDTms(100);
	XBYTE[0xB102]&=~0x04;
	LCM_RESET=1;
	USER_DelayDTms(100);
#endif
		LCD_Write_Command(0x11); //Exit Sleep
		USER_DelayDTms(20);
		LCD_Write_Command(0xfe);
		
		LCD_Write_Command(0x3a); 
		LCD_Write_OneData(0x05);
		
		LCD_Write_Command(0x26); //Set Default Gamma
		LCD_Write_OneData(0x04);   
		
/*			if(gc_ContrastSaveOrNot!=1) 
                { 
                        gc_ContrastSaveOrNot = 1; 
                        gc_Contrast=Default_Conotrast;        //原来的写法并未修改到gc_Contrast这个值，所以当你进入到对比度设置时会设置成255； 
                } 

                LCD_Write_OneData(gc_Contrast);   

*/

		LCD_Write_Command(0xf2);
		LCD_Write_OneData(0x00);
		
		LCD_Write_Command(0xB1);//Set Frame Rate
		LCD_Write_OneData(0x0c);
		LCD_Write_OneData(0x14);
		LCD_Write_Command(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
		LCD_Write_OneData(0x0c);//0x0c
		
		LCD_Write_OneData(0x05);//0x05
		
		LCD_Write_Command(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & VGL
		LCD_Write_OneData(0x02);//0x02
		
		LCD_Write_Command(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
		LCD_Write_OneData(0x40);//0x40
		LCD_Write_OneData(0x48);//0x48
		
		
		LCD_Write_Command(0xc7);
		LCD_Write_OneData(0x40);
		
		LCD_Write_Command(0x36);
		LCD_Write_OneData(0xc0); //0x00 正扫反扫 MY MX MV ML RGB MH
		
		LCD_Write_Command(0x3a); //Set Color Format
		LCD_Write_OneData(0x05);
		
		
		
		LCD_Write_Command(0xB4); 
		LCD_Write_OneData(0x00);
		
		LCD_Write_Command(0xf2); //Disable Gamma bit
		LCD_Write_OneData(0x00);
		
		LCD_Write_Command(0x29); // Display On
		LCD_Write_Command(0x2c); // Display On

		XBYTE[0xB421] = uc_strobe_bak;
		XBYTE[0xB800] = 1;	// restore NAND auto mode

}