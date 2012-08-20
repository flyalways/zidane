#ifndef _PROTOTYP_H_
#define _PROTOTYP_H_

/* -------------------------------------------------------------------------
   USB.C
   ------------------------------------------------------------------------- */
extern  void USB_Mode(void);
extern 	void PowerOn_Init_USB(void);
extern	void USBInitialize(void);
extern	void USER_DelayDTms(U16 DT);
extern 	void Fill_EndpointDataTable_Into_ExRamBase(void);
extern  void USB_I2C(void);
extern  void I2C_M_Tx(UBYTE ID, UBYTE Addr , UBYTE Wdata );
extern  void I2C_Clr_Int(void );
extern 	void I2C_Wait_Intpend(void );
extern  void I2C_M_Rx(UBYTE ID, UBYTE Addr);
/* -------------------------------------------------------------------------
   system.c
 --------------------------------------------------------------------------*/
extern void main(void);
extern void HostISR(void);
extern void Timer0(void);
extern void Init_CPU(void);
extern void Init_LCD(void);
extern void Init_Storage(void);
extern void Wait_PowerOn(void);
extern void Detect_Card_Process(void);
extern void USB_Mode_Disp(void);
extern void USB_On(void);
extern int  dbprintf(char *fmt, ...);
extern void USB_PlugDetect(void);
extern void TRI_DMA_BUF(BYTE DMA_DIRECTION,WORD DMA_ADDR,WORD DMA_SIZE);
extern bit Search_BankCode_Block(void);
/* -------------------------------------------------------------------------
  SD_HW.C
 --------------------------------------------------------------------------*/
extern  void    SD_Interface_Initial(void);
extern  void    SD_CMD0_Reset(void);
extern  void    SD_CMD55_PreACmd(void);
extern  bit     SD_Check_Idle(void);
extern  bit     SD_ACmd41_OCR(void);
extern  void    SD_Cmd2_CID(void);
extern  bit  	SD_Identification_Flow(void);
extern  void 	SD_Send_DummyCLK(UBYTE CLK_Num);
extern  UBYTE 	SD_Receive_Respond(UBYTE *tc_RespBuff,UBYTE tc_Resptype136bit, UBYTE tc_CRC);
extern  void 	SD_Trigger_CMD(void);
extern 	void 	SD_Cmd3_RCA(void);
extern  void 	SD_Cmd9_CSD(void);
extern  void 	SD_Cmd7_Select(void);
extern  void 	SD_Cmd13_Status(void);
extern  void 	SD_ACmd6_BusWidth(bit _4bit);
extern  void 	SD_Cmd16_Blocklen(void);
extern  void 	SD_ACmd51_SCR(void);
extern  void 	SD_Cmd12_Stop(void);
extern 	void 	SD_Cmd8_Send_If_Cond(void);
extern  bit 	SD_Card_Detect(void);
extern  void 	SD_TrigCMD_Receive_Respond(void);
//extern  void 	SD_Cmd6_SwitchFunc(void); 
extern  bit 	MMC_Cmd01_OCR(void);
extern  bit     MMC_Cmd01_OCR_Wait_Ready(void);
extern  void  	SD_MMC_Check_Mode(void);
/* -------------------------------------------------------------------------
  SD_CMD.C
 --------------------------------------------------------------------------*/
extern void 	SD_ReadSectors_USB(void);
extern void 	SD_WriteSectors_USB(void);
extern void 	SD_Read_LBA_Dos(LWORD Read_LBA ,UBYTE Buf_Index);
extern void 	SD_Write_LBA_Dos(LWORD Write_LBA,UBYTE Buf_Index,UBYTE Write_Stage);

/* -------------------------------------------------------------------------
   ISR.C
 --------------------------------------------------------------------------*/

/* -------------------------------------------------------------------------
   USB_EP0.C
 --------------------------------------------------------------------------*/
extern	void 	Control_Transfer_Process(void);
extern	void 	Read_Setup_Packet(void);
extern	void 	StandardCommand(void);
extern	void 	get_status(void);
extern	void 	clear_feature(void);
extern	void 	set_feature(void);
extern	void 	set_address(void);
extern	void 	get_descriptor(void);
extern	void 	set_descriptor(void);
extern	void 	get_configuration(void);
extern	void 	set_configuration(void);
extern	void 	get_interface(void);
extern	void 	set_interface(void);
extern	void 	synch_frame(void);
extern 	void 	VendorCommand(void);
/* -------------------------------------------------------------------------
   USB_BULK.C
 --------------------------------------------------------------------------*/
