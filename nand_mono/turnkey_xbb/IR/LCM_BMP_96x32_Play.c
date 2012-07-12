extern xdata	U8	gc_Err;
extern data bit gb_PickSongSet;
extern data bit gb_PickSong;
extern data bit gb_ChannelSet; 
extern xdata U8 gc_NextorPreFlag;
extern xdata U8 gc_SelectEQ;
extern xdata U8 gc_SelectVol;
extern data bit gb_Frequency_Song;
extern data	U8	gc_LCM_Media;
extern data	U8	gc_DisplayEQIcon;
extern data	U8	gc_DisplayPlayMode;
extern data	U8	gc_DisplayEQ;
extern data	U8	gw_DisplayVol;
extern xdata U8 	gc_CurrentCard_backup;
extern data	U8	gc_DisplaySongnameCount;
extern data	U8	gc_LCM_line;
extern xdata	U16	gw_DisplayFreq1;	
extern xdata	U16 gw_DispFileName_ByteOffset;
extern xdata	U16	gw_DisplayBitrateTime;
extern xdata	U16	gw_DisplayBitRate;
extern xdata	U16	gw_DisplayTime;
extern xdata	U16	gw_DisplayFreq;
extern xdata	U16	gw_DispSongNum;
extern xdata	U16	gw_DispSongNum1;
extern data U16 gw_LogData_Timer;
extern data	U16	gw_LCMScrollTimer;
extern data U8 gc_ShowTimer;
extern data U16 gw_FM_frequency;

extern data System_Struct gs_System_State;

extern xdata UBYTE gc_RepPlayMode;
extern xdata UBYTE  gcPlay_FileType;

bit	gb_FlashNoFileflag=0; 
xdata U8 gc_LRCLCMClear_Flag=0;  //20090513 chiayen add
xdata U8 gc_LRCDispSongName_flag=0;

void LCM_ShowChar8x16_1(U8 Num,U8 tc_column);

code	U8	Icon_NUM_8x16_1[]=
{
	0x00,0xC0,0x20,0x10,0x10,0x20,0xC0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,	// 0
	0x00,0x20,0x20,0xF0,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,	// 1
	0x00,0x60,0x10,0x10,0x10,0x10,0xE0,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,	// 2
	0x00,0x60,0x10,0x10,0x10,0x90,0x60,0x00,0x00,0x18,0x20,0x21,0x21,0x22,0x1C,0x00,	// 3
	0x00,0x00,0x80,0x40,0xF0,0x00,0x00,0x00,0x00,0x07,0x04,0x24,0x3F,0x24,0x24,0x00,	// 4
	0x00,0xF0,0x10,0x90,0x90,0x90,0x10,0x00,0x00,0x19,0x21,0x20,0x20,0x20,0x1F,0x00,	// 5
	0x00,0xC0,0x20,0x10,0x10,0x30,0x00,0x00,0x00,0x1F,0x22,0x21,0x21,0x21,0x1E,0x00,	// 6
	0x00,0x70,0x10,0x10,0x90,0x70,0x10,0x00,0x00,0x00,0x00,0x3E,0x01,0x00,0x00,0x00,	// 7
	0x00,0xE0,0x10,0x10,0x10,0x10,0xE0,0x00,0x00,0x1C,0x23,0x21,0x21,0x23,0x1C,0x00,	// 8
	0x00,0xE0,0x10,0x10,0x10,0x20,0xC0,0x00,0x00,0x01,0x32,0x22,0x22,0x11,0x0F,0x00,	// 9
	0x00,0x60,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00,0x00,0x00,0x00,0x00,	// :
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,	// .
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// space
};


