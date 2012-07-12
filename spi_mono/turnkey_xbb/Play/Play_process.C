#include "..\header\SPDA2K.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\IR\remote.h"
#include "..\header\host_init.h"
#include "..\header\variables.h"
#include "..\header\UI_config.h"
#include "..\LCM\LCM_BMP.h"


void Music_EQ_Cmd(void)
{
    DSP_EQ_Cmd(gs_DSP_GLOBAL_RAM.sc_EQ_Type);
    DSP_SpectrumOn();
}


U8 MP3_DataIn(void)
{   
	data	U8	tc_status;
	xdata	U16	tw_SmpRateIdx;

	while(L2_DSP_Read_DMem16(DSP_EmptyBuffer)>=512)
    {
        tc_status=DOS_Read_File(C_MusicFHandle);
        if(tc_status == DOS_END_OF_FILE)
        {
			L2_DSP_Write_DMem16(DSP_MP3_file_end_flag,1);
            return DSP_DATAIN_COMMAND_ERROR;
        }
        else if(tc_status)
        {
            return tc_status;   
        }

		L2_DSP_MCU_DM_DMA_24bit(gs_DSP_GLOBAL_RAM.sc_DM_Index++,0x20,(U16)gc_PlayRecordDataBuf);
        L2_DSP_SendCommandSet(DCMD_DatIn);
        if (gs_DSP_GLOBAL_RAM.sc_DM_Index>=24)
        {
            gs_DSP_GLOBAL_RAM.sc_DM_Index=0;  // DM Range: 0x2000 ~  0x2FFF
        }
    }

    tw_SmpRateIdx = L2_DSP_Read_DMem16(0x3F8E);	//0x3F8E is an index of mp3 sampling rate within DSP
	if((tw_SmpRateIdx==0)||(tw_SmpRateIdx==3)||(tw_SmpRateIdx==6))
	{
		XBYTE[0xB046]|=0x04;
	}
    else
	{
		XBYTE[0xB046]&=~0x04; 
	}
    return  DSP_SUCCESS;
}


U16 MP3_ReadTime(void) 
{
	code	U16	SampleRate[4][3]={{11025,12000,8000},{0,0,0},{22050,24000,16000},{44100,48000,32000}};
	data	U8	tc_SampleRate;
	data	U8	tc_MPEG_Type;
	data	U8	tc_Layer;
	data	U16 tw_DurTime;
	data	U16 tw_SamplePerFrame;
	xdata	U32 tdw_Mpeg_Status;
	xdata	U32 tdw_FrameCnt;

	tdw_Mpeg_Status=L2_DSP_Read_DMem24(DSP_MPEGSt);
	tdw_FrameCnt=L2_DSP_Read_DMem24(DSP_DecodeFrameCounter);
	tc_SampleRate=((U8)(tdw_Mpeg_Status>>5)&0x03);
	tc_MPEG_Type=((U8)(tdw_Mpeg_Status>>(14+8))&0x03);
	tc_Layer=((U8)(tdw_Mpeg_Status>>(12+8))&0x03);

	if(tdw_FrameCnt>0x20)
	{
		if(tc_Layer==0x03) //layer-I, reference ISO/IEC 11172-3 header description
		{
			tw_SamplePerFrame=384;
		}
		else if((tc_MPEG_Type!=0x03)&&(tc_Layer==0x01))  
		{
			tw_SamplePerFrame=576; 
		}
		else
		{
			tw_SamplePerFrame=1152;
		}
    
		if(tc_MPEG_Type!=0x01)
		{
			tw_DurTime=gw_CurrentSec+tdw_FrameCnt*tw_SamplePerFrame/SampleRate[tc_MPEG_Type][tc_SampleRate];
		}
		else
		{
			tw_DurTime=0;
		}
	}
	else//(JC)min ~32 frames -> 1sec
	{
		tw_DurTime=gw_CurrentSec;
	}

	return tw_DurTime;
}

U8 MP3_EOF_Proc(void)
{                       
	data	U16 tw_BufRem;
	data	U16 tw_RampDownBufSz;
	data	U16 tw_dec_bsbuf_rem;
	
	L2_DSP_Write_DMem16(0x3F44,8000);	//Jimi 100517, enlarge DSP MP3 decoder ramp down step
	L2_DSP_Write_DMem16(0x3F45,8000);
	
	tw_BufRem = L2_DSP_Read_DMem16(DSP_RemainBuffer);
	tw_RampDownBufSz =(U16)(((U32)gs_System_State.w_SampleRate * 1000 )/8/20);  // 0.05 sec for ramp down
	tw_dec_bsbuf_rem = L2_DSP_Read_DMem16(DSP_MP3_dec_bsbuf_rem);   //Jimi 091126

	if(tw_RampDownBufSz<512)     //for 8k bitrate condition(M2L3 & M2.5L3)
	{
		tw_RampDownBufSz=512;
	}
    
	if(tw_BufRem<tw_RampDownBufSz)
	{
		if(L2_DSP_Read_DMem16(DSP_RampDownComplete) == 0)	//(Jimi 091126)RampDownOK == 0
		{
			if(tw_dec_bsbuf_rem < tw_RampDownBufSz)	//Jimi 100629, 0x3FB5: RampDownFlag
			{
				return 1;
			}
		}
		else
		{	
			return 1;
		}
	}
    
	return 0;
}


U8 WMA_err_bs_proc(void)
{
	data	U8	tc_iloop;
	data	U8	tc_tmp;
	xdata	U32	tdw_testcluster;
	
	tc_tmp=L2_DSP_Read_DMem16(DSP_DecodeStatus);
	if(tc_tmp&0x20)	//wma format error or drm
	{
		DSP_ResetCmd();
		return DSP_USER_FILE_TYPE_ERROR;
	}
    
	if(tc_tmp&0xC0)
	{
		tdw_testcluster = (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint >> 9)+5;
		if( tdw_testcluster >= (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize >> 9) )
		{
			return DSP_RUNNING_STATUS1_ERROR;
		}

		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster =gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint = tdw_testcluster << 9;
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = DOS_Seek_File(gs_System_State.c_FileHandle,gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint>>9);
		L2_DSP_Write_DMem16(DSP_EmptyBuffer,0x3C00);
		L2_DSP_Write_DMem16(DSP_RemainBuffer,0x0000);   
		gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;      
		L2_DSP_Write_DMem24(DSP_WMASectorOffset,(gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint>>9));      
		L2_DSP_Write_DMem16(DSP_DecodeStatus,0x0000);
		L2_DSP_Write_DMem16(DSP_WMARandomFlag,0x0001);   //set random flag
		tc_iloop = 0;

		do
		{
			if (DOS_Read_File(C_MusicFHandle) == DOS_END_OF_FILE)
			{
				return DSP_DATAIN_COMMAND_ERROR;
			}

			L2_DSP_MCU_DM_DMA_24bit(gs_DSP_GLOBAL_RAM.sc_DM_Index++,0x20,(U16)gc_PlayRecordDataBuf);
			L2_DSP_SendCommandSet(DCMD_DatIn); 
			
			tc_iloop ++;
		}while(tc_iloop!=5);
	}
	return DSP_SUCCESS; 
}


U8 WMA_DataIn(void)
{
	xdata	U8  tc_errbs;
	
	/* Error Bitstream Process */
	tc_errbs = WMA_err_bs_proc();
	
	if(tc_errbs == DSP_USER_FILE_TYPE_ERROR || tc_errbs == DSP_RUNNING_STATUS1_ERROR)
	{
		return DSP_DATAIN_COMMAND_ERROR;
	}   
	
	while(L2_DSP_Read_DMem16(DSP_EmptyBuffer)>512)
	{
		if(DOS_Read_File(0x00)==DOS_END_OF_FILE)
		{
			return DSP_DATAIN_COMMAND_ERROR;
		}
		
		L2_DSP_MCU_DM_DMA_24bit(gs_DSP_GLOBAL_RAM.sc_DM_Index++,0x20,(U16)gc_PlayRecordDataBuf);
		L2_DSP_SendCommandSet(DCMD_DatIn);
		
		if (gs_DSP_GLOBAL_RAM.sc_DM_Index >= 30)  //20090420 chiayen modify 36->30
		{
			gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // DM Range: 0x2000 ~  0x37FF
		}
	}
	return  DSP_SUCCESS;
}


