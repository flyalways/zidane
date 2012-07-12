#include "SPDA2K.h"
xdata U8  gc_SD_CSD[16];
xdata U8  gc_RespBuff[17];


void  SD_Interface_Initial(void)
{
	DEVICE_REG[0x00] = 0x06;                            // switch to sd interface
	DEVICE_REG[0x50] = 0x03;                            // reset sd & crc interface;
	DEVICE_REG[0x51] = 0x03;                            // set SD CLK  1/128 per clk,1 bit mode  
	DEVICE_REG[0x57] = 0xFF;                            // set rsp time out
	DEVICE_REG[0x58] = 0xFF;                            // crc time out 
	DEVICE_REG[0x56] = 0x02;
	SDMMC_SDRCA.WD   = 0x0000;                          // default RCA is 0 
	SD_Capacity.LW   = 0x0001; 
	gbt_SDHC_Type =0; 									// 0:Stand Capacity , 1: High Capicity
	gbt_MMC_Type =0;                                  // 0:SD , 1: MMC
}


void  SD_CMD0_Reset(void)
{
	DEVICE_REG[0x5B] = 0x40;                            // 0x40 | 0x00(Cmd_0)
	DEVICE_REG[0x5C] = 0x00;                            //
	DEVICE_REG[0x5D] = 0x00;                            // 
	DEVICE_REG[0x5E] = 0x00;                            // 
	DEVICE_REG[0x5F] = 0x00;                            // 
	SD_Trigger_CMD();                                   //
	SD_Check_Idle();                                    // check done
	SD_Send_DummyCLK(8);                                // 8 clks before send cmd
}


void SD_CMD55_PreACmd(void)
{							 				// Command 55                              
	DEVICE_REG[0x5B] = 0x77;				// 0x40 | 0x37(Cmd_55)
	DEVICE_REG[0x5C] = SDMMC_SDRCA.BY[0];
	DEVICE_REG[0x5D] = SDMMC_SDRCA.BY[1];
	DEVICE_REG[0x5E] = 0x00;
	DEVICE_REG[0x5F] = 0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_RespBuff,0,0);
}    


bit SD_Check_Idle(void)
{
	xdata	U16	tw_SD_CountDown; 
	
	tw_SD_CountDown=2000;
	while(DEVICE_REG[0x54]&0x0F)
	{
		if(--tw_SD_CountDown==0)
		{
			DEVICE_REG[0x52] = 0x00;
			DEVICE_REG[0x50] = 0x03;
			return 1;
		}

		if(SD_Detect)
		{
			return 0;
		}
	}
	return 0;  
}
       

bit SD_ACmd41_OCR(void)
{
    U16 tw_SD_CountDown;

	if(gbt_USB_Detected)
	{ 
		tw_SD_CountDown=PseudoCard_TimeOut;
	}
	else
	{
		tw_SD_CountDown=(PseudoCard_TimeOut/2);	
	}

	do{    	
		SD_CMD55_PreACmd();
		DEVICE_REG[0x5B]=0x69;	// ACMD41
		if(gbt_SDHC_Type)
		{   	
			DEVICE_REG[0x5C] = 0x40;
		}
		else
		{
			DEVICE_REG[0x5C] = 0x00;     
		}    	
		DEVICE_REG[0x5D] = 0xFC;
		DEVICE_REG[0x5E] = 0x00;
		DEVICE_REG[0x5F] = 0x00;
		SD_Trigger_CMD();
		SD_Check_Idle();
		SD_Receive_Respond(gc_RespBuff,0,0);
		if(SD_Detect)
		{
			return 0;
		}    
    	
		if((--tw_SD_CountDown==0))
		{	 	
			return 0;
		}
	}while(!(gc_RespBuff[1]&0x80));

	if(gbt_SDHC_Type)
	{
		if(gc_RespBuff[1]&0x40)
		{
			gbt_SDHC_Type =1;	// High Capacity
		}
		else
		{
			gbt_SDHC_Type =0;	// Standard Capicity
		}
	}
	return 1;   
   
}    


