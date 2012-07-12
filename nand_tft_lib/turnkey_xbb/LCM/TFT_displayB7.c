#include "SPDA2K.h"
#include "..\LCM\LCM_func.h"
#include "..\LCM\TFT_config.h"
#include "..\LCM\TFT_display.h"
#include "..\header\variables.h"

extern	data	U8	gc_CurrentCard;
extern	xdata	U8	gc_LanguageSel;
extern	xdata	U8	gc_IconY;
extern	xdata	U8	gc_IconType;
extern	xdata	U16	gw_FileIndex[]; 
extern	xdata	U16	gw_FileTotalNumber[];

extern void LCD_DisplayIcon(U8 tw_IconIDNumber,U8 tc_CurrentOffset);

extern	xdata	U8	gc_JPGSetting;
extern	xdata	U8	gc_FuncPicCount;
extern	xdata	U8	gc_MenuItem;

#if 1
// USB
void TFT_ShowUSBIcon(void)
{
	LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
	LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_USB);
}
#endif

// 開機動畫
void TFT_PowerOnlogo(void)
{
	data	U16 i;

	LCM_set_view(TFT_IconDispDir,PowerONLogo_X_Pos,PowerONLogo_Y_Pos,PowerONLogo_H_Size,PowerONLogo_V_Size);
	for(i=0;i<PowerONLogoPicNum;i++)
	{
		LCD_DisplayIcon(PowerONLogoID,i);
		LCD_BACKLIGHT_ON;
		USER_DelayDTms(PowerONLogoTimer);
	}
}

// 關機動畫
void TFT_PowerOfflogo(void)
{
	data	U8 i;

	LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);

	for(i=0;i<PowerOFFLogoPicNum;i++)
	{
		LCD_DisplayIcon(PowerOFFLogoID,i);
		USER_DelayDTms(80);
	}
	USER_DelayDTms(500);
}

// 功能選項-Music/Movie/Photo/Setup
void TFT_ShowMenu(void)
{
    U8 x_menu;

	LCM_set_view(TFT_IconDispDir,MainMenu_Title_X_Pos,MainMenu_Title_Y_Pos,MainMenu_Title_H_Size,MainMenu_Title_V_Size);
	x_menu = gc_MenuItem+gc_LanguageSel*4;
	LCD_DisplayIcon(ID_MainMenu_Title,x_menu);

	LCM_set_view(TFT_IconDispDir,MainMenu_X_Pos,MainMenu_Y_Pos,MainMenu_H_Size,MainMenu_V_Size);
	x_menu = (gc_MenuItem*NumOfGroup+gc_FuncPicCount);//+gc_LanguageSel*16;
	LCD_DisplayIcon(ID_MainMenu,x_menu);

	gc_FuncPicCount++;
	if(gc_FuncPicCount==NumOfGroup)
	{
		gc_FuncPicCount=0;
	}
}
	
// JPG播放延遲時間
void TFT_Velocity(void)
{
	data	U8	i;
	data	U8	BCD[2];

	if(gc_JpegVelocity==10)
	{
		BCD[0]=1;
		BCD[1]=0;
	}
	else
	{
		BCD[0]=0;
		BCD[1]=gc_JpegVelocity;
	}

	for(i=0;i<2;i++)
	{
		LCM_set_view(TFT_IconDispDir,Velocity_X_Pos+(Velocity_H_Size+2)*i,Velocity_Y_Pos,Velocity_H_Size,Velocity_V_Size);
		LCD_DisplayIcon(VelocityID,BCD[i]);
	}
}


U8 CheckNum(U16 num)
{
	if(num>9999)	
	{
		return 0;
	}
	if(num>999)		
	{
		return 1;
	}
	if(num>99)	
	{
		return 2;
	}
	if(num>9)
	{
		return 3;
	}
	return 4;
}
// 記憶體資料
void TFT_MemoryInfo(void)
{
	data	U8	i,j;
	xdata	U8	BCD[15];
	data	U16	tc_used=gdw_CARD_TotalSizeMB-gdw_Dos_FreeMemorySizeMB;

	LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
	//LCD_DisplayIcon(MemInfoID,0/*gc_LanguageSel*/);
	LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_MemoryInfo_BG);

