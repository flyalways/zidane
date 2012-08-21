extern void USER_DelayDTms(U16 DT);
void TFT_Contrast_SET(U8 Contrast);

extern xdata   U8  gc_Contrast;
extern xdata U8 gc_ContrastSaveOrNot;

#define Default_Conotrast  60

//丽电视角不对的屏
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
	//=============================== 

	  

#if 1
        LCD_Write_Command(0x00);
	    LCD_Write_Command(0x00);
	    LCD_Write_OneData(0x00);
		LCD_Write_OneData(0x01);
        
		LCD_Write_Command(0x00);
	    LCD_Write_Command(0x01);
	    LCD_Write_OneData(0x11);
		LCD_Write_OneData(0x14);
        
		LCD_Write_Command(0x00);
	    LCD_Write_Command(0x03);
	    LCD_Write_OneData(0xb0);
		LCD_Write_OneData(0x00);
        USER_DelayDTms(20);
	    LCD_Write_OneData(0xb4);
		LCD_Write_OneData(0x00);
        USER_DelayDTms(20);
	    LCD_Write_OneData(0xbc);
		LCD_Write_OneData(0x0c);
         USER_DelayDTms(20);

		LCD_Write_Command(0x00);
	    LCD_Write_Command(0x0c);
	    LCD_Write_OneData(0x00);
		LCD_Write_OneData(0x00);
        USER_DelayDTms(20);

		LCD_Write_Command(0x00);
	    LCD_Write_Command(0x04);
	    LCD_Write_OneData(0x01);
		LCD_Write_OneData(0x05);
        USER_DelayDTms(20);

		LCD_Write_Command(0x00);
  		LCD_Write_Command(0x05);
	    LCD_Write_OneData(0x00);
		LCD_Write_OneData(0x00);//38
        

		LCD_Write_Command(0x00);
	    LCD_Write_Command(0x07);
	    LCD_Write_OneData(0x00);
		LCD_Write_OneData(0x03);
        
        LCD_Write_Command(0x00);
	    LCD_Write_Command(0x22);
#endif	    

	XBYTE[0xB421] = uc_strobe_bak;
	XBYTE[0xB800] = 1;	// restore NAND auto mode
}
void TFT_Contrast_SET(U8 Contrast)
{ //调整对比度

    U8	uc_strobe_bak;                            
    {     
        uc_strobe_bak = XBYTE[0xB421];
    	XBYTE[0xB800] = 0;	// disable NAND auto mode
    	XBYTE[0xB423] = 3;	// NAND /CS(bit0)=1, /WP(bit1)=1, ALE(bit2)=0, CLE(bit3)=0
    	XBYTE[0xB421] = 0x11;
////////////////////更改屏幕的对比度/////////////////////////  

	    //LCD_Write_Command(0x81);
	    //LCD_Write_OneData(Contrast);
//////////////////////////end////////////////////////////////
    	XBYTE[0xB421] = uc_strobe_bak;
    	XBYTE[0xB800] = 1;	// restore NAND auto mode
	}
}
