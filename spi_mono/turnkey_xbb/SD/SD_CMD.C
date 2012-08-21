#include "..\header\SPDA2K.h"
#include "..\header\variables.h"

void SD_Cmd12_Stop(void)
{
	XBYTE[0xB45B]=0x4C;
	XBYTE[0xB45C]=0x00;
	XBYTE[0xB45D]=0x00;
	XBYTE[0xB45E]=0x00;
	XBYTE[0xB45F]=0x00;
    SD_TrigCMD_Receive_Respond(); 
}


void SD_Read_LBA_Dos(U32 Read_LBA ,U8 Buf_Index)
{
	xdata	U16	tw_SD_CountDown;

	LBA.LW = Read_LBA;	
	XBYTE[0xB400] = 0x06;

	//SD/MMC 的準備階段開始
    XBYTE[0xB45B]=0x40+17;
    if(gb_SDHC_Type)
	{                                                                                 
		XBYTE[0xB45C]=LBA.BY[0];
		XBYTE[0xB45D]=LBA.BY[1];
		XBYTE[0xB45E]=LBA.BY[2];
		XBYTE[0xB45F]=LBA.BY[3];
	}                                                                                                   
    else
	{                                                                                               
		XBYTE[0xB45C]=(LBA.BY[1]<<1)+(LBA.BY[2]>>7);
		XBYTE[0xB45D]=(U8)(LBA.WD[1]>>7);
		XBYTE[0xB45E]=(LBA.BY[3]<<1);
		XBYTE[0xB45F]=0x00;
   	}                                                                                                   
	tw_SD_CountDown=SDMMCTimeOutLoop_10000;
	SD_Send_DummyCLK(1);
    while(!(XBYTE[0xB453]&0x20))
	{                  		  // Wait Ready                             
    	SD_Send_DummyCLK(1);
        if(--tw_SD_CountDown==0)  
		{
			goto RET;
		}

		if(gb_SD_pin)
		{                                                               
			goto RET;
		} 
    } 
                    
    SD_TrigCMD_Receive_Respond();  
	XBYTE[0xB455] = 0x00;
	XBYTE[0xB456] |=0x02;
	Device_Xfer_DataLength.WD = 0x01FF;                                                                 
	Device_Read_SRAM_Index.BY[0] = Buf_Index;			             
	Device_Read_SRAM_Index.BY[1] = 0x00; 
	SET_DEVICE_READ();                                                    
	Trigger_Data_Transfer();                                                        
	tw_SD_CountDown=SDMMCTimeOutLoop_10000; 

	while((!(XBYTE[0xB3C0]&0x01))||(XBYTE[0xB454]&0x0F))
	{
		if(--tw_SD_CountDown==0)  
		{
			goto RET;
		}

		if(gb_SD_pin)
		{                                                               
			goto RET;
		}   		
	}		                                                                                            
			       
	XBYTE[0xB3C0] = 0x00;                                                                        
	SD_Send_DummyCLK(1);
 	tw_SD_CountDown=SDMMCTimeOutLoop_10000;                                                              
	while(!(XBYTE[0xB453]&0x20))
	{                  		                              
		SD_Send_DummyCLK(1);
        if(--tw_SD_CountDown==0)
		{
         	goto RET;                                                                                      
        }
		if(gb_SD_pin)
		{                                                               
			goto RET;
		} 	                                                                                            
	}
	return;
RET:
	USER_Delay(200);
}

void SD_TrigCMD_Receive_Respond(void)
{
	XBYTE[0xB452]=0x20;
	SD_Check_Idle();
	XBYTE[0xB450]=0x03;
	XBYTE[0xB456]=0xE0;
	XBYTE[0xB451]&=0xF7;
	XBYTE[0xB452]=0x01;
	SD_Check_Idle(); 
}  


void SD_Check_Idle(void)
{
    xdata	U16	tw_SD_CountDown; 

	tw_SD_CountDown=SDMMCTimeOutLoop_1000;

	while(XBYTE[0xB454] & 0x0F)
	{
		if(--tw_SD_CountDown==0)
		{
			XBYTE[0xB452]=0x00;
			XBYTE[0xB450]=0x03;	// time out reset SD interface  
			return;
		}

		if(gb_SD_pin)
		{
			return;
		}   
	}
}


