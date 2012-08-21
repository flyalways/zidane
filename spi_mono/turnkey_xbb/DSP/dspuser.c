#include "..\header\SPDA2K.h"
#include "..\header\DSP.h"
#include "dspphysic.h"
#include "dspuser.h"
#include "..\header\variables.h"


void DSP_WakeUp(void)
{
	XBYTE[0xB010]|=0x04;	// wake up DSP
	// 2T-SRAM Enable
	XBYTE[0xB0F6] &= 0xF8;
	XBYTE[0xB0F7] |= 0x07;
	XBYTE[0xB0F8] |= 0x07;
	XBYTE[0xB0F9] |= 0x07;
}

void DSP_ClearReady(void)
{
	xdata	U16 tw_Data;

	tw_Data = L2_DSP_Read_DMem16(DSP_CMDExeStatus);
	L2_DSP_Write_DMem16(DSP_CMDExeStatus, tw_Data&0xFFFE); //clear DSP 0x3F0B.0 ready status
}

U8 DSP_SearchSunplusKey(void)
{
	code U8 SunplusKey[8] = {'S','u','n','p','l','u','s',0x00} ;
	data	U8	ta_GetString[8];
	data	U8	tc_Cnt;
	data	U16	tw_AddressIndex = DSP_SunplusKeyAddr;	// 0x3F11
	data	U8	*tpc_String = ta_GetString;


	// Read "SunplusKey" from DSP DM and put them into GetString[8].
	for(tc_Cnt=0; tc_Cnt<4; ++tc_Cnt)
	{
		U16	tc_Data = L2_DSP_Read_DMem16(tw_AddressIndex++);

		*(tpc_String+2*tc_Cnt)	= HIBYTE(tc_Data);
		*(tpc_String+2*tc_Cnt+1)= LOBYTE(tc_Data);
	}
	for(tc_Cnt=0; tc_Cnt<8; tc_Cnt++)
	{
		if(*(tpc_String+tc_Cnt)==SunplusKey[tc_Cnt])
		{
			return 0;
		}
	}
	return 1;
}


U8 DSP_Download(U8 tc_CurrentIMID,U8 tc_CurrentPMID)
{
	data	U8	tc_TotalIDNum;
	data	U8	tc_TempLoop;
	data	U8	tc_Loop;
	data	U16	tw_DSPCodeIMphyadr;
	data	U16	tw_DSPCodePMphyadr;
	data	U16	tw_IDIndexOffset;
	xdata	U8	tc_buf_idx_start;

	//1. Turn off DSP CLK
	XBYTE[0xB010]&=~0x01;
	//2. Download DSP IMPM
	{
 		//initial IM and PM phyadr value is 0xffff
		tw_DSPCodeIMphyadr=0xFFFF;
 		tw_DSPCodePMphyadr=0xFFFF;
		SPI_DSPHZK_Block_Read(0,1,gc_PlayRecordDataBufHB);
		tc_TotalIDNum=gc_PlayRecordDataBuf[0];

		for(tc_TempLoop=0;tc_TempLoop<tc_TotalIDNum;tc_TempLoop++)
		{//Scan the IMIP ID in the all ID index
			tw_IDIndexOffset=((U16)tc_TempLoop*9);
			if ((gc_PlayRecordDataBuf[tw_IDIndexOffset+1])==tc_CurrentIMID)
			{//if it is IM,move IM to DSP
				((U8 *)(&tw_DSPCodeIMphyadr))[0]=gc_PlayRecordDataBuf[tw_IDIndexOffset+2];
				((U8 *)(&tw_DSPCodeIMphyadr))[1]=gc_PlayRecordDataBuf[tw_IDIndexOffset+3]; 
				for(tc_Loop=0;tc_Loop<96;tc_Loop++)
				{
					SPI_DSPHZK_Block_Read(0,tw_DSPCodeIMphyadr+1,gc_PlayRecordDataBufHB);
					L2_DSP_MCU_IM_DMA(tc_Loop,(U16)gc_PlayRecordDataBuf);
					tw_DSPCodeIMphyadr++;
				}
				SPI_DSPHZK_Block_Read(0,1,gc_PlayRecordDataBufHB);
			}
			else if ((gc_PlayRecordDataBuf[tw_IDIndexOffset+1])==tc_CurrentPMID)
			{//if it is PM,move PM to DSP
				tc_buf_idx_start=0;

				((U8 *)(&tw_DSPCodePMphyadr))[0] = gc_PlayRecordDataBuf[tw_IDIndexOffset + 2];
				((U8 *)(&tw_DSPCodePMphyadr))[1] = gc_PlayRecordDataBuf[tw_IDIndexOffset + 3]; 

				if((tc_CurrentPMID==0x23)||(tc_CurrentPMID==0x24)||(tc_CurrentPMID==0x25))//(Jimi 091028)to save spi
				{
					tc_buf_idx_start=24;
				}

				for(tc_Loop=tc_buf_idx_start;tc_Loop<48;tc_Loop++)
				{
					SPI_DSPHZK_Block_Read(0,tw_DSPCodePMphyadr+1,gc_PlayRecordDataBufHB);
					L2_DSP_MCU_PM_DMA(tc_Loop,(U16)gc_PlayRecordDataBuf);

					tw_DSPCodePMphyadr++;
				}
				SPI_DSPHZK_Block_Read(0,0+1,gc_PlayRecordDataBufHB);
			}
		}
		if((tw_DSPCodePMphyadr==0xffff)&&(tw_DSPCodeIMphyadr==0xffff))
		{//the IM and PM is no exist
			return DSP_IMPM_INEXISTENCE_ERROR;
		}
	}

	//5. Enable DSP CLK
	XBYTE[0xB010]|=0x01;

 	//6. Release DSP reset
	XBYTE[0xB002]&=0xFE;                             // Enable DSP Run Normal Mode			
	XBYTE[0xB216]=0x00;  //Enable DM,IM,PM

	return 0;
}


