#include "..\IR\BMP_96x32.c"
#include "..\header\variables.h"

#define	LCD_start	18

data	bit	gb_LCM_reverse=0;
//data	bit	gb_Play_Pause=0;	// 0=Pause	1=Play
data	bit	gb_ChannelSet=0;  
xdata	U8	gc_Err=0;
data	bit	gb_PickSong=0;
data	bit	gb_Frequency_Song=0;	// 0=Show Frequency    1=Show Song Number/EQ/Vol
xdata	U8	gc_SelectEQ=0;			// 1=Show EQ
xdata	U8	gc_SelectVol=0;			// 1=Show Vol
data	U8	gc_LCM_Media=0xFF;	// 0=Show LineIn BMP  1=Show playing info
data	U8	gw_DisplayVol;
data	U8	gc_DisplayEQ;
data	U8	gc_DisplayEQIcon=0xFF;
data	U8	gc_DisplaySongnameCount=0;
data	U8	gc_LCM_line;
data	U8	gc_DisplayPlayMode=0xFF;
data	U8	gc_ShowTimer=0;
xdata	U16	gw_DisplayBitRate=0xFFFF;
xdata	U16	gw_DisplayTime=0xFFFF;
xdata	U16	gw_DisplayBitrateTime=0;
xdata	U16	gw_DisplayFreq=0xFFFF;
xdata	U16	gw_DisplayFreq1=0xFFFF;	
xdata	U16	gw_DispSongNum=0xFFFF;
xdata	U16	gw_DispSongNum1=0xFFFF;
data	U16	gw_LCMScrollTimer=0;
data 	U16 gw_FM_frequency=875;

extern DSP_GLOBAL gs_DSP_GLOBAL_RAM;
extern xdata U8 gc_NextorPreFlag;
extern data System_Struct gs_System_State;
extern data U8 gc_CurrentCard;
extern xdata U16 gw_irkey_count;
extern data U16 gw_LogData_Timer;
extern xdata U8 gc_NUMBER[4];
extern xdata U8	gc_FileLongName[];
extern xdata U16 gw_FileIndex[]; //3
extern xdata U16 gw_FileTotalNumber[];//3
extern xdata UBYTE gc_RepPlayMode;
extern xdata UBYTE gc_PhaseInx;
extern xdata UBYTE gcPlay_FileType;
extern xdata WORD gw_CurrentSec;
extern U8 USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);
extern UBYTE DOS_GetLongFileName(UBYTE tc_FileHandle, UBYTE * tpc_LongName);

void LCM_set_address(U8 page, U8 col);
void LCM_Display(void);
void LCM_ShowCompanyLogo(void);
void LCM_ShowFrequency(void);
void LCM_ShowPAUSE(void);
void LCM_ShowVOL(void);
void LCM_ShowFrequency_L(void);	// 12x32
void LCM_ShowPlayMode(void);	// Big icon
void LCM_ShowEQ(void);
void LCM_ShowERROR(void);
void LCM_ShowIRNum(void);
void LCM_ShowEQIcon(void);
void LCM_ClearScreen(void);
void LCM_init_process(void);
void LCM_Init(void);
void LCM_clear(void);
void LCM_ShowMediaType(void);
void LCM_ShowSongNumber(void);
void RollFileName();
void LCM_ShowWait();
void LCM_Disp_PCConnect();
void LCM_Disp_PC();
void LCM_Disp_MyMP3Logo();
void LCM_erase_one_page(U8 page);
void LCM_ShowPlayTime(U16 time);
void LCM_ShowChar12x32(U8 Num,U8 tc_column);
void LCM_ShowChar8x16(U8 Num,U8 tc_column);
void LCM_ShowChar8x16_L2(U8 Num,U8 tc_column);
void LRC_LCMDisplayString(U8 *tpc_DataBuf, U16 tw_nByte,U8 tc_RowAddr);
void LRC_LCMFillColumn(U8 tc_Page,U8 tc_CurrentColumn,U8 tc_TotalColumn);
void LCM_disp_HZKCharBMP(U8 Page, U8 Column,U8 *BMPdataBuf, U8 CharOrWord,U8 reverse);
U8 LCM_Disp_FileName(U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 DispOnOff);
U8 LCM_UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tbt_UnicodeOrISN);
extern xdata U8 gc_LRCLCMClear_Flag;  //20090513 chiayen add

