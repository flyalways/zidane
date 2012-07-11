#include "..\header\SPDA2K.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"

void MediaSelect(void)
{
	gb_ChangeMedia=0;
	if(gc_CurrentCard==2)
	{
		if(gb_SD_Exist==1)
		{
			gb_ChangeMedia=1;
		}
		else
		{
			if(gb_Host_Exist==1)
			{
				gc_CurrentCard=5;
				gb_ChangeMedia=1;
			}
			else
			{
				gc_CurrentCard=9;
			}
		}
	}
	else if(gc_CurrentCard==5)
	{
		if(gb_Host_Exist==1)
		{
			gb_ChangeMedia=1;
		}
		else
		{
			if(gb_SD_Exist==1)
			{
				gc_CurrentCard=2;
				gb_ChangeMedia=1;
			}
			else
			{
				gc_CurrentCard=9;
			}
		}
	}
}

bit MediaInitial(void)
{
	switch(gc_CurrentCard)
	{
		case 2: // SD
			if(SD_Identification_Flow()==1)
			{
				if(DOS_Initialize()==0)
				{
					return 0;	// Play SD
				}

				if(gb_Host_Exist)
				{
					if(Host_Initial()==0)
					{
						gc_CurrentCard=5;
						if(DOS_Initialize()==0)
						{
							return 0;
						}
					}
				}
				gc_CurrentCard=9;
				return 1;
			}
		break;

		case 5:	// Host
			if(Host_Initial()==0)
			{
				if(DOS_Initialize()==0)
				{
					return 0;
				}
			}

			if(gb_SD_Exist)
			{
				if(SD_Identification_Flow()==0)
				{
					gc_CurrentCard=2;
					if(DOS_Initialize()==0)
					{
						return 0;
					}
				}
			}
			gc_CurrentCard=9;
			return 1;
		break;
	}
		return 1;
}

bit SearchFolder_VOICE(void)
{
	xdata	U8	tc_Status;
	xdata	U8	tc_FileName[10]={0x00,0x00,0x01,0x00,0x0B,'V','O','I','C','E'};
	idata	SearchFdb ts_FDBEntry;

	ts_FDBEntry.c_Search_Mode=K_SPECIFIC_SHORT_FILENAME;//search free fdb entry mode 
	ts_FDBEntry.dw_FDB_StartCluster=gdw_DOS_RootDirClus;//set directory startcluster
	ts_FDBEntry.c_EXTSelect=1;
	ts_FDBEntry.c_Search_Attribute=1;
	ts_FDBEntry.pc_LongFileName=tc_FileName;
	ts_FDBEntry.c_type=0x02;
	EXT_NameC[0]=1;
	EXT_NameC[1]=0x20;
	EXT_NameC[2]=0x20;
    EXT_NameC[3]=0x20;
	tc_Status=Find_Fdb(&ts_FDBEntry);

	if(tc_Status)
	{	
		return 1;
	}
	else
	{
		gs_File_FCB[2].w_FDB_Offset=ts_FDBEntry.w_FDB_Offset;
		gs_File_FCB[2].dw_FDB_LogAdd=ts_FDBEntry.dw_FDB_LogAdd;
		DOS_Open_File_r(2,2,0);	// Get FDB's start cluster
		gs_System_State.c_FileHandle=1;
		gs_File_FCB[1].dw_FDB_StartCluster=gs_File_FCB[2].dw_File_StartCluster;
		DOS_Search_File(C_File_OneDir|C_Cnt_FileNo,C_RecordFileType,C_CmpExtName|C_Next);
		return 0;
	}
}

void WAV_Play_Init(void)
{
	data	U8	tc_SampleRate;

	WAV_Play_Download();	// Download DSP code
	L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sc_Volume);	// Set Volume
	gw_CurrentSec=0;
	gb_ZeroFileSize_Or_FormatError=0;
	tc_SampleRate=(U8)IMA_ADPCM_SampleRate_Parser();
	gs_System_State.w_SampleRate=tc_SampleRate;
	DOS_GetLongFileName(1,gc_FileLongName);
	#ifdef SPI_1M		  
	X_Unicode2ISN(gc_FileLongName);
	#endif
	LCM_Disp_FileName(&gc_FileLongName[5],gc_FileLongName[2],gc_FileLongName[4],1);

	gw_FileIndex[0]=gw_FileIndex[1];
	gw_FileTotalNumber[0]=gw_FileTotalNumber[1];
	RefreshAllDisplay();
	LCM_Disp_FileName(&gc_FileLongName[5],gc_FileLongName[2],gc_FileLongName[4],1);

	if(IMA_ADPCM_Play_PlayCmd(tc_SampleRate))
	{
		gb_ZeroFileSize_Or_FormatError=1;
	}
}

