#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\header\dos\fs_struct.h"
#include "..\IR\remote.h"
#include "..\IR\FM.h"
#include "..\header\Memalloc.h"
#include "..\header\variables.h"
#include "..\LCM\TFT_display.h"		// TFT

extern xdata System_Struct gs_System_State;
extern DSP_GLOBAL gs_DSP_GLOBAL_RAM;

extern void Music_EQ_Cmd(U8 tc_Type);
extern void Music_SetVolumeCmd(void);
extern void LCM_ShowSongNumber_1(void);
extern U8 DOS_Search_DIR(U8 tc_Mode);  //20090121 chiayen add
extern U8 LCD_disp_HZK_string(U8 Page,U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 ColumnOffset,bit reverse); //20090121 chiayen add

extern U8 LCD_Disp_FileName(U8 *tpc_DataBuf, U8 tc_ANSI, U8 tc_nByte, U8 tc_scroll_flag);
extern UBYTE DOS_GetDIRName(UBYTE tc_FileHandle, UBYTE tb_UicodeToISN, UBYTE * tpc_LongName);

extern void UI_DispGetDirName(U8 tc_DIRtableIndex);

void ir_init(void);
void ir_service(void);
void ir_commandservice(void);
void SaveNumber(U8 number);
void ClearIRBuffer(void);
void Frequency_Disp(void);
void Channel_Set_Init(void);
void Change_EQ(void);
void VOL_UP(void);
void VOL_DOWN(void);
void CHG_DN(void);
void CHG_UP(void);
void PickSong_Process(void);
void NextorPre_Song(bit tb_state);
void MediaChange(void);  //20090107 chiayen add
void LCD_Disp_DirName(void); //20090121 chiayen add
void ChannelSet_Process(void);//20090409 chiayen add

extern U8	DOS_Search_DIR_Prev_test(U8 tc_Mode);  //20090216 chiayen add
extern U8	DOS_Search_DIR_Next_test(U8 tc_Mode);

extern U8 GetCurrentDirInfo();

extern	data	U8	gc_ShowIRNumMain;
extern	data	bit	gb_TFT_refresh;

xdata U8 gb_NumCHSet=0;
xdata U8 gb_NumSet=0;

void ir_init(void)
{
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
	while(1);
#endif

	IR_REG[0x19] = 0x07; // clear all events
	IR_REG[0x18] = 0x07; // enable all IR interrupts
	IR_REG[0x17] |= 0x80; // IR reveiver HW enabled
	IR_REG[0x19] = 0x07; // clear all events
	EX0 = 1; EA = 1;
}


void ir_service(void)
{
	U16 tw_temp;	

	if(gc_IRCmdStatus!=0)
	{
		tw_temp=(IR_REG[0x1b]<<8)+IR_REG[0x1a];

		 switch(tw_temp)
        {
           case IR_6_Key:
                gc_irkey = 1;
                break;
           case IR_7_Key:
                gc_irkey = 2;
                break;
           case IR_18_Key:
                gc_irkey = 3;
                break;
           case IR_20_Key:
                gc_irkey = 4;
                break;
           case IR_21_Key:
                gc_irkey = 5;
                break;
           case IR_21_1_Key:
                gc_irkey = 7;
                break;
           default:
                gc_irkey = 0;
                break;
        }
		if(gc_irkey!=0)
		{
			if((IR_REG[0x1c]+IR_REG[0x1d])==0xFF)
			{
				if(gc_IRCmdStatus==1)
				{
					ir_commandservice();
					gc_IRCmdStatus=0;
				}
				if(gc_IRCmdStatus==2)
				{					
						ir_commandservice();
						gc_IRCmdStatus=0;
				}
			}					
		}
		else
		{
			gc_IRCmdStatus=0;
		}
		gc_IRCmdStatus=0;
	}
	gc_irkey=0;
}


