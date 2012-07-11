#include "SPDA2K.h"
extern xdata U8  gc_RespBuff[17];
 

void SD_Read_LBA_Dos(U32 Read_LBA ,U8 Buf_Index)
{
	U16 tw_SD_CountDown;
	
	LBA.LW = Read_LBA;	
	XBYTE[0xB400]=0x06;
	
	XBYTE[0xB45B]=0x51;	// CMD17
	if(gbt_SDHC_Type==1)
	{                                                                                 
		XBYTE[0xB45C]=LBA.BY[0];                                                        
		XBYTE[0xB45D]=LBA.BY[1];	// relative address 1                             
		XBYTE[0xB45E]=LBA.BY[2];	// 512Bytes/Block                          
		XBYTE[0xB45F]=LBA.BY[3];	// Sectors Address                
	}                                                                                                   
	else
	{                                                                                               
		XBYTE[0xB45C]=(LBA.BY[1]<<1)+(LBA.BY[2]>>7);
		XBYTE[0xB45D]=(U8)(LBA.WD[1]>>7);	// relative address 1                
		XBYTE[0xB45E]=(LBA.BY[3]<<1);		// 512Bytes/Block                     
		XBYTE[0xB45F]=0x00;					// Sectors Address                    
	}                                                                                                   

	tw_SD_CountDown=SDMMCTimeOutLoop_10000;
	while(1)
	{
    	SD_Send_DummyCLK(8);
		if((XBYTE[0xB453]&0x20)==0x20)
		{
			break;
		}

        if((--tw_SD_CountDown==0)||(SD_Detect)) 
		{
			goto RET;
		}
	}

    SD_TrigCMD_Receive_Respond();  
	XBYTE[0xB455]=0x00;
	XBYTE[0xB456]|=0x02;
	Device_Xfer_DataLength.WD=0x01FF;                                                                 
	Device_Read_SRAM_Index.BY[0]=Buf_Index;			             
	Device_Read_SRAM_Index.BY[1]=0x00; 
	SET_DEVICE_READ();                                                    
	Trigger_Data_Transfer();                                                        
	tw_SD_CountDown=SDMMCTimeOutLoop_10000;    
	while((!(XBYTE[0xB3C0]&0x01))||(XBYTE[0xB454]&0x0F))
	{
		//Wait Data transfer done
		if((--tw_SD_CountDown==0)||(SD_Detect))
		{
			goto RET;
		}
	}
			                                                                                            
	XBYTE[0xB3C0]=0x00;
 	tw_SD_CountDown=SDMMCTimeOutLoop_10000;
 	while(1)	// Wait Ready
	{
 		SD_Send_DummyCLK(8);
		if((XBYTE[0xB453]&0x20)==0x20)
		{
			break;
		}

		if((--tw_SD_CountDown==0)||(SD_Detect))
		{
			goto RET;
		}
	}
	XBYTE[0xB400] = 0x01;
	return;
RET:
	XBYTE[0xB400] = 0x01;
	USER_DelayDTms(150);
}


