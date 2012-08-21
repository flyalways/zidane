#include "..\Header\SPDA2K.h"
#include "..\header\variables.h"

void MainMenu(void)
{
	gc_MenuTimer=6;	// Preset Function Menu timer
	LCM_Clear_L0_L7();
	LCM_ShowMainMenu();
	LCM_ShowBattIcon();
	while(1)
	{
		if(gb_ShowBatt==1)
		{
			gb_ShowBatt=0;
			LCM_ShowBattIcon();
		}

		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=6;	// Reset Function Menu timer
			
			if(gc_KeyEvent==3)	// Next
			{
				gc_MainMenuNum++;
				if(gc_MainMenuNum==5)
				{
					gc_MainMenuNum=0;
				}
				LCM_ShowMainMenu();
			}
			else if(gc_KeyEvent==4)	// Prev
			{
				gc_MainMenuNum--;
				if(gc_MainMenuNum==-1)
				{
					gc_MainMenuNum=4;
				}
				LCM_ShowMainMenu();
			}
			else if(gc_KeyEvent==6)	// M
			{
				if(gc_MainMenuNum==0)
				{
					SourceDevicePreSet();
					gc_Task_Current=C_Task_MusicPlay;
				}
				else if(gc_MainMenuNum==1)
				{
					gc_Task_Current=C_Task_FM;
				}
				else if(gc_MainMenuNum==2)
				{
					gc_Task_Current=C_Task_PlayWAV;
				}
				else if(gc_MainMenuNum==3)
				{
					gc_Task_Current=C_Task_Calendar;
				}
				else
				{
					gc_Task_Current=C_Task_Setup;
				}
				break;
			}
			else if(gc_KeyEvent==0x16)	// L-M
			{
				gc_MenuTimer=0;	// Abort
			}
		}

		if(gb_AlarmTrigger==1)
		{
			if(CheckAlarmSetting()==0)	// Match Alarm Clock Setting
			{
				gc_Task_Last=C_Task_FuncOption;
				gc_Task_Current=C_Task_AlarmON;
			}
		}

		if(gc_MenuTimer==0)
		{
			gc_Task_Current=C_Task_Idle;
			break;
		}
	}
}

