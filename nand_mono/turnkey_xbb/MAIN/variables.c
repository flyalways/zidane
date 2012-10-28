#include "..\header\variables.h"
#include "..\header\UI_CONFIG.h"


data	bit	gb_DirPlay_Flag;  // 0:PlayAll  1:Play in Dir

data	bit	gb_SDNoFileflag=0; 
data	bit	gb_SD_Exist=0;
data	bit	gb_SD_Exist_pre=0;
data	bit	gb_Host_Exist=0;
data	bit	gb_Host_Exist_pre=0;
data	bit	gb_HostNoFileflag=0;

data	bit	gb_FindFlag;
data	U8	gc_IRCmdStatus=0;

data	U8	gc_PlayMenu_Timeout=0;

xdata	U8	gc_HostSectorUnit=1;
xdata	U16	gw_DIRFileIndex=0;
xdata	U16	gw_LogFileIndex=0;
xdata	U16	gw_SDLogFileIndex=0;
xdata	U16	gw_USBLogFileIndex=0;
xdata	U16	gw_nCompressionLevel; //Jimi 091118

xdata	U16	gw_Disp_CurrentSec=0;
xdata	U8	gc_XTimeBase=0;                 // Sequence index of handles in timer0 ISR.
data	U8	gc_LongKeyTimer=0;              // Count of T0 period: how long has this key been pressed.
data	U8	gc_LongKeyDelay=0;              // Setpoint of timing length of a long key. Default is MAX_REPEATKEYDELAY.

xdata	U8	gc_UIblock0_index=0;
xdata	U8	gc_Dosinitfail=0;
xdata	U16	gw_HostDIRFileIndex=0;
xdata	U16	gw_HostDIRFileTotalNumber=0;
xdata	U16	gw_SDDIRFileIndex=0;  //20090216 chiayen add
xdata	U16	gw_SDDIRFileTotalNumber=0; 
xdata	U16	gw_Random_Timer=0;
xdata	U16	gw_DIRFileTotalNumber=0;
xdata	U32	gdw_DIRFDB_StartCluster=0;
xdata	U32	gdw_SDDIRFDB_StartCluster=0;
xdata	U32	gdw_HostDIRFDB_StartCluster=0;  //20090216 chiayen add

xdata	U8	gc_NUMBER[4]={0xFF,0xFF,0xFF,0xFF};	// 20100406 - First time IR press number to select song fail

xdata	U8	gc_DirNoFile_Flag=0;
xdata	U8	Save_c_FileType[3];
xdata	U8	Save_dw_File_StartCluster[3];
xdata	U8	gc_DIRTimeOut_Flag=0;
xdata	U16	Save_gw_MusicFileTotalNum;
xdata	U16	gw_DirIndexTest[4];
xdata	U16	Save_w_FDB_Offset[3];
xdata	U16	Save_gw_FileIndex[3];
xdata	U16	Save_gw_FileDirIndex[3];
xdata	U32	Save_dw_FDB_StartCluster[3];
xdata	U32	Save_dw_FDB_Cluster[3];
xdata	U32	Save_dw_FDB_LogAdd[3];
xdata	U32	Save_dw_LongFDB_LogAdd0[3];
xdata	U32	Save_dw_LongFDB_LogAdd1[3];

xdata	U16	gw_TotalNumber=0;
xdata	U16	gw_DirTotalNumber=0;	//count dir total number in one dir
xdata	U16	gw_DirOverallNumber=0;  //count dir overall number, //20090216 chiayen add
xdata	U16	gw_DIRtableIndex=0;
xdata	U16	gw_Index=0;

xdata	U8	gc_FatherIndex=0;
xdata	U8	gc_SetIndex=0;
xdata	U8	gc_SetNumber=0;

xdata	U8	gc_Vrefinit_Timer=0;                // Times of timer peroid. The initial value is 60.
                                                // It means, after 60*timer, we will poll the task event.

xdata	U8	gc_KeyValue=0;                      // Cooked "key pressed" info including short and long keys.

xdata	U8	gc_KeyEvent;                        // Initialized by gc_KeyValue, but cooked again to be the index
                                                // in various task phase tab.

xdata	U8	gc_PhaseInx=0;                      // Task's phase. Be the index of action.

xdata	U16	gw_CurrentSec=0;                    // The latest time point in current song.
xdata	U16	gw_TotalSec=0;                      // The length of time of a song
xdata	U8	gc_T0IntervalLSB=C_T0_12MhzLSB;
xdata	U8	gc_T0IntervalMSB=C_T0_12MhzMSB;

data	U8	gc_key_Pressed=0;
data	U8	gc_key_PrevPressed=0;
data	U16 gw_DispFileName_ByteOffset=0;

xdata U32 gdw_ID3_v2_sz;	//Jimi 0901027
xdata U8 gc_language_offset;//多国语言的偏移（第几国语言）

xdata U8 gc_Num_Flag=0;