void SD_Write_LBA_Dos(U32 Write_LBA,U8 Buf_Index,U8 Write_Stage)
{
	xdata	U16	tw_SD_CountDown;                                                                              
	data	bit tbt_No_SD_Stop_CMD=0;    
	data	bit	tbt_SD_Start_Stage=0;	
	data	bit	tbt_SD_Data_Stage=0;
	data	bit tbt_SD_Stop_Stage=0;                                                     

	LBA.LW =Write_LBA;
	switch(Write_Stage)
	{
		case 0: // CMD+Data
			tbt_SD_Start_Stage 	= 1;
			tbt_SD_Data_Stage	= 1;
//			tbt_SD_Stop_Stage	= 0;
		break;
		
		case 1: // only data
//			tbt_SD_Start_Stage 	= 0;
			tbt_SD_Data_Stage	= 1;
//			tbt_SD_Stop_Stage	= 0;	
		break;

		case 2: //data + Stop CMD
//			tbt_SD_Start_Stage 	= 0;
			tbt_SD_Data_Stage	= 1;
			tbt_SD_Stop_Stage	= 1;	
		break;

		case 3:// only 1 sector
			tbt_SD_Start_Stage	= 1;
			tbt_SD_Data_Stage	= 1;
//			tbt_SD_Stop_Stage	= 0;
			tbt_No_SD_Stop_CMD	= 1;
		break;

		default:  //only  Stop cmd
//			tbt_SD_Start_Stage	= 0;
//			tbt_SD_Data_Stage	= 0;
			tbt_SD_Stop_Stage 	= 1;
		break;
	}	

	XBYTE[0xB400]=0x06;
	if(tbt_SD_Start_Stage==1)
	{
		if(tbt_No_SD_Stop_CMD==1)
		{
			XBYTE[0xB45B]=0x58;	// CMD24
		}
		else
		{
			XBYTE[0xB45B]=0x59;	// CMD25
		}

		if(gbt_SDHC_Type==1)
		{                                                                                 
			XBYTE[0xB45C]=LBA.BY[0];                                                        
			XBYTE[0xB45D]=LBA.BY[1];	// relative address 1                             
			XBYTE[0xB45E]=LBA.BY[2];	// 512Bytes/Block                          
			XBYTE[0xB45F]=LBA.BY[3];	// Sectors Address                
		}                                                                                                   
		else
		{                                                                                               
			XBYTE[0xB45C]=(LBA.BY[1]<<1)+(LBA.BY[2]>>7);
			XBYTE[0xB45D]=(U8)(LBA.WD[1]>>7);	// relative address 1                
			XBYTE[0xB45E]=(LBA.BY[3]<<1);		// 512Bytes/Block                     
			XBYTE[0xB45F]=0x00;					// Sectors Address                    
		}                                                                                              
		tw_SD_CountDown=65536;                                                        
		while(!(XBYTE[0xB453]&0x20))
		{
			SD_Send_DummyCLK(8);
			if((XBYTE[0xB453]&0x20)==0x20)
			{
				break;
			}

			if((--tw_SD_CountDown==0)||(SD_Detect))
			{
				break;
			}
		}
		SD_TrigCMD_Receive_Respond();
	}

	if(tbt_SD_Data_Stage==1)
	{
		XBYTE[0xB455]=0x00;
		XBYTE[0xB456]|=0x02;
		Device_Xfer_DataLength.WD=0x01FF;
		Device_Write_SRAM_Index.BY[0]=Buf_Index;
		Device_Write_SRAM_Index.BY[1]=0x00;
		SET_DEVICE_WRITE();                                                                                 
		Trigger_Data_Transfer();
		tw_SD_CountDown=SDMMCTimeOutLoop_10000;
		while((!(XBYTE[0xB3C0]&0x01))||(XBYTE[0xB454]&0x0F))
		{
			if((--tw_SD_CountDown==0)||(SD_Detect))
			{
				break;
			}
		}
		XBYTE[0xB3C0] = 0x00;			                                                                    
		XBYTE[0xB452] = 0x40;	
		while(XBYTE[0xB454]&0x0F);
	}

	if(tbt_SD_Stop_Stage==1)
	{
    	SD_Cmd12_Stop();                                                                            
 		XBYTE[0xB452]=0x40;	
 		while(XBYTE[0xB454]&0x0F);                            
 		SD_Send_DummyCLK(8); 	                                                                            
 	}                                                                                               
	XBYTE[0xB400]=0x01;
}


