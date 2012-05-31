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
/*	LCD_Write_Command( 0x00); 
	LCD_Write_Command( 0x00); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x01);
	USER_DelayDTms(100);
	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x17); 	
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x04);


	LCD_Write_Command( 0x00);     
	LCD_Write_Command( 0x12);   //liang du
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);  //  hei-bai 00  01  03  06

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x13);
    LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);
	USER_DelayDTms(20);
	LCD_Write_Command( 0x00); 
	LCD_Write_Command( 0x01); 
 	LCD_Write_OneData( 0x01);  //shui wen
 	LCD_Write_OneData( 0x13);  //shui wen 

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x02);
    LCD_Write_OneData( 0x04);
    LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x03);
    LCD_Write_OneData( 0x10);
    LCD_Write_OneData( 0x30);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x08); 
	LCD_Write_OneData( 0x02); 
	LCD_Write_OneData( 0x02);

	LCD_Write_Command( 0x00); 
	LCD_Write_Command( 0x09);
   	LCD_Write_OneData( 0x00); 
   	LCD_Write_OneData( 0x04);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x0B); 
	LCD_Write_OneData( 0x40); 
	LCD_Write_OneData( 0x06);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x0C); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x0D);
    LCD_Write_OneData( 0x11);
    LCD_Write_OneData( 0x1F);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x0E);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);                                                            
    LCD_Write_Command( 0x15);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x02);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x11); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x01);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x12); 
	LCD_Write_OneData( 0x00);
	LCD_Write_OneData( 0x1F);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x13); 
	LCD_Write_OneData( 0x11);
 	LCD_Write_OneData( 0x11);
 
	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x10);
    LCD_Write_OneData( 0x47);
    LCD_Write_OneData( 0x64);
	USER_DelayDTms(50);
 
	LCD_Write_Command( 0x00);       
    LCD_Write_Command( 0x12);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x1F);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x13);
    LCD_Write_OneData( 0x31);
    LCD_Write_OneData( 0x11);
	USER_DelayDTms( 40);
	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x10);
    LCD_Write_OneData( 0x47);
    LCD_Write_OneData( 0x60);
	USER_DelayDTms( 20);
////////////////////////
	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x30);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x31);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x07);

	LCD_Write_Command( 0x00);    
    LCD_Write_Command( 0x32);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x07);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x33);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x04);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x34);
    LCD_Write_OneData( 0x04);
    LCD_Write_OneData( 0x04);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x35);
    LCD_Write_OneData( 0x07);
    LCD_Write_OneData( 0x02);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x36);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x04);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x37);
    LCD_Write_OneData( 0x07);
    LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x38);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x09);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x39);
    LCD_Write_OneData( 0x07);
    LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x40);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x41);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x42);
    LCD_Write_OneData( 0x7F);
    LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x43);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x44);
    LCD_Write_OneData( 0x7F);
    LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x45);
    LCD_Write_OneData( 0x7F);
    LCD_Write_OneData( 0x00);
	USER_DelayDTms( 100);




	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x07);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x05);
	USER_DelayDTms( 30);
	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x07);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x25);
	USER_DelayDTms( 30);
	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x07);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x27);
	USER_DelayDTms( 30);
	LCD_Write_Command( 0x00); 
    LCD_Write_Command( 0x07);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x37);
	USER_DelayDTms( 100);
*/
	LCD_Write_Command( 0x00);
	LCD_Write_Command( 0x00); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x01);
	USER_DelayDTms( 100); 

 	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x07); 
	LCD_Write_OneData( 0x01); 
	LCD_Write_OneData( 0x04);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x03); 
	LCD_Write_OneData( 0x05); 
	LCD_Write_OneData( 0x08);
	USER_DelayDTms( 40); 	//05,08  

	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x04);
    LCD_Write_OneData( 0x00);
    LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x0c);
    LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);
	USER_DelayDTms( 40); 

	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x0d); 
	LCD_Write_OneData( 0x1b); 
	LCD_Write_OneData( 0x1e);
	USER_DelayDTms( 100);//0b,1e

	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x0e); 
	LCD_Write_OneData( 0x27); 
	LCD_Write_OneData( 0x10);
	USER_DelayDTms( 100);   //36,1e 

	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x01); 
	LCD_Write_OneData( 0x03); 
	LCD_Write_OneData( 0x13);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x02); 
	LCD_Write_OneData( 0x07); 
	LCD_Write_OneData( 0x00);

	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x05); 
	LCD_Write_OneData( 0x10); 
	LCD_Write_OneData( 0x30);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x06); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x0b); 
	LCD_Write_OneData( 0x40); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x0c); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x0f); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x02);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x10); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x11); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x16); 
	LCD_Write_OneData( 0x7f); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x17); 
	LCD_Write_OneData( 0xaf); 
	LCD_Write_OneData( 0x00);
    

	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x30); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x31); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);



	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x32); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x33); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x34); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x35); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x36); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x37); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x38); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x39); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);



	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x3a); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);


	LCD_Write_Command( 0x00);
    LCD_Write_Command( 0x3b); 
	LCD_Write_OneData( 0x00); 
	LCD_Write_OneData( 0x00);

   	  LCD_Write_Command( 0x00);    
	  LCD_Write_Command( 0x07); 
	  LCD_Write_OneData( 0x01); 
	  LCD_Write_OneData( 0x37);
	  USER_DelayDTms( 40);


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