void SD_Send_DummyCLK(U8 CLK_Num)
{
	data	U8	i;

	for(i=0;i<CLK_Num;i++)
	{
		XBYTE[0xB452]=0x20;                            // 8 clks before send cmd 
    	SD_Check_Idle();
    }         
}


void SD_ReadSectors_USB(void)
{                                                                          
	data	bit	tb_No_SD_Stop_CMD;
	xdata	U16	tw_SD_CountDown;
                                                                         
	gb_OddTemp=0;
	XBYTE[0xB400]=0x06;
	XBYTE[0xB450]=0x03; 
	
	if(TotalXferPageNum.WD==1)
	{
 		XBYTE[0xB45B]=0x40+17;
		tb_No_SD_Stop_CMD=1;
	}
    else
	{
    	XBYTE[0xB45B]=0x40+18;
    	tb_No_SD_Stop_CMD=0;
    }   

    if(gb_SDHC_Type)
	{                                                                                 
		XBYTE[0xB45C]=LBA.BY[0];
		XBYTE[0xB45D]=LBA.BY[1];
		XBYTE[0xB45E]=LBA.BY[2];
		XBYTE[0xB45F]=LBA.BY[3];
	}                                                                                                   
	else
	{
		XBYTE[0xB45C]=(LBA.BY[1]<<1)+(LBA.BY[2]>>7);
		XBYTE[0xB45D]=(U8)(LBA.WD[1]>> 7);
		XBYTE[0xB45E]=(LBA.BY[3]<<1);
		XBYTE[0xB45F]= 0x00;
   	}
                                                                                        
    tw_SD_CountDown=SDMMCTimeOutLoop_10000;
    SD_Send_DummyCLK(1);
    while(!(XBYTE[0xB453]&0x20))
	{
    	SD_Send_DummyCLK(1);                                                                            
        if((--tw_SD_CountDown==0)||(gb_SD_pin))
		{                                                               
		 	break;
		} 
    }
                       
    SD_TrigCMD_Receive_Respond();
 	XBYTE[0xB455]=0x00;
	XBYTE[0xB456]|=0x02;
	Device_Xfer_DataLength.WD = 0x01FF;                                                                 
	Device_Read_SRAM_Index.BY[0]=USB_BUF_Bank0_HIGH;
	Device_Read_SRAM_Index.BY[1] = 0x00;                                                                
	XBYTE[0xB304]=0x09;		//Reset DMA
	XBYTE[0xB304]=0x08;		//Normal DMA
	SET_DEVICE_READ();                                                                                  
	TotalXferPageNum.WD--;
	Trigger_Data_Transfer();                                                                            
	while(TotalXferPageNum.WD)
	{
		tw_SD_CountDown=SDMMCTimeOutLoop_10000;                                                          
		while((!(XBYTE[0xB3C0]&0x01))||(XBYTE[0xB454]&0x0F))
		{
			if((--tw_SD_CountDown==0)||(gb_SD_pin))
			{                                                               
			 	break;
			}                                                            
		}                                                                                         
		
		XBYTE[0xB3C0]=0x00;
		if(gb_OddTemp==0)
		{	// transfer data to usb                                                     
			tw_SD_CountDown=1000;
			while(XBYTE[0xB5A2]&0x01)
			{
				if(--tw_SD_CountDown==0)
				{
					goto USB_remove;
				}
			}
			XBYTE[0xB5C2]&=0xEF;	//Bulk-In Tx done
			XBYTE[0xB5E9]=0x01;	//Clear Bulk-In(EP1) Buffer
			XBYTE[0xB515]=0x00;
			XBYTE[0xB516]=0x02;
			XBYTE[0xB5A1]=0x01;
			XBYTE[0xB115]=USB_BUF_Bank1_HIGH;
			if(gb_USBHostIsHighSpeed) 
			{ //USB 2.0
			}                                                                                           
			else 
			{ //USB 1.1
				XBYTE[0xB510]=0x01;
				while(XBYTE[0xB5A1]);
				XBYTE[0xB510]=0x00;
			}                                                                                           
		}                                                                                               
		else
		{                                                                                          
			tw_SD_CountDown=1000;
			while(XBYTE[0xB5A1]&0x01)
			{
				if(--tw_SD_CountDown==0)
				{
					goto USB_remove;
				}
			}
			XBYTE[0xB5C2]&=0xFE;
			XBYTE[0xB5E9]=0x10;	//Clear Bulk-In(EP1) Buffer
			XBYTE[0xB517]=0x00;
			XBYTE[0xB518]=0x02;
			XBYTE[0xB5A2]=0x01;
			XBYTE[0xB115]=USB_BUF_Bank0_HIGH;
			if(gb_USBHostIsHighSpeed)
			{ //USB 2.0
			}                                                                                           
			else 
			{ //USB 1.1                                                                            
				XBYTE[0xB510]=0x01;
				while(XBYTE[0xB5A2]);                  
				XBYTE[0xB510]=0x00;
			}                                                                                           
		}                                                                                               
		gb_OddTemp=!gb_OddTemp;
		TotalXferPageNum.WD--;
		Trigger_Data_Transfer();                                                                        
	}                                                                                                   

	tw_SD_CountDown=SDMMCTimeOutLoop_10000;                                                              
	while((!(XBYTE[0xB3C0]&0x01))||(XBYTE[0xB454]&0x0F))
	{								//Wait Data xfer done                   
		if((--tw_SD_CountDown==0)||(gb_SD_pin))
		{                                                               
		 	break;
		}                                                                
	}		                                                                                            
	XBYTE[0xB3C0]=0x00;	
 
	tw_SD_CountDown=1000;
	if(!gb_OddTemp)
	{
		while(XBYTE[0xB5A2]&0x01)
		{
			tw_SD_CountDown--;
			if(tw_SD_CountDown==0)
			{
				goto USB_remove;
			}
		}
		XBYTE[0xB510]=0x01;	//BulkInEn auto turn-on(dma over)       
		XBYTE[0xB5E9]=0x01;	//Clear Bulk-In(EP1) Buffer.            
		XBYTE[0xB515]=0x00;	// 512-byte                                                                    
		XBYTE[0xB516]=0x02;
		tw_SD_CountDown=1000;
		XBYTE[0xB5A1]=0x01;
		while(XBYTE[0xB5A1])
		{
			tw_SD_CountDown--;
			if(tw_SD_CountDown==0)
			{
				goto USB_remove;
			}
		}
		XBYTE[0xB510]=0x00;
	}                                                                                                   
	else
	{
		while(XBYTE[0xB5A1]&0x01)
		{
			tw_SD_CountDown--;
			if(tw_SD_CountDown==0)
			{
				goto USB_remove;
			}
		}
		XBYTE[0xB510]=0x01;	// BulkInEn auto turn-on(dma over)
		XBYTE[0xB5E9]=0x10;	// Clear Bulk-In(EP1) Buffer
		XBYTE[0xB517]=0x00;	// 512-byte
		XBYTE[0xB518]=0x02;
		tw_SD_CountDown=1000;
		XBYTE[0xB5A2]=0x01;
		while(XBYTE[0xB5A2])
		{
			tw_SD_CountDown--;
			if(tw_SD_CountDown==0)
			{
				goto USB_remove;
			}
		}
		XBYTE[0xB510]=0x00;
	}	                                                                                                

	if(tb_No_SD_Stop_CMD==0)
	{
		SD_Cmd12_Stop();                                                                                
	}
                                                                                             
 	tw_SD_CountDown=SDMMCTimeOutLoop_10000;                                                              
 	while(!(XBYTE[0xB453]&0x20))
	{
		SD_Send_DummyCLK(1);
        if((--tw_SD_CountDown==0)||(gb_SD_pin))
		{
	      	goto USB_remove;                                                                                      
        }	                                                                                            
	}	                                                                                                
	return;
USB_remove:
	if(tb_No_SD_Stop_CMD==0)
	{
		SD_Cmd12_Stop();
	}
	SD_Send_DummyCLK(100);
	gb_USB_Plugin=0;
	return;
} 
                                                                                                      

