#include "SPDA2K.h"
#include "..\LCM\LCM_func.h"
#include "..\LCM\TFT_config.h"
#include "..\LCM\TFT_display.h"
#include "..\header\variables.h"


xdata	U8	gc_ShowMenuTimer=0;
xdata	U8	gc_FuncPicCount=0;
xdata	U8	gc_PlayBarBak=0;
xdata	U8	gc_ShowPingpuTimer=0;
xdata   U8	gb_ShowPingpu=0;

extern	data	bit	gb_TFT_VOL_FM;
extern	data	U8	gc_CurrentCard;

extern	xdata	U8	gc_LanguageSel;
extern	xdata	U16	gw_FileIndex[]; 
extern	xdata	U16	gw_FileTotalNumber[];
extern	DSP_GLOBAL	gs_DSP_GLOBAL_RAM;
extern	xdata	System_Struct gs_System_State;

extern void LCD_DisplayIcon(U8 tw_IconIDNumber,U8 tc_CurrentOffset);
void TFT_ShowPingPu(void);

void TFT_HEX_BCD(U16 num,U8 *Hi,U8 *MidH, U8 *MidL, U8 *Low)
{
	*Hi=num/1000;
	*MidH=(num-((U16)*Hi*1000) )/100;
	*MidL=(num- ((U16)*Hi*1000) - ((U16)*MidH*100) )/10;
	*Low=num%10;
	if(*Hi>10)
	{ 
		*Hi=*Hi%10;
	}
}

// EQ:NOR/DBB/JAZZ/...
void TFT_ShowEQIcon(void)
{
	LCM_set_view(TFT_IconDispDir,EQIcon_X_Pos,EQIcon_Y_Pos,EQIcon_H_Size,EQIcon_V_Size);
	LCD_DisplayIcon(EQIconID,gc_DisplayEQIcon);
}

// 播放模式:Normal/Repeat-1/Repeat-All
void TFT_ShowPlayMode(void)
{
	LCM_set_view(TFT_IconDispDir,PlayModeIcon_X_Pos,PlayModeIcon_Y_Pos,PlayModeIcon_H_Size,PlayModeIcon_V_Size);
	LCD_DisplayIcon(PlayModeIconID,gc_RepPlayMode);
}

// 儲存裝置:NAND(0)/U-Disk(1)/SDC(2)
void TFT_ShowMediaIcon(void)
{
	data	U8	i;

	if(gc_CurrentCard==5)
	{
		i=4;	// Host
	}
	else
	{
		i=gc_CurrentCard;
	}

	LCM_set_view(TFT_IconDispDir,MediaIcon_X_Pos,MediaIcon_Y_Pos,MediaIcon_H_Size,MediaIcon_V_Size);
	LCD_DisplayIcon(MediaIconID,i);
}

// 播放狀態:Play/Pause
void TFT_ShowPlayPauseIcon(void)
{
	LCM_set_view(TFT_IconDispDir,PlayIcon_X_Pos,PlayIcon_Y_Pos,PlayIcon_H_Size,PlayIcon_V_Size);
	LCD_DisplayIcon(PlayIconID,gc_DispPlayStatus);
}

// 音樂格式:MP3/WMA/APE
void TFT_ShowMusicType(void)
{
	LCM_set_view(TFT_IconDispDir,MusicType_X_Pos,MusicType_Y_Pos,MusicType_H_Size,MusicType_V_Size);
	LCD_DisplayIcon(MusicTypeID,gc_Media_type);	
}

// 主畫面FM頻率
void TFT_ShowFMFrequency(void)
{
	data	U8 	BCD[4];
	data	U8	i,j=0;

	TFT_HEX_BCD(gw_FM_frequency,(BCD+0),(BCD+1),(BCD+2),(BCD+3));
	
	for(i=0;i<4;i++)
	{
		if((i==0)&&(BCD[0]==0))
		{
			BCD[0]=10;	// Blank
		}
		else if(i==3)
		{
			j=FreqDotOffset+6;
		}
		LCM_set_view(TFT_IconDispDir,FM_Freq_X_Pos+FM_Freq_H_Size*i+j,FM_Freq_Y_Pos,FM_Freq_H_Size,FM_Freq_V_Size);
		LCD_DisplayIcon(FM_FreqID,BCD[i]);
	}
	//賒萸
		LCM_set_view(TFT_IconDispDir,ID_FM_DOT_X_Size,ID_FM_DOT_Y_Size,ID_FM_DOT_H_Size,ID_FM_DOT_V_Size);
		LCD_DisplayIcon(ID_FM_DOT,0);
}

