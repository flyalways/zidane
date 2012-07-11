
void TFT_init(void)
{
		data	U8 uc_strobe_bak;

	uc_strobe_bak = XBYTE[0xB421];
	XBYTE[0xB800] = 0;	// disable NAND auto mode
	XBYTE[0xB423] = 3;	// NAND /CS(bit0)=1, /WP(bit1)=1, ALE(bit2)=0, CLE(bit3)=0
#ifdef CAR_48
	XBYTE[0xB421] = 0x13;

	XBYTE[0xB102]|=0x3C;// P1.2/P1.3=Output
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
	//init=================TFT_8210==============================================
		LCD_Write_Command(0x01);  //sleep out
		USER_DelayDTms(50);
		LCD_Write_Command(0x11);  //sleep out
		USER_DelayDTms(50);
		    
		LCD_Write_Command(0xb1);
		LCD_Write_OneData(0x08);
		LCD_Write_OneData(0x03);
		LCD_Write_Command(0xc0);
		LCD_Write_OneData(0x04);
		LCD_Write_Command(0x1a);
		LCD_Write_Command(0xc1);
		LCD_Write_OneData(0x01);
		LCD_Write_Command(0xc2);
		LCD_Write_OneData(0x05);
		LCD_Write_Command(0xc5);
		LCD_Write_OneData(0x48);//0x4f
		LCD_Write_OneData(0x32);
		LCD_Write_Command(0xec);
		LCD_Write_OneData(0x02);
		LCD_Write_Command(0xf2);
		LCD_Write_OneData(0x01);
		USER_DelayDTms(20);
		LCD_Write_Command(0xE0);  //E0h gamma set
		LCD_Write_OneData(0x3b);
		LCD_Write_OneData(0x2b);
		LCD_Write_OneData(0x2a);
		LCD_Write_OneData(0x2d);
		LCD_Write_OneData(0x28);
		LCD_Write_OneData(0x0c);
		LCD_Write_OneData(0x57);
		LCD_Write_OneData(0xf0);
		LCD_Write_OneData(0x41);
		LCD_Write_OneData(0x12);
		LCD_Write_OneData(0x15);
		LCD_Write_OneData(0x12);
		LCD_Write_OneData(0x11);
		LCD_Write_OneData(0x10);
		LCD_Write_OneData(0x04);
		LCD_Write_Command(0xE1);  //E1h gamma set
		LCD_Write_OneData(0x04);
		LCD_Write_OneData(0x14);
		LCD_Write_OneData(0x15);
		LCD_Write_OneData(0x12);
		LCD_Write_OneData(0x17);
		LCD_Write_OneData(0x13);
		LCD_Write_OneData(0x28);
		LCD_Write_OneData(0x60);
		LCD_Write_OneData(0x3e);
		LCD_Write_OneData(0x0d);
		LCD_Write_OneData(0x2a);
		LCD_Write_OneData(0x2d);
		LCD_Write_OneData(0x2e);
		LCD_Write_OneData(0x2f);
		LCD_Write_OneData(0x3b);

		LCD_Write_Command(0x2a); // set X address
		LCD_Write_OneWord(0x00);
		LCD_Write_OneWord(0x7f);
		LCD_Write_OneWord(0x00);
		LCD_Write_OneWord(0x00);
		LCD_Write_Command(0x2b); // set Y address
		LCD_Write_OneWord(0x00);
		LCD_Write_OneWord(0x9f);
		LCD_Write_OneWord(0x00);
		LCD_Write_OneWord(0x00);

		LCD_Write_Command(0x36);
		LCD_Write_OneData(0x40);
		LCD_Write_Command(0x3a);
		LCD_Write_OneData(0x65);
		LCD_Write_Command(0x29);
//		USER_DelayDTms(100);
//		LCD_Write_Command(0x2C);
		
		USER_DelayDTms(20);
	
		XBYTE[0xB421] = uc_strobe_bak;
		XBYTE[0xB800] = 1;	// restore NAND auto mode


}void TFT_Contrast_SET(U8 Contrast)
{ //调整对比度

    U8	uc_strobe_bak;        
       if(gc_ContrastSaveOrNot==0) 
        { 
            gc_Contrast=Default_Conotrast; 
            Contrast=gc_Contrast; 
        }                           
            
        
        uc_strobe_bak = XBYTE[0xB421];
    	XBYTE[0xB800] = 0;	// disable NAND auto mode
    	XBYTE[0xB423] = 3;	// NAND /CS(bit0)=1, /WP(bit1)=1, ALE(bit2)=0, CLE(bit3)=0
    	XBYTE[0xB421] = 0x11;
////////////////////更改屏幕的对比度/////////////////////////  

	    LCD_Write_Command(0x2a);
	    LCD_Write_Command(Contrast);
//////////////////////////end////////////////////////////////
    	XBYTE[0xB421] = uc_strobe_bak;
    	XBYTE[0xB800] = 1;	// restore NAND auto mode
		gc_Contrast=Contrast;
		USER_LogFile_ReadWrite(1);
		gc_ContrastSaveOrNot=1;

}
