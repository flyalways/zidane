#include "..\header\SPDA2K.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"


void WAV_Play(void)
{
	data	U8	tc_temp;

	XBYTE[0xB046]&=~0x04;
	XBYTE[0xB0C1]=0xB3;
	XBYTE[0xB0C2]=0x00;	// disable ADC, MIC bias voltage and boost amplifier 18dB(0x3)
	gc_CurrentCard=gc_LastCard;
	gc_Play_FileType=2;
	gs_System_State.c_Phase=TASK_PHASE_STOP;
	gw_CurrentSec=0;
	gw_FileIndex[0]=0;
	gw_FileTotalNumber[0]=0;
	gb_SetVol=0;
	gb_LrcFileName_Exist=0;
	RefreshAllDisplay();
	// Select Media
	MediaSelect();	//gb_ChangeMedia=1->Success
	if(gc_CurrentCard!=2&&gc_CurrentCard!=5)
	{
		// No Media
		gc_Task_Current=C_Task_Idle;
		return;
	}

	while(1)
	{
		if(gb_ChangeMedia==1)
		{
			gb_ChangeMedia=0;
			gs_System_State.c_Phase=TASK_PHASE_STOP;
			if(MediaInitial()==0)
			{
				if(SearchFolder_VOICE()==0)	// Search Folder_VOICE OK
				{
					if(gw_FileTotalNumber[1]!=0)
					{
						gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
						DOS_Search_File(C_File_OneDir|C_By_Name,C_RecordFileType,C_CmpExtName|C_Next);
						gb_PlayNewWave=1;
					}
					else
					{
						// No Wave file
						gc_Task_Current=C_Task_Idle;
						return;
					}
				}
				else
				{
					// No VOICE folder--Add VOICE folder
					gc_Task_Current=C_Task_Idle;
					return;
				}
			}
		}

		if(gb_PlayNewWave==1)
		{
			gb_PlayNewWave=0;
			WAV_Play_Init();
			gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB_NULL;
		}

		// Access Key
		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=6;
			AccessKeyEvent_WAV();
			gc_KeyEvent=0;
		}
			
		if(gb_AlarmTrigger==1)
		{
			if(CheckAlarmSetting()==0)	// Match Alarm Clock Setting
			{
				gc_Task_Last=C_Task_PlayWAV;
				gc_Task_Current=C_Task_AlarmON;
			}
		}

		if(gc_Task_Current!=C_Task_PlayWAV)
		{
			IMA_ADPCM_Play_StopCmd();
			return;
		}

		if(gb_OrderRecordTrigger==1)
		{
			gc_Task_Current=C_Task_FM;
			IMA_ADPCM_Play_StopCmd();
			return;
		}

		// Leave Menu/Vol setting, Refresh All display
		if((gc_MenuTimer==0)&&(gc_WAVMenuItem!=0||gb_SetVol))
		{
			gb_SetVol=0;
			gc_WAVMenuItem=0;
			RefreshAllDisplay();
			LCM_Disp_FileName(&gc_FileLongName[5],gc_FileLongName[2],gc_FileLongName[4],1);
			USER_LogFile_ReadWrite(1);
		}

		if(gb_ShowBatt==1&&gc_WAVMenuItem==0)
		{
			gb_ShowBatt=0;
			LCM_ShowBattIcon();
		}

		if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_AB_NULL)
		{
			if(gc_ShowRepeatTimer==0)
			{
				gc_ShowRepeatTimer=10;
				LCM_ShowRepeatIcon();
			}
		}

		switch(gs_System_State.c_Phase)
		{
			case TASK_PHASE_PLAYACT:
				// Fill WAV data to DSP
				if((gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_AB)||(gs_File_FCB[1].dw_File_DataPoint<=gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint))
				{
					if(IMA_ADPCM_Play_DataIn()==DSP_DATAIN_COMMAND_ERROR)
					{
						if(IMA_ADPCM_EOF_Proc()==1)
						{
							gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB_NULL;
							IMA_ADPCM_Play_StopCmd();
							DOS_Search_File(C_File_OneDir|C_By_Name,C_RecordFileType,C_CmpExtName|C_Next);
							gb_PlayNewWave=1;
						}
					}
				}
				else
				{
					IMA_ADPCM_Repeat_DataIn();
				}
				// LCM Display
				if(gc_WAVMenuItem==0)
				{
					// ภWระ
					if(gc_PinPuTimer==0)
					{
						LCM_ShowPinPu();
						if((gc_ShowTimer==0)&&(gc_IR_Timer==0)&&(gb_SetVol==0))
						{
							// Play-Time
							gw_Disp_CurrentSec=IMA_ADPCM_Play_ReadTime();
							LCM_ShowPlayTime(gw_Disp_CurrentSec);
						}
					}
				}
			break;

			case TASK_PHASE_PAUSE:
			break;

			case TASK_PHASE_STOP:
			break;
		}

		tc_temp=gc_CurrentCard;
		WAV_SourceDetect_Process();
		if(gc_CurrentCard==9)
		{
			gc_Task_Current=C_Task_Idle;
			return;
		}
	}
}


