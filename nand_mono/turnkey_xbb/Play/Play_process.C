#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\header\music_play.h"
#include "..\IR\remote.h"
#include "..\IR\LCM_BMP.h"  //ryan
#include "..\header\host_init.h"
#include "..\header\variables.h"

#include "..\..\libsource\header\UI_config.h"
#if (LCM_SIZE==0)
	#include "..\IR\LCM_BMP_128x64_Play.c"
#endif
#if (LCM_SIZE==1)
	#include "..\IR\LCM_BMP_96x32_Play.c"
#endif
#if (LCM_SIZE==2)
    #include "..\IR\lcm_bmp_160x120_play.c"
#endif


extern code U8 CharIcon8x16[];  //0916 //20090107 chiayen add
extern xdata U8 gc_EQChangeFlag; //20090107 chiayen add
extern xdata U8 gc_DIRTimeOut_Flag; //20090107 chiayen add

xdata   U8  gc_PlayModeChange_Flag=0;  //20090107 chiayen add

data 	bit gb_InitLogFileIndex=0;
xdata 	U8  gc_CurrentCard_backup=0;
data 	U8  gc_RepPlayMode_Pre=0;

data 	System_Struct gs_System_State;

xdata 	U8  gcPlay_FileType=0;

extern 	xdata U8 gc_KeyDet_Mask;  //20090107 chiayen modify
extern bit	gb_DirPlay_Flag;  //20090107 chiayen add

extern	data	bit	  gb_HostError;  //20090107 chiayen add
extern 	data 	bit   gb_ChannelSet; 
extern 	xdata	U8	  gc_NextorPreFlag;
extern 	data 	bit   gb_PickSongSet;
extern 	data 	bit   gb_PickSong;
extern 	xdata 	U8    gc_SelectVol;       // 1=Show Vol
extern 	xdata 	U8    gc_SelectEQ;        // 0=Show Song Number  1=Show EQ
extern 	data 	bit   gb_Frequency_Song;  // 0=Show Frequency    1=Show Song Number/EQ
extern  data	U8	  gc_Dirchange_Timer;  //20090121 chiayen add
extern 	data 	U16   gw_IR_Timer;
extern 	data 	U16   gw_FM_frequency;
extern 	xdata 	U16   gw_irkey_count;
extern 	data 	U8	gc_ShowTimer; 
extern 	data 	U16   gw_LogData_Timer;
extern	xdata	U8    gc_CurrentHZK;  //20090107 chiayen add
extern 	xdata 	U8    gc_LogDataFlag;
extern 	xdata 	U8    gc_VolumeMute;
extern 	xdata   U8    gc_RepPlayMode_Dir;  //20090206 chiayen add for IR_DIR
extern 	xdata   U8    gc_RepPlayMode_Nor;  //20090206 chiayen add for IR_DIR
extern  xdata 	U8	  gc_PlayMenu_IR;  //20090206 chiayen add
extern	xdata 	U8 	  gc_IRRepAllFlag;  

 
extern 	U8   Get_LogData_PageIndex(void);
extern 	U8   WMA_ASF_Parser();
extern 	U8   USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);//use reserveblock for log
extern 	U16  MP3_Total_Time_Parser();
extern 	U16  MP3_Bitrate_Parser();
extern 	void FM_drive(void);
extern	void MediaChange(void);  //20090107 chiayen add
extern 	void PickSong_Process(void);
extern 	void FM_FREQ_CHG(U8 tc_UP_DOWN,U8 offset);
extern	void UI_fastFFFR_Play(U8 XFFFR);

extern U8 DOS_DIRtable(void);  //20090216 chiayen add

void Play_SDFlash_DosInit();
void Play_SourceDetect_Process();
void IR_Service_Process();
void Idle_Disp_Icon();

void Music_SetVolumeCmd(void);  //20090420 chiayen add

extern xdata U8 gc_RepPlayMode_Dir;  //20090216 chiayen test

data bit	 gb_Play_firstPlay=0x01;		//Jimi 081125 for EQ mode pop noise as next/prev  //20090420 chiayen add

extern U8	gc_clock_mode;  //20090526 chiayen add

extern xdata	U32	gdw_HOSTStartSectorRead;  //20090803 chiayen add

xdata U8 gc_FmFreqChg_Flag=0;
extern data U16 gw_IR_Setting_Timer;

extern void AS6200_PowerOff();

void Music_EQ_Cmd(U8 tc_Type)
{
    
    DSP_EQ_Cmd(tc_Type);
    gs_DSP_GLOBAL_RAM.sc_EQ_Type = tc_Type;
    DSP_SpectrumOn();
}


void Music_WakeUp(U8 tc_Type)
{   
    DSP_WakeUp(tc_Type);
}


U8 MP3_DataIn(void)
{   
    U8 tc_status;
	U16	tw_SmpRateIdx;

    while (L2_DSP_Read_DMem16(DSP_EmptyBuffer) >= 512)           //Jimi 091126
    {
        tc_status=DOS_Read_File(C_MusicFHandle);
        if(tc_status == DOS_END_OF_FILE)
        {
			L2_DSP_Write_DMem16(DSP_MP3_file_end_flag, 1);  //Jimi 091126
            return DSP_DATAIN_COMMAND_ERROR;
        }
        else if(tc_status)
        {
            return tc_status;   
        }

        L2_DSP_MCU_DM_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index++, 0x20, (U16)gc_PlayRecordDataBuf, DMA_24BIT_MODE);
        L2_DSP_SendCommandSet(DCMD_DatIn);

        if (gs_DSP_GLOBAL_RAM.sc_DM_Index >= 24)
		{
            gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // DM Range: 0x2000 ~  0x2FFF
		}
    }

    tw_SmpRateIdx = L2_DSP_Read_DMem16(0x3F8E);	//0x3F8E is an index of mp3 sampling rate within DSP
                                                                                                                //0x0:44.1k;  0x1:48k;  0x2:32k;  0x3:22.1k;  0x4:24k;  0x5:16k;  0x6:11.025k;  0x7:12k;  0x8:8k
    if(tw_SmpRateIdx == 0 || tw_SmpRateIdx == 3 || tw_SmpRateIdx == 6)
	{
        GLOBAL_REG[0x46] |=0x04;
	}
    else
	{
        GLOBAL_REG[0x46] &=0xFB; 
	}

    return  DSP_SUCCESS;
}


U16 MP3_Bitrate(void)
{
    return(L2_DSP_Read_DMem16(DSP_MP3Bitrate));
}


