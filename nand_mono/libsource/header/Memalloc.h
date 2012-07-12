extern	data	bit	gb_FindFile;
extern	data	bit	gb_TriggerFileSkip;
extern	data	bit	gb_ReadWriteDataArea;//select the area of read/write with flash
extern	data	bit	gb_FindFlag;
extern	data	bit	gb_CountDirFlag;
extern	data	bit	gb_LrcGetTimeOrNot;
extern	data	bit	gb_LrcFileName_Exist;

extern	data	U8	gc_CurrentCard;
extern	data	U8	gc_DispTimeBuf[];
extern	xdata	U8	gc_Task_Current;
extern	xdata	U8	gc_Task_Next;


extern xdata U16  gw_FileIndex[];
extern xdata U16  gw_FileTotalNumber[];
extern xdata U8   gc_FileLongName[];
extern xdata U8 gc_CurrentExistMedia;
extern xdata U32 gdw_DOS_RootDirClus;
extern xdata U32 gdw_DOS_FatMaxCluster;
extern xdata U8 gc_DOS_SectorPerCluster;
extern xdata U32 gdw_DOS_DataAddr;
extern xdata U8  gc_DOS_FileSystemType;
extern xdata U32 gdw_DOS_RootDirAddr;
extern	xdata U16 	gw_DirTotalNumber;	//count dir total number in one dir

extern xdata U32 gdw_DOS_SectorPerFAT;
extern xdata U32 gdw_DOS_Fat1Addr;
extern xdata U32 gdw_DOS_Fat2Addr;

extern xdata U32     gdw_CurrFlag ;
extern xdata U32     gdw_StartCluster2 ;

extern xdata   U16 gw_init_needed;

extern xdata File_FCB  gs_File_FCB[];
extern xdata DIR_FCB gs_DIR_FCB[3];
extern xdata U16 gw_DirIndex[2];
extern	xdata	U16	gw_FileSkipNumber;

extern xdata U8  gc_NeedNewBlock;
extern bit  gb_NeedEraseUpdateBlock;
extern xdata U32 gdw_CurrFlag;
extern xdata U32 gdw_StartCluster2;

extern DSP_GLOBAL gs_DSP_GLOBAL_RAM;

extern xdata U8 gc_TaskEvent_bFlag;

extern xdata	U32	gdw_LrcFilePointer;
extern xdata	U32	gdw_LrcFileSize;
extern xdata	U32	gdw_LrcFileCurrentCluster ;
extern xdata	U32	gdw_LrcFileStartCluster ;
extern xdata	U16	gw_LrcOffset;
extern xdata	U8	gc_LrcCurrentLen;
extern xdata	U8	gc_LrcDisplayBuf[LRC_DISPLAY_LYRICS_CHARNUMBER];
extern	xdata	U16	gw_PrevTimeSec;


