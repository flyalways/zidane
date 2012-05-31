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
		LCD_Write_Command(0xD7);                  //Autoread control
	    LCD_Write_OneData(0xDF);                //Disable Autoread
		LCD_Write_Command(0xE0);                  //Control OTP/MTP
	    LCD_Write_OneData(0x00);                //Read Mode
		USER_DelayDTms(10);                      //delay 10mS
		LCD_Write_Command(0xE3);                  //Control OTP/MTP
		USER_DelayDTms(20);                      //delay 20mS
		LCD_Write_Command(0xE1);                  //Close Read Mode
	
	 /*************SLEEP OUT*********************************/
		LCD_Write_Command(0x11);                  //Sleep Out
	    LCD_Write_Command(0x28);                  //display off
	    USER_DelayDTms(50);
		//LCD_Write_Command(0x20);                  //Normal Display
	 /*************vop setting*********************************/
		LCD_Write_Command(0xC0);                  //Set Vop by initial Module
	    LCD_Write_OneData(0xf9);                //  vop=14v  (04)
		LCD_Write_OneData(0x00);                 // base on Module
	   // LCD_Write_OneData(0x00);
	   LCD_Write_Command(0xc1);
	   LCD_Write_Command(0xc2);
	   LCD_Write_Command(0xC7);              //VopOffset
	   LCD_Write_OneData(0x00);         //
	   LCD_Write_OneData(0x00);         //


	/**************set register*************************************/
	    LCD_Write_Command(0xc3);                // bias select
	    LCD_Write_OneData(0x05);                //1/9 bias
		LCD_Write_Command(0xC4);                  //Setting Booster times
	    LCD_Write_OneData(0x07);                // Booster X 8
		LCD_Write_Command(0xc5);                  //Booster  eff
	    LCD_Write_OneData(0x21);                // BE = 0x01 (Level 2)
		LCD_Write_Command(0xCb);                  //Vg with booster x2 control
	    LCD_Write_OneData(0x01);                //Vg from Vdd2
	    LCD_Write_Command(0xcc);               // ID1 = 00
	    LCD_Write_OneData(0x00);
		LCD_Write_Command(0xCE);                  //ID3 = 00
	    LCD_Write_OneData(0x00);                //
		LCD_Write_Command(0xB7);                  //COM/SEG Direction for glass
	   	LCD_Write_OneData(0x00);                // Setting by LCD module  (48)
	    LCD_Write_Command(0xD0);                  //Analog circuit setting
	   	LCD_Write_OneData(0x1d);                //
	
		LCD_Write_Command(0xb5);                  //N-Line
	    LCD_Write_OneData(0x8d);                // Non-RST, 14-line inversion
	
		LCD_Write_Command(0xd7);                  //Auto read Set
	    LCD_Write_OneData(0x9f);                //  OTP Disable
	
		LCD_Write_Command(0xb4);                  //PTL Mode Select
	    LCD_Write_OneData(0x18);                //  PTLMOD à Normal Mode
		LCD_Write_Command(0x3a);                  //Color mode = 65k
	    LCD_Write_OneData(0x05);                //
	  	LCD_Write_Command(0x36);                  //Memory Access Control
		LCD_Write_OneData(0xc8);                //Setting by LCD module
	
		LCD_Write_Command(0xb0);                  //Duty = 160 duty
	    LCD_Write_OneData(0x9f);                //
	
		LCD_Write_Command(0x20);                  //Display Inversion OFF
	
	
		LCD_Write_Command(0xf7);                  //command for temp sensitivity.
	  	LCD_Write_Command(0x06);                  //
	    LCD_Write_Command(0x29);                 //display on
	
	
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
