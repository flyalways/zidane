#include "..\Header\SPDA2K.h"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"

#include "FM_receiveH.h"
#include "FM_IIC.c"
///////////////////
//d检测FM模组的类型
//gc_FM_Type=0xff：未检测到FM
xdata U8 gc_FM_Type;

void FM_Initial();
void FM_OPEN(void);
void FM_Powerdown(void);

void FM_Task(void)
{
	gc_Play_FileType=2;
	gb_LCM_InverseDisp=0;
	set_clock_mode(CLOCK_MODE_REC);
	gb_Dosinitfail=0;
	gdw_HOSTStartSectorRead=0xFFFFFFF0;
	gc_CurrentCard=9;


	if(gc_LastCard==2)
	{
		if(gb_SD_Exist==1)
		{
			if(SD_Identification_Flow())
			{
				gc_CurrentCard=2;
				if(DOS_Initialize()!=0)
				{
					gc_CurrentCard=9;
				}
			}
		}
	}
	else if(gc_LastCard==5)
	{
		if(HOST_Check()==0)
		{
			gb_Host_Exist=1;
		}

		if(gb_Host_Exist==1)
		{
			if(Host_Initial()==0)
			{
				if(DOS_Initialize()!=0)
				{
					gc_CurrentCard=9;
				}
			}
			else
			{
				gc_CurrentCard=9;
			}
		}
	}
	else
	{
		gb_Host_Exist=0;
		gb_SD_Exist=0;
	}
	gc_LastCard=gc_CurrentCard;

	set_clock_mode(CLOCK_MODE_MP3);
	LCM_ShowFMAllIcon();
	LCM_ShowFrequency_L();
	gb_SetVol=0;
	MP3_Download();
	gs_System_State.c_Phase=TASK_PHASE_STOP;

	gc_FM_CH_COUNT=0;
	gb_FM_initial_flag=0;
	if(gb_FM_initial_flag==0)
	{
		gb_FM_initial_flag=1;	
		FM_Initial();
	}
	else
	{
//FM_OPEN();//FM_UnMUTE(); maybe unmute here --sunzhk add 
	}
	Linein_Set();
	gb_LogDataFlag=0;
	gs_System_State.c_Phase=TASK_PHASE_STOP;
	gc_KeyEvent=0;

	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=6;
			AccessKeyEvent_FM();
			gc_KeyEvent=0;
			if(gc_Task_Current!=C_Task_FM)
			{
				Linein_Out();
				//------Clear IR Variable----------------------------
				ClearIRNumberVariable();
				gc_IR_Timer=0;
				gc_ShowFMRVolTimer=0;
				gb_ClearBigIcon=0;
FM_Powerdown();	//FM_MUTE();  maybe not power off FM,only mute fm is OK		--sunzhk 
				return;
			}
		}

		if(gb_OrderRecordTrigger==1)
		{
			if(gs_System_State.c_Phase==TASK_PHASE_STOP)
			{
				// Start Order Record
				gc_FMMenuItem=1;
				gc_FMMenuNum=0;
				gc_KeyEvent=0x06; //	Issue M key
				AccessKeyEvent_FM();
				gc_KeyEvent=0;
			}
			else
			{
				gb_OrderRecordTrigger=0;
			}
		}

		if(gdw_RecRemaintime==0&&gs_System_State.c_Phase==TASK_PHASE_REC)
		{
			gc_KeyEvent=0x06; //	Issue M key
			AccessKeyEvent_FM();
			gc_KeyEvent=0;
			if(gb_Suspend_OrderRecord==1)
			{
				gb_Suspend_OrderRecord=0;
				gc_Task_Current=C_Task_Suspend;
				Linein_Out();
				return;
			}
		}

		// Leave Menu/Vol setting, Refresh All display
		if((gc_MenuTimer==0)&&(gc_FMMenuItem!=0||gb_SetVol))
		{
			gb_SetVol=0;
			gc_FMMenuItem=0;
			gb_LogDataFlag=0;
			// Show LCM icon
			LCM_ShowFMAllIcon();
			if(gs_System_State.c_Phase==TASK_PHASE_STOP)
			{
				LCM_ShowFrequency_L();
				USER_LogFile_ReadWrite(1);
			}
			if(gs_System_State.c_Phase==TASK_PHASE_REC)
			{
				LCM_ShowRECFilename();
			}
		}

		if(gb_ShowBatt==1&&gc_FMMenuItem==0)
		{
			gb_ShowBatt=0;
			LCM_ShowBattIcon();
		}

		if(gb_AlarmTrigger==1)
		{
			if(gs_System_State.c_Phase==TASK_PHASE_REC)
			{
				// If Recording, don't care alarm clock
				gb_AlarmTrigger=0;
			}
			else
			{
				if(CheckAlarmSetting()==0)	// Match Alarm Clock Setting
				{
					gc_Task_Last=C_Task_FM;
					gc_Task_Current=C_Task_AlarmON;
					Linein_Out();
					return;
				}
			}
		}

		if(gs_System_State.c_Phase==TASK_PHASE_REC)
		{
			if(IMA_ADPCM_Rec_DataOut()!=DSP_SUCCESS)
			{
				IMA_ADPCM_Rec_StopCmd();
				gs_System_State.c_Phase=TASK_PHASE_STOP;
				gc_FMMenuItem=9;
			}

			if((gc_CurrentCard==2&&gb_SD_pin==1)||(gc_CurrentCard==5&&Host_DetectDevice()==1))
			{
				IMA_ADPCM_Rec_StopCmd();
				gs_System_State.c_Phase=TASK_PHASE_STOP;
				gc_FMMenuItem=9;
			}
		}
		else if(gs_System_State.c_Phase==TASK_PHASE_STOP)
		{
			DetectSDorHOSTState();
		}
   }
}

