#include "..\Header\SPDA2K.h"
#include "..\header\UI_config.h"
#include "..\header\variables.h"
#include "..\LCM\LCM_BMP.h"

void LCM_ShowYear(U8 tc_Year)
{
	gc_LCM_line=3;
	tc_Year+=10;
	LCM_ShowChar8x16(2,24);	// "2"
	LCM_ShowChar8x16(0,32);	// "0"
	LCM_ShowChar8x16(tc_Year/10,40);
	LCM_ShowChar8x16(tc_Year%10,48);
}


void LCM_ShowMonth(U8 tc_Month)
{
	gc_LCM_line=3;
	LCM_ShowChar8x16(tc_Month/10,64);
	LCM_ShowChar8x16(tc_Month%10,72);
}


void LCM_ShowDay(U8 tc_Day)
{
	gc_LCM_line=3;
	LCM_ShowChar8x16(tc_Day/10,88);
	LCM_ShowChar8x16(tc_Day%10,96);
}


void LCM_ShowHour(U8 tc_Hour)
{
	gc_LCM_line=5;
	LCM_ShowChar8x16(tc_Hour/10,32);
	LCM_ShowChar8x16(tc_Hour%10,40);
}


void LCM_ShowMin(U8 tc_Min)
{
	gc_LCM_line=5;
	LCM_ShowChar8x16(tc_Min/10,56);
	LCM_ShowChar8x16(tc_Min%10,64);
}


void LCM_ShowSec(U8 tc_Sec)
{
	gc_LCM_line=5;
	LCM_ShowChar8x16(tc_Sec/10,80);
	LCM_ShowChar8x16(tc_Sec%10,88);
}

void LCM_ShowSetupMenu(U8 tc_item)
{
	data	U8	i;
	xdata	U8	*p;

	switch(tc_item)
	{
		case 0:	// ¨t²Î®É¶¡
			p=gc_Str_Clock[gc_CurrentHZK];
		break;

		case 1:	// ­I¥ú®É¶¡
			p=gc_Str_Backlight[gc_CurrentHZK];
		break;

		case 2:	// »y¨¥
			p=gc_Str_Language[gc_CurrentHZK];
		break;

		case 3:	// ¾xÄÁ
			p=gc_Str_AlarmClock[gc_CurrentHZK];
		break;

		case 4:	// °h¥X
			p=gc_Str_Exit[gc_CurrentHZK];
		break;
	}

	if(tc_item==gc_SetupMenuNum)
	{
		gb_LCM_InverseDisp=1;
	}

	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,(tc_item%4)*16,1);
	gb_LCM_InverseDisp=0;
}

void LCM_ShowTitle_SetClock(void)
{
	data	U8	i;
	xdata	U8	*p;

	p=gc_Str_Clock[gc_CurrentHZK];
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],32,0,1);
}

#ifdef SPI_1M
/////////////////////2M spi 2 language
void LCM_ShowCalendarFrame(void)
{
	LCM_Clear_L0_L7();
	if(gc_CurrentHZK==LANG_CHS)
	{
		gc_LrcDisplayBuf[0]=0xC4;	// ¦~
		gc_LrcDisplayBuf[1]=0xEA;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,32,0,0);
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,32,48,0);
		gc_LrcDisplayBuf[0]=0xD4;	// ¤ë
		gc_LrcDisplayBuf[1]=0xC2;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,64,0,0);
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,64,48,0);	
		gc_LrcDisplayBuf[0]=0xC8;	// ¤é
		gc_LrcDisplayBuf[1]=0xD5;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,96,0,0);
		gc_LrcDisplayBuf[0]=0xD0;	// ¬P´Á
		gc_LrcDisplayBuf[1]=0xC7;
		gc_LrcDisplayBuf[2]=0xC6;
		gc_LrcDisplayBuf[3]=0xDA;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,4,80,24,0);
		gc_LCM_line=3;
		gb_LCM_InverseDisp=1;
		LCM_ShowChar10x16(10,22);	// :
		LCM_ShowChar10x16(10,48);	// :
		gb_LCM_InverseDisp=0;
	}
	else
	{
		gc_LCM_line=3;
		gb_LCM_InverseDisp=1;
		LCM_ShowChar10x16(10,46);	// :
		LCM_ShowChar10x16(10,72);	// :
		gc_LCM_line=0;
		gb_LCM_InverseDisp=0;
		LCM_ShowChar8x16(11,56);	// -
		LCM_ShowChar8x16(11,80);	// -
	}
	LCM_ShowCalendarWeek();
	LCM_ShowCalendarYear();
	LCM_ShowCalendarMonth();
	LCM_ShowCalendarDay();
	LCM_ShowCalendarHour();
	LCM_ShowCalendarMinute();
	LCM_ShowCalendarSecond();
	if(gc_CurrentHZK==LANG_CHS)
	{
		Get_Lunar(gc_Year+1910,gc_Month,gc_Day);
		LCM_ShowLunar();
	}
}


