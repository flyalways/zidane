#include "..\Header\SPDA2K.h"
#include "..\header\UI_config.h"
#include "..\header\variables.h"
#include "LCM_BMP.h"


void LCM_ShowCompanyLogo(void)
{
    xdata	U16	i;
	data	U8	j;
	xdata	U16	tw_Addr;

	LCM_Clear_L0_L7();
	tw_Addr=CompanyLogo;
	LCM_ReadSPIBMPdata(tw_Addr,3072);
	for(j=0;j<4;j++)
	{
		LCM_set_address(j+2,16);

		for(i=0;i<96;i++)
		{
			LCM_write_data(gc_DirLCMtempBuf[((U16)j*96)+i]);
		}
	}
	USER_Delay(1000);
}


void LCM_ShowSDHostIcon(void)
{
	data	U8	i;
	data	U16	j;
	xdata	U16	tw_Addr;
	
	switch(gc_CurrentCard)
	{
		case 0x02:    //SD
			tw_Addr=SD_Icon12x16;
			LCM_ReadSPIBMPdata(tw_Addr,24);
		break;

		case 0x05:   //Host
			tw_Addr=USB_Icon12x16;
			LCM_ReadSPIBMPdata(tw_Addr,24);
		break;

		default:
			for(j=0;j<24;j++)
			{
				gc_DirLCMtempBuf[j]=0;
			}
		break;
	}
	
	for(i=0;i<2;i++)
	{
		LCM_set_address(i,98);
		for(j=0;j<12;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[j+i*12]);
		}
	}
}


void LCM_ShowFMIcon(void)
{
	data	U8	i;
	xdata	U16	j;
	xdata	U16	tw_Addr;

	tw_Addr=FM_Icon24x16;
	LCM_ReadSPIBMPdata(tw_Addr,48);
	for(i=0;i<2;i++)
	{
		LCM_set_address(i,0);
		for(j=0;j<24;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[j+i*24]);
		}
	}
}


void LCM_ShowWait()
{
	gc_LCM_line=4;
	LCM_ShowChar8x16(35,26);	// W
	LCM_ShowChar8x16(39,38);	// a
	LCM_ShowChar8x16(47,50);	// i
	LCM_ShowChar8x16(58,62);	// t
	LCM_ShowChar8x16(65,74);	// .
	LCM_ShowChar8x16(65,86);	// .
	LCM_ShowChar8x16(65,98);	// .
}


void LCM_Show_NoFile(void)
{
	gc_LCM_line=4;
	LCM_ShowChar8x16(26,26);	// N
	LCM_ShowChar8x16(53,38);	// o
	LCM_ShowChar8x16(12,50);	// 
	LCM_ShowChar8x16(18,62);	// F
	LCM_ShowChar8x16(47,74);	// i
	LCM_ShowChar8x16(50,86);	// l
	LCM_ShowChar8x16(43,98);	// e
	gc_LCM_line=0;
}


void RefreshAllDisplay(void)
{
	LCM_Clear_L0_L7();
	LCM_ShowRepeatIcon();
	if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_AB_NULL)
	{
		LCM_ShowSongNum();
		LCM_ShowSongTotalNum();
	}
	else
	{
		gb_FlashRepeatIcon=0;
		gc_ShowRepeatTimer=10;
	}
	LCM_ShowEQIcon();
	LCM_ShowMusicType();
	LCM_ShowBitRate();
	LCM_ShowSDHostIcon();
	LCM_ShowBattIcon();
	LCM_ShowPlayModeIcon();
	LCM_ShowSongTime();
	LCM_ShowPlayTime(0);
	LCM_ShowPinPu();
	LCM_ShowPlayModeIcon();
	LCM_ShowPlayPauseIcon();
}


void LCM_ShowBacklightTime(void)
{
	gc_LCM_line=3;
	LCM_ShowChar12x32(gc_LCMBacklightTime/10,52);
	LCM_ShowChar12x32(gc_LCMBacklightTime%10,64);
}

void LCM_ShowFMAllIcon(void)
{
	gs_System_State.w_SampleRate=8<<gc_RecordQuality;
	LCM_Clear_L0_L7();
	LCM_ShowFMIcon();
	LCM_ShowBattIcon();
	LCM_ShowSDHostIcon();
	LCM_ShowMusicType();
	LCM_ShowBitRate();
	LCM_ShowFMOrderRecordIcon();
	LCM_ShowAlarmIcon();
}

void LCM_ShowFMOrderRecordIcon(void)	// Order Record Icon & Time
{
	data	U8	i;
	data	U16	j;
	xdata	U16	tw_Addr;

	if(gb_OrderRecordConfirm==1||gb_OrderRecordTrigger==1)
	{
		gc_LCM_line=6;
		LCM_ShowChar8x16(10,34);										// :
		LCM_ShowChar8x16(gdw_OrderRecordTime/36000,16);			// 10H
		LCM_ShowChar8x16((gdw_OrderRecordTime/3600)%10,24);		// 1H
		LCM_ShowChar8x16((((gdw_OrderRecordTime/60)%60)/10),38);	// 10M
		LCM_ShowChar8x16((((gdw_OrderRecordTime/60)%60)%10),46);	// 1M
		tw_Addr=OrderRec_Icon11x16;
		LCM_ReadSPIBMPdata(tw_Addr,22);
	}
	else
	{
		return;
	}
	
	for(i=0;i<2;i++)
	{
		LCM_set_address(i+6,3);
		for(j=0;j<11;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[j+i*11]);
		}
	}
}

void LCM_ShowAlarmIcon(void)	// Alarm Icon & Time
{
	data	U8	i;
	data	U16	j;
	xdata	U16	tw_Addr;

	if(gb_AlarmON==1)
	{
		gc_LCM_line=6;
		LCM_ShowChar8x16(10,100);										// :
		LCM_ShowChar8x16(gdw_AlarmTime/36000,82);			// 10H
		LCM_ShowChar8x16((gdw_AlarmTime/3600)%10,90);		// 1H
		LCM_ShowChar8x16((((gdw_AlarmTime/60)%60)/10),104);	// 10M
		LCM_ShowChar8x16((((gdw_AlarmTime/60)%60)%10),112);	// 1M
		tw_Addr=AlarmIcon16x16;
		LCM_ReadSPIBMPdata(tw_Addr,32);
	}
	else
	{
		return;
	}
	
	for(i=0;i<2;i++)
	{
		LCM_set_address(i+6,64);
		for(j=0;j<16;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[j+i*16]);
		}
	}
}

void LCM_Show_LineIn(void)
{
	LCM_Clear_L0_L7();	

	gc_LCM_line=4;
	LCM_ShowChar8x16(24,26);	// L
	LCM_ShowChar8x16(47,38);	// i
	LCM_ShowChar8x16(52,50);	// n
	LCM_ShowChar8x16(43,62);	// e
	LCM_ShowChar8x16(12,74);	//  
	LCM_ShowChar8x16(21,86);	// I
	LCM_ShowChar8x16(52,98);	// n
	gc_LCM_line=0;
}

