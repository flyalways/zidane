//#ifdef TFT_18H
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
			tc_Dir=0x58;
		break;

		case	1:	// ICON
			tc_Dir=0x78;
		break;

		case	2:	// JPG
			tc_Dir=0x78;
		break;

		case	3:	// BMP
			mirrored_y_start = TFT_V_Size-1-y_end+4;
			mirrored_y_end = TFT_V_Size-1-y_start+4;
			y_start=mirrored_y_start;
			y_end=mirrored_y_end;
			tc_Dir=0xf8;
		break;
	}

	//Set Start X.Y address
	LCD_Write_Command(0x2A); // set X address
	LCD_Write_OneWord(x_start);
	LCD_Write_OneWord(x_end);
	LCD_Write_Command(0x2B); // set Y address
	LCD_Write_OneWord(y_start+2);
	LCD_Write_OneWord(y_end+2);
	//SetScanDirection
	LCD_Write_Command(0x36);		
	LCD_Write_OneData(tc_Dir);
  	LCD_Write_Command(0x2C);//Memory Write
	  
	//Switch back to NAND I/F	  
	LCD_Data2NandMode();	  
}
/*
#else

#endif */