void LCM_ShowChar12x32(U8 Num,U8 tc_column)
{
	U8	i,j;

	for(i=0;i<4;i++)
	{
		LCM_set_address(i,tc_column);
		for(j=0;j<12;j++)
		{
			LCM_write_data(CharIcon12x32[Num*48+j+i*12]);
		}
	}
}



void LCM_ShowFrequency(void)	// 8x16
{
	U8	tc_column;

	tc_column=1;

	if(gw_FM_frequency>999)
	{
		LCM_ShowChar8x16(1,tc_column);
		tc_column+=8;
	}
	else
	{
		LCM_ShowChar8x16(12,tc_column);	// space
		tc_column+=8;	
	}

	LCM_ShowChar8x16((gw_FM_frequency%1000)/100,tc_column);
	tc_column+=8;	

	LCM_ShowChar8x16((gw_FM_frequency%100)/10,tc_column);
	tc_column+=8;

	LCM_ShowChar8x16(11,tc_column);	// .
	tc_column+=4;
	LCM_ShowChar8x16(12,tc_column);	// space
	tc_column-=2;
	LCM_ShowChar8x16(gw_FM_frequency%10,tc_column);
}


void LCM_ShowChar8x16(U8 Num,U8 tc_column)
{
	U8	i,j;

	for(i=0;i<2;i++)
	{
		LCM_set_address(i+gc_LCM_line,tc_column);
		for(j=0;j<8;j++)
		{
			LCM_write_data(Icon_NUM_8x16[Num*16+j+i*8]);
		}
	}
}


void LCM_ShowVOL(void)
{
	U8	tc_column;
	U8	VOL;

	tc_column=12;
	LCM_ShowChar12x32(28,tc_column);	// V
	tc_column+=12;
	LCM_ShowChar12x32(23,tc_column);	// O
	tc_column+=12;
	LCM_ShowChar12x32(20,tc_column);	// L
	tc_column+=24;

	VOL=gs_DSP_GLOBAL_RAM.sw_Volume/2;
	LCM_ShowChar12x32(VOL/10,tc_column);
	tc_column+=12;
	LCM_ShowChar12x32(VOL%10,tc_column);
}


void LCM_ShowSongNumber(void)	// 12x32
{
	U8	tc_column;

	tc_column=8;
	if(gw_irkey_count==0)  //chiayentest
	{
		LCM_ShowChar12x32(gw_FileIndex[0]/100,tc_column);
		tc_column+=12;
		LCM_ShowChar12x32((gw_FileIndex[0]%100)/10,tc_column);
		tc_column+=12;
		LCM_ShowChar12x32(gw_FileIndex[0]%10,tc_column);
		tc_column+=12;
	}
	else
	{
		LCM_ShowChar12x32(gw_irkey_count/100,tc_column);
		tc_column+=12;
		LCM_ShowChar12x32((gw_irkey_count%100)/10,tc_column);
		tc_column+=12;
		LCM_ShowChar12x32(gw_irkey_count%10,tc_column);
		tc_column+=12;		
	}

	LCM_ShowChar12x32(10,tc_column);	// "/"
	tc_column+=12;
	LCM_ShowChar12x32(gw_FileTotalNumber[0]/100,tc_column);
	tc_column+=12;
	LCM_ShowChar12x32((gw_FileTotalNumber[0]%100)/10,tc_column);
	tc_column+=12;
	LCM_ShowChar12x32(gw_FileTotalNumber[0]%10,tc_column);
	
	gw_DisplayTime=0xFFFF;
	gw_DisplayFreq1=0xFFFF;
	gc_DisplayEQIcon=0xFF;
}


