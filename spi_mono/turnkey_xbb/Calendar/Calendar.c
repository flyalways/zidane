#include "..\Header\SPDA2K.h"
#include "..\header\variables.h"
#ifdef SPI_1M
void DisplayCalendar(void)
{
	data	U8	tc_temp;

	RTC_Calendar();	// RTC seconds to Calendar(YY-MM-DD HH:MM:SS)
	gc_Year+=100;	// RTC year=2010~2039, Calendar year=1910~2099(Base offset 100)
	LCM_ShowCalendarFrame();
	gc_MenuTimer=10;
	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=10;
			switch(gc_KeyEvent)
			{
				case 0x03:	// Next
					gb_ADJ=0;
					Day_Adj();
					LCM_ShowCalendarDay();
					tc_temp=gc_Week;
					if(gb_AdjOver)
					{
						Month_Adj();
						LCM_ShowCalendarMonth();
						if(gb_AdjOver)
						{
							Year_Adj();
							LCM_ShowCalendarYear();
						}
					}
					gc_Week=tc_temp;
					LCM_ShowCalendarWeek();
					if(gc_CurrentHZK==LANG_CHS)
					{
						Get_Lunar(gc_Year+1910,gc_Month,gc_Day);
						LCM_ShowLunar();
					}
				break;

				case 0x04:	// Prev
					gb_ADJ=1;
					Day_Adj();
					LCM_ShowCalendarDay();
					tc_temp=gc_Week;
					if(gb_AdjOver)
					{
						Month_Adj();
						LCM_ShowCalendarMonth();
						if(gb_AdjOver)
						{
							Year_Adj();
							LCM_ShowCalendarYear();
						}
					}
					gc_Week=tc_temp;
					LCM_ShowCalendarWeek();
					if(gc_CurrentHZK==LANG_CHS)
					{
						Get_Lunar(gc_Year+1910,gc_Month,gc_Day);
						LCM_ShowLunar();
					}
				break;

				case 0x13:	// L-Next
					gc_LongKeyCount=20;
					gb_ADJ=0;
					Month_Adj();
					LCM_ShowCalendarDay();
					LCM_ShowCalendarMonth();
					if(gb_AdjOver)
					{
						Year_Adj();
						LCM_ShowCalendarYear();
					}
					LCM_ShowCalendarWeek();
					if(gc_CurrentHZK==LANG_CHS)
					{
						Get_Lunar(gc_Year+1910,gc_Month,gc_Day);
						LCM_ShowLunar();
					}
				break;

				case 0x14:	// L-Prev
					gc_LongKeyCount=20;
					gb_ADJ=1;
					Month_Adj();
					LCM_ShowCalendarDay();
					LCM_ShowCalendarMonth();
					if(gb_AdjOver)
					{
						Year_Adj();
						LCM_ShowCalendarYear();
					}
					LCM_ShowCalendarWeek();
					if(gc_CurrentHZK==LANG_CHS)
					{
						Get_Lunar(gc_Year+1910,gc_Month,gc_Day);
						LCM_ShowLunar();
					}
				break;

				case 0x06:	// M
					gc_MenuTimer=0;	// Exit
				break;
			}
		}

		if(gb_RTC_wakeup)
		{
			gb_RTC_wakeup=0;
			gc_Sec++;
			if(gc_Sec==60)
			{
				gc_Sec=0;
			}
			LCM_ShowCalendarSecond();
			if(gc_Sec==0)
			{
				gc_Min++;
				LCM_ShowCalendarMinute();
				if(gc_Min>59)
				{
					gc_Hour++;
					if(gc_Hour>23)
					{
						gc_Hour=0;
					}
					LCM_ShowCalendarHour();
				}
			}
		}

		if(gc_MenuTimer==0)
		{
			gc_Task_Current=C_Task_Idle;
			return;
		}

		if(gb_OrderRecordTrigger==1)
		{
			gc_Task_Current=C_Task_FM;
			return;
		}
	}
}