U16 WMA_ReadTime(void)
{
	xdata	U32 tw_cur_time_h;
	xdata	U32 tw_cur_time;
	
	tw_cur_time_h=L2_DSP_Read_DMem24(DSP_WMACurrenTimeHigh);
	tw_cur_time=L2_DSP_Read_DMem24(DSP_WMACurrenTimeLow)+(tw_cur_time_h<<24);
	
	return(tw_cur_time/1000);    
}


U8 WMA_EOF_Proc(void)
{    
	data	U8  tc_wma_eof_cnt=0;
	data	U16	tw_DSP_RemainBufPrev=0;
	xdata	U16	tw_bs_buf_remain;
	xdata	U16	tw_DSP_dec_status;
	xdata	U16	tw_RampDownBufSz;
	
	tw_bs_buf_remain=L2_DSP_Read_DMem16(DSP_RemainBuffer);
	tw_RampDownBufSz=(U16)(((U32)gs_System_State.w_SampleRate*1000)/8/2);  // 0.5 sec for ramp down

	if(tw_bs_buf_remain<=tw_RampDownBufSz)
	{
		return 1;
	}
	
	if(tw_bs_buf_remain==tw_DSP_RemainBufPrev)
	{
		tc_wma_eof_cnt++;
	}   
	else
	{
		tw_DSP_RemainBufPrev=tw_bs_buf_remain;
		tc_wma_eof_cnt=0;
	}    
	tw_DSP_dec_status = L2_DSP_Read_DMem16(DSP_DecodeStatus) & 0xC0;
	
	if( tw_DSP_dec_status || (tc_wma_eof_cnt >=10) )
	{
		L2_DSP_Write_DMem16(DSP_DecodeStatus, tw_DSP_dec_status|0xC0 );//為了避免之後的play_stop hang住
		return 1;
	}
    
	return 0;   
}


U8 Music_PlayCmd(void)
{
	// sent "PLAY" command to DSP
	if(L2_DSP_SendCommandSet(DCMD_Play)!=DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
	
	if(gc_Play_FileType==0)	//(Jimi 091028)wma do not need to reset bitstream address cuz the asf parser is contiguous to wma decoder 
	{
		gs_DSP_GLOBAL_RAM.sc_DM_Index=0;  // reset bitstream address of DM//(Jimi 091104 mdf)
		// MP3 file open        
		if(DOS_Open_File_r(C_MusicFHandle,C_Open_FoundFile,C_NullFileName))
		{
			return DSP_PLAY_COMMAND_ERROR;
		}
	}
	return 0;
}


U8 Music_PauseCmd(void)
{   
	data	U8  tc_Ret;
	data	U16	TimeOUT;
	
	DSP_WakeUp();
	tc_Ret=DSP_PauseCmd();
	TimeOUT=0xFFFF;

	if(gc_Play_FileType==0)
	{
		do
		{
			TimeOUT--;
			if(TimeOUT==0)
			{
				return DSP_RUNNING_STATUS1_ERROR;
			}
		} while(!(L2_DSP_Read_DMem16(DSP_RunningStatus)&0x0002));
	}
	else
	{
		do
		{
			TimeOUT--;
			if(TimeOUT==0)
			{
				return DSP_RUNNING_STATUS1_ERROR;
			}
		} while (!L2_DSP_Read_DMem16(DSP_RampDownComplete)); //H1226 DSP_RampDownComplete = 0x3FB8, indicates RampDown OK or NOT
	}

	return tc_Ret;
}


U8 Music_ResumeCmd(void)
{
	DSP_WakeUp();
	
	if(L2_DSP_SendCommandSet(DCMD_Play)!=DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
	return DSP_SUCCESS;
}


U8 Music_StopCmd(U8 tc_FileHandle)
{
	data	U8  tc_Ret;
	data	U8	tc_value;
	data	U16 tw_temp;
	data	U16 dbgTmp;

	DSP_WakeUp();
	tc_Ret=DSP_StopCmd();
	
	// Reset decode status
	tw_temp=0xFFFF;
	do
	{
		tw_temp--;
		if(tw_temp==0)
		{
			tc_Ret=DSP_DECODE_STATUS_TIMEOUT_ERROR;
			break;
		}
	}while(!((L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)==0x00C0));  //wait file end

	tw_temp=L2_DSP_Read_DMem16(DSP_DecodeStatus);
	L2_DSP_Write_DMem16(DSP_DecodeStatus,tw_temp&0xFF3F);
	
	// Close current file
	gs_File_FCB[tc_FileHandle].dw_File_StartCluster=0xFFFFFFFF;
	 
	// wait for ramp down ok and Turn off DSP clock, ycc 081113
	tw_temp=0xFFFF;
	while(1)
	{
		dbgTmp=L2_DSP_Read_DMem16(DSP_RestartFlag);  //read DSP 0x3F0B.0 ready status
		tw_temp--;     
		if((tw_temp==0)||(dbgTmp==0))
		{           
			break; 
		} 
	}
	tc_value=XBYTE[0xB010]&0xFE;
	XBYTE[0xB010]=tc_value;   
    return tc_Ret;
}


void play_stop()
{
	if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
	{
		Music_StopCmd(gs_System_State.c_FileHandle);
		gs_System_State.c_Phase=TASK_PHASE_STOP;
	}
}


void RandomGetFileIndex(void)
{
	data	U16	tw_Num;
	
	tw_Num=(gw_Random_Timer%gw_FileTotalNumber[gs_System_State.c_FileHandle])+1;
	
	if(gw_FileIndex[gs_System_State.c_FileHandle]==tw_Num)
	{
		gw_FileSkipNumber=0;
	}
	else
	{
		if (gw_FileIndex[gs_System_State.c_FileHandle] > tw_Num)
		{
			gw_FileSkipNumber=gw_FileTotalNumber[gs_System_State.c_FileHandle] - (gw_FileIndex[gs_System_State.c_FileHandle] - tw_Num);
		}
		else
		{
			gw_FileSkipNumber=(tw_Num - gw_FileIndex[gs_System_State.c_FileHandle]);
		}
	}
}


void play_next()
{
	gc_LCMScrollTimer=0;
	switch(gs_System_State.c_Phase)
	{
		case TASK_PHASE_STOP:
			if(gw_FileIndex[gs_System_State.c_FileHandle]==0)
			{
				return;
			}
		break;

		case TASK_PHASE_PAUSE:
		case TASK_PHASE_PLAYACT:
			Music_StopCmd(gs_System_State.c_FileHandle);
			gs_System_State.c_Phase=TASK_PHASE_STOP;
			gb_Mp3FileEnd=1;
		break;
	}

	switch(gc_RepPlayMode)
	{
		case C_RepeatAll:
		case C_PlayAllOnce:
		case C_RepeatOne:
			gb_FDBLastFlag=0;
			DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
			if(gb_FDBLastFlag)
			{
				PlayNextFolder();
			}
		break;

		case C_InDirRepeat:
		case C_InDirOnce:
			DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
		break;

		case C_RandomPlay:
			RandomGetFileIndex();
			if(gb_DirPlay_Flag==1)
			{
				gb_TriggerFileSkip=1;
				DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
				gb_TriggerFileSkip=0;
			}
			else
			{
				DOS_Search_File(C_File_All|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
			}
		break;
	}
	LCM_Clear_L2_L5();
	gc_ShowTimer=67;
	LCM_ShowSongNumber();
}


void play_prev()
{
	gc_LCMScrollTimer=0;
	switch(gs_System_State.c_Phase)
	{
		case TASK_PHASE_STOP:
			if(gw_FileIndex[0]==0)
			{
				return;
			}
		break;

		case TASK_PHASE_PAUSE:
		case TASK_PHASE_PLAYACT:
			Music_StopCmd(0);
			gs_System_State.c_Phase=TASK_PHASE_STOP;
			gb_Mp3FileEnd=1;
		break;
	}

	switch(gc_RepPlayMode)
	{
		case C_RepeatAll:
		case C_PlayAllOnce:
		case C_RepeatOne:
			if(gw_FileIndex[0]==1)
			{
				PlayPrevFolder();
				DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Prev);
			}
			else
			{
				DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Prev);
			}
		break;

		case C_InDirRepeat:
		case C_InDirOnce:
			DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Prev);
		break;

		case C_RandomPlay:
			RandomGetFileIndex();
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
			gb_TriggerFileSkip=0;
		break;
	}
	LCM_Clear_L2_L5();
	gc_ShowTimer=67;
	LCM_ShowSongNumber();
	gc_PrevFlag=1;
}


