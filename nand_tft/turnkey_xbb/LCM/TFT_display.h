extern	void	TFT_HEX_BCD(U16 num,U8 *Hi,U8 *MidH,U8 *MidL,U8 *Low);
extern	void	TFT_HEX_BCD_5(U16 num,U8 *AA,U8 *BB,U8 *CC,U8 *DD,U8 *EE);
extern	void 	TFT_ShowPingpu(void);
extern	void    TFT_Show_VOL(void);
extern	void	TFT_PowerOnlogo(void);
extern	void	TFT_PowerOfflogo(void);
extern	void	TFT_ShowUSBIcon(void);
extern	void	TFT_ShowEQIcon(void);
extern	void	TFT_ShowPlayMode(void);
extern	void	TFT_ShowMediaIcon(void);
extern	void	TFT_ShowMediaIcon_M(void);
extern	void	TFT_ShowPlayPauseIcon(void);
extern	void	TFT_ShowPlayRatioBar(void);
extern	void	TFT_ShowMusicType(void);
extern	void	TFT_ShowFMFrequency(void);
extern	void	TFT_ShowSongIndex(void);
extern	void	TFT_ShowTotalSong(void);
extern	void	TFT_ShowBitRate(void);
extern	void	TFT_ShowPlayTime(void);
extern	void	TFT_ShowFMFrequency(void);
extern	void	TFT_ShowMenu(void);
extern	void	TFT_ShowSongLength(void);
extern	void	TFT_Main(void);
extern	void	TFT_Main_VOL_FM_ADJ(void);
extern	void	TFT_ShowFMFreqAdj(void);
extern	void	TFT_ShowVOLAdj(void);
extern	void	TFT_PlayingOperation(void);
extern	void	TFT_SetupMenu(void);
extern	void	TFT_EQMenu(void);
extern	void	TFT_RepeatMenu(void);
extern	void	TFT_PAUSE(void);
extern	void	TFT_PhotoMenu(void);
extern	void	TFT_Velocity(void);
extern	void	TFT_FileSelect(void);
extern	void	TFT_FileIcon(void);
extern	void	TFT_LanguageMenu(void);
extern	void	TFT_FWVERSION(void);
extern	void	TFT_MemoryInfo(void);
extern	void	TFT_LRCBackGround(void);
extern	void	TFT_MovieMenu(void);
extern	void	TFT_EQFrame(void);
extern	void	TFT_MoviePlayMode(void);
extern	void	TFT_IRNumMain(void);
extern	void	TFT_IRNum(void);
extern	void	TFT_ShowError(void);
//---------------sunhk add new display===//0703
extern void	Menu_Disp_Item_Play(U8 tc_MenuType,U8 tc_Select,U8 tc_RefreshType);
extern void Menu_Disp_Item_Other(U8 tc_MenuType,U8 tc_Select,U8 tc_RefreshType);

