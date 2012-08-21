#include "..\header\dos\fs_struct.h"

extern	void	load_nand_bank(void);
extern	void	dbprintf(char *fmt, ...);
extern	void	SPI_Load_Code_To_DRAM(U8 u_Seccnt);
extern  void    SPI_Init(void);
extern  void    SPI_Read_ID(void);
extern  void    SPI_Device_Parameter(void);
extern	void    SPI_Read_Data(unionU16 tw_SPIAddr);
extern	void	SPI_Erase_Block(U8 tc_BlockNum);
extern  void	SPI_UserData_Block_Erase(void);
extern	void	SPI_UserData_Block_Read(U16 Sector_Addr,U8 Buf_Index);
extern	void	SPI_UserData_Block_Write(U16 Sector_Addr,U8 Buf_Index);
extern	void	SPI_Write_Data_ADh(unionU16 tw_SPIAddr);
extern	void	SPI_Write_Data_02h(unionU16 tw_SPIAddr);
extern  void    SPI_DSPHZK_Block_Read(U8 Area,U16 Offset_Addr,U8 Buf_Index);
extern	void	LCM_ReadSPIBMPdata(U16 tw_Addr,U16 tw_Length);
extern	void	Get_LogData_PageIndex(void);
extern	void	Read_LogData(void); 
extern	void	USER_LogFile_ReadWrite(bit tb_ReadOrWrite);
extern	void	Sysetm_Init(void);
extern	void	InitCPURAM(void);
extern	void	InitSystemRAM(void);
extern	void	Variables_Init(void);
extern	void	play_stop(void);
extern	void	play_next(void);
extern	void	play_prev(void);
extern	void	USER_Delay(U16 DT);
extern	void	Reset_DMA(void);
extern	void	SET_DEVICE_READ_TO_DRAM(void);
extern	void	Trigger_Data_Transfer(void);
extern	void	Trigger_Data_Transfer_Done(void);
extern	void	SET_DEVICE_READ(void);
extern	void	SET_DEVICE_WRITE(void);
extern  void	SD_Send_DummyCLK(U8 CLK_Num);
extern  void	SD_Trigger_CMD(void);
extern  void	SD_TrigCMD_Receive_Respond(void);
extern	void	SD_MMC_Check_Mode(void);
extern  void    SD_CMD0_Reset(void);
extern  void    SD_CMD55_PreACmd(void);
extern	void	SD_Cmd7_Select(void);
extern	void	SD_Cmd13_Status(void);
extern	void	SD_Cmd16_Blocklen(void);
extern	void	SD_ACmd6_BusWidth(bit _4bit);
extern	void	SD_ReadSectors(void);
extern	void	SD_ReadSectors_USB(void);
extern	void	SD_WriteSectors(void);
extern	void	SD_WriteSectors_USB(void);
extern	void	SD_Read_LBA_Dos(U32 Read_LBA ,U8 Buf_Index);
extern	void	LCM_Init(void);
extern	void	LCM_Show_NoFile(void);
extern	void	LCM_ShowWait(void);
extern	void	LCM_ShowVOL(void);
extern	void	LCM_ShowFMIcon(void);
extern	void	LCM_Show_FM_CHNUM(U8 chnum);
extern	void	LCM_set_address(U8 page, U8 col);
extern	void	LCM_ShowCompanyLogo(void);
extern	void	LCM_ShowFrequency_L();	// 12x32
extern	void	LCM_ShowMusicType(void);
extern	void	LCM_ShowSDHostIcon(void);
extern	void	LCM_ShowEQ(void);
extern	void	LCM_ShowEQIcon(void);
extern	void	LCM_ShowPlayModeIcon(void);  //11x16 ICON
extern	void	LCM_ShowBitRate(void);
extern	void	LCM_ShowERROR(void);
extern	void	LCM_ShowSongNumber(void);
extern	void	LCM_ShowPlayTime(U16 time);
extern	void	LCM_ShowRECTime(U16 time);
extern	void	LCM_ShowRECRemainTime(U32 time);
extern	void	LCM_ShowSongTime(void);
extern	void	LCM_ShowClock(void);
extern	void	LCM_ShowChar5x8(U8 Num,U8 tc_column);
extern	void	LCM_ShowChar6x8(U8 Num,U8 tc_column);
extern	void	LCM_ShowChar16x32(U8 Num,U8 tc_column);
extern	void	LCM_ShowChar10x16(U8 Num,U8 tc_column);
extern	void	LCM_ShowChar12x32(U8 Num,U8 tc_column);
extern	void	LCM_ShowChar8x16(U8 Num,U8 tc_column);
extern	void	LCM_ShowMainMenu(void);
extern	void	LCM_ShowSetupMenu(U8 tc_item);
extern	void	LCM_ShowPlayMenu(U8 tc_item);
extern	void	LCM_ShowWAVPlayMenu(U8 tc_item);
extern	void	LCM_ShowRepeatMenu(U8 tc_item);
extern	void	LCM_ShowEQMenu(U8 tc_item);
extern	void	LCM_ShowFMMenu(U8 tc_item);
extern	void	LCM_ShowFMMenu_Quality(U8 tc_item);
extern	void	LCM_ShowRecordString(void);
extern	void	LCM_ShowStartTimeString(void);
extern	void	LCM_ShowDurationString(void);
extern	void	LCM_ShowRecONOFFString(void);
extern	void	LCM_ShowDuration(void);
extern	void	LCM_ShowFMAllIcon(void);
extern	void	LCM_ShowFMOrderRecordIcon(void);
extern	void	LCM_DisplayString(U8 *tpc_DataBuf, U16 tw_nByte,U8 tc_RowAddr);
extern	void	LCM_DisplayMenuString(U8 *tpc_DataBuf, U16 tw_nByte,U8 tc_ColumnAddr,U8 tc_RowAddr,bit tb_Fill);
extern	void	LCM_ShowTitle_SetClock(void);
extern	void	LCM_ShowYear(U8 tc_Year);
extern	void	LCM_ShowMonth(U8 tc_Month);
extern	void	LCM_ShowDay(U8 tc_Day);
extern	void	LCM_ShowHour(U8 tc_Hour);
extern	void	LCM_ShowMin(U8 tc_Min);
extern	void	LCM_ShowSec(U8 tc_Sec);
extern	void	LCM_ShowBacklightTime(void);
extern	void	LCM_ShowCountryMenu(void);
extern	void	LCM_ShowCalendarFrame(void);
extern	void	LCM_ShowCalendarYear(void);
extern	void	LCM_ShowCalendarMonth(void);
extern	void	LCM_ShowCalendarDay(void);
extern	void	LCM_ShowCalendarHour(void);
extern	void	LCM_ShowCalendarMinute(void);
extern	void	LCM_ShowCalendarSecond(void);
extern	void	LCM_ShowCalendarWeek(void);
extern	void	LCM_ShowLunar(void);
extern	void	LCM_ShowBattIcon(void);
extern	void	LCM_ShowSongNum(void);
extern	void	LCM_ShowSongTotalNum(void);
extern	void	LCM_ShowPinPu(void);
extern	void	LCM_ShowPlayPauseIcon(void);
extern	void	LCM_ShowRepeatIcon(void);
extern	void	LCM_ShowRecordStartTime_Hour(void);
extern	void	LCM_ShowRecordStartTime_Minute(void);
extern	void	LCM_ShowAlarmTime_Hour(void);
extern	void	LCM_ShowAlarmTime_Minute(void);
extern	void	LCM_ShowAlarmClockString(void);
extern	void	LCM_ShowTimeString(void);
extern	void	LCM_ShowRepeatString(void);
extern	void	LCM_ShowAlarmONOFFString(void);
extern	void	LCM_ShowAlarmRepeatONOFFString(void);
extern	void	LCM_ShowDaysString(U8 tc_day);
extern	void	LCM_ShowDaysSelect(void);
extern	void	LCM_ShowAlarmIcon(void);
extern	void	LCM_ShowClockIcon(void);