// 播放曲數
void TFT_ShowSongIndex(void)
{
	data	U8 	BCD[4];
	data	U8	i;

	TFT_HEX_BCD(gw_FileIndex[gs_System_State.c_FileHandle],(BCD+0),(BCD+1),(BCD+2),(BCD+3));
	for(i=0;i<4;i++)
	{
		LCM_set_view(TFT_IconDispDir,SongIndex_X_Pos+SongIndex_H_Size*i+i,SongIndex_Y_Pos,SongIndex_H_Size,SongIndex_V_Size);
		LCD_DisplayIcon(SongIndexID,BCD[i]);	
	}
}

// 總曲數
void TFT_ShowTotalSong(void)
{
	data	U8 	BCD[4];
	data	U8	i;

	TFT_HEX_BCD(gw_FileTotalNumber[gs_System_State.c_FileHandle],(BCD+0),(BCD+1),(BCD+2),(BCD+3));
	for(i=0;i<4;i++)
	{
		LCM_set_view(TFT_IconDispDir,TotalSong_X_Pos+TotalSong_H_Size*i+i,TotalSong_Y_Pos,TotalSong_H_Size,TotalSong_V_Size);
		LCD_DisplayIcon(TotalSongID,BCD[i]);	
	}
	//珆尨訇話
		LCM_set_view(TFT_IconDispDir,ID_XIEGANG_X_Size,ID_XIEGANG_Y_Size,ID_XIEGANG_H_Size,ID_XIEGANG_V_Size);
		LCD_DisplayIcon(ID_TOTALNUM_XIEGANG,0);	
}

// BitRate
void TFT_ShowBitRate(void)
{
	data	U8 	BCD[4];
	data	U8	i;

	TFT_HEX_BCD(gw_DisplayBitRate,(BCD+0),(BCD+1),(BCD+2),(BCD+3));
	for(i=0;i<3;i++)
	{
#ifdef TFT_18V
		LCM_set_view(TFT_IconDispDir,BitRate_X_Pos+(BitRate_H_Size+1)*i,BitRate_Y_Pos,BitRate_H_Size,BitRate_V_Size);
		LCD_DisplayIcon(BitRateID,BCD[i+1]);
#else
	       LCM_set_view(TFT_IconDispDir,BitRate_X_Pos+(BitRate_H_Size+BitRate_gap)*i,BitRate_Y_Pos,BitRate_H_Size,BitRate_V_Size);
			LCD_DisplayIcon(BitRateID,BCD[i+1]);
		   //賒KBPS
		LCM_set_view(TFT_IconDispDir,ID_KBPS_X_Size,ID_KBPS_Y_Size,ID_KBPS_H_Size,ID_KBPS_V_Size);
		LCD_DisplayIcon(ID_KBPS,0);	
#endif
		
	}
}

// 音樂播放主畫面
void TFT_Main(void)
{
	LCM_set_view(TFT_IconDispDir,ID_PlayBG_X_Pos,ID_PlayBG_Y_Pos,ID_PlayBG_H_Size,ID_PlayBG_V_Size);
	LCD_DisplayIcon(ID_PlayBG,0); //090805,蜊峈垀衄弊模腔Play掖劓珆尨眈肮腔芞え 
	LCM_set_view(TFT_IconDispDir,ID_FM_ICON_X_Size,ID_FM_ICON_Y_Size,ID_FM_ICON_H_Size,ID_FM_ICON_V_Size);
	LCD_DisplayIcon(ID_FM_ICON,0);
	TFT_Show_VOL();
	
//珆尨貉棵芞梓
	LCM_set_view(TFT_IconDispDir,ID_LYRIC_X_Size,ID_LYRIC_Y_Size,ID_LYRIC_H_Size,ID_LYRIC_V_Size);
	LCD_DisplayIcon(ID_LYRIC,0);

   	if(gc_Play_FileType==2)
	{
#ifdef TFT_18V
		LCM_set_view(TFT_IconDispDir,43,18,19,16);	
#else
	  	LCM_set_view(TFT_IconDispDir,13,54,38,36);
#endif
        LCD_DisplayIcon(39,0);
	}
}