extern	void 	Save_CBW_Packet(void);
extern	void 	Decode_CBW_Command(void);
extern	void 	SetResponseINQUIRYData(void);
extern	void 	SetRequestSenseData(void);
extern	void 	SetReadFormatCapacity(void);
extern	void 	SetPartitionCapacity(void);
extern	void 	SetReadCapacityData(void);
extern	bit 	Check_LBA_ID(void);
extern	void 	StallBulkRW(void);
extern	void 	SetTOC_PMA_ATIP_Data(UBYTE tc_data);
/* -------------------------------------------------------------------------
   USB_CMDS.C
 --------------------------------------------------------------------------*/
extern	void	LBA2DBP(void);
extern	void	ReadSectors(void);
extern	void	NoCard_ReadSectors(void);
extern	void	ReadATABlock(void);
extern	void	WriteSectors(void);
extern	void	NoCard_WriteSectors(void);
extern	void	WriteATABlock(void);
extern	void	SetHBlockToHDev(void);
extern	void	SetHBlock_To_WhichGroup(void);

/* -------------------------------------------------------------------------
   FEPR.C
 --------------------------------------------------------------------------*/
extern	bit	    FlashReadID(void);
extern	void	FlashReset(void);
extern	void	HPageToFPage_InAddr(void);
extern	void	CHK_Flash_Ready(void);
extern	void	FBlock_FPage_To_PhyAddr(void);
extern	void	Flash_ReadSpare(U8 cSbyte);
extern	void	NAND_FBlock_FPage_FBank_To_PhyAddr_NAND(void);
extern	U8	    Flash_ReadPage(void);
extern  void    FLASH_MultReadPage(void);
extern	U8	    Flash_EraseOneBlock(void);
extern	void	Flash_WritePage(void);
extern  void    FLASH_MultWritePage(UBYTE Mark);
extern	void	Flash_ReadFullPage(bit ECC);
extern	void	Flash_WriteFullPage(bit ECC);
extern	void	SET_DEVICE_READ(void);
extern	void	SET_DEVICE_WRITE(void);
extern	void	Trigger_Data_Transfer_Done(void);
extern	void	Trigger_Data_Transfer(void);
extern	void	CHK_Data_Transfer_Done(void);
extern	void	Reset_DMA(void);

extern	void	TRIGGER_FLASH_CMD_COL_ROW_ADDR_CMD(void);
extern	void	TRIGGER_FLASH_LBLOCK_CMD_COL_ROW_ADDR(void);
extern	void	TRIGGER_FLASH_LBLOCK_CMD_COL_ROW_ADDR_CMD(void);
extern	void	TRIGGER_FLASH_SBLOCK_CMD_COL_ROW_ADDR_CMD(void);
extern	void	TRIGGER_FLASH_SBLOCK_CMD_COL_ROW_ADDR(void);
extern	void	TRIGGER_FLASH_CMD_COL_ROW_ADDR(void);
extern	void	TRIGGER_FLASH_CMD_COL_ADDR_CMD(void);
extern	void	TRIGGER_FLASH_CMD(void);
extern  void    Flash_State_Initial(void);
extern  void    NAND_InAddr_To_Column_NAND(void);
extern  void    CHK_NandAutocmp_Done(void);
extern  bit	    Flash_ChkStatus(void);
extern  void    Flash_ReadFactor(void);
extern  void    Flash_WriteFactor(void);
extern  bit     CHK_ECC_Done(void);
extern  void 	Flash_Sequent_Write(UBYTE Secount,bit spare );
extern  bit 	Flash_Sequent_Read(UBYTE Secount);
extern  U8 		Flash_MultiBank_Erase(U8 tbt_CHK_Rdy);
extern  void 	SET_DEVICE_READ_TO_DRAM(void);
extern  void 	FLASH_Backup_Blcok(UWORD StartPage , UWORD EndPage , U8 Spare_Mark, U8 TargetDev);
extern	void	DSPHZK_Block_Read(U8 Area,U16 Offset_Addr,U8 SectorCnt,U8 Buf_Index);
extern	U8		UserData_Block_Read(U8 Block_index,U16 Sector_Addr,U8 SectorCnt,U8 Buf_Index);
extern	U8		UserData_Block_Write(U8 Block_index,U16 Sector_Addr,U8 SectorCnt,U8 Buf_Index);
extern	U8		UserData_Block_Erase(U8 Block_index);
extern  void    Flash_Read_SettingInfo(void);
extern  void    USB_Bank0_BulkOut(UBYTE Buf_Sel);
extern  void    USB_Bank0_BulkIn(UBYTE Buf_Sel);
extern  void  	ENA_ECC_Decode(void);
extern  void  	ENA_ECC_Encode(void);
extern  void 	Flash_EraseOneBlock_NO_ChkRdy(void);
extern  U8	 	CHK_ECCData_FF(void);
//extern  void 	Flash_2Plane_Write(void );
/* -------------------------------------------------------------------------
   FLASH.C
 --------------------------------------------------------------------------*/