extern	void LCM_Show_LineIn(void) ;

extern	void	LCM_FillColumn(U8 tc_Page,U8 tc_CurrentColumn,U8 tc_TotalColumn);
extern	void	LCM_disp_HZKCharBMP(U8 Page, U8 Column,U8 *BMPdataBuf, U8 CharOrWord,U8 reverse);
extern	void	LCM_Clear_L0_L1(void);
extern	void	LCM_Clear_L0_L7(void);
extern	void	LCM_Clear_L2_L5(void);
extern	void	LCM_Clear_L2_L7(void);
extern	void	LCM_Clear_L6_L7(void);
extern	void	LCM_erase_one_page(U8 tc_page);
extern	void	RefreshAllDisplay(void);
extern	void	Host_register_INIT(void);
extern	void	SourceDevicePreSet(void);
extern	void	RandomGetFileIndex(void);
extern	void	DOS_Count_All_Dir(void);
extern	void	Music_EQ_Cmd(void);
extern	void	DSP_WakeUp(void);
extern	void	UI_fastFFFR_Play(U8 XFFFR);
extern	void	Idle_Task(void);
extern	void	Play_Task(void);
extern	void	FM_Task(void);
extern	void	USB_Task(void);
extern	void	Play_SDFlash_DosInit(void);
extern	void	set_clock_mode(U8 mode);
extern	void	PlayPrevFolder(void);
extern	void	PlayNextFolder(void);
extern	void	DetectSDorHOSTState(void);
extern	void	CheckIRKeyPushDownTimerOut(U8 x_Sg_Fr);
extern	void	FMIRNumberKey(U16 x_tmpfreq);
extern	void	CL6017FreqSaveChannel(U8 NextorPrev);
extern	void	Linein_VolAdj(void);
extern	void	Linein_Out(void);
extern	void	Linein_Set(void);
extern	void	change_clock_ratio(U8 r);
extern	void	DosSearchErrorNextStorageType(void);
extern	void	FillAudioDataToDSP(void);
extern	void	LRC_DisplayFunction(void);
extern	void	ReadBackupDataCompare(void);
extern	void	ClearIRNumberVariable(void);
extern	void	IR_Service_Process(void);
extern	void	JumpToTargetFolder(void);
extern	void	Play_SourceDetect_Process(void);
extern	void	WAV_SourceDetect_Process(void);
extern	void	WAV_Play(void);
extern	void	play_fffr_end(void);
extern  void	SD_Check_Idle(void);
extern	void	SD_Write_LBA_Dos(U32 Write_LBA,U8 Write_Stage);
extern	void	Host_Write_LBA_Dos(U32 Write_LBA,U8 Write_Stage);
extern	void	Write_LogData(void);
extern	void	USER_FillSettingInfo(void);
extern	void	USER_GetSettingInfo(void);
extern	void	tran_in(U8 addr,U8 amount,U16 offset);
extern	void	tran_in_(U8 addr,U8 amount,U16 offset);
extern	void	L2_DSP_Write_DMem16(U16 tw_Addr,U16 tw_Data);
extern	void	L2_DSP_Write_DMem24(U16 tw_Addr,U32 tdw_Data);
extern	void	L2_DSP_MCU_IM_DMA(U8 BufferIndex,U16 tw_Src);
extern	void	L2_DSP_MCU_PM_DMA(U8 BufferIndex,U16 tw_Src);
extern	void	L2_DSP_DM_MCU_DMA(U16 BufferIndex,U16 tw_Src);
extern	void	DSP_Write_MMR16(U16 tw_Addr,U16 tw_Data);
extern	void	MediaChange(void);
extern	void	LCM_DispHZK(void);
extern	void	OpenAudioFile(void);
extern	void	MenuModeSelect(void);
extern	void	DisplayMenuFunction(void);
extern	void	MenuDisplayExit(void);
extern	void	DisplayFirstMenu(void);
extern	void	DisplayGetDataToLCD(U8 x_page,U8 x_StringLine,U8 (*StringCode)[16],U8 x_Reverse);
extern	void	DisplayMenuScroll(U8 x_CurrentIndex,U8 x_MenuCnt,U8 (*t_StringArray)[16]);
extern	void	CheckAudioStatus(void);
extern	void	MP3FunctionProc(void);
extern	void	Change_EQ(void);
extern	void	IR_Init(void);
extern	void	L2_DSP_MCU_DM_DMA_24bit(U16 BufferIndex,U8 tc_Dest_Addr_High,U16 tw_Src);
extern	void	L2_DSP_MCU_DM_DMA_16bit(U16 BufferIndex,U8 tc_Dest_Addr_High,U16 tw_Src);
extern	void	Syn_Reset(void);
extern	void	USB_Mode(void);
extern	void	RTC_Init(void);
extern	void	RTC_Write(U8 address,U8 x);
extern	void	RTC_DisableInterrupt(void);
extern	void	RTC_EnableInterrupt(void);
extern	void	Suspend(void);
extern	void	ScanKey(void);
extern	void	Key_Detect(void);
extern	void	Get_KeyValue(U8 ADCValue);
extern	void	MainMenu(void);
extern	void	SetupMenu(void);
extern	void	SetClock(void);
extern	void	SetBacklight(void);
extern	void	SetLanguage(void);
extern	void	SetAlarmClock(void);
extern	void	SetOFFTimer(void);
extern	void	DisplayCalendar(void);
extern	void	RTC_Calendar(void);
extern	void	RTC_SnapValue(void);
extern	void	Year_Adj(void);
extern	void	Month_Adj(void);
extern	void	Day_Adj(void);
extern	void	Day_Adj1(void);
extern	void	Get_Lunar(U16 Y,U8 M,U8 D);
extern	void	VOL_Adj(void);
extern	void	AccessKeyEvent_WAV(void);
extern	void	MediaSelect(void);
extern	void	WAV_Play_Init(void);
extern	void	count_free_cluster_fat16(U8 *p,U8 size1,U8 size2);
extern	void	AccessKeyEvent_FM(void);
extern	void	AccessKeyEvent_FM_STOP(void);
extern	void	AccessKeyEvent_FM_REC(void);
extern	void	AccessKeyEvent_FM_PAUSE(void);
extern	void	FM_AutoSearch(void);
extern	void	FM_Open(void);
extern	void	FM_Mute(void);
extern	void	FM_Initial_CL6017(void);
extern	void	GetTotalFreeCluster(void);
extern	void	REC_Start(void);
extern	void	REC_Stop(void);
extern	void	LCM_ShowRECFilename(void);
extern	void	LCM_ShowUSBConnectIcon(void);
extern	void	PowerOn_Init_USB(void);
extern	void	MP3_seek_trig(void);
extern	void	WMA_seek_trig(void);
extern	void	WAVE_FF_FR(void);
extern	void	FillFDBHeader(void);
extern	void	AlarmMusic(void);
extern	void	AlarmMusic_DataIn(void);
extern	void	GetAlarmData(void);

