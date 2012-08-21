extern void USER_DelayDTms(U16 DT);
void TFT_Contrast_SET(U8 Contrast);

extern xdata   U8  gc_Contrast;
extern xdata U8 gc_ContrastSaveOrNot;
#define Default_Conotrast  31//18//35
//CS1783 init
void TFT_init(void)
{
	data	U8 uc_strobe_bak;

	uc_strobe_bak = XBYTE[0xB421];
	XBYTE[0xB800] = 0;	// disable NAND auto mode
	XBYTE[0xB423] = 3;	// NAND /CS(bit0)=1, /WP(bit1)=1, ALE(bit2)=0, CLE(bit3)=0
#ifdef CAR_48
	XBYTE[0xB421] = 0x13;

	XBYTE[0xB039]|=0x04;//  Reset :XFGPIO10 set output enable
	LCM_RESET_HI;
	USER_DelayDTms(100);
	LCM_RESET_LO;
	USER_DelayDTms(100);
	LCM_RESET_HI;
	USER_DelayDTms(200);
#else
	XBYTE[0xB421] = 0x11;

	XBYTE[0xB102]|=0x04;	// P1_2=LCM reset
	LCM_RESET=0;
	USER_DelayDTms(100);
	XBYTE[0xB102]&=~0x04;
	LCM_RESET=1;
	USER_DelayDTms(200);
#endif
	//Init....==============================
		LCD_Write_Command(0xD1);       // ENABLE INTERAL OSCILLATOR
		USER_DelayDTms(300);
	    LCD_Write_Command(0x94);        //EXIT SLEEP MODE
		USER_DelayDTms(300);
	    LCD_Write_Command(0x20);       //SET POWER CONTROL REGISTER
	    LCD_Write_OneData(0x0b);      //5X BOOST
		USER_DelayDTms(50);

	   // LCD_Write_Command(0xF3);      //stabilize the voltage generator
	   // LCD_Write_OneData(0x90);
	    //LCD_Write_OneData(0x05);
	    //LCD_Write_OneData(0x60);
	    //LCD_Write_OneData(0x84);
	
	    LCD_Write_Command(0x81);        //SET CONTRAST LEVEL & INTERAL REGULATOR

	 	if(gc_ContrastSaveOrNot!=1) 
        { 
            gc_ContrastSaveOrNot = 1; 
            gc_Contrast=Default_Conotrast;        //原来的写法并未修改到gc_Contrast这个值，所以当你进入到对比度设置时会设置成255； 
        } 

        LCD_Write_OneData(gc_Contrast);   


		LCD_Write_OneData(0x05);
	
	
	    //LCD_Write_Command(0x82);        //Set Temperature compensation coefficient
	    //LCD_Write_OneData(0x00);           //00
		USER_DelayDTms(50);
	    LCD_Write_Command(0xfb);         //SET BIASING RATIO
	    LCD_Write_OneData(0x03);        //1/9 BIAS
	   //USER_DelayDTms(100);
	   LCD_Write_Command(0xf7);         //SELECT PWM/FRC
	   LCD_Write_OneData(0x28);        //
	   LCD_Write_OneData(0x2e);        //
	   LCD_Write_OneData(0x05);        //
	//	USER_DelayDTms(100);
	    LCD_Write_Command(0xbb);         //SET COM OUTPUT SCAN DIRECTION
	    LCD_Write_OneData(0x01);         //

	   LCD_Write_Command(0xbc);         //SET  DATA OUT SCAN DIRECTION
	    LCD_Write_OneData(0x02);         //02
	   LCD_Write_OneData(0x01);        //RGB01
	  LCD_Write_OneData(0x00);        //65k   00H


	
	    LCD_Write_Command(0xca);         //SET DISPLAY CONTROL
	    LCD_Write_OneData(0x00);         //
	    LCD_Write_OneData(0x27);         //
	    LCD_Write_OneData(0x00);         //
		USER_DelayDTms(50);
	    LCD_Write_Command(0xf2);          //SET FRAME FREQUENCY
	    LCD_Write_OneData(0x08);
	    LCD_Write_OneData(0x1f);
	

		LCD_Write_Command(0x15);         //SET COM OUTPUT SCAN DIRECTION
	    LCD_Write_OneData(0x00);         //
		LCD_Write_OneData(0x7f);         //

		LCD_Write_Command(0x75);         //SET COM OUTPUT SCAN DIRECTION
	    LCD_Write_OneData(0x00);         //
		LCD_Write_OneData(0x9f);         //

	    LCD_Write_Command(0xa2);          //DISABLE ICON
	    LCD_Write_Command(0xaf);         //SET DISPLAY ON
		//USER_DelayDTms(200);
	    LCD_Write_Command(0xa7);         //NORMAL/INVERSE DISPLAY
		USER_DelayDTms(100);
	
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

	    LCD_Write_Command(0x81);
	    LCD_Write_OneData(Contrast);
//////////////////////////end////////////////////////////////
    	XBYTE[0xB421] = uc_strobe_bak;
    	XBYTE[0xB800] = 1;	// restore NAND auto mode
		gc_Contrast=Contrast;
		USER_LogFile_ReadWrite(1);
		gc_ContrastSaveOrNot=1;

}