void LCM_ShowFrequency_L(void)	// 12x32
{
	U8	tc_column;

	tc_column=0;
	if(gw_FM_frequency>999)
	{
		LCM_ShowChar12x32(1,tc_column);
	}
	else
	{
		LCM_ShowChar12x32(31,tc_column);
	}
	tc_column+=12;

	LCM_ShowChar12x32((gw_FM_frequency%1000)/100,tc_column);
	tc_column+=12;
	LCM_ShowChar12x32((gw_FM_frequency%100)/10,tc_column);
	tc_column+=12;
	LCM_ShowChar12x32(32,tc_column);	// "."
	tc_column+=8;
	LCM_ShowChar12x32(gw_FM_frequency%10,tc_column);
	tc_column+=12;
	LCM_ShowChar12x32(21,tc_column);	// M
	tc_column+=12;
	LCM_ShowChar12x32(17,tc_column);	// H
	tc_column+=12;
	LCM_ShowChar12x32(30,tc_column);	// z	
}


void LCM_ShowPlayMode(void)	// Big icon
{
	U8	i,j,k=0;
	U8	tc_column;

	tc_column=32;	
	for(i=0;i<4;i++)
	{
		LCM_set_address(i,tc_column);
		for(j=0;j<32;j++)
		{
			LCM_write_data(PlayModeIcon96x32[(gc_RepPlayMode-1)*128+j+i*32]);
		}
	}
}


void LCM_ShowEQ(void)
{
    U8 tc_word,i;
	U8 tc_column;
	U8 tc_EQ_table[8];

	switch(gs_DSP_GLOBAL_RAM.sc_EQ_Type)
	{
		default:
			gs_DSP_GLOBAL_RAM.sc_EQ_Type=0;
		case 0:	// NORMAL
			tc_word=6;
			tc_EQ_table[0]=22;
			tc_EQ_table[1]=23;
			tc_EQ_table[2]=25;
			tc_EQ_table[3]=21;
			tc_EQ_table[4]=11;
			tc_EQ_table[5]=20;
			break;
		case 1:	// DBB
			tc_word=3;
			tc_EQ_table[0]=14;
			tc_EQ_table[1]=12;
			tc_EQ_table[2]=12;
			break;
		case 2:	// JAZZ
			tc_word=4;
			tc_EQ_table[0]=19;
			tc_EQ_table[1]=11;
			tc_EQ_table[2]=29;
			tc_EQ_table[3]=29;
			break;
		case 3:	// DISC
			tc_word=4;
			tc_EQ_table[0]=14;
			tc_EQ_table[1]=18;
			tc_EQ_table[2]=26;
			tc_EQ_table[3]=13;
			break;
		case 4:	// POP
			tc_word=3;
			tc_EQ_table[0]=24;
			tc_EQ_table[1]=23;
			tc_EQ_table[2]=24;
			break;
		case 5:	// CLASS
			tc_word=5;
			tc_EQ_table[0]=13;
			tc_EQ_table[1]=20;
			tc_EQ_table[2]=11;
			tc_EQ_table[3]=26;
			tc_EQ_table[4]=26;
			break;
	}
	tc_column=(8-tc_word)*6;
	for(i=0;i<tc_word;i++)
	{
		LCM_ShowChar12x32(tc_EQ_table[i],tc_column);		
		tc_column+=12;		
	}
}


void LCM_ShowPAUSE(void)
{
	LCM_ClearScreen();
	LCM_ShowChar12x32(24,18);	// P
	LCM_ShowChar12x32(11,30);	// A
	LCM_ShowChar12x32(27,42);	// U
	LCM_ShowChar12x32(26,54);	// S
	LCM_ShowChar12x32(15,66);	// E
}