extern	bit		CheckUSBPlugIN(void);
extern  bit 	MMC_Cmd01_OCR(void);
extern  bit     MMC_Cmd01_OCR_Wait_Ready(void);
extern	bit		StorgeNoMp3FileChangeNext(void);
extern  bit		SD_Identification_Flow(void);
extern	bit		HOST_Check(void);
extern	bit		SD_Check(void);
extern	bit		SearchFolder_VOICE(void);
extern	bit		MediaInitial(void);
extern	bit		DOS_Close_File_w(U8 *tpc_FileName);
extern	bit		Host_DetectDevice(void);
extern	bit		CreateFolder_VOICE(void);
extern	bit		CheckAlarmSetting(void);

extern	U8		READ_SARADC(U8 tc_ADCChannel);
extern	U8		WMA_ASF_Parser(void);
extern	U8		DOS_Search_DIR(U8 tc_Mode);
extern	U8		DOS_Search_DIR_Prev_test(U8 tc_Mode);
extern	U8		DOS_Search_DIR_Next_test(U8 tc_Mode);
extern	U8		DOS_GetLongFileName(U8 tc_FileHandle, U8 *tpc_LongName);
extern  U8 X_Unicode2ISN(U8 *UnicodeArray);	 //-sunzhk  add
extern	U8		GetCurrentDirInfo(void);
extern	U8		Find_Fdb(SearchFdb *p_mp);
extern	U8		Count_Dir_Fdb(SearchFdb *p_mp);
extern	U8		Host_Initial(void);
extern	U8		FM_SearchAllChannel(void);
extern	U8		Host_phy_Command_check(void);
extern	U8		Host_phy_Command_check_(U8 offset,U16 toggle);
extern	U8		HOST_Read_Logicsector(U32 LBAaddr);
extern	U8		L2_DSP_DM_Triger(U8 tc_DSP_ErrorStatus);
extern	U8		L2_DSP_SendCommandSet(U8 tc_Cmd);
extern	U8		LCM_Disp_FileName(U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 DispOnOff);
extern	U8		LCM_UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tbt_UnicodeOrISN);
extern	U8		RTC_Read(U8 address);
extern	U8		IMA_ADPCM_Play_PlayCmd(U8 tc_SampleRate);
extern	U8		IMA_ADPCM_Play_PauseCmd(void);
extern	U8		IMA_ADPCM_Play_ResumeCmd(void);
extern	U8		IMA_ADPCM_Play_StopCmd(void);
extern	U8		IMA_ADPCM_Play_DataIn(void);
extern	U8		IMA_ADPCM_Rec_StopProcess(void);
extern	U8		IMA_ADPCM_Rec_StopCmd(void);
extern	U8		IMA_ADPCM_Rec_DataOut(void);
extern	U8		IMA_ADPCM_Rec_PlayCmd(U8 tc_SampleRate,U8 tc_ChannelNum);
extern	U8		IMA_ADPCM_Play_FF_FR_Cmd(void);
extern	U8		IMA_ADPCM_seek_trig(void);
extern	U8		IMA_ADPCM_EOF_Proc(void);
extern	U8		IMA_ADPCM_Repeat_DataIn(void);
extern	U8		Music_PauseCmd(void);
extern	U8		DOS_SearchFreeCluster(U8 tc_SearchMode);
extern	U8		free_clusters_in_sector(U8 *p,U8 size,U8 cluster_entry_size,U32 tdw_FreeClusterNum);
extern	U8		count_free_cluster_in_fat16_sector(U8 *p, U8 size);
extern	U8		count_free_cluster_in_fat32_sector(U8 *p, U8 size);
extern	U8		fat16_free_cluster_offset(U8 *p,U8 size);
extern	U8		fat32_free_cluster_offset(U8 *p,U8 size);
extern	U8		DOS_Read_LogicSector(U32 tdw_LogicSectorAddr);
extern	U8		DOS_MarkFATUpdate(U32 tdw_MarkFATAddr,U8 tc_MarkValue);
extern	U8		DOS_Write_LogicSector(U32 tdw_LogicSectorAddr,U8 tc_AppendW);
extern	U8		DOS_Open_File_w(void);
extern	U8		DOS_Write_File(U8 tc_AppendWr);
extern	U8		SCSICMD_03_REQUESTSENSE(void);
extern	U8		SCSICMD_12_INQUIRY(void); 
extern	U8		SCSICMD_1A_MODESENCE(void);
extern	U8		SCSICMD_23_READFORMATCAPACITY(void);
extern	U8		SCSICMD_25_READCAPACITY(void);
extern	U8		MP3_FF_FR_Cmd(void);
extern	U8		WMA_FF_FR_Cmd(void);
extern	U8		MP3_Repeat_DataIn(void);
extern	U8		WMA_Repeat_DataIn(void);
extern	U8		Music_PlayCmd(void);
extern	U8		Music_StopCmd(U8 tc_FileHandle);

