#include "..\Header\SPDA2K.h"
#include "..\header\UI_config.h"
#include "..\header\variables.h"
#include "..\LCM\LCM_BMP.h"

void LCM_ShowRepeatMenu(U8 tc_item)
{
	data	U8	i;
	xdata	U8	*p;

	switch(tc_item)
	{
		case 0:	// 全部循環
			p=gc_Str_RepeatAll[gc_CurrentHZK];
		break;

		case 1:	// 普通播放
			p=gc_Str_PlayAll[gc_CurrentHZK];
		break;

		case 2:	// 單曲循環
			p=gc_Str_Repeat1[gc_CurrentHZK];
		break;

		case 3:	// 隨機播放
			p=gc_Str_Random[gc_CurrentHZK];
		break;

		case 4:	// 目錄循環
			p=gc_Str_RepeatFolder[gc_CurrentHZK];
		break;

		case 5:	// 目錄播放
			p=gc_Str_Folder[gc_CurrentHZK];
		break;
	}

	if(tc_item==gc_RepeatMenuNum)
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


void LCM_ShowEQMenu(U8 tc_item)
{
	data	U8	i;
	xdata	U8	*p;

	switch(tc_item)
	{
		case 0:	// NORMAL
			p=gc_Str_Natural[gc_CurrentHZK];
		break;

		case 1:	// DBB
			p=gc_Str_DBB[gc_CurrentHZK];
		break;

		case 2:	// JAZZ
			p=gc_Str_JAZZ[gc_CurrentHZK];
		break;

		case 3:	// ROCK
			p=gc_Str_ROCK[gc_CurrentHZK];
		break;

		case 4:	// POP
			p=gc_Str_POP[gc_CurrentHZK];
		break;

		case 5:	// Classic
			p=gc_Str_Classic[gc_CurrentHZK];
		break;
	}
	if(tc_item==gs_DSP_GLOBAL_RAM.sc_EQ_Type)
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

void LCM_ShowFMMenu(U8 tc_item)
{
	data	U8	i;
	xdata	U8	*p;

	switch(tc_item)
	{
		case 0:	// Record
			p=gc_Str_Record[gc_CurrentHZK];
		break;

		case 1:	// Save channel
			p=gc_Str_SaveCH[gc_CurrentHZK];
		break;

		case 2:	// Delete channel
			p=gc_Str_DelCH[gc_CurrentHZK];
		break;

		case 3:	// Delete all
			p=gc_Str_DelAll[gc_CurrentHZK];
		break;

		case 4:	// Order record
			p=gc_Str_OrderRecord[gc_CurrentHZK];
		break;

		case 5:	// Quality
			p=gc_Str_Quality[gc_CurrentHZK];
		break;

		case 6:	// Exit
			p=gc_Str_Exit1[gc_CurrentHZK];
		break;
	}
	if(tc_item==gc_FMMenuNum)
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

void LCM_ShowFMMenu_Quality(U8 tc_item)
{
	data	U8	i;
	xdata	U8	*p;

	switch(tc_item)
	{
		case 0:	// Low
			p=gc_Str_Low[gc_CurrentHZK];
		break;

		case 1:	// Middle
			p=gc_Str_Middle[gc_CurrentHZK];
		break;

		case 2:	// High
			p=gc_Str_High[gc_CurrentHZK];
		break;
	}
	if(tc_item==gc_RecordQuality)
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

void LCM_ShowUSBConnectIcon(void)
{
    xdata	U16	i;
	data	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=PCIcon128x32;
	LCM_ReadSPIBMPdata(tw_Addr,512);
	for(j=0;j<4;j++)
	{
		LCM_set_address(j+2,0);

		for(i=0;i<128;i++)
		{
			LCM_write_data(gc_DirLCMtempBuf[((U16)j*128)+i]);
		}
	}
}

void LCM_ShowFrequency_L()	// 12x32
{
	data	U16 tw_FM_frequency;

	tw_FM_frequency=gw_FM_frequency;
	if(tw_FM_frequency>999)
	{
		LCM_ShowChar12x32(1,16);
	}
	else
	{
		LCM_ShowChar12x32(31,16);
	}
	LCM_ShowChar12x32((tw_FM_frequency%1000)/100,28);
	LCM_ShowChar12x32((tw_FM_frequency%100)/10,40);
	LCM_ShowChar12x32(32,52);	// "."
	LCM_ShowChar12x32(tw_FM_frequency%10,60);
	LCM_ShowChar12x32(21,72);	// M
	LCM_ShowChar12x32(17,84);	// H
	LCM_ShowChar12x32(30,96);	// z	
}

void LCM_ShowRecordString(void)
{
	data	U8	i;
	xdata	U8	*p;

	p=gc_Str_Record[gc_CurrentHZK];
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,0,1);
}

void LCM_ShowStartTimeString(void)
{
	data	U8	i;
	xdata	U8	*p;

	p=gc_Str_Start[gc_CurrentHZK];
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,16,1);
}

void LCM_ShowDurationString(void)
{
	data	U8	i;
	xdata	U8	*p;

	p=gc_Str_Duration[gc_CurrentHZK];
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,32,1);
}

void LCM_ShowRecONOFFString(void)
{
	data	U8	i;
	xdata	U8	*p;

	if(gb_OrderRecord==1)
	{
		p=gc_Str_ON[gc_CurrentHZK];
	}
	else
	{
		p=gc_Str_OFF[gc_CurrentHZK];
	}
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],70,0,0);
}

void LCM_ShowAlarmONOFFString(void)
{
	data	U8	i;
	xdata	U8	*p;

	if(gb_AlarmON==1)
	{
		p=gc_Str_ON[gc_CurrentHZK];
	}
	else
	{
		p=gc_Str_OFF[gc_CurrentHZK];
	}
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],70,0,0);
}

void LCM_ShowAlarmRepeatONOFFString(void)
{
	data	U8	i;
	xdata	U8	*p;

	if(gb_AlarmRepeatON==1)
	{
		p=gc_Str_ON[gc_CurrentHZK];
	}
	else
	{
		p=gc_Str_OFF[gc_CurrentHZK];
	}
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],70,32,0);
}