void LCM_ShowCalendarYear(void)
{
	data	U8	tc_Offset=0;
	xdata	U8	tc_Year;

	tc_Year=gc_Year+10;
	if(gc_CurrentHZK!=LANG_CHS)
	{
		tc_Offset=24;
	}
	gc_LCM_line=0;
	gb_LCM_InverseDisp=0;
	if(tc_Year<100)
	{
		LCM_ShowChar8x16(1,tc_Offset);	// 1
		LCM_ShowChar8x16(9,8+tc_Offset);// 9
	}
	else
	{
		tc_Year-=100;
		LCM_ShowChar8x16(2,tc_Offset);	// 2
		LCM_ShowChar8x16(0,8+tc_Offset);// 0
	}
	LCM_ShowChar8x16(tc_Year/10,16+tc_Offset);
	LCM_ShowChar8x16(tc_Year%10,24+tc_Offset);
}


void LCM_ShowCalendarMonth(void)
{
	data U8	tc_Offset=0;

	if(gc_CurrentHZK!=LANG_CHS)
	{
		tc_Offset=16;
	}
	gc_LCM_line=0;
	gb_LCM_InverseDisp=0;
	LCM_ShowChar8x16(gc_Month/10,48+tc_Offset);
	LCM_ShowChar8x16(gc_Month%10,56+tc_Offset);
}


void LCM_ShowCalendarDay(void)
{
	data U8	tc_Offset=0;

	if(gc_CurrentHZK!=LANG_CHS)
	{
		tc_Offset=8;
	}
	gc_LCM_line=0;
	gb_LCM_InverseDisp=0;
	LCM_ShowChar8x16(gc_Day/10,80+tc_Offset);
	LCM_ShowChar8x16(gc_Day%10,88+tc_Offset);
}


void LCM_ShowCalendarHour(void)
{
	data U8	tc_Offset=0;

	if(gc_CurrentHZK!=LANG_CHS)
	{
		tc_Offset=24;
	}
	gc_LCM_line=3;
	gb_LCM_InverseDisp=1;
	LCM_ShowChar10x16(gc_Hour/10,4+tc_Offset);
	LCM_ShowChar10x16(gc_Hour%10,14+tc_Offset);
}

void LCM_ShowCalendarMinute(void)
{
	data U8	tc_Offset=0;

	if(gc_CurrentHZK!=LANG_CHS)
	{
		tc_Offset=24;
	}
	gc_LCM_line=3;
	gb_LCM_InverseDisp=1;
	LCM_ShowChar10x16(gc_Min/10,30+tc_Offset);
	LCM_ShowChar10x16(gc_Min%10,40+tc_Offset);
}

void LCM_ShowCalendarSecond(void)
{
	data U8	tc_Offset=0;

	if(gc_CurrentHZK!=LANG_CHS)
	{
		tc_Offset=24;
	}
	gc_LCM_line=3;
	gb_LCM_InverseDisp=1;
	LCM_ShowChar10x16(gc_Sec/10,56+tc_Offset);
	LCM_ShowChar10x16(gc_Sec%10,66+tc_Offset);
}