void SD_WriteSectors_USB(void)
{                                                                         
	data	bit	tb_No_SD_Stop_CMD;
	xdata	U16	tw_SD_CountDown;
                                                                           
	gb_OddTemp=0;
	XBYTE[0xB400]=0x06;                                                               
	XBYTE[0xB450]=0x03;                                                               

	// USB-->SRAM Bank 0	// 需要先收一筆data否則會一直被Timeout                                      
	XBYTE[0xB512]=(U8)gb_USBHostIsHighSpeed;
	XBYTE[0xB5D2]&=0xFD;// Disable Bulk-Out中斷                  
	XBYTE[0xB5E9]=0x02;	// USB-->SRAM                           
	XBYTE[0xB515]=0x00;	// 512-byte                                                                               
	XBYTE[0xB516]=0x02;
	XBYTE[0xB5A1]=0x02;	// Allow Bulk-Out Tx

	//SD/MMC 的準備階段開始                                                                             
    if(TotalXferPageNum.WD==1)
	{
		// single sector only
    	XBYTE[0xB45B]=0x40|24;
    	tb_No_SD_Stop_CMD=1;                                                                         
    }
    else
	{
    	XBYTE[0xB45B]=0x40|25;
    	tb_No_SD_Stop_CMD=0;                                                                         
    }

    if(gb_SDHC_Type)
	{                                                                                 
		XBYTE[0xB45C]=LBA.BY[0];
		XBYTE[0xB45D]=LBA.BY[1];
		XBYTE[0xB45E]=LBA.BY[2];
		XBYTE[0xB45F]=LBA.BY[3];
    }                                                                                                   
	else
	{
		XBYTE[0xB45C]=(LBA.BY[1]<<1)+(LBA.BY[2]>>7);
		XBYTE[0xB45D]=(U8)(LBA.WD[1]>>7);
		XBYTE[0xB45E]=(LBA.BY[3]<<1);
		XBYTE[0xB45F]=0x00;
    }                                                                                                   
	tw_SD_CountDown=SDMMCTimeOutLoop_10000;
	SD_Send_DummyCLK(1);
	while(!(XBYTE[0xB453]&0x20))
	{	// Wait Ready
		SD_Send_DummyCLK(1);
        if(--tw_SD_CountDown==0)
	    {
			break;
    	}

		if(gb_SD_pin)
		{                                                               
		 	break;
		}     
	}
             
    SD_TrigCMD_Receive_Respond();        
	XBYTE[0xB455]=0x00;
	XBYTE[0xB456]|=0x02;
	Device_Xfer_DataLength.WD=0x01FF;
	Device_Write_SRAM_Index.BY[0]=USB_BUF_Bank0_HIGH;
	Device_Write_SRAM_Index.BY[1] = 0x00;                                                               
	SET_DEVICE_WRITE();                                                                                 
	TotalXferPageNum.WD--;
	while(TotalXferPageNum.WD)
	{
		XBYTE[0xB452]=0x40;
		while(XBYTE[0xB454]&0x0F);
		if(gb_OddTemp)
		{ //Bank1-> Flash, USB -> Bank0
			while(XBYTE[0xB5A2]&0x02);
			XBYTE[0xB5C2]&=0xDF;
			XBYTE[0xB112]=0x00;
			XBYTE[0xB113]=USB_BUF_Bank1_HIGH;
			XBYTE[0xB3B0]=0x01;	// Trigger Data Xfer
			XBYTE[0xB5E9]=0x02;	// Clear Bulk-Out(EP2) Buffer
			XBYTE[0xB515]=0x00;	// 512-byte
			XBYTE[0xB516]=0x02;
			XBYTE[0xB5A1]=0x02;
		}                                                                                               
		else
		{ //Bank0 -> Flash, USB -> Bank1
			while(XBYTE[0xB5A1]&0x02);                                                               
			XBYTE[0xB5C2]&=0xFD;			                                                        
			XBYTE[0xB112]=0x00;                                                                    
			XBYTE[0xB113]=USB_BUF_Bank0_HIGH;
			XBYTE[0xB3B0]=0x01;	// Trigger Data Xfer                     
			XBYTE[0xB5E9]=0x20;	//Clear Bulk-Out(EP2) Buffer            
			XBYTE[0xB517]=0x00;
			XBYTE[0xB518]=0x02;
			XBYTE[0xB5A2]=0x02;
		}		                                                                                        
		gb_OddTemp=!gb_OddTemp;	                                                                
		TotalXferPageNum.WD--;                               
		tw_SD_CountDown=SDMMCTimeOutLoop_10000; 
		while((!(XBYTE[0xB3C0]&0x01))||(XBYTE[0xB454]&0x0F))
		{
	        if(--tw_SD_CountDown==0)
		    {
				break;
	    	}
			if(gb_SD_pin)
			{                                                               
			 	break;
			}     
		} 
		XBYTE[0xB3C0]=0x00;		                                                                    
	}// end while                                                                                       
	XBYTE[0xB452]=0x40;	      
	while(XBYTE[0xB454]&0x0F);   
	                                                                                                    
	if(gb_OddTemp)
	{	//Bank1-> Flash, USB -> Bank0
		while(XBYTE[0xB5A2]&0x02);
		XBYTE[0xB5C2]&=0xDF;
		XBYTE[0xB112]=0x00;
		XBYTE[0xB113]=USB_BUF_Bank1_HIGH;
		XBYTE[0xB3B0]=0x01;	//Trigger Data Xfer                         
	}                                                                                                   
	else
	{	//Bank0 -> Flash, USB -> Bank1
		while(XBYTE[0xB5A1]&0x02);                                                                   
		XBYTE[0xB5C2]&=0xFD;			                                                            
		XBYTE[0xB112]=0x00;
		XBYTE[0xB113]=USB_BUF_Bank0_HIGH;
		XBYTE[0xB3B0]=0x01;	//Trigger Data Xfer                                             
	}			                                                                                        

	tw_SD_CountDown=SDMMCTimeOutLoop_10000;                                                              
	while((!(XBYTE[0xB3C0]&0x01))||(XBYTE[0xB454]&0x0F))
	{
        if(--tw_SD_CountDown==0)
	    {
			break;
    	}
		if(gb_SD_pin)
		{                                                               
		 	break;
		}     
	}                                                                                               
	XBYTE[0xB3C0]=0x00;			                                                                    
	XBYTE[0xB452]=0x40;	
	while(XBYTE[0xB454]&0x0F)
	{
		if(gb_SD_pin)
		{                                                               
		 	break;
		}     
	}
	
	if(!tb_No_SD_Stop_CMD)
	{
    	SD_Cmd12_Stop();                                                                                
    }                                                                                                   
	XBYTE[0xB452]=0x40;
	while(XBYTE[0xB454]&0x0F);
	SD_Send_DummyCLK(1);
}    

