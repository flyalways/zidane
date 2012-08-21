

void LCM_set_view(U8 view_type, U16 x_start, U16 y_start, U16 x_size, U16 y_size)
  {
		data	U8 x_end,y_end,mirrored_y_start,mirrored_y_end;
		data	U8	tc_Dir;
	
		LCD_Nand2DataMode();
	
		x_end = x_start + x_size - 1;
		y_end = y_start + y_size - 1;
	
		switch(view_type)
		{
			case	0:	// HZK
				mirrored_y_start = y_start;
				mirrored_y_end = y_end;
				y_start=x_start;
				y_end=x_end;
				x_start	= mirrored_y_start;
				x_end= 	mirrored_y_end;				
			//	tc_Dir=0x50;//’˝œ‘
				tc_Dir=0x90;//µπœ‘
	
			break;
	
			case	1:	// ICON// JPG
			case	2:	
			//	tc_Dir=0x70;//’˝œ‘
				tc_Dir=0xb0;//µπœ‘
	
			break;
	
			case	3:	// BMP
				mirrored_y_start = TFT_V_Size-1-y_end;
				mirrored_y_end = TFT_V_Size-1-y_start;
				y_start=mirrored_y_start;
				y_end=mirrored_y_end;
			//	tc_Dir=0x30;//’˝œ‘
				tc_Dir=0xf0;//µπœ‘
	
			break;
		}
	
		//Set Start X.Y address
		LCD_Write_Command(0x2A); // set X address
		LCD_Write_OneWord(x_start);
		LCD_Write_OneWord(x_end);
		LCD_Write_OneWord(0x00);
		LCD_Write_OneWord(0x00);
		
		LCD_Write_Command(0x2B); // set Y address
		LCD_Write_OneWord(y_start);
		LCD_Write_OneWord(y_end);
		LCD_Write_OneWord(0x00);
		LCD_Write_OneWord(0x00);
	
		//SetScanDirection
		LCD_Write_Command(0x36);		
		LCD_Write_OneData(tc_Dir);
	  	LCD_Write_Command(0x2C);//Memory Writ
	
		//Switch back to NAND I/F	  
		LCD_Data2NandMode();
  }