U16 MP3_ReadTime(void)
{
	code	U16	SampleRate[4][3]={{11025,12000,8000},{0,0,0},{22050,24000,16000},{44100,48000,32000}};
	data	U8 tc_SampleRate,tc_MPEG_Type,tc_Layer;
	data	U16 tw_DurTime;
	data	U16 tw_SamplePerFrame;
	xdata	U32 tdw_Mpeg_Status = L2_DSP_Read_DMem24(DSP_MPEGSt);
	xdata	U32 tdw_FrameCnt = L2_DSP_Read_DMem24(DSP_DecodeFrameCounter);

    tc_SampleRate = ((U8)(tdw_Mpeg_Status>>5)&0x03);
    tc_MPEG_Type = ((U8)(tdw_Mpeg_Status>>(14+8))&0x03);
    tc_Layer = ((U8)(tdw_Mpeg_Status>>(12+8))&0x03);

    if(tdw_FrameCnt>0x20)
    {
        if(tc_Layer == 0x03) //layer-I, reference ISO/IEC 11172-3 header description
		{
            tw_SamplePerFrame = 384;                
    	}
        else if((tc_MPEG_Type != 0x03) && (tc_Layer == 0x01))  
		{
            tw_SamplePerFrame = 576; 
    	}
        else
        {
            tw_SamplePerFrame = 1152;
        }
    
        if(tc_MPEG_Type != 0x01)
		{
            tw_DurTime=gw_CurrentSec+tdw_FrameCnt*tw_SamplePerFrame/SampleRate[tc_MPEG_Type][tc_SampleRate];
    	}
    	else
    	{
        	tw_DurTime = 0;
    	}
    }
    else//(JC)min ~32 frames -> 1sec
    {
        tw_DurTime = gw_CurrentSec;
    }

    return tw_DurTime;
}

U8 MP3_EOF_Proc(void)
{                       
    U16 tw_BufRem;
    U16 tw_RampDownBufSz;
    U16 tw_dec_bsbuf_rem;    //Jimi 091126

   	L2_DSP_Write_DMem16(0x3F44, 8000);	//Jimi 100517, enlarge DSP MP3 decoder ramp down step
	L2_DSP_Write_DMem16(0x3F45, 8000);

    tw_BufRem = L2_DSP_Read_DMem16(DSP_RemainBuffer);
    tw_RampDownBufSz =(U16)(((U32)gs_System_State.w_BitRate * 1000 )/8/20);  // 0.2 sec for ramp down
    tw_dec_bsbuf_rem = L2_DSP_Read_DMem16(DSP_MP3_dec_bsbuf_rem);   //Jimi 091126
    
    if(tw_RampDownBufSz < 512 )     //for 8k bitrate condition(M2L3 & M2.5L3)
	{
        tw_RampDownBufSz = 512;
	}
    
    if(tw_BufRem < tw_RampDownBufSz )
	{
		if(L2_DSP_Read_DMem16(DSP_RampDownComplete) == 0)	//(Jimi 091126)RampDownOK == 0
		{
			if( (tw_dec_bsbuf_rem < tw_RampDownBufSz) || (L2_DSP_Read_DMem16(0x3FB5) == 0) )	//Jimi 100629, 0x3FB5: RampDownFlag
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


//=================WMA=================
U8 WMA_err_bs_proc(void)
{
    data	U16 tw_tmp;
    data	U16 tw_DRMStatus;
    data	U16 tw_DecodeStatus;
	
	tw_DRMStatus = L2_DSP_Read_DMem16(DSP_WMAhasJanusDRM);
	tw_tmp = L2_DSP_Read_DMem16(DSP_DecodeStatus);
	tw_DecodeStatus = tw_tmp & 0x0020;

	if( (tw_DecodeStatus == 0x0020) || tw_DRMStatus  )	//wma format error or drm
	{
        DSP_ResetCmd();
		return DSP_USER_FILE_TYPE_ERROR;
	}
	
	tw_DecodeStatus = tw_tmp & 0x00C0;
    if(tw_DecodeStatus!=0x0000)
	{
		U32 tdw_testcluster;
		U8 tc_iloop;

		tdw_testcluster = (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint >> 9)+5;

		if( tdw_testcluster >= (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize >> 9) )
		{
			return DSP_RUNNING_STATUS1_ERROR;
		}

		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster =gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint = tdw_testcluster << 9;
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = DOS_Seek_File(gs_System_State.c_FileHandle,gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint>>9);

		L2_DSP_Write_DMem16(DSP_EmptyBuffer,0x3C00);	//reset BS buffer information	//Jimi081112 mdf size to 0x3C00
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

			L2_DSP_MCU_DM_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index++, 0x20, (U16)gc_PlayRecordDataBuf, DMA_24BIT_MODE);
		   	L2_DSP_SendCommandSet(DCMD_DatIn); 

		   	tc_iloop ++;
		}while(tc_iloop!=5);

	}
	return DSP_SUCCESS;	
}


U8 WMA_DataIn(void)  //20090803 chiayen modify
{
    U8  tc_errbs;

/* Error Bitstream Process */
    tc_errbs = WMA_err_bs_proc();

    if(tc_errbs == DSP_USER_FILE_TYPE_ERROR || tc_errbs == DSP_RUNNING_STATUS1_ERROR)
    {
        return DSP_DATAIN_COMMAND_ERROR;
    }   

    while (L2_DSP_Read_DMem16(DSP_EmptyBuffer) > 512) //wei.tang modify 081024  //for test play WMA file 081024
    {
        if (DOS_Read_File(C_MusicFHandle) == DOS_END_OF_FILE)
        {
            return DSP_DATAIN_COMMAND_ERROR;
        }

        L2_DSP_MCU_DM_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index++, 0x20, (U16)gc_PlayRecordDataBuf, DMA_24BIT_MODE);
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
    U32 tw_cur_time_h;
    U32 tw_cur_time;

    tw_cur_time_h = L2_DSP_Read_DMem24(DSP_WMACurrenTimeHigh);
    tw_cur_time = L2_DSP_Read_DMem24(DSP_WMACurrenTimeLow) + (tw_cur_time_h<<24);

    return(tw_cur_time / 1000);    
}


U16 WMA_Bitrate(void)
{
    return(L2_DSP_Read_DMem16(DSP_WMABitrate));
}


U8 WMA_EOF_Proc(void)
{    
	data	U8  tc_wma_eof_cnt=0;
	data	U16	tw_DSP_RemainBufPrev=0;
	xdata	U16	tw_bs_buf_remain;
	xdata	U16	tw_DSP_dec_status;
	xdata	U16	tw_RampDownBufSz;
    
    tw_bs_buf_remain = L2_DSP_Read_DMem16(DSP_RemainBuffer);
    tw_RampDownBufSz =(U16)(((U32)gs_System_State.w_BitRate * 1000 )/8/2);  // 0.5 sec for ramp down

    if(tw_bs_buf_remain <= tw_RampDownBufSz)
    {
        return 1;
    }

    if(tw_bs_buf_remain == tw_DSP_RemainBufPrev)
	{
        tc_wma_eof_cnt ++;
	}   
    else
    {
        tw_DSP_RemainBufPrev = tw_bs_buf_remain;
        tc_wma_eof_cnt = 0;
    }    
    tw_DSP_dec_status = L2_DSP_Read_DMem16(DSP_DecodeStatus) & 0xC0;

    if( tw_DSP_dec_status || (tc_wma_eof_cnt >=10) )
    {
        L2_DSP_Write_DMem16(DSP_DecodeStatus, tw_DSP_dec_status|0xC0 );//磷ぇplay_stop hang
        
        return 1;
    }
    
    return 0;   
}


U8 Music_PlayCmd(void)
{
    // sent "PLAY" command to DSP
    if(L2_DSP_SendCommandSet(DCMD_Play) != DCMD_Play)
    {
        return DSP_PLAY_COMMAND_ERROR;
    }

    if(gcPlay_FileType!=1)                  //(Jimi 091028)wma do not need to reset bitstream address cuz the asf parser is contiguous to wma decoder 
    {
        gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // reset bitstream address of DM//(Jimi 091104 mdf)
        // MP3 file open        
    	if(DOS_Open_File_r(C_MusicFHandle, C_Open_FoundFile, C_NullFileName))
		{
        	return DSP_PLAY_COMMAND_ERROR;
		}
	}

    //dbprintf("idx=%x\n",gs_DSP_GLOBAL_RAM.sc_DM_Index);
    return DSP_SUCCESS;
}


void Music_VolumeUpCmd(void)
{
    if (gs_DSP_GLOBAL_RAM.sw_Volume<50)
    {
        gs_DSP_GLOBAL_RAM.sw_Volume+=2;
    }
    L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sw_Volume);     // Vloume Range:0~63
}