extern	U16		MP3_Total_Time_Parser(void);
extern	U16		MP3_Bitrate_Parser(void);
extern	U16		DSP_Read_MMR16(U16 tw_Addr);
extern	U16		L2_DSP_Read_DMem16(U16 tw_Addr);
extern	U16		IMA_ADPCM_SampleRate_Parser(void);
extern	U16		IMA_ADPCM_Play_ReadTime(void);
extern	U16		IMA_ADPCM_Rec_Time(void);

extern	U32		L2_DSP_Read_DMem24(U16 tw_Addr);




extern	void 	Read_Setup_Packet(void);
extern	void 	StandardCommand(void);
extern	void 	get_status(void);
extern	void 	clear_feature(void);
extern	void 	set_feature(void);
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
extern	void	SetCylTempToHBlock(void);
extern	void	SetCylTempToHBlock(void);
extern	void	SetHBlock_To_WhichGroup(void);

/*---------------------------------------------------------------------------
	Dos.C
----------------------------------------------------------------------------*/
extern 	void	Dos_Read_LBA(U32 Read_LBA ,U8 Buf_Index);
extern 	void 	Dos_Write_LBA(U32 Write_LBA,U8 Buf_Index,U8 Write_Stage);	
extern 	void 	NAND_FlashReadSector_DOS(void);
extern	void 	NAND_FlashWriteSector_DOS(void);
extern 	void 	Move_RAM(U16 RAM_Source,U16 RAM_Destination,U16 RAM_Size);
//---------------------------------------------------------------------------
// dosinit.c
//---------------------------------------------------------------------------
extern  U8 DOS_Initialize(void);
extern  U8 JudgeBootSector(void);