extern	void    Init_Flash_Reg(void);
extern	void    Init_Flash_Variable(void);
extern	void    InitFlash(void);
extern	void	UpDateLink(void);
extern	void	WriteMapping(void);
extern	void	SET_FLastBI_0xA5_FAddress(void);
extern	void	NAND_FlashReadSector_Large(void);
extern	void	NAND_FlashWriteSector_Large(void);
extern  void 	RW_Data_Stage(UWORD tw_SourcePage,UWORD tw_TargetPage,U8 tc_Xfe_PageCnt );
extern  void 	NAND_Laod_Data_To_Buf(UWORD tw_StartPage,U8 tc_Bufaddr);

extern  void    Load_Code_From_Flash_To_Ram(void);
//extern  bit 	NAND_Flash_Program_Fail_Handle(UWORD StartPage,UWORD EndPage,bit Mark);
//extern  void 	NAND_Flash_Updata_BMT_Table(void);
//extern  void 	Flash_Get_DataGroupSize(void);
extern  void 	LoadBlock_From_MAP(U8 tc_ptr);
extern 	U8 		CHK_DSBlock_Exist_Or_New(U8 tc_Which_D,U8 tc_ExtLBLK, bit tbt_Need_New);
extern  void 	NAND_HPage_To_SBlock_SPage(void);
extern  void 	Flash_Close_SmallBlock(bit tbt_Dummy);
extern  void 	NAND_Combin_E_D_To_New_E(U8 tc_Inx ,U8 tc_END_SBlock);
extern  void 	NAND_Erase_Block_LEDR(U8 tc_Inx);
extern  void 	NAND_Clear_D_SBlock_Flag(U8 tc_group);
extern  void 	NAND_Combin_H_E_D_To_New_H(U8 tc_Inx);
extern 	U8	 	CHK_RandomPage_Exist(U8 tc_Inx,U16 tw_HPage);
extern  void 	NAND_Clear_R_PageFlag(U8 tc_group);
extern 	void 	NAND_Combin_H_R_To_New_H(U8 tc_Inx);
extern  U8 		CHK_RandomPage_Space(U8 tc_Inx);
extern  void 	NAND_Close_Random_Block(void);
extern  void 	Load_E_D_Block_From_MAP(U8 tc_ptr,U8 tc_Inx);
extern  void 	NAND_Close_E_SBlock(U8 tc_Inx);
extern  void 	NAND_Fill_H2E_SBlock(U8 tc_Inx,U8 tc_END_SBlock);
extern  void 	NAND_Fill_H2E_SPage(U8 tc_Inx,U8 tc_END_SPage);
extern  void 	NAND_Update_PWR_INX(U8 tc_Inx);
extern  void 	NAND_Set_Spare_Data(U8 tc_Type,U8 tc_Inx);
extern  void 	NAND_Set_FBlock(U8 tc_Inx);
extern  void 	USB_Bank0_1_BulkOut(U8 Use_Bank1 ,U8 Xfer_1024);
extern 	void 	USB_CHK_Bank0_1_BulkOut_Done(U8 Chk_Bank1);
extern  void 	USB_Rcevie_Data_To_BUf(U8 tc_BufPtr , U8 tc_Xfercnt);
extern  void 	NAND_SBlock_SPage_To_FDBP(void);
extern  void 	NAND_OverWrite_Mode(U8 tc_Extend_Count);
extern  void 	NAND_SequentWrite_Mode(U8 tc_Extend_Count);
extern  void 	NAND_E_D_ReadSector(U8 tc_Extend_Count);
extern 	void 	NAND_Set_Mark_Register(U8 tbt_2nd_data);
extern 	void 	USB_To_BUf(U8 tc_BufPtr , U8 tbt_1024);
extern  void 	NAND_Close_Dummy_Block(U8 tc_Inx);
extern  void 	NAND_Write_Q1_Mark(U8 tc_Inx);
/* -------------------------------------------------------------------------
   FLSHSCAN.C
 --------------------------------------------------------------------------*/