void Music_SetVolumeCmd(void)
{
    L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sw_Volume);
}


void Music_VolumeDownCmd(void)
{
    if (gs_DSP_GLOBAL_RAM.sw_Volume>1)
    {
        gs_DSP_GLOBAL_RAM.sw_Volume-=2;
    }
    L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sw_Volume);     // Vloume Range:0~63
}


U8 Music_PauseCmd(void)  //20090803 chiayen modify
{   
    data	U8  tc_Ret;
	data	U16	TimeOUT;

    Music_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);
	tc_Ret=DSP_PauseCmd();
	TimeOUT=0xFFFF;

	if(gcPlay_FileType==0)
	{
		do
		{
			TimeOUT--;
			if (TimeOUT == 0)
			{
				return DSP_RUNNING_STATUS1_ERROR;
			}
		} while (!(L2_DSP_Read_DMem16(DSP_RunningStatus)&0x0002));
	}
	else
	{
		do
		{
			TimeOUT--;
			if (TimeOUT == 0)
			{
				return DSP_RUNNING_STATUS1_ERROR;
			}
		} while (!L2_DSP_Read_DMem16(DSP_RampDownComplete)); //H1226 DSP_RampDownComplete = 0x3FB8, indicates RampDown OK or NOT
	}

	return tc_Ret;
}


U8 Music_ResumeCmd(void)
{
    Music_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);

    if(L2_DSP_SendCommandSet(DCMD_Play) != DCMD_Play)
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

    Music_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);
    tc_Ret = DSP_StopCmd();

    // Reset decode status
	tw_temp=0xffff;
	do
    {
		tw_temp --;
		if(tw_temp==0)
        {
            tc_Ret = DSP_DECODE_STATUS_TIMEOUT_ERROR;
            break;
        }
	}while(!((L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)==0x00C0));  //wait file end

	tw_temp=L2_DSP_Read_DMem16(DSP_DecodeStatus);
	L2_DSP_Write_DMem16(DSP_DecodeStatus,tw_temp&0xFF3F);

    // Close current file
	DOS_Close_File_r(tc_FileHandle);
     
	// wait for ramp down ok and Turn off DSP clock, ycc 081113
	tw_temp=0xFFFF;
	while(1)
	{
   		dbgTmp = L2_DSP_Read_DMem16(DSP_RestartFlag);  //read DSP 0x3F0B.0 ready status
		tw_temp--;     
		if((tw_temp==0)||(dbgTmp==0))
       	{			
       		break; 
	   	} 
	}
	tc_value=XBYTE[0xB010] & 0xFE;
	XBYTE[0xB010] = tc_value;   

    return tc_Ret;
}


void play_stop()
{
    if(gs_System_State.c_Phase != TASK_PHASE_STOP)
    {           
        U8 tc_Ret = Music_StopCmd(gs_System_State.c_FileHandle);
        gs_System_State.c_Phase = TASK_PHASE_STOP;
        gc_PhaseInx=9;
    }
}


void RandomGetFileIndex(void)
{
	data	U16	tw_Num;

    tw_Num=(gw_Random_Timer%gw_FileTotalNumber[gs_System_State.c_FileHandle])+1;
    
    if(gw_FileIndex[gs_System_State.c_FileHandle]==tw_Num)
    {
        gw_FileSkipNumber = 0;
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
    data	U8 tc_playmode_status;

    tc_playmode_status=gc_RepPlayMode;
    if(gb_PickSong==1 && gc_RepPlayMode==C_RandomPlay)
    {
        gc_RepPlayMode=2;
    }

    if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        if(gw_FileIndex[gs_System_State.c_FileHandle]==0)
		{
            return;//if no this line,error will happen
		}
        gc_PhaseInx=9;
        
    }
    else if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
    {
        gc_PhaseInx=3;
        gc_KeyDet_Mask=1;//(JC)H0508 avoid key-press to change phase index  //20090107 chiayen modify  
    }
    else if(gs_System_State.c_Phase==TASK_PHASE_PAUSE)
    {
        gc_PhaseInx=9;
 		       
    }

    if(gs_System_State.c_Phase!=TASK_PHASE_STOP)//(JC)H0505
    {
        U8 tc_Ret = Music_StopCmd(gs_System_State.c_FileHandle);
        gs_System_State.c_Phase=TASK_PHASE_STOP;
    }

    if((gc_RepPlayMode==C_InDirRepeat)||(gc_RepPlayMode==C_InDirPlay))
    {
        DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);  
    }
    else if(gc_RepPlayMode==C_RandomPlay)//Ching 080805
    {
        RandomGetFileIndex();
		if(gb_DirPlay_Flag==1) //20081226 chiayen add
		{
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
			gb_TriggerFileSkip=0;
		}
		else
		{
	        DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
	    }
    }
    else
    {		
		if(gb_DirPlay_Flag==1) //20090107 chiayen add
		{
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
			gb_TriggerFileSkip=0;
		}
	    else
	    {
			DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
	    }
    }

    gs_System_State.c_Phase=TASK_PHASE_STOP;
    if(((gb_PickSong==0) && (gw_irkey_count==0)) && (gb_InitLogFileIndex==0))
    {
		gw_DispSongNum1=0xFFFF;
        LCM_ClearScreen(); 
        gc_ShowTimer=40;  
        LCM_ShowSongNumber(); 
        gb_Frequency_Song=1;
        gc_SelectVol=0;
    }

    gc_RepPlayMode=tc_playmode_status;
    Idle_Disp_Icon();
    gw_CurrentSec=0;
}


void play_prev()
{

    if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        if(gw_FileIndex[gs_System_State.c_FileHandle]==0)
		{
            return;//if no this line,error will happen
		}
        gc_PhaseInx=9;        
    }
    else if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
    {
        gc_PhaseInx=3;
        gc_KeyDet_Mask=1;//(JC)H0508 avoid key-press to change phase index //20090107 chiayen modify     
    }
    else if(gs_System_State.c_Phase==TASK_PHASE_PAUSE)
    {
        gc_PhaseInx=9;        
    }
    
    if(gs_System_State.c_Phase!=TASK_PHASE_STOP)//(JC)H0505
    {
        U8 tc_Ret = Music_StopCmd(gs_System_State.c_FileHandle);
        gs_System_State.c_Phase=TASK_PHASE_STOP;
    }

    if((gc_RepPlayMode==C_InDirRepeat)||(gc_RepPlayMode==C_InDirPlay))
    {
        DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Prev);
    }
    else if(gc_RepPlayMode==C_RandomPlay)//Ching 080805
    {
        RandomGetFileIndex();
		if(gb_DirPlay_Flag==1) //20090107 chiayen add
		{
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
			gb_TriggerFileSkip=0;
		}
		else
		{
	        DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
	    }
    }
    else
    {
		if(gb_DirPlay_Flag==1) //20090107 chiayen add 
		{
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Prev);
			gb_TriggerFileSkip=0;
		}
	    else
	    {
	        DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType,C_CmpExtName|C_Prev);
	    }
    }

    gs_System_State.c_Phase=TASK_PHASE_STOP;

    if((gb_PickSong==0) && (gw_irkey_count==0))
    {
		gw_DispSongNum1=0xFFFF;
        LCM_ClearScreen(); 
        gc_ShowTimer=40; 
        LCM_ShowSongNumber();
        gb_Frequency_Song=1;
        gc_SelectVol=0;
    }
    Idle_Disp_Icon();
    gw_CurrentSec=0;
}