void LCM_Display(void)
{
	U16	tw_DisplayTime;

	if(((gcPlay_FileType==1)&&(gs_System_State.c_Phase == TASK_PHASE_PLAYACT))&&(((gb_PickSong==1)||(gb_ChannelSet==1)) || gc_CurrentCard!=5))
	{
		WMA_DataIn();
	}
	   
	if(((gw_ShowTimer==0) && (gc_LRCLCMClear_Flag==1)) && gc_LrcFileName_Exist && (gb_PickSong==0) && (gb_ChannelSet==0))  //20090513 chiayen add
	{
		LCM_ClearScreen();
		gc_LRCLCMClear_Flag=0;
	}
	// Show "ERROR"
	if(gc_Err==1)
	{
		if(gc_LCM_Media!=9)
		{
			gc_LCM_Media=9;
			LCM_ShowERROR();
		}
		gw_DisplayTime=0xFFFF;
		gw_DisplayFreq1=0xFFFF;
		gc_DisplayEQIcon=0xFF;
		gc_LRCLCMClear_Flag=1;
		return;
	}
	
	if((gb_ChannelSet==1)||(gb_PickSong==1))
	{
		if((gc_LCM_Media!=10)&&(gb_ChannelSet==1))
		{
			gc_LCM_Media=10;
			LCM_ClearScreen();
		}
		if((gc_LCM_Media!=11)&&(gb_PickSong==1))
		{
			gc_LCM_Media=11;
			LCM_ClearScreen();
		}
		LCM_ShowIRNum();
		gw_DisplayTime=0xFFFF;
		gw_DisplayFreq1=0xFFFF;
		gc_DisplayEQIcon=0xFF;
//		gc_LRCLCMClear_Flag=1;
		return;
	}

	if(1)
	{
		// Show "PAUSE"
		if(gs_System_State.c_Phase == TASK_PHASE_PAUSE)
		{			
			if(gc_LCM_Media!=3)	// 3=Pause
			{
				gc_LCM_Media=3;
				LCM_ShowPAUSE();
				gw_DisplayTime=0xFFFF;
				gw_DisplayFreq1=0xFFFF;
				gc_DisplayEQIcon=0xFF;
//				gc_LRCLCMClear_Flag=1;   //20090107 chiayen mark
			}
			return;
		}

		// Show play mode icon
		if((gw_ShowTimer!=0)&&(gc_DisplayPlayMode!=gc_RepPlayMode))
		{
			if(gc_LCM_Media!=7)	// 7=Play mode
			{
				gc_LCM_Media=7;
				LCM_ClearScreen();
			}
			gc_DisplayPlayMode=gc_RepPlayMode;
			LCM_ShowPlayMode();
			gw_DisplayTime=0xFFFF;
			gw_DisplayFreq1=0xFFFF;
			gc_DisplayEQIcon=0xFF;
			gw_DisplayFreq=gw_FM_frequency;
			gc_LRCLCMClear_Flag=1;
			return;
		}

		// Show EQ
		if((gw_ShowTimer!=0)&&(gc_SelectEQ==1))
		{
			gc_LCM_Media=8;		// 8=EQ

			if(gc_DisplayEQ!=gs_DSP_GLOBAL_RAM.sc_EQ_Type)
			{
				gc_DisplayEQ=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
				LCM_ClearScreen();
				LCM_ShowEQ();
				gw_DisplayTime=0xFFFF;
				gw_DisplayFreq1=0xFFFF;
				gc_DisplayEQIcon=0xFF;
				gc_LRCLCMClear_Flag=1;
			}
			return;
		}

		// Change Frequency
		if((gb_Frequency_Song==0)&&(gw_ShowTimer!=0))
		{
			if(gw_DisplayFreq!=gw_FM_frequency)
			{
				if(gc_LCM_Media!=6)
				{
					gc_LCM_Media=6;	// 6=Change Frequency
					LCM_ClearScreen();
				}
				gw_DisplayFreq=gw_FM_frequency;
				LCM_ShowFrequency_L();
				gw_DisplayTime=0xFFFF;
				gw_DisplayFreq1=0xFFFF;
				gc_DisplayEQIcon=0xFF;
				gc_LRCLCMClear_Flag=1;
			}
			return;
		}		
		// Change Volumn
		if((gc_SelectVol==1)&&(gw_ShowTimer!=0))
		{			
			if(gw_DisplayVol!=gs_DSP_GLOBAL_RAM.sw_Volume)
			{
				if(gc_LCM_Media!=4)
				{
					gc_LCM_Media=4;	// 4=Change VOL
					LCM_ClearScreen();
				}
				gw_DisplayVol=gs_DSP_GLOBAL_RAM.sw_Volume;
				LCM_ShowVOL();
				gw_DisplayTime=0xFFFF;
				gw_DisplayFreq1=0xFFFF;
				gc_DisplayEQIcon=0xFF;
				gc_LRCLCMClear_Flag=1;
			}
			return;
		}		
		else
		{
			gw_DisplayVol=0xFF;
		}

		// Show Song Number
		if((gb_Frequency_Song==1)&&(gw_ShowTimer!=0))
		{
			if(gw_DisplayVol!=gs_DSP_GLOBAL_RAM.sw_Volume)
			{
				if(gw_DispSongNum!=gw_FileIndex[0])
				{
					gw_DispSongNum=gw_FileIndex[0];
					if(gc_LCM_Media!=5)
					{
						gc_LCM_Media=5;	// 5=Song Number
						LCM_ClearScreen();
					}
					LCM_ShowSongNumber();
					gw_DisplayTime=0xFFFF;
					gw_DisplayFreq1=0xFFFF;
					gc_DisplayEQIcon=0xFF;
					gc_LRCLCMClear_Flag=1;
				}
			}
			return;
		}
		else
		{
			gw_DisplayVol=0xFF;
		}

		gc_LCM_Media=0xff;
		// Show Play mode(128x32 only)
		gc_DisplayPlayMode=gc_RepPlayMode;

		// Display Song name
		if(gw_FileTotalNumber[0]==0)
		{
			if((gc_CurrentCard==0)  && (gb_FlashNoFileflag==0))
			{
				LCM_erase_one_page(2);
				LCM_erase_one_page(3);
				gb_FlashNoFileflag=1;
			}
			//LCM_Show_NOMP3FILE();
			LCM_ShowPlayTime(0);
			gw_DisplayTime=tw_DisplayTime;
		}
		else
		{
			if(gw_DispSongNum1!=gw_FileIndex[0])
			{
				U8	i;
				
				if((gc_CurrentCard==2) && (SD_Detect!=0))  ////if SD not exist return DOS_CLUSTER_LINK_ERR chiayen0813
				{
					return;		
				}
			
				if((gc_CurrentCard==5) && (gb_HostConnect == 0x00))  ////if Host not exist return DOS_CLUSTER_LINK_ERR chiayen 20081017
				{
					return;		
				}

				for(i=0;i<48;i++)
				{
					gc_FileLongName[i]=0;
				}
				gw_DispSongNum1=gw_FileIndex[0];
				DOS_GetLongFileName(0,gc_FileLongName);
				gw_LogData_Timer=2;  //20090107 chiayen modify  
				LCM_ClearScreen();
				LCM_Disp_FileName(&gc_FileLongName[5],gc_FileLongName[2],gc_FileLongName[4],1);
				gw_LCMScrollTimer=40;
				gw_DisplayTime=0xFFFF;
				gw_Disp_CurrentSec=0;
				gw_DisplayFreq1=0xFFFF;
				gc_DisplayEQIcon=0xFF;				
				gc_LRCDispSongName_flag=1;
			}
			if(gw_LCMScrollTimer==0)
			{
				if(!gc_LrcFileName_Exist)
				{
					RollFileName();
				}
				gw_LCMScrollTimer=40;
			}					
		}
	}

	// Display Frequency
	if((gw_DisplayFreq1!=gw_FM_frequency) && ((!gc_LrcFileName_Exist) || (gc_LRCDispSongName_flag==1)))
	{
		gw_DisplayFreq=gw_FM_frequency;
		gw_DisplayFreq1=gw_FM_frequency;
		gb_FlashNoFileflag=0;
		LCM_erase_one_page(0);
		LCM_erase_one_page(1);
		LCM_ShowFrequency();
	}

	if((gc_DisplayEQIcon!=gs_DSP_GLOBAL_RAM.sc_EQ_Type) && ((!gc_LrcFileName_Exist) || (gc_LRCDispSongName_flag==1)))
	{
		gc_DisplayEQIcon=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
		LCM_ShowEQIcon();
	}

	if(((gcPlay_FileType==1)&&(gs_System_State.c_Phase == TASK_PHASE_PLAYACT))&&(gc_Num_Flag==1 || ((gb_PickSong==1)||(gb_ChannelSet==1)) || gc_CurrentCard!=5))
	{
		WMA_DataIn();
	}

	if((!gc_LrcFileName_Exist) || (gc_LRCDispSongName_flag==1))
	{
		tw_DisplayTime=gw_Disp_CurrentSec;
		if(tw_DisplayTime!=gw_DisplayTime)
		{	
			LCM_ShowPlayTime(tw_DisplayTime);
			gw_DisplayTime=tw_DisplayTime;
		}
	}
	return;
}