void SD_ReadSectors_USB(void)
{
	bit	tbt_No_SD_Stop_CMD;
	U16	tw_SD_CountDown;
	                                                                     
	gbt_OddTemp=0;
	XBYTE[0xB400]=0x06;
	XBYTE[0xB450]=0x03;

	if(TotalXferPageNum.WD==1)
	{
		XBYTE[0xB45B]=0x51;	// CMD17
		tbt_No_SD_Stop_CMD=1;
	}
	else
	{
		XBYTE[0xB45B]=0x52;	// CMD18
		tbt_No_SD_Stop_CMD=0;
	}

	if(gbt_SDHC_Type==1)
	{                                                                                 
		XBYTE[0xB45C]=LBA.BY[0];
		XBYTE[0xB45D]=LBA.BY[1];	// relative address 1
		XBYTE[0xB45E]=LBA.BY[2];	// 512Bytes/Block
		XBYTE[0xB45F]=LBA.BY[3];	// Sectors Address
	}                                                                                                   
	else
	{                                                                                               
		XBYTE[0xB45C]=(LBA.BY[1]<<1)+(LBA.BY[2]>>7);
		XBYTE[0xB45D]=(U8)(LBA.WD[1]>>7);	// relative address 1
		XBYTE[0xB45E]=(LBA.BY[3]<<1);		// 512Bytes/Block
		XBYTE[0xB45F]=0x00;					// Sectors Address
	} 

	tw_SD_CountDown=SDMMCTimeOutLoop_10000;
	while(1)
	{
    	SD_Send_DummyCLK(8);
		if((XBYTE[0xB453]&0x20)==0x20)
		{
			break;
		}

        if((--tw_SD_CountDown==0)||(SD_Detect)) 
		{
			break;
		}
	}
                     
    SD_TrigCMD_Receive_Respond();
	XBYTE[0xB455]=0x00;
	XBYTE[0xB456]|=0x02;
	Device_Xfer_DataLength.WD=0x01FF;
	Device_Read_SRAM_Index.BY[0]=USB_BUF_Bank0_HIGH;
	Device_Read_SRAM_Index.BY[1]=0x00;
	Reset_DMA();
	SET_DEVICE_READ();                                                                                  
	TotalXferPageNum.WD--;	                                                                        
	Trigger_Data_Transfer();                                                                            
	while(TotalXferPageNum.WD) 
	{
		tw_SD_CountDown=SDMMCTimeOutLoop_10000;                                                          
		while((!(XBYTE[0xB3C0]&0x01))||(XBYTE[0xB454]&0x0F))
		{
			if((--tw_SD_CountDown==0)||(SD_Detect))
			{
				break;
			}
		}
		XBYTE[0xB3C0]=0x00;
		if(!gbt_OddTemp)
		{	// transfer data to usb                                                     
			while(XBYTE[0xB5A2]&0x01);                                                                 
			XBYTE[0xB5C2]&=0xEF;	//Bulk-In Tx done                       
			XBYTE[0xB5E9] = 0x01;	//Clear Bulk-In(EP1) Buffer
			XBYTE[0xB516] = 0x02;	//512Byte
			XBYTE[0xB515] = 0x00;
			XBYTE[0xB5A1] = 0x01;	//Allow Bulk-In Tx.
			XBYTE[0xB115] = USB_BUF_Bank1_HIGH;
			if(gbt_USBHostIsHighSpeed)
			{	//USB 2.0                                                     
//				XBYTE[0xB5E9] = 0x01;	//Clear Bulk-In(EP1) Buffer
//				XBYTE[0xB516] = 0x02;	//512Byte
//				XBYTE[0xB515] = 0x00;
//				XBYTE[0xB5A1] = 0x01;	//Allow Bulk-In Tx.
//				XBYTE[0xB115] = USB_BUF_Bank1_HIGH;
			}                                                                                           
			else 
			{	//USB 1.1                                                                            
				XBYTE[0xB510] = 0x01;							//BulkInEn auto turn-on(dma over)       
//				XBYTE[0xB5E9] = 0x01;							//Clear Bulk-In(EP1) Buffer.            
//				XBYTE[0xB516] = 0x02;							//512Byte                               
//				XBYTE[0xB515] = 0x00;                                                                   
//				XBYTE[0xB5A1] = 0x01;							//Allow Bulk-In Tx.                     
//				XBYTE[0xB115] = USB_BUF_Bank1_HIGH;				// 080104 joyce add                     
				while (XBYTE[0xB5A1]);							//Bulk-In Tx done                       
				XBYTE[0xB510] = 0x00;							//BulkInEn nonauto turn-on              
			}                                                                                           
		}                                                                                               
		else 
		{                                                                                          
			while (XBYTE[0xB5A1]&0x01);							//Bulk-In Tx done                       
			XBYTE[0xB5C2]   &= 0xFE;	                                                                
			XBYTE[0xB5E9] = 0x10;							//Clear Bulk-In(EP1) Buffer.            
			XBYTE[0xB518] = 0x02;							//512Byte                               
			XBYTE[0xB517] = 0x00;                                                                   
			XBYTE[0xB5A2] = 0x01;							//Allow Bulk-In Tx.				        
			XBYTE[0xB115] = USB_BUF_Bank0_HIGH;				// 080104 joyce add                     
			if (gbt_USBHostIsHighSpeed) 
			{ //USB 2.0                                                     
//				XBYTE[0xB5E9] = 0x10;							//Clear Bulk-In(EP1) Buffer.            
//				XBYTE[0xB518] = 0x02;							//512Byte                               
//				XBYTE[0xB517] = 0x00;                                                                   
//				XBYTE[0xB5A2] = 0x01;							//Allow Bulk-In Tx.				        
//				XBYTE[0xB115] = USB_BUF_Bank0_HIGH;				// 080104 joyce add                     
			}                                                                                           
			else 
			{ //USB 1.1                                                                            
				XBYTE[0xB510] = 0x01;							//BulkInEn auto turn-on(dma over)       
//				XBYTE[0xB5E9] = 0x10;							//Clear Bulk-In(EP1) Buffer.            
//				XBYTE[0xB518] = 0x02;							//512Byte                               
//				XBYTE[0xB517] = 0x00;                                                                   
//				XBYTE[0xB5A2] = 0x01;							//Allow Bulk-In Tx.				        
//				XBYTE[0xB115] = USB_BUF_Bank0_HIGH;
				while (XBYTE[0xB5A2]);							//Bulk-In Tx done                       
				XBYTE[0xB510] = 0x00;							//BulkInEn nonauto turn-on              
			}                                                                                           
		}                                                                                               
		gbt_OddTemp = !(gbt_OddTemp);                                                                   
		(TotalXferPageNum.WD)--;	                                                                    
		Trigger_Data_Transfer();                                                                        
	}                                                                                                   
	tw_SD_CountDown=SDMMCTimeOutLoop_10000;                                                              
	while((!(XBYTE[0xB3C0] & 0x01))||(XBYTE[0xB454] & 0x0F))
	{								//Wait Data xfer done                   
		if((--tw_SD_CountDown==0)||(SD_Detect))
		{
			break;
		}
	}		                                                                                            
	XBYTE[0xB3C0] = 0x00;	                                                                            
	if(!gbt_OddTemp) 
	{
		while(XBYTE[0xB5A2]&0x01);		//Bulk-In Tx done                       
		XBYTE[0xB510] = 0x01;			//BulkInEn auto turn-on(dma over)       
		XBYTE[0xB5E9] = 0x01;			//Clear Bulk-In(EP1) Buffer.            
		XBYTE[0xB516] = 0x02;			//512Byte                               
		XBYTE[0xB515] = 0x00;                                                                           
		XBYTE[0xB5A1] = 0x01;			//Allow Bulk-In Tx.                     
		while (XBYTE[0xB5A1]);			//Bulk-In Tx done                       
		XBYTE[0xB510] = 0x00;			//BulkInEn nonauto turn-on              
	}                                                                                                   
	else
	{
		while (XBYTE[0xB5A1]&0x01);		//Bulk-In Tx done                       
		XBYTE[0xB510] = 0x01;			//BulkInEn auto turn-on(dma over)       
		XBYTE[0xB5E9] = 0x10;			//Clear Bulk-In(EP1) Buffer.            
		XBYTE[0xB518] = 0x02;			//512Byte                               
		XBYTE[0xB517] = 0x00;
		XBYTE[0xB5A2] = 0x01;			//Allow Bulk-In Tx.                     
		while (XBYTE[0xB5A2]);			//Bulk-In Tx done                       
		XBYTE[0xB510] = 0x00;			//BulkInEn nonauto turn-on              
	}	                                                                                                
	
	if(!tbt_No_SD_Stop_CMD)
	{                                                                            
		SD_Cmd12_Stop();                                                                                
	}                                                                                                   

	tw_SD_CountDown=SDMMCTimeOutLoop_10000;                                                              
	while(1)
	{
    	SD_Send_DummyCLK(8);
		if((XBYTE[0xB453]&0x20)==0x20)
		{
			break;
		}

        if((--tw_SD_CountDown==0)||(SD_Detect)) 
		{
			break;
		}
	}
 	XBYTE[0xB400] = 0x01;
}                                                                                                       
                                                                                                      

