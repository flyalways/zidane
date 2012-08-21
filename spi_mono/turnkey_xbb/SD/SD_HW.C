#include "..\header\SPDA2K.h"
#include "..\header\variables.h"


void  SD_CMD0_Reset(void)
{
    XBYTE[0xB45B]=0x40;
    XBYTE[0xB45C]=0x00;
    XBYTE[0xB45D]=0x00;
    XBYTE[0xB45E]=0x00;
    XBYTE[0xB45F]=0x00;
    SD_Trigger_CMD();
    SD_Check_Idle();
   	SD_Send_DummyCLK(1);	// 8 clks before send cmd
}


void SD_CMD55_PreACmd(void)
{
	XBYTE[0xB45B]=0x40+55;
	XBYTE[0xB45C]=SDMMC_SDRCA.BY[0];
	XBYTE[0xB45D]=SDMMC_SDRCA.BY[1];
	XBYTE[0xB45E]=0x00;
	XBYTE[0xB45F]=0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_SDRespBuff,0,0); 
}    


bit SD_ACmd41_OCR(void)
{
	data	U16	tw_SD_CountDown;
	
	tw_SD_CountDown=12000;	
	
	do{    	
		SD_CMD55_PreACmd();
		XBYTE[0xB45B]=0x40+41;
		if(gb_SDHC_Type)
		{   	
			XBYTE[0xB45C]=0x40;
		}
		else
		{
			XBYTE[0xB45C]=0x00;
		}    	
		XBYTE[0xB45D]=0xFC;
		XBYTE[0xB45E]=0x00;
		XBYTE[0xB45F]=0x00;
		SD_Trigger_CMD();
		SD_Check_Idle();
		SD_Receive_Respond(gc_SDRespBuff,0,0);
		if(gb_SD_pin)
		{                              // card out 
			return 0;
		}    
    	if((--tw_SD_CountDown==0))
		{	
		  return 0;
		}
	}while(!(gc_SDRespBuff[1]&0x80)); 
	
	if(gb_SDHC_Type)
	{
		if(gc_SDRespBuff[1]&0x40)
		{
			gb_SDHC_Type=1;      							// High Capacity
		}
		else
		{
			gb_SDHC_Type=0;								// Standard Capicity
		}
	}
	return 1;   
}    


void SD_Cmd2_CID(void)
{
	XBYTE[0xB45B]=0x40+2;
	XBYTE[0xB45C]=0x00;
	XBYTE[0xB45D]=0x00;
	XBYTE[0xB45E]=0x00;
	XBYTE[0xB45F]=0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_SDRespBuff,1,0);                // 136 bit    
} 


void SD_Cmd3_RCA(void)
{
    if(gb_MMC_Type)
	{
    	SDMMC_SDRCA.BY[0]=0x1F;
    	SDMMC_SDRCA.BY[1]=0x3C;
    }
    else
	{
    	SDMMC_SDRCA.BY[0]=0x00;
    	SDMMC_SDRCA.BY[1]=0x00;
    }
    XBYTE[0xB45B]=0x40+3;
    XBYTE[0xB45C]=SDMMC_SDRCA.BY[0];
    XBYTE[0xB45D]=SDMMC_SDRCA.BY[1];
    XBYTE[0xB45E]=0x00;
    XBYTE[0xB45F]=0x00;
    SD_Trigger_CMD();
    SD_Check_Idle();
    SD_Receive_Respond(gc_SDRespBuff,0,0);                // 48 bit 
    if(!gb_MMC_Type)
	{
        SDMMC_SDRCA.BY[0]=gc_SDRespBuff[1];
    	SDMMC_SDRCA.BY[1]=gc_SDRespBuff[2];
    }  
}


void SD_Cmd8_Send_If_Cond(void)
{ 
   	XBYTE[0xB45B]=0x40+8;
   	XBYTE[0xB45C]=0x00;
   	XBYTE[0xB45D]=0x00;
   	XBYTE[0xB45E]=0x01;
   	XBYTE[0xB45F]=0xAA;
	SD_Trigger_CMD();
   	SD_Check_Idle();
   	if(SD_Receive_Respond(gc_SDRespBuff,0,0))
	{	// Time out -->No Responce 
   		gb_SDHC_Type=0; // 0:Stand Capacity , 1: High Capicity
   	}
 	else
	{
 		gb_SDHC_Type=1; // 0:Stand Capacity , 1: High Capicity
 	}
}