void SD_Cmd2_CID(void)
{
	DEVICE_REG[0x5B] = 0x42;
	DEVICE_REG[0x5C] = 0x00;
	DEVICE_REG[0x5D] = 0x00;
	DEVICE_REG[0x5E] = 0x00;
	DEVICE_REG[0x5F] = 0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_RespBuff,1,0);                // 136 bit    
} 


void SD_Cmd3_RCA(void)
{
	if(gbt_MMC_Type)
	{
		SDMMC_SDRCA.BY[0]=0x1F;
		SDMMC_SDRCA.BY[1]=0x3C;
	}
	else
	{
		SDMMC_SDRCA.BY[0] = 0x00;
		SDMMC_SDRCA.BY[1] = 0x00;
	}
	XBYTE[0xB45B] = 0x43;
	XBYTE[0xB45C] = SDMMC_SDRCA.BY[0];
	XBYTE[0xB45D] = SDMMC_SDRCA.BY[1];
	XBYTE[0xB45E] = 0x00;
	XBYTE[0xB45F] = 0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_RespBuff,0,0);	// 48 bit 
	
	if(!gbt_MMC_Type)
	{
		SDMMC_SDRCA.BY[0] = gc_RespBuff[1];
		SDMMC_SDRCA.BY[1] = gc_RespBuff[2];
	}  
}


void SD_Cmd8_Send_If_Cond(void)
{
	DEVICE_REG[0x5B] = 0x48;
	DEVICE_REG[0x5C] = 0x00;
	DEVICE_REG[0x5D] = 0x00;
	DEVICE_REG[0x5E] = 0x01;
	DEVICE_REG[0x5F] = 0xAA;
	SD_Trigger_CMD();
	SD_Check_Idle();
	if(SD_Receive_Respond(gc_RespBuff,0,0))
	{
		gbt_SDHC_Type = 0; // 0:Stand Capacity , 1: High Capicity
	}
	else
	{
		gbt_SDHC_Type = 1; // 0:Stand Capacity , 1: High Capicity
	}
}


void SD_Cmd9_CSD(void)
{
	data	U8	tc_ReadBl_Len;
	data	U8	tc_CSizeMult;	
	xdata	U8	tc_Counter;
	data	U16 tw_BlockLen;
	data	U16	tw_Mult;
	data	U16	tw_CSize;
	xdata	U32	tdw_CardTotalSize;
	xdata	U32	tdw_SD_BlockNum;    
    
	DEVICE_REG[0x5B] = 0x49;
	DEVICE_REG[0x5C] = SDMMC_SDRCA.BY[0];
	DEVICE_REG[0x5D] = SDMMC_SDRCA.BY[1];
	DEVICE_REG[0x5E] = 0x00;
	DEVICE_REG[0x5F] = 0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_RespBuff,1,0);	// 136 bit  
   
    for(tc_Counter=0; tc_Counter<16; tc_Counter++)
	{
    	gc_SD_CSD[tc_Counter]=gc_RespBuff[tc_Counter+1];
    }

    if(gbt_MMC_Type)
	{
    	tc_ReadBl_Len=gc_SD_CSD[0x05]&0x0f;	
    	tw_BlockLen=0x01<< (tc_ReadBl_Len);// 直接轉成sector
		tw_CSize  	=  (gc_SD_CSD[6]&0X03)<<10;
		tw_CSize    =  tw_CSize|(gc_SD_CSD[7]<<2);
		tw_CSize    =  tw_CSize|(gc_SD_CSD[8]>>6);
		tc_CSizeMult=((gc_SD_CSD[9] & 0x03) << 1) | (gc_SD_CSD[10] >> 7); 
		tw_Mult =	0x0001 << (tc_CSizeMult + 2);
		tdw_SD_BlockNum = ((U32)(tw_CSize +	1) *tw_Mult);              
		tdw_CardTotalSize = (U32)(tdw_SD_BlockNum * tw_BlockLen);
		
		if(gbt_SDHC_Type)
		{
			SD_Capacity.LW    =  (tdw_CardTotalSize);  // 直接就是Sector 
			gdw_CARD_TotalSizeMB = (SD_Capacity.LW);
		}
		else
		{
			SD_Capacity.LW    =  (tdw_CardTotalSize>>9); 
			gdw_CARD_TotalSizeMB = (SD_Capacity.LW); 
    	}    	
    }
    else
	{    	
    	if((gc_SD_CSD[0x00]&0xC0)==0x40)
		{//SD2.0 or High Capicity    	 
			tdw_CardTotalSize =  ((gc_SD_CSD[7]&0x3F)<<16) |(gc_SD_CSD[8]<<8)|gc_SD_CSD[9];
			SD_Capacity.LW    =  (tdw_CardTotalSize+1)<<10 ;    
			gdw_CARD_TotalSizeMB = (SD_Capacity.LW);
    	}
	    else
		{   //SD1.1 or Standard Capicity    
	    	tc_ReadBl_Len=gc_SD_CSD[0x05]&0x0f;	
    		tw_BlockLen=0x01<< (tc_ReadBl_Len);// 直接轉成sector
			tw_CSize  	=  (gc_SD_CSD[6]&0X03)<<10;
			tw_CSize    =  tw_CSize|(gc_SD_CSD[7]<<2);
			tw_CSize    =  tw_CSize|(gc_SD_CSD[8]>>6);
			tc_CSizeMult=((gc_SD_CSD[9] & 0x03) << 1) | (gc_SD_CSD[10] >> 7); 
			tw_Mult =	0x0001 << (tc_CSizeMult + 2);
			tdw_SD_BlockNum = ((U32)(tw_CSize +	1) *tw_Mult);              
			tdw_CardTotalSize = (U32)(tdw_SD_BlockNum * tw_BlockLen);
			SD_Capacity.LW    =  (tdw_CardTotalSize>>9);    
			gdw_CARD_TotalSizeMB = (SD_Capacity.LW); 
		}
	}
} 


