extern void USER_DelayDTms(U16 DT);
void TFT_Contrast_SET(U8 Contrast);

extern xdata   U8  gc_Contrast;
extern xdata U8 gc_ContrastSaveOrNot;

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
	LCD_Write_OneData(0x05);  
   
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



	LCD_Write_Command(	0xe0);
	LCD_Write_OneData(	0x0f);
	LCD_Write_OneData(	0x1a);
	LCD_Write_OneData(	0x0f);
	LCD_Write_OneData(	0x18);
	LCD_Write_OneData(	0x2f);
	LCD_Write_OneData(	0x28);
	LCD_Write_OneData(	0x20);
	LCD_Write_OneData(	0x22);
	LCD_Write_OneData(	0x1f);
	LCD_Write_OneData(	0x1b);
	LCD_Write_OneData(	0x23);
	LCD_Write_OneData(	0x37);
	LCD_Write_OneData(	0x00);
	LCD_Write_OneData(	0x07);
	LCD_Write_OneData(	0x02);
	LCD_Write_OneData(	0x10);
	
	LCD_Write_Command(	0xe1);
	LCD_Write_OneData(	0x0f);
	LCD_Write_OneData(	0x1b);
	LCD_Write_OneData(	0x0f);
	LCD_Write_OneData(	0x17);
	LCD_Write_OneData(	0x33);
	LCD_Write_OneData(	0x2c);
	LCD_Write_OneData(	0x29);
	LCD_Write_OneData(	0x2e);
	LCD_Write_OneData(	0x30);
	LCD_Write_OneData(	0x30);
	LCD_Write_OneData(	0x39);
	LCD_Write_OneData(	0x3f);
	LCD_Write_OneData(	0x00);
	LCD_Write_OneData(	0x07);
	LCD_Write_OneData(	0x03);
	LCD_Write_OneData(	0x10);
	
	LCD_Write_Command(	0x3a);
	LCD_Write_OneData(	0x05);
	
	LCD_Write_Command(	0xf0);
	LCD_Write_OneData(	0x01);
	
	LCD_Write_Command(	0xf6);
	LCD_Write_OneData(	0x00);

	LCD_Write_Command(0x29); // Display On
	
	
	USER_DelayDTms(20);


		XBYTE[0xB421] = uc_strobe_bak;
		XBYTE[0xB800] = 1;	// restore NAND auto mode

}
void TFT_Contrast_SET(U8 Contrast)
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

	    //LCD_Write_Command(0x2a);
	    //LCD_Write_Command(Contrast);
//////////////////////////end////////////////////////////////
    	XBYTE[0xB421] = uc_strobe_bak;
    	XBYTE[0xB800] = 1;	// restore NAND auto mode
		gc_Contrast=Contrast;
		USER_LogFile_ReadWrite(1);
		gc_ContrastSaveOrNot=1;

}
