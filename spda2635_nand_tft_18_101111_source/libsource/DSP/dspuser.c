#include "SPDA2K.h"
#include "DSP.h"
#include "dspphysic.h"
#include "dspuser.h"


extern	data	bit  gb_Play_firstPlay;  //20090420 chiayen add


void DSP_WakeUp(U8 tc_Flag)
{
	if (tc_Flag & DSP_CLK_CTRL)
	{
		GLOBAL_REG[0x10] |= 0x04;	//(JC)H0520; wake up DSP  
	}

	// 2T-SRAM Enable
	if (tc_Flag & SRAM2T_CTRL)
	{
		GLOBAL_REG[0xF6] &= 0xF8;
		GLOBAL_REG[0xF7] |= 0x07;
		GLOBAL_REG[0xF8] |= 0x07;
		GLOBAL_REG[0xF9] |= 0x07;
	}
}

void DSP_ClearReady(void)
{
	U16 tw_Data;

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
		if(*(tpc_String+tc_Cnt) == SunplusKey[tc_Cnt])
		{
			return 0;
		}
	}
	return 1;
}


U8 DSP_Download(U8 tc_CurrentIMID, U8 tc_CurrentPMID)
{
	U8 tc_TotalIDNum;
	U8 tc_TempLoop;
	U16 tw_DSPCodeIMphyadr,tw_DSPCodePMphyadr;
	U16 tw_IDIndexOffset;
	U16 tw_Loop;
	U8 tc_value;


	//1. Turn off DSP CLK
	tc_value=GLOBAL_REG[0x10] & 0xFE;
	GLOBAL_REG[0x10]=tc_value;//GLOBAL_REG[0x10] &= 0xFE;							// Turn off DSP clk
	//2. Download DSP IMPM
	{
 		//initial IM and PM phyadr value is 0xffff
		tw_DSPCodeIMphyadr = 0xffff;
 		tw_DSPCodePMphyadr = 0xffff;

		DSPHZK_Block_Read(0,0+1,1,gc_PlayRecordDataBufHB);
		tc_TotalIDNum = gc_PlayRecordDataBuf[0];

		for (tc_TempLoop = 0; tc_TempLoop < tc_TotalIDNum; tc_TempLoop ++)
		{//Scan the IMIP ID in the all ID index
			tw_IDIndexOffset = ((U16)tc_TempLoop * 9);
			if ((gc_PlayRecordDataBuf[tw_IDIndexOffset + 1]) == tc_CurrentIMID)
			{//if it is IM,move IM to DSP
				((U8 *)(&tw_DSPCodeIMphyadr))[0] = gc_PlayRecordDataBuf[tw_IDIndexOffset + 2];
				((U8 *)(&tw_DSPCodeIMphyadr))[1] = gc_PlayRecordDataBuf[tw_IDIndexOffset + 3]; 
				for(tw_Loop = 0; tw_Loop < 96; tw_Loop ++)
				{
					DSPHZK_Block_Read(0,tw_DSPCodeIMphyadr+1,1,gc_PlayRecordDataBufHB);
					L2_DSP_MCU_IM_DMA(tw_Loop, (U16)gc_PlayRecordDataBuf);

					tw_DSPCodeIMphyadr ++;
				}
				DSPHZK_Block_Read(0,0+1,1,gc_PlayRecordDataBufHB);
			}
			else if ((gc_PlayRecordDataBuf[tw_IDIndexOffset + 1]) == tc_CurrentPMID)
			{//if it is PM,move PM to DSP
				U8 tc_buf_idx_start = 0;

				((U8 *)(&tw_DSPCodePMphyadr))[0] = gc_PlayRecordDataBuf[tw_IDIndexOffset + 2];
				((U8 *)(&tw_DSPCodePMphyadr))[1] = gc_PlayRecordDataBuf[tw_IDIndexOffset + 3]; 

				if((tc_CurrentPMID == 0x23)||(tc_CurrentPMID == 0x24)||(tc_CurrentPMID == 0x25))//(Jimi 091028)to save spi
					tc_buf_idx_start = 24;
				for(tw_Loop = tc_buf_idx_start; tw_Loop < 48; tw_Loop ++)
				{
					DSPHZK_Block_Read(0,tw_DSPCodePMphyadr+1,1,gc_PlayRecordDataBufHB);
					L2_DSP_MCU_PM_DMA(tw_Loop, (U16)gc_PlayRecordDataBuf);

					tw_DSPCodePMphyadr ++;
				}
				DSPHZK_Block_Read(0,0+1,1,gc_PlayRecordDataBufHB);
			}

		}
		if ((tw_DSPCodePMphyadr == 0xffff) && (tw_DSPCodeIMphyadr == 0xffff))	
		{//the IM and PM is no exist
			return DSP_IMPM_INEXISTENCE_ERROR;
		}
	}

	//4. Reset DSP 
#if DSP_RESET  //  ycc081031    
	GLOBAL_REG[0x02] |= 0x01;                                                             
#else
	tc_value=GLOBAL_REG[0x10] | 0x01;
#endif

	//5. Enable DSP CLK
	GLOBAL_REG[0x10]=tc_value;//GLOBAL_REG[0x10] |= 0x01;

 	//6. Release DSP reset
	GLOBAL_REG[0x02] &= 0xFE;                             // Enable DSP Run Normal Mode			
			
	DSP_REG[0x16]=0x00;  //Enable DM,IM,PM

	return DSP_SUCCESS;
}