void SD_Write_LBA_Dos(U32 Write_LBA,U8 Write_Stage)
{
	data	bit	tb_No_SD_Stop_CMD=0;
	data	bit	tb_SD_Start_Stage=0;
	data	bit	tb_SD_Data_Stage=0;
	data	bit	tb_SD_Stop_Stage=0;
	data	U16	tw_SD_CountDown;

	LBA.LW=Write_LBA;
	switch(Write_Stage)
	{
		case 0: // CMD+Data
			tb_SD_Start_Stage=1;
			tb_SD_Data_Stage=1;
			break;
		case 1: // only data
			tb_SD_Data_Stage=1;
			break;
		case 2: //data + Stop CMD
			tb_SD_Data_Stage=1;
			tb_SD_Stop_Stage=1;	
			break;
		case 3:// only 1 sector
			tb_SD_Start_Stage=1;
			tb_SD_Data_Stage=1;
			tb_No_SD_Stop_CMD=1;
			break;
		default:  //only  Stop cmd
			tb_SD_Stop_Stage=1;
			break;
	}	
	/////////////////////////////////////////////////////////////////                          
	//SD/MMC 的準備階段開始                                                                             
    XBYTE[0xB400]=0x06;
    if(tb_SD_Start_Stage)
	{
    	if(tb_No_SD_Stop_CMD)
		{                            		// single sector only                     
    		XBYTE[0xB45B]=0x58;                             	// 0x04| 0x11(Cmd_17) read single block
    	}
    	else
		{
    		XBYTE[0xB45B]=0x59;                             	// 0x04| 0x12(Cmd_18) read multi block 
    	}
    	if(gb_SDHC_Type)
		{
      		XBYTE[0xB45C]=LBA.BY[0];
        	XBYTE[0xB45D]=LBA.BY[1];                 			// relative address 1                             
        	XBYTE[0xB45E]=LBA.BY[2];                        	// 512Bytes/Block                          
        	XBYTE[0xB45F]=LBA.BY[3];                           // Sectors Address                
    	}                                                                                                   
    	else
		{                                                                                               
      		XBYTE[0xB45C]=(LBA.BY[1]<<1)+(LBA.BY[2]>>7);
       		XBYTE[0xB45D]=(U8)(LBA.WD[1]>> 7);              // relative address 1                
       		XBYTE[0xB45E]=(LBA.BY[3]<<1);                      // 512Bytes/Block                     
       		XBYTE[0xB45F]=0x00;                               // Sectors Address                    
    	}                                                                                                   
    	tw_SD_CountDown=SDMMCTimeOutLoop_10000;
    	SD_Send_DummyCLK(1);                                                                                
    	while(!(XBYTE[0xB453]&0x20))
		{
			// Wait Ready                             
    		SD_Send_DummyCLK(2);                                                                            
        	if(--tw_SD_CountDown==0)
			{
				goto RET;
			}
			if(!SD_Card_Detect())
			{
				goto RET;
			}
    	}
    	SD_TrigCMD_Receive_Respond();        
	}

	if(tb_SD_Data_Stage)
	{  // SD/MMC  write data
		XBYTE[0xB455]=0x00;
		XBYTE[0xB456]|=0x02;
		Device_Xfer_DataLength.WD=0x01FF;
		if(gb_ReadWriteDataArea==0)
		{
			Device_Write_SRAM_Index.BY[0]=gc_PlayRecordDataBufHB;
		}
		else
		{
			Device_Write_SRAM_Index.BY[0]=gc_UserDataBufHB;
		}
		Device_Write_SRAM_Index.BY[1]=0x00;                                                               
		SET_DEVICE_WRITE();                                                                                 
		Trigger_Data_Transfer();
		tw_SD_CountDown=SDMMCTimeOutLoop_10000;                                                              
		while((!(DMA_REG[0xC0]&0x01))||(XBYTE[0xB454]&0x0F))
		{
			if(--tw_SD_CountDown==0)
			{
				goto RET;
			}
		 	if(!SD_Card_Detect())
			{
				goto RET;
			}
		}                                                                                               
		DMA_REG[0xC0]=0x00;			                                                                    
		XBYTE[0xB452]=0x40;
		tw_SD_CountDown=SDMMCTimeOutLoop_10000;
		while(XBYTE[0xB454]&0x0F)
		{
			if(--tw_SD_CountDown==0)
			{
				goto RET;
			}
			if(!SD_Card_Detect())
			{
				goto RET;
			}
		}
	}
	if(tb_SD_Stop_Stage)
	{ // SD/MMC Stop CMD
    	SD_Cmd12_Stop();                                                                            
 		XBYTE[0xB452]=0x40;	
 		tw_SD_CountDown=SDMMCTimeOutLoop_10000;
		while(XBYTE[0xB454]&0x0F)
		{
			if(--tw_SD_CountDown==0)
			{
				goto RET;
			}
			if(!SD_Card_Detect()) 
			{
				goto RET;
			}
		}                            
 		SD_Send_DummyCLK(1);
 	} 
	return;
RET:
	USER_Delay(200);                                                                                              
}