#else
void DisplayCalendar(void)
{
	data	U8	tc_temp;

	RTC_Calendar();	// RTC seconds to Calendar(YY-MM-DD HH:MM:SS)
	gc_Year+=100;	// RTC year=2010~2039, Calendar year=1910~2099(Base offset 100)
	LCM_ShowCalendarFrame();
	gc_MenuTimer=10;
	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=10;
			switch(gc_KeyEvent)
			{
				case 0x03:	// Next
					gb_ADJ=0;
					Day_Adj();
					LCM_ShowCalendarDay();
					tc_temp=gc_Week;
					if(gb_AdjOver)
					{
						Month_Adj();
						LCM_ShowCalendarMonth();
						if(gb_AdjOver)
						{
							Year_Adj();
							LCM_ShowCalendarYear();
						}
					}
					gc_Week=tc_temp;
					LCM_ShowCalendarWeek();
					if(gc_CurrentHZK==LANG_CHS||gc_CurrentHZK==LANG_CHT)
					{
						Get_Lunar(gc_Year+1910,gc_Month,gc_Day);
						LCM_ShowLunar();
					}
				break;

				case 0x04:	// Prev
					gb_ADJ=1;
					Day_Adj();
					LCM_ShowCalendarDay();
					tc_temp=gc_Week;
					if(gb_AdjOver)
					{
						Month_Adj();
						LCM_ShowCalendarMonth();
						if(gb_AdjOver)
						{
							Year_Adj();
							LCM_ShowCalendarYear();
						}
					}
					gc_Week=tc_temp;
					LCM_ShowCalendarWeek();
					if(gc_CurrentHZK==LANG_CHS||gc_CurrentHZK==LANG_CHT)
					{
						Get_Lunar(gc_Year+1910,gc_Month,gc_Day);
						LCM_ShowLunar();
					}
				break;

				case 0x13:	// L-Next
					gc_LongKeyCount=20;
					gb_ADJ=0;
					Month_Adj();
					LCM_ShowCalendarDay();
					LCM_ShowCalendarMonth();
					if(gb_AdjOver)
					{
						Year_Adj();
						LCM_ShowCalendarYear();
					}
					LCM_ShowCalendarWeek();
					if(gc_CurrentHZK==LANG_CHS||gc_CurrentHZK==LANG_CHT)
					{
						Get_Lunar(gc_Year+1910,gc_Month,gc_Day);
						LCM_ShowLunar();
					}
				break;

				case 0x14:	// L-Prev
					gc_LongKeyCount=20;
					gb_ADJ=1;
					Month_Adj();
					LCM_ShowCalendarDay();
					LCM_ShowCalendarMonth();
					if(gb_AdjOver)
					{
						Year_Adj();
						LCM_ShowCalendarYear();
					}
					LCM_ShowCalendarWeek();
					if(gc_CurrentHZK==LANG_CHS||gc_CurrentHZK==LANG_CHT)
					{
						Get_Lunar(gc_Year+1910,gc_Month,gc_Day);
						LCM_ShowLunar();
					}
				break;

				case 0x06:	// M
					gc_MenuTimer=0;	// Exit
				break;
			}
		}

		if(gb_RTC_wakeup)
		{
			gb_RTC_wakeup=0;
			gc_Sec++;
			if(gc_Sec==60)
			{
				gc_Sec=0;
			}
			LCM_ShowCalendarSecond();
			if(gc_Sec==0)
			{
				gc_Min++;
				LCM_ShowCalendarMinute();
				if(gc_Min>59)
				{
					gc_Hour++;
					if(gc_Hour>23)
					{
						gc_Hour=0;
					}
					LCM_ShowCalendarHour();
				}
			}
		}

		if(gc_MenuTimer==0)
		{
			gc_Task_Current=C_Task_Idle;
			return;
		}

		if(gb_OrderRecordTrigger==1)
		{
			gc_Task_Current=C_Task_FM;
			return;
		}
	}
}