void LCM_ShowCalendarWeek(void)
{
	data	U8	i;
	xdata	U8	tc_w;
	xdata	U8	*p;

	switch(gc_Week)
	{
		case 0:	// Monday
			p=gc_Str_Monday[gc_CurrentHZK];
		break;

		case 1:	// Tuesday
			p=gc_Str_Tuesday[gc_CurrentHZK];
		break;

		case 2:	// Wednesday
			p=gc_Str_Wednesday[gc_CurrentHZK];
		break;

		case 3:	// Thursday
			p=gc_Str_Thursday[gc_CurrentHZK];
		break;

		case 4:	// Friday
			p=gc_Str_Friday[gc_CurrentHZK];
		break;

		case 5:	// Saturday
			p=gc_Str_Saturday[gc_CurrentHZK];
		break;

		case 6:	// Sunday
			p=gc_Str_Sunday[gc_CurrentHZK];
		break;
	}

	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}

	if(gc_CurrentHZK==LANG_CHS)
	{
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],112,24,0);
	}
	else
	{
		LCM_Clear_L6_L7();	
		tc_w=4;	   
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],(128-tc_w*p[0])/2,48,0);
	}
}


void LCM_ShowLunar(void)
{
	xdata	U8	tc_temp;
	// ¥Ò¤A¤þ¤B¥³¤v©°¨¯¤Ð¬Ñ
	code U8 K_Str_DiZhi[]={0xBC,0xD7,0xD2,0xD2,0xB1,0xFB,0xB6,0xA1,0xCE,0xEC,0xBC,0xBA,0xB8,0xFD,0xD0,0xC1,0xC8,0xC9,0xB9,0xEF};	
	// ¤l¤¡±G¥f¨°¤x¤È¥¼¥Ó¨»¦¦¥è
	code U8 K_Str_Tiangan[]={0xD7,0xD3,0xB3,0xF3,0xD2,0xFA,0xC3,0xAE,0xB3,0xBD,0xCB,0xC8,0xCE,0xE7,0xCE,0xB4,0xC9,0xEA,0xD3,0xCF,0xD0,0xE7,0xBA,0xA5};	
	// ¥¿¤G¤T¥|¤­¤»¤C¤K¤E¤Q¥VÃ¾
	code U8 K_Str_LunarMonth[]={0xD5,0xFD,0xB6,0xFE,0xC8,0xFD,0xCB,0xC4,0xCE,0xE5,0xC1,0xF9,0xC6,0xDF,0xB0,0xCB,0xBE,0xC5,0xCA,0xAE,0xB6,0xAC,0xC0,0xB0};

	// Display Lunar Year
	gb_LCM_InverseDisp=0;
	gc_LrcDisplayBuf[0]=K_Str_DiZhi[gc_Dizhi*2+0];
	gc_LrcDisplayBuf[1]=K_Str_DiZhi[gc_Dizhi*2+1];
	gc_LrcDisplayBuf[2]=K_Str_Tiangan[gc_Tiangan*2+0];
	gc_LrcDisplayBuf[3]=K_Str_Tiangan[gc_Tiangan*2+1];
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,4,0,48,0);
	// Display Lunar Month
	gc_LrcDisplayBuf[0]=K_Str_LunarMonth[gc_Month_Lunar*2-2];
	gc_LrcDisplayBuf[1]=K_Str_LunarMonth[gc_Month_Lunar*2-1]; 
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,48,48,0);
	// Display Lunar Day
	if(gc_Day_Lunar%10==0)
	{
		if(gc_Day_Lunar/10==1)
		{
			gc_LrcDisplayBuf[0]=0xb3;	// ªì
			gc_LrcDisplayBuf[1]=0xf5;
		}
		else if(gc_Day_Lunar/10==2)
		{
			gc_LrcDisplayBuf[0]=0xb6;	// ¤G
			gc_LrcDisplayBuf[1]=0xfe;
		}
		else
		{
			gc_LrcDisplayBuf[0]=0xc8;	// ¤T
			gc_LrcDisplayBuf[1]=0xfd;			
		}
		gc_LrcDisplayBuf[2]=0xca;	// ¤Q
		gc_LrcDisplayBuf[3]=0xae;			
	}
	else
	{
		if(gc_Day_Lunar/10==0)
		{
			gc_LrcDisplayBuf[0]=0xb3;	// ªì
			gc_LrcDisplayBuf[1]=0xf5;
		}
		else if(gc_Day_Lunar/10==1)
		{
			gc_LrcDisplayBuf[0]=0xca;	// ¤Q
			gc_LrcDisplayBuf[1]=0xae;			
		}
		else
		{
			gc_LrcDisplayBuf[0]=0xbb;	// ¤Ü
			gc_LrcDisplayBuf[1]=0xca;			
		}

		tc_temp=gc_Day_Lunar%10;
		if(tc_temp==1)
		{
			gc_LrcDisplayBuf[2]=0xd2;	// ¤@
			gc_LrcDisplayBuf[3]=0xbb;
		}
		else
		{
			gc_LrcDisplayBuf[2]=K_Str_LunarMonth[tc_temp*2-2];
			gc_LrcDisplayBuf[3]=K_Str_LunarMonth[tc_temp*2-1];
		}
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,4,80,48,0);
	// Display Leap-month
	if(gb_LeapMonth==1)
	{
		gb_LCM_InverseDisp=1;
		gc_LrcDisplayBuf[0]=0xc8;	// Èò
		gc_LrcDisplayBuf[1]=0xf2;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,112,48,0);
	}
	else
	{
		gc_LrcDisplayBuf[0]=0x20;
		gc_LrcDisplayBuf[1]=0x00;
		gc_LrcDisplayBuf[2]=0x20;
		gc_LrcDisplayBuf[3]=0x00;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,4,112,48,0);
	}
}