//===============  UI flow  =================
void Idle_Disp_Icon()
{   
    gcPlay_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;

    if(gcPlay_FileType==0)
    {
        gs_System_State.w_BitRate=MP3_Bitrate_Parser();
        gw_TotalSec = MP3_Total_Time_Parser();//(JC)H0630 LCM test
    }
    else if(gcPlay_FileType==1)
    {
        WMA_ASF_Parser();
    }

    DOS_GetLongFileName(0, gc_FileLongName);
}


void play_idle()
{
    
}

xdata U8 gc_playint2Plause=0;//0 进到int后再进playpause，1 pause状态进到playpause函数
void play_init()
{  
    if(gw_init_needed & SET_BIT0)
    {
        gw_init_needed&=CLR_BIT0;      
        gs_System_State.c_FileHandle=0;
        gs_System_State.c_Phase = TASK_PHASE_STOP;
        gs_DSP_GLOBAL_RAM.sw_Volume = 20;//(JC)init value
        gs_DSP_GLOBAL_RAM.sc_EQ_Type=0;//(JC)init value
        gb_LrcFileName_Exist=0;//(JC)H0702
        gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // reset bitstream address of DM
        gs_DSP_GLOBAL_RAM.sdw_File_ACluster = 0;
        gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint = 0;
        gs_DSP_GLOBAL_RAM.sdw_File_BCluster = 0;
        gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint = 0;
        gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode = REPEAT_AB_NULL;
        gw_CurrentSec = 0;
		gc_playint2Plause=0;	//--sunzhk add 100503
        //(JC)search music file overall by FDB, findout 1st one in ROOT

		if(gb_DirPlay_Flag==1)
		{
			U16 tw_FileIndex=0;
	        Get_LogData_PageIndex();  //chiayen0808
	        USER_LogFile_ReadWrite(0);//use reserveblock for log

			gb_DirPlay_Flag=0;
            gc_RepPlayMode_Pre=gc_RepPlayMode;
            gc_RepPlayMode=2;
            gb_InitLogFileIndex=1;
			tw_FileIndex=gw_FileIndex[0];
            gw_FileSkipNumber=gw_FileIndex[0];
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);
			if(DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next))
			{
				gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
				DOS_Search_File(C_File_OneDir|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);
				gw_FileSkipNumber=0;
				gw_FileIndex[0]=0;		
		        if(DOS_Search_File(C_File_All|C_By_Time, C_MusicFileType, C_CmpExtName|C_Next))
		        {
		            if(gc_CurrentCard==2) //if SD no file change to Host or flash
		            {
		                gb_SDNoFileflag=1;  
		            }
					if(gc_CurrentCard==5) //if Host no file change to SD or flash
		            {
		                gb_HostNoFileflag=1;  
		            }
					gb_FlashNoFileflag=0;
		            gw_init_needed |= SET_BIT8;
		            gc_PhaseInx = 9; dbprintf("1\n");
		
		            return;
		        }				
			}
			else
			{
				gw_FileIndex[0]=tw_FileIndex;
			}
			gb_TriggerFileSkip=0;
            gc_RepPlayMode=gc_RepPlayMode_Pre;
            gb_InitLogFileIndex=0;			
			gb_DirPlay_Flag=1;dbprintf("2\n");
		}
		else
		{
			gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
	        if(DOS_Search_File(C_File_All|C_By_Time, C_MusicFileType, C_CmpExtName|C_Next))
	        {
	            if(gc_CurrentCard==2) //if SD no file change to Host or flash
	            {
	                gb_SDNoFileflag=1;  
	            }
				if(gc_CurrentCard==5) //if Host no file change to SD or flash
	            {
	                gb_HostNoFileflag=1;  
	            }
				gb_FlashNoFileflag=0;
	            gw_init_needed |= SET_BIT8;
	            gc_PhaseInx = 9;dbprintf("3\n");
	
	            return;
	        }
		}

        gcPlay_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;

        if(gcPlay_FileType==0)//(JC)MP3
        {
            set_clock_mode(CLOCK_MODE_MP3);
        }
        else if((gcPlay_FileType==1) || (gcPlay_FileType==2))//(JC)WMA //Jimi: gcPlay_FileType==2 ==>WMV
        {
            set_clock_mode(CLOCK_MODE_WMA);
        }

        if(gcPlay_FileType==0)
        {
            gs_System_State.w_BitRate=MP3_Bitrate_Parser();
            gw_TotalSec = MP3_Total_Time_Parser();//(JC)H0630 LCM test
        }
        else if(gcPlay_FileType==1)
        {
            WMA_ASF_Parser();
        }
	
		if(gb_DirPlay_Flag==0)
		{
	        Get_LogData_PageIndex();  //chiayen0808
	        USER_LogFile_ReadWrite(0);//use reserveblock for log
		}

        if((gw_FM_frequency<875) || (gw_FM_frequency>1080))
        {
            gw_FM_frequency=875;
        }

        if((gs_DSP_GLOBAL_RAM.sw_Volume<0) || (gs_DSP_GLOBAL_RAM.sw_Volume>50))
        {
            gs_DSP_GLOBAL_RAM.sw_Volume=20; 
        }

        if((gs_DSP_GLOBAL_RAM.sc_EQ_Type<0) || (gs_DSP_GLOBAL_RAM.sc_EQ_Type>5))
        {
            gs_DSP_GLOBAL_RAM.sc_EQ_Type=0; 
        }

		if(gc_language_offset > 17)
		{
			gc_language_offset=HZK_LANGUAGE;
			gc_CurrentHZK=gc_CurrentHZK=LanguageTable[gc_language_offset];	
		}

        if((gc_RepPlayMode<1) || (gc_RepPlayMode>3))
        {
            gc_RepPlayMode=2;
        }

        if((gc_RepPlayMode_Dir<1) || (gc_RepPlayMode_Dir>3))  //20090216 chiayen add
        {
            gc_RepPlayMode_Dir=2;
        }

		if(gc_IRRepAllFlag==1)
		{
			gc_RepPlayMode=2;
			gc_RepPlayMode_Nor=2;
			gc_IRRepAllFlag=0;
		}

		if(gb_DirPlay_Flag==0)
		{
			gw_FileIndex[0]=1;dbprintf("4\n");
	        if(gc_CurrentCard==0)
	        { 
	            if(gw_LogFileIndex<=gw_FileTotalNumber[0])
	            {
	                if(gw_LogFileIndex==0)
	                {
	                    gw_LogFileIndex=1;  
	                }
	                if(gw_LogFileIndex!=gw_FileIndex[0])
	                {
	                    gc_RepPlayMode_Pre=gc_RepPlayMode;
	                    gc_RepPlayMode=2;
	                    gb_InitLogFileIndex=1;
	                    gw_FileSkipNumber=gw_LogFileIndex-1;
	                    play_next();
	                    gc_RepPlayMode=gc_RepPlayMode_Pre;
	                    gb_InitLogFileIndex=0;
	                }
	            }
	        }
	        else if(gc_CurrentCard==2)
	        {
	            if(gw_SDLogFileIndex==0)
	            {
	                gw_SDLogFileIndex=1;    
	            }
	            if((gw_SDLogFileIndex<=gw_FileTotalNumber[0]))
	            {
	                if(gw_SDLogFileIndex!=gw_FileIndex[0])
	                {
	                    gc_RepPlayMode_Pre=gc_RepPlayMode;
	                    gc_RepPlayMode=2;
	                    gb_InitLogFileIndex=1;
	                    gw_FileSkipNumber=gw_SDLogFileIndex-1;
	                    play_next();
	                    gc_RepPlayMode=gc_RepPlayMode_Pre;
	                    gb_InitLogFileIndex=0;
	                }
	            }
	        }
	        else if(gc_CurrentCard==5)
	        {
	            if(gw_USBLogFileIndex==0)
	            {
	                gw_USBLogFileIndex=1;    
	            }
	            if((gw_USBLogFileIndex<=gw_FileTotalNumber[0]))
	            {
	                if(gw_USBLogFileIndex!=gw_FileIndex[0])
	                {
	                    gc_RepPlayMode_Pre=gc_RepPlayMode;
	                    gc_RepPlayMode=2;
	                    gb_InitLogFileIndex=1;
	                    gw_FileSkipNumber=gw_USBLogFileIndex-1;
	                    play_next();
	                    gc_RepPlayMode=gc_RepPlayMode_Pre;
	                    gb_InitLogFileIndex=0;
	                }
	            }
	        }
		}//gb_DirPlay_Flag==0
		else
		{
		}
	} 
	gc_DIRTimeOut_Flag=0;
    DOS_GetLongFileName(0,gc_FileLongName);         
    LCM_ShowMediaType();
	gw_DispSongNum1=0xFFFF;
    gc_PhaseInx=3;
}


