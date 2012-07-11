#include "..\Header\SPDA2K.h"
#include "..\header\UI_config.h"
#include "..\header\variables.h"
#include "LCM_BMP.h"

	
void LCM_ShowCountryMenu(void)
{
	data	U8	i;
	xdata	U8	*p;

	if(gc_CurrentHZK>16)
	{
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,0,0,(gc_CurrentHZK&3)*16,1);
	}
	else
	{
		p=gc_Str_Country[gc_CurrentHZK];
		for(i=0;i<p[0];i++)
		{
			gc_LrcDisplayBuf[i]=p[1+i];
		}
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,(gc_CurrentHZK&3)*16,1);
	}
}

void LCM_ShowMainMenu(void)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U8	k;
	xdata	U8	m;
	xdata	U8	tc_Offset;
	xdata	U8	*p;
	xdata	U16	tw_Addr;

	gb_LCM_InverseDisp=0;
	switch(gc_MainMenuNum)
	{
		case 0:	// 音樂(Music)
			p=gc_Str_Music[gc_CurrentHZK];
		break;

		case 1:	// FM 收音(FM Radio)
			p=gc_Str_Radio[gc_CurrentHZK];
		break;

		case 2:	// 錄音播放(WAV play)
			p=gc_Str_WAV[gc_CurrentHZK];
		break;

		case 3:	// 日曆(Calendar)
			p=gc_Str_Calendar[gc_CurrentHZK];
		break;

		case 4:	// 設定(Setup)
			p=gc_Str_Setup[gc_CurrentHZK];
		break;
	}

	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_Clear_L0_L1();
#ifdef SPI_1M
	if(gc_CurrentHZK==LANG_CHS)
	{
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],(128-p[0]*8)/2,0,1);
	}
	else
	{
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],(128-p[0]*4)/2,0,1);
	}
#else
	if(gc_CurrentHZK==LANG_CHS||gc_CurrentHZK==LANG_CHT||gc_CurrentHZK==LANG_JAP||gc_CurrentHZK==LANG_KOR)
	{
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],(128-p[0]*8)/2,0,1);
	}
	else
	{
		LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],(128-p[0]*4)/2,0,1);
	}
#endif
	// Display Function Icon
	j=(gc_MainMenuNum+3)%5;
	for(i=0;i<5;i++)
	{
		switch(j)
		{
			case 0:
				if(i==2)
				{
					tw_Addr=MusicIcon34x32;
				}
				else
				{
					tw_Addr=MusicIcon17x16;
				}
			break;

			case 1:
				if(i==2)
				{
					tw_Addr=FMIcon34x32;
				}
				else
				{
					tw_Addr=FMIcon17x16;
				}
			break;

			case 2:
				if(i==2)
				{
					tw_Addr=PlayRecordIcon34x32;
				}
				else
				{
					tw_Addr=PlayRecordIcon17x16;
				}
			break;

			case 3:
				if(i==2)
				{
					tw_Addr=CalendarIcon34x32;
				}
				else
				{
					tw_Addr=CalendarIcon17x16;
				}
			break;

			case 4:
				if(i==2)
				{
					tw_Addr=SetupIcon34x32;
				}
				else
				{
					tw_Addr=SetupIcon17x16;
				}
			break;
		}
		j=(j+1)%5;
		switch(i)
		{
			case 0:
				tc_Offset=8;
			break;
	
			case 1:
				tc_Offset=29;
			break;
	
			case 3:
				tc_Offset=82;
			break;
	
			case 4:
				tc_Offset=103;
			break;
		}
		if(i==2)
		{
			LCM_ReadSPIBMPdata(tw_Addr,136);
			for(k=0;k<4;k++)
			{
				LCM_set_address(k+2,47);
				for(m=0;m<34;m++)
				{
					LCM_write_data(gc_DirLCMtempBuf[m+k*34]);
				}
			}
		}
		else
		{
			LCM_ReadSPIBMPdata(tw_Addr,34);
			for(k=0;k<2;k++)
			{
				LCM_set_address(k+6,tc_Offset);
				for(m=0;m<17;m++)
				{
					LCM_write_data(gc_DirLCMtempBuf[m+k*17]);
				}
			}
		}
	}
}

void LCM_ShowPlayMenu(U8 tc_item)
{
	data	U8	i;
	xdata	U8	*p;

	switch(tc_item)
	{
		case 0:	// 循環模式
			p=gc_Str_Repeat[gc_CurrentHZK];
		break;

		case 1:	// 音效設定
			p=gc_Str_EQ[gc_CurrentHZK];
		break;

		case 2:	// 復讀模式
			p=gc_Str_AB[gc_CurrentHZK];
		break;

		case 3:	// 退出
			p=gc_Str_Exit2[gc_CurrentHZK];
		break;
	}

	if(tc_item==gc_PlayMenuNum)
	{
		gb_LCM_InverseDisp=1;	
	}
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,tc_item*16,1);
	gb_LCM_InverseDisp=0;
}

void LCM_ShowWAVPlayMenu(U8 tc_item)
{
	data	U8	i;
	xdata	U8	*p;

	switch(tc_item)
	{
		case 0:	// 音效設定
			p=gc_Str_EQ[gc_CurrentHZK];
		break;

		case 1:	// 復讀模式
			p=gc_Str_AB[gc_CurrentHZK];
		break;

		case 2:	// 退出
			p=gc_Str_Exit2[gc_CurrentHZK];
		break;
	}

	if(tc_item==gc_WAVPlayMenuNum)
	{
		gb_LCM_InverseDisp=1;	
	}
	for(i=0;i<p[0];i++)
	{
		gc_LrcDisplayBuf[i]=p[1+i];
	}
	LCM_DisplayMenuString(&gc_LrcDisplayBuf,p[0],0,tc_item*16,1);
	gb_LCM_InverseDisp=0;
}


