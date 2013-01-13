#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\header\dos\fs_struct.h"
#include "..\IR\remote.h"
#include "..\IR\LCM_BMP.h"
#include "..\IR\FM.h"
#include "..\header\Memalloc.h"
#include "..\header\variables.h"

data bit gb_PickSongSet=0;
xdata U8 gc_NextorPreFlag=0;// 0:FF 1:FR
data U8 gc_Dirchange_Timer=0; //20090121 chiayen add
data U16 gw_IR_Timer=0;
xdata U16 gw_irkey_count=0;
data U16 gw_SongNumber=1;  //temp
data U16 gw_SongNumberSD;

xdata U8 gc_VolumeMute=0;
xdata U8 gc_irkey=0;
xdata U8 gc_IRdetMask=0;
xdata U8 gc_Volumebackup=0;
extern data System_Struct gs_System_State;
extern bit gb_FlashNoFileflag;
extern bit	gb_DirPlay_Flag;  //20090121 chiayen test
extern data	bit	gb_PickSong;
extern data	bit	gb_ChannelSet;
extern xdata	U8	gc_Err;
extern data	bit	gb_Frequency_Song;	// 0=Show Frequency    1=Show Song Number/EQ/Vol
extern xdata	U8	gc_SelectVol;
extern xdata	U8	gc_SelectEQ;
extern data	U8	gc_IR_Setting;

extern data	U8	gc_LCM_line;
extern xdata	U16	gw_DisplayFreq1;
extern xdata	U16	gw_DisplayTime;
extern data	U8	gc_LCM_Media; //20090121 chiayen test
extern data	U8	gc_DisplayEQIcon;
extern xdata	U16	gw_DisplayBitRate;
extern data	U8	gc_DisplayPlayMode;
extern data	U8	gc_LCM_Media;	// 0=Show LineIn BMP  1=Show playing info
extern data	U8	gc_CurrentCard;
extern data	U8	gc_DisplaySongnameCount;
extern data	U16	gw_IR_Setting_Timer;
extern data	U8	gc_ShowTimer;
extern xdata	U16	gw_DisplayFreq;
extern xdata	U16	gw_DisplayTime;
extern data U16 gw_FM_frequency;  // range=87.7MHz~107.9MHz
extern data	U16	gw_ChangeFrequencyTimer;
extern xdata	U16	gw_DispSongNum;
extern xdata	U16	gw_DispSongNum1;
extern data U16 gw_LogData_Timer; //chiayen0812
extern xdata U16  gw_FileTotalNumber[];
extern xdata U16  gw_FileIndex[]; //3
extern xdata U16  gw_FileTotalNumber[];//3
extern xdata WORD  gw_FileSkipNumber;	//Ching 080805 - For quick search , if not 0
extern xdata UBYTE gc_PhaseInx;
extern xdata UBYTE gw_SearchAmount;
extern xdata UBYTE gc_RepPlayMode;
extern DSP_GLOBAL gs_DSP_GLOBAL_RAM;

extern void Music_EQ_Cmd(U8 tc_Type);
extern void Music_SetVolumeCmd(void);
extern void LCM_ShowSongNumber_1(void);
extern void LCM_ShowSongNumber_dir(void);
extern U8 DOS_Search_DIR(U8 tc_Mode);  //20090121 chiayen add
extern U8 LCD_disp_HZK_string(U8 Page,U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 ColumnOffset,bit reverse); //20090121 chiayen add

void ir_init(void);
void ir_service(void);
void ir_commandservice(void);
void SaveNumber(U8 number);
//void SaveNumber_PickSong(U8 number);
void ClearIRBuffer(void);
void Frequency_Disp(void);
void Channel_Set_Init(void);
void Pick_Song_Init(void);
void Change_EQ(void);
void VOL_UP(void);
void VOL_DOWN(void);
void PickSong_Process(void);
void NextorPre_Song(U8 tb_state);
void MediaChange(void);  //20090107 chiayen add
void UI_DispGet_DirName(void); //20090121 chiayen add
void LCD_Disp_DirName(void); //20090121 chiayen add

extern U8	DOS_Search_DIR_Prev_test(U8 tc_Mode);  //20090216 chiayen add
extern U8	DOS_Search_DIR_Next_test(U8 tc_Mode);
extern xdata WORD 	gw_DirOverallNumber;
extern xdata WORD 	gw_DirTotalNumber;
extern U8 GetCurrentDirInfo();
xdata U8 gc_FolderPlay_Flag=0;  //20090216 chiayen add
xdata U8 gc_PlayMenu_IR=0;  //20090206 chaiyen add for IR_DIR

extern data bit gb_SD_Exist;
extern data bit gb_Host_Exist;

extern xdata U8 gc_test;

extern 	xdata U8 gc_KeyDet_Mask;  //20090810 chiayen add
extern U8	gc_clock_mode;
xdata U8 gc_RPTMenuSet=0;
xdata U8 gc_IRNum_Count=0;
void PickFreq_Process(void);
extern xdata U16 gw_AllDirTotalNumber;

code U8 FloderIcon16x16_Dir[]=
{
	0x00,0x00,0xF8,0x88,0x88,0x88,0x88,0x00,0x60,0x00,0x38,0xE0,0x80,0x00,0x00,0x00,
	0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x03,0x0E,0x00,0x00,
};


void LCM_ShowFloderIcon(U8 Column)
{
#if 0
	U8	i,j,k=0;
	gc_LCM_line=0;
	#if (LCM_SIZE==1)  //20090225
	LCM_clear();
	#else
	LCM_clear_dir();  //20090216 chiayen modify
	#endif
	for(i=2;i<4;i++)
	{
		LCM_set_address(i+gc_LCM_line,Column);
		for(j=0;j<16;j++)
		{
			LCM_write_data(FloderIcon16x16_Dir[k]);	
			k++;
		}
	}
	gc_LCM_Media=0xff;
	gc_DisplayEQIcon=0xFF;
	gw_DisplayBitRate=0xFFFF;
//	gc_DisplayPlayMode=0xFF;  //20090216 chiayen mark
	gw_DispSongNum1=0xFF;
	#if (LCM_SIZE==1)
	gc_LCM_line=0;
	gw_DisplayTime=0xFFFF;
	gw_DisplayFreq1=0xFFFF;
	#endif
	gw_DispFileName_ByteOffset=0;
#endif
}

