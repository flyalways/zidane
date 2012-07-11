#include "..\header\SPDA2K.h"
#include "..\header\variables.h"
#include "..\DSP\dspuser.h"
#include "..\DSP\DSPPHYSIC.H"

void UI_fastFFFR_Play(U8 XFFFR)
{
	gc_LongKeyCount=30;
	if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
	{  
	    if(XFFFR==0)
	    {// play_fastfwd
		    gs_System_State.c_Phase=TASK_PHASE_FASTFWD;
	    }
	    else
	    {// play_fastrev -- XFFFR == 1
		    gs_System_State.c_Phase=TASK_PHASE_FASTREV;
	    }

		if(gc_Play_FileType==0)
		{
			gw_CurrentSec=MP3_ReadTime();
			gs_System_State.w_SampleRate=L2_DSP_Read_DMem16(DSP_MP3Bitrate);
			MP3_FF_FR_Cmd();
		}
		else
		{
			gw_CurrentSec=WMA_ReadTime();
			gs_System_State.w_SampleRate=L2_DSP_Read_DMem16(DSP_WMABitrate);
			WMA_FF_FR_Cmd();
		}
	}
	else if(gs_System_State.c_Phase==TASK_PHASE_FASTFWD)
	{	
		if(gw_CurrentSec<gw_TotalSec)
		{
			gw_CurrentSec+=1;
			gw_Disp_CurrentSec=gw_CurrentSec;
		}
	}
	else if(gs_System_State.c_Phase==TASK_PHASE_FASTREV)
	{	
		if(gw_CurrentSec!=0)
		{
			gw_CurrentSec-=1;
			gw_Disp_CurrentSec=gw_CurrentSec;
		}
	}
}

U8 MP3_FF_FR_Cmd(void)
{
	data	U8  tc_Ret;
	data	U16	TimeOUT=0xFFFF;
	xdata	U16	temp;

	tc_Ret=DSP_StopCmd();

	do{
		TimeOUT --;
		if(TimeOUT==0)
		{
			return DSP_DECODE_STATUS_TIMEOUT_ERROR;
		}
	}while(!((L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)==0x00C0));

	temp=L2_DSP_Read_DMem16(DSP_DecodeStatus);
	L2_DSP_Write_DMem16(DSP_DecodeStatus,temp&0xFF3F);
	L2_DSP_Write_DMem16(DSP_RemainBuffer,0);
	L2_DSP_Write_DMem16(DSP_EmptyBuffer,0x3000);
	gs_DSP_GLOBAL_RAM.sc_DM_Index=0;
	return tc_Ret;
}


U8 WMA_FF_FR_Cmd()
{
	data	U8  tc_Ret;
	data	U16	TimeOUT=0xFFFF;

	DSP_WakeUp();
	tc_Ret=DSP_PauseCmd();

	do{
		if(--TimeOUT==0)
		{
			return DSP_DECODE_STATUS_TIMEOUT_ERROR;
		}
	} while (!L2_DSP_Read_DMem16(DSP_RampDownComplete)); //Jimi 090417, DSP_RampDownComplete = 0x3FB8, indicates RampDown OK or NOT

	return tc_Ret;
}

void play_fffr_end()
{
	xdata	U32	tdw_datapoint;

	gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster=gs_File_FCB[0].dw_File_StartCluster;
	tdw_datapoint=gw_CurrentSec*(gs_File_FCB[0].dw_File_TotalSize/gw_TotalSec);	

    if(tdw_datapoint==0)
	{
		gs_File_FCB[0].dw_File_DataPoint=0;
		gb_Mp3FileEnd=1;
		return;
	}
	else
    {
		gs_File_FCB[0].dw_File_CurrentCluster=DOS_Seek_File(0,tdw_datapoint>>9);
		if(gc_Play_FileType==0)
		{
			MP3_seek_trig();
		}
		else
		{
			WMA_seek_trig();
		}
    }
}

void MP3_seek_trig(void)
{
	data	U8	tc_Cnt;
	data	U16 tw_FrmCnt;

	// restart DSP, Jimi 081125
	tw_FrmCnt=L2_DSP_Read_DMem16(DSP_DecodeFrameCounter);
	for(tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if (DSP_PlayInit()==DSP_SUCCESS)
		{
			break;
		}

		if(tc_Cnt>4) 
		{
			XBYTE[0xB002]|=0x01;	// DSP reset
		}
		XBYTE[0xB002]&=~0x01;	// Enable DSP Run Normal Mode
	}

	L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sc_Volume);
	L2_DSP_Write_DMem16(DSP_PostProcessSelect, L2_DSP_Read_DMem16(DSP_PostProcessSelect)|0x0020);
	{
		U8 tc_timeout = 0xFF;
		while( (L2_DSP_Read_DMem16(DSP_PostProcessSelect)&0x0020))	//Handshake with DSP to make sure that DSP has ramp digital volume up.
		{
			if (!(tc_timeout--))
			{
				break;
			}
		}
	}
	Music_EQ_Cmd();
	L2_DSP_Write_DMem16(DSP_DecodeFrameCounter, tw_FrmCnt);
	L2_DSP_SendCommandSet(DCMD_Play);
}


void WMA_seek_trig(void)
{
	L2_DSP_Write_DMem16(DSP_EmptyBuffer,0x3C00);	//reset BS buffer information //Jimi081112 mdf size to 0x3C00
	L2_DSP_Write_DMem16(DSP_RemainBuffer,0x0000);	
	gs_DSP_GLOBAL_RAM.sc_DM_Index=0;		
	L2_DSP_Write_DMem24(DSP_WMASectorOffset,(gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint>>9));		
	L2_DSP_Write_DMem16(DSP_DecodeStatus,0x0000);
	L2_DSP_Write_DMem16(DSP_WMARandomFlag,0x0001);   //set random flag
	L2_DSP_SendCommandSet(DCMD_Play);
}



