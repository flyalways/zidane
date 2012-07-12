#include "..\Header\SPDA2K.h"
#include "..\header\variables.h"

void SetAlarmClock(void)
{
	data	bit	tb_AlarmON;
	data	bit	tb_AlarmRepeatON;
	data	U8	tc_temp;

	tb_AlarmON=gb_AlarmON;
	tb_AlarmRepeatON=gb_AlarmRepeatON;
	gdw_AlarmTime_Bak=gdw_AlarmTime;
	LCM_Clear_L0_L7();
	gb_LCM_InverseDisp=1;
	LCM_ShowAlarmONOFFString();
	gb_LCM_InverseDisp=0;
	LCM_ShowAlarmClockString();
	LCM_ShowTimeString();
	LCM_ShowRepeatString();
	LCM_ShowAlarmTime_Hour();
	LCM_ShowChar8x16(10,87);	// :
	LCM_ShowAlarmTime_Minute();
	LCM_ShowAlarmRepeatONOFFString();
	gc_SetAlarmStep=0;
	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=6;	// Reset Function Menu timer
			switch(gc_KeyEvent)
			{
				case 0x03:	// Next
					gb_LCM_InverseDisp=1;
					if(gc_SetAlarmStep==0)
					{
						gb_AlarmON=!gb_AlarmON;
						LCM_ShowAlarmONOFFString();
					}
					else if(gc_SetAlarmStep==1)
					{
						gdw_AlarmTime+=3600;
						if(gdw_AlarmTime>=86400)
						{
							gdw_AlarmTime-=86400;
						}
						LCM_ShowAlarmTime_Hour();
					}
					else if(gc_SetAlarmStep==2)
					{
						if(gdw_AlarmTime%3600==3540)
						{
							gdw_AlarmTime-=3540;
						}
						else
						{
							gdw_AlarmTime+=60;
						}
						LCM_ShowAlarmTime_Minute();
					}
					else if(gc_SetAlarmStep==3)
					{
						gb_AlarmRepeatON=!gb_AlarmRepeatON;
						gb_LCM_InverseDisp=1;
						LCM_ShowAlarmRepeatONOFFString();
					}
					else
					{
						if(gc_AlarmRepeat_DaySelect==3)
						{
							gc_AlarmRepeat_DaySelect=4;
							LCM_ShowDaysString(4);
							LCM_ShowDaysString(5);
							LCM_ShowDaysString(6);
							LCM_ShowDaysString(7);
						}
						else if(gc_AlarmRepeat_DaySelect==7)
						{
							gc_AlarmRepeat_DaySelect=0;
							LCM_ShowDaysString(0);
							LCM_ShowDaysString(1);
							LCM_ShowDaysString(2);
							LCM_ShowDaysString(3);
						}
						else
						{
							gc_AlarmRepeat_DaySelect++;
							LCM_ShowDaysString(gc_AlarmRepeat_DaySelect-1);
							LCM_ShowDaysString(gc_AlarmRepeat_DaySelect);
						}
						LCM_ShowDaysSelect();
					}
				break;

				case 0x04:	// Prev
					gb_LCM_InverseDisp=1;
					if(gc_SetAlarmStep==0)
					{
						gb_AlarmON=!gb_AlarmON;
						gb_LCM_InverseDisp=1;
						LCM_ShowAlarmONOFFString();
					}
					else if(gc_SetAlarmStep==1)
					{
						if(gdw_AlarmTime<3600)
						{
							gdw_AlarmTime+=86400-3600;
						}
						else
						{
							gdw_AlarmTime-=3600;
						}
						LCM_ShowAlarmTime_Hour();
					}
					else if(gc_SetAlarmStep==2)
					{
						if(gdw_AlarmTime%3600<60)
						{
							gdw_AlarmTime+=3600-60;
						}
						else
						{
							gdw_AlarmTime-=60;
						}
						LCM_ShowAlarmTime_Minute();
					}
					else if(gc_SetAlarmStep==3)
					{
						gb_AlarmRepeatON=!gb_AlarmRepeatON;
						gb_LCM_InverseDisp=1;
						LCM_ShowAlarmRepeatONOFFString();
					}
					else
					{
						if(gc_AlarmRepeat_DaySelect==0)
						{
							gc_AlarmRepeat_DaySelect=7;
							LCM_ShowDaysString(4);
							LCM_ShowDaysString(5);
							LCM_ShowDaysString(6);
							LCM_ShowDaysString(7);
						}
						else if(gc_AlarmRepeat_DaySelect==4)
						{
							gc_AlarmRepeat_DaySelect=3;
							LCM_ShowDaysString(0);
							LCM_ShowDaysString(1);
							LCM_ShowDaysString(2);
							LCM_ShowDaysString(3);
						}
						else
						{
							gc_AlarmRepeat_DaySelect--;
							LCM_ShowDaysString(gc_AlarmRepeat_DaySelect+1);
							LCM_ShowDaysString(gc_AlarmRepeat_DaySelect);
						}
						LCM_ShowDaysSelect();
					}
				break;

				case 0x06:	// M
					if(gc_SetAlarmStep==0)
					{
						gb_LCM_InverseDisp=0;
						if(gb_AlarmON==0)
						{
							gc_Task_Current=C_Task_Setup;
							USER_LogFile_ReadWrite(1);
							return;
						}
						LCM_ShowAlarmONOFFString();
						gb_LCM_InverseDisp=1;
						LCM_ShowAlarmTime_Hour();
					}
					else if(gc_SetAlarmStep==1)
					{
						gb_LCM_InverseDisp=0;
						LCM_ShowAlarmTime_Hour();
						gb_LCM_InverseDisp=1;
						LCM_ShowAlarmTime_Minute();
					}
					else if(gc_SetAlarmStep==2)
					{
						gb_LCM_InverseDisp=0;
						LCM_ShowAlarmTime_Minute();
						gb_LCM_InverseDisp=1;
						LCM_ShowAlarmRepeatONOFFString();
					}
					else if(gc_SetAlarmStep==3)
					{
						if(gb_AlarmRepeatON==0)
						{
							gc_Task_Current=C_Task_Setup;
							gb_LCM_InverseDisp=0;
							USER_LogFile_ReadWrite(1);
							return;
						}
						else
						{
							gb_LCM_InverseDisp=0;
							LCM_Clear_L0_L7();
							gc_AlarmRepeat_DaySelect=0;
							LCM_ShowDaysString(0);
							LCM_ShowDaysString(1);
							LCM_ShowDaysString(2);
							LCM_ShowDaysString(3);
							LCM_ShowDaysSelect();
						}
					}
					else
					{
						if(gc_AlarmRepeat_DaySelect==7)	// Exit
						{
							gc_Task_Current=C_Task_Setup;
							gb_LCM_InverseDisp=0;
							USER_LogFile_ReadWrite(1);
							return;
						}
						tc_temp=1<<gc_AlarmRepeat_DaySelect;
						if(gc_AlarmDaysSelect&tc_temp)
						{
							gc_AlarmDaysSelect&=~tc_temp;
						}
						else
						{
							gc_AlarmDaysSelect|=tc_temp;
						}
						LCM_ShowDaysSelect();
						gc_SetAlarmStep--;
					}
					gc_SetAlarmStep++;
				break;
			}
		}

		if(gc_MenuTimer==0)
		{
			gb_LCM_InverseDisp=0;
			gc_Task_Current=C_Task_Setup;
			gb_AlarmON=tb_AlarmON;
			gb_AlarmRepeatON=tb_AlarmRepeatON;
			gdw_AlarmTime=gdw_AlarmTime_Bak;
			return;
		}
	}
}