void LCM_ClearScreen(void)
{
    U8 i;
	for(i=0;i<4;i++)
    {
        LCM_erase_one_page(i);
	}
}


void LCM_erase_one_page(U8 page)
{
	U8 i;

	LCM_set_address(page, 0);
	for (i=132; i; i--)
	{
		LCM_write_data(0);
	}
}


void LCM_ShowPlayTime(U16 time)
{
	U8	tc_column;

	tc_column=59;
	LCM_ShowChar8x16_1(time/600,tc_column);
	tc_column+=8;	

	LCM_ShowChar8x16_1((time%600)/60,tc_column);
	tc_column+=8;

	LCM_ShowChar8x16_1(10,tc_column);
	tc_column+=4;

	LCM_ShowChar8x16_1((time%60)/10,tc_column);
	tc_column+=8;

	LCM_ShowChar8x16_1(time%10,tc_column);
}


void LCM_ShowChar8x16_1(U8 Num,U8 tc_column)
{
	U8	i,j;

	for(i=0;i<2;i++)
	{
		LCM_set_address(i+gc_LCM_line,tc_column);
		for(j=0;j<8;j++)
		{
			LCM_write_data(Icon_NUM_8x16_1[Num*16+j+i*8]);
		}
	}
}


U8 LCM_Disp_FileName(U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 DispOnOff)
{
	U8 i,Column;
	U8 Tmp_DataBuf[32];
	U8 Sts;
	U8 tc_ColumnNum,tc_FirstWorldColumnNum;

	DispOnOff=1;
	tc_FirstWorldColumnNum = 0x00;
	Sts = 0;
	i=0;
	Column=0;

	while(i<nByte)
	{
		if(DispOnOff)
		{
			if(!tc_ISNOrUnicode)
			{
				tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(DataBuf+i+1),*(DataBuf+i),Tmp_DataBuf,1);
			}else{
				tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(DataBuf+i),*(DataBuf+i+1),Tmp_DataBuf,0);
			}
			if(i==0)
			{
				tc_FirstWorldColumnNum = tc_ColumnNum;
			}
		}			
		if((Column+(tc_ColumnNum&0x7f))>96)
		{
			Sts=1;
			goto DispOver;
		}
		if(DispOnOff)
		{	
			if((tc_ColumnNum&0x7f) > 8)
			{
				LCM_disp_HZKCharBMP(2,Column,Tmp_DataBuf,LCM_IsWord,0);
			}
			else
			{
				LCM_disp_HZKCharBMP(2,Column,Tmp_DataBuf,LCM_IsChar,0);
			}
		}
		Column+=(tc_ColumnNum&0x7f);

		if(tc_ColumnNum&0x80)
		{
			i+=2;
		}else
		{
			i+=1;
		}
	}
	Sts=0;