void L2_DSP_MCU_DM_DMA_16bit(U16 BufferIndex,U8 tc_Dest_Addr_High,U16 tw_Src)
{
	data	U16 tw_DSP_Word_Align;
   	data	U16 tw_DSPAddr;
	xdata	U8  tc_DSPAddr_High;

	// ----- DMA Reset -----
	XBYTE[0xB304] = 0x09;
	XBYTE[0xB304] = 0x08;
	XBYTE[0xB3C0] = 0x00;		// clear DMA complete
	XBYTE[0xB330] = 0x01;		// clear checksum

	// ----- DMA initial settings for DM transfer -----
	XBYTE[0xB216] = 0x00;	// Remain DSP mode.  When you do MCU -> DM, MCU will stop DSP automaticlly
	XBYTE[0xB301] = 0x50;	// DMA : SRAM --> DM
	XBYTE[0xB302] = 0xFF;	// Define data size in DMA (assume 512 bytes)
	XBYTE[0xB303] = 0x01;

	// Destination
	XBYTE[0xB340]=0x01;	// 16-bit DMA
	*((U8 *)&tw_DSPAddr)=((*((U8 *)&BufferIndex+1))<<1);
	*((U8 *)&tw_DSPAddr+1)=0;
	tw_DSP_Word_Align=tw_DSPAddr>>1;
	XBYTE[0xB21D]=0x01;	// offset
	tc_DSPAddr_High=*((U8 *)&tw_DSP_Word_Align);
	tc_DSPAddr_High+=tc_Dest_Addr_High;
	XBYTE[0xB217]=*((U8 *)&tw_DSP_Word_Align+1);
	XBYTE[0xB218]=tc_DSPAddr_High;
	// Source
	XBYTE[0xB112]=(tw_Src<<8)>>8;
	XBYTE[0xB113]=(tw_Src>>8);
	Trigger_Data_Transfer_Done();
}

U8 IMA_ADPCM_Play_DataIn(void)
{
	if((L2_DSP_Read_DMem16(DSP_EmptyBuffer)*2/3)>=512)
	{
		if(DOS_Read_File(1)==DOS_END_OF_FILE)
		{
			return DSP_DATAIN_COMMAND_ERROR;
		}

		L2_DSP_MCU_DM_DMA_16bit(gs_DSP_GLOBAL_RAM.sc_DM_Index++,0x20,(U16)gc_PlayRecordDataBuf);
		L2_DSP_SendCommandSet(DCMD_DatIn);
		if(gs_DSP_GLOBAL_RAM.sc_DM_Index>=24)
		{
			gs_DSP_GLOBAL_RAM.sc_DM_Index=0;
		}
	}
	return	0;
}

