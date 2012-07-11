#include "..\Header\SPDA2K.h"
#include "..\header\variables.h"

code	U16	K_Year[4]={365,365,366,365};
code	U8	K_Month[12]={31,28,31,30,31,30,31,31,30,31,30,31};

void SetClock(void)
{
	data	U8	i;
	data	U16	tw_Days;
	data	U32	tdw_Seconds;

	RTC_Calendar();
	LCM_Clear_L0_L7();
	LCM_ShowTitle_SetClock();
	gc_LCM_line=3;
	LCM_ShowChar8x16(11,56);	// "/"
	LCM_ShowChar8x16(11,80);	// "/"
	gc_LCM_line=5;
	LCM_ShowChar8x16(10,48);	// ":"
	LCM_ShowChar8x16(10,72);	// ":"

	LCM_ShowMonth(gc_Month);
	LCM_ShowDay(gc_Day);
	LCM_ShowHour(gc_Hour);
	LCM_ShowMin(gc_Min);
	LCM_ShowSec(gc_Sec);
	gb_LCM_InverseDisp=1;
	LCM_ShowYear(gc_Year);

	gc_MenuTimer=6;
	i=0;
	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=6;	// Reset Function Menu timer
			gb_LCM_InverseDisp=1;
			switch(gc_KeyEvent)
			{
				case 0x16:	// L-M
					gc_MenuTimer=0;		// Abort
				break;

				case 0x03:	// Next
					gb_ADJ=0;
					switch(i)
					{
						case 0:	// Year(2010(0)~2039(29))
							gc_Year++;
							if(gc_Year>29)
							{
								gc_Year=0;
							}
							LCM_ShowYear(gc_Year);
						break;

						case 1:	// Month
							Month_Adj();
							LCM_ShowMonth(gc_Month);
							gb_LCM_InverseDisp=0;
							LCM_ShowDay(gc_Day);
						break;

						case 2: // Day
							Day_Adj();
							LCM_ShowDay(gc_Day);
						break;

						case 3:	// Hour
							gc_Hour++;
							if(gc_Hour==24)
							{
								gc_Hour=0;
							}
							LCM_ShowHour(gc_Hour);
						break;

						case 4:	// Minute
							gc_Min++;
							if(gc_Min==60)
							{
								gc_Min=0;
							}
							LCM_ShowMin(gc_Min);
						break;

						case 5:	// Second
							gc_Sec++;
							if(gc_Sec==60)
							{
								gc_Sec=0;
							}
							LCM_ShowSec(gc_Sec);
						break;
					}
				break;

				case 0x04:	// Prev
					gb_ADJ=1;
					switch(i)
					{
						case 0:	// Year(2010(0)~2039(29))
							if(gc_Year==0)
							{
								gc_Year=29;
							}
							else
							{
								gc_Year--;
							}
							LCM_ShowYear(gc_Year);
						break;

						case 1:	// Month
							Month_Adj();
							LCM_ShowMonth(gc_Month);
							gb_LCM_InverseDisp=0;
							LCM_ShowDay(gc_Day);
						break;

						case 2:	// Day
							Day_Adj1();
							LCM_ShowDay(gc_Day);
						break;
		
						case 3:	// Hour
							gc_Hour--;
							if(gc_Hour==-1)
							{
								gc_Hour=23;
							}
							LCM_ShowHour(gc_Hour);
						break;

						case 4:	// Minute
							gc_Min--;
							if(gc_Min==-1)
							{
								gc_Min=59;
							}
							LCM_ShowMin(gc_Min);
						break;

						case 5:	// Second
							gc_Sec--;
							if(gc_Sec==-1)
							{
								gc_Sec=59;
							}
							LCM_ShowSec(gc_Sec);
						break;
					}
				break;

				case 0x06:	// M
					gb_LCM_InverseDisp=0;
					switch(i)
					{
						case 0:
							LCM_ShowYear(gc_Year);
						break;
						case 1:
							LCM_ShowMonth(gc_Month);
						break;
						case 2:
							LCM_ShowDay(gc_Day);
						break;
						case 3:
							LCM_ShowHour(gc_Hour);
						break;
						case 4:
							LCM_ShowMin(gc_Min);
						break;
					}
					i++;
					gb_LCM_InverseDisp=1;
					switch(i)
					{
						case 1:
							LCM_ShowMonth(gc_Month);
						break;
						case 2:
							LCM_ShowDay(gc_Day);
						break;
						case 3:
							LCM_ShowHour(gc_Hour);
						break;
						case 4:
							LCM_ShowMin(gc_Min);
						break;
						case 5:
							LCM_ShowSec(gc_Sec);
						break;
						case 6:
							gc_LCM_line=0;
							gb_LCM_InverseDisp=0;
							// Set into RTC
							tw_Days=gc_Day-1;
							gc_Month--;
							while(gc_Month)
							{
								gc_Month--;
								tw_Days+=K_Month[gc_Month];
							}
							while(gc_Year)
							{
								gc_Year--;
								tw_Days+=K_Year[gc_Year&0x03];
							}
							if(((gc_Year+2)&3==0)&&(gc_Month>2))
							{
								tw_Days++;
							}

							tdw_Seconds=gc_Sec;
							tdw_Seconds+=(U16)gc_Min*60;
							tdw_Seconds+=(U32)gc_Hour*3600;
							tdw_Seconds+=(U32)tw_Days*86400;
							RTC_Write(0x00,0x0D);	// enable RTC, load mode
							RTC_Write(0x10,0xFF);
							RTC_DisableInterrupt();
							if(tdw_Seconds&0x01)
							{
								RTC_Write(0x11,0xF0);
							}
							else
							{
								RTC_Write(0x11,0x70);
							}
							tdw_Seconds=tdw_Seconds>>1;
							RTC_Write(0x12,(U8)tdw_Seconds);
							tdw_Seconds=tdw_Seconds>>8;
							RTC_Write(0x13,(U8)tdw_Seconds);
							tdw_Seconds=tdw_Seconds>>8;
							RTC_Write(0x14,(U8)tdw_Seconds);
							tdw_Seconds=tdw_Seconds>>8;
							RTC_Write(0x15,(U8)tdw_Seconds);
							RTC_Write(0xB0,0x01);	// load start value
							RTC_Write(0x00,0x05);
							RTC_EnableInterrupt();
							LCM_Clear_L0_L7();
							gc_Task_Current=C_Task_Setup;
							return;
						break;
					}				
				break;
			}
		}

		if(gb_AlarmTrigger==1)
		{
			if(CheckAlarmSetting()==0)	// Match Alarm Clock Setting
			{
				gc_Task_Last=C_Task_SetClock;
				gc_Task_Current=C_Task_AlarmON;
			}
		}

		if(gc_MenuTimer==0)
		{
			gc_Task_Current=C_Task_Idle;
			gc_LCM_line=0;
			gb_LCM_InverseDisp=0;
			break;
		}
	}
}


