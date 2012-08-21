
void TFT_init(void)
{
	data	U8 i;

#ifdef CAR_64	// 54124B
	U8 code	E0_gamma[16]={0x00,0x01,0x0C,0x29,0x27,0x0B,0x11,0x06,0x06,0x05,0x0E,0x0F,0x00,0x03,0x04,0x08};
	U8 code	E1_gamma[16]={0x06,0x1F,0x20,0x16,0x0A,0x04,0x04,0x10,0x06,0x03,0x03,0x02,0x0E,0x0D,0x05,0x05};
	
	XBYTE[0xB800] = 0;	// disable NAND auto mode
	XBYTE[0xB423] = 3;	// NAND /CS(bit0)=1, /WP(bit1)=1, ALE(bit2)=0, CLE(bit3)=0
	
	XBYTE[0xB102]|=0x04;	// P1_2=LCM reset
	P1&=0xFB;	
	USER_DelayDTms(35);
	P1|=0x04;
	USER_DelayDTms(35);
	LCD_Write_Command(0x11);  //sleep out
	USER_DelayDTms(35);
	LCD_Write_Command(0x3A);
	LCD_Write_OneData(0x05);

	LCD_Write_Command(0xC0);
	LCD_Write_OneData(0x05);
	LCD_Write_OneData(0x00);
	USER_DelayDTms(35);
	
	LCD_Write_Command(0xc2);
	LCD_Write_OneData(0x05);
	LCD_Write_OneData(0x00);
	USER_DelayDTms(100);
	
	LCD_Write_Command(0xc5);  //SET VCOMH step = 0.025V C4H=4.225V	
	LCD_Write_OneData(0xc0); //VCOMH = 4.225V	
	
	LCD_Write_Command(0xc6);  //VCOMAC Step = 0.05V 0EH = 4.7V
	LCD_Write_OneData(0x0b);	//VCOMAC = 4.7V
	USER_DelayDTms(20);
	
	LCD_Write_Command(0xE0);  //E0h gamma set
	for(i=0;i<16;i++)
	{
		LCD_Write_OneData(E0_gamma[i]);
	}
	LCD_Write_Command(0xE1);  //E1h gamma set
	for(i=0;i<16;i++)
	{
		LCD_Write_OneData(E1_gamma[i]);
	}
	
	LCD_Write_Command(0x26);
	LCD_Write_OneData(0x04);
	
	LCD_Write_Command(0x29);
	LCD_Write_Command(0x2C);
	
	XBYTE[0xB800] = 1;	// restore NAND auto mode
#else	// 54124C
	U8 code	E0_gamma[16]={0x00,0x01,0x15,0x33,0x29,0x0B,0x18,0x02,0x00,0x04,0x0E,0x0F,0x00,0x07,0x03,0x06};
	U8 code	E1_gamma[16]={0x06,0x20,0x1D,0x15,0x0A,0x04,0x04,0x16,0x06,0x04,0x02,0x00,0x0F,0x0E,0x04,0x05};
	
	XBYTE[0xB400]=1;
	XBYTE[0xB800] = 0;	// disable NAND auto mode
	XBYTE[0xB423] = 3;	// NAND /CS(bit0)=1, /WP(bit1)=1, ALE(bit2)=0, CLE(bit3)=0

	LCD_Write_Command(0x11);  //sleep out
	USER_DelayDTms(20);
	LCD_Write_Command(0xc0);  //GVDD step = 0.05V  04=4.6V
	LCD_Write_OneData(0x04); //Source = 4.6V
	LCD_Write_OneData(0x00); //VCI1 = 2.75V
	USER_DelayDTms(20);
	
	LCD_Write_Command(0xc1);  //AVDD VGL VGL VCL
	LCD_Write_OneData(0x05); //VGH = 14.7V  VGL=-7.35V 
	USER_DelayDTms(100);
	
	LCD_Write_Command(0xc5);  //SET VCOMH step = 0.025V C4H=4.225V	
	LCD_Write_OneData(0xc4); //VCOMH = 4.225V	
	
	LCD_Write_Command(0xc6);  //VCOMAC Step = 0.05V 0EH = 4.7V
	LCD_Write_OneData(0x0e);	//VCOMAC = 4.7V
	
	LCD_Write_Command(0xf0);  //Enable framerate adjust
	LCD_Write_OneData(0x5a);	//
	
	LCD_Write_Command(0xf2);  //framerate set 
	LCD_Write_OneData(0x02);	//01=80 Hz 02 = 60Hz */
		
	LCD_Write_Command(0xf3);  //framerate set 
	LCD_Write_OneData(0x11);
	
	LCD_Write_Command(0x3A); //interface format
	LCD_Write_OneData(0x05);	//05 = 65K  06 = 262K  
	USER_DelayDTms(20);
	
	LCD_Write_Command(0xE0);  //E0h gamma set
	for(i=0;i<16;i++)
	{
		LCD_Write_OneData(E0_gamma[i]);
	}
	LCD_Write_Command(0xE1);  //E1h gamma set
	for(i=0;i<16;i++)
	{
		LCD_Write_OneData(E1_gamma[i]);
	}
	
	LCD_Write_Command(0x36);
	LCD_Write_OneData(0x00);
	
	LCD_Write_Command(0x29);
	LCD_Write_Command(0x2C);
	
	XBYTE[0xB800] = 1;	// restore NAND auto mode
#endif
}