extern	void	SearchBMT(void);
extern	void 	LoadFirstTable(void);
extern	void	Create_Zone_Lookup_Table(void);
extern	void	Chk_infor(void);
extern	void 	Search_Q1Q2_Table(U8 tc_ExtInx);
extern	void 	Search_Random_Page(void);
extern  void 	Search_Dummy_Page(U8 tc_ExtInx);
extern  void 	Check_LoadMap(void);
extern  void 	Flash_AutoBlankCheck(void);
extern  void 	Flash_AutoMarkGoodTag(void);
extern  void 	Flash_AutoMapping(void);
extern  void 	Flash_AutoRecovery(void);
extern  void 	Flash_Set_Spare_Data( UBYTE Pattern_A ,UBYTE Pattern_B);
extern  U16     Search_Last_Page(U16 TBlock);//090820  Jay add




extern  U16     Flash_Search_ReplaceBlock_Index(void);
extern  void  	NAND_Update_WearBlock (U8 tc_Dev);
extern  void    Flash_Set_Feature(U8 cAddr,U8 cdata );
extern  void 	NAND_ZONE_To_BMT_DBP(U16 tw_ZoneNum);
extern	void 	NAND_Recover_Block(U16 tw_Inx, U8 tc_Recover_Type );// 100617 Jay add
/* -------------------------------------------------------------------------
   St_cmds.C
 --------------------------------------------------------------------------*/
extern	void	SelfTestCmd(void);
extern	void	Flash_Block_Addr(void);
extern	void	Flash_Structure_Addr(void);
extern  void 	LoadInit(void);
extern  void 	DOS_Read_Write_Structure(void);
extern 	void 	ProgramRam_To_USB(void);
extern 	void 	USB_To_ProgramRam(void);

/*---------------------------------------------------------------------------
	Dos.C
----------------------------------------------------------------------------*/
extern 	bit 	Dos_Read_LBA(LWORD Read_LBA ,UBYTE Buf_Index);  // 081111 Jay fix
extern 	void 	Dos_Write_LBA(LWORD Write_LBA,UBYTE Buf_Index,UBYTE Write_Stage);	
extern 	bit 	NAND_FlashReadSector_DOS(void);
extern 	void 	Move_RAM(UWORD RAM_Source,UWORD RAM_Destination,UWORD RAM_Size);
//---------------------------------------------------------------------------
// dosinit.c
//---------------------------------------------------------------------------
extern  UBYTE DOS_Initialize(void);
extern  UBYTE JudgeBootSector(void);
//extern UBYTE X_Unicode2ISN(unsigned char *UnicodeArray, bit Convert);  //20081003 mark

//---------------------------------------------------------------------------
// dosrw.c
//---------------------------------------------------------------------------
extern  UBYTE DOS_Read_LogicSector(LWORD tdw_LogicSectorAddr, UBYTE tc_ReadSectorNumber);
extern  UBYTE DOS_Write_LogicSector(LWORD tdw_LogicSectorAddr, UBYTE tc_WriteSectorNumber, UBYTE tc_AppendW);