void SD_Cmd9_CSD(void)
{
    data	U8	tc_ReadBl_Len;
	data	U8	tc_CSizeMult;	
	data	U16	tw_Mult;
	data	U16	tw_BlockLen;
	data	U16	tw_CSize;
    xdata	U8	tc_Counter;
	xdata	U32	tdw_CardTotalSize;
    xdata	U32	tdw_SD_BlockNum;    
    
    XBYTE[0xB45B]=0x40+9;
    XBYTE[0xB45C]=SDMMC_SDRCA.BY[0];
    XBYTE[0xB45D]=SDMMC_SDRCA.BY[1];
    XBYTE[0xB45E]=0x00;
    XBYTE[0xB45F]=0x00;
    SD_Trigger_CMD();
    SD_Check_Idle();
    SD_Receive_Respond(gc_SDRespBuff,1,0);                // 136 bit  
   
    for(tc_Counter=0;tc_Counter<16;tc_Counter++)
	{
    	gc_SD_CSD[tc_Counter]=gc_SDRespBuff[tc_Counter+1];
    }
    
    if(gb_MMC_Type)
	{
    	tc_ReadBl_Len=gc_SD_CSD[0x05]&0x0f;	
    	tw_BlockLen=0x01<<(tc_ReadBl_Len);// 直接轉成sector
		tw_CSize=(gc_SD_CSD[6]&0X03)<<10;
		tw_CSize=tw_CSize|(gc_SD_CSD[7]<<2);
		tw_CSize=tw_CSize|(gc_SD_CSD[8]>>6);
		tc_CSizeMult=((gc_SD_CSD[9]&0x03)<<1)|(gc_SD_CSD[10]>>7); 
		tw_Mult=0x0001<<(tc_CSizeMult+2);
		tdw_SD_BlockNum=((U32)(tw_CSize+1)*tw_Mult);              
		tdw_CardTotalSize=(U32)(tdw_SD_BlockNum*tw_BlockLen);
		if(gb_SDHC_Type)
		{
			SD_Capacity.LW=(tdw_CardTotalSize);  // 直接就是Sector 
		}
		else
		{
			SD_Capacity.LW=(tdw_CardTotalSize>>9); 
    	}    	
    }
    else
	{    	
    	if((gc_SD_CSD[0x00]&0xC0)==0x40)
		{//SD2.0 or High Capicity    	 
			tdw_CardTotalSize=((gc_SD_CSD[7]&0x3F)<<16)|(gc_SD_CSD[8]<<8)|gc_SD_CSD[9];
			SD_Capacity.LW=(tdw_CardTotalSize+1)<<10 ;    
    	}
    	else
		{   //SD1.1 or Standard Capicity    
	    	tc_ReadBl_Len=gc_SD_CSD[0x05]&0x0f;	
    		tw_BlockLen=0x01<<(tc_ReadBl_Len);// 直接轉成sector
			tw_CSize=(gc_SD_CSD[6]&0X03)<<10;
			tw_CSize=tw_CSize|(gc_SD_CSD[7]<<2);
			tw_CSize=tw_CSize|(gc_SD_CSD[8]>>6);
			tc_CSizeMult=((gc_SD_CSD[9]&0x03)<<1)|(gc_SD_CSD[10]>>7); 
			tw_Mult=0x0001<<(tc_CSizeMult+2);
			tdw_SD_BlockNum=((U32)(tw_CSize+1)*tw_Mult);              
			tdw_CardTotalSize=(U32)(tdw_SD_BlockNum*tw_BlockLen);
			SD_Capacity.LW=(tdw_CardTotalSize>>9);    
		}
	}
	gdw_CARD_TotalSizeMB=SD_Capacity.LW-1;
} 


void SD_Trigger_CMD(void)
{
	SD_Send_DummyCLK(1);
    SD_Check_Idle();
    XBYTE[0xB450]=0x03;	// reset crc                       
    XBYTE[0xB452]=0x01;	// send command  
}