#endif
void Year_Adj(void)
{
	if(gb_ADJ==0)
	{
		gc_Year++;
		if(gc_Year>179)
		{
			gc_Year=0;
		}
	}
	else
	{
		if(gc_Year==0)
		{
			gc_Year=179;
		}
		else
		{
			gc_Year--;
		}
	}
}

void Month_Adj(void)
{
	xdata	U8	tc_x;

	tc_x=0;
	gb_AdjOver=0;
	if(((gc_Year+2)&0x03)==0)
	{
		tc_x=1;
	}

	if(gb_ADJ==0)
	{
		gc_Month++;
		if(gc_Month>12)
		{
			gc_Month=1;
			gb_AdjOver=1;
		}
		if(gc_Month==3)
		{
			gc_Week+=tc_x;
		}
		else if(gc_Month==5||gc_Month==7||gc_Month==10||gc_Month==12)
		{
			gc_Week+=2;
		}
		else
		{
			gc_Week+=3;
		}
	}
	else
	{
		gc_Month--;
		if(gc_Month==0)
		{
			gc_Month=12;
			gb_AdjOver=1;
		}

		if(gc_Month==2)
		{
			gc_Week+=tc_x;
		}
		else if(gc_Month==4||gc_Month==6||gc_Month==9||gc_Month==11)
		{
			gc_Week+=5;
		}
		else
		{
			gc_Week+=4;
		}
	}

	if(gc_Month==2)
	{
		if(gc_Day>(28+tc_x))
		{
			gc_Week=gc_Week+7-(gc_Day-28-tc_x);
			gc_Day=28+tc_x;
		}
	}
	else if(gc_Month==4||gc_Month==6||gc_Month==9||gc_Month==11)
	{
		if(gc_Day>30)
		{
			gc_Week=gc_Week+6;
			gc_Day=30;
		}
	}
	gc_Week%=7;
}


void Day_Adj(void)
{
	xdata	U8 tc_x;

	tc_x=0;
	gb_AdjOver=0;

	if(((gc_Year+2)%4)==0)
	{
		tc_x=1;
	}

	if(gb_ADJ==0)	// INC
	{
		gc_Day++;
		if(gc_Month==2)
		{
			if(gc_Day>28+tc_x)
			{
				gc_Day=1;
				gb_AdjOver=1;
			}
		}
		else if(gc_Month==4||gc_Month==6||gc_Month==9||gc_Month==11)
		{
			if(gc_Day>30)
			{
				gc_Day=1;
				gb_AdjOver=1;
			}
		}
		else
		{
			if(gc_Day>31)
			{
				gc_Day=1;
				gb_AdjOver=1;
			}
		}

		if(gc_Week==6)
		{
			gc_Week=0;
		}
		else
		{
			gc_Week++;
		}
	}
	else
	{
		gc_Day--;
		if(gc_Day==0)
		{
			gb_AdjOver=1;
			if(gc_Month==3)
			{
				gc_Day=28+tc_x;
			}
			else if(gc_Month==5||gc_Month==7||gc_Month==10||gc_Month==12)
			{
				gc_Day=30;
			}
			else
			{
				gc_Day=31;
			}
		}

		if(gc_Week==0)
		{
			gc_Week=6;
		}
		else
		{
			gc_Week--;
		}
	}
}

void Day_Adj1(void)
{
	xdata	U8 tc_x;

	tc_x=0;

	if(((gc_Year+2)%4)==0)
	{
		tc_x=1;
	}

	gc_Day--;
	if(gc_Day==0)
	{
		if(gc_Month==2)
		{
			gc_Day=28+tc_x;
		}
		else if(gc_Month==4||gc_Month==6||gc_Month==9||gc_Month==11)
		{
			gc_Day=30;
		}
		else
		{
			gc_Day=31;
		}
	}
}