void SD_Trigger_CMD(void)
{
	DEVICE_REG[0x52] = 0x20;	// 8 clks before send cmd 
    SD_Check_Idle();
    DEVICE_REG[0x50] = 0x03;	// reset crc                       
    DEVICE_REG[0x52] = 0x01;    // send command  
}


void SD_TrigCMD_Receive_Respond(void)
{
	DEVICE_REG[0x52] = 0x20;	// 8 clks before send cmd 
    SD_Check_Idle();
    DEVICE_REG[0x50] = 0x03;	// reset crc 
    DEVICE_REG[0x56] = 0xE0;
    DEVICE_REG[0x51]&= 0xF7;  
	DEVICE_REG[0x52] = 0x01;	// send command
	SD_Check_Idle(); 
}


bit  SD_Identification_Flow(void)
{    
    SD_Interface_Initial();
	if(SD_Detect)
	{
   		goto RET;
    }
    SD_Send_DummyCLK(80);  								// send 64 clk 
    SD_CMD0_Reset();
	if(SD_Detect)
	{
		goto RET;
    } 

    SD_MMC_Check_Mode();
	if(SD_Detect)
	{
		goto RET;
    }
    
	if(gbt_MMC_Type)
	{//MMC card
    	SD_CMD0_Reset();
    	if(!MMC_Cmd01_OCR_Wait_Ready())
		{
			goto RET;
    	}
    	DEVICE_REG[0x51] |=0x40;  // MMC mode   	
    }
    else
	{			// SD card 
   		SD_CMD0_Reset();
    	SD_Cmd8_Send_If_Cond();   
    	if(!SD_ACmd41_OCR())
		{
			goto RET;
    	}
    }
    SD_Cmd2_CID();
	if(SD_Detect)
	{
		goto RET;
    }
    SD_Cmd3_RCA();		// get RCA
	if(SD_Detect)
	{
		goto RET;
    }   
    SD_Cmd9_CSD();     // det CSD
	if(SD_Detect)
	{
		goto RET;
    }
	SD_Cmd13_Status();
	if(SD_Detect)
	{
		goto RET;
    }
	SD_Cmd7_Select();// pust to transfer stat
	if(SD_Detect)
	{
		goto RET;
    }	
	if(!gbt_MMC_Type)
	{
		DEVICE_REG[0x51]|= 0x04;  // set 4bit mode
    	SD_ACmd6_BusWidth(1);   // form 1 bit to 4 bit
    }   
	if(SD_Detect)
	{
		goto RET;
    }
    DEVICE_REG[0x51]&= 0xFD;  
    DEVICE_REG[0x56]|=0xA0;    
    SD_Cmd16_Blocklen(); 	// set 512 block length
	if(SD_Detect)
	{
		goto RET;
    }
	return 1;
RET:
	USER_DelayDTms(150);
	return 0;
}


