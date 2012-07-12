
void TFT_init(void)
{
	data	U8 i, uc_strobe_bak;

	//U8 code	E0_gamma[16]={0x00,0x01,0x0C,0x29,0x27,0x0B,0x11,0x06,0x06,0x05,0x0E,0x0F,0x00,0x03,0x04,0x08};
	//U8 code	E1_gamma[16]={0x06,0x1F,0x20,0x16,0x0A,0x04,0x04,0x10,0x06,0x03,0x03,0x02,0x0E,0x0D,0x05,0x05};


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
#if 0
	LCD_Write_Command(0x11); 
	USER_DelayDTms(20);
 	LCD_Write_Command(0xb1); 
	LCD_Write_OneData(0x01);     
   	LCD_Write_OneData(0x2c);  
	LCD_Write_OneData(0x2d);

 	LCD_Write_Command(0xb2); 
	LCD_Write_OneData(0x01);     
   	LCD_Write_OneData(0x2c);  
	LCD_Write_OneData(0x2d);

 	LCD_Write_Command(0xb3); 
	LCD_Write_OneData(0x01);     
   	LCD_Write_OneData(0x2c);  
	LCD_Write_OneData(0x2d);
	LCD_Write_OneData(0x01);     
   	LCD_Write_OneData(0x2c);  
	LCD_Write_OneData(0x2d);

	LCD_Write_Command(0xb4); 
	LCD_Write_OneData(0x07);     
           
 	LCD_Write_Command(0xc0); 
	LCD_Write_OneData(0xa2);     
   	LCD_Write_OneData(0x02);  
	LCD_Write_OneData(0x84);
                                            
 	LCD_Write_Command(0xc1); 
	LCD_Write_OneData(0xc5);  
   
 	LCD_Write_Command(0xc2); 
	LCD_Write_OneData(0x0a);                                     
	LCD_Write_OneData(0x00); 

 	LCD_Write_Command(0xc3); 
	LCD_Write_OneData(0x8a); 
   	LCD_Write_OneData(0x2a);  

 	LCD_Write_Command(0xc4); 
	LCD_Write_OneData(0x8a);    
	LCD_Write_OneData(0xee);   

 	LCD_Write_Command(0xc5); 
	LCD_Write_OneData(0x0e);  

//	LCD_Write_Command(0x36); 
//	LCD_Write_OneData(0xe8);//屏的旋转控制,需要配合调2A和2B的值



	LCD_Write_Command(0xe0); 
	LCD_Write_OneData(0x02);
	LCD_Write_OneData(0x1c);	
	LCD_Write_OneData(0x07);	
	LCD_Write_OneData(0x12);
	LCD_Write_OneData(0x37);
	LCD_Write_OneData(0x32);	
	LCD_Write_OneData(0x29);	
	LCD_Write_OneData(0x2d);
	LCD_Write_OneData(0x29);
	LCD_Write_OneData(0x25);	
	LCD_Write_OneData(0x2b);	
	LCD_Write_OneData(0x39);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x01);	
	LCD_Write_OneData(0x03);
	LCD_Write_OneData(0x10);	

	LCD_Write_Command(0xe1); 
	LCD_Write_OneData(0x03);
	LCD_Write_OneData(0x1d);	
	LCD_Write_OneData(0x07);	
	LCD_Write_OneData(0x06);
	LCD_Write_OneData(0x2e);
	LCD_Write_OneData(0x2c);	
	LCD_Write_OneData(0x29);	
	LCD_Write_OneData(0x2d);
	LCD_Write_OneData(0x2e);
	LCD_Write_OneData(0x2e);	
	LCD_Write_OneData(0x37);	
	LCD_Write_OneData(0x3f);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x02);
	LCD_Write_OneData(0x10);

 	LCD_Write_Command(0xf0); 
	LCD_Write_OneData(0x01);     
 	LCD_Write_Command(0xf6); 
	LCD_Write_OneData(0x00);  

	LCD_Write_Command(0x3a); 
	LCD_Write_OneData(0x05);
 
 /* 	Trans_Com_16(0x2a); 
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x02);	
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x81);

	Trans_Com_16(0x2b); 
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x01);	
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0xa0);
*/ 
  /* Trans_Com_16(0x2a); 
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x9f);

	Trans_Com_16(0x2b); 
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x7f);
	*/
	USER_DelayDTms(20);  

	LCD_Write_Command(0x29); 
