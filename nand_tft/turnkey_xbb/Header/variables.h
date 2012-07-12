#include "define.h"
#include "..\..\libsource\header\UI_config.h"

#ifndef __VARIABLES_H__
#define __VARIABLES_H__

#define	gw_font_sector	538
#define	gw_icon_sector	5036
//PLAY
extern xdata   U8  gc_PlayModeChange_Flag;  //20090107 chiayen add

extern data 	bit gb_SDNoFileflag; 
extern data 	bit gb_InitLogFileIndex;
extern data 	bit gb_SD_Exist;
extern data 	bit gb_SD_Exist_pre;
extern data 	bit gb_Host_Exist;
extern data 	bit gb_Host_Exist_pre;
extern	xdata	U8	gc_HostSectorUnit;

extern xdata 	U8	gc_HostNoFileflag;
extern xdata 	U8  gc_CurrentCard_backup;
extern xdata 	U8  gc_RepPlayMode_Pre;

extern xdata 	U8  gc_Play_FileType;
extern xdata 	U16   gw_Disp_CurrentSec;

extern bit gb_FlashNoFileflag;  //20090309 chiayen modify
extern xdata 	U8   gc_SelectVol;       // 1=Show Vol  //20090309 chiayen modify
extern xdata	U8    gc_CurrentHZK;  //20090107 chiayen add  //20090309 chiayen mark

extern xdata U8 gc_Media_type;
extern xdata U8 gc_DisplayVol;

extern	data	bit  gb_Play_firstPlay;  //20090319
extern xdata U8 gc_MenuHZK;

extern xdata U8 gc_DispPlayStatus;
extern xdata U16 gw_DispTotalTime;

extern xdata U8 gc_TuneVolFreqStatus;
extern xdata U8 gc_DispWallpaper;
extern xdata U8 gc_DispPlayMenuAct;

extern xdata U8 gc_JpegVelocity;  //20090331
extern xdata U8 gc_AutoJpeg_Flag;  //20090331
extern xdata U16 gw_AutoJpeg_Timer;  //20090331

extern xdata U32 gdw_Dos_FreeMemorySizeMB;


extern xdata U16 gw_x_start;
extern xdata U16 gw_y_start;
extern xdata U16 gw_width;
extern xdata U16 gw_height;

extern xdata	U8	gc_CurrentCard_Bak;
extern xdata	U8	gc_LCMDispIndexBak;
extern xdata	U8	gc_MenuItemBak;
extern xdata	U16	gw_Disp_CurrentSecBak;

extern data	bit	gb_LRCDispLevel;
extern data	bit	gb_TFT_refresh;	// 1=主畫面全部ICON重
extern data	bit	gb_TFT_VOL_FM;		// 0=VOL adj.  1=FM adj.
extern data	bit	gb_TFT_scroll;
extern xdata	U8	gc_EQBak;

extern xdata	U8	gc_MenuItemCount;

extern U16	gw_LrcNextOffset;
extern U32	gdw_LrcNextFilePointer;
extern U8	gc_LrcCurrentLenNext;
extern U8	gb_LrcGetTimeNextOrNot;

//extern data U8 gc_PKSongCHSetMode;
extern data	bit	gb_ShowIRNum;
extern xdata	U16	gw_IRNum;

extern xdata U8 gc_Dosinitfail;
extern xdata U8	gc_strobe_bak;
extern xdata U8  gc_0xB400_bak; 
extern xdata U16 gw_BmpInx;
extern xdata U8  gc_BmpString[];

//extern xdata U16 gw_font_sector;
//extern xdata U16 gw_icon_sector;

extern data U8 gc_KeyDet_Mask;  //20090107 chiayen modify

extern data U8 gc_LongKeyCount;

extern xdata U8 gc_Vrefinit_Timer;
extern xdata U8 gc_KeyValue;
extern xdata U8 gc_KeyEvent;
extern data U8 gc_PhaseInx;
extern xdata U8 gc_RepPlayMode;
extern xdata U16 gw_TotalSec;
extern xdata U8 gc_T0IntervalLSB;
extern xdata U8 gc_T0IntervalMSB;

extern U8 gc_key_Pressed;
extern U8 gc_key_PrevPressed;
extern bit gbLKeyTimer_Timeout;
extern bit gbGetKey_Timeout;

extern data bit gb_IR_Setting;
extern data bit gb_USB_ISR_Detected; 
extern data U8 gc_IRCmdStatus;
extern data U8 gc_ircount;
extern data U8 gw_ChangeFrequencyTimer;
extern data U8 gw_IR_Setting_Timer;
extern data U8 gw_LogData_Timer;
extern xdata U8 gc_LogDataFlag;
extern xdata U16 gw_Random_Timer;
extern xdata U8 gc_XTimeBase;
extern xdata U8 gc_LongKeyTimer;
extern xdata U8 gc_LongKeyDelay;
extern data	bit	gb_Frequency_Song;
extern data	bit	gb_SelectEQ;  
extern data	bit	gb_PickSong;  
extern data	bit	gb_ChannelSet; 
extern data	U8	gc_ShowTimer;
extern data	U16	gw_DisplayFreq;
extern data	U16	gw_DisplayTime;
extern data	U16	gw_DisplayBitrateTime;
extern data	U16	gw_LCMScrollTimer;      //20090309 chiayen modify

