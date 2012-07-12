#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\IR\remote.h"
#include "..\header\host_init.h"
#include "..\header\variables.h"


//----------------------------------------------------------------------------------------------------------
void UI_fastFFFR_Play(U8 XFFFR);
void play_fffr_end();
U8 MP3_FF_FR_Cmd(void);
U8 WMA_FF_FR_Cmd();
void MP3_seek_trig(void);		//Jimi 080530
static void WMA_seek_trig();		//Jimi 080530

extern xdata U8  gcPlay_FileType;
xdata	U16	gw_CTStartTime=0;
extern void LCM_ShowPlayTime(U16 time);

//-----------------------------------------------------------------------------------------------------------
extern	U16 const_timer_get_counter(void);
extern void Music_EQ_Cmd(U8 tc_Type);
extern void Music_SetVolumeCmd(void);
extern void Music_WakeUp(U8 tc_Type);
extern code U8 (code * Music_EOF_Proc[])(void);
extern code U16 (code * Music_Bitrate[])(void);
extern code U16 (code * Music_ReadTime[])(void); 
extern code U8 (code * Music_Data_In[])(void); 
extern code U8 (code * Music_Download[])(void);

//----------------------------------------------------------------------------------------------------------

extern data  System_Struct gs_System_State;

void UI_fastFFFR_Play(U8 XFFFR)
{
    xdata	U8	tc_Ret;

	if((gs_System_State.c_Phase != TASK_PHASE_FASTFWD)&&(gs_System_State.c_Phase!=TASK_PHASE_FASTREV))
	{
		if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)       //tangwei add 081010
		{    
		    if(XFFFR==0)
		    {// play_fastfwd
			    gs_System_State.c_Phase=TASK_PHASE_FASTFWD;
		    }
		    else
		    {// play_fastrev
			    gs_System_State.c_Phase=TASK_PHASE_FASTREV;
		    }

		    gs_System_State.w_BitRate = (*Music_Bitrate[gcPlay_FileType])();
		    gw_CurrentSec = (*Music_ReadTime[gcPlay_FileType])();//(JC)Time reset after fffr
			switch(gcPlay_FileType)
			{
				case 0x00: //MP3
					tc_Ret=MP3_FF_FR_Cmd();
					break;
				case 0x01: //WMA
					tc_Ret=WMA_FF_FR_Cmd();
					break;
			}

		    if (tc_Ret != DSP_SUCCESS)
			{
//				dbprintf("FF err!!\n");		    
			}
		    gw_CTStartTime=const_timer_get_counter();//(JC)H0925 using const timer
		}
	}

	if(gs_System_State.c_Phase==TASK_PHASE_FASTFWD)
	{	
		if(gw_CurrentSec<gw_TotalSec)
		{
			if( (const_timer_get_counter() - gw_CTStartTime) >= 40)//(JC)H0925 using const timer;~200ms
			{
				gw_CTStartTime=const_timer_get_counter();//(JC)H0925 using const timer
				gw_CurrentSec+=1;
				gw_Disp_CurrentSec=gw_CurrentSec;//(JC)H0930
			}
		}
	}
	else if(gs_System_State.c_Phase==TASK_PHASE_FASTREV)
	{
		if(gw_CurrentSec!=0)
		{
			if( (const_timer_get_counter() - gw_CTStartTime) >= 40)//(JC)H0925 using const timer;~200ms
			{
				gw_CTStartTime=const_timer_get_counter();//(JC)H0925 using const timer
				gw_CurrentSec-=1;
				gw_Disp_CurrentSec=gw_CurrentSec;//(JC)H0930
			}
		}
	}

	LCM_ShowPlayTime(gw_CurrentSec);
}

U8 MP3_FF_FR_Cmd(void)
{
	xdata	U8  tc_Ret;
	xdata	U16 TimeOUT=0xffff;
	xdata	U16 temp;

	tc_Ret = DSP_StopCmd();

	// Reset decode status
	do{
		TimeOUT --;
		if(TimeOUT==0)
		{
			return DSP_DECODE_STATUS_TIMEOUT_ERROR;
		}
	}while(!((L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)==0x00C0));  //wait file end

	temp = L2_DSP_Read_DMem16(DSP_DecodeStatus);
	L2_DSP_Write_DMem16(DSP_DecodeStatus,temp&0xFF3F);
	L2_DSP_Write_DMem16(DSP_RemainBuffer, 0);				//Jimi 081127
	L2_DSP_Write_DMem16(DSP_EmptyBuffer, 0x3000);			//Jimi 081127
	gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;						//Jimi 081127

	return tc_Ret;
}