U8 IMA_ADPCM_Play_PlayCmd(U8 tc_SampleRate)
{
	data	U16	tw_CtrlMode;

	if(tc_SampleRate==16)
	{
		tw_CtrlMode=0x7010;
	}
	else if(tc_SampleRate==8)
	{
		tw_CtrlMode=0x7000;
	}
	else if(tc_SampleRate==32)	
	{
		tw_CtrlMode=0x7020;
	}
	else
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	L2_DSP_Write_DMem16(DSP_ADPCMModeControl,tw_CtrlMode);
	if(L2_DSP_SendCommandSet(DCMD_Play)!=DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
	gs_DSP_GLOBAL_RAM.sc_DM_Index=0;	// reset bitstream address of DM
	if(DOS_Open_File_r(C_RecordFHandle,C_Open_FoundFile,C_NullFileName))
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
	return 0;
}

U8 IMA_ADPCM_Play_PauseCmd(void)
{
	data	U16	TimeOUT=0xFFFF;

	if(L2_DSP_SendCommandSet(DCMD_Pause)!=DCMD_Pause)
	{
		return DSP_PAUSE_COMMAND_ERROR;
	}

	do
	{
		TimeOUT--;
		if (TimeOUT == 0)
		{
			return DSP_RUNNING_STATUS1_ERROR;
		}
	} while(!(L2_DSP_Read_DMem16(DSP_RunningStatus)&0x0002));

	return 0;
}

U8 IMA_ADPCM_Play_ResumeCmd(void)
{
	if(L2_DSP_SendCommandSet(DCMD_Play)!=DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
	return 0;
}

U8 IMA_ADPCM_Play_StopCmd(void)
{
	data	U8  tc_Ret;
	data	U16 TimeOUT=0xFFFF;
	xdata	U16	tc_temp;

	tc_Ret=DSP_StopCmd();

	do{
		TimeOUT --;
		if(TimeOUT==0)
		{
			tc_Ret=DSP_DECODE_STATUS_TIMEOUT_ERROR;
			break;
		}
	}while(!((L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)==0x00C0));  //wait file end

	tc_temp=L2_DSP_Read_DMem16(DSP_DecodeStatus);
	L2_DSP_Write_DMem16(DSP_DecodeStatus,tc_temp&0xFF3F);
	gs_File_FCB[1].dw_File_StartCluster=0xFFFFFFFF;
	gb_PlayOtherWAV=1;
	return tc_Ret;
}

U16 IMA_ADPCM_Play_ReadTime(void)
{
	data	U8	tc_status;
	xdata	U16 tw_DurTime;
	xdata	U32 tdw_FrameNum;

	tc_status=L2_DSP_Read_DMem16(DSP_ADPCMModeControl);
	tdw_FrameNum=L2_DSP_Read_DMem24(DSP_DecodeFrameCounter);
	tw_DurTime=(tdw_FrameNum*505)/1000;

	if(tc_status&0x20) // 32K sample rate 
	{
		tw_DurTime=tw_DurTime/32;
	}
	else if(tc_status&0x10) // 16K sample rate 
	{
		tw_DurTime=tw_DurTime/16;
	}
	else  // 8K samplpe rate
	{
		tw_DurTime=tw_DurTime/8;
	}

	return tw_DurTime+gw_CurrentSec;
}

U16 IMA_ADPCM_SampleRate_Parser(void)//(JC)H0723 Parsing the ADPCM file header to get sample rate
{
	data	U8	tc_temp;
	data	U16 tw_SampleRate;	
	
	if(DOS_Open_File_r(C_RecordFHandle,C_Open_FoundFile,C_NullFileName)||(gs_File_FCB[1].dw_File_TotalSize==0))
	{
		return 1;//(JC)file open fail 
	}
	DOS_Read_File(C_RecordFHandle);
	if(gc_PlayRecordDataBuf[0x00]!=0x52||gc_PlayRecordDataBuf[0x01]!=0x49 		//check "RIFF" tag
	|| gc_PlayRecordDataBuf[0x02]!=0x46||gc_PlayRecordDataBuf[0x03]!=0x46)
	{
		return 2;//Jimi 100408, Format Error
	}

	if(gc_PlayRecordDataBuf[0x08]!=0x57||gc_PlayRecordDataBuf[0x09]!=0x41 		//check "WAVE" tag
	|| gc_PlayRecordDataBuf[0x0A]!=0x56||gc_PlayRecordDataBuf[0x0B]!=0x45)
	{
		return 3;//Jimi 100408, Format Error
	}

	if(gc_PlayRecordDataBuf[0x10]!=0x14||gc_PlayRecordDataBuf[0x11]!=0x00 		//check format chunk size
	|| gc_PlayRecordDataBuf[0x12]!=0x00||gc_PlayRecordDataBuf[0x13]!=0x00)
	{
		return 4;//Jimi 100408, Format Error
	}

	if(gc_PlayRecordDataBuf[0x14]!=0x11||gc_PlayRecordDataBuf[0x15]!=0x00)		//check iFormatTag
	{
		return 5;//Jimi 100408, Format Error
	}

	gc_WaveChannelNumber=gc_PlayRecordDataBuf[0x16];
	tw_SampleRate=gc_PlayRecordDataBuf[0x19];
	tw_SampleRate=(tw_SampleRate<<8)+gc_PlayRecordDataBuf[0x18];
	tw_SampleRate/=1000;

	gs_File_FCB[1].dw_File_StartCluster=0xFFFFFFFF;
	gs_File_FCB[1].dw_File_CurrentCluster=gs_File_FCB[1].dw_File_StartCluster;
	gs_File_FCB[1].dw_File_DataPoint=0;

	tc_temp=tw_SampleRate;
	gw_TotalSec=((gs_File_FCB[1].dw_File_TotalSize/512*505)/4000/gc_WaveChannelNumber);
	gw_TotalSec=gw_TotalSec/(tc_temp/8);

	return tw_SampleRate;
}

void AccessKeyEvent_WAV(void)
{
	data	U8	tc_Step;
	xdata	U32	tdw_DataPoint;

	switch(gs_System_State.c_Phase)
	{
		case TASK_PHASE_PLAYACT:
			switch(gc_KeyEvent)
			{
				case 0x02:	// Play
					if(gc_WAVMenuItem==0)
					{
						IMA_ADPCM_Play_PauseCmd();
						gs_System_State.c_Phase=TASK_PHASE_PAUSE;
						LCM_ShowPlayPauseIcon();
						ClearIRNumberVariable();
					}
					else
					{
					}
				break;

				case 0x03:	// Next
					if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_A)
					{
						// 記憶A point
						gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_B;
						//gs_DSP_GLOBAL_RAM.sdw_File_ACluster=gs_File_FCB[1].dw_File_CurrentCluster;
						gs_DSP_GLOBAL_RAM.sdw_File_ACluster = 0xFFFFFFFF;
						tdw_DataPoint=L2_DSP_Read_DMem16(DSP_RemainBuffer)*2/3;		//jimiyu 081117
						tdw_DataPoint&=0xFFFFFE00;
						gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint=(gs_File_FCB[1].dw_File_DataPoint)-(tdw_DataPoint);
						gw_AFrameCnt=L2_DSP_Read_DMem16(DSP_DecodeFrameCounter);		//Jimi 080530, to get A point frame count
					}
					else if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_B)
					{
						// 記憶B point & 開始Repeat
						gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB;
						gs_DSP_GLOBAL_RAM.sdw_File_BCluster=gs_File_FCB[1].dw_File_CurrentCluster;
						gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint=gs_File_FCB[1].dw_File_DataPoint;
					}
					else if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_AB)
					{
						gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_A;
					}
					else if(gc_WAVMenuItem==0)
					{
						ClearIRNumberVariable();
						if(gb_SetVol)
						{
							gb_ADJ=0;
							VOL_Adj();
						}
						else
						{
							IMA_ADPCM_Play_StopCmd();
							gb_PlayNewWave=1;
							DOS_Search_File(C_File_OneDir|C_By_Name,C_RecordFileType,C_CmpExtName|C_Next);
						}
					}
					else if(gc_WAVMenuItem==1)	// WAV play Menu(EQ/A-B/Exit)
					{
						tc_Step=gc_WAVPlayMenuNum;
						gc_WAVPlayMenuNum=(gc_WAVPlayMenuNum+1)%3;
						LCM_ShowWAVPlayMenu(tc_Step);
						LCM_ShowWAVPlayMenu(gc_WAVPlayMenuNum);
					}
					else if(gc_WAVMenuItem==2)	// EQ Menu
					{
						if(gs_DSP_GLOBAL_RAM.sc_EQ_Type==3)
						{
							gs_DSP_GLOBAL_RAM.sc_EQ_Type=4;
							LCM_ShowEQMenu(4);
							LCM_ShowEQMenu(5);
							LCM_erase_one_page(4);
							LCM_erase_one_page(5);
							LCM_erase_one_page(6);
							LCM_erase_one_page(7);
						}
						else if(gs_DSP_GLOBAL_RAM.sc_EQ_Type==5)
						{
							gs_DSP_GLOBAL_RAM.sc_EQ_Type=0;
							LCM_ShowEQMenu(0);
							LCM_ShowEQMenu(1);
							LCM_ShowEQMenu(2);
							LCM_ShowEQMenu(3);
						}
						else
						{
							gs_DSP_GLOBAL_RAM.sc_EQ_Type++;
							LCM_ShowEQMenu(gs_DSP_GLOBAL_RAM.sc_EQ_Type-1);
							LCM_ShowEQMenu(gs_DSP_GLOBAL_RAM.sc_EQ_Type);
						}
						Music_EQ_Cmd();
					}
				break;

				case 0x04:	// Prev
					if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_A)
					{
					}
					else if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_B)
					{
						gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_A;
					}
					else if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_AB)
					{
						gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_A;
					}
					else if(gc_WAVMenuItem==0)
					{
						ClearIRNumberVariable();
						if(gb_SetVol)
						{
							gb_ADJ=1;
							VOL_Adj();
						}
						else
						{
							IMA_ADPCM_Play_StopCmd();
							gb_PlayNewWave=1;
							DOS_Search_File(C_File_OneDir|C_By_Name,C_RecordFileType,C_CmpExtName|C_Prev);
						}
					}
					else if(gc_WAVMenuItem==1)	// WAV play Menu(EQ/A-B/Exit)
					{
						tc_Step=gc_WAVPlayMenuNum;
						gc_WAVPlayMenuNum=(gc_WAVPlayMenuNum+2)%3;
						LCM_ShowWAVPlayMenu(tc_Step);
						LCM_ShowWAVPlayMenu(gc_WAVPlayMenuNum);							
					}
					else if(gc_WAVMenuItem==2)
					{
						if(gs_DSP_GLOBAL_RAM.sc_EQ_Type==0)
						{
							gs_DSP_GLOBAL_RAM.sc_EQ_Type=5;
							LCM_ShowEQMenu(4);
							LCM_ShowEQMenu(5);
							LCM_erase_one_page(4);
							LCM_erase_one_page(5);
							LCM_erase_one_page(6);
							LCM_erase_one_page(7);
						}
						else if(gs_DSP_GLOBAL_RAM.sc_EQ_Type==4)
						{
							gs_DSP_GLOBAL_RAM.sc_EQ_Type=3;
							LCM_ShowEQMenu(0);
							LCM_ShowEQMenu(1);
							LCM_ShowEQMenu(2);
							LCM_ShowEQMenu(3);
						}
						else
						{
							gs_DSP_GLOBAL_RAM.sc_EQ_Type--;
							LCM_ShowEQMenu(gs_DSP_GLOBAL_RAM.sc_EQ_Type+1);
							LCM_ShowEQMenu(gs_DSP_GLOBAL_RAM.sc_EQ_Type);
						}
						Music_EQ_Cmd();
					}
				break;

				case 0x05:	// V
					if(gc_WAVMenuItem==0&&gb_SetVol==0)
					{
						gb_SetVol=1;
						gc_MenuTimer=6;
						LCM_Clear_L2_L5();
						LCM_ShowVOL();
						ClearIRNumberVariable();
					}
					else
					{
						gc_MenuTimer=0;
					}
				break;

				case 0x06:	// M
					if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_AB_NULL)
					{
						gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB_NULL;
						LCM_ShowRepeatIcon();	// Clear Repeat Icon
						LCM_ShowSongNum();
						LCM_ShowSongTotalNum();
					}
					else if(gc_WAVMenuItem==0)
					{
						// Enter WAV Play Menu
						gc_WAVMenuItem=1;
						gc_MenuTimer=6;
						ClearIRNumberVariable();
						LCM_ShowWAVPlayMenu(0);
						LCM_ShowWAVPlayMenu(1);
						LCM_ShowWAVPlayMenu(2);
						LCM_erase_one_page(6);
						LCM_erase_one_page(7);
					}
					else if(gc_WAVMenuItem==1)	// ** Enter WAV Play Sub-Menu **
					{
						if(gc_WAVPlayMenuNum==0)	// EQ menu
						{
							gc_WAVMenuItem=2;
							if(gs_DSP_GLOBAL_RAM.sc_EQ_Type<4)
							{
								LCM_ShowEQMenu(0);
								LCM_ShowEQMenu(1);
								LCM_ShowEQMenu(2);
								LCM_ShowEQMenu(3);
							}
							else
							{
								LCM_ShowEQMenu(4);
								LCM_ShowEQMenu(5);
								LCM_erase_one_page(4);
								LCM_erase_one_page(5);
								LCM_erase_one_page(6);
								LCM_erase_one_page(7);
							}
						}
						else if(gc_WAVPlayMenuNum==1)	// A-B
						{
							gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_A;
							gc_MenuTimer=0;
						}
						else	// Exit
						{
							gc_MenuTimer=0;
						}
					}
					else if(gc_WAVMenuItem==2)	// Confirm EQ
					{
						gc_MenuTimer=0;
						LCM_ShowEQIcon();
					}
					else	// Exit
					{
						gc_MenuTimer=0;
					}
				break;

				case 0x12:	// L-Play
					gc_Task_Current=C_Task_Suspend;
				break;

				case 0x13:	// L-Next
					gb_ADJ=0;
					WAVE_FF_FR();
				break;

				case 0x14:	// L-Prev
					gb_ADJ=1;
					WAVE_FF_FR();
				break;

				case 0x16:	// L-M
					gc_Task_Current=C_Task_Idle;
					gc_LastCard=gc_CurrentCard;
				break;
			}		
		break;

		case TASK_PHASE_FASTFWD:
		case TASK_PHASE_FASTREV:
			switch(gc_KeyEvent)
			{
				case 0x13:	// L-Next
				case 0x14:	// L-Prev
					WAVE_FF_FR();
				break;

				case 0x23:	// R-Next
				case 0x24:	// R-Prev
					IMA_ADPCM_seek_trig();
					gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
				break;
			}
		break;

		case TASK_PHASE_PAUSE:
		case TASK_PHASE_STOP:
			switch(gc_KeyEvent)
			{
				case 0x02://C_PlayPause:
					IMA_ADPCM_Play_ResumeCmd();
					gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
					LCM_ShowPlayPauseIcon();
					gc_ShowTimer=0;
				break;

				case 0x03:	// Next
					if(gc_WAVMenuItem==0)	// 1=WAV-Menu  2=EQ-Mode
					{
						ClearIRNumberVariable();
						if(gb_SetVol)
						{
							gb_ADJ=0;
							VOL_Adj();
						}
						else
						{
							if(gs_System_State.c_Phase==TASK_PHASE_PAUSE)
							{
								IMA_ADPCM_Play_StopCmd();
								gs_System_State.c_Phase=TASK_PHASE_STOP;
							}
							gb_PlayNewWave=1;
							DOS_Search_File(C_File_OneDir|C_By_Name,C_RecordFileType,C_CmpExtName|C_Next);
						}
					}
				break;

				case 0x04:	// Prev
					if(gc_WAVMenuItem==0)	// 1=WAV-Menu  2=EQ-Mode
					{
						ClearIRNumberVariable();
						if(gb_SetVol)
						{
							gb_ADJ=1;
							VOL_Adj();
						}
						else
						{		
							if(gs_System_State.c_Phase==TASK_PHASE_PAUSE)
							{
								IMA_ADPCM_Play_StopCmd();
								gs_System_State.c_Phase=TASK_PHASE_STOP;
							}
							gb_PlayNewWave=1;
							DOS_Search_File(C_File_OneDir|C_By_Name,C_RecordFileType,C_CmpExtName|C_Prev);
						}
					}
				break;

				case 0x05:	// V
					if(gc_WAVMenuItem==0)
					{
						gb_SetVol=1;
						gc_MenuTimer=6;
						LCM_Clear_L2_L5();
						LCM_ShowVOL();
						ClearIRNumberVariable();
					}
					else
					{
						gc_MenuTimer=0;
					}
				break;

				case 0x12:	// L-Play
					gc_Task_Current=C_Task_Suspend;
				break;

				case 0x16:	// L-M
					gc_Task_Current=C_Task_Idle;
					gc_LastCard=gc_CurrentCard;
				break;
			}
		break;
	}
}