void ir_init(void)
{
#if 0

	// set time out
	IR_REG[0x00] = LO_BYTE(TIME_OUT_MIN);
	IR_REG[0x01] = HI_BYTE(TIME_OUT_MIN);
	// start 1 max
	IR_REG[0x02] = LO_BYTE(START1_MAX);
	IR_REG[0x03] = HI_BYTE(START1_MAX);
	// start 1 min
	IR_REG[0x04] = LO_BYTE(START1_MIN);
	IR_REG[0x05] = HI_BYTE(START1_MIN);
	// start 0 max
	IR_REG[0x06] = LO_BYTE(START0_MAX);
	IR_REG[0x07] = HI_BYTE(START0_MAX);
	// start 0 min
	IR_REG[0x08] = LO_BYTE(START0_MIN);
	IR_REG[0x09] = HI_BYTE(START0_MIN);
#if (IR_MODULATION==IR_MOD_PPM) || (IR_MODULATION==IR_MOD_PWM)
	// logic 1 max
	IR_REG[0x0a] = LO_BYTE(LOGIC1_MAX);
	IR_REG[0x0b] = HI_BYTE(LOGIC1_MAX);
	// logic 1 min
	IR_REG[0x0c] = LO_BYTE(LOGIC1_MIN);
	IR_REG[0x0d] = HI_BYTE(LOGIC1_MIN);
#else
	// bit time
	IR_REG[0x0a] = LO_BYTE(BIT_TIME);
	IR_REG[0x0b] = HI_BYTE(BIT_TIME);
	// sample point
	IR_REG[0x0c] = LO_BYTE(SAMPLE_POINT);
	IR_REG[0x0d] = HI_BYTE(SAMPLE_POINT);
#endif
	// logic 0 max
	IR_REG[0x0e] = LO_BYTE(LOGIC0_MAX);
	IR_REG[0x0f] = HI_BYTE(LOGIC0_MAX);
	// logic 0 min
	IR_REG[0x10] = LO_BYTE(LOGIC0_MIN);
	IR_REG[0x11] = HI_BYTE(LOGIC0_MIN);
	// set sampling period = 1 usec
	IR_REG[0x12] = LO_BYTE(T_SAMPLE);
	IR_REG[0x13] = HI_BYTE(T_SAMPLE);		
	// set polarity
#if IR_MODULATION == IR_MOD_PPM
	IR_REG[0x17] = 0x01;
#elif IR_MODULATION == IR_MOD_PWM
	IR_REG[0x17] = 0x03;
#elif IR_MODULATION == IR_MOD_BIPHASE
	IR_REG[0x17] = 0x07;
#elif IR_MODULATION == IR_MOD_BINARY
	IR_REG[0x17] = 0x05;
#else
//	dbprintf("Unsupported format.\n");
	while(1);
#endif

	IR_REG[0x19] = 0x07; // clear all events
	IR_REG[0x18] = 0x07; // enable all IR interrupts
	IR_REG[0x17] |= 0x80; // IR reveiver HW enabled
	IR_REG[0x19] = 0x07; // clear all events
//	dbprintf("IR HW enabled\n");
	EX0 = 1; EA = 1;

#endif
}


void ir_service(void)
{
#if 0

	U16 tw_temp=0;	
	if(gc_IRCmdStatus!=0)
	{
		tw_temp=(IR_REG[0x1b]<<8)+IR_REG[0x1a];
		if(tw_temp==IR_6_Key)
		{
			gc_irkey=1;
		}
		if(tw_temp==IR_7_Key)
		{
			gc_irkey=2;	
		}
		if(tw_temp==IR_18_Key)
		{
			gc_irkey=3;	
		}
		if(tw_temp==IR_20_Key)
		{
			gc_irkey=4;	
		}
		if(tw_temp==IR_23_Key)
		{
			gc_irkey=5;	
		}
		if((tw_temp==IR_11_Key) || (tw_temp==IR_11_1_Key))
		{
			gc_irkey=6;	
		}

		if(gc_irkey!=0)
		{
			if((IR_REG[0x1c]+IR_REG[0x1d])==0xFF)
			{
				if(gc_IRCmdStatus==1)
				{
					ir_commandservice();
				}
				else if(gc_IRCmdStatus==2)
				{					
					ir_commandservice();
				}
			}					
		}
		gc_IRCmdStatus=0;
	}
	gc_irkey=0;
}


void ir_commandservice(void)
{

	if(LCM_BACKLIGHT==1)
	{
		if((gc_irkey!=6))
		{
			return;
		}
		else
		{
			if(IR_REG[0x1c]!=0x08)
			{
				return;
			}		
		}	
	}
    
	if(gc_irkey==1)	// 6-key
	{
		if(gs_System_State.c_Phase == TASK_PHASE_PAUSE)
		{
	        if(IR_REG[0x1c]==0x01)	// paly/pause
			{
				if(gc_IRCmdStatus==1)
				{
					gc_PhaseInx=3;
					gc_ShowTimer=1;
					ClearIRBuffer();
				}
			}
			return;
		}

	    switch(IR_REG[0x1c])
	    {
	        case 0x00:	// CH
				gw_ChangeFrequencyTimer=120;
				gb_Frequency_Song=0;
				Frequency_Disp();
				break;
	        case 0x01:	// paly/pause
				if(gc_IRCmdStatus==1)
				{
					gc_PhaseInx=3;
					gc_ShowTimer=1;
					ClearIRBuffer();
				}
				break;
	        case 0x02:	// prev
				if(gw_ChangeFrequencyTimer!=0)
				{
					gw_ChangeFrequencyTimer=120;
					FM_FREQ_CHG(0,1);
				}
				else if(gc_IRCmdStatus==1)
				{
					NextorPre_Song(1);
				}
				break;
	        case 0x03:	// next
				if(gw_ChangeFrequencyTimer!=0)
				{
					gw_ChangeFrequencyTimer=120;
					FM_FREQ_CHG(1,1);
				}
				else if(gc_IRCmdStatus==1)
				{
					NextorPre_Song(0);
				}
				break;
	        case 0x04:	// volume --
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					gw_ChangeFrequencyTimer=0;
					VOL_DOWN();
				}
				break;
	        case 0x05:	// volume ++
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					gw_ChangeFrequencyTimer=0;
					VOL_UP();
				}
				break;
	    }
	}
	else if(gc_irkey==2)	// 7-key
	{
		if(gs_System_State.c_Phase == TASK_PHASE_PAUSE)
		{
	        if(IR_REG[0x1c]==0x19)	// paly/pause
			{
				if(gc_IRCmdStatus==1)
				{
					gc_PhaseInx=3;
					gc_ShowTimer=1;
					ClearIRBuffer();
				}
			}
			return;
		}

	    switch(IR_REG[0x1c])
	    {
	        case 0x1C:	// Frequency UP
				FM_FREQ_CHG(1,1);
				gw_ChangeFrequencyTimer=120;
				break;
			case 0x09:	// Frequency DOWN
				FM_FREQ_CHG(0,1);
				gw_ChangeFrequencyTimer=120;
				break;
	        case 0x19:	// paly/pause				
				if(gc_IRCmdStatus==1)
				{
					gc_PhaseInx=3;
					gc_ShowTimer=1;
					ClearIRBuffer();
				}
				break;
	        case 0x02:	// prev
				if(gc_IRCmdStatus==1)
				{
					gw_ChangeFrequencyTimer=0;
					NextorPre_Song(1);
				}
				break;
	        case 0x13:	// next
				if(gc_IRCmdStatus==1)
				{
					gw_ChangeFrequencyTimer=0;
					NextorPre_Song(0);
				}
				break;
	        case 0x06:	// volume --
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					VOL_DOWN();
				}
				break;
	        case 0x04:	// volume ++
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					VOL_UP();
				}
				break;
	    }
	}

	else if(gc_irkey==3)	// 21-key			//客户码： 02FD
	{
		if(gs_System_State.c_Phase == TASK_PHASE_PAUSE)
		{
	        if(IR_REG[0x1c]==0x05)	// paly/pause
			{
				if(gc_IRCmdStatus==1)
				{
					gc_PhaseInx=3;
					gc_ShowTimer=1;
					gc_IR_Setting=0;
					ClearIRBuffer();
				}
			}
			else if(IR_REG[0x1c]==0x18)
			{
				if(gc_IRCmdStatus==1)
				{
				
					gc_PhaseInx=C_PlayModeChg;
				}
			}
			else if(IR_REG[0x1c]==0x1a)
			{
				if(gc_IRCmdStatus==1)
				{
				
					gb_PickSong=0;  //20090513 chiayen add
					gb_ChannelSet=0;  //20090513 chiayen add
					gc_ShowTimer=0;  //20090513 chiayen add
					gc_Task_Next=C_Task_Setting;	
				}
			}
			return;
		}

	    switch(IR_REG[0x1c])
	    {
			U8 tc_SearchDirTimeout;
#if 0
             case 0x01:	// Menu
				if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
					gb_PickSong=0;  //20090513 chiayen add
					gb_ChannelSet=0;  //20090513 chiayen add
					gc_ShowTimer=0;  //20090513 chiayen add
					//gc_Task_Next=C_Task_PlayMenu_IR;
				//	gc_Task_Next=C_Task_PlayMenu;
					gc_PhaseInx =C_PlayModeChg;
				}
				gc_PlayMenu_IR=1;  //20090206 chaiyen add for IR_DIR
				break;
			 case 0x04:	// mode//loop mode
			   if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
                    PlayMode_CHG();
				}
				break;