void LCM_ShowAlarmClockString(void)
{
	data	U8	i;
	xdata	U8	*p;

	p=gc_Str_AlarmClock[gc_CurrentHZK];
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}

	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,0,0);
}

void LCM_ShowTimeString(void)
{
	data	U8	i;
	xdata	U8	*p;

	p=gc_Str_Time[gc_CurrentHZK];
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}

	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,16,0);
}

void LCM_ShowRepeatString(void)
{
	data	U8	i;
	xdata	U8	*p;

	p=gc_Str_AlarmRepeat[gc_CurrentHZK];
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}

	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,32,0);
}

void LCM_ShowDaysString(U8 tc_Day)
{
	data	U8	i;
	data	U8	x;
	xdata	U8	*p;

	switch(tc_Day)
	{
		case 0:	// Monday
			p=gc_Str_Monday[gc_CurrentHZK];
		break;

		case 1:	// Tuesday
			p=gc_Str_Tuesday[gc_CurrentHZK];
		break;

		case 2:	// Wednesday
			p=gc_Str_Wednesday[gc_CurrentHZK];
		break;

		case 3:	// Thursday
			p=gc_Str_Thursday[gc_CurrentHZK];
		break;

		case 4:	// Friday
			p=gc_Str_Friday[gc_CurrentHZK];
		break;

		case 5:	// Saturday
			p=gc_Str_Saturday[gc_CurrentHZK];
		break;

		case 6:	// Sunday
			p=gc_Str_Sunday[gc_CurrentHZK];
		break;

		case 7:	// Exit
			p=gc_Str_Exit[gc_CurrentHZK];
			LCM_Clear_L6_L7();
		break;
	}
	if((gc_CurrentHZK==LANG_CHS)&&tc_Day!=7)
	{
		gc_LrcDisplayBuf[0]=0xD0;	// ¬P´Á
		gc_LrcDisplayBuf[1]=0xc7;
		gc_LrcDisplayBuf[2]=0xc6;
		gc_LrcDisplayBuf[3]=0xda;
		gc_LrcDisplayBuf[4]=p[1];
		gc_LrcDisplayBuf[5]=p[2];
		x=4;
	}
	else
	{
		x=0;
		for(i=0;i<p[0];i++)
		{
			gc_LrcDisplayBuf[i]=p[1+i];
		}
	}

	gb_LCM_InverseDisp=0;
	if(tc_Day==gc_AlarmRepeat_DaySelect)
	{
		gb_LCM_InverseDisp=1;
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0]+x,16,(tc_Day%4)*16,1);
}