void WAV_SourceDetect_Process(void)
{
	switch(gc_CurrentCard)
	{
		case 2:	// SD
			if(gb_SD_pin==0)	// SD Present
			{
				if(gb_Host_Exist==0)
				{
					if(Host_DetectDevice()==0)	// Host Insert, Change to Host
					{
						IMA_ADPCM_Play_StopCmd();
						gb_Host_Exist=1;
						gc_CurrentCard=5;
						gb_ChangeMedia=1;
					}
				}
				else
				{
					if(Host_DetectDevice()==1)
					{
						gb_Host_Exist=0;
						gb_SDNoFileflag=0;
					}
				}
			}
			else				// SD Remove
			{
				IMA_ADPCM_Play_StopCmd();
				gb_SD_Exist=0;
				gb_SDNoFileflag=0;
				if(gb_Host_Exist==1)
				{
					gc_CurrentCard=5;
					gb_ChangeMedia=1;
				}
				else
				{
					gc_CurrentCard=9;
				}
			}
		break;

		case 5:	// Host
			if(Host_DetectDevice()==0)	// Host Present
			{
				if(gb_SD_Exist==0)
				{
					if(gb_SD_pin==0)	// SD Insert, Change to SD
					{	
						IMA_ADPCM_Play_StopCmd();
						gb_SD_Exist=1;
						gc_CurrentCard=2;
						gb_ChangeMedia=1;	
					}
				}
				else
				{
					if(gb_SD_pin==1)
					{
						gb_SD_Exist=0;
						gb_SDNoFileflag=0;
					}
				}
			}
			else				// Host Remove
			{
				IMA_ADPCM_Play_StopCmd();
				gb_Host_Exist=0;
				gb_HostNoFileflag=0;
				if(gb_SD_Exist==1)
				{
					gc_CurrentCard=2;
					gb_ChangeMedia=1;
				}
				else
				{
					gc_CurrentCard=9;
				}
			}
		break;
	}
}