//---------------------------------------------------------------------------
// dosfile.c
//---------------------------------------------------------------------------
extern  U8	DOS_Search_File(U8 tc_Mode, U8 tc_Type, U8 tc_PrevOrNext);
extern  U8 DOS_Read_File(U8 tc_FileHandle);
extern  U8 DOS_Open_File_r(U8 tc_FileHandle, U8 tc_OpenMode, U8 * tpc_FileName);
extern  U32 DOS_Seek_File(U8 tc_FileHandle, U32 tdw_SectorNumber);

extern void DosFreeClusterCalculate(U8 tc_FileHandle, U8 tc_ClusterInc);

//---------------------------------------------------------------------------
// dosfdb.c
//---------------------------------------------------------------------------
extern  U8 USER_FindDir(U8 tc_UserDirNum, U32 tdw_DirStartCluster);
extern  void USER_Create_Dir(U8 tc_UserDirNum, U32 tdw_DirStartCluster);
extern  U8 DOS_AddNewFdb(U8 *tpc_FdbEntry, U32 tdw_DirStartCluster);
extern  void DOS_FillFdb(U8 *tpc_Fdb, U8 * tpc_Name, U32 tdw_StartCluster, U32 tdw_FileSize);
extern  U32 DOS_MakeDir(U8 * tpc_DirName, U32 tdw_DirStartCluster);
extern  U8 DOS_WriteDirInitData1(U32 tdw_CurrentStart, U32 tdw_ParentStart);