void LCM_ShowDaysSelect(void)
{
	data	U8	i;
	data	U8	k;
	data	U8	tc_DaySelect;
	data	U8	tc_Count;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	gb_LCM_InverseDisp=0;
	tc_DaySelect=gc_AlarmDaysSelect;
	if(gc_AlarmRepeat_DaySelect<4)
	{
		tc_Count=4;
	}
	else
	{
		tc_Count=3;
		tc_DaySelect=tc_DaySelect>>4;
	}

	for(k=0;k<tc_Count;k++)
	{
		tw_Addr=Select16x16;
		if(tc_DaySelect&0x01)
		{
			tw_Addr+=32;
		}
		tc_DaySelect=tc_DaySelect>>1;
		LCM_ReadSPIBMPdata(tw_Addr,32);
		for(i=0;i<2;i++)
		{
			LCM_set_address(i+k*2,0);
			for(j=0;j<16;j++)
			{
				LCM_write_data(gc_DirLCMtempBuf[i*16+j]);
			}
		}
	}
}
	  

#else
/////////////////////////////////////////////
////////////////////////////////////////////
///////////////4M spi 17 lang
void LCM_ShowCalendarFrame(void)
{
	LCM_Clear_L0_L7();
	if(gc_CurrentHZK==LANG_CHS||gc_CurrentHZK==LANG_CHT)
	{
		gc_LrcDisplayBuf[0]=0x74;	// ¦~
		gc_LrcDisplayBuf[1]=0x5E;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,32,0,0);
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,32,48,0);
		gc_LrcDisplayBuf[0]=0x08;	// ¤ë
		gc_LrcDisplayBuf[1]=0x67;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,64,0,0);
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,64,48,0);	
		gc_LrcDisplayBuf[0]=0xE5;	// ¤é
		gc_LrcDisplayBuf[1]=0x65;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,96,0,0);
		gc_LrcDisplayBuf[0]=0x1F;	// ¬P´Á
		gc_LrcDisplayBuf[1]=0x66;
		gc_LrcDisplayBuf[2]=0x1F;
		gc_LrcDisplayBuf[3]=0x67;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,4,80,24,0);
		gc_LCM_line=3;
		gb_LCM_InverseDisp=1;
		LCM_ShowChar10x16(10,22);	// :
		LCM_ShowChar10x16(10,48);	// :
		gb_LCM_InverseDisp=0;
	}
	else
	{
		gc_LCM_line=3;
		gb_LCM_InverseDisp=1;
		LCM_ShowChar10x16(10,46);	// :
		LCM_ShowChar10x16(10,72);	// :
		gc_LCM_line=0;
		gb_LCM_InverseDisp=0;
		LCM_ShowChar8x16(11,56);	// -
		LCM_ShowChar8x16(11,80);	// -
	}
	LCM_ShowCalendarWeek();
	LCM_ShowCalendarYear();
	LCM_ShowCalendarMonth();
	LCM_ShowCalendarDay();
	LCM_ShowCalendarHour();
	LCM_ShowCalendarMinute();
	LCM_ShowCalendarSecond();
	if(gc_CurrentHZK==LANG_CHS||gc_CurrentHZK==LANG_CHT)
	{
		Get_Lunar(gc_Year+1910,gc_Month,gc_Day);
		LCM_ShowLunar();
	}
}


void LCM_ShowCalendarYear(void)
{
	data	U8	tc_Offset=0;
	xdata	U8	tc_Year;

	tc_Year=gc_Year+10;
	if(gc_CurrentHZK!=LANG_CHS&&gc_CurrentHZK!=LANG_CHT)
	{
		tc_Offset=24;
	}
	gc_LCM_line=0;
	gb_LCM_InverseDisp=0;
	if(tc_Year<100)
	{
		LCM_ShowChar8x16(1,tc_Offset);	// 1
		LCM_ShowChar8x16(9,8+tc_Offset);// 9
	}
	else
	{
		tc_Year-=100;
		LCM_ShowChar8x16(2,tc_Offset);	// 2
		LCM_ShowChar8x16(0,8+tc_Offset);// 0
	}
	LCM_ShowChar8x16(tc_Year/10,16+tc_Offset);
	LCM_ShowChar8x16(tc_Year%10,24+tc_Offset);
}