void WAVE_FF_FR(void)
{
	gc_LongKeyCount=30;
	if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
	{  
	    if(gb_ADJ==0)
	    {// play_fastfwd
		    gs_System_State.c_Phase=TASK_PHASE_FASTFWD;
	    }
	    else
	    {// play_fastrev
		    gs_System_State.c_Phase=TASK_PHASE_FASTREV;
	    }

		gw_CurrentSec=IMA_ADPCM_Play_ReadTime();
		IMA_ADPCM_Play_FF_FR_Cmd();
	}
	else if(gs_System_State.c_Phase==TASK_PHASE_FASTFWD)
	{	
		if(gw_CurrentSec<gw_TotalSec)
		{
			gw_CurrentSec+=1;
		}
	}
	else if(gs_System_State.c_Phase==TASK_PHASE_FASTREV)
	{	
		if(gw_CurrentSec!=0)
		{
			gw_CurrentSec-=1;
		}
	}
	LCM_ShowPlayTime(gw_CurrentSec);
}

U8 IMA_ADPCM_Play_FF_FR_Cmd(void)
{
	data	U8	tc_Ret;
	data	U16	TimeOUT;
	xdata	U16 temp;

	tc_Ret=DSP_PauseCmd();
	TimeOUT=0xFFFF;
	do
	{
		TimeOUT --;
		if(TimeOUT==0)
		{
			return DSP_RUNNING_STATUS1_ERROR;
		}
	} while(!(L2_DSP_Read_DMem16(DSP_RunningStatus)&0x0002));

	temp=L2_DSP_Read_DMem16(DSP_DecodeStatus);
	L2_DSP_Write_DMem16(DSP_DecodeStatus,temp&0xFF3F);
	L2_DSP_Write_DMem16(DSP_RemainBuffer,0);				//Jimi 081127, ycc081204
	L2_DSP_Write_DMem16(DSP_EmptyBuffer,0x3000);			//Jimi 081127, ycc081204
	gs_DSP_GLOBAL_RAM.sc_DM_Index=0;						//Jimi 081127, ycc081204

	return tc_Ret;
}