DispOver:
	while(Column<96 && DispOnOff)
	{
		LCM_set_address(2,Column);
		LCM_write_data(0x00);
		LCM_set_address(2+1,Column);
		LCM_write_data(0x00);
		Column++;
	}
	if(Sts)
	{
		if(tc_FirstWorldColumnNum&0x80)
		{
			gw_DispFileName_ByteOffset++;
		}
		gw_DispFileName_ByteOffset+=1;
	}else{
		gw_DispFileName_ByteOffset=0;
	}
	return Sts;//overstep display area
}


void LCM_disp_HZKCharBMP(U8 Page, U8 Column,U8 *BMPdataBuf, U8 CharOrWord,U8 reverse)
{
	U8 i,j;

	if(CharOrWord)//char(8x16)--------word(12x16)
	{
		i=0;j=31;
	}else{
		i=0;j=15;
	}
	LCM_set_address(Page,Column);

	for(;i<j;i=i+2)
	{
		if(reverse)
			LCM_write_data(~*(BMPdataBuf+(i)));
		else
			LCM_write_data(*(BMPdataBuf+(i)));
	}

	if(CharOrWord)
	{
		i=1;j=32;
	}else{
		i=1;j=16;
	}

	LCM_set_address(Page+1,Column);
	
	for(;i<j;i=i+2)
	{
		if(reverse)
			LCM_write_data(~*(BMPdataBuf+(i)));
		else
			LCM_write_data(*(BMPdataBuf+(i)));
	}
}