void LCM_ShowERROR(void)
{
	LCM_ClearScreen();
	LCM_ShowChar12x32(15,18);	// E
	LCM_ShowChar12x32(25,30);	// R
	LCM_ShowChar12x32(25,42);	// R	
	LCM_ShowChar12x32(23,54);	// O
	LCM_ShowChar12x32(25,66);	// R
}


void LCM_ShowIRNum(void)
{
	U8	i;
	U8	tc_column;
	U8	tc_ShowCount=4;
	U8	tc_offset=0;
	
	if(gb_ChannelSet==1)	// Frequency
	{
	    gc_LRCLCMClear_Flag=1;  //20090513 chiayen add
		tc_column=0;
	}
	else	// SONG
	{
		gb_LCM_reverse=0;
		tc_offset=0;
		tc_column=tc_offset+(U8)gb_LCM_reverse*LCM_ReverseOffset;
		LCM_ShowChar12x32(26,tc_column);	// S		
		LCM_ShowChar12x32(23,tc_column+12);	// O
		LCM_ShowChar12x32(22,tc_column+24);	// N
		LCM_ShowChar12x32(16,tc_column+36);	// G
		LCM_ShowChar12x32(31,tc_column+48);	// space

		tc_column+=60;
		if(((gc_NUMBER[2]==2)||(gc_NUMBER[2]==1))&&((gc_NUMBER[1]==0xFF)||(gc_NUMBER[0]==0xFF)))
		{
			LCM_ShowChar12x32(gc_NUMBER[2],tc_column);		
			tc_column+=12;
			if(gc_NUMBER[1]==0xFF)
			{
				LCM_ShowChar12x32(0,tc_column);		
				tc_column+=12;
				if(gc_NUMBER[0]==0xFF)
				{
					LCM_ShowChar12x32(0,tc_column);	
				}
				else
				{
					LCM_ShowChar12x32(gc_NUMBER[0],tc_column);	
				}
				return;
			}
			else
			{
				LCM_ShowChar12x32(gc_NUMBER[1],tc_column);		
				tc_column+=12;
				if(gc_NUMBER[0]==0xFF)
				{
					LCM_ShowChar12x32(0,tc_column);	
				}
				else
				{
					LCM_ShowChar12x32(gc_NUMBER[0],tc_column);	
				}
			}
			return;
		}
		else 
		{
			if(gc_NUMBER[2]!=0xFF)
			{
				LCM_ShowChar12x32(gc_NUMBER[2],tc_column);		
				tc_column+=12;
			}

			if(gc_NUMBER[1]!=0xFF)
			{
				LCM_ShowChar12x32(gc_NUMBER[1],tc_column);		
				tc_column+=12;
			}

			if(gc_NUMBER[0]!=0xFF)
			{
				LCM_ShowChar12x32(gc_NUMBER[0],tc_column);		
			}
		}
		return;
	}

	if(gc_NUMBER[3]!=0xFF)
	{
		LCM_ShowChar12x32(gc_NUMBER[3],tc_column);		
		tc_column+=12;
		tc_ShowCount--;
	}

	if(gc_NUMBER[2]!=0xFF)
	{
		if((tc_ShowCount==4)&&(gc_NUMBER[2]!=1))
		{
			LCM_ShowChar12x32(31,tc_column);	// space		
			tc_column+=12;
			tc_ShowCount--;
		}
		LCM_ShowChar12x32(gc_NUMBER[2],tc_column);		
		tc_column+=12;
		tc_ShowCount--;
	}

	if(gc_NUMBER[1]!=0xFF)
	{
		if((tc_ShowCount==4)&&(gc_NUMBER[1]!=1))
		{
			LCM_ShowChar12x32(31,tc_column);	// space		
			tc_column+=12;
			tc_ShowCount--;
		}
		LCM_ShowChar12x32(gc_NUMBER[1],tc_column);		
		tc_column+=12;
		tc_ShowCount--;
	}


	if(gc_NUMBER[0]!=0xFF)
	{
		if((tc_ShowCount==4)&&(gc_NUMBER[0]!=1))
		{
			LCM_ShowChar12x32(31,tc_column);	// space		
			tc_column+=12;
			tc_ShowCount--;
		}
		if(tc_ShowCount==1)	// Frequency
		{
			LCM_ShowChar12x32(32,tc_column);	// .
			tc_column+=8;
		}
		LCM_ShowChar12x32(gc_NUMBER[0],tc_column);		
		tc_column+=12;
		tc_ShowCount--;
	}


	for(i=0;i<tc_ShowCount;i++)
	{
		if(i==(tc_ShowCount-1))	// Frequency
		{
			LCM_ShowChar12x32(32,tc_column);	// .
			tc_column+=8;
		}
		LCM_ShowChar12x32(31,tc_column);		
		tc_column+=12;
	}

	LCM_ShowChar12x32(21,tc_column);	// M
	tc_column+=12;
	LCM_ShowChar12x32(17,tc_column);	// H
	tc_column+=12;
	LCM_ShowChar12x32(30,tc_column);	// z	
}


