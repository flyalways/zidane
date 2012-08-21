#include "..\header\variables.h"

//Play
xdata   U8  gc_PlayModeChange_Flag=0;  //20090107 chiayen add

data 	bit gb_SDNoFileflag=0; 
data 	bit gb_InitLogFileIndex=0;
data 	bit gb_SD_Exist=0;
data 	bit gb_SD_Exist_pre=0;
data 	bit gb_Host_Exist=0;
data 	bit gb_Host_Exist_pre=0;
xdata 	U8	gc_HostNoFileflag=0;
xdata 	U8  gc_RepPlayMode_Pre=0;
xdata 	U8	gc_Play_FileType=0;
xdata 	U16	gw_Disp_CurrentSec=0;
xdata 	U8  gc_CurrentCard_backup=0;

data	bit	gb_FlashNoFileflag=0;  //20090309 chiayen modify
xdata 	U8	gc_SelectVol=1;       // 1=Show Vol  //20090309 chiayen modify
xdata	U8	gc_CurrentHZK;  //20090107 chiayen add  //20090309 chiayen mark

xdata U8 gc_Media_type=0xFF;
xdata U8 gc_DisplayVol=0xFF;

data bit	 gb_Play_firstPlay=0x01;		//Jimi 081125 for EQ mode pop noise as next/prev  20090319
xdata U8 gc_MenuHZK=1;

xdata U8 gc_DispPlayStatus=0xFF;
xdata U16 gw_DispTotalTime=0xFFFF;
xdata U8 gc_TuneVolFreqStatus=0;  //祏vol key篈 0:normal 1:turn vol 2:turn freq
xdata U8 gc_DispWallpaper=1;      //∕﹚琌璶穝陪ボ璉春  0:no 1:yes
xdata U8 gc_DispPlayMenuAct=0;    //0:PlayMenu 1:EQ 2:PlayMode

xdata U8 gc_JpegVelocity=3;  //20090331
xdata U8 gc_AutoJpeg_Flag=1;  //20090331//自动标志
xdata U16 gw_AutoJpeg_Timer=1;  //20090331

xdata U32 gdw_Dos_FreeMemorySizeMB=0;

xdata U16 gw_x_start;
xdata U16 gw_y_start;
xdata U16 gw_width;
xdata U16 gw_height;

xdata	U8	gc_CurrentCard_Bak=0xFF;
xdata	U8	gc_LCMDispIndexBak;
xdata	U8	gc_MenuItemBak=0;
xdata	U16	gw_Disp_CurrentSecBak=0xFFFF;

data	bit	gb_LRCDispLevel;
data	bit	gb_TFT_refresh=0;	// 1=礶场ICON
data	bit	gb_TFT_VOL_FM;		// 0=VOL adj.  1=FM adj.
data	bit	gb_TFT_scroll;
xdata	U8	gc_EQBak=0xFF;

xdata	U8	gc_MenuItemCount=0;

U16	gw_LrcNextOffset=0;
U32	gdw_LrcNextFilePointer=0;
U8	gc_LrcCurrentLenNext=0;
U8	gb_LrcGetTimeNextOrNot=0;

xdata	U8	gc_HostSectorUnit=1;
//data U8 gc_PKSongCHSetMode=0;
data	bit	gb_ShowIRNum=0;
xdata	U16	gw_IRNum=0xFFFF;

xdata	U8	gc_MenuItem=0;

#ifdef Contrast_Set
xdata   U8  gc_Contrast = 0;
xdata   U8  gc_Contrast_bak = 0;
#endif

xdata U8 gc_Dosinitfail=0;
//================================
//System

xdata U16 gw_BmpInx;
xdata U8 gc_BmpString[1536];

//xdata U16 gw_font_sector=538;
//xdata U16 gw_icon_sector=5036;


data U8 gc_KeyDet_Mask=0;  //20090107 chiayen modify

data U8 gc_LongKeyCount=0;

xdata U8 gc_Vrefinit_Timer=0;
xdata U8 gc_KeyValue=0;
xdata U8 gc_KeyEvent;
data U8 gc_PhaseInx=0;
xdata U8 gc_RepPlayMode=2;
xdata U16 gw_TotalSec=0;
xdata U8 gc_T0IntervalLSB=C_T0_12MhzLSB;
xdata U8 gc_T0IntervalMSB=C_T0_12MhzMSB;

U8 gc_key_Pressed=0;
U8 gc_key_PrevPressed=0;
//==========================================
//ISR
bit gbLKeyTimer_Timeout=0;
bit gbGetKey_Timeout=0;

data bit gb_IR_Setting=0;
data bit gb_USB_ISR_Detected=0; 
data U8 gc_IRCmdStatus=0;
data U8 gc_ircount=0;
data U8 gw_ChangeFrequencyTimer=0;
data U8 gw_IR_Setting_Timer=0;
data U8 gw_LogData_Timer=0;  //chiayen0812
xdata U8 gc_LogDataFlag=0;//chiayen0812 
xdata U16 gw_Random_Timer=0;
xdata U8 gc_XTimeBase=0;
xdata U8 gc_LongKeyTimer=0;
xdata U8 gc_LongKeyDelay=0;
data	bit	gb_Frequency_Song=0;  //20090309 chiayen modify
data	bit	gb_SelectEQ=0;  //20090309 chiayen modify
//data	bit	gb_Err=0;  //20090309 chiayen modify
data	bit	gb_PickSong=0;  //20090309 chiayen modify  
data	bit	gb_ChannelSet=0;  //20090309 chiayen modify  
data	U8	gc_ShowTimer=0;  //20090309 chiayen modify  
data	U16	gw_DisplayFreq=0;  //20090309 chiayen modify  
data	U16	gw_DisplayTime=0;  //20090309 chiayen modify
data	U16	gw_DisplayBitrateTime=0;
data	U16	gw_LCMScrollTimer=0;      //20090309 chiayen modify
//=====================================
//Logdata
data  U16 gw_LogFileIndex=0;
data  U16 gw_SDLogFileIndex=0;
data  U16 gw_USBLogFileIndex=0;
xdata U8  gc_UIblock0_index=0;
xdata U16 gw_DIRFileIndex=0;
xdata U16 gw_DIRFileTotalNumber=0;
xdata U32 gdw_DIRFDB_StartCluster=0;