void VOL_Adj(void)
{
	gc_LogData_Timer=67;
	if(gb_ADJ==0)
	{
		if(gs_DSP_GLOBAL_RAM.sc_Volume<42)
		{
			gs_DSP_GLOBAL_RAM.sc_Volume+=2;
			L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sc_Volume);     // Vloume Range:0~63
			LCM_ShowVOL();
		}
	}
	else
	{
		if(gs_DSP_GLOBAL_RAM.sc_Volume>1)
		{
			gs_DSP_GLOBAL_RAM.sc_Volume-=2;
			L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sc_Volume);     // Vloume Range:0~63
			LCM_ShowVOL();
		}
	}
}

void ReadBackupDataCompare(void)
{
	data	U8	tc_clock_mode_backup;

	tc_clock_mode_backup=gc_clock_mode;
	set_clock_mode(CLOCK_MODE_DOS);
	DOS_Count_All_Dir();
	gs_File_FCB[0].dw_FDB_StartCluster=gdw_DOS_RootDirClus;
	DOS_Search_File(C_File_All|C_Cnt_FileNo,C_MusicFileType,C_CmpExtName|C_Next);
	set_clock_mode(tc_clock_mode_backup);
	if(gw_FileTotalNumber[0]==0)
	{
		// No Music file
		gw_FileIndex[0]=0;
		LCM_Show_NoFile();
		if(StorgeNoMp3FileChangeNext())
		{
			gb_Mp3FileEnd=0;
			gb_Dosinitfail=1;
			return;
		}
	}

	Get_LogData_PageIndex();
	USER_LogFile_ReadWrite(0);
	DOS_Search_File(C_File_OneDir|C_Cnt_FileNo,C_MusicFileType,C_CmpExtName|C_Next);
	if(gw_FileTotalNumber[0]==0)
	{
		PlayNextFolder();
	}
	JumpToTargetFolder();
}


bit StorgeNoMp3FileChangeNext(void)
{
	data	U8	x_NoFile;
	data	U8	x_BackUpClock;
	data	U8	x_DosInit;
	
	x_NoFile=0;
	x_DosInit=0;
	gb_Dosinitfail=0;
	switch(gc_CurrentCard)
	{
		case 0x02:   //SD-> HOST-> FM
			if((gb_Host_Exist==1)&&(gb_HostNoFileflag==0))
			{
				if(!Host_Initial())
				{
					gc_CurrentCard=5;
					x_DosInit=1;
				}
			}
		break;
		
		case 0x05:   //HOST-> SD-> FM
			if((gb_SD_Exist==1)&&(gb_SDNoFileflag==0))
			{
				if(SD_Identification_Flow())
				{
					gc_CurrentCard=2;
					x_DosInit=1;
				}
			}
		break;
	}

	if(x_DosInit)
	{
		x_BackUpClock=gc_clock_mode;
		x_NoFile=0;
		if(!DOS_Initialize())
		{
			set_clock_mode(CLOCK_MODE_DOS);
			gw_FileSkipNumber=0;
			gb_FindFlag = 0;
			gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
			DOS_Search_File(C_File_All|C_Cnt_FileNo,C_MusicFileType,C_CmpExtName|C_Next);// count music file no. in root
			set_clock_mode(x_BackUpClock);
			gc_KeyEvent=0;
			if(gw_FileTotalNumber[0])
			{
	            x_NoFile=1;
			}
		}
	}

	if(x_NoFile==0)
	{
		gb_FindFlag=0;
		gc_Task_Current=C_Task_Idle;
		gc_CurrentCard=9;
		return 1;
	}
	return 0;
}


void DosSearchErrorNextStorageType(void)
{
	switch(gc_CurrentCard)
	{
		case 0x02: //Current SD Type
			if(gb_Host_Exist)
			{
				gc_CurrentCard=5;
				if(!Host_Initial())
				{
					if(!DOS_Initialize())
					{
						gb_Dosinitfail=1;
					}
				}
			}
		break;

		case 0x05:  //Host
			if(gb_SD_Exist)
			{
				gc_CurrentCard=2;
				if(SD_Identification_Flow())
				{
					if(!DOS_Initialize())
					{
						gb_Dosinitfail=1;
					}
				}
			}
		break;
	}

	if(gb_Dosinitfail)
	{
		gb_Dosinitfail=0;
	}
	else
	{
		gb_FindFlag=0;
		gc_Task_Current=C_Task_Idle;
		gc_CurrentCard=9;
		gb_Dosinitfail=1;
	}
}


void Play_SDFlash_DosInit()
{
	data	U8  tc_clock_mode_backup; 

	tc_clock_mode_backup=gc_clock_mode;
	gb_Dosinitfail=0;					// 20100106-maxliao(未加此行,只要Fail一次就再也讀不到)
	gdw_HOSTStartSectorRead=0xFFFFFFF0;	// 20100106-maxliao
	set_clock_mode(CLOCK_MODE_DOS);
	if(DOS_Initialize())
	{
		DosSearchErrorNextStorageType();
	}

	gw_FileSkipNumber=0;
	gb_FindFlag=0;
    //-------------------------------------------------------------------------------------------------------
	set_clock_mode(tc_clock_mode_backup);
	gs_System_State.c_FileHandle=0;

    gb_Dosinitfail=0;
	gb_LrcFileName_Exist=0;
	gs_DSP_GLOBAL_RAM.sc_DM_Index=0;  // reset bitstream address of DM//(Jimi 091104 mdf)
	gs_DSP_GLOBAL_RAM.sdw_File_ACluster=0;
	gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint=0;
	gs_DSP_GLOBAL_RAM.sdw_File_BCluster=0;
	gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint=0;
	gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB_NULL;
	gw_CurrentSec=0;
	gw_FileIndex[0]=0;
	//-------------讀取上一次的資料---------------------------------------
	ReadBackupDataCompare();
	//--------------------------------------------------------------------
	gc_Play_FileType=gs_File_FCB[0].c_FileType;
	if(gc_Play_FileType==0)
	{
		set_clock_mode(CLOCK_MODE_MP3);
		gs_System_State.w_SampleRate=MP3_Bitrate_Parser();
		gw_TotalSec=MP3_Total_Time_Parser();//(JC)H0630 LCM test
	}
	else
	{
		set_clock_mode(CLOCK_MODE_WMA);
		WMA_ASF_Parser();
	}

	gc_KeyEvent=0;
	gc_LCMScrollTimer=0;
	gb_Mp3FileEnd=1;
}