void play_proc()
{
    U8 tc_p, tc_settime_status;

    gc_KeyDet_Mask=0;  //20090107 chiayen modify

    if ( (gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_B) || (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint<=gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint) )
    {   // normal play
        tc_settime_status=play_SetTime2DisplayBuf((*Music_ReadTime[gcPlay_FileType])());//(JC)H0711
        
        if(gb_LrcFileName_Exist)
        {
            if(!tc_settime_status)
            {
                Lyric_GetTimeStamp();
                if(gb_LrcGetTimeOrNot)
                {
                    gb_LrcGetTimeOrNot=0;
					
                    Lyric_GetLyrics();
                    //(JC)sim LCM Display by UART
                    for(tc_p=0; tc_p<gc_LrcCurrentLen; tc_p++)
                    {
                        if(gc_LrcCurrentLen==1&&gc_LrcDisplayBuf[tc_p]==0x0d)
                    {
                            break;
                        }
                        if(gc_Num_Flag==0 && gc_ShowTimer==0 && gb_PickSong==0 && gb_ChannelSet==0  && gs_System_State.c_Phase == TASK_PHASE_PLAYACT)  //ryantest
                        {
                            #if(LCM_SIZE==0)||(LCM_SIZE==2)
                            LRC_LCMDisplayString(&gc_LrcDisplayBuf,gc_LrcCurrentLen-1,16);
							//===sunzhk add 100428
							LCM_erase_one_page(6);
							LCM_erase_one_page(7);
                            #endif
							#if(LCM_SIZE==1)
							LRC_LCMDisplayString(&gc_LrcDisplayBuf,gc_LrcCurrentLen-1,0);
                            #endif
                        }
                    }
                    //(JC)sim LCM Display by UART
                }
            }
        }
       
        if ((*Music_Data_In[gcPlay_FileType])())// == DSP_DATAIN_COMMAND_ERROR)
        {
            gc_PhaseInx = 2;    //End of File for play
            return;//(JC)H0620
        }
        
        if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_B)
        {
            if(gc_RepPlayMode==4)
            {
                if((*Music_ReadTime[gcPlay_FileType])()>10)
                {
                    gc_PhaseInx = 2;
                    return;
                }
            }
        }
    }

	if(gc_EQChangeFlag==1) //20090106 chiayen add
	{
		Music_EQ_Cmd(gs_DSP_GLOBAL_RAM.sc_EQ_Type);  
		gc_EQChangeFlag=0;
	} 
}


void play_playpause()
{ 
	U32 tdw_File_StartCluster;  //20090106 chiayen add
	U32 tdw_FDB_LogAdd;  //20090106 chiayen add
 
	gc_PlayMenu_IR=0;

	#if BankingCode_Recover
    if(gbt_Code_NeedRecover)
	{
		U8 tc_Reg_Status,tc_CE;
//		dbprintf("code recover \n");
		tc_Reg_Status =DEVICE_REG[0x00];
		DEVICE_REG[0x00]= 0x01; 
		tc_CE= FDBP.cFDev;
		for(gc_Global_I =0; gc_Global_I<gc_CodeBlock_Num;gc_Global_I++)
		{
			gw_SourceBlock = Bank_Block_A[gc_Global_I];
			gw_TargetBlock = Bank_Block_B[gc_Global_I];
			FDBP.cFDev=0;
			FDBP.cFPlane=0;
			FDBP.wFBlock = gw_TargetBlock;
			Flash_EraseOneBlock();			
			FLASH_Backup_Blcok(0,NAND_INFO.wPAGE_NUM,4,0);// 4-->bank  mark 
		}
		gbt_Code_NeedRecover=0;
		DEVICE_REG[0x00]=tc_Reg_Status ;
		FDBP.cFDev = tc_CE;
	}
	#endif

    if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_AB_NULL)
    {
        gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB_NULL;
        gc_PhaseInx=1;
        return;
    }
    if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
    {
        Music_PauseCmd();
        gs_System_State.c_Phase = TASK_PHASE_PAUSE;
    }
    else if(gs_System_State.c_Phase == TASK_PHASE_PAUSE)
    {
        Music_ResumeCmd();
		#if(LCM_SIZE==1)
		if(gc_LrcFileName_Exist)
		{
			gc_LRCLCMClear_Flag=1;
		}
		else
		{
			gc_Dirchange_Timer =0;
			gw_LCMScrollTimer=0;
		}
		#endif
        gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
    }
    else if ((gs_System_State.c_Phase == TASK_PHASE_FASTFWD)||(gs_System_State.c_Phase==TASK_PHASE_FASTREV))
    {
        gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // reset bitstream address of DM
        gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
    }
    else
    {
        gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
        DOS_GetLongFileName(0,gc_FileLongName);
        gb_LrcFileName_Exist=Lyric_FileSearch();//(JC)Lyric file exist?
        gcPlay_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;
        
        if(gcPlay_FileType==0)//(JC)MP3
        {
          
			{
            	set_clock_mode(CLOCK_MODE_MP3);
			}
            gs_System_State.w_BitRate=MP3_Bitrate_Parser();
            gw_TotalSec = MP3_Total_Time_Parser();//(JC)H0630 LCM test
	        gs_DSP_GLOBAL_RAM.sc_DM_Index=0;  // reset bitstream address of DM

        }
        else
        {
            set_clock_mode(CLOCK_MODE_WMA);
			WMA_ASF_Parser();
        }

        Music_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);
        
        if((*Music_Download[gcPlay_FileType])())    // 材Ω磅︽璶更 IM, PM    //Jimi Yu 080509
        {
            gc_PhaseInx=2;                          // when wma 
            return; 
        }

 //       gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // reset bitstream address of DM
        gs_DSP_GLOBAL_RAM.sdw_File_ACluster = 0;
        gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint = 0;
        gs_DSP_GLOBAL_RAM.sdw_File_BCluster = 0;
        gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint = 0;
        gw_CurrentSec = 0;
		gw_Disp_CurrentSec=0;

        Music_SetVolumeCmd();

		//Jimi 081208
		L2_DSP_Write_DMem16(DSP_PostProcessSelect, L2_DSP_Read_DMem16(DSP_PostProcessSelect)|0x0020);
		{
			U8 tc_timeout = 0xFF;
			while( (L2_DSP_Read_DMem16(DSP_PostProcessSelect)&0x0020))	//Handshake with DSP to make sure that DSP has ramp digital volume up.
			{
				if (!(tc_timeout--))
					break;
			}
		}

        Music_EQ_Cmd(gs_DSP_GLOBAL_RAM.sc_EQ_Type);

		if(gb_DirPlay_Flag==1)  //20090106 chiayen add
        {
			tdw_File_StartCluster=gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
			tdw_FDB_LogAdd=gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_LogAdd;
			DOS_Search_File(C_File_OneDir|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);
			gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster=tdw_File_StartCluster;
			gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_LogAdd=tdw_FDB_LogAdd;
		}
