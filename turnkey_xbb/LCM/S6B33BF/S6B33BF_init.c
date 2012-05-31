extern void USER_DelayDTms(U16 DT);
void TFT_Contrast_SET(U8 Contrast);

extern xdata   U8  gc_Contrast;
extern xdata U8 gc_ContrastSaveOrNot;

#define Default_Conotrast  0xd8
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
 	 LCD_Write_Command(0x50);                       
     USER_DelayDTms(20);
	 
     LCD_Write_Command(0x2c);                       
     USER_DelayDTms(20);

     LCD_Write_Command(0x02);  
     LCD_Write_Command(0x01);  

     LCD_Write_Command(0x26);  
     LCD_Write_Command(0x01);  
     USER_DelayDTms(20);

     LCD_Write_Command(0x26);  
     LCD_Write_Command(0x09);  
     USER_DelayDTms(20);

     LCD_Write_Command(0x26);  
     LCD_Write_Command(0x0b);  
     USER_DelayDTms(20);

     LCD_Write_Command(0x26);  
     LCD_Write_Command(0x0f);  
     USER_DelayDTms(20);
	 
  //   LCD_Write_Command(0x10);  
  //  LCD_Write_Command(0x23);  

     LCD_Write_Command(0x20);  //VIN1 DC*1.5
     LCD_Write_Command(0x01); 
     
     LCD_Write_Command(0x22);  
     LCD_Write_Command(0x11);  
     
     LCD_Write_Command(0x24);  
     LCD_Write_Command(0x22);  
     
     LCD_Write_Command(0x28);  
     LCD_Write_Command(0x01);   

     LCD_Write_Command(0x2a);  //vop
     if(gc_ContrastSaveOrNot!=1) 
        { 
            gc_ContrastSaveOrNot = 1; 
            gc_Contrast=Default_Conotrast;        //原来的写法并未修改到gc_Contrast这个值，所以当你进入到对比度设置时会设置成255； 
        } 

        LCD_Write_Command(gc_Contrast);   

  //   LCD_Write_Command(0x2b);  
  //   LCD_Write_Command(0xc7);  

     LCD_Write_Command(0x30);  
     LCD_Write_Command(0x1f);  

     LCD_Write_Command(0x32);  
     LCD_Write_Command(0x0e);  

     LCD_Write_Command(0x34);  
     LCD_Write_Command(0xd1);  

     LCD_Write_Command(0x36);  
     LCD_Write_Command(0x00);  

//     LCD_Write_Command(0x40);  
//     LCD_Write_Command(0x00);
     
     LCD_Write_Command(0x42);  
     LCD_Write_Command(0x00); 
     LCD_Write_Command(0x9f);  

     LCD_Write_Command(0x43);  
     LCD_Write_Command(0x00);  
     LCD_Write_Command(0x7f);  

     LCD_Write_Command(0x45);  
     LCD_Write_Command(0x00); 
     
     LCD_Write_Command(0x53);  
     LCD_Write_Command(0x00); 
     
     LCD_Write_Command(0x55);  
     LCD_Write_Command(0x00); 
     
     //LCD_Write_Command(0x56);  
     //LCD_Write_Command(0x01);  
     
     //LCD_Write_Command(0x57);  
     //LCD_Write_Command(0xa0);
     
     USER_DelayDTms(200);   
   
     LCD_Write_Command(0x51);  
     USER_DelayDTms(50);      

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

	    LCD_Write_Command(0x2a);
	    LCD_Write_Command(Contrast);
//////////////////////////end////////////////////////////////
    	XBYTE[0xB421] = uc_strobe_bak;
    	XBYTE[0xB800] = 1;	// restore NAND auto mode
		gc_Contrast=Contrast;
		USER_LogFile_ReadWrite(1);
		gc_ContrastSaveOrNot=1;

}