void SD_Cmd7_Select(void)
{
	DEVICE_REG[0x5B] = 0x47;
	DEVICE_REG[0x5C] = SDMMC_SDRCA.BY[0];
	DEVICE_REG[0x5D] = SDMMC_SDRCA.BY[1];
	DEVICE_REG[0x5E] = 0x00;
	DEVICE_REG[0x5F] = 0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_RespBuff,0,0);                // 48 bit  
}                              


void SD_Cmd13_Status(void)
{
	DEVICE_REG[0x5B] = 0x4D;
	DEVICE_REG[0x5C] = SDMMC_SDRCA.BY[0];
	DEVICE_REG[0x5D] = SDMMC_SDRCA.BY[1];
	DEVICE_REG[0x5E] = 0x00;
	DEVICE_REG[0x5F] = 0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_RespBuff,0,0);                // 48 bit        
}


void SD_ACmd6_BusWidth(bit _4bit)
{
	SD_CMD55_PreACmd();
	DEVICE_REG[0x5B] = 0x46;
	DEVICE_REG[0x5C] = 0x00;
	DEVICE_REG[0x5D] = 0x00;
	DEVICE_REG[0x5E] = 0x00;
	if(_4bit)
	{
		DEVICE_REG[0x5F] = 0x02;
	}
	else
	{
		DEVICE_REG[0x5F] = 0x00;
	}
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_RespBuff,0,0);                // 48 bit    
}


void SD_Cmd12_Stop(void)
{
	DEVICE_REG[0x5B] = 0x4C;
	DEVICE_REG[0x5C] = 0x00;
	DEVICE_REG[0x5D] = 0x00;
	DEVICE_REG[0x5E] = 0x00;
	DEVICE_REG[0x5F] = 0x00;
	SD_TrigCMD_Receive_Respond(); 
}            


void SD_Cmd16_Blocklen(void)
{
	DEVICE_REG[0x5B] = 0x50;
	DEVICE_REG[0x5C] = 0x00;
	DEVICE_REG[0x5D] = 0x00;
	DEVICE_REG[0x5E] = 0x02;
	DEVICE_REG[0x5F] = 0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_RespBuff,0,0);                // 48 bit    
	DEVICE_REG[0x55] =0x00;
	DEVICE_REG[0x56]|=0x02;
}


void SD_Send_DummyCLK(U8 CLK_Num)
{
	U8 k;

	CLK_Num=CLK_Num>>3;
	for(k=0;k<CLK_Num;k++)
	{
		DEVICE_REG[0x52] = 0x20;// 8 clks before send cmd 
    	SD_Check_Idle();
    }         
}