U8 WMA_FF_FR_Cmd()
{
	xdata	U8  tc_Ret;
	xdata	U16 TimeOUT=0xffff;

	Music_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);
	tc_Ret = DSP_PauseCmd();

	// Reset decode status
	do{
		TimeOUT --;
		if(TimeOUT==0)
		{
			return DSP_DECODE_STATUS_TIMEOUT_ERROR;
		}
	} while (!L2_DSP_Read_DMem16(DSP_RampDownComplete)); //Jimi 090417, DSP_RampDownComplete = 0x3FB8, indicates RampDown OK or NOT
	return tc_Ret;
}


void play_fffr_end()
{
	U32	tdw_datapoint;

    gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster=gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
    if(gs_System_State.c_Phase==TASK_PHASE_FASTFWD)
    {
	    if(gw_CurrentSec==gw_TotalSec)
	    {
		    tdw_datapoint=0;
		    gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint=gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize;
			L2_DSP_Write_DMem16(DSP_RemainBuffer, 0);	//Jimi 090213
		    gs_System_State.c_Phase=TASK_PHASE_STOP;	//Jimi 080530, if fast forward to the end of the file then stop and play next
		    gc_PhaseInx=2;
	    }
	    else
	    {
			tdw_datapoint=gw_CurrentSec*(gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize/gw_TotalSec);
	    }
    }
    else if(gs_System_State.c_Phase==TASK_PHASE_FASTREV)
    {
	    if(gw_CurrentSec==0)
	    {
		    tdw_datapoint=0;
		    gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint=0;
		    gs_System_State.c_Phase=TASK_PHASE_STOP;
		    gc_PhaseInx=3;
	    }
	    else
	    {
			tdw_datapoint=gw_CurrentSec*(gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize/gw_TotalSec);	
	    }
    }
    if(tdw_datapoint>0)
    {
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = DOS_Seek_File(gs_System_State.c_FileHandle, tdw_datapoint>>9);
	    gc_PhaseInx=3;
		switch(gcPlay_FileType)
		{
			case 0x00: //MP3
				MP3_seek_trig();
			break;

			case 0x01: //WMA
				WMA_seek_trig();
			break;
		}
	}
}


void MP3_seek_trig(void)		//Jimi 080530
{
	xdata	U8	tc_Cnt;
	xdata	U16	tw_FrmCnt;
	xdata	U8	tc_timeout;

	// restart DSP, Jimi 081125
	tw_FrmCnt = L2_DSP_Read_DMem16(DSP_DecodeFrameCounter);
	for (tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if (DSP_PlayInit()==DSP_SUCCESS)
		{
			break;
		}

		if (tc_Cnt < 5) 
		{
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
		else
		{
			GLOBAL_REG[0x02] |= 0x01;		// DSP reset
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
	}

	Music_SetVolumeCmd();
	// Jimi Yu 081208
	L2_DSP_Write_DMem16(DSP_PostProcessSelect, L2_DSP_Read_DMem16(DSP_PostProcessSelect)|0x0020);
	tc_timeout = 0xFF;
	while( (L2_DSP_Read_DMem16(DSP_PostProcessSelect)&0x0020))	//Handshake with DSP to make sure that DSP has ramp digital volume up.
	{
		if(!(tc_timeout--))
		{
			break;
		}
	}
	Music_EQ_Cmd(gs_DSP_GLOBAL_RAM.sc_EQ_Type);
	L2_DSP_Write_DMem16(DSP_DecodeFrameCounter, tw_FrmCnt);
	L2_DSP_SendCommandSet(DCMD_Play);
}


void WMA_seek_trig()		//Jimi 080530
{
	L2_DSP_Write_DMem16(DSP_EmptyBuffer,0x3C00);	//reset BS buffer information //Jimi081112 mdf size to 0x3C00
	L2_DSP_Write_DMem16(DSP_RemainBuffer,0x0000);	
	gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;		
	L2_DSP_Write_DMem24(DSP_WMASectorOffset,(gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint>>9));		
	L2_DSP_Write_DMem16(DSP_DecodeStatus,0x0000);
	L2_DSP_Write_DMem16(DSP_WMARandomFlag,0x0001);   //set random flag

	L2_DSP_SendCommandSet(DCMD_Play);
}



