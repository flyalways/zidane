
//===============================小LCM========================================
//===============================小LCM========================================
//===============================小LCM========================================
#if 1

//#define DaoXian



//电阻在R2上

	void LCM_set_view(U8 view_type, U16 x_start, U16 y_start, U16 x_size, U16 y_size)
	{
		data	U8 x_end,y_end,mirrored_y_start,mirrored_y_end,mirrored_x_start;
		data	U8	tc_Dir;

		LCD_Nand2DataMode();


		x_end = x_start + x_size - 1;
		y_end = y_start + y_size - 1;



	switch(view_type)
	{
		case	0:	// HZK	
			
			#ifdef DaoXian		
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x00);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x25);
			#else	
		    //倒显
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x08);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x23);
		#endif 

		break;

		case	1:	// ICON// JPG
		case	2:	
			#ifdef DaoXian	
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x02);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x25);
			
			#else//倒	
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x0a);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x22);
			#endif
		break;

		case	3:	// BMP
			#ifdef DaoXian	
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x02);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x20);
			#else//倒

			LCD_Write_Command(0x40);
			LCD_Write_Command(0x0a);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x27);
			#endif
		break;
	}
	#ifdef DaoXian	
	//Set Start X.Y address
	LCD_Write_Command(0x42); // set X address
	LCD_Write_Command(x_start);
	LCD_Write_Command(x_end);
	
	LCD_Write_Command(0x43); // set Y address
	LCD_Write_Command(y_start+2);
	LCD_Write_Command(y_end+2);
	#else
		//Set Start X.Y address
	LCD_Write_Command(0x42); // set X address
	LCD_Write_Command(x_start+2);
	LCD_Write_Command(x_end+2);
	
	LCD_Write_Command(0x43); // set Y address
	LCD_Write_Command(y_start+2);
	LCD_Write_Command(y_end+2);
	#endif

	  
	//Switch back to NAND I/F	  
	LCD_Data2NandMode();	  
}


#else	
//========================================大LCM=================================
//========================================大LCM=================================
//========================================大LCM=================================
//#define DaoXian

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
			#ifndef DaoXian
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x01);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x24);
			#else
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x01);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x21);
			#endif
				

		break;

		case	1:	// ICON// JPG
		case	2:	
		#ifndef DaoXian
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x02);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x24);
		#else
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x02);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x21);
		
		#endif

		break;

		case	3:	// BMP
			#ifndef DaoXian
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x02);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x20);
			#else
			LCD_Write_Command(0x40);
			LCD_Write_Command(0x02);

			LCD_Write_Command(0x10);
			LCD_Write_Command(0x25);
			#endif
		break;
	}
//Set Start X.Y address
	LCD_Write_Command(0x42); // set X address
	LCD_Write_Command(x_start+1);
	LCD_Write_Command(x_end+1);
	
	LCD_Write_Command(0x43); // set Y address
	LCD_Write_Command(y_start+2);
	LCD_Write_Command(y_end+2);

	  
	//Switch back to NAND I/F	  
	LCD_Data2NandMode();	  
}
#endif