#endif 
//===================TM7735_V1=============================
	LCD_Write_Command(0x11); 
	USER_DelayDTms(20);
 	LCD_Write_Command(0xb1); 
	LCD_Write_OneData(0x01);     
   	LCD_Write_OneData(0x2c);  
	LCD_Write_OneData(0x2d);

 	LCD_Write_Command(0xb2); 
	LCD_Write_OneData(0x01);     
   	LCD_Write_OneData(0x2c);  
	LCD_Write_OneData(0x2d);

 	LCD_Write_Command(0xb3); 
	LCD_Write_OneData(0x01);     
   	LCD_Write_OneData(0x2c);  
	LCD_Write_OneData(0x2d);
	LCD_Write_OneData(0x01);     
   	LCD_Write_OneData(0x2c);  
	LCD_Write_OneData(0x2d);

	LCD_Write_Command(0xb4); 
	LCD_Write_OneData(0x07);     
           
 	LCD_Write_Command(0xc0); 
	LCD_Write_OneData(0xa2);     
   	LCD_Write_OneData(0x02);  
	LCD_Write_OneData(0x84);
                                            
 	LCD_Write_Command(0xc1); 
	LCD_Write_OneData(0xc5);  
   
 	LCD_Write_Command(0xc2); 
	LCD_Write_OneData(0x0a);                                     
	LCD_Write_OneData(0x00); 

 	LCD_Write_Command(0xc3); 
	LCD_Write_OneData(0x8a); 
   	LCD_Write_OneData(0x2a);  

 	LCD_Write_Command(0xc4); 
	LCD_Write_OneData(0x8a);    
	LCD_Write_OneData(0xee);   

 	LCD_Write_Command(0xc5); 
	LCD_Write_OneData(0x0e);  

//	LCD_Write_Command(0x36); 
//	LCD_Write_OneData(0xe8);//屏的旋转控制,需要配合调2A和2B的值



	LCD_Write_Command(0xe0); 
	LCD_Write_OneData(0x02);
	LCD_Write_OneData(0x1c);	
	LCD_Write_OneData(0x07);	
	LCD_Write_OneData(0x12);
	LCD_Write_OneData(0x37);
	LCD_Write_OneData(0x32);	
	LCD_Write_OneData(0x29);	
	LCD_Write_OneData(0x2d);
	LCD_Write_OneData(0x29);
	LCD_Write_OneData(0x25);	
	LCD_Write_OneData(0x2b);	
	LCD_Write_OneData(0x39);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x01);	
	LCD_Write_OneData(0x03);
	LCD_Write_OneData(0x10);	

	LCD_Write_Command(0xe1); 
	LCD_Write_OneData(0x03);
	LCD_Write_OneData(0x1d);	
	LCD_Write_OneData(0x07);	
	LCD_Write_OneData(0x06);
	LCD_Write_OneData(0x2e);
	LCD_Write_OneData(0x2c);	
	LCD_Write_OneData(0x29);	
	LCD_Write_OneData(0x2d);
	LCD_Write_OneData(0x2e);
	LCD_Write_OneData(0x2e);	
	LCD_Write_OneData(0x37);	
	LCD_Write_OneData(0x3f);
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x02);
	LCD_Write_OneData(0x10);

 	LCD_Write_Command(0xf0); 
	LCD_Write_OneData(0x01);     
 	LCD_Write_Command(0xf6); 
	LCD_Write_OneData(0x00);  

	LCD_Write_Command(0x3a); 
	LCD_Write_OneData(0x05);
 
 /* 	Trans_Com_16(0x2a); 
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x02);	
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x81);

	Trans_Com_16(0x2b); 
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x01);	
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0xa0);
*/ 
  /* Trans_Com_16(0x2a); 
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x9f);

	Trans_Com_16(0x2b); 
	LCD_Write_OneData(0x00);
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x00);	
	LCD_Write_OneData(0x7f);
	*/
	USER_DelayDTms(20);  

	LCD_Write_Command(0x29); 

	
	

	LCD_Write_Command(0x29); 
	USER_DelayDTms(200);  
	
	XBYTE[0xB800] = 1;	// restore NAND auto mode
	XBYTE[0xB421] = uc_strobe_bak;

}