void Play_SourceDetect_Process(void)
{
    data	U8	xc_temp;

   	gb_SD_Exist_pre=gb_SD_Exist;
	if(gb_SD_pin==0)
	{
		gb_SD_Exist=1;     //SD exist
	}
	else
	{
		gb_SD_Exist=0;
		gb_SDNoFileflag=0;
	}
	gb_Host_Exist_pre=gb_Host_Exist;
	if(!Host_DetectDevice())
	{
		gb_Host_Exist=1;    //host_exist
	}
	else
	{
		gb_Host_Exist=0;
		gb_HostNoFileflag=0;
	}

	if(gb_HostError==1)
	{
		if(gb_SD_Exist==0)
		{
			gc_CurrentCard=9;
		}
		gb_HostError=0;
	}
	else
	{
		if(gb_MediaMask)
		{	//手動切換 //無檔案時也可以切換
			gb_MediaMask=0;
		}
		else
		{	//自動切換
			if((gb_SD_Exist_pre==gb_SD_Exist)&&(gb_Host_Exist_pre==gb_Host_Exist)) 
			{
				return;
			}
		}
	}

	switch(gc_CurrentCard)
	{
		case 0x02:   //SD-----------------------------------------
            if(gb_Host_Exist)
			{
				play_stop();          //切換到HOST MODE
				gc_CurrentCard=5;
			}
			else
			{
				if(gb_SD_Exist)
				{
					gc_CurrentCard=2;
					return;            //SD插著,HOST拔出
				}
				else
				{
					play_stop();       // Idle
					gc_CurrentCard=9;
				}
			}
		break;

		case 0x05:   //HOST---------------------------------------
			if(gb_SD_pin==0)
			{
				play_stop();          //切換到SD MODE
				gc_CurrentCard=2;
			}
			else
			{
				if(gb_Host_Exist)
				{
					gc_CurrentCard=5;
					return;            //HOST插著,SD拔出
				}
				else
				{
					play_stop();       // FM MODE
					gc_CurrentCard = 9;
				}
			}
		break;
	}
	LCM_Clear_L2_L5();
	LCM_ShowWait();
	set_clock_mode(CLOCK_MODE_MP3); 
    //----------------------------------------------------------
	xc_temp=0;
	switch(gc_CurrentCard)
	{
		case 0x02: //SD CARD--------------------------------------
			if(SD_Identification_Flow()) //return 1 Success
			{
               xc_temp=1;
			}
			else
			{
				if((gb_Host_Exist==1)&&(gb_HostNoFileflag==0))
				{
					if(!Host_Initial())
					{
						gc_CurrentCard=5;
						gb_FindFlag=0;
						xc_temp=1;
					}
				}
			}
		break;

		case 0x05: //HOST-----------------------------------------
			if(!Host_Initial())
			{
				xc_temp=1;
				gb_FindFlag=0;
			}
			else
			{
				if((gb_SD_Exist==1)&&(gb_SDNoFileflag==0))
				{
					if(SD_Identification_Flow())
					{
						gc_CurrentCard=2;
						xc_temp=1;
					}
				}
			}
		break;
	}
    //------------Check DosInit-----------------------------------------------------------
	if(xc_temp)
	{
		Play_SDFlash_DosInit();
		gc_LastCard=gc_CurrentCard;
	}
	else
	{
		gc_CurrentCard=9;
		gb_FindFlag=0;
		gc_Task_Current=C_Task_Idle;
	}
}


void LRC_DisplayFunction(void)
{
	Lyric_GetTimeStamp();
	if(gb_LrcGetTimeOrNot)
	{
		gb_LrcGetTimeOrNot=0; 
		Lyric_GetLyrics();
		if(gc_LrcCurrentLen)
		{
			if(gc_Play_FileType==0)
			{
				MP3_DataIn();
			}
			else
			{
				WMA_DataIn();	
			}
			LCM_DisplayString(&gc_LrcDisplayBuf,gc_LrcCurrentLen-1,32);
		}
	}

}


void CheckAudioStatus(void)
{
	data	U8	tc_status;

	if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
	{
	    if((gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_AB)||(gs_File_FCB[0].dw_File_DataPoint<=gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint))
		{
			if(gc_Play_FileType==0)
			{
				gw_Disp_CurrentSec=MP3_ReadTime();
				tc_status=MP3_DataIn();
				if(tc_status)
				{
					tc_status=MP3_EOF_Proc();
				}
			}
			else
			{
				gw_Disp_CurrentSec=WMA_ReadTime();
				tc_status=WMA_DataIn();
				if(tc_status)
				{
					tc_status=WMA_EOF_Proc();
				}
			}

			if(tc_status)
			{ //End of File for play
				play_stop();
				if(gc_CurrentCard==2&&gb_SD_pin==1)
				{
					gc_CurrentCard=9;
					return;
				}
				gw_CurrentSec=0;
				gb_Mp3FileEnd=1;

				switch(gc_RepPlayMode)
				{       
					case C_RepeatAll:
					case C_PlayAllOnce:
						gb_FDBLastFlag=0;
						DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
						if(gb_FDBLastFlag==1&&gc_RepPlayMode==C_PlayAllOnce)
						{
							gb_Mp3FileEnd=0;
						}
						else if(gb_FDBLastFlag)
						{
							PlayNextFolder();
						}
					break;

					case C_RepeatOne:
					break;

					case C_RandomPlay:
						RandomGetFileIndex();
						DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
					break;

					case C_InDirRepeat:
					case C_InDirOnce:
						gb_FDBLastFlag=0;
						DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
						if(gb_FDBLastFlag==1)
						{
							if(gc_RepPlayMode==C_InDirRepeat)
							{
								gw_FileIndex[0]=1;
							}
							else
							{
								gw_FileIndex[0]--;
								gb_Mp3FileEnd=0;
							}
						}
					break;
				}
			}
			else
			{
				gc_PrevFlag=0;
			}
		}
		else
		{
			if(gc_Play_FileType==0)
			{
				MP3_Repeat_DataIn();
			}
			else
			{
				WMA_Repeat_DataIn();
			}
		}
	}
}


void OpenAudioFile(void)
{
	data	U8 tc_timeout;

	if(gb_Mp3FileEnd)
	{
		USER_LogFile_ReadWrite(1);
		gb_Mp3FileEnd=0;
		gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
		DOS_GetLongFileName(0,gc_FileLongName);	 
		//-------------LRC Clear Buffer100-------------------------------
		gb_LrcFileName_Exist=Lyric_FileSearch();//(JC)Lyric file exist?
		#ifdef SPI_1M		  
		X_Unicode2ISN(gc_FileLongName);
		#endif
		if(gb_LrcFileName_Exist)
		{
			memset(gc_LrcDisplayBuf,0,100); //Clear
		}
		gc_LrcCurrentLen=0;
		
		gc_Play_FileType=gs_File_FCB[0].c_FileType;

		gs_DSP_GLOBAL_RAM.sdw_File_ACluster=0;
		gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint=0;
		gs_DSP_GLOBAL_RAM.sdw_File_BCluster=0;
		gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint=0;
		gw_CurrentSec=0;
		gw_Disp_CurrentSec=0;
		if(gc_Play_FileType==0)
		{
			if(gb_LrcFileName_Exist==1)
			{
				set_clock_mode(CLOCK_MODE_MP3L);
			}
			else
			{
				set_clock_mode(CLOCK_MODE_MP3);
			}
			gs_System_State.w_SampleRate=MP3_Bitrate_Parser();
			gw_TotalSec=MP3_Total_Time_Parser();
			gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster=gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
			gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster=DOS_Seek_File(gs_System_State.c_FileHandle, gdw_ID3_v2_sz >> 9);
			gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint=gdw_ID3_v2_sz & 0xFFFFFE00;
			gs_DSP_GLOBAL_RAM.sc_DM_Index=0;  // reset bitstream address of DM//(Jimi 091104 mdf)
			if(MP3_Download())
			{
				play_next();
				return;
			}
		}
		else
		{
			set_clock_mode(CLOCK_MODE_WMA);
			WMA_ASF_Parser();
			if(WMA_Download())
			{
				play_next();
				return;
			}
		}

		L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sc_Volume);
		L2_DSP_Write_DMem16(DSP_PostProcessSelect,L2_DSP_Read_DMem16(DSP_PostProcessSelect)|0x0020);
		tc_timeout=0xFF;
		while((L2_DSP_Read_DMem16(DSP_PostProcessSelect)&0x0020))  //Handshake with DSP to make sure that DSP has ramp digital volume up.
		{
			if(!(tc_timeout--))
			{
				break;
			}
		}
		Music_EQ_Cmd();
		//-------------------------------------------------------------------------------
		gw_DispFileName_ByteOffset=0; 
		gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB_NULL; // 每次新播一首曲目，都要把 RepeatAB 重置
		RefreshAllDisplay();
		if(Music_PlayCmd()==DSP_PLAY_COMMAND_ERROR)
		{
			play_next();
		}
	}
}