xdata U16 gw_SDDIRFileIndex=0;  //20090216 chiayen add
xdata U16 gw_SDDIRFileTotalNumber=0;
xdata U32 gdw_SDDIRFDB_StartCluster=0;

xdata U16 gw_HostDIRFileIndex=0;
xdata U16 gw_HostDIRFileTotalNumber=0;
xdata U32 gdw_HostDIRFDB_StartCluster=0;  //20090216 chiayen add
//======================================
//IR
data bit gb_PickSongSet=0;
data U8 gc_Dirchange_Timer=0; //20090121 chiayen add
data U8 gw_IR_Timer=0;
data U16 gw_irkey_count=0;
xdata U16 gw_SongNumber=1;  //temp
xdata U16 gw_SongNumberSD;

xdata U8 gc_VolumeMute=0;
xdata U8 gc_NUMBER[4]={0xFF,0xFF,0xFF,0xFF};
xdata U8 gc_irkey=0;
xdata U8 gc_IRdetMask=0;
xdata U8 gc_Volumebackup=0;

xdata	U16	gw_DisplayFreq1=0;
xdata	U8	gc_DisplayEQIcon=0xFF;  //20090309 chiayen modify
xdata	U16	gw_DispSongNum1=0xFFFF; //20090121 chiayen test  //20090309 chiayen modify  
data	U16	gw_DispSongNum=0xFFFF;  //20090309 chiayen modify
data	U16	gw_DispTotalSong=0xFFFF;
xdata U8 gc_FolderPlay_Flag=0;  //20090216 chiayen add
//==============================
//Dir
bit		gb_DirPlay_Flag=0;  // 0:PlayAll  1:Play in Dir

data  U8  gc_PlayMenu_Timeout=0;

xdata U8  gc_DirNoFile_Flag=0;
xdata U8  Save_c_FileType[3];
xdata U8  Save_dw_File_StartCluster[3];//lijian.peng 081011 for return to music
xdata U16 gw_TotalFileNum=0;
xdata U16 Save_gw_MusicFileTotalNum;
xdata U16 gw_DirIndexTest[4];
xdata U16 Save_w_FDB_Offset[3];
xdata U16 Save_gw_FileIndex[3];
xdata U16 Save_gw_FileDirIndex[3];
xdata U32 Save_dw_FDB_StartCluster[3];
xdata U32 Save_dw_FDB_Cluster[3];
xdata U32 Save_dw_FDB_LogAdd[3];
xdata U32 Save_dw_LongFDB_LogAdd0[3];
xdata U32 Save_dw_LongFDB_LogAdd1[3];

xdata U16 	gw_TotalNumber=0;		//count dir+file number in one dir
xdata U16 	gw_DirTotalNumber=0;	//count dir total number in one dir
xdata U16 	gw_DirOverallNumber=0;  //count dir overall number, //20090216 chiayen add
xdata U16 	gw_AllDirTotalNumber=0;
xdata U16	gw_DIRtableIndex=0;
xdata U16 	gw_Index=0;


xdata U8 gc_test=0;
xdata U8 gc_SDSetIndex=0;  //chiayen 20090320
xdata U8 gc_TaskMode_BkUp=1;//(JC)I0304 //20090320 chiayen test
bit	gbt_SD_BuildDIRFlag=0; //Ching 090306

data	 U8		gc_LCM_Media=0;  //20090309 chiayen modify
xdata	 U8		gc_DisplayPlayMode=0xFF;  //20090309 chiayen modify


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
data	 U16	gw_DisplayBitRate=0xFFFF; //20090309 chiayen modify

xdata	U8	gc_PlayMenu_IR=0;  //20090206 chaiyen add for IR_DIR
xdata	U8	gc_strobe_bak;
xdata	U8  gc_0xB400_bak; 
xdata	U8	gc_LCMDispIndex=0;
xdata	U8	gc_FatherIndex=0;
xdata	U8	gc_DirReBuildFlag=0;
xdata	U8	gc_SetIndex=0;		//for rolling mechanism
xdata	U8	gc_SetNumber=0;
xdata	U8	gc_EQChangeFlag=0;
xdata	U16	gw_FM_frequency=875;  // range=87.7MHz~107.9MHz  //20090309 chiayen mark

//========================sunzhk add 0703====================//
data	U8	gc_refresh_type=0;//0:refresh all ,1:next op refresh,2:prev op refresh 0xff:not refresh
//xdata   U8  gc_Contrast;	//对比度设置界面

xdata	U16	gw_CurrentSec=0;
xdata	U16	gw_CTStartTime=0;
xdata	U8	gb_fffr_end=0;

xdata U8 gc_folder_disp=0;


xdata U32 gdw_ID3_v2_sz;	//Jimi 0901027