extern U32  DOS_ClusterLogicAddr(U32 tdw_Cluster);
extern U8  DOS_ConnectFATChain(U32 tdw_StartCluster, U32 tdw_TailCluster);
extern U32  GetValidClusterCnt(void);
extern U32  DOS_GetAFreeCluster(void);
extern U32  DOS_GetNextCluster(U32 tdw_CurrentCluster, U32 tdw_ClusterNumber);


//-------------------------------
extern void play_endfile(void);
extern void play_playpause(void);
extern void play_fastfwd(void);
extern void play_fastrev(void);
extern void play_idle(void);
extern void play_set_repAB(void);
extern void play_back2uplevel(void);
extern void play_menu(void);
extern void play_vol(void);

extern void PlayMode_CHG(void);  //chiayen0807


extern  bit 	SD_Card_Detect(void);


extern	U8		Lyric_OffsetLRC(void);
extern void Lyric_GetLyrics(void);
extern void Lyric_GetTimeStamp(void);
extern U8 Lyric_FileSearch(void);
extern U8 Lyric_ReadLrcFile(U8 tc_ReadNextSector);
extern void TuneVol_init(void);
extern  void    SPI_SET_ADDR(sSPI_ADDR *sBPS);
extern  void    SPI_Send_Byte(U8 out);
extern  U8      SPI_Get_Byte(void);
extern  U8      SPI_Read_Status(void);
extern  void    SPI_Wait_Busy(void);
extern  void    SPI_WRSR(U8 Out);
extern  void    SPI_WREN(void);
extern  void    SPI_WRDI(void);
//extern  void    SPI_Write_Data(sSPI_ADDR *sBPS,U8 Seccnt);
extern  U8      Check_SPI_BankCode(void);
extern  void    SPI_Structure_Addr(void);
extern  void    Load_Code_From_SPI_To_Ram(void);
extern	U8		SD_Receive_Respond(U8 *tc_RespBuff,U8 tc_Resptype136bit, U8 tc_CRC);
extern	U8		MP3_Download(void);
extern	U8		WMA_Download(void);
extern	U8		WAV_Play_Download(void);
extern	U8		IMA_ADPCM_Rec_Download(void);
extern	U8		MP3_Repeat_DataIn(void);
extern	U8		WMA_Repeat_DataIn(void);
extern	U8		MP3_DataIn(void);
extern	U8		WMA_DataIn(void);
extern	U8		MP3_EOF_Proc(void);
extern	U8		WMA_EOF_Proc(void);
extern	U16		MP3_ReadTime(void);
extern	U16		WMA_ReadTime(void);

extern	void	USBInitialize(void);