void FillAudioDataToDSP(void)
{
	if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
	{
		if(gc_Play_FileType==0)
		{
              MP3_DataIn();
		}
		else 
		{
             WMA_DataIn();
		}
	}
}


void MP3FunctionProc(void)
{
	data	U8	tc_Step;
	data	U16	tw_temp;
	xdata	U32	tdw_DataPoint;

	switch(gs_System_State.c_Phase)
	{
		case TASK_PHASE_FASTFWD:	// FF/FR
			switch(gc_KeyEvent)
			{
				case 0x13:	// FF
					UI_fastFFFR_Play(0);
					LCM_ShowPlayTime(gw_Disp_CurrentSec);
					gc_LongKeyCount=30;
				break;

				case 0x23:	// Stop FF/FR
					play_fffr_end();
					gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
				break;
			}
		break;

		case	TASK_PHASE_FASTREV:	// 20100317 - FF/FR
			switch(gc_KeyEvent)
			{
				case 0x14:	// FR
					UI_fastFFFR_Play(1);
					LCM_ShowPlayTime(gw_Disp_CurrentSec);
					gc_LongKeyCount=30;
				break;

				case 0x24:	// Stop FF/FR
					play_fffr_end();
					gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
				break;
			}
		break;

		case TASK_PHASE_PLAYACT:
			if((gc_MenuTimer==0)&&(gc_PlayMenuItem!=0||gb_SetVol))
			{
				gb_SetVol=0;
				gc_PlayMenuItem=0;
				RefreshAllDisplay();
				USER_LogFile_ReadWrite(1);
			}

			if(gc_KeyEvent!=0)
			{
				if(gc_PlayMenuItem!=0)
				{
					gc_MenuTimer=6;
				}

				switch(gc_KeyEvent)
				{
					case 0x02:	// Play
						if(gc_PlayMenuItem==0)
						{
							Music_PauseCmd();
							gs_System_State.c_Phase=TASK_PHASE_PAUSE;
							LCM_ShowPlayPauseIcon();
							ClearIRNumberVariable();
						}
						else
						{
						}
					break;

					case 0x03:	// Next
						if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_A)
						{
							gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_B;
							if(gc_Play_FileType==0)
							{
								gs_DSP_GLOBAL_RAM.sdw_File_ACluster=0xFFFFFFFF;
								tdw_DataPoint=L2_DSP_Read_DMem16(DSP_RemainBuffer);
								tdw_DataPoint&=0xFFFFFE00;
								gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint=(gs_File_FCB[0].dw_File_DataPoint)-(tdw_DataPoint);
								gw_AFrameCnt=L2_DSP_Read_DMem16(DSP_DecodeFrameCounter);// to get A point frame count
							}
							else
							{
								gw_APointSec=WMA_ReadTime();				//Jimi 080529
								tdw_DataPoint=L2_DSP_Read_DMem16(DSP_RemainBuffer);
								tdw_DataPoint&=0xfffffe00;
								gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint=(gs_File_FCB[0].dw_File_DataPoint)-(tdw_DataPoint);
							}
						}
						else if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_B)
						{
							gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB;
							if(gc_Play_FileType==0)	// MP3
							{
								gs_DSP_GLOBAL_RAM.sdw_File_BCluster=gs_File_FCB[0].dw_File_CurrentCluster;
								gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint=gs_File_FCB[0].dw_File_DataPoint;
							}
							else					// WMA
							{
								if(WMA_ReadTime()!=gw_APointSec)
								{
									gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint=gs_File_FCB[0].dw_File_DataPoint;
									gs_DSP_GLOBAL_RAM.sdw_File_BCluster=gs_File_FCB[0].dw_File_CurrentCluster;
									WMA_FF_FR_Cmd();
									gs_File_FCB[0].dw_File_DataPoint=0;
									gs_File_FCB[0].dw_File_CurrentCluster=gs_File_FCB[0].dw_File_StartCluster;
									gs_DSP_GLOBAL_RAM.sdw_File_ACluster=DOS_Seek_File(0,gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint>>9);
									gs_File_FCB[0].dw_File_CurrentCluster=gs_DSP_GLOBAL_RAM.sdw_File_ACluster;
									gs_File_FCB[0].dw_File_DataPoint=gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint;
									WMA_seek_trig();
								}
							}
						}
						else if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_AB)
						{
							gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_A;
						}
						else if(gc_PlayMenuItem==0)	// 1=Play-Menu  2=Repeat-Mode  3=EQ-Option
						{
							gb_ClearBigIcon=1;
							ClearIRNumberVariable();
							if(gb_SetVol)
							{
								gb_ADJ=0;
								VOL_Adj();
							}
							else
							{						
								play_next();
							}
						}
						else if(gc_PlayMenuItem==1)	// Play Menu(Repeat/EQ/A-B/Exit)
						{
							tc_Step=gc_PlayMenuNum;
							gc_PlayMenuNum=(gc_PlayMenuNum+1)&0x03;
							LCM_ShowPlayMenu(tc_Step);
							LCM_ShowPlayMenu(gc_PlayMenuNum);
						}
						else if(gc_PlayMenuItem==2)	// Repeat Menu
						{
							tc_Step=gc_RepeatMenuNum;
							gc_RepeatMenuNum=(gc_RepeatMenuNum+1)%6;
							if(gc_RepeatMenuNum==4)
							{
								LCM_ShowRepeatMenu(4);
								LCM_ShowRepeatMenu(5);
								LCM_erase_one_page(4);
								LCM_erase_one_page(5);
								LCM_erase_one_page(6);
								LCM_erase_one_page(7);
							}
							else if(gc_RepeatMenuNum==0)
							{
								LCM_ShowRepeatMenu(0);
								LCM_ShowRepeatMenu(1);
								LCM_ShowRepeatMenu(2);
								LCM_ShowRepeatMenu(3);
							}
							else
							{
								LCM_ShowRepeatMenu(tc_Step);
								LCM_ShowRepeatMenu(gc_RepeatMenuNum);
							}
						}
						else if(gc_PlayMenuItem==3)	// EQ Menu
						{
							tc_Step=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
							gs_DSP_GLOBAL_RAM.sc_EQ_Type=(gs_DSP_GLOBAL_RAM.sc_EQ_Type+1)%6;
							if(gs_DSP_GLOBAL_RAM.sc_EQ_Type==4)
							{
								LCM_ShowEQMenu(4);
								LCM_ShowEQMenu(5);
								LCM_erase_one_page(4);
								LCM_erase_one_page(5);
								LCM_erase_one_page(6);
								LCM_erase_one_page(7);
							}
							else if(gs_DSP_GLOBAL_RAM.sc_EQ_Type==0)
							{
								LCM_ShowEQMenu(0);
								LCM_ShowEQMenu(1);
								LCM_ShowEQMenu(2);
								LCM_ShowEQMenu(3);
							}
							else
							{
								LCM_ShowEQMenu(tc_Step);
								LCM_ShowEQMenu(gs_DSP_GLOBAL_RAM.sc_EQ_Type);
							}
							Music_EQ_Cmd();
						}
					break;

					case 0x04:	// Prev
						if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_A)
						{
						}
						else if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_B)
						{
							gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_A;
						}
						else if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_AB)
						{
							gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_A;
						}
						else if(gc_PlayMenuItem==0)
						{
							gb_ClearBigIcon=1;
							ClearIRNumberVariable();
							if(gb_SetVol)
							{
								gb_ADJ=1;
								VOL_Adj();
							}
							else
							{
								play_prev();
							}
						}
						else if(gc_PlayMenuItem==1)	// Play Menu(Repeat/EQ/A-B/Exit)
						{
							tc_Step=gc_PlayMenuNum;
							gc_PlayMenuNum=(gc_PlayMenuNum-1)&0x03;
							LCM_ShowPlayMenu(tc_Step);
							LCM_ShowPlayMenu(gc_PlayMenuNum);							
						}
						else if(gc_PlayMenuItem==2)
						{
							tc_Step=gc_RepeatMenuNum;
							gc_RepeatMenuNum=(gc_RepeatMenuNum+5)%6;
							if(gc_RepeatMenuNum==5)
							{
								LCM_ShowRepeatMenu(4);
								LCM_ShowRepeatMenu(5);
								LCM_erase_one_page(4);
								LCM_erase_one_page(5);
								LCM_erase_one_page(6);
								LCM_erase_one_page(7);
							}
							else if(gc_RepeatMenuNum==3)
							{
								LCM_ShowRepeatMenu(0);
								LCM_ShowRepeatMenu(1);
								LCM_ShowRepeatMenu(2);
								LCM_ShowRepeatMenu(3);
							}
							else
							{
								LCM_ShowRepeatMenu(tc_Step);
								LCM_ShowRepeatMenu(gc_RepeatMenuNum);
							}
						}
						else if(gc_PlayMenuItem==3)
						{
							tc_Step=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
							gs_DSP_GLOBAL_RAM.sc_EQ_Type=(gs_DSP_GLOBAL_RAM.sc_EQ_Type+5)%6;
							if(gs_DSP_GLOBAL_RAM.sc_EQ_Type==5)
							{
								LCM_ShowEQMenu(4);
								LCM_ShowEQMenu(5);
								LCM_erase_one_page(4);
								LCM_erase_one_page(5);
								LCM_erase_one_page(6);
								LCM_erase_one_page(7);
							}
							else if(gs_DSP_GLOBAL_RAM.sc_EQ_Type==3)
							{
								LCM_ShowEQMenu(0);
								LCM_ShowEQMenu(1);
								LCM_ShowEQMenu(2);
								LCM_ShowEQMenu(3);
							}
							else
							{
								LCM_ShowEQMenu(tc_Step);
								LCM_ShowEQMenu(gs_DSP_GLOBAL_RAM.sc_EQ_Type);
							}
							Music_EQ_Cmd();
						}
					break;

					case 0x05:	// V
						if(gc_PlayMenuItem==0&&gb_SetVol==0)
						{
							gb_ClearBigIcon=1;
							gb_SetVol=1;				
							LCM_Clear_L2_L5();
							LCM_ShowVOL();
							gc_MenuTimer=6;
							ClearIRNumberVariable();
						}
						else
						{
							gc_MenuTimer=0;
						}
					break;

					case 0x06:	// M
						if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_AB_NULL)
						{
							gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB_NULL;
							LCM_ShowRepeatIcon();	// Clear Repeat Icon
							LCM_ShowSongNum();
							LCM_ShowSongTotalNum();
						}
						else if(gc_PlayMenuItem==0)
						{
							// Enter Play Menu
							gc_PlayMenuItem=1;
							gc_MenuTimer=6;
							ClearIRNumberVariable();
							LCM_ShowPlayMenu(0);
							LCM_ShowPlayMenu(1);
							LCM_ShowPlayMenu(2);
							LCM_ShowPlayMenu(3);
						}
						else if(gc_PlayMenuItem==1)	// ** Play Menu **
						{
							// Enter Repeat-Mode Menu
							gc_RepeatMenuNum=gc_RepPlayMode;
							if(gc_PlayMenuNum==0)	// Repeat Mode
							{
								gc_PlayMenuItem=2;
								if(gc_RepeatMenuNum<4)
								{
									LCM_ShowRepeatMenu(0);
									LCM_ShowRepeatMenu(1);
									LCM_ShowRepeatMenu(2);
									LCM_ShowRepeatMenu(3);
								}
								else
								{
									LCM_ShowRepeatMenu(4);
									LCM_ShowRepeatMenu(5);
									LCM_erase_one_page(4);
									LCM_erase_one_page(5);
									LCM_erase_one_page(6);
									LCM_erase_one_page(7);
								}
							}
							else if(gc_PlayMenuNum==1)	// EQ menu
							{
								gc_PlayMenuItem=3;
								if(gs_DSP_GLOBAL_RAM.sc_EQ_Type<4)
								{
									LCM_ShowEQMenu(0);
									LCM_ShowEQMenu(1);
									LCM_ShowEQMenu(2);
									LCM_ShowEQMenu(3);
								}
								else
								{
									LCM_ShowEQMenu(4);
									LCM_ShowEQMenu(5);
									LCM_erase_one_page(4);
									LCM_erase_one_page(5);
									LCM_erase_one_page(6);
									LCM_erase_one_page(7);
								}
							}
							else if(gc_PlayMenuNum==2)	// A-B
							{
								gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_A;
								gc_MenuTimer=0;
							}
							else	// Exit
							{
								gc_MenuTimer=0;
							}
						}
						else if(gc_PlayMenuItem==2)	// Confirm Repeat mode
						{
							gc_RepPlayMode=gc_RepeatMenuNum;
							gc_MenuTimer=0;
							LCM_ShowPlayModeIcon();
						}
						else if(gc_PlayMenuItem==3)	// Confirm EQ
						{
							gc_MenuTimer=0;
							LCM_ShowEQIcon();
						}
						else	// Exit
						{
							gc_MenuTimer=0;
						}
					break;

					case 0x12:	// L-Play
						gc_Task_Current=C_Task_Suspend;
					break;

					case 0x13:	// L-Next(FF)
						LCM_ShowSongTime();
						UI_fastFFFR_Play(0);
					break;

					case 0x14:	// L-Prev(FR)
						LCM_ShowSongTime();
						UI_fastFFFR_Play(1);
					break;

					case 0x15:	// L-V
						MediaChange();
					break;

					case 0x16:	// L-M
						gc_LastCard=gc_CurrentCard;
						gc_Task_Current=C_Task_Idle;
					break;
				}
			}
			FillAudioDataToDSP();

			if((gs_System_State.c_Phase==TASK_PHASE_PLAYACT)&&(gc_PlayMenuItem==0))
			{
				// 頻譜
				if(gc_PinPuTimer==0)
				{
					LCM_ShowPinPu();
				}

				if(gb_RTC_wakeup==1)	// 1-Sec
				{
					gb_RTC_wakeup=0;
					// Bit-Rate
					if(gc_Play_FileType==0)
					{
						tw_temp=L2_DSP_Read_DMem16(DSP_MP3Bitrate);
					}
					else
					{
						tw_temp=L2_DSP_Read_DMem16(DSP_WMABitrate);
					}
					if(gs_System_State.w_SampleRate!=tw_temp)
					{
						gs_System_State.w_SampleRate=tw_temp;
						LCM_ShowBitRate();
					}

					if(gb_LrcFileName_Exist==1)
					{
						// LRC
						LRC_DisplayFunction();	 							
					}

					if((gc_ShowTimer==0)&&(gc_IR_Timer==0))
					{
						// Play-Time
						LCM_ShowPlayTime(gw_Disp_CurrentSec);
					}
				}
			}

			//---------IR KEY NUMBER PUSH DOWN CHECK--------------------------
			if(gb_IRNumberFlag!=0)
			{
				CheckIRKeyPushDownTimerOut(2);
			}

			//---------DisPlay File Name---------------------------------------
			if((gc_ShowTimer==0)&&(gc_IR_Timer==0)&&(gc_PlayMenuItem==0))
			{
				if(gb_ClearBigIcon==1)
				{
					gb_SetVol=0;
					gb_ClearBigIcon=0;
					gc_LCMScrollTimer=0;
					LCM_Clear_L2_L7();
					LCM_ShowSongTime();
					LCM_ShowPlayTime(gw_Disp_CurrentSec);
				}
				//---- Long File Name Diplay---------------------------------------
				if(gb_LrcFileName_Exist==0)
				{
					if(gc_LCMScrollTimer==0)
					{
						LCM_Disp_FileName(&gc_FileLongName[gw_DispFileName_ByteOffset+5],gc_FileLongName[2],gc_FileLongName[4]-gw_DispFileName_ByteOffset,1);
						gc_LCMScrollTimer=34;
					}
				}
			}
		break;

		case TASK_PHASE_PAUSE:
			switch(gc_KeyEvent)
			{
				case 0x02://C_PlayPause:
					Music_ResumeCmd();
					gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
					LCM_ShowPlayPauseIcon();
					gb_ClearBigIcon=1;
					gc_ShowTimer=0;
					gc_LCMScrollTimer=0;
					//-----LRC Display------------------------------------------
					if(gb_LrcFileName_Exist)
					{
						gb_ClearBigIcon=0;
						if(gc_Play_FileType==0)
						{
							MP3_DataIn();
						}
						else
						{
							WMA_DataIn();	
						}
						LCM_DisplayString(&gc_LrcDisplayBuf,gc_LrcCurrentLen-1,32);
					}
				break;

				case 0x03:	// Next
					if(gc_PlayMenuItem==0)	// 1=Play-Menu  2=Repeat-Mode  3=EQ-Option
					{
						gb_ClearBigIcon=1;
						ClearIRNumberVariable();
						if(gb_SetVol)
						{
							gb_ADJ=0;
							VOL_Adj();
						}
						else
						{						
							play_next();
						}
					}
				break;

				case 0x05:	// V
					if(gc_PlayMenuItem==0&&gb_SetVol==0)
					{
						gb_ClearBigIcon=1;
						gb_SetVol=1;				
						LCM_Clear_L2_L5();
						LCM_ShowVOL();
						ClearIRNumberVariable();
					}
					else
					{
						gc_MenuTimer=0;
					}
				break;

				case 0x12:	// L-Play
					gc_Task_Current=C_Task_Suspend;
				break;

				case 0x16:	// L-M
					gc_Task_Current=C_Task_Idle;
				break;
			}
		break;

		case TASK_PHASE_STOP:
			switch(gc_KeyEvent)
			{
				case 0x02://C_PlayPause:
					gb_Mp3FileEnd=1;
					gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
					LCM_ShowPlayPauseIcon();
					gb_ClearBigIcon=1;
					gc_ShowTimer=0;
					gc_LCMScrollTimer=0;
				break;

				case 0x03:	// Next
					if(gc_PlayMenuItem==0)	// 1=Play-Menu  2=Repeat-Mode  3=EQ-Option
					{
						gb_ClearBigIcon=1;
						ClearIRNumberVariable();
						if(gb_SetVol)
						{
							gb_ADJ=0;
							VOL_Adj();
						}
						else
						{						
							play_next();
						}
					}
				break;

				case 0x05:	// V
					if(gc_PlayMenuItem==0&&gb_SetVol==0)
					{
						gb_ClearBigIcon=1;
						gb_SetVol=1;				
						LCM_Clear_L2_L5();
						LCM_ShowVOL();
						ClearIRNumberVariable();
					}
					else
					{
						gc_MenuTimer=0;
					}
				break;

				case 0x12:	// L-Play
					gc_Task_Current=C_Task_Suspend;
				break;

				case 0x16:	// L-M
					gc_Task_Current=C_Task_Idle;
				break;
			}
		break;
	}
	gc_KeyEvent=0;
}