void SD_WriteSectors_USB(void)
{                                                                         
	U16 tw_SD_CountDown;
	bit tbt_No_SD_Stop_CMD=0;
	gbt_OddTemp =0;
	
	XBYTE[0xB400] = 0x06;                                                               
	XBYTE[0xB450] = 0x03;                                                               

	// USB-->SRAM Bank 0	// 需要先收一筆data否則會一直被Timeout                                      
	if (gbt_USBHostIsHighSpeed) 
	{
		XBYTE[0xB512]=0x01;
	}
	else
	{
		XBYTE[0xB512]=0x00;
	}
	XBYTE[0xB5D2] &= 0xFD;		//Disable Bulk-Out中斷                  
	XBYTE[0xB5E9] = 0x02;		// USB-->SRAM                           
	XBYTE[0xB516] = 0x02;		//512Byte                               
	XBYTE[0xB515] = 0x00;
	XBYTE[0xB5A1] = 0x02;		//Allow Bulk-Out Tx.	                

	if(TotalXferPageNum.WD==1)
	{
		XBYTE[0xB45B]=0x58;	// CMD24
		tbt_No_SD_Stop_CMD=1;
	}
	else
	{
		XBYTE[0xB45B]=0x59;	// CMD25
		tbt_No_SD_Stop_CMD=0;
	}

	if(gbt_SDHC_Type)
	{                                                                                 
		XBYTE[0xB45C] = LBA.BY[0];
		XBYTE[0xB45D] = LBA.BY[1];
		XBYTE[0xB45E] = LBA.BY[2];
		XBYTE[0xB45F] = LBA.BY[3];
	}                                                                                                   
	else
	{
		XBYTE[0xB45C] = (LBA.BY[1]<<1)+(LBA.BY[2]>>7);
		XBYTE[0xB45D] = (U8)(LBA.WD[1]>>7);
		XBYTE[0xB45E] = (LBA.BY[3]<<1);
		XBYTE[0xB45F] =  0x00;
	}                                                                                                   
    tw_SD_CountDown=SDMMCTimeOutLoop_10000;
	while(1)
	{
    	SD_Send_DummyCLK(8);
		if((XBYTE[0xB453]&0x20)==0x20)
		{
			break;
		}

        if((--tw_SD_CountDown==0)||(SD_Detect)) 
		{
			break;
		}
	}
             
	SD_TrigCMD_Receive_Respond();        

	XBYTE[0xB455] = 0x00;
	XBYTE[0xB456]|= 0x02;
	Device_Xfer_DataLength.WD = 0x01FF;                                                                 
	Device_Write_SRAM_Index.BY[0] = USB_BUF_Bank0_HIGH;			// 080104 joyce add                     
	Device_Write_SRAM_Index.BY[1] = 0x00;                                                               
	SET_DEVICE_WRITE();                                                                                 
	(TotalXferPageNum.WD)--;  
	while(TotalXferPageNum.WD)
	{
		XBYTE[0xB452] = 0x40;	      
		while(XBYTE[0xB454] & 0x0F);   
		if(gbt_OddTemp) 
		{ //Bank 1-> Flash, USB -> Bank 0                                              
			while(XBYTE[0xB5A2]&0x02);                                                                  
			XBYTE[0xB5C2]   &= 0xDF;                                                                    
			XBYTE[0xB113]    = USB_BUF_Bank1_HIGH;			// 080104 joyce add                         
			XBYTE[0xB112]    = 0x00;                                                                    
			XBYTE[0xB3B0]    = 0x01;							//Trigger Data Xfer                     
			XBYTE[0xB5E9]    = 0x02;							//Clear Bulk-Out(EP2) Buffer            
			XBYTE[0xB516]    = 0x02;							//512Byte                               
			XBYTE[0xB515]    = 0x00;                                                                    
			XBYTE[0xB5A1]    = 0x02;
		}                                                                                               
		else 
		{ //Bank 0 -> Flash, USB -> Bank 1                                                         
			while (XBYTE[0xB5A1] & 0x02);                                                               
			XBYTE[0xB5C2]   &= 0xFD;			                                                        
			XBYTE[0xB113]    = USB_BUF_Bank0_HIGH;			// 080104 joyce mark                        
			XBYTE[0xB112]    = 0x00;                                                                    
			XBYTE[0xB3B0]    = 0x01;							//Trigger Data Xfer                     
			XBYTE[0xB5E9]    = 0x20;							//Clear Bulk-Out(EP2) Buffer            
			XBYTE[0xB518]    = 0x02;							//512Byte                               
			XBYTE[0xB517]    = 0x00;                                                                    
			XBYTE[0xB5A2]    = 0x02;
		}		                                                                                        
		gbt_OddTemp = !(gbt_OddTemp);	                                                                
		(TotalXferPageNum.WD)--;                               
		tw_SD_CountDown=SDMMCTimeOutLoop_10000; 
		while((!(XBYTE[0xB3C0] & 0x01))||(XBYTE[0xB454] & 0x0F))
		{
	        if((--tw_SD_CountDown==0)||(SD_Detect))
		    {
				break;
	    	}
		} 
		XBYTE[0xB3C0] = 0x00;		                                                                    
	}// end while                                                                                       
	XBYTE[0xB452] = 0x40;	      
	while(XBYTE[0xB454] & 0x0F);   
	
	if (gbt_OddTemp) 
	{ //Bank 1-> Flash, USB -> Bank 0
		while(XBYTE[0xB5A2]&0x02);                                                                      
		XBYTE[0xB5C2]   &= 0xDF;                                                                        
		XBYTE[0xB113]    = USB_BUF_Bank1_HIGH;
		XBYTE[0xB112]    = 0x00;                                                                        
		XBYTE[0xB3B0]    = 0x01;	//Trigger Data Xfer                         
	}
	else 
	{ //Bank 0 -> Flash, USB -> Bank 1                                                             
		while (XBYTE[0xB5A1] & 0x02);                                                                   
		XBYTE[0xB5C2]   &= 0xFD;			                                                            
		XBYTE[0xB113]	 = USB_BUF_Bank0_HIGH;
		XBYTE[0xB112]    = 0x00;                                                                        
		XBYTE[0xB3B0]    = 0x01;	//Trigger Data Xfer                                             
	}			                                                                                        

	tw_SD_CountDown=SDMMCTimeOutLoop_10000;                                                              
	while((!(XBYTE[0xB3C0] & 0x01))||(XBYTE[0xB454] & 0x0F))
	{
        if((--tw_SD_CountDown==0)||(SD_Detect))
	    {
			break;
    	}
	}                                                                                               
	XBYTE[0xB3C0] = 0x00;
	XBYTE[0xB452] = 0x40;
	while(XBYTE[0xB454] & 0x0F)
	{
		if(SD_Detect)
		{                                                               
			break;
		}     
	}
	
	if(!tbt_No_SD_Stop_CMD)
	{
		SD_Cmd12_Stop();
	}
 	XBYTE[0xB452] = 0x40;
 	while(XBYTE[0xB454]&0x0F);
 	SD_Send_DummyCLK(8); 	                                                                            
	XBYTE[0xB400] = 0x01;
}                                                                                                       