void LCM_ShowCalendarMonth(void)
{
	data U8	tc_Offset=0;

	if(gc_CurrentHZK!=LANG_CHS&&gc_CurrentHZK!=LANG_CHT)
	{
		tc_Offset=16;
	}
	gc_LCM_line=0;
	gb_LCM_InverseDisp=0;
	LCM_ShowChar8x16(gc_Month/10,48+tc_Offset);
	LCM_ShowChar8x16(gc_Month%10,56+tc_Offset);
}


void LCM_ShowCalendarDay(void)
{
	data U8	tc_Offset=0;

	if(gc_CurrentHZK!=LANG_CHS&&gc_CurrentHZK!=LANG_CHT)
	{
		tc_Offset=8;
	}
	gc_LCM_line=0;
	gb_LCM_InverseDisp=0;
	LCM_ShowChar8x16(gc_Day/10,80+tc_Offset);
	LCM_ShowChar8x16(gc_Day%10,88+tc_Offset);
}


void LCM_ShowCalendarHour(void)
{
	data U8	tc_Offset=0;

	if(gc_CurrentHZK!=LANG_CHS&&gc_CurrentHZK!=LANG_CHT)
	{
		tc_Offset=24;
	}
	gc_LCM_line=3;
	gb_LCM_InverseDisp=1;
	LCM_ShowChar10x16(gc_Hour/10,4+tc_Offset);
	LCM_ShowChar10x16(gc_Hour%10,14+tc_Offset);
}

void LCM_ShowCalendarMinute(void)
{
	data U8	tc_Offset=0;

	if(gc_CurrentHZK!=LANG_CHS&&gc_CurrentHZK!=LANG_CHT)
	{
		tc_Offset=24;
	}
	gc_LCM_line=3;
	gb_LCM_InverseDisp=1;
	LCM_ShowChar10x16(gc_Min/10,30+tc_Offset);
	LCM_ShowChar10x16(gc_Min%10,40+tc_Offset);
}

void LCM_ShowCalendarSecond(void)
{
	data U8	tc_Offset=0;

	if(gc_CurrentHZK!=LANG_CHS&&gc_CurrentHZK!=LANG_CHT)
	{
		tc_Offset=24;
	}
	gc_LCM_line=3;
	gb_LCM_InverseDisp=1;
	LCM_ShowChar10x16(gc_Sec/10,56+tc_Offset);
	LCM_ShowChar10x16(gc_Sec%10,66+tc_Offset);
}

void LCM_ShowCalendarWeek(void)
{
	data	U8	i;
	xdata	U8	tc_w;
	xdata	U8	*p;

	switch(gc_Week)
	{
		case 0:	// Monday
			p=gc_Str_Monday[gc_CurrentHZK];
		break;

		case 1:	// Tuesday
			p=gc_Str_Tuesday[gc_CurrentHZK];
		break;

		case 2:	// Wednesday
			p=gc_Str_Wednesday[gc_CurrentHZK];
		break;

		case 3:	// Thursday
			p=gc_Str_Thursday[gc_CurrentHZK];
		break;

		case 4:	// Friday
			p=gc_Str_Friday[gc_CurrentHZK];
		break;

		case 5:	// Saturday
			p=gc_Str_Saturday[gc_CurrentHZK];
		break;

		case 6:	// Sunday
			p=gc_Str_Sunday[gc_CurrentHZK];
		break;
	}

	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}

	if(gc_CurrentHZK==LANG_CHS||gc_CurrentHZK==LANG_CHT)
	{
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],112,24,0);
	}
	else
	{
		LCM_Clear_L6_L7();
		if(gc_CurrentHZK==LANG_JAP||gc_CurrentHZK==LANG_KOR)
		{
			tc_w=8;
		}
		else
		{
			tc_w=4;
		}
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],(128-tc_w*p[0])/2,48,0);
	}
}