void Play_Task()
{
	gb_Mp3FileEnd=1;//sunzhk add 0314wq
	gc_CurrentCard=gc_LastCard;
	while(1)
	{
		Key_Detect();
		if(gc_IRCmdStatus!=0)
		{
			IR_Service_Process();
		}

		if(gc_CurrentCard!=9)
		{
			MP3FunctionProc();
		}

		if(gc_CurrentCard!=9)
		{
			OpenAudioFile();
		}

		if(gc_CurrentCard!=9)
		{
			CheckAudioStatus();
		}
		Play_SourceDetect_Process();

		if(gb_LogDataFlag==1)
		{
			gb_LogDataFlag=0;
			USER_LogFile_ReadWrite(1);
		}

		if(gb_ShowBatt==1&&gc_PlayMenuItem==0)
		{
			gb_ShowBatt=0;
			LCM_ShowBattIcon();
		}

		if(gc_CurrentCard==9)
		{
			gc_Task_Current=C_Task_Idle;
		}

		if(gb_OrderRecordTrigger==1)
		{
			gc_Task_Current=C_Task_FM;
		}

		if(gb_AlarmTrigger==1)
		{
			if(CheckAlarmSetting()==0)	// Match Alarm Clock Setting
			{
				gc_Task_Last=C_Task_MusicPlay;
				gc_Task_Current=C_Task_AlarmON;
			}
		}
/*
if(!(XBYTE[0xb409]&0x04))//LineIn insert 
{
	dbprintf("LineIn insert\n");
	gc_Task_Current=C_Task_LineIn;	
}*/
		if(gc_Task_Current!=C_Task_MusicPlay)
		{
			if((gs_System_State.c_Phase==TASK_PHASE_PLAYACT)||(gs_System_State.c_Phase==TASK_PHASE_PAUSE))
			{
				play_stop();
			}
			break;
		}

		if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_AB_NULL)
		{
			if(gc_ShowRepeatTimer==0)
			{
				gc_ShowRepeatTimer=10;
				LCM_ShowRepeatIcon();
			}
		}
	}
}