U8 IMA_ADPCM_seek_trig(void)
{
	xdata	U32	tdw_datapoint;

	DOS_Open_File_r(C_RecordFHandle,C_Open_FoundFile,C_NullFileName);
	gs_File_FCB[1].dw_File_CurrentCluster=gs_File_FCB[1].dw_File_StartCluster;
	tdw_datapoint=gw_CurrentSec*((gs_File_FCB[1].dw_File_TotalSize-60)/gw_TotalSec)+60;
	gs_File_FCB[1].dw_File_CurrentCluster=DOS_Seek_File(1,tdw_datapoint>>9);

	//Jimi 110111
	L2_DSP_Write_DMem16(DSP_DecodeFrameCounter,0);

	while((L2_DSP_Read_DMem16(DSP_EmptyBuffer)*2/3)>=ONE_SECTOR)			// jimiyu 081117
	{
		if(DOS_Read_File(1)==DOS_END_OF_FILE)
		{
			return DSP_DATAIN_COMMAND_ERROR;
		}

		L2_DSP_MCU_DM_DMA_16bit(gs_DSP_GLOBAL_RAM.sc_DM_Index++,0x20,(U16)gc_PlayRecordDataBuf);
		L2_DSP_SendCommandSet(DCMD_DatIn);
		if(gs_DSP_GLOBAL_RAM.sc_DM_Index>=24)
		{
			gs_DSP_GLOBAL_RAM.sc_DM_Index=0;
		}
	}

	L2_DSP_Write_DMem16(0x3F55,L2_DSP_Read_DMem16(0x3F55)&~0x0508); //clear bit[10],bit[8],bit[3]
	L2_DSP_Write_DMem16(0x3F57,0);
	L2_DSP_Write_DMem16(0x3F50,0x201E);
	L2_DSP_Write_DMem16(0x3F21,L2_DSP_Read_DMem16(0x3F21)-90);
	L2_DSP_Write_DMem16(0x3F22,L2_DSP_Read_DMem16(0x3F22)+90);

	if(L2_DSP_SendCommandSet(DCMD_Play)!=DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
	return 0;
}

U8 IMA_ADPCM_EOF_Proc(void)
{						
	data	U16 tw_BufRem;
	xdata	U16 tw_RampDownBufSz;

	tw_BufRem=L2_DSP_Read_DMem16(DSP_RemainBuffer)*2/3;			// jimiyu 081117
	tw_RampDownBufSz=(U16)(((U32)gs_System_State.w_SampleRate*1000)/8/2);	// 0.5 sec for ramp down

	if(tw_RampDownBufSz<512 )		//for 8k bitrate condition(M2L3 & M2.5L3)
	{
		tw_RampDownBufSz=512;
	}
	
	if((tw_BufRem<tw_RampDownBufSz)||(gs_File_FCB[1].dw_File_TotalSize==0)) //Ching 090116
	{
		return 1;
	}
	
	return 0;
}

U8 IMA_ADPCM_Repeat_DataIn(void)
{
	xdata	U16	temp;
	data	U16	TimeOUT=0xFFFF;

	while((L2_DSP_Read_DMem16(DSP_EmptyBuffer)*2/3)>=ONE_SECTOR)
	{
		if(DOS_Read_File(1)==DOS_END_OF_FILE)
		{
			return DSP_DATAIN_COMMAND_ERROR;
		}

		L2_DSP_MCU_DM_DMA_16bit(gs_DSP_GLOBAL_RAM.sc_DM_Index++,0x20,(U16)gc_PlayRecordDataBuf);
		L2_DSP_SendCommandSet(DCMD_DatIn);
		if(gs_DSP_GLOBAL_RAM.sc_DM_Index>=24)
		{
			gs_DSP_GLOBAL_RAM.sc_DM_Index=0;
		}
	}


	if(L2_DSP_SendCommandSet(DCMD_Pause)!=DCMD_Pause)
	{
		return DSP_PAUSE_COMMAND_ERROR;
	}

	do
	{
		TimeOUT--;
		if (TimeOUT == 0)
		{
			return DSP_RUNNING_STATUS1_ERROR;
		}
	} while(!(L2_DSP_Read_DMem16(DSP_RunningStatus)&0x0002));

	temp=L2_DSP_Read_DMem16(DSP_DecodeStatus);
	L2_DSP_Write_DMem16(DSP_DecodeStatus,temp&0xFF3F);

	L2_DSP_Write_DMem16(DSP_RemainBuffer,0);
	L2_DSP_Write_DMem16(DSP_EmptyBuffer,0x3000);
	L2_DSP_Write_DMem16(DSP_DecodeFrameCounter,gw_AFrameCnt);
	gs_DSP_GLOBAL_RAM.sc_DM_Index=0;

	if(gs_DSP_GLOBAL_RAM.sdw_File_ACluster==0xFFFFFFFF)
	{	// recalculate cluster location
		gs_File_FCB[1].dw_File_DataPoint=0;
		gs_File_FCB[1].dw_File_CurrentCluster=gs_File_FCB[1].dw_File_StartCluster;
		gs_DSP_GLOBAL_RAM.sdw_File_ACluster=DOS_Seek_File(1,gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint>>9);
	}
	gs_File_FCB[1].dw_File_CurrentCluster=gs_DSP_GLOBAL_RAM.sdw_File_ACluster;
	gs_File_FCB[1].dw_File_DataPoint=gs_DSP_GLOBAL_RAM.sdw_File_ADataPoint;

	while((L2_DSP_Read_DMem16(DSP_EmptyBuffer)*2/3)>=ONE_SECTOR)
	{
		if(DOS_Read_File(1)==DOS_END_OF_FILE)
		{
			return DSP_DATAIN_COMMAND_ERROR;
		}

		L2_DSP_MCU_DM_DMA_16bit(gs_DSP_GLOBAL_RAM.sc_DM_Index++,0x20,(U16)gc_PlayRecordDataBuf);
		L2_DSP_SendCommandSet(DCMD_DatIn);
		if(gs_DSP_GLOBAL_RAM.sc_DM_Index>=24)
		{
			gs_DSP_GLOBAL_RAM.sc_DM_Index=0;
		}
	}

	L2_DSP_Write_DMem16(0x3F55,L2_DSP_Read_DMem16(0x3F55)&~0x0508); //clear bit[10],bit[8],bit[3]
	L2_DSP_Write_DMem16(0x3F57,0);
	L2_DSP_Write_DMem16(0x3F50,0x201E);
	L2_DSP_Write_DMem16(DSP_RemainBuffer,L2_DSP_Read_DMem16(DSP_RemainBuffer)-90);
	L2_DSP_Write_DMem16(DSP_EmptyBuffer,L2_DSP_Read_DMem16(DSP_EmptyBuffer)+90);

	if(L2_DSP_SendCommandSet(DCMD_Play)!=DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	return	0;
}