//---------------------------------------------------------------------------
// dosfile.c
//---------------------------------------------------------------------------
extern  UBYTE	DOS_Search_File(UBYTE tc_Mode, UBYTE tc_Type, UBYTE tc_PrevOrNext);
extern  UBYTE DOS_Read_File(UBYTE tc_FileHandle);
extern  UBYTE DOS_Write_File(UBYTE tc_FileHandle, UBYTE tc_tc_AppendW);
extern  UBYTE DOS_Open_File_r(UBYTE tc_FileHandle, UBYTE tc_OpenMode, UBYTE * tpc_FileName);
extern  UBYTE DOS_Open_File_w(UBYTE tc_FileHandle);
extern  void DOS_Close_File_r(UBYTE tc_FileHandle);
extern  UBYTE DOS_Close_File_w(UBYTE tc_FileHandle, UBYTE *tpc_FileName);
extern  void DOS_Init_gc_PlayRecordDataBuf(UBYTE Value);
extern  LWORD DOS_Seek_File(UBYTE tc_FileHandle, LWORD tdw_SectorNumber);

extern void DosFreeClusterCalculate(UBYTE tc_FileHandle, UBYTE tc_ClusterInc);

//---------------------------------------------------------------------------
// dosfdb.c
//---------------------------------------------------------------------------
extern  UBYTE USER_FindDir(UBYTE tc_UserDirNum, LWORD tdw_DirStartCluster);
extern  void USER_Create_Dir(UBYTE tc_UserDirNum, LWORD tdw_DirStartCluster);
extern  UBYTE DOS_AddNewFdb(UBYTE *tpc_FdbEntry, LWORD tdw_DirStartCluster);
extern  void DOS_FillFdb(UBYTE *tpc_Fdb, UBYTE * tpc_Name, LWORD tdw_StartCluster, LWORD tdw_FileSize);
extern  UBYTE DOS_GetLongFileName(UBYTE tc_FileHandle, UBYTE * tpc_LongName);
extern  LWORD DOS_MakeDir(UBYTE * tpc_DirName, LWORD tdw_DirStartCluster);
extern  UBYTE DOS_WriteDirInitData1(LWORD tdw_CurrentStart, LWORD tdw_ParentStart);

//---------------------------------------------------------------------------
// dosfat.c
//---------------------------------------------------------------------------
extern UBYTE  DOS_ClearFATChain(LWORD tdw_StartCluster);
extern LWORD  DOS_ClusterLogicAddr(LWORD tdw_Cluster);
extern UBYTE  DOS_ConnectFATChain(LWORD tdw_StartCluster, LWORD tdw_TailCluster);
extern UBYTE  DOS_CopyFATtoFAT(LWORD tdw_StartCluster, LWORD tdw_TailCluster, UBYTE tbt_fat1tofat2);
extern LWORD  GetValidClusterCnt(void);
extern LWORD  DOS_GetNextCluster(LWORD tdw_CurrentCluster, LWORD tdw_ClusterNumber);
extern UBYTE  DOS_MarkFATUpdate(LWORD tdw_MarkFATAddr,UBYTE tc_Value);
//extern UBYTE  DOS_SearchFreeCluster(UBYTE tc_SearchMode);

//---------------------------------------------------------------------------
// free_cluster.c
//---------------------------------------------------------------------------
extern void  count_free_cluster_fat16(UBYTE xdata * p, UBYTE size1, UBYTE size2);
extern UBYTE count_free_cluster_in_fat16_sector(UBYTE xdata * p, UBYTE size);
extern UBYTE count_free_cluster_in_fat32_sector(UBYTE xdata * p, UBYTE size);
extern UBYTE fat16_free_cluster_offset(UBYTE xdata * p, UBYTE size);
extern UBYTE fat32_free_cluster_offset(UBYTE xdata * p, UBYTE size);
extern UBYTE free_clusters_in_sector(UBYTE xdata * p, UBYTE size, UBYTE cluster_entry_size, LWORD tdw_FreeClusterNum);

//---------------------------------------------------------------------------
// Test_Print1.c
//---------------------------------------------------------------------------
extern bit FLASH_MultRead_DRAM(bit tbt_Backup, UBYTE tc_FPage, UBYTE tc_Block1, UBYTE tc_Block0, UBYTE tc_PageNum);


//===========UI TEST=============
extern void Idle_Task(void);
extern void Play_Task(void);
extern void Jpeg_Task(void);
extern void Mjpeg_Task(void);
extern void Menu_Task(void);
extern void PlayMenu_Task(void);

//-------------------------------
extern void play_init(void);
extern void play_proc(void);
extern void play_endfile(void);
extern void play_playpause(void);
extern void play_stop(void);
extern void play_next(void);
extern void play_prev(void);
extern void play_fastfwd(void);
extern void play_fastrev(void);
extern void play_idle(void);
extern void play_fffr_end(void);
extern void play_set_repAB(void);
extern void play_volup(void);
extern void play_voldn(void);
extern void play_back2uplevel(void);
extern void play_menu(void);
extern void play_vol(void);