#endif 
	        case 0x05:	// paly/pause
				if(gc_IRCmdStatus==1)
				{
					gc_PhaseInx=3;
					gc_ShowTimer=1;
					gc_IR_Setting=0;
					ClearIRBuffer();
				}
				break;
	        case 0x1a:	// EQ			
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					if((gc_IRCmdStatus==1)&&(gw_FileTotalNumber[0]!=0))
					{
						Change_EQ();
					}
				}
				break;

	        case 0x09:	// volume --
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					VOL_DOWN();
				}
				break;
	        case 0x01:	// volume ++
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					VOL_UP();
				}
				break;
	        case 0x04:	// prev
			 if(gc_IRCmdStatus==1)
				{
					NextorPre_Song(1);
				}
	 			break;
	        case 0x06:	// next
               if(gc_IRCmdStatus==1)
				{
					NextorPre_Song(0);
				}
				break;
			case 0x00:	// CH-

				

					FM_FREQ_CHG(0,1);
				
				break;
			case 0x02:	// CH+
	
				FM_FREQ_CHG(1,1);
				
				break;
			case 0x08:    //Pick Song
				Pick_Song_Init();
				break;
		        case 0x0A:	//CH set
				Channel_Set_Init();
				break;				
			case 0x18:	// S/F
				if(gc_IRCmdStatus==1)  //for IRSourceChange
				{
					MediaChange();  //20090107 chiayen add
				}
				break;
			case 0x1d:	// mute
				if(gc_IRCmdStatus==1) 
				{
					if(gc_VolumeMute==0)
					{
						gc_Volumebackup=gs_DSP_GLOBAL_RAM.sw_Volume;
						gs_DSP_GLOBAL_RAM.sw_Volume=0;
						Music_SetVolumeCmd();
						gc_VolumeMute=1;
						gs_DSP_GLOBAL_RAM.sw_Volume=gc_Volumebackup;
					}
					else
					{
						Music_SetVolumeCmd();
						gc_VolumeMute=0;
					}
				}
				break;
 #if 0
			case 0x00:	// power on/off
				if(gc_IRCmdStatus==1)  
				{
				/*	if(LCM_BACKLIGHT==0)
					{
						LCM_BACKLIGHT=1;				
						play_stop();
						LCM_ShowPlayTime(0);
					}
					else if(LCM_BACKLIGHT==1)
					{
						LCM_BACKLIGHT=0;
						gc_PhaseInx=3;
					}*/
					gc_PhaseInx=C_PowerOff;
				}
			break;
 #endif
			case 0x19:	// 0
				SaveNumber(0);
				break;
			case 0x0c:	// 1
				SaveNumber(1);
				break;
			case 0x0d:	// 2
				SaveNumber(2);
				break;
			case 0x0e:	// 3
				SaveNumber(3);
				break;
			case 0x10:	// 4
				SaveNumber(4);
				break;
			case 0x11:	// 5
				SaveNumber(5);
				break;
			case 0x12:	// 6
				SaveNumber(6);
				break;
			case 0x14:	// 7
				SaveNumber(7);
				break;
			case 0x15:	// 8
				SaveNumber(8);
				break;
			case 0x16:	// 9
				SaveNumber(9);
				break;
	    }
	}

	else if(gc_irkey==4)	// 21-key					//客户码：00FF
	{
		if(gs_System_State.c_Phase == TASK_PHASE_PAUSE)
		{
	        if(IR_REG[0x1c]==0x43)	// paly/pause
			{
				if(gc_IRCmdStatus==1)
				{
					gc_PhaseInx=3;
					gc_ShowTimer=1;
					gc_IR_Setting=0;
					ClearIRBuffer();
				}
			}
			else if(IR_REG[0x1c]==0x46)
			{
				if(gc_IRCmdStatus==1)
				{
				 
					gc_PhaseInx=C_PlayModeChg;
				}
			}
			return;
		}

	    switch(IR_REG[0x1c])
	    {
			U8 tc_SearchDirTimeout;
			case 0x19:	// +100
				if(gc_IRCmdStatus==1)  //20090216 chiayen add
				{					
					if((gw_FileTotalNumber[0]!=0) && (gc_KeyDet_Mask==0)) //20090810 chiayen modify
					{										
				#if 1
						U32 dw_File_StartCluster_Backup=0;
						U32 dw_FDB_StartCluster_Backup=0;
						U8  tc_clock_mode_backup;  //20090817 chiayen add
 
						gb_DirPlay_Flag=1;
				
						play_stop();
						gw_DirOverallNumber = gw_AllDirTotalNumber;

						tc_clock_mode_backup=gc_clock_mode;//20090817 chiayen add
						set_clock_mode(CLOCK_MODE_DOS);  //20090817 chiayen add
						GetCurrentDirInfo();

						for(tc_SearchDirTimeout=0;tc_SearchDirTimeout<=255;tc_SearchDirTimeout++)
						{
							DOS_Search_DIR_Prev_test(0x23);
	
							gb_FindFlag = 0;

							gs_File_FCB[0].dw_FDB_StartCluster = gs_DIR_FCB[0].dw_File_StartCluster;

							dw_File_StartCluster_Backup=gs_DIR_FCB[0].dw_File_StartCluster;
							dw_FDB_StartCluster_Backup=gs_DIR_FCB[0].dw_FDB_StartCluster;

							gc_test=0;
							DOS_Search_File(0x10,0,0x10);

							if(gw_FileTotalNumber[0])
							{
								DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
								gw_FileIndex[0]=1;
								break;
							}
							gs_DIR_FCB[0].dw_File_StartCluster=dw_File_StartCluster_Backup;
							gs_DIR_FCB[0].dw_FDB_StartCluster=dw_FDB_StartCluster_Backup;
						}
						set_clock_mode(tc_clock_mode_backup);  //20090817 chiayen add

						if(gs_File_FCB[0].dw_FDB_StartCluster == gdw_DOS_RootDirClus)
						{
//						    LCM_clear_dir();
							LCM_ShowFloderIcon(0);
						}
						else
						{
							UI_DispGet_DirName();
							LCD_Disp_DirName();
						}
						#if (LCM_SIZE==1)  //20090225
						LCM_ShowSongNumber_1();  //20090205 chiayen test
						#else
						LCM_ShowSongNumber_dir();
						
						USER_DelayDTms(250);
						#endif
						gc_Dirchange_Timer=150;
						gb_FindFlag = 1;
						gc_PhaseInx = 3;
						gb_DirPlay_Flag=1;
						gc_FolderPlay_Flag=1;  //20090216 chiayen add
                        gw_DispSongNum=0xFFFF;
						gc_KeyDet_Mask=1;  //20090810 chiayen add
				#else
						if(gw_FileTotalNumber[0]<100)
						{
							// Show Err
							gc_Err=1;
							gc_ShowTimer=60;
						}
						else
						{
							if(gw_FileTotalNumber[0]!=0)
							{
								Pick_Song_Init();
								gc_NUMBER[2]=1;
							}
						}
				#endif
					}
				}
				break;
			case 0x0d:	// +200
				if(gc_IRCmdStatus==1)  //20090216 chiayen add
				{
					if((gw_FileTotalNumber[0]!=0) && (gc_KeyDet_Mask==0))  //20090810 chiayen modify
					{
				#if 1
						U8  tc_clock_mode_backup;  //20090817 chiayen add
						play_stop();

						tc_clock_mode_backup=gc_clock_mode;//20090817 chiayen add
						set_clock_mode(CLOCK_MODE_DOS);  //20090817 chiayen add

						for(tc_SearchDirTimeout=0;tc_SearchDirTimeout<=255;tc_SearchDirTimeout++)
						{
							gs_DIR_FCB[0].dw_FDB_StartCluster = gs_File_FCB[0].dw_FDB_StartCluster;							
							DOS_Search_DIR(0x23);

							gb_FindFlag = 0;													
							if(gc_test==0)
							{
								gs_File_FCB[0].dw_FDB_StartCluster = gs_DIR_FCB[0].dw_File_StartCluster;
							}
							else
							{
								gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
								gc_test=0;
							}
							DOS_Search_File(0x10,0,0x10);
							if(gw_FileTotalNumber[0])
							{
								DOS_Search_File(C_File_All|C_By_Time, C_MusicFileType, C_CmpExtName|C_Next);
								gw_FileIndex[0]=1;
								break;
							}														
						}
						set_clock_mode(tc_clock_mode_backup);  //20090817 chiayen add

						if(gs_File_FCB[0].dw_FDB_StartCluster == gdw_DOS_RootDirClus)
						{
//						    LCM_clear_dir();
							LCM_ShowFloderIcon(0);
						}
						else
						{
							UI_DispGet_DirName();
							LCD_Disp_DirName();
						}
						#if (LCM_SIZE==1)  //20090225
						LCM_ShowSongNumber_1();  //20090205 chiayen test
						#else
						LCM_ShowSongNumber_dir();
						#endif
						gc_Dirchange_Timer=150;
						gb_FindFlag = 1;
						gc_PhaseInx = 3;
						gb_DirPlay_Flag=1;
						gc_FolderPlay_Flag=1;  //20090216 chiayen add
                        gw_DispSongNum=0xFFFF;
						gc_KeyDet_Mask=1;  //20090810 chiayen add				
				#else
						if(gw_FileTotalNumber[0]<200)
						{
							// Show Err
							gc_Err=1;
							gc_ShowTimer=60;
							ClearIRBuffer();
						}
						else	
						{
							if(gw_FileTotalNumber[0]!=0)
							{
								Pick_Song_Init();
								gc_NUMBER[2]=2;
							}
						}
				 #endif
					}				
				}
				break;
             case 0x46:	// Menu
				if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
					gb_PickSong=0;  //20090513 chiayen add
					gb_ChannelSet=0;  //20090513 chiayen add
					gc_ShowTimer=0;  //20090513 chiayen add
					//gc_Task_Next=C_Task_PlayMenu_IR;
				//	gc_Task_Next=C_Task_PlayMenu;
					gc_PhaseInx =C_PlayModeChg;
				if(gc_IRCmdStatus==1)
				  {
				 
					gc_PhaseInx=C_PlayModeChg;
				  }
			   }
				gc_PlayMenu_IR=1;  //20090206 chaiyen add for IR_DIR
				break;
#if 0
			 case 0x04:	// mode//loop mode
			   if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
                    PlayMode_CHG();
				}
				break;