void LCM_ShowEQIcon(void)
{
	U8	i,j,k;

	k=gs_DSP_GLOBAL_RAM.sc_EQ_Type*38;
	for(i=0;i<2;i++)
	{
		LCM_set_address(i,38);
		for(j=0;j<19;j++)
		{
			LCM_write_data(Icon_EQ_19x16[k]);
			k++;
		}
	}
}


void LCM_init_process(void)
{
	LCM_Init();
	LCM_BACKLIGHT=0;
	LCM_ShowCompanyLogo();  //chiayenmark for car	
	LCM_clear();  //20081124 chiayen add
}

#ifdef CAR_64
	#if(LCM_SIZE==1)
	U8 code LCM_InitCommand[]={0xe2,0xa2,0xa0,0xc8,0xa6,0xa4,0x22,0x81,0x1c,0x2f,0x40,0xaf};//96x32
	#endif
	#if(LCM_SIZE==0)||(LCM_SIZE==2)
	U8 code LCM_InitCommand[]={0xe2,0xa2,0xa0,0xc8,0xa6,0xa4,0x24,0x81,24,0x2f,0x40,0xaf};//128x64
	#endif
#endif

#ifdef EVB_128
U8 code LCM_InitCommand[]={0xe2,0xa2,0xa0,0xc8,0xa6,0xa4,0x24,0x81,24,0x2f,0x40,0xaf};//128x64
#endif

void LCM_Init(void)
{
	U8 i;

    XBYTE[0xB102]|=0x06;
	LCM_RESET=0;
	USER_DelayDTms(20);
	LCM_RESET=1;

	for (i=0; i<sizeof(LCM_InitCommand); i++)  //fill LCD parameter
	{
		LCM_write_command(LCM_InitCommand[i]);
	}
	LCM_clear();
}


void LCM_clear(void)
{	
	U8 i;

	for (i=0; i<8; i++)
	{
		LCM_erase_one_page(i);	
	}
}

