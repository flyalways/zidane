#ifndef _VARIABLES_H_
#define _VARIABLES_H_

#include "define.h"

extern	data 	bit gb_SDNoFileflag; 
extern	data 	bit gb_SD_Exist;
extern	data 	bit gb_SD_Exist_pre;
extern	data 	bit gb_Host_Exist;
extern	data 	bit gb_Host_Exist_pre;
extern	data 	bit gb_HostNoFileflag;
extern	xdata	U8	gc_HostSectorUnit;
extern	xdata	U16	gw_Disp_CurrentSec;
extern	xdata	U8 gc_Dosinitfail;

extern	xdata	U8	gc_IRCmdStatus;
extern	xdata	U16	gw_Random_Timer;
extern	xdata	U8	gc_XTimeBase;
extern	data	U8	gc_LongKeyTimer;
extern	data	U8	gc_LongKeyDelay;

extern	xdata	U16	gw_DIRFileIndex;
extern	xdata	U16	gw_LogFileIndex;
extern	xdata	U16	gw_SDLogFileIndex;
extern	xdata	U16	gw_USBLogFileIndex;
extern	xdata	U8	gc_UIblock0_index;
extern	xdata	U16	gw_DIRFileTotalNumber;
extern	xdata	U16	gw_CurrentFolderNum;
extern	xdata	U32	gdw_DIRFDB_StartCluster;
extern	xdata	U16	gw_SDDIRFileIndex;  //20090216 chiayen add
extern	xdata	U16	gw_SDDIRFileTotalNumber;
extern	xdata	U32	gdw_SDDIRFDB_StartCluster;

extern	xdata	U16	gw_HostDIRFileIndex;
extern	xdata	U16	gw_HostDIRFileTotalNumber;
extern	xdata	U32	gdw_HostDIRFDB_StartCluster;  //20090216 chiayen add

extern	xdata	U8	gc_NUMBER[4];

extern	bit		gb_DirPlay_Flag;  // 0:PlayAll  1:Play in Dir

extern	xdata 	U8	gc_PlayMenu_Timeout;
extern	xdata	U8	gc_DirNoFile_Flag;
extern	xdata	U8	Save_c_FileType[3];
extern	xdata	U8	Save_dw_File_StartCluster[3];
extern	xdata	U8	gc_DIRTimeOut_Flag;
extern	xdata	U16	Save_gw_MusicFileTotalNum;
extern	xdata	U16	gw_DirIndexTest[4];
extern	xdata	U16	Save_w_FDB_Offset[3];
extern	xdata	U16	Save_gw_FileIndex[3];
extern	xdata	U16	Save_gw_FileDirIndex[3];
extern	xdata	U32	Save_dw_FDB_StartCluster[3];
extern	xdata	U32	Save_dw_FDB_Cluster[3];
extern	xdata	U32	Save_dw_FDB_LogAdd[3];
extern	xdata	U32	Save_dw_LongFDB_LogAdd0[3];
extern	xdata	U32	Save_dw_LongFDB_LogAdd1[3];

extern	xdata	U16	gw_TotalNumber;
extern	xdata	U16	gw_DirTotalNumber;
extern	xdata	U16	gw_DirOverallNumber;
extern	xdata	U16	gw_DIRtableIndex;
extern	xdata	U16	gw_Index;

extern	xdata	U8	gc_FatherIndex;
extern	xdata	U8	gc_SetIndex;		//for rolling mechanism
extern	xdata	U8	gc_SetNumber;

extern	xdata	U8	gc_Vrefinit_Timer;
extern	xdata	U8	gc_KeyValue;
extern	xdata	U8	gc_KeyEvent;
extern	xdata	U8	gc_PhaseInx;
extern	xdata	U8	gc_RepPlayMode;
extern	xdata	U16	gw_CurrentSec;
extern	xdata	U16	gw_TotalSec;
extern	xdata	U8	gc_T0IntervalLSB;
extern	xdata	U8	gc_T0IntervalMSB;
extern	xdata	U8	gc_key_Pressed;
extern	xdata	U8	gc_key_PrevPressed;

extern	xdata	U16 gw_DispFileName_ByteOffset;


extern	data	bit	gb_FindFlag;

extern	xdata U32 gdw_ID3_v2_sz;	//Jimi 0901027
extern code U8 LanguageTable[];
extern U8 gc_language_offset;
extern xdata U8 gc_Num_Flag;

#endif