U8 SD_Receive_Respond(U8 *tc_RespBuff,U8 tc_Resptype136bit, U8 tc_CRC)
{
	data	U8	tc_FullFlag;
	data	U8	tc_CRC7;
	data	U8	tc_i;
	xdata	U16	tw_SD_CountDown;     

	tw_SD_CountDown = 80;

	if(tc_Resptype136bit)
	{
		DEVICE_REG[0x51] |=0x08;			
	}
	else
	{	
		DEVICE_REG[0x51] &=~0x08;
	}	
	DEVICE_REG[0x57]  = 0xFF; 
	DEVICE_REG[0x56] |= 0x20; // auto send dummy clk
	DEVICE_REG[0x52]  = 0x02;
	do
	{
		tc_FullFlag=(DEVICE_REG[0x53]&0x02);
		if((DEVICE_REG[0x53]&0x40)||(--tw_SD_CountDown==0))
		{
			DEVICE_REG[0x50] =0x03;	
			return 1;     // Time out
		}		
	}while((tc_FullFlag==0));
	//48bit
	tc_RespBuff[0] = DEVICE_REG[0x60];
	tc_RespBuff[1] = DEVICE_REG[0x61];
	tc_RespBuff[2] = DEVICE_REG[0x62];
	tc_RespBuff[3] = DEVICE_REG[0x63];
	tc_RespBuff[4] = DEVICE_REG[0x64];
	tc_RespBuff[5] = DEVICE_REG[0x65];	

	tc_CRC7=tc_RespBuff[5];
	if(tc_Resptype136bit)	//136 bit
	{
		for(tc_i=0;tc_i<11;tc_i++) 
		{
			do
			{
				tc_FullFlag=(DEVICE_REG[0x53]&0x02);
				if((DEVICE_REG[0x53]&0x40)||(--tw_SD_CountDown==0))
				{
					DEVICE_REG[0x50] =0x03;	
					return 1;     // Time out
				}		
			}while((tc_FullFlag==0));		
			tc_RespBuff[6+tc_i] = DEVICE_REG[0x65];		
		}
		tc_CRC7=tc_RespBuff[16];
	}
	if((tc_CRC7!=DEVICE_REG[0x66])&&tc_CRC)
	{//CRC error
		DEVICE_REG[0x50] =0x03;
		return 0x01;		
	}	
	return 0;	
} 


void  SD_MMC_Check_Mode(void)
{
	SD_CMD0_Reset();
	if(MMC_Cmd01_OCR())
	{   // By CMD1     
		gbt_MMC_Type =1;  // MMC Mode
	}
	else
	{
		gbt_MMC_Type =0;  //SD Mode
	}
	SD_Send_DummyCLK(64);         
}
/***************************************************************************
* NAME:         bit MMC_Cmd01_OCR(void)
* DESCRIPTION:
***************************************************************************/
bit MMC_Cmd01_OCR(void)
{
	DEVICE_REG[0x5B] = 0x41;
	DEVICE_REG[0x5C] = 0x80;
	DEVICE_REG[0x5D] = 0xFF;                            	//2.7~3.6 Volt
	DEVICE_REG[0x5E] = 0x00;
	DEVICE_REG[0x5F] = 0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	if(SD_Receive_Respond(gc_RespBuff,0,0))
	{    			// Time out -->No Responce 
		return 0;
	}
	else
	{ 		
		return 1; 		
	} 
}


bit MMC_Cmd01_OCR_Wait_Ready(void)
{    
	xdata	U16	tw_SD_CountDown;
	
	if(gbt_USB_Detected)
	{ 
		tw_SD_CountDown = PseudoCard_TimeOut;
	}
	else
	{
		tw_SD_CountDown = (PseudoCard_TimeOut/2);	
	}
	
	do{
		DEVICE_REG[0x5B] = 0x41;
		DEVICE_REG[0x5C] = 0x80;
		DEVICE_REG[0x5D] = 0xFF;
		DEVICE_REG[0x5E] = 0x00;
		DEVICE_REG[0x5F] = 0x00;
		SD_Trigger_CMD();
		SD_Check_Idle();
		SD_Receive_Respond(gc_RespBuff,0,0);
		if(SD_Detect)
		{
			return 0;
		}    
		if((--tw_SD_CountDown==0))
		{
			return 0;
		}
	}while(!(gc_RespBuff[1]&0x80));    

	if(gc_RespBuff[1]&0x40)
	{
		gbt_SDHC_Type =1;	// High Capacity MMC Sector mode    	
	}
	else
	{
		gbt_SDHC_Type =0;	// Standard Capicity   MMC Byte mode
	}	
	return 1;       
}  