void L2_DSP_MCU_DM_DMA_24bit(U16 BufferIndex,U8 tc_Dest_Addr_High,U16 tw_Src)
{
	data	U16 tw_DSP_Word_Align;
	xdata	U8  tc_DSPAddr_High;
	code	U16	gw_DSP_Addr_Table[3]={0x0000,0x80aa,0x4155};

	// ----- DMA Reset -----
	XBYTE[0xB304] = 0x09;
	XBYTE[0xB304] = 0x08;
	XBYTE[0xB3C0] = 0x00;		// clear DMA complete
	XBYTE[0xB330] = 0x01;		// clear checksum

	// ----- DMA initial settings for DM transfer -----
	XBYTE[0xB216] = 0x00;	// Remain DSP mode.  When you do MCU -> DM, MCU will stop DSP automaticlly
	XBYTE[0xB301] = 0x50;	// DMA : SRAM --> DM
	XBYTE[0xB302] = 0xFF;	// Define data size in DMA (assume 512 bytes)
	XBYTE[0xB303] = 0x01;

	// Destination
	XBYTE[0xB340]=0x00;	// 24-bit DMA
	tw_DSP_Word_Align=gw_DSP_Addr_Table[BufferIndex%3]+0x200*(BufferIndex/3);
	tc_DSPAddr_High=*((U8 *)&tw_DSP_Word_Align);
	XBYTE[0xB21D]=(U8)(tc_DSPAddr_High>>6);
	tc_DSPAddr_High=*((U8 *)&tw_DSP_Word_Align);
	tc_DSPAddr_High=tc_DSPAddr_High&0x3F;
	tc_DSPAddr_High+=tc_Dest_Addr_High;
	XBYTE[0xB217]=*((U8 *)&tw_DSP_Word_Align+1);
	XBYTE[0xB218]=tc_DSPAddr_High;
	// Source
	XBYTE[0xB112]=(tw_Src<<8)>>8;
	XBYTE[0xB113]=(tw_Src>>8);
	Trigger_Data_Transfer_Done();
}


