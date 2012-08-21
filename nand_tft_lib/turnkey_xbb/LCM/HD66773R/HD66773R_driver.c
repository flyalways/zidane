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
		/*				
			
			LCD_Write_Command( 0x00); 
			LCD_Write_Command( 0x01); 
			LCD_Write_OneData( 0x07); 
			LCD_Write_OneData( 0x0c);
			*/
			
			LCD_Write_Command( 0x00); 
			LCD_Write_Command(0x05);

			LCD_Write_OneData( 0x10); 
			LCD_Write_OneData( 0x30);
			//tc_Dir=0x58;//0x98
		break;

		case	1:	// ICON// JPG
		case	2:	
			/*
			LCD_Write_Command( 0x00); 
			LCD_Write_Command( 0x01); 
			LCD_Write_OneData( 0x07); 
			LCD_Write_OneData( 0x0c);
			*/
			LCD_Write_Command( 0x00); 
			LCD_Write_Command(0x05);

			LCD_Write_OneData( 0x10); 
			LCD_Write_OneData( 0x38);		

		break;

		case	3:	// BMP

			LCD_Write_Command(0x00); 
			LCD_Write_Command(0x05);

			LCD_Write_OneData( 0x10); 
			LCD_Write_OneData( 0x2e);	
			//tc_Dir=0x38;//0xF8
		break;
	}
//if(view_type)
	//Set Start X.Y address
	
	LCD_Write_Command( 0x00); 
     LCD_Write_Command( 0x17); // set X address
	
	 LCD_Write_OneData(x_end);
	 LCD_Write_OneData(x_start);


	 LCD_Write_Command( 0x00); 
     LCD_Write_Command(0x16); // set Y address 	
	 LCD_Write_OneData(y_end);
	 LCD_Write_OneData(y_start);


	//SetScanDirection
	LCD_Write_Command( 0x00);
	LCD_Write_Command(0x21); 
	LCD_Write_OneData(x_start);
	LCD_Write_OneData(y_start);
	LCD_Write_Command( 0x00);
	LCD_Write_Command(0x22);
	  
	//Switch back to NAND I/F	  
	LCD_Data2NandMode();	 
}
/*
#else

#endif */