#endif
	        case 0x43:	// paly/pause
				if(gc_IRCmdStatus==1)
				{
                    if(gc_Num_Flag==1)
                    {
                        gc_Num_Flag=0;
                        gb_PickSongSet=1;
                        PickSong_Process();
    
                    }
                    else
                    {
    					
    					gc_PhaseInx=3;
    					gc_ShowTimer=1;
    					gc_IR_Setting=0;
    					ClearIRBuffer();
                    }
				}
			
				break;
	        case 0x09:	// EQ			
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					if((gc_IRCmdStatus==1)&&(gw_FileTotalNumber[0]!=0))
					{
						Change_EQ();
					}
				}
				break;

	        case 0x07:	// volume --
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					VOL_DOWN();
				}
				break;
	        case 0x15:	// volume ++
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					VOL_UP();
				}
				break;
	        case 0x44:	// prev
			 if(gc_IRCmdStatus==1)
				{
					NextorPre_Song(1);
				}
	 			break;
	        case 0x40:	// next
               if(gc_IRCmdStatus==1)
				{
					NextorPre_Song(0);
				}
				break;
			case 0x45:	// CH-
 				if(gc_Num_Flag==1)
                {
                    gc_Num_Flag=0;
               	    gb_PickSong=0;
					gb_ChannelSet=1;
					PickFreq_Process();
                    
                    if(gc_Err==0)
                    {
                        gc_ShowTimer=1;   
                    } 
                }
               
				else
				{
					ClearIRBuffer();
					FM_FREQ_CHG(0,1);
				}
				break;
			case 0x47:	// CH+
				 if(gc_Num_Flag==1)
                {
                    gc_Num_Flag=0;
               	    gb_PickSong=0;
					gb_ChannelSet=1;
					PickFreq_Process();
                    if(gc_Err==0)
                    {
                        gc_ShowTimer=1;   
                    } 
                }
               
				else
				{
					ClearIRBuffer();
					FM_FREQ_CHG(1,1);
				}
				break;
  #if 0
			case 0x1e:	// S/F
				if(gc_IRCmdStatus==1)  //for IRSourceChange
				{
					MediaChange();  //20090107 chiayen add
				}
				break;
			case 0x1d:	// mute
				if(gc_IRCmdStatus==1) 
				{
					if(gc_VolumeMute==0)
					{
						gc_Volumebackup=gs_DSP_GLOBAL_RAM.sw_Volume;
						gs_DSP_GLOBAL_RAM.sw_Volume=0;
						Music_SetVolumeCmd();
						gc_VolumeMute=1;
						gs_DSP_GLOBAL_RAM.sw_Volume=gc_Volumebackup;
					}
					else
					{
						Music_SetVolumeCmd();
						gc_VolumeMute=0;
					}
				}
				break;
			case 0x00:	// power on/off
				if(gc_IRCmdStatus==1)  
				{
				/*	if(LCM_BACKLIGHT==0)
					{
						LCM_BACKLIGHT=1;				
						play_stop();
						LCM_ShowPlayTime(0);
					}
					else if(LCM_BACKLIGHT==1)
					{
						LCM_BACKLIGHT=0;
						gc_PhaseInx=3;
					}*/
					gc_PhaseInx=C_PowerOff;
				}
			break;
 #endif
			case 0x16:	// 0
				SaveNumber(0);
				break;
			case 0x0c:	// 1
				SaveNumber(1);
				break;
			case 0x18:	// 2
				SaveNumber(2);
				break;
			case 0x5e:	// 3
				SaveNumber(3);
				break;
			case 0x08:	// 4
				SaveNumber(4);
				break;
			case 0x1c:	// 5
				SaveNumber(5);
				break;
			case 0x5a:	// 6
				SaveNumber(6);
				break;
			case 0x42:	// 7
				SaveNumber(7);
				break;
			case 0x52:	// 8
				SaveNumber(8);
				break;
			case 0x4a:	// 9
				SaveNumber(9);
				break;
	    }
	}
	else if(gc_irkey==5)	// 23-key
	{
		if(gs_System_State.c_Phase == TASK_PHASE_PAUSE)
		{
	        if(IR_REG[0x1c]==0x02)	// paly/pause
			{
				if(gc_IRCmdStatus==1)
				{
					gc_PhaseInx=3;
					gc_ShowTimer=1;
					gc_IR_Setting=0;
					ClearIRBuffer();
				}
			}
			else if(IR_REG[0x1c]==0x01)
			{
				if(gc_IRCmdStatus==1)
				{
				
					gc_PhaseInx=C_PlayModeChg;
				}
			}
			return;
		}

	    switch(IR_REG[0x1c])
	    {
			U8 tc_SearchDirTimeout;
			case 0x77:	// +100
				if(gc_IRCmdStatus==1)  //20090216 chiayen add
				{					
					if((gw_FileTotalNumber[0]!=0) && (gc_KeyDet_Mask==0)) //20090810 chiayen modify
					{										
				#if 1
						U32 dw_File_StartCluster_Backup=0;
						U32 dw_FDB_StartCluster_Backup=0;
						U8  tc_clock_mode_backup;  //20090817 chiayen add
 
						gb_DirPlay_Flag=1;
				
						play_stop();
						gw_DirOverallNumber = gw_AllDirTotalNumber;

						tc_clock_mode_backup=gc_clock_mode;//20090817 chiayen add
						set_clock_mode(CLOCK_MODE_DOS);  //20090817 chiayen add
						GetCurrentDirInfo();

						for(tc_SearchDirTimeout=0;tc_SearchDirTimeout<=255;tc_SearchDirTimeout++)
						{
							DOS_Search_DIR_Prev_test(0x23);
	
							gb_FindFlag = 0;

							gs_File_FCB[0].dw_FDB_StartCluster = gs_DIR_FCB[0].dw_File_StartCluster;

							dw_File_StartCluster_Backup=gs_DIR_FCB[0].dw_File_StartCluster;
							dw_FDB_StartCluster_Backup=gs_DIR_FCB[0].dw_FDB_StartCluster;

							gc_test=0;
							DOS_Search_File(0x10,0,0x10);

							if(gw_FileTotalNumber[0])
							{
								DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
								gw_FileIndex[0]=1;
								break;
							}
							gs_DIR_FCB[0].dw_File_StartCluster=dw_File_StartCluster_Backup;
							gs_DIR_FCB[0].dw_FDB_StartCluster=dw_FDB_StartCluster_Backup;
						}
						set_clock_mode(tc_clock_mode_backup);  //20090817 chiayen add

						if(gs_File_FCB[0].dw_FDB_StartCluster == gdw_DOS_RootDirClus)
						{
//						    LCM_clear_dir();
							LCM_ShowFloderIcon(0);
						}
						else
						{
							UI_DispGet_DirName();
							LCD_Disp_DirName();
						}
						#if (LCM_SIZE==1)  //20090225
						LCM_ShowSongNumber_1();  //20090205 chiayen test
						#else
						LCM_ShowSongNumber();
						#endif
						gc_Dirchange_Timer=150;
						gb_FindFlag = 1;
						gc_PhaseInx = 3;
						gb_DirPlay_Flag=1;
						gc_FolderPlay_Flag=1;  //20090216 chiayen add
						gc_KeyDet_Mask=1;  //20090810 chiayen add
				#else
						if(gw_FileTotalNumber[0]<100)
						{
							// Show Err
							gc_Err=1;
							gc_ShowTimer=60;
						}
						else
						{
							if(gw_FileTotalNumber[0]!=0)
							{
								Pick_Song_Init();
								gc_NUMBER[2]=1;
							}
						}
				#endif
					}
				}
				break;
			case 0x66:	// +200
				if(gc_IRCmdStatus==1)  //20090216 chiayen add
				{
					if((gw_FileTotalNumber[0]!=0) && (gc_KeyDet_Mask==0))  //20090810 chiayen modify
					{
				#if 1
						U8  tc_clock_mode_backup;  //20090817 chiayen add
						play_stop();

						tc_clock_mode_backup=gc_clock_mode;//20090817 chiayen add
						set_clock_mode(CLOCK_MODE_DOS);  //20090817 chiayen add

						for(tc_SearchDirTimeout=0;tc_SearchDirTimeout<=255;tc_SearchDirTimeout++)
						{
							gs_DIR_FCB[0].dw_FDB_StartCluster = gs_File_FCB[0].dw_FDB_StartCluster;							
							DOS_Search_DIR(0x23);

							gb_FindFlag = 0;													
							if(gc_test==0)
							{
								gs_File_FCB[0].dw_FDB_StartCluster = gs_DIR_FCB[0].dw_File_StartCluster;
							}
							else
							{
								gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
								gc_test=0;
							}
							DOS_Search_File(0x10,0,0x10);
							if(gw_FileTotalNumber[0])
							{
								DOS_Search_File(C_File_All|C_By_Time, C_MusicFileType, C_CmpExtName|C_Next);
								gw_FileIndex[0]=1;
								break;
							}														
						}
						set_clock_mode(tc_clock_mode_backup);  //20090817 chiayen add

						if(gs_File_FCB[0].dw_FDB_StartCluster == gdw_DOS_RootDirClus)
						{
							LCM_ShowFloderIcon(0);
						}
						else
						{
							UI_DispGet_DirName();
							LCD_Disp_DirName();
						}
						#if (LCM_SIZE==1)  //20090225
						LCM_ShowSongNumber_1();  //20090205 chiayen test
						#else
						LCM_ShowSongNumber();
						#endif
						gc_Dirchange_Timer=150;
						gb_FindFlag = 1;
						gc_PhaseInx = 3;
						gb_DirPlay_Flag=1;
						gc_FolderPlay_Flag=1;  //20090216 chiayen add
						gc_KeyDet_Mask=1;  //20090810 chiayen add				
				#else
						if(gw_FileTotalNumber[0]<200)
						{
							// Show Err
							gc_Err=1;
							gc_ShowTimer=60;
							ClearIRBuffer();
						}
						else	
						{
							if(gw_FileTotalNumber[0]!=0)
							{
								Pick_Song_Init();
								gc_NUMBER[2]=2;
							}
						}
				 #endif
					}				
				}
				break;
             case 0x01:	// Menu
				if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
					gb_PickSong=0;  //20090513 chiayen add
					gb_ChannelSet=0;  //20090513 chiayen add
					gc_ShowTimer=0;  //20090513 chiayen add
                    gc_Num_Flag=0;
					//gc_Task_Next=C_Task_PlayMenu_IR;
				//	gc_Task_Next=C_Task_PlayMenu;
					gc_PhaseInx =C_PlayModeChg;
				}
				gc_PlayMenu_IR=1;  //20090206 chaiyen add for IR_DIR
				break;
			 case 0x04:	// mode//loop mode
			   if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
                    PlayMode_CHG();
				}
				break;
	        case 0x02:	// paly/pause
				if(gc_IRCmdStatus==1)
				{
                    if(gc_Num_Flag==1)
                    {
                        gc_Num_Flag=0;
                        gb_PickSongSet=1;
                        PickSong_Process();
    
                    }
                    else
                    {
    					
    					gc_PhaseInx=3;
    					gc_ShowTimer=1;
    					gc_IR_Setting=0;
    					ClearIRBuffer();
                    }
				}
				break;
	        case 0x08:	// EQ			
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					if((gc_IRCmdStatus==1)&&(gw_FileTotalNumber[0]!=0))
					{
						Change_EQ();
					}
				}
				break;

	        case 0x09:	// volume --
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					VOL_DOWN();
				}
				break;
	        case 0x0a:	// volume ++
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					VOL_UP();
				}
				break;
	        case 0x0d:	// prev
                if(gc_Num_Flag==1)
                {
                    gc_Num_Flag=0;
                    gb_PickSongSet=1;
                    PickSong_Process();

                } else if(gc_IRCmdStatus==1)
				{
					NextorPre_Song(1);
				}
	 			break;
	        case 0x0e:	// next
                if(gc_Num_Flag==1)
                {
                    gc_Num_Flag=0;
                    gb_PickSongSet=1;
                    PickSong_Process();

                }else if(gc_IRCmdStatus==1)
				{
					NextorPre_Song(0);
				}
				break;
			case 0x05:	// CH-
                if(gc_Num_Flag==1)
                {
                    gc_Num_Flag=0;
               	    gb_PickSong=0;
					gb_ChannelSet=1;
					PickFreq_Process();
                    
                    if(gc_Err==0)
                    {
                        gc_ShowTimer=1;   
                    } 
                }
               
				else
				{
					ClearIRBuffer();
					FM_FREQ_CHG(0,1);
				}
				break;
			case 0x06:	// CH+
                if(gc_Num_Flag==1)
                {
                    gc_Num_Flag=0;
               	    gb_PickSong=0;
					gb_ChannelSet=1;
					PickFreq_Process();
                    if(gc_Err==0)
                    {
                        gc_ShowTimer=1;   
                    } 
                }
               
				else
				{
				ClearIRBuffer();
				FM_FREQ_CHG(1,1);
				}
				break;
			case 0x1e:	// S/F
				if(gc_IRCmdStatus==1)  //for IRSourceChange
				{
					MediaChange();  //20090107 chiayen add
				}
				break;
			case 0x1d:	// mute
				if(gc_IRCmdStatus==1) 
				{
					if(gc_VolumeMute==0)
					{
						gc_Volumebackup=gs_DSP_GLOBAL_RAM.sw_Volume;
						gs_DSP_GLOBAL_RAM.sw_Volume=0;
						Music_SetVolumeCmd();
						gc_VolumeMute=1;
						gs_DSP_GLOBAL_RAM.sw_Volume=gc_Volumebackup;
					}
					else
					{
						Music_SetVolumeCmd();
						gc_VolumeMute=0;
					}
				}
				break;
			case 0x00:	// power on/off
				if(gc_IRCmdStatus==1)  
				{
				/*	if(LCM_BACKLIGHT==0)
					{
						LCM_BACKLIGHT=1;				
						play_stop();
						LCM_ShowPlayTime(0);
					}
					else if(LCM_BACKLIGHT==1)
					{
						LCM_BACKLIGHT=0;
						gc_PhaseInx=3;
					}*/
					gc_PhaseInx=C_PowerOff;
				}
			break;
			case 0x0c:	// 0
				SaveNumber(0);
				break;
			case 0x10:	// 1
				SaveNumber(1);
				break;
			case 0x11:	// 2
				SaveNumber(2);
				break;
			case 0x12:	// 3
				SaveNumber(3);
				break;
			case 0x14:	// 4
				SaveNumber(4);
				break;
			case 0x15:	// 5
				SaveNumber(5);
				break;
			case 0x16:	// 6
				SaveNumber(6);
				break;
			case 0x18:	// 7
				SaveNumber(7);
				break;
			case 0x19:	// 8
				SaveNumber(8);
				break;
			case 0x1a:	// 9
				SaveNumber(9);
				break;
	    }
	}
	else if(gc_irkey==6)	// 11-key
	{
		if(gs_System_State.c_Phase == TASK_PHASE_PAUSE)
		{
	        if(IR_REG[0x1c]==0x00)	// paly/pause
			{
				if(gc_IRCmdStatus==1)
				{
					gc_PhaseInx=3;
					gc_ShowTimer=1;
					ClearIRBuffer();
				}
			}
			return;
		}

	    switch(IR_REG[0x1c])
	    {
	        case 0x02:	// Frequency UP
				FM_FREQ_CHG(1,1);
				gw_ChangeFrequencyTimer=120;
				break;
			case 0x01:	// Frequency DOWN
				FM_FREQ_CHG(0,1);
				gw_ChangeFrequencyTimer=120;
				break;
	        case 0x00:	// paly/pause				
				if(gc_IRCmdStatus==1)
				{
					gc_PhaseInx=3;
					gc_ShowTimer=1;
					ClearIRBuffer();
				}
				break;
	        case 0x09:	// prev
				if(gc_IRCmdStatus==1)
				{
					gw_ChangeFrequencyTimer=0;
					NextorPre_Song(1);
				}
				break;
	        case 0x0A:	// next
				if(gc_IRCmdStatus==1)
				{
					gw_ChangeFrequencyTimer=0;
					NextorPre_Song(0);
				}
				break;
	        case 0x05:	// volume --
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					VOL_DOWN();
				}
				break;
	        case 0x06:	// volume ++
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					VOL_UP();
				}
				break;
			  case 0x1e:	// S/F
				if(gc_IRCmdStatus==1)  //for IRSourceChange
				{
					MediaChange();  //20090107 chiayen add
				}
				break;
			case 0x1d:	// mute
				if(gc_IRCmdStatus==1) 
				{
					if(gc_VolumeMute==0)
					{
						gc_Volumebackup=gs_DSP_GLOBAL_RAM.sw_Volume;
						gs_DSP_GLOBAL_RAM.sw_Volume=0;
						Music_SetVolumeCmd();
						gc_VolumeMute=1;
						gs_DSP_GLOBAL_RAM.sw_Volume=gc_Volumebackup;
					}
					else
					{
						Music_SetVolumeCmd();
						gc_VolumeMute=0;
					}
				}
				break;
			case 0x08:	// power on/off
				if(gc_IRCmdStatus==1)  
				{
				/*	if(LCM_BACKLIGHT==0)
					{
						LCM_BACKLIGHT=1;				
						play_stop();
						LCM_ShowPlayTime(0);
					}
					else if(LCM_BACKLIGHT==1)
					{
						LCM_BACKLIGHT=0;
						gc_PhaseInx=3;
					}*/
					gc_PhaseInx=C_PowerOff;
				}
				break;

			case 0x04:	// EQ
				if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
				{
					if((gc_IRCmdStatus==1)&&(gw_FileTotalNumber[0]!=0))
					{
						Change_EQ();
					}
				}
				break;
	    }
	}