//==========================================================

//==========================================================
		gw_DispFileName_ByteOffset=0;  //20090717 chiayen add for rolling file name

        if(Music_PlayCmd()==DSP_PLAY_COMMAND_ERROR)
        {
            //(JC)H0521; pls check if error handle needed
            gc_PhaseInx=5;  //20081219 chiayen modify for 0Byte mp3
            return;
        }
		if(gcPlay_FileType==0)//(JC)MP3
        {
		  if(gb_LrcFileName_Exist==1)
			{
				set_clock_mode(CLOCK_MODE_MP3L);
			}
			else
			{
            	set_clock_mode(CLOCK_MODE_MP3);

			}
		}
        gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode = REPEAT_AB_NULL; // –Ω穝冀Ρヘ常璶р RepeatAB 竚
    }
    gc_PhaseInx=1;
}


void play_endfile()
{
    U32 tdw_FDB_LogAddr;
    
    play_SetTime2DisplayBuf((*Music_ReadTime[gcPlay_FileType])());//Jimi 080804

	(*Music_EOF_Proc[gcPlay_FileType])();	// 0601
//    if( (*Music_EOF_Proc[gcPlay_FileType])() ) //Jimi 080522
    {
        play_stop();
        gw_CurrentSec=0;
        //============== test cyclic ==============
        if(gc_RepPlayMode==C_NoRepeat)//(JC)no repeat
        {
            gc_PhaseInx=9;  
        }
        else if(gc_RepPlayMode==C_RepeatOne)//(JC)repeat one
        {
            gc_PhaseInx=3;      
        }
        else if(gc_RepPlayMode==C_RepeatAll||gc_RepPlayMode==C_IntroPlay)//(JC)repeat all || introduction
        {
			if(gb_DirPlay_Flag==1) //20090107 chiayen add
			{
				DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
			}
			else
			{
            	DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType,C_CmpExtName|C_Next);
			}
            gc_PhaseInx=3;
        }
        else if(gc_RepPlayMode==C_RandomPlay)//(JC)random
        {
            RandomGetFileIndex();
			if(gb_DirPlay_Flag==1) //20081226 chiayen add
			{
				gb_TriggerFileSkip=1;
				DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
				gb_TriggerFileSkip=0;
			}
			else
			{
				DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
			}
            gc_PhaseInx=3;
        }
        else if(gc_RepPlayMode==C_InDirRepeat)//(JC)repeat in a dir
        {
            DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
            gc_PhaseInx=3;
        }
        else if(gc_RepPlayMode==C_InDirPlay)
        {
            tdw_FDB_LogAddr=gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_LogAdd;
            DOS_Search_File(C_File_OneDir|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);
            gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_LogAdd=tdw_FDB_LogAddr;
            if(gw_FileIndex[gs_System_State.c_FileHandle]==gw_FileTotalNumber[gs_System_State.c_FileHandle])
            {
                gc_PhaseInx=1;  
            }
            else
            {
                DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
                gc_PhaseInx=3;
            }   
        }
    }
}


void play_volup()
{
    gw_LogData_Timer=60; 
    gc_ShowTimer=72;       
    gb_Frequency_Song=1;    // 0=Show Frequency    1=Show Song Number/EQ
    gc_SelectVol=1;         // 1=Show Vol
    gc_SelectEQ=0;          // 0=Show Song Number  1=Show EQ
    gc_VolumeMute=0;

    Music_VolumeUpCmd();
    gc_PhaseInx=1;
}


void play_voldn()
{
    gw_LogData_Timer=60;
    gc_ShowTimer=72;
    gb_Frequency_Song=1;    // 0=Show Frequency    1=Show Song Number/EQ
    gc_SelectVol=1;         // 1=Show Vol
    gc_SelectEQ=0;          // 0=Show Song Number  1=Show EQ
    gc_VolumeMute=0;

    Music_VolumeDownCmd();
    gc_PhaseInx=1;  
}


void play_back2uplevel()
{
    if((gs_System_State.c_Phase==TASK_PHASE_PLAYACT)||(gs_System_State.c_Phase==TASK_PHASE_PAUSE))
    {
        play_stop();    
    }
    gc_Task_Next=C_Task_Menu;
    gw_init_needed |= SET_BIT8;
    gc_PhaseInx=0;
}


U8 play_SetTime2DisplayBuf(U16 tw_TimeSec)
{
    if(gw_PrevTimeSec != tw_TimeSec)
    {
        gw_PrevTimeSec=tw_TimeSec;
        gc_DispTimeBuf[0] = (U8)(tw_TimeSec/3600);//(JC)Hours
        gc_DispTimeBuf[1] = (U8)((tw_TimeSec-gc_DispTimeBuf[0]*3600)/60);//(JC)Mins
        gc_DispTimeBuf[2] = (U8)(tw_TimeSec%60);//(JC)Sec
        return 0;
    }
    return 1;
}


WORD play_GetTimeFromDisplayBuf()
{
    return ((((U16)gc_DispTimeBuf[0])*3600)+(((U16)gc_DispTimeBuf[1])*60)+gc_DispTimeBuf[2]);
}


void FM_FREQ_CHGUP(void) //chiayen0807
{
    FM_FREQ_CHG(1,1);
    if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        gc_PhaseInx=0x09;   
    }
    else
    {
        gc_PhaseInx=1;
    }
}