void LCM_ShowLunar(void)
{
	xdata	U8	tc_temp;
	// ¥Ò¤A¤þ¤B¥³¤v©°¨¯¤Ð¬Ñ
	code U8 K_Str_DiZhi[]={0x32,0x75,0x59,0x4E,0x19,0x4E,0x01,0x4E,0x0A,0x62,0xF1,0x5D,0x9A,0x5E,0x9B,0x8F,0xEC,0x58,0x78,0x76};	
	// ¤l¤¡±G¥f¨°¤x¤È¥¼¥Ó¨»¦¦¥è
	code U8 K_Str_Tiangan[]={0x50,0x5B,0x11,0x4E,0xC5,0x5B,0x6F,0x53,0xB0,0x8F,0xF3,0x5D,0x48,0x53,0x2A,0x67,0x33,0x75,0x49,0x91,0x0C,0x62,0xA5,0x4E};	
	// ¥¿¤G¤T¥|¤­¤»¤C¤K¤E¤Q¥VÃ¾
	code U8 K_Str_LunarMonth[]={0x63,0x6B,0x8C,0x4E,0x09,0x4E,0xDB,0x56,0x94,0x4E,0x6D,0x51,0x03,0x4E,0x6B,0x51,0x5D,0x4E,0x41,0x53,0xAC,0x51,0xD8,0x81};

	// Display Lunar Year
	gb_LCM_InverseDisp=0;
	gc_LrcDisplayBuf[0]=K_Str_DiZhi[gc_Dizhi*2+0];
	gc_LrcDisplayBuf[1]=K_Str_DiZhi[gc_Dizhi*2+1];
	gc_LrcDisplayBuf[2]=K_Str_Tiangan[gc_Tiangan*2+0];
	gc_LrcDisplayBuf[3]=K_Str_Tiangan[gc_Tiangan*2+1];
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,4,0,48,0);
	// Display Lunar Month
	if(gc_Month_Lunar==12&&gc_CurrentHZK==LANG_CHS)
	{
		gc_LrcDisplayBuf[0]=0x4A;	// ÛK
		gc_LrcDisplayBuf[1]=0x81;
	}
	else
	{
		gc_LrcDisplayBuf[0]=K_Str_LunarMonth[gc_Month_Lunar*2-2];
		gc_LrcDisplayBuf[1]=K_Str_LunarMonth[gc_Month_Lunar*2-1];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,48,48,0);
	// Display Lunar Day
	if(gc_Day_Lunar%10==0)
	{
		if(gc_Day_Lunar/10==1)
		{
			gc_LrcDisplayBuf[0]=0x1D;	// ªì
			gc_LrcDisplayBuf[1]=0x52;
		}
		else if(gc_Day_Lunar/10==2)
		{
			gc_LrcDisplayBuf[0]=0x8C;	// ¤G
			gc_LrcDisplayBuf[1]=0x4E;
		}
		else
		{
			gc_LrcDisplayBuf[0]=0x09;	// ¤T
			gc_LrcDisplayBuf[1]=0x4E;			
		}
		gc_LrcDisplayBuf[2]=0x41;	// ¤Q
		gc_LrcDisplayBuf[3]=0x53;			
	}
	else
	{
		if(gc_Day_Lunar/10==0)
		{
			gc_LrcDisplayBuf[0]=0x1D;	// ªì
			gc_LrcDisplayBuf[1]=0x52;
		}
		else if(gc_Day_Lunar/10==1)
		{
			gc_LrcDisplayBuf[0]=0x41;	// ¤Q
			gc_LrcDisplayBuf[1]=0x53;			
		}
		else
		{
			gc_LrcDisplayBuf[0]=0xFF;	// ¤Ü
			gc_LrcDisplayBuf[1]=0x5E;			
		}

		tc_temp=gc_Day_Lunar%10;
		if(tc_temp==1)
		{
			gc_LrcDisplayBuf[2]=0x00;	// ¤@
			gc_LrcDisplayBuf[3]=0x4E;
		}
		else
		{
			gc_LrcDisplayBuf[2]=K_Str_LunarMonth[tc_temp*2-2];
			gc_LrcDisplayBuf[3]=K_Str_LunarMonth[tc_temp*2-1];
		}
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,4,80,48,0);
	// Display Leap-month
	if(gb_LeapMonth==1)
	{
		gb_LCM_InverseDisp=1;
		if(gc_CurrentHZK==LANG_CHS)
		{
			gc_LrcDisplayBuf[0]=0xF0;	// …z
			gc_LrcDisplayBuf[1]=0x95;
		}
		else	// CHT
		{
			gc_LrcDisplayBuf[0]=0x8F;	// ¶|
			gc_LrcDisplayBuf[1]=0x95;
		}
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,2,112,48,0);
	}
	else
	{
		gc_LrcDisplayBuf[0]=0x20;
		gc_LrcDisplayBuf[1]=0x00;
		gc_LrcDisplayBuf[2]=0x20;
		gc_LrcDisplayBuf[3]=0x00;
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,4,112,48,0);
	}
}