#endif
}

void SaveNumber(U8 number)
{
#if 0

	if(gc_IRCmdStatus==1)
	{
		gw_IR_Timer=120;
		if(gb_ChannelSet==0 && gb_PickSong==0&&gc_irkey!=3)
		{
			gc_Num_Flag = 1;
			gc_NUMBER[3]=gc_NUMBER[2];
			gc_NUMBER[2]=gc_NUMBER[1];
			gc_NUMBER[1]=gc_NUMBER[0];
			gc_NUMBER[0]=number;	
		}else
		{
			if((gc_NUMBER[0]==0xFF) && (gc_NUMBER[2]!=1) && ((gc_irkey!=3)) && ((XBYTE[0xB40B]&0x06)!=0x02) && (gb_ChannelSet==0))
			{
				if(gw_FileTotalNumber[0]!=0)
				{
					Pick_Song_Init();
				}
				else
				{
					return;
				}
			}
	
			if(((gc_NUMBER[2]==2)||(gc_NUMBER[2]==1))&&(gb_PickSong==1))
			{
				if(gc_NUMBER[0]==0xFF)
				{
					gc_NUMBER[0]=number;
				}
				else if(gc_NUMBER[1]==0xFF)
				{
					gc_NUMBER[1]=gc_NUMBER[0];
					gc_NUMBER[0]=number;
				}
			}
			else
			{
				gc_NUMBER[3]=gc_NUMBER[2];
				gc_NUMBER[2]=gc_NUMBER[1];
				gc_NUMBER[1]=gc_NUMBER[0];
				gc_NUMBER[0]=number;
			}

		}
		if(gc_Num_Flag==1)
		{
		 
		}else if(gb_ChannelSet==1)
		{
			if(((gc_NUMBER[2]!=0xff)&&(gc_NUMBER[2]!=0x01))||(gc_NUMBER[3]!=0xff))
			{
				U16	frequency;
				frequency=gc_NUMBER[2]*100+gc_NUMBER[1]*10+gc_NUMBER[0];
				if(gc_NUMBER[3]!=0xFF)
				{
					frequency+=1000;
				}
				if(((frequency>=875)&&(frequency<=1080))||((frequency>=881)&&(frequency<=1079)&&((P3&0x01)==0x01)))
				{
					gw_FM_frequency=frequency;
					FM_drive();
					gc_ShowTimer=72;
					gw_DisplayFreq=0xFFFF;	// Force display frequency again
				}
				else
				{
					gc_Err=1;	// Show Err
					gc_ShowTimer=72;
				}
				gb_ChannelSet=0;
			}
		}else if(gb_PickSong==1)
		{
			if((gc_NUMBER[2]!=0xFF)&&(gc_NUMBER[0]!=0xFF)&&(gc_NUMBER[1]!=0xFF))
			{ 
				LCM_Display(); 
				gb_PickSongSet=1;
				gw_IR_Timer=120;
				gb_Frequency_Song=1;
			}
		}
	}
#endif
}