// 當前播放時間&Bar
void TFT_ShowPlayTime(void)
{
	data	U8 	BCD[4];
	data	U8	i,j=0;

	BCD[0]=gw_Disp_CurrentSec/600;
	BCD[1]=(gw_Disp_CurrentSec/60)%10;
	BCD[2]=(gw_Disp_CurrentSec%60)/10;
	BCD[3]=(gw_Disp_CurrentSec%60)%10;

	for(i=0;i<4;i++)
	{
		if(i==2)
		{
			j=SecondOffset+10;
		}

		LCM_set_view(TFT_IconDispDir,PlayTime_X_Pos+(PlayTime_H_Size+2)*i+j,PlayTime_Y_Pos,PlayTime_H_Size,PlayTime_V_Size);
		LCD_DisplayIcon(PlayTimeID,BCD[i]);
	}
	// 賒奀潔腔謗萸	
		LCM_set_view(TFT_IconDispDir,ID_PLAYTIME_DOT_X_Size,ID_PLAYTIME_DOT_Y_Size,ID_PLAYTIME_DOT_H_Size,ID_PLAYTIME_DOT_V_Size);
		LCD_DisplayIcon(ID_PLAYTIME_DOT,0);


	TFT_ShowPlayRatioBar();
}

void TFT_ShowPingPu(void)
{
		static U8	k=0;
	//珆尨けび
		LCM_set_view(TFT_IconDispDir,ID_PINGPU_X_Size,ID_PINGPU_Y_Size,ID_PINGPU_H_Size,ID_PINGPU_V_Size);
		LCD_DisplayIcon(ID_PingPu,k);
		k++;
		if(k>13)
		{
			k=0;
		}
}


// 播放時間BAR
void TFT_ShowPlayRatioBar(void)
{
	data	U8	i;

	if(gc_Play_FileType==2)	// For MP4
	{
		i=PlayRatioBarNum-1;
	}
	else
	{
		i=((gw_Disp_CurrentSec*PlayRatioBarNum)/gw_DispTotalTime);
	}

	if(i>=PlayRatioBarNum)
	{
		i=PlayRatioBarNum-1;
	}

	if(gc_PlayBarBak==i)
	{
		return;
	}

	gc_PlayBarBak=i;

	LCM_set_view(TFT_IconDispDir,PlayRatioBar_X_Pos,PlayRatioBar_Y_Pos,PlayRatioBar_H_Size,PlayRatioBar_V_Size);
	LCD_DisplayIcon(PlayRatioBarID,i);	// need play status
}


// 歌曲時間
void TFT_ShowSongLength(void)
{
	data	U8 	BCD[4];
	data	U8	i,j=0;

	BCD[0]=gw_DispTotalTime/600;
	BCD[1]=(gw_DispTotalTime/60)%10;
	BCD[2]=(gw_DispTotalTime%60)/10;
	BCD[3]=(gw_DispTotalTime%60)%10;
	
	for(i=0;i<4;i++)
	{
		if(i>=2)
		{
			j=SongTotalTimeOffset;
		}
		else
		{
			j=0;
		}
		LCM_set_view(TFT_IconDispDir,SongTotalTime_X_Pos+SongTotalTime_H_Size*i+i+j,SongTotalTime_Y_Pos,SongTotalTime_H_Size,SongTotalTime_V_Size);
		LCD_DisplayIcon(SongTotalTimeID,BCD[i]);
	}
	//賒軞奀潔腔萸
		LCM_set_view(TFT_IconDispDir,ID_TOTALTIME_DOT_X_Size,ID_TOTALTIME_DOT_Y_Size,ID_TOTALTIME_DOT_H_Size,ID_TOTALTIME_DOT_V_Size);
		LCD_DisplayIcon(ID_TOTALTIME_DOT,0);
}