void FM_FREQ_CHGDOWN(void) //chiayen0807
{
    FM_FREQ_CHG(0,1);
    if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        gc_PhaseInx=0x09;   
    }
    else
    {
        gc_PhaseInx=1;
    }
}


void IR_Service_Process()
{
    if(gc_IRCmdStatus!=0)
    {
		if(gc_IRCmdStatus==1)  //20090216 chiayen add
		{
			gc_Dirchange_Timer=0;
		}
        ir_service();
    }

	if((gw_IR_Timer==0) && (gw_irkey_count!=0))
	{
		gw_irkey_count=0;
		if(gc_NextorPreFlag==0)
		{
			play_next();
		}
		else
		{
			play_prev();
		}
		gc_LCM_Media=0xFF;		
	}	
}


void Play_SDFlash_DosInit()  //20090803 chiayen modify
{
	U8  tc_clock_mode_backup;  //20090817 chiayen mark
	tc_clock_mode_backup=gc_clock_mode; //20090817 chiayen mark
	set_clock_mode(CLOCK_MODE_DOS);  //20090803 chiayen mark

	if(gc_CurrentCard!=5)
	{
		gc_HostSectorUnit=1;
	}

	if (DOS_Initialize())
	{
//		dbprintf("Nand DOS init fail11!!\n");
		gc_PhaseInx=9;
		gc_Dosinitfail=1;
		gdw_HOSTStartSectorRead=0xFFFFFFF0; //20090803 chiayen add
	}
	else
	{
		set_clock_mode(CLOCK_MODE_DOS);  //20090817 chiayen mark
		gb_FindFlag = 0;
		gc_PhaseInx=0;
		gw_init_needed=0xFFFF;
		DOS_DIRtable();  //20090817 chiayen mark 

		gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;           
		DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);//(JC)count music file no. in root
		gc_PhaseInx=0;
//		dbprintf("Nand DOS init ok!!\n");
	}
	set_clock_mode(tc_clock_mode_backup);  //20090817 chiayen mark
}


void Play_SourceDetect_Process()
{
	gb_SD_Exist_pre=gb_SD_Exist;
	if(!SD_Detect)  //SD exist
    {
        gb_SD_Exist=1;
    }
	else
	{
		gb_SD_Exist=0;
		gb_SDNoFileflag=0;
	}

	gb_Host_Exist_pre=gb_Host_Exist;
	if(!Host_DetectDevice())
	{
		gb_Host_Exist=1;	//host_exist		
	}
	else
	{
		gb_Host_Exist=0;
		gb_HostNoFileflag=0;
	}

	if((gc_CurrentCard==0) && ((gb_Host_Exist_pre!=gb_Host_Exist)||(gb_SD_Exist_pre!=gb_SD_Exist)))
	{
		if((gb_Host_Exist_pre!=gb_Host_Exist) && (gb_Host_Exist==1))
		{
			gc_CurrentCard=5;
		}
		else if((gb_SD_Exist_pre!=gb_SD_Exist) && (gb_SD_Exist==1))
		{
			gc_CurrentCard=2;	
		}
		else
		{
			gc_CurrentCard=0;	
		}
	}
	else
	{
		if(gc_CurrentCard==2)
		{
			if(gc_Dosinitfail==0)  //20090803 chiayen add
			{
				if((gb_Host_Exist_pre!=gb_Host_Exist) && (gb_Host_Exist==1))
				{
					gc_CurrentCard=5;
				}
				else if(((gb_SD_Exist==0) || (gb_SDNoFileflag==1)) && (gb_Host_Exist==1))
				{
					gc_CurrentCard=5;
					if(gb_HostNoFileflag==1)
					{
						gc_CurrentCard=0;	
					}
				}
				else if((gb_SD_Exist==0) || (gb_SDNoFileflag==1))
				{
					gc_CurrentCard=0;	
				}
			}
			else //20090803 chiayen add for SD Dos initial fail
			{
				if((gb_Host_Exist==1) && (gb_HostNoFileflag==0))
				{
					gc_CurrentCard=5;
				}
				else
				{
					gc_CurrentCard=0;
				}
				gc_Dosinitfail=0;				
			}
		}

		if(gc_CurrentCard==5)
		{
			if(gc_Dosinitfail==0)  //20090803 chiayen add
			{
				if((gb_SD_Exist_pre!=gb_SD_Exist) && (gb_SD_Exist==1))
				{
					gc_CurrentCard=2;
				}
				else if(((gb_Host_Exist==0) || (gb_HostNoFileflag==1)) && (gb_SD_Exist==1))
				{
					gc_CurrentCard=2;
					if(gb_SDNoFileflag==1)
					{
						gc_CurrentCard=0;	
					}
				}
				else if((gb_Host_Exist==0) || (gb_HostNoFileflag==1))
				{
					gc_CurrentCard=0;	
				}
			}
			else //host dos initial fail  //20090803 chiayen add
			{
				if((gb_SD_Exist==1) && (gb_SDNoFileflag==0))
				{
					gc_CurrentCard=2;
				}
				else
				{
					gc_CurrentCard=0;
				}
				gc_Dosinitfail=0;
			}
		}
	}

    if(gc_CurrentCard_backup!=gc_CurrentCard)  //auto  //20090803 chiayen modify
    {
		U8 tc_CurrentCard;//20090803 chiayen add for media change
		tc_CurrentCard=gc_CurrentCard; //20090803 chiayen add for media change		

        #if(LCM_SIZE==0)||(LCM_SIZE==2)    // 128x64
        LCM_ClearScreen();
		LCM_ShowPlayTime(0);
        #endif
		#if(LCM_SIZE==1)
		{
			LCM_ClearScreen();
			gw_DisplayTime=0xFFFF;
			gw_DisplayFreq1=0xFFFF;
			gc_DisplayEQIcon=0xFF;
		}
		#endif

		gc_CurrentCard=gc_CurrentCard_backup;  //20090803 chiayen add for media change
		play_stop();
		gc_CurrentCard=tc_CurrentCard; //20090803 chiayen add for media change
		LCM_ShowWait();
		set_clock_mode(CLOCK_MODE_MP3); //20090803 chiayen add for media change
        if(gc_CurrentCard==2)
        {           
            if(SD_Identification_Flow())
            {
                gc_CardExist |=0x02;
            }
            else if((gb_Host_Exist==1) && (gb_HostNoFileflag==0)) //20090730 chiayen modify
            {
				if(!Host_Initial())
				{
					gc_CurrentCard = CURRENT_MEDIA_HOST;
					gb_FindFlag = 0;
	            }
	            else
	            {
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;
					InitFlash();					    
            	}
        	}
			else 
        	{
				gc_CurrentCard=0;
				gc_CardExist &=0xFD;
				gb_FindFlag = 0;
				InitFlash();
			}
			Play_SDFlash_DosInit();
        }
		else if(gc_CurrentCard==5)
		{
			if(!Host_Initial())
			{
				gc_CurrentCard = 5;
				gb_FindFlag = 0;
			}
			else if((gb_SD_Exist==1) && (gb_SDNoFileflag==0)) //20090730 chiayen add
			{
	            if(SD_Identification_Flow())
	            { 
	                gc_CardExist |=0x02;
					gc_CurrentCard=2;      
				}
				else
				{
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;
					InitFlash();
				}
			}
			else 
        	{
				gc_CurrentCard=0;
				gb_FindFlag = 0;
				InitFlash();
			}
			Play_SDFlash_DosInit();
		}
		else if(gc_CurrentCard==0)
        {
			gc_CardExist &=0xFD;
			InitFlash();
			Play_SDFlash_DosInit();
		}
    }	
}