void JumpToTargetFolder(void)
{
	xdata	U8	temp;

	gb_FindFlag=0;
	gb_LrcFileName_Exist=0;
	// 計算目錄中之歌曲數
	DOS_Search_File(C_File_OneDir|C_Cnt_FileNo,C_MusicFileType,C_CmpExtName|C_Next);
	if(gw_FileTotalNumber[0]==0)
    {
        if(gc_CurrentCard==2) //if SD no file change to Host or flash
        {
            gb_SDNoFileflag=1; 
		    return;
        }
		if(gc_CurrentCard==5) //if Host no file change to SD or flash
        {
            gb_HostNoFileflag=1;  
		    return;
        }
    }

	if(gw_FileIndex[0]==0)
	{
		gw_FileIndex[0]=1;
	}
	gw_FileSkipNumber=gw_FileIndex[0];
	gw_FileIndex[0]=0;
	temp=DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
	if((temp==1)||(gb_HostError==1))
	{
		gb_HostError=0;
		gs_File_FCB[0].dw_FDB_StartCluster=gdw_DOS_RootDirClus;
		DOS_Search_File(C_File_OneDir|C_Cnt_FileNo,C_MusicFileType,C_CmpExtName|C_Next);
		gw_FileSkipNumber=0;
		gw_FileIndex[0]=0;
		if(gw_FileTotalNumber[0]==0)
	    {
	        if(gc_CurrentCard==2) //if SD no file change to Host or flash
	        {
	            gb_SDNoFileflag=1; 
 			    return;
	        }
			if(gc_CurrentCard==5) //if Host no file change to SD or flash
	        {
	            gb_HostNoFileflag=1;  
 			    return;
	        }
	    }
		else
		{
			DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
		}
	}
}

U8 MP3_Repeat_DataIn(void)
{
	data	U8	tc_Cnt;
	data	U8	i;
	data	U16	TimeOUT;

	// Fill bitstream into buffer
	for(i=0;i<10;i++)
	{
		if(L2_DSP_Read_DMem16(DSP_EmptyBuffer)>=512)
		{
			if(DOS_Read_File(0))
			{
				break;
			}
			L2_DSP_MCU_DM_DMA_24bit(gs_DSP_GLOBAL_RAM.sc_DM_Index++,0x20,(U16)gc_PlayRecordDataBuf);
			L2_DSP_SendCommandSet(DCMD_DatIn);

			if(gs_DSP_GLOBAL_RAM.sc_DM_Index>=24)
			{
				gs_DSP_GLOBAL_RAM.sc_DM_Index=0;	// DM Range: 0x2000 ~  0x2FFF
			}
		}
		else
		{
			break;
		}
	}
	// Send STOP command to DSP
	if(DSP_StopCmd()!=0)
	{
		return DSP_STOP_COMMAND_ERROR;
	}

	TimeOUT=0xFFFF;
	do{
		TimeOUT --;
		if(TimeOUT==0)
		{
			return DSP_DECODE_STATUS_TIMEOUT_ERROR;
		}
	}while(!((L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)==0x00C0));  //wait file end

	L2_DSP_Write_DMem16(DSP_DecodeStatus,L2_DSP_Read_DMem16(DSP_DecodeStatus)&0xFF3F);
	L2_DSP_Write_DMem16(DSP_RemainBuffer,0);
	L2_DSP_Write_DMem16(DSP_EmptyBuffer,0x3000);
	gs_DSP_GLOBAL_RAM.sc_DM_Index=0;

	if(gs_DSP_GLOBAL_RAM.sdw_File_ACluster==0xFFFFFFFF)
	{	// recalculate cluster location
		gs_File_FCB[0].dw_File_DataPoint=0;
		gs_File_FCB[0].dw_File_CurrentCluster=gs_File_FCB[0].dw_File_StartCluster;
		gs_DSP_GLOBAL_RAM.sdw_File_ACluster=DOS_Seek_File(0,gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint>>9);		
	}

	// Seek to right point
	gs_File_FCB[0].dw_File_CurrentCluster=gs_DSP_GLOBAL_RAM.sdw_File_ACluster;
	gs_File_FCB[0].dw_File_DataPoint=gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint;

	// restart DSP, Jimi 081125
	for(tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if(DSP_PlayInit()==0)
		{
			break;
		}

		if(tc_Cnt>=5) 
		{
			XBYTE[0xB002]|=0x01;		// DSP reset
		}
		XBYTE[0xB002]&=0xFE;		// Enable DSP Run Normal Mode
	}
	L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sc_Volume);
	// Jimi Yu 081208
	L2_DSP_Write_DMem16(DSP_PostProcessSelect,L2_DSP_Read_DMem16(DSP_PostProcessSelect)|0x0020);
	TimeOUT=0xFFFF;
	while((L2_DSP_Read_DMem16(DSP_PostProcessSelect)&0x0020))	//Handshake with DSP to make sure that DSP has ramp digital volume up.
	{
		TimeOUT--;
		if(TimeOUT==0)
		{
			break;
		}
	}
	Music_EQ_Cmd();
	L2_DSP_Write_DMem16(DSP_DecodeFrameCounter,gw_AFrameCnt);	// Jimi 080530, to Get actual time

	// Send PLAY command to DSP
	if(L2_DSP_SendCommandSet(DCMD_Play)!=DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
	gs_DSP_GLOBAL_RAM.sc_DM_Index=0;	// reset bitstream address of DM
	return	0;
}

U8 WMA_Repeat_DataIn(void)
{
	data	U8	i;
	data	U16	TimeOUT;

	for(i=0;i<10;i++)
	{
		if(L2_DSP_Read_DMem16(DSP_EmptyBuffer)>=3*512)
		{
			if(DOS_Read_File(0))
			{
				break;
			}

			L2_DSP_MCU_DM_DMA_24bit(gs_DSP_GLOBAL_RAM.sc_DM_Index++,0x20,(U16)gc_PlayRecordDataBuf);
			L2_DSP_SendCommandSet(DCMD_DatIn);

			if (gs_DSP_GLOBAL_RAM.sc_DM_Index>=30)
			{
				gs_DSP_GLOBAL_RAM.sc_DM_Index=0;	// DM Range: 0x2000 ~  0x37FF
			}
		}
	}
	// Send Pause command to DSP
	if (DSP_PauseCmd()!=DSP_SUCCESS)
	{
		return DSP_PAUSE_COMMAND_ERROR;
	}
	TimeOUT=0xFFFF;
	do{
		TimeOUT --;
		if(TimeOUT==0)
		{
			return DSP_DECODE_STATUS_TIMEOUT_ERROR;
		}
	} while(!L2_DSP_Read_DMem16(DSP_RampDownComplete)); //Jimi 090417, DSP_RampDownComplete = 0x3FB8, indicates RampDown OK or NOT

	// Seek to right point
	gs_File_FCB[0].dw_File_CurrentCluster=gs_DSP_GLOBAL_RAM.sdw_File_ACluster;
	gs_File_FCB[0].dw_File_DataPoint=gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint;
	
	//WMA Seek trigger and reset BS buffer, and send play cmd
	WMA_seek_trig();
	return	0;
}