extern void FM_FREQ_CHGUP(void);  //chiayen0807
extern void FM_FREQ_CHGDOWN(void);  //chiayen0807
extern void PlayMode_CHG(void);  //chiayen0807

extern void rec_idle(void);
extern void rec_init(void);
extern void rec_proc(void);
extern void rec_endfile(void);
extern void rec_recpause(void);
extern void rec_stop(void);

extern void voice_idle(void);
extern void voice_init(void);
extern void voice_proc(void);
extern void voice_endfile(void);
extern void voice_playpause(void);
extern void voice_stop(void);
extern void voice_next(void);
extern void voice_prev(void);
extern void voice_fastfwd(void);
extern void voice_fastrev(void);
extern void voice_fffr_end(void);
extern void voice_volup(void);
extern void voice_voldn(void);
extern void voice_set_repAB(void);
extern void voice_back2uplevel(void);
extern void voice_menu(void);

extern void jpeg_idle(void);
extern void jpeg_proc(void);
extern void jpeg_endfile(void);
extern void jpeg_playpause(void);
extern void jpeg_stop(void);
extern void jpeg_next(void);
extern void jpeg_prev(void);
extern void jpeg_back2uplevel(void);


extern void mjpeg_idle(void);
extern void mjpeg_proc(void);
extern void mjpeg_playpause(void);
extern void mjpeg_stop(void);
extern void mjpeg_endfile(void);
extern void mjpeg_next(void);
extern void mjpeg_prev(void);
extern void mjpeg_fastfwd(void);
extern void mjpeg_fastrev(void);
extern void mjpeg_fffr_end(void);
extern void mjpeg_volup(void);
extern void mjpeg_voldn(void);
extern void mjpeg_back2uplevel(void);
extern void fmr_back2uplevel(void);

extern void fmr_idle(void);
extern void fmr_proc(void);
extern void fmr_stop(void);
extern void fmr_volup(void);
extern void fmr_voldn(void);
extern void fmr_nextch(void);
extern void fmr_prevch(void);

extern void Menu_loop(void);

extern void menu_init(void);
extern void menu_proc(void);
extern void menu_idle(void);
//extern void menu_next(void);
//extern void menu_prev(void);

extern void phoneBook_init(void);
extern void phoneBook_proc(void);

extern UBYTE MP3_Download(void);
extern UBYTE WMA_Download(void);

extern UBYTE MP3_DataIn(void);
extern UBYTE WMA_DataIn(void);

extern UBYTE MP3_FF_FR_Cmd(void);

extern WORD MP3_ReadTime(void);
extern WORD WMA_ReadTime(void);

extern WORD MP3_Bitrate(void);
extern WORD WMA_Bitrate(void);

extern void MP3_RepeatAB(void);
extern void WMA_RepeatAB(void);

extern UBYTE MP3_EOF_Proc(void);
extern UBYTE WMA_EOF_Proc(void);

extern WORD MP3_file_total_sec(void);
extern WORD WMA_file_total_sec(void);

extern void MP3_seek_trig(void);		//Jimi 080530
extern void WMA_seek_trig(void);


extern void	set_clock_mode(U8 mode);

//=====================================
extern void RandomGetFileIndex(void);

extern void Polling_TaskEvents(void);

extern void Detect_USB(void);
extern void Charger_UpSelect(U8 tc_GoalValue);
extern void Charger_DownSelect(U8 tc_GoalValue);
extern void USB_Task(void);
extern void Setting_Task(void);
extern void LoBatt_Task(void);
extern void FSError_Task(void);
extern void DiscFull_Task(void);

extern U8 play_SetTime2DisplayBuf(U16 tw_TimeSec);
extern WORD play_GetTimeFromDisplayBuf(); 
//===================Lyrics===================
extern U8 Lyric_OffsetLRC(void);
extern void Lyric_GetLyrics(void);
extern void Lyric_GetTimeStamp(void);
extern U8 Lyric_FileSearch(void);
extern U8 Lyric_ReadLrcFile(U8 tc_ReadNextSector);

#endif