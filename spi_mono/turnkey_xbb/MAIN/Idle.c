#include "..\Header\SPDA2K.h"
#include "..\header\variables.h"
#include "..\LCM\LCM_BMP.h"

void Idle_Task(void)
{
	gb_LCM_InverseDisp=0;
	LCM_Clear_L0_L7();
	LCM_ShowChar16x32(10,78);
	LCM_ShowChar16x32(10,33);
	gc_LCMBacklightTimer=gc_LCMBacklightTime;
	gb_RTC_wakeup=1;
	gb_LCM_ForceShow=1;
	gc_CurrentCard=9;
	LCM_ShowBattIcon();
	LCM_ShowFMOrderRecordIcon();
	LCM_ShowAlarmIcon();
	gc_Idle2Suspend=60;

	while(1)
	{
		if(gb_ShowBatt==1)
		{
			gb_ShowBatt=0;
			LCM_ShowBattIcon();
		}

		if(gb_RTC_wakeup==1)
		{
			LCM_ShowClock();
			gb_RTC_wakeup=0;
		}

		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_Idle2Suspend=60;
			gb_LCM_InverseDisp=0;
			gb_LCM_ForceShow=1;
//			LCM_ShowClock();
			LCM_BACKLIGHT=0;

			switch(gc_KeyEvent)
			{
				case 0x06:	// M
				case 0x16:	// L-M
					gc_Task_Current=C_Task_FuncOption;
					return;
				break;

				case 0x12:	// L-Play
					gc_Task_Current=C_Task_Suspend;
					return;
				break;
			}
		}

		if(gb_SD_Exist==0)
		{
			if(SD_Check()==0)
			{
				gb_SD_Exist=1;
				gc_CurrentCard=2;
			}
		}
		else
		{
			if(SD_Detect==1)
			{
				gb_SD_Exist=0;
			}
		}

		if(gb_Host_Exist==0)
		{
			if(HOST_Check()==0)
			{
				gb_Host_Exist=1;
				gc_CurrentCard=5;
			}
		}
		else
		{
			if(Host_DetectDevice()==1)
			{
				gb_Host_Exist=0;
			}
		}

		if(gc_CurrentCard!=9)
		{
			gc_LastCard=gc_CurrentCard;
			gb_LCM_InverseDisp=0;
			LCM_Clear_L0_L7();
			LCM_ShowWait();
			LCM_BACKLIGHT=0;
			Play_SDFlash_DosInit();
			gc_Task_Current=C_Task_MusicPlay;
			return;
		}

		if(gb_OrderRecordTrigger==1)
		{
			gc_Task_Current=C_Task_FM;
			return;
		}
		
		if(gb_AlarmTrigger==1)
		{
			if(CheckAlarmSetting()==0)	// Match Alarm Clock Setting
			{
				gc_Task_Last=C_Task_Idle;
				gc_Task_Current=C_Task_AlarmON;
				return;
			}
		}

		if(gc_Idle2Suspend==0)
		{
			gc_Task_Current=C_Task_Suspend;
			return;
		}
	}
}