U8 DSP_PlayInit(void)
{
	xdata	U16 dbgTmp;
	data	U16 TimeOUT;

	// Jimi 091104 for EQ mode pop noise as next/prev
	if(gb_Play_firstPlay==1)
	{
		L2_DSP_Write_DMem16(DSP_PostProcessSelect,0x8003);
		gb_Play_firstPlay=0;
	}
	else
	{
		L2_DSP_Write_DMem16(DSP_PostProcessSelect,0x0003);
	}

	L2_DSP_SendCommandSet(DCMD_Restart); // ycc081031,force DSP jump 0 without hardware reset
	L2_DSP_Write_DMem16(DSP_RestartFlag,0x1235); // ycc081031

	if(DSP_SearchSunplusKey()!=DSP_SUCCESS)
	{
		return DSP_CHECK_SUNPLUSKEY_ERROR;
	}       

	// Clear all buffers in DSP
	DSP_ClearReady();
	L2_DSP_Write_DMem16(DSP_GoControl,0x1235);            
	{
	    TimeOUT = 0xFFFF;
		do{
	    	dbgTmp = L2_DSP_Read_DMem16(DSP_CMDExeStatus)&0x0001;  //read DSP 0x3F0B.0 ready status
	       	TimeOUT --;
	      	if(TimeOUT==0)
	        {
	          	return DSP_CMD_EXE_STATUS0_ERROR;
	      	} 
	  	}while(dbgTmp!=1);
	}

	return DSP_SUCCESS;
}


void DSP_SurroundDisable(void)
{
    data	U16 tc_Data;

	tc_Data=L2_DSP_Read_DMem16(DSP_EffectControl);
    L2_DSP_Write_DMem16(DSP_EffectControl, tc_Data&0xF7FF);
	tc_Data=L2_DSP_Read_DMem16(DSP_SurroundDPCFLAG);
	L2_DSP_Write_DMem16(DSP_SurroundDPCFLAG,tc_Data&0xFFFD);
}


U8 DSP_PauseCmd(void)
{
	if(L2_DSP_SendCommandSet(DCMD_Pause)!=DCMD_Pause)
	{
		return DSP_PAUSE_COMMAND_ERROR;
	}

	return 0;
}


U8  DSP_StopCmd(void)
{
    if(L2_DSP_SendCommandSet(DCMD_Stop)!=DCMD_Stop)
	{
		return DSP_STOP_COMMAND_ERROR;
	}

  	return 0;
}

void DSP_EQ_Cmd(U8 tc_EQType)
{
	data	U16	tw_TmpData;
	xdata	U8	*tpc_EQPoint;
	code U8  EQ_TABLE[][7]=
	{
		24,24,20,20,14,9,5,	 //DBB
		18,16,14,1,16,18,17, //JAZZ
		20,20,15,10,2,8,22,	 //ROCK
		12,18,4,12,18,12,8,	 //POP
		7,10,14,17,18,10,7,	 //LIVE(CLASSIC)
	};

	tw_TmpData=L2_DSP_Read_DMem16(DSP_EffectControl); 

	DSP_SurroundDisable();
	if(!tc_EQType) 
	{	//EQ disable == NORMAL MODE
		L2_DSP_Write_DMem16(DSP_EffectControl,tw_TmpData&0xFDFF);////EQ disable trigger
	}
	else 
	{				// EQ enable
		tc_EQType--;
		tpc_EQPoint = &EQ_TABLE[tc_EQType][0];
		L2_DSP_Write_DMem16(DSP_EQBand0,(U16)(tpc_EQPoint[0]));
		L2_DSP_Write_DMem16(DSP_EQBand1,(U16)(tpc_EQPoint[1]));
		L2_DSP_Write_DMem16(DSP_EQBand2,(U16)(tpc_EQPoint[2]));
		L2_DSP_Write_DMem16(DSP_EQBand3,(U16)(tpc_EQPoint[3]));
		L2_DSP_Write_DMem16(DSP_EQBand4,(U16)(tpc_EQPoint[4]));
		L2_DSP_Write_DMem16(DSP_EQBand5,(U16)(tpc_EQPoint[5]));
		L2_DSP_Write_DMem16(DSP_EQBand6,(U16)(tpc_EQPoint[6]));
		tw_TmpData &= 0xFF7F;////'Spectrum Only' disable trigger
		L2_DSP_Write_DMem16(DSP_EffectControl,tw_TmpData|0x0300);//EQ enable trigger
	}
}

void DSP_SpectrumOn(void)
{
	data	U16	tw_TmpData;

	tw_TmpData=L2_DSP_Read_DMem16(DSP_EffectControl); 
	L2_DSP_Write_DMem16(DSP_EffectControl,tw_TmpData|0x0400);	//Open Spectrum
}

U8  DSP_ResetCmd(void)	//Jimi 080522
{
    if(L2_DSP_SendCommandSet(DCMD_Reset)!=DCMD_Reset)
	{
		return DSP_STOP_COMMAND_ERROR;
	}

  	return 0;
}