void PickFreq_Process(void)
{
#if 0
 
	if(gb_ChannelSet==1)
	{
		if(((gc_NUMBER[2]!=0xff)&&(gc_NUMBER[2]!=0x01))||(gc_NUMBER[3]!=0xff))
		{
			U16	frequency;
			frequency=gc_NUMBER[2]*100+gc_NUMBER[1]*10+gc_NUMBER[0];
			if(gc_NUMBER[3]!=0xFF)
			{
				frequency+=1000;
			}
			if(((frequency>=875)&&(frequency<=1080))||((frequency>=881)&&(frequency<=1079)&&((P3&0x01)==0x01)))
			{
				gw_FM_frequency=frequency;
				FM_drive();
				gc_ShowTimer=72;
				gw_DisplayFreq=0xFFFF;	// Force display frequency again
			}
			else
			{
				gc_Err=1;	// Show Err
				gc_ShowTimer=72;
			}
			gb_ChannelSet=0;
			gc_IRNum_Count=0;
		}
	}
#endif
}

#if 0
void ClearIRBuffer(void)
{
	gb_ChannelSet=0;
	gb_PickSong=0;
	gc_NUMBER[0]=0xFF;
	gc_NUMBER[1]=0xFF;
	gc_NUMBER[2]=0xFF;
	gc_NUMBER[3]=0xFF;
}


