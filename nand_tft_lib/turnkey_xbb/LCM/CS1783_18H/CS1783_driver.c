//#ifdef TFT_18H
void LCM_set_view(U8 view_type, U16 x_start, U16 y_start, U16 x_size, U16 y_size)
{
	data	U8 x_end,y_end;
	data	U8	tc_Dir;

	LCD_Nand2DataMode();

	x_end = x_start + x_size - 1;
	y_end = y_start + y_size - 1;

	switch(view_type)
	{
		case	0:	// HZK
		
			//tc_Dir=0x01;//正显//(扫描方向，首先128的必须反过来，160的也要反过来，才会是倒显)
			tc_Dir = 0x02;//倒显

		break;

		case	1:	// ICON// JPG
		case	2:	
			//tc_Dir=0x05;
			tc_Dir =0x06;//倒显
		break;

		case	3:	// BMP
		
			//tc_Dir=0x06;
				tc_Dir=0x05;//倒显

		break;
	}

	
	//Set Start X.Y address


	LCD_Write_Command(0x75); // set X address
	LCD_Write_OneData(x_start);
	LCD_Write_OneData(x_end);
	
	LCD_Write_Command(0x15); // set Y address
	LCD_Write_OneData(y_start);
	LCD_Write_OneData(y_end);
	//SetScanDirection

	LCD_Write_Command(0xBC);		
	LCD_Write_OneData(tc_Dir);
	LCD_Write_OneData(0x01);
	LCD_Write_OneData(0x00);

  	LCD_Write_Command(0x5C);//Memory Writ

	  
	//Switch back to NAND I/F	  
	LCD_Data2NandMode();	  
}