U8 DSP_PlayInit(void)
{
#if DSP_RESET   //ycc081030 test
#else 
	// Jimi 091104 for EQ mode pop noise as next/prev
	if(gb_Play_firstPlay == 1)
	{
		L2_DSP_Write_DMem16(DSP_PostProcessSelect, 0x8003);
		gb_Play_firstPlay = 0;
	}
	else
	{
		L2_DSP_Write_DMem16(DSP_PostProcessSelect, 0x0003);
	}

	//U16 tw_temp;
	L2_DSP_SendCommandSet(DCMD_Restart); // ycc081031,force DSP jump 0 without hardware reset
	L2_DSP_Write_DMem16(DSP_RestartFlag,0x1235); // ycc081031
#endif

	//DSP_Write_MMR16(0x3FC0, 0x05B5);  // jimi,  ycc 081205

	if(DSP_SearchSunplusKey()!=DSP_SUCCESS)
	{
		return DSP_CHECK_SUNPLUSKEY_ERROR;
	}       
	else
	{
//	   	dbprintf("Find DSPSunplusKey !!\n");
	}

	// Clear all buffers in DSP
	DSP_ClearReady();
	L2_DSP_Write_DMem16(DSP_GoControl,0x1235);            
	{
		U16 dbgTmp;
	    U16 TimeOUT = 0xFFFF;
    
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

	tc_Data = L2_DSP_Read_DMem16(DSP_EffectControl);
    L2_DSP_Write_DMem16(DSP_EffectControl, tc_Data&0xF7FF);
	tc_Data = L2_DSP_Read_DMem16(DSP_SurroundDPCFLAG);
	L2_DSP_Write_DMem16(DSP_SurroundDPCFLAG,0xFFFD & tc_Data);
}

void DSP_SurroundEnable(void)
{
    data	U16	tc_Data;

    tc_Data = L2_DSP_Read_DMem16(DSP_EffectControl);
    L2_DSP_Write_DMem16(DSP_EffectControl, tc_Data&0xFDFF);

    tc_Data = L2_DSP_Read_DMem16(DSP_EffectControl);
    L2_DSP_Write_DMem16(DSP_EffectControl, tc_Data|0x0800);
	L2_DSP_Write_DMem16(DSP_SurroundDPCFLAG,0x0003);
}
 

U8 DSP_PauseCmd(void)  //20090803 chiayen modify
{
	if(L2_DSP_SendCommandSet(DCMD_Pause)!=DCMD_Pause)
	{
		return DSP_PAUSE_COMMAND_ERROR;
	}

	return DSP_SUCCESS;
}


U8  DSP_StopCmd(void)
{
    if(L2_DSP_SendCommandSet(DCMD_Stop) != DCMD_Stop)
	{
		return DSP_STOP_COMMAND_ERROR;
	}

  	return DSP_SUCCESS;
}

void DSP_EQ_Cmd(U8 tc_EQType)
{
	code U8  EQ_TABLE[][7]=
	{
		24,24,20,20,14,9,5,	 //DBB
		18,16,14,1,16,18,17, //JAZZ
		20,20,15,10,2,8,22,	 //ROCK
		12,18,4,12,18,12,8,	 //POP
		7,10,14,17,18,10,7,	 //LIVE(CLASSIC)
	};
	data	U16 tw_TmpData;
	xdata	U8  *tpc_EQPoint;

	tw_TmpData=L2_DSP_Read_DMem16(DSP_EffectControl); 

	if(tc_EQType == EQ_SSRmode) 
	{
		L2_DSP_Write_DMem16(DSP_EffectControl,tw_TmpData&0xFDFF);//change to NORMAL
		DSP_SurroundEnable(); 
	}
	else 
	{
		DSP_SurroundDisable();
		if(!tc_EQType) 
		{	//EQ disable == NORMAL MODE
			L2_DSP_Write_DMem16(DSP_EffectControl,tw_TmpData&0xFDFF);////EQ disable trigger
		}
		else 
		{				// EQ enable
			if(tc_EQType == EQ_USERDEFINE)
			{//user EQ
				tpc_EQPoint = &EQ_TABLE[tc_EQType][0];	 // user define
			}
			else
			{//default EQ
				tc_EQType--;
				tpc_EQPoint = &EQ_TABLE[tc_EQType][0];
			}
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
}

void DSP_SpectrumOn(void)
{
	data	U16	tw_TmpData;

	tw_TmpData=L2_DSP_Read_DMem16(DSP_EffectControl); 
	L2_DSP_Write_DMem16(DSP_EffectControl,tw_TmpData|0x0400);	//Open Spectrum
}

U8  DSP_ResetCmd(void)	//Jimi 080522
{
    if(L2_DSP_SendCommandSet(DCMD_Reset) != DCMD_Reset)
	{
		return DSP_STOP_COMMAND_ERROR;
	}

  	return DSP_SUCCESS;
}