extern data  U16 gw_LogFileIndex;
extern data  U16 gw_SDLogFileIndex;
extern data  U16 gw_USBLogFileIndex;
extern xdata U8  gc_UIblock0_index;
extern xdata U16 gw_DIRFileIndex;
extern xdata U16 gw_DIRFileTotalNumber;
extern xdata U32 gdw_DIRFDB_StartCluster;
extern xdata U16 gw_SDDIRFileIndex;  //20090216 chiayen add
extern xdata U16 gw_SDDIRFileTotalNumber;
extern xdata U32 gdw_SDDIRFDB_StartCluster;

extern xdata U16 gw_HostDIRFileIndex;
extern xdata U16 gw_HostDIRFileTotalNumber;
extern xdata U32 gdw_HostDIRFDB_StartCluster;  //20090216 chiayen add

extern data bit gb_PickSongSet;
extern data U8 gc_Dirchange_Timer; //20090121 chiayen add
extern data U8 gw_IR_Timer;
extern data U16 gw_irkey_count;
extern xdata U16 gw_SongNumber;  //temp
extern xdata U16 gw_SongNumberSD;

extern xdata U8 gc_VolumeMute;
extern xdata U8 gc_NUMBER[4];
extern xdata U8 gc_irkey;
extern xdata U8 gc_IRdetMask;
extern xdata U8 gc_Volumebackup;

extern xdata	U16	gw_DisplayFreq1;
extern xdata	U8	gc_DisplayEQIcon;  //20090309 chiayen modify
extern xdata	U16	gw_DispSongNum1; //20090121 chiayen test  //20090309 chiayen modify  
extern data	U16	gw_DispSongNum ;  //20090309 chiayen modify
extern data	U16	gw_DispTotalSong;
extern xdata U8 gc_FolderPlay_Flag;  //20090216 chiayen add
extern xdata U8 gc_PlayMenu_IR;  //20090206 chaiyen add for IR_DIR

extern xdata	U8	gc_MenuItem;
extern bit		gb_DirPlay_Flag;  // 0:PlayAll  1:Play in Dir

extern data  U8  gc_PlayMenu_Timeout;
extern xdata U8  gc_DirNoFile_Flag;
extern xdata U8  Save_c_FileType[3];
extern xdata U8  Save_dw_File_StartCluster[3];
extern xdata U16 gw_TotalFileNum;
extern xdata U16 Save_gw_MusicFileTotalNum;
extern xdata U16 gw_DirIndexTest[4];
extern xdata U16 Save_w_FDB_Offset[3];
extern xdata U16 Save_gw_FileIndex[3];
extern xdata U16 Save_gw_FileDirIndex[3];
extern xdata U32 Save_dw_FDB_StartCluster[3];
extern xdata U32 Save_dw_FDB_Cluster[3];
extern xdata U32 Save_dw_FDB_LogAdd[3];
extern xdata U32 Save_dw_LongFDB_LogAdd0[3];
extern xdata U32 Save_dw_LongFDB_LogAdd1[3];

extern xdata U16 	gw_TotalNumber;		//count dir+file number in one dir
extern xdata U16 	gw_DirTotalNumber;	//count dir total number in one dir
extern xdata U16 	gw_DirOverallNumber;  //count dir overall number, //20090216 chiayen add
extern xdata U16 	gw_AllDirTotalNumber;
extern xdata U16	gw_DIRtableIndex;
extern xdata U16 	gw_Index;

extern xdata U8 gc_FatherIndex;
extern xdata U8 gc_DirReBuildFlag;
extern xdata U8 gc_SetIndex;		//for rolling mechanism
extern xdata U8 gc_SetNumber;

extern xdata U8 gc_test;
extern xdata U8 gc_SDSetIndex;  //chiayen 20090320
extern xdata U8 gc_TaskMode_BkUp;//(JC)I0304 //20090320 chiayen test
extern bit	gbt_SD_BuildDIRFlag; //Ching 090306

//=================================
//playmenu
extern xdata U8 gc_EQChangeFlag;
extern xdata U8 gc_LCMDispIndex;

extern data	 U8		gc_LCM_Media;  //20090309 chiayen modify
extern xdata	 U8		gc_DisplayPlayMode;  //20090309 chiayen modify
extern data	 U16	gw_DisplayBitRate; //20090309 chiayen modify

extern xdata    U16 	gw_FM_frequency;  // range=87.7MHz~107.9MHz  //20090309 chiayen mark


//========================sunzhk add 0703====================//
extern data	U8	gc_refresh_type;//0:refresh all ,1:next op refresh,2:prev op refresh 0xff:not refresh
extern xdata U8 gc_folder_disp;

extern	xdata	U16	gw_CurrentSec;
extern	xdata	U16	gw_CTStartTime;
extern	xdata	U8	gb_fffr_end;

#ifdef Contrast_Set
extern xdata   U8  gc_Contrast;
extern xdata   U8  gc_Contrast_bak;
#endif

extern	xdata U32 gdw_ID3_v2_sz;	//Jimi 0901027
#endif // __VARIABLES_H__

