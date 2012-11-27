#include "define.h"
#include "dos\fs_struct.h"


data	bit	gb_FindFile;
data	bit	gb_TriggerFileSkip;
data	bit	gb_ReadWriteDataArea;//select the area of read/write with flash
data	bit	gb_CountDirFlag;
data	bit	gb_LrcGetTimeOrNot;
data	bit	gb_LrcFileName_Exist;

data	U8	gc_CurrentCard;
data	U8	gc_DispTimeBuf[3];
xdata	U8	gc_Task_Current;
xdata	U8	gc_Task_Next;
xdata U16 gw_FileIndex[3];
xdata U16 gw_FileTotalNumber[3];                // NR of files in the current dir. It means recursively.
xdata U8 gc_FileLongName[512] _at_ 0x7500;
xdata U8 gc_CurrentExistMedia;
xdata U32 gdw_DOS_RootDirClus;
xdata U32 gdw_DOS_FatMaxCluster;
xdata U8 gc_DOS_SectorPerCluster;
xdata U32 gdw_DOS_DataAddr;
xdata U8 gc_DOS_FileSystemType;
xdata U32 gdw_DOS_RootDirAddr;
xdata U32 gdw_DOS_SectorPerFAT;
xdata U32 gdw_DOS_Fat1Addr;
xdata U32 gdw_DOS_Fat2Addr;

xdata File_FCB gs_File_FCB[3];
xdata DIR_FCB gs_DIR_FCB[3];
xdata U16   gw_DirIndex[2];
xdata U8  gc_NeedNewBlock;
xdata U16 gw_FileSkipNumber;	//Ching 080805 - For quick search , if not 0
bit          gb_NeedEraseUpdateBlock;
data U8 gc_ReadWriteDataArea;//select the area of read/write with flash

xdata U32 gdw_CurrFlag;
xdata U32 gdw_StartCluster2;

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

xdata U16 gw_init_needed;               // Each bit is a flag for various initialization.
                                        // Usually work together with SET_BITx or CLR_BITx.