// FM頻率調整
void TFT_ShowFMFreqAdj(void)
{
	data	U8 	BCD[5];
	data	U8	i;

	TFT_HEX_BCD(gw_FM_frequency,(BCD+0),(BCD+1),(BCD+2),(BCD+4));
	
	for(i=0;i<5;i++)
	{
		if((i==0)&&(BCD[0]==0))
		{
			BCD[0]=10;	// Blank
		}
		else if(i==3)
		{
			BCD[3]=11;//j=FreqAdjDotOffset;
		}

		LCM_set_view(TFT_IconDispDir,FMAdj_X_Pos+FMAdj_H_Size*i+i,FMAdj_Y_Pos,FMAdj_H_Size,FMAdj_V_Size);
		LCD_DisplayIcon(FMAdjID,BCD[i]);
	}
}
void  TFT_Show_VOL()
{
	data	U8	i;
	data	U8  k;
	U8 tc_vol[2];
    U8 tc_DisplayVol;

	tc_DisplayVol=gs_DSP_GLOBAL_RAM.sw_Volume/2;   

    tc_vol[0]=tc_DisplayVol/10;
    tc_vol[1]=tc_DisplayVol- (tc_DisplayVol/10)*10;

	LCM_set_view(TFT_IconDispDir,ID_VOL_ICON_X_Size,ID_VOL_ICON_Y_Size,ID_VOL_ICON_H_Size,ID_VOL_ICON_V_Size);
    LCD_DisplayIcon(ID_VOL_ICON,0);					//賒秞講芞梓
    for(i=0;i<2;i++)
    {
		if(i==1)
		{
			k=ID_VOLNUM_SPACE;
		}
		else k=0;
		
        LCM_set_view(TFT_IconDispDir,ID_VOLNUM_X_Size+(ID_VOLNUM_H_Size*i)+k,ID_VOLNUM_Y_Size,ID_VOLNUM_H_Size,ID_VOLNUM_V_Size);
        LCD_DisplayIcon(ID_VOLNUM,tc_vol[i]);	//秞講杅趼
    }
}

// VOL調整
void TFT_ShowVOLAdj(void)
{
    data	U8 tc_vol[2];
    data	U8 i;
	xdata	U8 tc_DisplayVol;

    tc_DisplayVol=gc_DisplayVol/2;   

    tc_vol[0]=tc_DisplayVol/10;
    tc_vol[1]=tc_DisplayVol- (tc_DisplayVol/10)*10;
    for(i=0;i<2;i++)
    {
        LCM_set_view(TFT_IconDispDir,VOLAdj_X_Pos+(VOLAdj_H_Size+2)*i,VOLAdj_Y_Pos,VOLAdj_H_Size,VOLAdj_V_Size);
        LCD_DisplayIcon(VOLAdjID,tc_vol[i]);
    }
}

// LRC背景圖
void TFT_LRCBackGround(void)
{
	LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
	LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_FullLRC_BG);
}

// EQ Frame(遙控器選EQ)
void TFT_EQFrame(void)
{
    U8 x_EQ_F;
	// EQ frame背景
	LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
	//LCD_DisplayIcon(40,0);
	LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_EQ_FRAME);
	// EQ
	LCM_set_view(TFT_IconDispDir,EQFrame_X_Pos,EQFrame_Y_Pos,EQFrame_H_Size,EQFrame_V_Size);
    x_EQ_F= gs_DSP_GLOBAL_RAM.sc_EQ_Type+gc_LanguageSel*6;
    LCD_DisplayIcon(EQFrameID,x_EQ_F);
}

// IR數字選曲選頻主畫面
void TFT_IRNumMain(void)
{
	LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
	LCD_DisplayIcon(FMORSONG_BG,gc_LanguageSel);
}

// IR數字選曲選頻(數字)
void TFT_IRNum(void)
{
	data	U8	i;
	data	U8	j;

	for(i=4;i>0;i--)
	{
		LCM_set_view(TFT_IconDispDir,IRNum_X_Pos+(IRNum_H_Size+2)*(4-i),IRNum_Y_Pos,IRNum_H_Size,IRNum_V_Size);

		j=gc_NUMBER[i-1];
		if(j==0xFF)
		{
			j=10;
		}
		LCD_DisplayIcon(IRNumID,j);
	}
}