bit  SD_Identification_Flow(void)
{   
	// SD Interface Initial
	XBYTE[0xB400]=0x06;		// switch to sd interface
	XBYTE[0xB450]=0x03;		// reset sd & crc interface;
	XBYTE[0xB451]=0x03;		// set SD CLK  1/128 per clk,1 bit mode  
    XBYTE[0xB456]=0x02;
	XBYTE[0xB457]=0xFF;		// set rsp time out
	XBYTE[0xB458]=0xFF;		// crc time out 
	SDMMC_SDRCA.WD=0x0000;	// default RCA is 0 
	SD_Capacity.LW=0x0001; 
	gb_SDHC_Type=0; 		// 0:Stand Capacity , 1: High Capicity
	gb_MMC_Type=0;			// 0:SD , 1: MMC
	gc_HostSectorUnit=1;

	if(gb_SD_pin)
	{
		goto RET;
	}
    SD_Send_DummyCLK(8);  // send 64 clk 
	SD_CMD0_Reset();
	if(gb_SD_pin)
	{
		goto RET;
	} 
	
	SD_MMC_Check_Mode();
	if(gb_SD_pin)
	{
		goto RET;
	}
	
	if(gb_MMC_Type)
	{//MMC card
		SD_CMD0_Reset();
    	if(MMC_Cmd01_OCR_Wait_Ready()==0)
		{
			goto RET;
		}
		XBYTE[0xB451] |=0x40;  // MMC mode   	
	}
	else
	{	// SD card 
		SD_CMD0_Reset();
		SD_Cmd8_Send_If_Cond();   
    	if(SD_ACmd41_OCR()==0)
		{
			goto RET;
		}
	}
	
	SD_Cmd2_CID();
	if(gb_SD_pin)
	{
		goto RET;
	}
	
	SD_Cmd3_RCA();		// get RCA
	if(gb_SD_pin)
	{
		goto RET;
	}   
	
	SD_Cmd9_CSD();     // det CSD
	if(gb_SD_pin)
	{
		goto RET;
	}
	
	SD_Cmd13_Status();
	if(gb_SD_pin)
	{
		goto RET;
	}
	
	SD_Cmd7_Select();// pust to transfer stat
	if(gb_SD_pin)
	{
		goto RET;
	}	
	
	if(!gb_MMC_Type)
	{	
		XBYTE[0xB451]|= 0x04;  // set 4bit mode
		SD_ACmd6_BusWidth(1);   // form 1 bit to 4 bit
	}   
	XBYTE[0xB451]&=0xFD;
	XBYTE[0xB456]|=0xA0;    
	SD_Cmd16_Blocklen(); 	// set 512 block length
	if(gb_SD_pin)
	{
		goto RET;
	}	
	return 1; 
RET:
	USER_Delay(200);
	return 0;
}


void SD_Cmd7_Select(void)
{
	XBYTE[0xB45B]=0x40+7;
	XBYTE[0xB45C]=SDMMC_SDRCA.BY[0];
	XBYTE[0xB45D]=SDMMC_SDRCA.BY[1];
	XBYTE[0xB45E]=0x00;
	XBYTE[0xB45F]=0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_SDRespBuff,0,0);	// 48 bit    
}                              


void SD_Cmd13_Status(void)
{
	XBYTE[0xB45B]=0x40+13;
	XBYTE[0xB45C]=SDMMC_SDRCA.BY[0];
	XBYTE[0xB45D]=SDMMC_SDRCA.BY[1];
	XBYTE[0xB45E]=0x00;
	XBYTE[0xB45F]=0x00;
	SD_Trigger_CMD();
	SD_Check_Idle();
	SD_Receive_Respond(gc_SDRespBuff,0,0);	// 48 bit        
}


void SD_ACmd6_BusWidth(bit _4bit)
{
    SD_CMD55_PreACmd();
    XBYTE[0xB45B]=0x40+6;
    XBYTE[0xB45C]=0x00;
    XBYTE[0xB45D]=0x00;
    XBYTE[0xB45E]=0x00;
   	XBYTE[0xB45F]=((U8)_4bit)<<1;
    SD_Trigger_CMD();
    SD_Check_Idle();
    SD_Receive_Respond(gc_SDRespBuff,0,0);	// 48 bit    
}

void SD_Cmd16_Blocklen(void)
{
    XBYTE[0xB45B]=0x40+16;
    XBYTE[0xB45C]=0x00;
    XBYTE[0xB45D]=0x00;
    XBYTE[0xB45E]=0x02;
    XBYTE[0xB45F]=0x00;
    SD_Trigger_CMD();
    SD_Check_Idle();
    SD_Receive_Respond(gc_SDRespBuff,0,0);	// 48 bit    
	XBYTE[0xB455]=0x00;
	XBYTE[0xB456]|=0x02;
}