void ir_commandservice(void)
{
	U8	tc_SearchDirTimeout;
								  

///======================IR=======IR========IR===========================
///======================IR=======IR========IR===========================
///======================IR=======IR========IR===========================
///======================IR=======IR========IR===========================
	if(gc_irkey==5)	// 21-1key
	{
//		dbprintf("IR Key=%bx\n",IR_REG[0x1c]);

	    switch(IR_REG[0x1c])
	    {
			case 0x00:	//ON/OFF 
				if(gc_IRCmdStatus==1)  //for IRSourceChange
				{
					if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
					{
						if(gc_Task_Current == C_Task_Play)
						{
							play_stop();
						}
						else if(gc_Task_Current == C_Task_Jpeg)
						{
							jpeg_stop();
						}
						else if(gc_Task_Current == C_Task_Mjpeg)
						{
							mjpeg_stop();
						}
					}
                  	gc_Task_Next=C_Task_PowerOff;
					gc_PhaseInx=0xff; 
				}

				break;
			case 0x1e:								//just for suo lang.
			case 0x01:	//Mode	//Menu   
				if(gc_Task_Current == C_Task_Play)
                { 
					if((gc_IRCmdStatus==1) && (gc_TuneVolFreqStatus <3))
					{	
                 		play_stop();
				    	gc_Task_Next=C_Task_Menu;
					}
				}
				else if(gc_Task_Current == C_Task_Jpeg)
				{
					if((gc_IRCmdStatus==1) && (gc_TuneVolFreqStatus <31))
					{
						jpeg_stop();
				    	gc_Task_Next=C_Task_Menu;
					}				
					
				}
				else if(gc_Task_Current == C_Task_Mjpeg)
				{
					if((gc_IRCmdStatus==1) && (gc_TuneVolFreqStatus <21))
					{
						mjpeg_stop();
				    	gc_Task_Next=C_Task_Menu;
					}
				}
				ClearIRBuffer();
				break;

			case 0x04:	//RPT   
 #if 0
				if(gc_IRCmdStatus==1)  //20090216 chiayen add
				{					
					if(gw_FileTotalNumber[0]!=0)
					{	
						U32 dw_File_StartCluster_Backup=0;
						U32 dw_FDB_StartCluster_Backup=0;
 
						gb_DirPlay_Flag=1;
				
						play_stop();
						gw_DirOverallNumber = gw_DirTotalNumber;
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

						gc_Dirchange_Timer=5;
						gw_LogData_Timer=2;
						gb_TFT_refresh=1;
						if(gb_LrcFileName_Exist==1)
						{
							gc_DispWallpaper=0;
						}
						gc_PhaseInx = 3;
						gb_DirPlay_Flag=1;
						gc_FolderPlay_Flag=1;
					}
				}
                                 gc_folder_disp=1; //sunzhk add
//				dbprintf("curind=%x,totle=%x\n",gw_FileIndex[0],gw_FileTotalNumber[0]);
				//	DOS_Search_File(C_File_OneDir|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);
#else
				if(gc_IRCmdStatus==1)
				{
					if(gc_Task_Current == C_Task_Play)
					{
                  		gc_ShowTimer=72;
						if(gc_DispPlayMenuAct==0)
						{
							gc_TuneVolFreqStatus=3;
						}
					}else if(gc_Task_Current == C_Task_Jpeg)
					{
						if(gc_DispPlayMenuAct==0)
						{
							gc_TuneVolFreqStatus=31;
						}				
					}else if(gc_Task_Current == C_Task_Mjpeg)
					{
						if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
						{
							mjpeg_playpause();
						}
		
		 		    	gc_ShowTimer=72;
						if((gc_DispPlayMenuAct==0) || (gc_DispPlayMenuAct==0x11))
						{
							gc_TuneVolFreqStatus=21;
						}					
					}
				}
#endif
			
				break;

	        case 0x02:	// paly/pause
				if(gc_IRCmdStatus==1)
				{
					if(gb_NumSet==0)
					{
					if(gc_Task_Current == C_Task_Play)
					{
                  		gc_PhaseInx=3;
						gc_ShowTimer=1;
						
					}else if(gc_Task_Current == C_Task_Jpeg)
					{
						gc_PhaseInx=3;
					}
					else if(gc_Task_Current == C_Task_Mjpeg)
					{
						gc_PhaseInx=3;
					}
					ClearIRBuffer();
				}
					else if(gb_NumSet==1)
	                {
		              	gb_PickSongSet=1;
						gw_IR_Timer=1;
						gb_Frequency_Song=1;
		              
		              	gb_NumSet=0;
	                }
				}
				break;
	        case 0x08:
#if 0	// DIR <<
				if(gc_IRCmdStatus==1)  //20090216 chiayen add
				{
					if(gw_FileTotalNumber[0]!=0)
					{
						play_stop();
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

						gc_Dirchange_Timer=5;
						gb_TFT_refresh=1;
						if(gb_LrcFileName_Exist==1)
						{
							gc_DispWallpaper=0;
						}

						gb_FindFlag = 1;
						gc_PhaseInx = 3;
						gb_DirPlay_Flag=1;
						gc_FolderPlay_Flag=1;  //20090216 chiayen add				
					}				
				}
                                gc_folder_disp=1; //sunzhk add
#else
				if(gc_Task_Current == C_Task_Play)
				{
					if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
					{
						if((gc_IRCmdStatus==1)&&(gw_FileTotalNumber[0]!=0))
						{
							Change_EQ();
						}
					}
				}

#endif
	// EQ
				break;
	        case 0x09:	// volume --
				if((gc_Task_Current == C_Task_Play)||(gc_Task_Current == C_Task_Mjpeg))
				{
					VOL_DOWN();
				}
				break;
	        case 0x0a:	// volume ++
				if((gc_Task_Current == C_Task_Play)||(gc_Task_Current == C_Task_Mjpeg))
				{
					VOL_UP();
				}
				break;
	        case 0x0d:	// prev	
 				if(gc_TuneVolFreqStatus < 3)
				{
					if((gc_IRCmdStatus==1) && (gb_NumCHSet==0))
					{
						NextorPre_Song(1);
					}
				}

	 			break;
	        case 0x0e:	// next	
						
				if(gc_TuneVolFreqStatus < 3)
				{
						if((gc_IRCmdStatus==1) && (gb_NumCHSet==0))
						{
							NextorPre_Song(0);
						}
				}
				break;
				case 0x06:	// CH+
				if((gc_Task_Current == C_Task_Play) ||(gc_Task_Current == C_Task_Mjpeg))
				{
					if(gb_NumSet==0)
					{
						ClearIRBuffer();
						CHG_UP();
					}
					else if(gb_NumSet==1)
					{
						ChannelSet_Process();
						gc_ShowIRNumMain=0;
						gb_ChannelSet=1;
						gb_NumSet=0;
						ClearIRBuffer();
					}
				}

				break;
			case 0x05:	// CH-
				if((gc_Task_Current == C_Task_Play) ||(gc_Task_Current == C_Task_Mjpeg))
				{
					if(gb_NumSet==0)
					{
						ClearIRBuffer();
						CHG_DN();
					}
					else if(gb_NumSet==1)
					{
						ChannelSet_Process();
						gc_ShowIRNumMain=0;
						gb_ChannelSet=1;
						gb_NumSet=0;
						ClearIRBuffer();
					}
				}
				
				break;
			case 0x1d:	 //mute
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
		/*	case 0x1e:	  //u/sd
				if(gc_IRCmdStatus==1)  //for IRSourceChange
				{
					MediaChange();  //20090107 chiayen add
				}				
				break;
			case 0x0a:				//CH_set
				if(gb_NumSet==1)
				{
					ChannelSet_Process();
					gb_ShowIRNumMain=0;
					gb_ChannelSet=1;
					gb_NumSet=0;
				}	
				else
				{
					ClearIRBuffer();
				}
				break;

			case 0x08:				//PkSong
				if(gb_NumSet==1)
                {
	              	gb_PickSongSet=1;
					gw_IR_Timer=1;
					gb_Frequency_Song=1;
	              
	              	gb_NumSet=0;
                }
			  	else
			  	{
			  		ClearIRBuffer();
			  	}
			  	
				break;*/
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
			case 0x1A:	// 9
				SaveNumber(9);
				break;
	    }
	}
	if(gc_irkey==7)	// 21-1key
	{
//		dbprintf("IR Key=%bx\n",IR_REG[0x1c]);

	    switch(IR_REG[0x1c])
	    {

			case 0x05:	// paly/pause
					if(gc_IRCmdStatus==1)
				{
					if(gb_NumSet==0)
					{
						if(gc_Task_Current == C_Task_Play)
						{
	                  		gc_PhaseInx=3;
							gc_ShowTimer=1;
							
						}else if(gc_Task_Current == C_Task_Jpeg)
						{
							gc_PhaseInx=3;
						}
						else if(gc_Task_Current == C_Task_Mjpeg)
						{
							gc_PhaseInx=3;
						}
							ClearIRBuffer();
					}
					else if(gb_NumSet==1)
	                {
		              	gb_PickSongSet=1;
						gw_IR_Timer=1;
						gb_Frequency_Song=1;
		              
		              	gb_NumSet=0;
	                }
					else
					{
						ClearIRBuffer();
					}
				}
				break;
		#if 0									//just for suo lang.
			case 0x46:	//Mode	//Menu   
				if(gc_Task_Current == C_Task_Play)
                { 
					if((gc_IRCmdStatus==1) && (gc_TuneVolFreqStatus <3))
					{	
                 		play_stop();
				    	gc_Task_Next=C_Task_Menu;
					}
				}
				else if(gc_Task_Current == C_Task_Jpeg)
				{
					if((gc_IRCmdStatus==1) && (gc_TuneVolFreqStatus <31))
					{
						jpeg_stop();
				    	gc_Task_Next=C_Task_Menu;
					}				
					
				}
				else if(gc_Task_Current == C_Task_Mjpeg)
				{
					if((gc_IRCmdStatus==1) && (gc_TuneVolFreqStatus <21))
					{
						mjpeg_stop();
				    	gc_Task_Next=C_Task_Menu;
					}
				}

				break;
#endif
/*
			case 0x19:	//100+
#if 1
				if(gc_Task_Current == C_Task_Play)
				{
				if(gc_IRCmdStatus==1)  //20090216 chiayen add
				{					
					if(gw_FileTotalNumber[0]!=0)
					{	
						U32 dw_File_StartCluster_Backup=0;
						U32 dw_FDB_StartCluster_Backup=0;
 
						gb_DirPlay_Flag=1;
				
						play_stop();
						gw_DirOverallNumber = gw_DirTotalNumber;
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

						gc_Dirchange_Timer=5;
						gw_LogData_Timer=2;
						gb_TFT_refresh=1;
						if(gb_LrcFileName_Exist==1)
						{
							gc_DispWallpaper=0;
						}
						gc_PhaseInx = 3;
						gb_DirPlay_Flag=1;
						gc_FolderPlay_Flag=1;
					}
				}
                                 gc_folder_disp=1; //sunzhk add
		
				}
#else
				if(gc_IRCmdStatus==1)
				{
					if(gc_Task_Current == C_Task_Play)
					{
                  		gc_ShowTimer=72;
						if(gc_DispPlayMenuAct==0)
						{
							gc_TuneVolFreqStatus=3;
						}
					}else if(gc_Task_Current == C_Task_Jpeg)
					{
						if(gc_DispPlayMenuAct==0)
						{
							gc_TuneVolFreqStatus=31;
						}				
					}else if(gc_Task_Current == C_Task_Mjpeg)
					{
						if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
						{
							mjpeg_playpause();
						}
		
		 		    	gc_ShowTimer=72;
						if((gc_DispPlayMenuAct==0) || (gc_DispPlayMenuAct==0x11))
						{
							gc_TuneVolFreqStatus=21;
						}					
					}
				}
#endif

				break;

	        case 0x0d:					//0x200
				if(gc_Task_Current == C_Task_Play)
				{
				if(gc_IRCmdStatus==1)  //20090216 chiayen add
	
				{
					if(gw_FileTotalNumber[0]!=0)
					{
						play_stop();
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

						gc_Dirchange_Timer=5;
						gb_TFT_refresh=1;
						if(gb_LrcFileName_Exist==1)
						{
							gc_DispWallpaper=0;
						}

						gb_FindFlag = 1;
						gc_PhaseInx = 3;
						gb_DirPlay_Flag=1;
						gc_FolderPlay_Flag=1;  //20090216 chiayen add				
					}				
				
                                
			}
                                        gc_folder_disp=1; //sunzhk add
			}
			break;
*/
	        case 0x1a:				//eq
				if(gc_Task_Current == C_Task_Play)
				{
					if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
					{
						if((gc_IRCmdStatus==1)&&(gw_FileTotalNumber[0]!=0))
						{
							Change_EQ();
						}
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
				if((gc_Task_Current == C_Task_Play) ||(gc_Task_Current == C_Task_Mjpeg))
				{
					VOL_UP();
				}
				break;
	        case 0x04:	// prev	
 				if(gc_TuneVolFreqStatus < 3)
				{
					if((gc_IRCmdStatus==1) && (gb_NumCHSet==0))
					{
						NextorPre_Song(1);
					}
				}

	 			break;
	        case 0x06:	// next	
						
				if(gc_TuneVolFreqStatus < 3)
				{
						if((gc_IRCmdStatus==1) && (gb_NumCHSet==0))
						{
							NextorPre_Song(0);
						}
				}
				break;
				case 0x02:	// CH+
				if((gc_Task_Current == C_Task_Play) ||(gc_Task_Current == C_Task_Mjpeg))
				{
					if(gb_NumSet==0)
					{
						ClearIRBuffer();
						CHG_UP();
					}
					else if(gb_NumSet==1)
					{
						ChannelSet_Process();
						gc_ShowIRNumMain=0;
						gb_ChannelSet=1;
						gb_NumSet=0;
						ClearIRBuffer();
					}
				}

				break;
			case 0x00:	// CH-
					if((gc_Task_Current == C_Task_Play) ||(gc_Task_Current == C_Task_Mjpeg))
				{
					if(gb_NumSet==0)
					{
						ClearIRBuffer();
						CHG_DN();
					}
					else if(gb_NumSet==1)
					{
						ChannelSet_Process();
						gc_ShowIRNumMain=0;
						gb_ChannelSet=1;
						gb_NumSet=0;
						ClearIRBuffer();
					}
				}
				
				break;
		/*	case 0x1d:	 //mute
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
				break;*/
			case 0x18:	  //u/sd
				if(gc_IRCmdStatus==1)  //for IRSourceChange
				{
					MediaChange();  //20090107 chiayen add
				}				
				break;
			case 0x0a:				//CH_set
				if(gc_IRCmdStatus==1)
				{
					if(gb_NumSet==1)
					{
						ChannelSet_Process();
						gc_ShowIRNumMain=0;
						gb_ChannelSet=1;
						gb_NumSet=0;
					ClearIRBuffer();
					}	
					else
					{
						ClearIRBuffer();
					}
					break;
				}
				break;
			case 0x08:				//PkSong
			if(gc_IRCmdStatus==1)
			{
				if(gb_NumSet==1)
                {
	              	gb_PickSongSet=1;
					gw_IR_Timer=1;
					gb_Frequency_Song=1;
	              	gb_NumSet=0;
                }
			  	else
			  	{
			  		ClearIRBuffer();
			  	}
			 }	
				break;
			case 0x19:	//     IR 有两个0键
			case 0x1e:	// 0
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
}

void SaveNumber(U8 number)
{
	if(gc_Task_Current != C_Task_Play)
	{return;}
    if(gc_IRCmdStatus==1)
    {
        gw_IR_Timer=120;

	 			
		gc_NUMBER[3]=gc_NUMBER[2];
		gc_NUMBER[2]=gc_NUMBER[1];
		gc_NUMBER[1]=gc_NUMBER[0];
		gc_NUMBER[0]=number;
		gb_ShowIRNum=1;
		gc_ShowIRNumMain=0;	
			if(gb_ShowIRNum==1)						//显示数字
			{
				gc_ShowTimer=0;
				gc_TuneVolFreqStatus=0;
			
				if(gc_ShowIRNumMain==0)
				{
					gc_ShowIRNumMain=1;
					gb_TFT_refresh=1;
					gc_DispWallpaper=0;
					TFT_IRNumMain();
				}
			
				TFT_IRNum();
			}
        gb_NumSet=1;
        gc_PhaseInx=1;
        
    }
}
void ClearIRBuffer(void)
{
	gb_ChannelSet=0;
	gb_PickSong=0;
	gc_NUMBER[0]=0xFF;
	gc_NUMBER[1]=0xFF;
	gc_NUMBER[2]=0xFF;
	gc_NUMBER[3]=0xFF;
	gb_ShowIRNum=0;
	gb_NumSet=0;
}


void Change_EQ(void)
{
//	gb_Err=0;
	ClearIRBuffer();
	gw_LogData_Timer=60; //chiayen0812
	gw_IR_Setting_Timer=120;
	gb_IR_Setting=1;
	gs_DSP_GLOBAL_RAM.sc_EQ_Type++;
	if(gs_DSP_GLOBAL_RAM.sc_EQ_Type>5)
	{
		gs_DSP_GLOBAL_RAM.sc_EQ_Type=0;	
	}
	Music_EQ_Cmd(gs_DSP_GLOBAL_RAM.sc_EQ_Type);
	gc_ShowTimer=/*120*/72;
	gb_Frequency_Song=1;	// 0=Show Frequency    1=Show Song Number/EQ
	gb_SelectEQ=1;			// 0=Show Song Number  1=Show EQ
}


void VOL_UP(void)
{
	if(gw_irkey_count!=0)
	{		
//		LCM_ClearScreen();  //20090309 chiayen mark
	}
//	gb_Err=0;	
	ClearIRBuffer();
	gw_IR_Setting_Timer=120;
	gb_IR_Setting=1;
	if(gc_Task_Current == C_Task_Play)
	{
		gc_PhaseInx=C_PlayVolUp;
	}else if(gc_Task_Current == C_Task_Mjpeg)
	{
		gc_PhaseInx=C_MjpegVolUp;
	}
}

void CHG_UP(void)
{
	if(gw_irkey_count!=0)
	{		
//		LCM_ClearScreen();  //20090309 chiayen mark
	}
//	gb_Err=0;	
	ClearIRBuffer();
	gw_IR_Setting_Timer=120;
	gb_IR_Setting=1;
	if(gc_Task_Current == C_Task_Play)
	{
		gc_PhaseInx=C_PlayChgUp;
	}
	else if(gc_Task_Current == C_Task_Mjpeg)
	{
		gc_PhaseInx=C_MjpegChgUp;
	}
}

void CHG_DN(void)
{
	if(gw_irkey_count!=0)
	{		
//		LCM_ClearScreen();  //20090309 chiayen mark
	}
//	gb_Err=0;	
	ClearIRBuffer();
	gw_IR_Setting_Timer=120;
	gb_IR_Setting=1;
	if(gc_Task_Current == C_Task_Play)
	{
		gc_PhaseInx=C_PlayChgDn;
	}
	else if(gc_Task_Current == C_Task_Mjpeg)
	{
		gc_PhaseInx=C_MjpegChgDn;
	}
}

void VOL_DOWN(void)
{
	if(gw_irkey_count!=0)
	{
//		LCM_ClearScreen();  //20090309 chiayen mark
	}
//	gb_Err=0;			
	ClearIRBuffer();
	gw_IR_Setting_Timer=120;
	gb_IR_Setting=1;
	if(gc_Task_Current == C_Task_Play)
	{
		gc_PhaseInx=C_PlayVolDn;//home
	}
	else if(gc_Task_Current == C_Task_Mjpeg)
	{
		gc_PhaseInx=C_MjpegVolDn;//home
	}
}


void NextorPre_Song(bit tb_state)
{
	gc_TuneVolFreqStatus=0;
	if(gb_LrcFileName_Exist==1)
	{
		gb_TFT_refresh=1;
		gc_DispWallpaper=0;
	}

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
	}
	else
	{
		if(tb_state==0)
		{
			if(gc_Task_Current == C_Task_Play)
			{
				play_next();		
			}else if(gc_Task_Current == C_Task_Jpeg)
			{
				jpeg_next();
			}else if(gc_Task_Current == C_Task_Mjpeg)
			{
				mjpeg_next();
			}
		}
		else
		{		
			if(gc_Task_Current == C_Task_Play)
			{
				play_prev();	
			}else if(gc_Task_Current == C_Task_Jpeg)
			{
				jpeg_prev();
			}else if(gc_Task_Current == C_Task_Mjpeg)
			{
				mjpeg_prev();
			}
		}
		ClearIRBuffer();
	}
}


void PickSong_Process(void)
{
	data	U16	tw_SongNumber=0;

	gb_PickSongSet=0;
	gw_DispSongNum=0xFFFF;

	if(gc_NUMBER[0]!=0xFF)
	{
		tw_SongNumber=gc_NUMBER[0];
		if(gc_NUMBER[1]!=0xFF)
		{
			tw_SongNumber+=gc_NUMBER[1]*10;
			if(gc_NUMBER[2]!=0xFF)
			{
				tw_SongNumber+=gc_NUMBER[2]*100;
				if(gc_NUMBER[3]!=0xFF)
				{
					tw_SongNumber+=gc_NUMBER[3]*1000;
				}
			}
		}
	}

	ClearIRBuffer();
	gb_NumSet=0;
	gc_ShowTimer=72;	
	gw_IR_Timer=0;


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
		gb_Frequency_Song=1;
	}
	else
	{
		if(tw_SongNumber==0)
		{
			gb_Frequency_Song=1;
		}
		else
		{
			TFT_ShowError();
			gb_Frequency_Song=0;
			gc_ShowIRNumMain=0;
			gc_ShowTimer=72;
			return;
		}
	}

	if ((gw_SongNumber!=gw_FileIndex[0])&&(gc_CurrentCard==5)&&(gw_FileTotalNumber[0]!=0))
	{
		gw_SongNumber=gw_FileIndex[0];
	}

	if ((gw_SongNumberSD!=gw_FileIndex[0])&&((gc_CurrentCard==3)||(gc_CurrentCard==2))&&(gw_FileTotalNumber[0]!=0))
	{
		gw_SongNumberSD=gw_FileIndex[0];
	}
}

void ChannelSet_Process(void)
{
	U16	tw_frequency;

	
	tw_frequency=gc_NUMBER[2]*100+gc_NUMBER[1]*10+gc_NUMBER[0];
	if(gc_NUMBER[3]!=0xff)
	{						
		tw_frequency=tw_frequency+gc_NUMBER[3]*1000;
	}

	if((tw_frequency>=875)&&(tw_frequency<=1080))
	{
		gw_FM_frequency=tw_frequency;
		FM_drive();
		gc_ShowTimer=5;
		gw_DisplayFreq=0xFFFF;
		gw_IR_Timer=0;
		gb_NumSet=0;
		gc_NUMBER[0]=0xFF;
		gc_NUMBER[1]=0xFF;
		gc_NUMBER[2]=0xFF;
		gc_NUMBER[3]=0xFF;
	}
	else
	{
		TFT_ShowError();
		gc_ShowTimer=72;

		gw_IR_Timer=0;
		gb_NumSet=0;
		gc_NUMBER[0]=0xFF;
		gc_NUMBER[1]=0xFF;
		gc_NUMBER[2]=0xFF;
		gc_NUMBER[3]=0xFF;
		return;
	}
//	TFT_Main_VOL_FM_ADJ();
//	TFT_ShowFMFreqAdj();
	gw_LogData_Timer=10;
}
#if 1
void MediaChange(void)  //20090107 chiayen add
{
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
}
#endif

