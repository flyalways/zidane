#include "define.h"
#include "dos\fs_struct.h"


data	bit	gb_FindFile;
data	bit	gb_TriggerFileSkip;
data	bit	gb_ReadWriteDataArea;//select the area of read/write with flash
data	bit	gb_CountDirFlag;
data	bit	gb_LrcGetTimeOrNot;
data	bit	gb_LrcFileName_Exist;
data	bit	gb_FindFlag;
data	bit	gb_NeedEraseUpdateBlock;
data	U8	gc_CurrentCard;

xdata	U8	gc_Task_Current;
xdata	U8	gc_Task_Next;
xdata	U8	gc_FileLongName[256];
xdata	U8	gc_DOS_SectorPerCluster;
xdata	U8	gc_DOS_FileSystemType;
xdata	U8	gc_ClusBufValidSize;		//!< The number of free clusters in gdw_ClusterBuffer[].
xdata	U8	gc_ClusBufValidPoint;		//!< The index of gdw_ClusterBuffer[] which is last read.
xdata	U16	gw_FileIndex[3];
xdata	U16	gw_FileTotalNumber[3];
xdata	U32	gdw_DOS_RootDirClus;
xdata	U32	gdw_DOS_FatMaxCluster;
xdata	U32	gdw_DOS_DataAddr;
xdata	U32	gdw_DOS_RootDirAddr;
xdata	U32	gdw_DOS_SectorPerFAT;
xdata	U32	gdw_DOS_Fat1Addr;
xdata	U32	gdw_DOS_Fat2Addr;
xdata	U32	gdw_TotalFreeClusNumber;
xdata	U32	gdw_FreeClusterNum[2];
xdata	U32	gdw_ClusterBuffer[8];		//!< FAT free cluster address buffer managed by gc_ClusBufValidSize & gc_ClusBufValidPoint.
xdata	U32	gdw_CurrFlag;
xdata	U32	gdw_StartCluster2;

xdata File_FCB gs_File_FCB[3];
xdata DIR_FCB gs_DIR_FCB[3];
xdata U16   gw_DirIndex[2];
xdata U8  gc_NeedNewBlock;
xdata U16 gw_FileSkipNumber;	//Ching 080805 - For quick search , if not 0


DSP_GLOBAL gs_DSP_GLOBAL_RAM;

xdata U8 gc_TaskEvent_bFlag=0;
xdata	U16	gw_LrcOffset;

xdata	U32	gdw_LrcFilePointer;
xdata	U32	gdw_LrcFileSize;
xdata	U32	gdw_LrcFileCurrentCluster ;
xdata	U32	gdw_LrcFileStartCluster ;
xdata	U8	gc_LrcCurrentLen;
xdata	U8	gc_LrcDisplayBuf[LRC_DISPLAY_LYRICS_CHARNUMBER];

xdata U16 gw_PrevTimeSec=0xffff;

xdata U16 gw_init_needed;