//	if(gc_LanguageSel!=16)
	{
		for(i=0;i<3;i++)
		{
			if(i==0)
			{
				TFT_HEX_BCD_5(gdw_CARD_TotalSizeMB,BCD+0,BCD+1,BCD+2,BCD+3,BCD+4);
				j=CheckNum(gdw_CARD_TotalSizeMB);
			}
			else if(i==1)
			{
				TFT_HEX_BCD_5(tc_used,BCD+5,BCD+6,BCD+7,BCD+8,BCD+9);
				j=CheckNum(tc_used);
			}
			else
			{
				TFT_HEX_BCD_5(gdw_Dos_FreeMemorySizeMB,BCD+10,BCD+11,BCD+12,BCD+13,BCD+14);
				j=CheckNum(gdw_Dos_FreeMemorySizeMB);
			}
			
			for(;j<5;j++)
			{
#ifdef	TFT_18_V
				LCM_set_view(TFT_IconDispDir,MemInfoNUM_X_Pos+j*MemInfoNUM_H_Size,MemInfoNUM_Y_Pos+i*22,MemInfoNUM_H_Size,MemInfoNUM_V_Size);
#else
		        	LCM_set_view(TFT_IconDispDir,MemInfoNUM_X_Pos+j*MemInfoNUM_H_Size,MemInfoNUM_Y_Pos+i*17,MemInfoNUM_H_Size,MemInfoNUM_V_Size);
#endif
				LCD_DisplayIcon(MemInfoNumID,BCD[i*5+j]);
			}
		}
	}
}

void TFT_HEX_BCD_5(U16 num,U8 *AA,U8 *BB,U8 *CC,U8 *DD,U8 *EE)
{
	*AA=num/10000;
	*BB=(num%10000)/1000;
	*CC=(num%1000)/100;
	*DD=(num%100)/10;
	*EE=num%10;
}

// 儲存裝置:NAND(0)/U-Disk(1)/SDC(2)
void TFT_ShowMediaIcon_M(void)
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

// PAUSE 
void TFT_PAUSE(void)
{
	LCM_set_view(TFT_IconDispDir,Pause_X_Pos,Pause_Y_Pos,Pause_H_Size,Pause_V_Size);
	LCD_DisplayIcon(PauseID,0);
}


// 文件格式Icon
void TFT_FileIcon(void)
{
	LCM_set_view(TFT_IconDispDir,FileIcon_X_Pos,FileIcon_Y_Pos+20*gc_IconY,FileIcon_H_Size,FileIcon_V_Size);
	LCD_DisplayIcon(FileIconID,gc_IconType);
}



 void Folder_Display()
{
//	U8 testbuf[]={16,0,'V',0,'1',0,'.',0,'0',0,'1',0,'.',0,'0',0,'1'};
	U8 i;
	U8 	BCD[4];
	//扢离晊奀奀潔
//	gc_FolderTimer=5;
	//珆尨掖劓
	LCM_set_view(TFT_IconDispDir,0,0,TFT_H_Size,TFT_V_Size);
	LCD_DisplayIcon(ID_PUBLIC_FULL,OFFSET_Floder_BG);   
	//刲坰狟珨跺恅璃標

	//珆尨絞ヶ⑻醴
//	HEX_to_BCD(gw_FileIndex[gs_System_State.c_FileHandle],BCD+0,4);
	TFT_HEX_BCD(gw_FileIndex[0],(BCD+0),(BCD+1),(BCD+2),(BCD+3));

	for(i=0;i<4;i++)
	{
		LCM_set_view(TFT_IconDispDir,30+FMAdj_H_Size*i,50,FMAdj_H_Size,FMAdj_V_Size);
		LCD_DisplayIcon(FMAdjID,BCD[i]);	
	} 
	//珆尨軞⑻醴
//	HEX_to_BCD(gw_FileTotalNumber[gs_System_State.c_FileHandle],BCD+0,4);
	TFT_HEX_BCD(gw_FileTotalNumber[0],(BCD+0),(BCD+1),(BCD+2),(BCD+3));

	for(i=0;i<4;i++)
	{
		LCM_set_view(TFT_IconDispDir,85+FMAdj_H_Size*i,50,FMAdj_H_Size,FMAdj_V_Size);
		LCD_DisplayIcon(FMAdjID,BCD[i]);	
	}
	//珆尨恅璃標靡備
 //	Menu_Disp_String(5,5,testbuf,0xffff,0); 
//	Menu_Disp_String(Voice_Item_X,Voice_Item_Y,testbuf,Voice_Item_WordColor,Voice_Item_WordColor_BG);
//    gs_str_scroll_state.c_str_1st_char=0;  //20090331
//    gs_str_scroll_state.c_shift_in_1st_char=0;
	//LCD_Disp_FileName(&gc_FileLongName[5], gc_FileLongName[2], gc_FileLongName[4], 1);

}
