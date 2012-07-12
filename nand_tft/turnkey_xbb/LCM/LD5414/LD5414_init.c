extern void USER_DelayDTms(U16 DT);

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
	LCD_Write_Command(0x01);
	USER_DelayDTms(12);
	LCD_Write_Command(0x11);
	USER_DelayDTms(12);
	
	LCD_Write_Command(0x3A);
	LCD_Write_OneData(0x55);     
	
	LCD_Write_Command(0xC0);
	LCD_Write_OneData(0x01);
	LCD_Write_OneData(0x01);
	
	LCD_Write_Command(0xC3);
	LCD_Write_OneData(0x06);
	LCD_Write_Command(0xC5);
	LCD_Write_OneData(0x80);//0xa0
	LCD_Write_Command(0xC6);
	LCD_Write_OneData(0x01);
	LCD_Write_Command(0xB4);
	LCD_Write_OneData(0x04);
	LCD_Write_Command(0xF0);
	LCD_Write_OneData(0x5A);
	LCD_Write_Command(0xF2);
	LCD_Write_OneData(0x00);
	LCD_Write_Command(0xF3);
	LCD_Write_OneData(0x31);
	
	LCD_Write_Command(0xE0);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x03);
	LCD_Write_OneData(0x04);
	LCD_Write_OneData(0x32);
	LCD_Write_OneData(0x2D);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x10);
	LCD_Write_OneData(0x05);
	LCD_Write_OneData(0x02);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x0E);
	LCD_Write_OneData(0x0F);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x04);
	LCD_Write_OneData(0x03);
	LCD_Write_OneData(0x05);
	
	LCD_Write_Command(0xE1);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x2D);
	LCD_Write_OneData(0x30);
	LCD_Write_OneData(0x04);
	LCD_Write_OneData(0x03);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x05);
	LCD_Write_OneData(0x10);
	LCD_Write_OneData(0x05);
	LCD_Write_OneData(0x03);
	LCD_Write_OneData(0x04);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x0F);
	LCD_Write_OneData(0x0E);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x02);
	LCD_Write_Command(0x36);
	LCD_Write_OneData(0xC8);
	LCD_Write_Command(0x29);
	USER_DelayDTms(150);
	LCD_Write_Command(0x2c); 
	
	
	XBYTE[0xB421] = uc_strobe_bak;
	XBYTE[0xB800] = 1;	// restore NAND auto mode
	
}
