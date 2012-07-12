#include "..\Header\SPDA2K.h"
#include "..\header\variables.h"


void SetupMenu(void)
{
	gc_MenuTimer=6;	// Preset Function Menu timer
	if(gc_SetupMenuNum<4)
	{
		LCM_ShowSetupMenu(0);
		LCM_ShowSetupMenu(1);
		LCM_ShowSetupMenu(2);
		LCM_ShowSetupMenu(3);
	}
	else
	{
		LCM_Clear_L2_L7();
		LCM_ShowSetupMenu(4);
	}
	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=6;	// Reset Function Menu timer
			if(gc_KeyEvent==3)	// Next
			{
				if(gc_SetupMenuNum==3)
				{
					gc_SetupMenuNum=4;
					LCM_Clear_L2_L7();
					LCM_ShowSetupMenu(4);
				}
				else if(gc_SetupMenuNum==4)
				{
					gc_SetupMenuNum=0;
					LCM_ShowSetupMenu(0);
					LCM_ShowSetupMenu(1);
					LCM_ShowSetupMenu(2);
					LCM_ShowSetupMenu(3);
				}
				else
				{
					gc_SetupMenuNum++;
					LCM_ShowSetupMenu(gc_SetupMenuNum-1);
					LCM_ShowSetupMenu(gc_SetupMenuNum);
				}
			}
			else if(gc_KeyEvent==4)	// Prev
			{
				if(gc_SetupMenuNum==0)
				{
					gc_SetupMenuNum=4;
					LCM_Clear_L2_L7();
					LCM_ShowSetupMenu(4);
				}
				else if(gc_SetupMenuNum==4)
				{
					gc_SetupMenuNum=3;
					LCM_ShowSetupMenu(0);
					LCM_ShowSetupMenu(1);
					LCM_ShowSetupMenu(2);
					LCM_ShowSetupMenu(3);
				}
				else
				{
					gc_SetupMenuNum--;
					LCM_ShowSetupMenu(gc_SetupMenuNum+1);
					LCM_ShowSetupMenu(gc_SetupMenuNum);
				}
			}
			else if(gc_KeyEvent==6)
			{
				switch(gc_SetupMenuNum)
				{
					case 0:
						gc_Task_Current=C_Task_SetClock;
					break;

					case 1:
						gc_Task_Current=C_Task_SetBacklight;
					break;

					case 2:
						gc_Task_Current=C_Task_SetLanguage;
					break;

					case 3:
						gc_Task_Current=C_Task_SetAlarm;
					break;

					default:
						gc_Task_Current=C_Task_Idle;
					break;
				}
				return;
			}
		}

		if(gb_AlarmTrigger==1)
		{
			if(CheckAlarmSetting()==0)	// Match Alarm Clock Setting
			{
				gc_Task_Last=C_Task_Setup;
				gc_Task_Current=C_Task_AlarmON;
			}
		}

		if(gc_MenuTimer==0)
		{
			gc_Task_Current=C_Task_Idle;
			return;
		}
	}
}