void FM_Initial()
{
	if(gc_FM_CHAmount==0)
	{
		gw_FM_frequency= 875;
	}
	else
	{
		gw_FM_frequency = gw_FM_CH[0]; 
	}

#ifdef Mode_FM_CL6017
	CL6017_FMInit();
	CL6017_FMTune();
#elif defined Mode_FM_RDA5807P
	RDA5807P_FMInit();
	RDA5807P_FMTune();//set 进来的频率
#elif defined Mode_FM_QN8035
	QND_Init();
	QND_TuneToCH();
#endif
		
}

void FM_Powerdown(void)//关掉FM
{
#ifdef Mode_FM_CL6017
	CL6017_FMPowerdown();
#elif defined Mode_FM_RDA5807P
	RDA5807P_FMPowerdown(); 
#elif defined Mode_FM_QN8035
	QNF_PowerDown();
#endif
}

///////////////////////以下操作在在按键相应中调用
void FM_MUTE(void)
{
#ifdef Mode_FM_CL6017
	CL6017_MUTE();
#elif defined Mode_FM_RDA5807P
	RDA5807P_MUTE();
#elif defined Mode_FM_QN8035
	QNF_SetMute(1);//mute
#endif
}
void FM_UnMUTE(void)
{
#ifdef Mode_FM_CL6017
	CL6017_UnMUTE();
#elif defined Mode_FM_RDA5807P
	RDA5807P_UnMUTE();
#elif defined Mode_FM_QN8035
	QNF_SetMute(0);//Unmute
#endif
}

void FM_FMVolumeSet(U16 vol)
{
#ifdef Mode_FM_CL6017
	CL6017_FMVolumeSet(vol);
#elif defined Mode_FM_RDA5807P
	RDA5807P_FMVolumeSet(vol); 
#elif defined Mode_FM_QN8035
	QND_RXConfigAudio(vol);
#endif
}
//before this func ,please set gw_FM_frequency!!-sunzhk add 
void FM_FMTune()//设台，tc_mute=1时设台后无声音（用作搜台时设台）
{
#ifdef Mode_FM_CL6017
	CL6017_FMTune();
#elif defined Mode_FM_RDA5807P
	RDA5807P_FMTune();
#elif defined Mode_FM_QN8035
	QND_TuneToCH();
#endif
}
void FM_FMSeekFromCurrentCh(U8 seekUp)//seekUp=0向上搜台，seekUp=1向下搜台
{
#ifdef Mode_FM_CL6017
	CL6017_FMSeekFromCurrentCh(seekUp);
#elif defined Mode_FM_RDA5807P
	RDA5807P_FMSeekFromCurrentCh(seekUp); 
#elif defined Mode_FM_QN8035
	QN8035_FMSeekFromCurrentCh(seekUp);
#endif
}


void FM_Auto_SearchAllFreq(U16 x_FreqTemp)//全部搜台
{
#ifdef Mode_FM_CL6017
	CL6017AutoSearchAllFreq(x_FreqTemp);
#elif defined Mode_FM_RDA5807P
	RDA5807P_FMSeekFromCurrentCh(2); //全部搜台
#elif defined Mode_FM_QN8035
	QN8035_FMSeekAllCh(x_FreqTemp);  
#endif
}

