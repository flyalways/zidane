//=====================================================================================================
//=====================================================================================================
//以下参数请User根据FM模组的实际情况设置
extern code U16 FM_Useless_ChannelTable[];
extern U8 code Menu_Combination_FM[][2];
extern U8 code StateChangeTable_FM[][5];
extern xdata U8 gc_FM_ADC_Level[];
//======================== Define For FM=========================================
#define EURHighestFM             108000  //108.Mhz//FM
#define EURLowestFM              87500   //76000 	//lijian.peng just for RDA5800 070912 
#define JAPHighestFM             90000  //108.Mhz//FM
#define JAPLowestFM              76000   //76000  
#define     FM_USELESS_CHANNEL   10


//============ FM AutoSearch UP Or Down ====================================
#define FM_SearchDown         0
#define FM_SearchUp           1

//============ FM Mode===============================================
#define  FM_Tea5767
//=====add for FM====================================================
#define FM_Search_StepLength  100     //调台步进量
#define 	K_MAX_FM_CHL		  20      //FM存储频道数
#define REFERENCE_FREQ 		  1024    //32768/32=1024//32.768
#define FM_MANUAL_MODE  0
#define FM_AUTO_MODE	1


#define     STATE_FM_MANUAL              	0x11
#define     STATE_FM_AutoSearchUp          	0x12
#define     STATE_FM_AutoSearchDown        	0x13


//==I2C====================================
extern void I2CStart(void);
extern void I2CStop(void);
extern U8 WaitAck(void);
extern U8 I2CSendByte(unsigned char ch);
extern unsigned char I2CReceiveByte(U8 SendAck);
//========FM Function====================================
extern void System_SetFMVolume(U8 tc_Volume);
//================================================================================
extern U8   Tuner_Preset(U32 tdw_SearchedVCO, U8 tc_FM_SearchMode);
extern U32  Tuner_Search(U8 tc_FM_SearchUpDown,U8 tc_FM_SearchMode) ;
extern unsigned int X_Get_FM_Totalchannel();
extern U8 X_FM_GetFreeChannel();
extern U8 X_FM_channelExist(void);
extern U8 X_FM_Write_CurrChannelFreq();
extern void X_FM_Clear_AllPresetStation();
extern void X_FM_NextChannel(void);
extern void X_FM_PrevChannel(void);
extern U8 X_FM_DelChannel(void);
extern void UI_GET_FM_Frequence(void);

extern U8 FMInit();
extern void FM_Standby();
extern void FM_WriteSTR();
extern void FM_ReadSTR();
extern U8 FM_StepDown();
extern U8 FM_StepUp();
extern U8 FM_AutoSearchUp();
extern U8 FM_AutoSearchDown();
extern U8 FM_EnterAutoMode();
//=================================================================================

extern U8 Enter_FM_AutoSearch_Mode();
extern U8 FM_CheckFrequanceInQunue(U8 tc_FM_Mode,U32 tdw_SearchedVCO);
extern U8 FM_AutoSearchUpOrDown(U8 UpOrDown);
extern void FM_SearchProcess();
extern U8 IMA_ADPCM_Rec_Download(void);