void MediaChange_Play(void)  //20090803 chiayen add
{
	if(gc_Dosinitfail==0)
	{
		if((gb_SD_Exist==1) && (gb_SDNoFileflag==0))
		{
			gc_CurrentCard=2;	
		}
		else
		{
			gc_CurrentCard=0;
		}
	}
}
//=======================================
extern void TimeOutHandle();
void Play_Task()
{
    while(1)
    {
        //dbprintf("gw_IR_Timer=%x\n",gw_IR_Timer);
        //dbprintf("Phase= %bx\n", gc_PhaseInx);
		switch(gc_PhaseInx)
        {
            case C_PlayInit:
                play_init();
            break;

            case C_PlayProc:
                play_proc();
            break;
            
            case C_PlayEndfile:
                play_endfile();
            break;

            case C_PlayPause:
				if(!gb_FlashNoFileflag)
				{
	                play_playpause();
					if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT && gb_LrcFileName_Exist && gc_playint2Plause==1 )	//--sunzhk add 100503
					{
					 	LCM_erase_one_page(2);
						LCM_erase_one_page(3);
						LCM_erase_one_page(4);
						LCM_erase_one_page(5);
						LCM_erase_one_page(6);
						LCM_erase_one_page(7);
					}
					gc_playint2Plause = 1;	   	//--sunzhk add 100503
					gc_FmFreqChg_Flag=0;
				}
				else
				{
					gc_PhaseInx=9;	
				}
            break;

            case C_PlayStop:
                play_stop();    
					gc_playint2Plause = 0; 		//--sunzhk add 100503
            break;

            case C_PlayNext:
				if(!gb_FlashNoFileflag)
				{
	                play_next();
				}
            break;

            case C_PlayPrev:
				if(!gb_FlashNoFileflag)
				{
	                play_prev();
				}
            break;

            case C_PlayFF:
				UI_fastFFFR_Play(0);
            break;

            case C_PlayFR:
				UI_fastFFFR_Play(1);
            break;

            case C_PlayFfFrEnd:
				play_fffr_end();
            break;

            case C_PlayIdle:
                play_idle();
            break;

            case C_PlaySetRepAB:

            break;

            case C_PlayVolUp:
                play_volup();
            break;

            case C_PlayVolDn:
                play_voldn();
            break;

            case C_PlayUpLevel:
                play_back2uplevel();
            break;

            case C_PlayMenu:

            break;

            case C_TuneVol:

            break;

            case C_FmFreqChgUp:
                FM_FREQ_CHGUP();
            break;

            case C_FmFreqChgDown:
                FM_FREQ_CHGDOWN();
            break;

			case C_FmFreqChg:
			    if(gc_FmFreqChg_Flag==0)
				{
				    FM_FREQ_CHGUP();
	                gc_FmFreqChg_Flag=1;
					gb_Frequency_Song=0;	// 0=Show Frequency    1=Show Song Number/EQ
					gw_DispSongNum=0xFFFF;
					gc_ShowTimer=72;
					gw_IR_Setting_Timer=120;
					gw_DisplayFreq=0xffff;
					if(gs_System_State.c_Phase==TASK_PHASE_STOP)
				    {
				        gc_PhaseInx=0x09;   
				    }
				    else
				    {
				        gc_PhaseInx=1;
				    }
				  }
				  else  if(gc_FmFreqChg_Flag==1)
				  {
				     FM_FREQ_CHGUP();
					 gc_ShowTimer=72;
					 gw_IR_Setting_Timer=120;
				  }
            break;

            case C_PlayModeChg:
               if(gc_FmFreqChg_Flag==0)
			   {
	                //PlayMode_CHG();
					#if 1
	//				if(gc_CurrentCard==0)  //20090216 chiayen mark  
					{
						if(gs_System_State.c_Phase==TASK_PHASE_PAUSE)
						{
							gc_Task_Next=C_Task_PlayMenu;
						}
						else if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
						{
							gb_PickSong=0;  //20090513 chiayen add
							gb_ChannelSet=0;  //20090513 chiayen add
							gc_ShowTimer=0;  //20090513 chiayen add
							gc_Task_Next=C_Task_Setting;	
						}
					}
	//				else
					{
						if(gs_System_State.c_Phase==TASK_PHASE_STOP)
					    {
					        gc_PhaseInx=0x09;   
					    }
					    else
					    {
					        gc_PhaseInx=1;
					    }
					}
					#endif
				}
				else if(gc_FmFreqChg_Flag==1)
				{
                   FM_FREQ_CHGUP();
				}
            break;
			case C_PowerOff:
				if((gs_System_State.c_Phase==TASK_PHASE_PLAYACT)||(gs_System_State.c_Phase==TASK_PHASE_PAUSE))
                {
                    play_stop();    
                }
               // LCM_ShowPowerOff();
//			   	AS6200_PowerOff();
				gc_Task_Next=C_Task_PowerOff;
            break;
            case C_Play_to_fm:
                gc_Task_Next = C_Task_FM_NEW;
                break;

        }
        //(JC)======play icon proc=========s
        gw_Disp_CurrentSec=play_GetTimeFromDisplayBuf();

        if((gs_System_State.c_Phase == TASK_PHASE_PLAYACT)  && (gcPlay_FileType!=0x03))
        {
            gs_System_State.w_BitRate = (*Music_Bitrate[gcPlay_FileType])();
        }

		if((gcPlay_FileType==0x03)&&(gs_System_State.w_BitRate>0x320))
		{
			gs_System_State.w_BitRate = 0x236; 	
		}

		if(gc_Dirchange_Timer==0)
		{
        	LCM_Display();
		}

        gc_CurrentCard_backup=gc_CurrentCard;

		IR_Service_Process();

        if(gb_PickSongSet==1)
        {			
            PickSong_Process();
        }
        
        Polling_TaskEvents();

        if(gc_LogDataFlag==1)
        {
            USER_LogFile_ReadWrite(1);
            gc_LogDataFlag=0;
        }
		
		if(gb_HostError==1)  //20090107 chiayen add
		{
			gb_HostError=0;
			MediaChange_Play();
		}

		Play_SourceDetect_Process();
                
        if(gc_Task_Current!=gc_Task_Next)
        {
            gc_Task_Current=gc_Task_Next;
            if((gc_Task_Current==C_Task_USB) || (gc_Task_Current==C_Task_Dir) || (gc_Task_Current==C_Task_PlayMenu)
		    || (gc_Task_Current==C_Task_PlayMenu_IR)|| (gc_Task_Current==C_Task_Setting)|| (gc_Task_Current==C_Task_RPTMenu)) //20090107 chiayen add
            {
                if((gs_System_State.c_Phase==TASK_PHASE_PLAYACT)||(gs_System_State.c_Phase==TASK_PHASE_PAUSE))
                {
                    play_stop();    
                }
            }

            // Add my stuff to prepare FM task.
            if (gc_Task_Current == C_Task_FM_NEW)
            {
                play_stop();
            }

            // Quit the play task.
            break;  
        }
    }
}