U8 SD_Receive_Respond(U8 *tc_RespBuff,U8 tc_Resptype136bit,U8 tc_CRC)
{ 
	data	U8	tc_FullFlag;
	data	U8	tc_CRC7;
	data	U8	tc_i;
	data	U8	tc_SD_CountDown;     

	tc_SD_CountDown=0;
	if(tc_Resptype136bit)
	{	
		XBYTE[0xB451]|=0x08;			
	}
	else
	{	
		XBYTE[0xB451]&=0xF7;
	}	
	XBYTE[0xB457]=0xFF; 
	XBYTE[0xB456]|=0x20; // auto send dummy clk
	XBYTE[0xB452]=0x02;
	do
	{
		tc_FullFlag=(XBYTE[0xB453]&0x02);
		if((XBYTE[0xB453]&0x40)||(--tc_SD_CountDown==0))
		{
			XBYTE[0xB450]=0x03;	
			return 1;     // Time out
		}		
	}while((tc_FullFlag==0));
	//48bit
	tc_RespBuff[0]=XBYTE[0xB460];
	tc_RespBuff[1]=XBYTE[0xB461];
	tc_RespBuff[2]=XBYTE[0xB462];
	tc_RespBuff[3]=XBYTE[0xB463];
	tc_RespBuff[4]=XBYTE[0xB464];
	tc_RespBuff[5]=XBYTE[0xB465];	
	tc_CRC7=tc_RespBuff[5];
	if(tc_Resptype136bit)	//136 bit
	{
		for(tc_i=0;tc_i<11;tc_i++) 
		{
			do
			{
				tc_FullFlag=(XBYTE[0xB453]&0x02);
				if((XBYTE[0xB453]&0x40)||(--tc_SD_CountDown==0))
				{
					XBYTE[0xB450] =0x03;	
					return 1;     // Time out
				}		
			}while((tc_FullFlag==0));		
			tc_RespBuff[6+tc_i]=XBYTE[0xB465];		
		}
		tc_CRC7=tc_RespBuff[16];
	}
	if((tc_CRC7!=XBYTE[0xB466])&&tc_CRC)
	{//CRC error
		XBYTE[0xB450]=0x03;
		return 1;		
	}	
	return 0;	
} 


void  SD_MMC_Check_Mode(void)
{
	SD_CMD0_Reset();
	if(MMC_Cmd01_OCR())
	{   // By CMD1     
		gb_MMC_Type =1;  // MMC Mode
	}
	else
	{
		gb_MMC_Type =0;  //SD Mode
	}
	SD_Send_DummyCLK(8);         
}


bit MMC_Cmd01_OCR(void)
{
   	XBYTE[0xB45B]=0x40+1;
   	XBYTE[0xB45C]=0x80;
   	XBYTE[0xB45D]=0xFF;	//2.7~3.6 Volt
   	XBYTE[0xB45E]=0x00;
   	XBYTE[0xB45F]=0x00;
	SD_Trigger_CMD();
   	SD_Check_Idle();
   	if(SD_Receive_Respond(gc_SDRespBuff,0,0))
	{
 		return 0;
 	}
 	else
	{ 		
		return 1; 		
 	} 
}


bit MMC_Cmd01_OCR_Wait_Ready(void)
{    
	U16 tw_SD_CountDown;

	tw_SD_CountDown=PseudoCard_TimeOut;	

	do{
 		XBYTE[0xB45B]=0x40+1;
    	XBYTE[0xB45C]=0x80;
    	XBYTE[0xB45D]=0xFF;
    	XBYTE[0xB45E]=0x00;
    	XBYTE[0xB45F]=0x00;
		SD_Trigger_CMD();
    	SD_Check_Idle();
    	SD_Receive_Respond(gc_SDRespBuff,0,0);
    	if(gb_SD_pin)
		{                              // card out 
    		return 0;
    	}    
    	if((--tw_SD_CountDown==0))
		{
    	  return 0;
    	}
    }while(!(gc_SDRespBuff[1]&0x80)); 
   
    if(gc_SDRespBuff[1]&0x40)
	{
    	gb_SDHC_Type =1;									// High Capacity MMC Sector mode    	
    }
    else
	{
    	gb_SDHC_Type =0;									// Standard Capicity   MMC Byte mode
    }	
    return 1;       
} 

 