void Pick_Song_Init(void)
{
	if(gw_FileTotalNumber[0]!=0)
	{
		gb_PickSong=1;
		gb_ChannelSet=0;
		gc_Err=0;
		gw_IR_Timer=120;
		gc_NUMBER[0]=0xFF;
		gc_NUMBER[1]=0xFF;
		gc_NUMBER[2]=0xFF;
		gc_NUMBER[3]=0xFF;
	}
}


void Channel_Set_Init(void)
{
	gb_ChannelSet=1;
	gb_PickSong=0;
	gc_Err=0;
	gw_IR_Timer=120;
	gc_NUMBER[0]=0xFF;
	gc_NUMBER[1]=0xFF;
	gc_NUMBER[2]=0xFF;
	gc_NUMBER[3]=0xFF;
}

		

void Frequency_Disp(void)
{
	gc_ShowTimer=72;
	gw_DisplayFreq=gw_FM_frequency;
	if(gc_LCM_Media!=15)
	{
		gc_LCM_Media=15;
		LCM_ClearScreen();
		LCM_ShowFrequency_L();
		gw_DisplayTime=0xFFFF;
	}	
	gb_PickSong=0;
	gc_Err=0;
	gc_NUMBER[0]=gw_FM_frequency%10;
	gc_NUMBER[1]=(gw_FM_frequency%100)/10;
	gc_NUMBER[2]=(gw_FM_frequency%1000)/100;
	if(gw_FM_frequency>999)
	{
		gc_NUMBER[3]=1;
	}
	else
	{
		gc_NUMBER[3]=0xFF;
	}
}


void Change_EQ(void)
{
	gc_Err=0;
	ClearIRBuffer();
	gw_LogData_Timer=60; //chiayen0812
	gw_IR_Setting_Timer=120;
	gc_IR_Setting=1;
	gs_DSP_GLOBAL_RAM.sc_EQ_Type++;
	if(gs_DSP_GLOBAL_RAM.sc_EQ_Type>5)
	{
		gs_DSP_GLOBAL_RAM.sc_EQ_Type=0;	
	}
	Music_EQ_Cmd(gs_DSP_GLOBAL_RAM.sc_EQ_Type);
	gc_ShowTimer=72;
	gb_Frequency_Song=1;	// 0=Show Frequency    1=Show Song Number/EQ
	gc_SelectEQ=1;			// 0=Show Song Number  1=Show EQ
	gb_PickSongSet =0;
	gc_Num_Flag =0;
	
}