void RollFileName()
{
    if((!gw_LCMScrollTimer))
	{
		LCM_Disp_FileName(&gc_FileLongName[gw_DispFileName_ByteOffset+5],gc_FileLongName[2],gc_FileLongName[4]-gw_DispFileName_ByteOffset,1);
	}
}


void LRC_LCMDisplayString(U8 *tpc_DataBuf, U16 tw_nByte,U8 tc_RowAddr) 
{	
	U8 tc_ColumnEnd;
	U8 temp_RowAddr;
	U8 Tmp_DataBuf[32];
	U8 tc_ColumnAddr,tc_ColumnNum;
	U8 tc_temp;
	U8 CharDispOffset = 0;//bsb
	U16 tw_Loop;

	gc_LRCDispSongName_flag=0;
	temp_RowAddr = tc_RowAddr;
	tc_ColumnEnd=96-CharDispOffset;
	tc_ColumnAddr = 0;
	tc_temp = 0;
	tw_Loop = 0;
	if(gcPlay_FileType==0)  //20081211 chiayen add for LRC
	{
	MP3_DataIn();  //chiayen add
	}
	else if(gcPlay_FileType==1)
	{
		WMA_DataIn();	
	}

	while(tw_Loop < tw_nByte)
	{
		tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(tpc_DataBuf+tw_Loop),*(tpc_DataBuf+tw_Loop+1),Tmp_DataBuf,0);
		if(*(tpc_DataBuf+tw_Loop)&0x80)
		{
			tc_temp = 16;
		}
		else
		{
		    tc_temp = 8;
		}

		if((tc_ColumnAddr + tc_temp) >= tc_ColumnEnd)
		{
			LRC_LCMFillColumn(tc_RowAddr/8,tc_ColumnAddr,tc_ColumnEnd);
			if(tc_RowAddr + 16 >=temp_RowAddr+32) 
			{
				break;
			}
			tc_ColumnAddr = 0;
			tc_RowAddr += 16;
		}
		if((tc_ColumnNum&0x7f)>8)
		{
			LCM_disp_HZKCharBMP(tc_RowAddr/8,tc_ColumnAddr,Tmp_DataBuf,1,0);
		}
		else
		{
			LCM_disp_HZKCharBMP(tc_RowAddr/8,tc_ColumnAddr,Tmp_DataBuf,0,0);
			if(tc_temp == 16)  LRC_LCMFillColumn(tc_RowAddr/8,tc_ColumnAddr + 8,tc_ColumnAddr + 16);
		}
		tc_ColumnAddr+=tc_temp;
		if(tc_ColumnNum&0x80)
		{
			tw_Loop+=2;
		}
		else
		{
			tw_Loop+=1;
		}
	}
	if(tc_ColumnAddr < tc_ColumnEnd)
	{
		LRC_LCMFillColumn(tc_RowAddr/8,tc_ColumnAddr,tc_ColumnEnd);
	}
	tc_RowAddr += 16;
	while(tc_RowAddr < temp_RowAddr+32)
	{
		LRC_LCMFillColumn(tc_RowAddr/8,0,tc_ColumnEnd);
		tc_RowAddr += 16;
	}
}


void LRC_LCMFillColumn(U8 tc_Page,U8 tc_CurrentColumn,U8 tc_TotalColumn)
{
	U8 tc_Width,i,j;

	tc_Width = tc_TotalColumn - tc_CurrentColumn;
	for(j=0;j<2;j++)
	{
		LCM_set_address(tc_Page+j,tc_CurrentColumn);
		for(i=0;i<tc_Width;i++)
		{		
			LCM_write_data(0);	
		}
	}
}