void LCM_ShowAlarmClockString(void)
{
	data	U8	i;
	xdata	U8	*p;

	p=gc_Str_AlarmClock[gc_CurrentHZK];
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}

	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,0,0);
}

void LCM_ShowTimeString(void)
{
	data	U8	i;
	xdata	U8	*p;

	p=gc_Str_Time[gc_CurrentHZK];
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}

	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,16,0);
}

void LCM_ShowRepeatString(void)
{
	data	U8	i;
	xdata	U8	*p;

	p=gc_Str_AlarmRepeat[gc_CurrentHZK];
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}

	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,32,0);
}

void LCM_ShowDaysString(U8 tc_Day)
{
	data	U8	i;
	data	U8	x;
	xdata	U8	*p;

	switch(tc_Day)
	{
		case 0:	// Monday
			p=gc_Str_Monday[gc_CurrentHZK];
		break;

		case 1:	// Tuesday
			p=gc_Str_Tuesday[gc_CurrentHZK];
		break;

		case 2:	// Wednesday
			p=gc_Str_Wednesday[gc_CurrentHZK];
		break;

		case 3:	// Thursday
			p=gc_Str_Thursday[gc_CurrentHZK];
		break;

		case 4:	// Friday
			p=gc_Str_Friday[gc_CurrentHZK];
		break;

		case 5:	// Saturday
			p=gc_Str_Saturday[gc_CurrentHZK];
		break;

		case 6:	// Sunday
			p=gc_Str_Sunday[gc_CurrentHZK];
		break;

		case 7:	// Exit
			p=gc_Str_Exit[gc_CurrentHZK];
			LCM_Clear_L6_L7();
		break;
	}
	if((gc_CurrentHZK==LANG_CHS||gc_CurrentHZK==LANG_CHT)&&tc_Day!=7)
	{
		gc_LrcDisplayBuf[0]=0x1F;	// ¬P´Á
		gc_LrcDisplayBuf[1]=0x66;
		gc_LrcDisplayBuf[2]=0x1F;
		gc_LrcDisplayBuf[3]=0x67;
		gc_LrcDisplayBuf[4]=p[1];
		gc_LrcDisplayBuf[5]=p[2];
		x=4;
	}
	else
	{
		x=0;
		for(i=0;i<p[0];i++)
		{
			gc_LrcDisplayBuf[i]=p[1+i];
		}
	}

	gb_LCM_InverseDisp=0;
	if(tc_Day==gc_AlarmRepeat_DaySelect)
	{
		gb_LCM_InverseDisp=1;
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0]+x,16,(tc_Day%4)*16,1);
}

void LCM_ShowDaysSelect(void)
{
	data	U8	i;
	data	U8	k;
	data	U8	tc_DaySelect;
	data	U8	tc_Count;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	gb_LCM_InverseDisp=0;
	tc_DaySelect=gc_AlarmDaysSelect;
	if(gc_AlarmRepeat_DaySelect<4)
	{
		tc_Count=4;
	}
	else
	{
		tc_Count=3;
		tc_DaySelect=tc_DaySelect>>4;
	}

	for(k=0;k<tc_Count;k++)
	{
		tw_Addr=Select16x16;
		if(tc_DaySelect&0x01)
		{
			tw_Addr+=32;
		}
		tc_DaySelect=tc_DaySelect>>1;
		LCM_ReadSPIBMPdata(tw_Addr,32);
		for(i=0;i<2;i++)
		{
			LCM_set_address(i+k*2,0);
			for(j=0;j<16;j++)
			{
				LCM_write_data(gc_DirLCMtempBuf[i*16+j]);
			}
		}
	}
}
#endif