#if 0
void LRC_LCMDisplayString(U8 *tpc_DataBuf, U16 tw_nByte,U8 tc_RowAddr) 
{	
	U8 tc_ColumnEnd;
	U8 temp_RowAddr;
	U8 Tmp_DataBuf[32];
	U8 tc_ColumnAddr,tc_ColumnNum;
	U8 tc_temp;
	U8 CharDispOffset = 0;//bsb
	U16 tw_Loop;

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
#endif

void LCM_Disp_PCConnect()  //for display USB
{
	LCM_clear();
	LCM_Disp_PC();
	LCM_Disp_MyMP3Logo();
	LCM_Disp_USBLine();
}


void LCM_Disp_PC()  //for display USB
{
	#if(LCM_SIZE==0)||(LCM_SIZE==2)
	U8  code X_C_PC[]={4,24,24,86,0,0};//offset3:start_column,offset5:start_page
	#endif
	#if(LCM_SIZE==1)
	U8  code X_C_PC[]={4,24,24,72,0,0};//offset3:start_column,offset5:start_page
	#endif

	LCM_Disp_XBMPArray_USB(X_C_PC,LCM_BmpPC);
}


void LCM_Disp_MyMP3Logo()	//for dispale USB
{
	#if(LCM_SIZE==0)||(LCM_SIZE==2)
	U8  code X_C_MyMP3Logo[]={4,38,38,8,0,0};//offset3:start_column,offset5:start_page
	#endif
	#if(LCM_SIZE==1)
	U8  code X_C_MyMP3Logo[]={4,38,38,0,0,0};//offset3:start_column,offset5:start_page
	#endif

	LCM_Disp_XBMPArray_USB(X_C_MyMP3Logo,LCM_BmpMyMP3Logo);
}


void LCM_ShowCompanyLogo(void)
{
    U8 i,j,k;

	LCM_clear();

	for(k=0;k<5;k++)
	{
		for(j=0;j<4;j++)
		{
			LCM_set_address(j,0);
	
			for(i=0;i<100;i++)
			{
				LCM_write_data(Icon_PowerOnLogo_100x32[((U16)j*100+k*400)+i]);
			}
		}
		USER_DelayDTms(200);
	}
}


void LCM_ShowMediaType(void)
{
}


void LCM_ShowWait()
{
	U8	tc_column;
	gc_LCM_line=2;
	tc_column=21;
	LCM_ShowChar8x16(13,tc_column);	// W
	tc_column+=12;
	LCM_ShowChar8x16(14,tc_column);	// a
	tc_column+=12;
	LCM_ShowChar8x16(15,tc_column);	// i
	tc_column+=12;
	LCM_ShowChar8x16(16,tc_column);	// t
	tc_column+=6;
	LCM_ShowChar8x16(11,tc_column);	// .
	tc_column+=6;
	LCM_ShowChar8x16(11,tc_column);	// .
	tc_column+=6;
	LCM_ShowChar8x16(11,tc_column);	// .
	gc_LCM_line=0;
}


void LCD_Disp_NoFile()  //20090107 chiayen add
{
	U8	tc_column;
	gc_LCM_line=2;
	tc_column=20;
	LCM_ShowChar8x16(17,tc_column);	// N
	tc_column+=8;
	LCM_ShowChar8x16(18,tc_column);	// o
	tc_column+=8;
	LCM_ShowChar8x16(19,tc_column);	//  
	tc_column+=8;
	LCM_ShowChar8x16(20,tc_column);	// F
	tc_column+=8;
	LCM_ShowChar8x16(21,tc_column);	// i
	tc_column+=8;
	LCM_ShowChar8x16(22,tc_column);	// l
	tc_column+=8;
	LCM_ShowChar8x16(23,tc_column);	// e
	gc_LCM_line=0;
}


void LCM_ShowSongNumber_1(void)	// 8x16
{
	U8	tc_column;
	gc_LCM_line=2;
	tc_column=8;

	LCM_ShowChar8x16(gw_FileIndex[0]/100,tc_column);
	tc_column+=8;
	LCM_ShowChar8x16((gw_FileIndex[0]%100)/10,tc_column);
	tc_column+=8;
	LCM_ShowChar8x16(gw_FileIndex[0]%10,tc_column);
	tc_column+=8;		

	LCM_ShowChar8x16(12,tc_column);	// "/"
	tc_column+=8;
	LCM_ShowChar8x16(gw_FileTotalNumber[0]/100,tc_column);
	tc_column+=8;
	LCM_ShowChar8x16((gw_FileTotalNumber[0]%100)/10,tc_column);
	tc_column+=8;
	LCM_ShowChar8x16(gw_FileTotalNumber[0]%10,tc_column);
	
	gw_DisplayTime=0xFFFF;
	gw_DisplayFreq1=0xFFFF;
	gc_DisplayEQIcon=0xFF;
	gc_LCM_line=0;
}