void VOL_UP(void)
{	
	gc_Err=0;
	ClearIRBuffer();
	gw_IR_Setting_Timer=120;
	gc_IR_Setting=1;
	gc_PhaseInx=12;
}


void VOL_DOWN(void)
{
	gc_Err=0;			
	ClearIRBuffer();
	gw_IR_Setting_Timer=120;
	gc_IR_Setting=1;
	gc_PhaseInx=13;
}


void NextorPre_Song(U8 tb_state)
{
	if((gc_RepPlayMode==C_RandomPlay && (gc_IRdetMask==1)) || (gb_FlashNoFileflag))
	{
		return;
	}

	if(gc_RepPlayMode==C_RandomPlay)
	{
		gc_IRdetMask=1;
		ClearIRBuffer();
		play_next();
		gc_IRdetMask=0;
//		gw_DisplayFreq = 0xffff;
	}
	else
	{
		gc_NextorPreFlag=tb_state;
		gw_irkey_count=1;
		ClearIRBuffer();
	}
}
#endif

void PickSong_Process(void)
{
#if 0

	if(gb_PickSongSet==1)
	{
		U16	tw_SongNumber=0;

		gc_SelectVol=0;
		gb_PickSongSet=0;
		gw_DispSongNum=0xFFFF;
		gc_IRNum_Count=0;

		if((gc_NUMBER[2]==2)||(gc_NUMBER[2]==1))	// For 200+ & 100+
		{
			if(gc_NUMBER[0]==0xFF)
			{
				gc_NUMBER[0]=0;
			}
			if(gc_NUMBER[1]==0xFF)
			{
				gc_NUMBER[1]=0;
			}
		}

        	if(gc_NUMBER[0]!=0xFF)
			{
				tw_SongNumber=gc_NUMBER[0];
				if(gc_NUMBER[1]!=0xFF)
				{
					tw_SongNumber+=gc_NUMBER[1]*10;
					if(gc_NUMBER[2]!=0xFF)
					{
						tw_SongNumber+=gc_NUMBER[2]*100;

					}
				}
			}
		if((tw_SongNumber<=gw_FileTotalNumber[0])&&(tw_SongNumber!=0))
		{
			if(tw_SongNumber==gw_FileIndex[0])
			{
				gw_FileSkipNumber=1;
			}
			else if(tw_SongNumber>gw_FileIndex[0])
			{
				gw_FileSkipNumber=tw_SongNumber-gw_FileIndex[0];
				play_next();
			}
			else
			{
				gw_FileSkipNumber=(gw_FileTotalNumber[0]-gw_FileIndex[0])+tw_SongNumber;
				play_next();
			}
			gc_ShowTimer=72;
			gb_Frequency_Song=1;
			gb_PickSong=0;
            gc_Num_Flag=0;
		}
		else
		{
			gc_ShowTimer=0;	
			gb_PickSong=0;
            gc_Num_Flag=0;
			if(tw_SongNumber==0)
			{
				gc_Err=0;
				gb_Frequency_Song=1;
			}
			else
			{
				gc_Err=1;	// Show Err
				gb_Frequency_Song=0;
			}
			gc_ShowTimer=60;

		}
	}

	if ((gw_SongNumber!=gw_FileIndex[0])&&(gc_CurrentCard==5)&&(gw_FileTotalNumber[0]!=0))
	{
		gc_DisplaySongnameCount=0;
		gw_SongNumber=gw_FileIndex[0];
	}

	if ((gw_SongNumberSD!=gw_FileIndex[0])&&((gc_CurrentCard==3)||(gc_CurrentCard==2))&&(gw_FileTotalNumber[0]!=0))
	{
		gc_DisplaySongnameCount=0;
		gw_SongNumberSD=gw_FileIndex[0];
	}
#endif
}

void MediaChange(void)  //20090107 chiayen add
{
#if 0

	if((gb_SD_Exist==1) && (gb_Host_Exist==1))
	{
		if(gc_CurrentCard==0)
		{
			gc_CurrentCard=2;	
		}
		else if(gc_CurrentCard==2)
		{
			gc_CurrentCard=5;
		}
		else
		{
			gc_CurrentCard=0;
		}	
	}
	else if((gb_SD_Exist==1) && (gb_Host_Exist==0))
	{
		if(gc_CurrentCard==0)
		{
			gc_CurrentCard=2;	
		}
		else if(gc_CurrentCard==2)
		{
			gc_CurrentCard=0;
		}
	}
	else if((gb_SD_Exist==0) && (gb_Host_Exist==1))
	{
		if(gc_CurrentCard==0)
		{
			gc_CurrentCard=5;	
		}
		else if(gc_CurrentCard==5)
		{
			gc_CurrentCard=0;
		}
	}
#endif
}

#if 0
void UI_DispGet_DirName(void)
{
	WORD tw_Length;
	UBYTE tc_TableOffset;

	memcpy(&gs_File_FCB[2].dw_LongFDB_LogAdd0,&gs_DIR_FCB[0].dw_LongFDB_LogAdd0,4);//it's useful when the LFN FDB is at two sectors
   	memcpy(&gs_File_FCB[2].dw_FDB_LogAdd,&gs_DIR_FCB[0].dw_FDB_LogAdd,4);
    memcpy(&gs_File_FCB[2].w_FDB_Offset,&gs_DIR_FCB[0].w_FDB_Offset,2);

	DOS_GetLongFileName(2,gc_FileLongName);
	memcpy(&tw_Length,&gc_FileLongName[3],2);

	if(tw_Length>/*C_BUF_STRING*/122) 
	{
		memcpy(&gc_DirLCMtempBuf[0], &gc_FileLongName[0],/*C_BUF_STRING*/122+6);
		gc_DirLCMtempBuf[1] = 2;	//dir
		gc_DirLCMtempBuf[3] = 0;	//high byte of length
		gc_DirLCMtempBuf[4] = 122;//low byte of length
	}
	else
	{
		tc_TableOffset = (U8)(tw_Length+6);
		memcpy(&gc_DirLCMtempBuf[0], &gc_FileLongName[0],tc_TableOffset);
		gc_DirLCMtempBuf[1] = 2;	//dir
	}
}


void LCD_Disp_DirName(void)
{
	#if (LCM_SIZE==1)
	LCM_clear();
	#else
	LCM_clear_dir();
	#endif

	LCD_disp_HZK_string(2, &gc_DirLCMtempBuf[5], gc_DirLCMtempBuf[2], gc_DirLCMtempBuf[4], 0, 0); //Ching 080926
	gc_LCM_Media=0xff;
	gc_DisplayEQIcon=0xFF;
	gw_DisplayBitRate=0xFFFF;
	gw_DispSongNum1=0xFF;

	#if (LCM_SIZE==1)
	gc_LCM_line=0;
	gw_DisplayTime=0xFFFF;
	gw_DisplayFreq1=0xFFFF;
	#endif
	gw_DispFileName_ByteOffset=0;
}

void PlayMode_CHG(void)  //chiayen0807
{
	gc_Num_Flag =0;
	gb_PickSong=0;
    gc_ShowTimer=40;
    gw_LogData_Timer=60; //chiayen0812
    gc_RepPlayMode++;
    if(gc_RepPlayMode>3)
    {
        gc_RepPlayMode=1;               
    }
    if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        gc_PhaseInx=0x09;   
    }
    else
    {
        gc_PhaseInx=1;
    }
}
#endif