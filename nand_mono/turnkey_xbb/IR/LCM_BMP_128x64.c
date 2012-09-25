#include "..\IR\BMP_128x64.c"
#include "..\header\variables.h"

data	bit	gb_LCM_reverse=0;
data	bit	gb_ChannelSet=0;  
xdata	U8	gc_Err=0;
data	bit	gb_PickSong=0;
data	bit	gb_Frequency_Song=0;	// 0=Show Frequency    1=Show Song Number/EQ/Vol
xdata	U8	gc_SelectEQ=0;			// 1=Show EQ
xdata	U8	gc_SelectVol=0;			// 1=Show Vol
xdata 	U8  gc_Media_type=0xFF;
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
xdata	U16	gw_DispSongNum=0xFFFF;
xdata	U16	gw_DispSongNum1=0xFFFF;
data	U16	gw_LCMScrollTimer=0;
data 	U16 gw_FM_frequency=875;
data	U16	gw_DisplayPinPuTime=0;	  //---sunzhk add 100427

extern DSP_GLOBAL gs_DSP_GLOBAL_RAM;
extern xdata U8 gc_NextorPreFlag;
extern data System_Struct gs_System_State;
extern data U8 gc_CurrentCard;
extern xdata U16 gw_irkey_count;
extern data U16 gw_LogData_Timer;
extern xdata U8 gc_LrcFileName_Exist;
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
extern void LCM_ShowFreqNum(U8 Num,U8 tc_column);
void LCM_set_address(U8 page, U8 col);
void LCM_Display(void);
void LCM_ShowCompanyLogo(void);
//void LCM_ShowLineIn(void);
void LCM_ShowFMIcon(void);
void LCM_ShowSDIcon(void);
void LCM_ShowNandIcon(void);
void LCM_ShowUSBIcon(void);
void LCM_ShowFrequency(void);
void LCM_ShowPAUSE(void);
void LCM_ShowVOL(void);
void LCM_ShowFrequency_L(void);	// 12x32
void LCM_ShowPlayMode(void);	// Big icon
void LCM_ShowPlayModeIcon(void);
void LCM_ShowEQ(void);
void LCM_Show_NOMP3FILE(void);
void LCM_ShowERROR(void);
void LCM_ShowIRNum(void);
void LCM_ShowIRNum_PicKSong(void);
void LCM_ShowMediaType(void);
void LCM_ShowEQIcon(void);
void LCM_ShowPlayVol(void);
void LCM_ShowBitRate(void);
//void LCM_ShowBitRateIcon(void);
void LCM_ClearScreen(void);
void LCM_ClearPage(U8 Page,U8 Page1);
void LCM_init_process(void);
void LCM_Init(void);
#if (SERIAL_MONO == FEATURE_OFF)  // Re-#define to mine
void LCM_clear(void);
#endif
void LCM_clear_dir(void);  //20090216 chiayen add 
void LCM_ShowSongNumber(void);
void LCM_ShowSongNumber_dir(void);
void RollFileName();
void LCM_Disp_PCConnect();
void LCM_Disp_PC();
void LCM_Disp_MyMP3Logo();
void LCM_erase_one_page(U8 page);
void LCM_ShowPlayTime(U16 time);
void LCM_ShowChar12x32(U8 Num,U8 tc_column);
void LCM_ShowChar12x32_dir(U8 Num,U8 tc_column);
void LCM_ShowChar8x16(U8 Num,U8 tc_column);
void LCM_ShowChar8x16_L2(U8 Num,U8 tc_column);
void LRC_LCMDisplayString(U8 *tpc_DataBuf, U16 tw_nByte,U8 tc_RowAddr);
void LRC_LCMFillColumn(U8 tc_Page,U8 tc_CurrentColumn,U8 tc_TotalColumn);
void LCM_disp_HZKCharBMP(U8 Page, U8 Column,U8 *BMPdataBuf, U8 CharOrWord,U8 reverse);
U8 LCM_Disp_FileName(U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 DispOnOff);
U8 LCM_UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tbt_UnicodeOrISN);
extern void LCM_ShowChar16x14(U8 Num,U8 tc_column);

extern code U8 CharIcon8x16[];  //0916

extern xdata U8 gc_IRNum_Count;
extern void Menu_Disp_Item_Play(U8 tc_MenuType,U8 tc_Select,U8 tc_RefreshType);	  //tc_RefreshType always =1;


void LCM_ShowCompanyLogo(void)
{
    U8 i,j,k=0;
	U8 tc_offset=0;

	LCM_clear();
	gb_LCM_reverse=0;
	tc_offset=0;
	k=0;

	for(j=0;j<8;j++)//
	{
		LCM_set_address(j+k,(U8)gb_LCM_reverse*LCM_ReverseOffset+tc_offset);		   // 

			for(i=0;i<128;i++)
			{
				LCM_write_data(CompanyLogo96x32[((U16)j*128)+i]);
			}
	}

	LCM_BACKLIGHT=0;
}


void LCM_ShowChar12x32(U8 Num,U8 tc_column)
{
	U8	i,j,k;

	k=2;	
	for(i=0;i<4;i++)
	{
		LCM_set_address(i+k,tc_column);
		for(j=0;j<12;j++)
		{
			LCM_write_data(CharIcon12x32[Num*48+j+i*12]);
		}
	}
}
void LCM_ShowChar12x32_dir(U8 Num,U8 tc_column)
{
	U8	i,j,k;

	k=4;	
	for(i=0;i<4;i++)
	{
		LCM_set_address(i+k,tc_column);
		for(j=0;j<12;j++)
		{
			LCM_write_data(CharIcon12x32[Num*48+j+i*12]);
		}
	}
}


void LCM_ShowSDIcon(void)
{
	U8	i;
	U16	j;

	for(i=0;i<2;i++)
	{
		LCM_set_address(i,0);
		for(j=0;j<24;j++)
		{
			LCM_write_data(SD_Icon128x32[j+i*24]);
		}
	}
}


void LCM_ShowNandIcon(void)
{
	U8	i;
	U16	j;

	for(i=0;i<2;i++)
	{
		LCM_set_address(i,0);
		for(j=0;j<24;j++)
		{
			LCM_write_data(NAND_ICON24X16[j+i*24]);
		}
	}
}


void LCM_ShowUSBIcon(void)
{
	U8	i;
	U16	j;

	for(i=0;i<2;i++)
	{
		LCM_set_address(i,0);
		for(j=0;j<24;j++)
		{
			LCM_write_data(USB_Icon24x16[j+i*24]);
		}
	}
}


void LCM_ShowFrequency(void)	// 14x32
{
	U8	tc_column;

/*	if((gc_CurrentCard==2)||(gc_CurrentCard==3)||(gc_CurrentCard==5))	// Media insert
	{
		tc_column=58;
	}
	else
	{
		tc_column=58;	
	}
*/
	LCM_ClearPage(4,8);
	tc_column =50;
	if(gw_FM_frequency>999)
	{
		LCM_ShowFreqNum(1,tc_column);					//homeing
		tc_column+=16;
	}
	else
	{
		LCM_ShowFreqNum(10,tc_column);	// space
		tc_column+=16;
		//if((tc_column==(8+(U8)gb_LCM_reverse*LCM_ReverseOffset))||(tc_column==(16+(U8)gb_LCM_reverse*LCM_ReverseOffset)))	// No Media
		//{
		//	LCM_ShowChar8x16(12,tc_column);	// space
		//	tc_column+=8;	
		//}
	}

	LCM_ShowFreqNum((gw_FM_frequency%1000)/100,tc_column);
	tc_column+=16;	

	LCM_ShowFreqNum((gw_FM_frequency%100)/10,tc_column);
	tc_column+=16;

	LCM_ShowFreqNum(11,tc_column);
	tc_column+=16;

	LCM_ShowFreqNum(gw_FM_frequency%10,tc_column);
	/*tc_column+=8;

	LCM_ShowChar8x16(25,tc_column);	// M
	tc_column+=8;
	LCM_ShowChar8x16(20,tc_column);	// H
	tc_column+=8;	
	LCM_ShowChar8x16(64,tc_column);	// z*/
}




void LCM_ShowChar8x16(U8 Num,U8 tc_column)
{
	U8	i,j;

	for(i=0;i<2;i++)
	{
		LCM_set_address(i+gc_LCM_line,tc_column);
		for(j=0;j<8;j++)
		{
			LCM_write_data(CharIcon8x16[Num*16+j+i*8]);
		}
	}
}


void LCM_ShowChar8x16_L2(U8 Num,U8 tc_column)
{
	U8	i,j,k;

	k=0;//1
	for(i=2;i<4;i++)
	{
		LCM_set_address(i+k,tc_column);
		for(j=0;j<8;j++)
		{
			LCM_write_data(CharIcon8x16[Num*16+j+(i-2)*8]);
		}
	}
}


void LCM_ShowPAUSE(void)
{
    U8 i,j,k=0;
	U8	tc_column;

	tc_column=(2*16)+((U8)gb_LCM_reverse*LCM_ReverseOffset);
	tc_column=tc_column-16;
	k=2;
	LCM_ClearScreen();
	for(j=0;j<4;j++)
	{
		LCM_set_address(j+k,tc_column);
		for(i=0;i<96;i++)
		{
			LCM_write_data(PAUSEIcon96x32[((U16)j*96)+i]);
		}
	}
}


void LCM_ShowVOL(void)
{
	U8	tc_column;
	U8	VOL;

	tc_column=28;
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

void LCM_ShowPlayVol(void)
{
	U8	tc_column;
	U8	VOL;

	tc_column=2;

	gc_LCM_line=0;
	VOL=gs_DSP_GLOBAL_RAM.sw_Volume/2;
	LCM_ShowChar8x16(VOL/10,tc_column);
	tc_column+=10;
	LCM_ShowChar8x16(VOL%10,tc_column);
}
void LCM_ShowSongNumber(void)	// 12x32
{
	U8	tc_column;

	tc_column=24;
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
}

void LCM_ShowSongNumber_dir(void)	// 12x32
{
	U8	tc_column;

	tc_column=24;
	if(gw_irkey_count==0)  //chiayentest
	{
		LCM_ShowChar12x32_dir(gw_FileIndex[0]/100,tc_column);
		tc_column+=12;
		LCM_ShowChar12x32_dir((gw_FileIndex[0]%100)/10,tc_column);
		tc_column+=12;
		LCM_ShowChar12x32_dir(gw_FileIndex[0]%10,tc_column);
		tc_column+=12;
	}
	else
	{
		LCM_ShowChar12x32_dir(gw_irkey_count/100,tc_column);
		tc_column+=12;
		LCM_ShowChar12x32_dir((gw_irkey_count%100)/10,tc_column);
		tc_column+=12;
		LCM_ShowChar12x32_dir(gw_irkey_count%10,tc_column);
		tc_column+=12;		
	}
	LCM_ShowChar12x32_dir(10,tc_column);	// "/"
	tc_column+=12;
	LCM_ShowChar12x32_dir(gw_FileTotalNumber[0]/100,tc_column);
	tc_column+=12;
	LCM_ShowChar12x32_dir((gw_FileTotalNumber[0]%100)/10,tc_column);
	tc_column+=12;
	LCM_ShowChar12x32_dir(gw_FileTotalNumber[0]%10,tc_column);
	
}

void LCM_ShowFrequency_L(void)	// 12x32
{
	U8	tc_column;

	tc_column=16;
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
 /*
void LCM_ShowChar20x24(U8 Num,U8 tc_column)
{
	U8	i,j,k;

	k=3;
	for(i=0;i<3;i++)
	{
		LCM_set_address(i+k,tc_column);
		for(j=0;j<20;j++)
		{
			LCM_write_data(PlayModeIcon96x32[Num*60+j+i*20]);
		}
	}
}  */

#if 1
void LCM_ShowPlayMode(void)
{

	switch(gc_RepPlayMode)
	{
		case 3:	// 随机播放
			 Menu_Disp_Item_Play(5,1,0x80);
			break;
		case 2:	// 全部循环
			 Menu_Disp_Item_Play(5,0,0x80);
			break;
		case 1:	// 单曲循环
			 Menu_Disp_Item_Play(5,2,0x80);
			break;
	}

}
#endif

#if 0
void LCM_ShowPlayMode(void)
{
    U8 tc_word,i;
	U8 tc_column;
	U8 tc_PlayMode_table[8];

	switch(gc_RepPlayMode)
	{
		case 3:	// 随机播放
			tc_word=4;
			tc_PlayMode_table[0]=37;
			tc_PlayMode_table[1]=15;
			tc_PlayMode_table[2]=2;
			tc_PlayMode_table[3]=19;
			break;
		case 2:	// 全部循环
			tc_word=4;
			tc_PlayMode_table[0]=78;
			tc_PlayMode_table[1]=79;
			tc_PlayMode_table[2]=43;
			tc_PlayMode_table[3]=16;
			break;
		case 1:	// 单曲循环
			tc_word=4;
			tc_PlayMode_table[0]=7;
			tc_PlayMode_table[1]=30;
			tc_PlayMode_table[2]=43;
			tc_PlayMode_table[3]=16;
			break;
	}
	tc_column=(8-tc_word)*6;
	tc_column+=16;
	for(i=0;i<tc_word;i++)
	{
		LCM_ShowChar16x14(tc_PlayMode_table[i],tc_column);     		
		tc_column+=14;		
	}
}
#endif

#if 0
void LCM_ShowPlayMode(void)	// Big icon
{
	U8	i,j,k=0;
	U8	tc_column;

	k=2;
	tc_column=48;	
	for(i=0;i<4;i++)
	{
		LCM_set_address(i+k,tc_column);
		for(j=0;j<32;j++)
		{
			LCM_write_data(PlayModeIcon96x32[(gc_RepPlayMode-1)*128+j+i*32]);
		}
	}
}
#endif

void LCM_ShowPlayModeIcon(void)	// Play mode small icon
{
	U8	i,j,k;

	for(i=0;i<2;i++)
	{
		LCM_set_address(i,49);
		for(j=0;j<11;j++)
		{
			k=PlayModeIcon128x64[(gc_RepPlayMode-1)*22+j+i*11];
			LCM_write_data(k);
		}
	}
}


void LCM_ShowEQ(void)
{
/*    U8 tc_word,i;
	U8 tc_column;
	U8 tc_EQ_table[3];
 
	switch(gs_DSP_GLOBAL_RAM.sc_EQ_Type)
	{
		default:
			gs_DSP_GLOBAL_RAM.sc_EQ_Type=0;
		case 0:	// NORMAL
			tc_word=2;
			tc_EQ_table[0]=19;
			tc_EQ_table[1]=12;
			break;
		case 1:	// DBB
			tc_word=3;
			tc_EQ_table[0]=18;
            tc_EQ_table[1]=22;
			tc_EQ_table[2]=17;
			break;
		case 2:	// JAZZ
			tc_word=2;
			tc_EQ_table[0]=9;
			tc_EQ_table[1]=13;
			break;
		case 3:	// DISCO
			tc_word=2;
			tc_EQ_table[0]=16;
			tc_EQ_table[1]=6;
			break;
		case 4:	// POP
			tc_word=2;
			tc_EQ_table[0]=10;
			tc_EQ_table[1]=15;
			break;
		case 5:	// CLASS
			tc_word=2;
			tc_EQ_table[0]=8;
			tc_EQ_table[1]=5;
			break;
	}
 	tc_column=(6-tc_word)*6;
	tc_column+=20;
	for(i=0;i<tc_word;i++)
	{
		 LCM_ShowChar20x24(tc_EQ_table[i],tc_column);     		
		tc_column+=20;		
	} */
	
   Menu_Disp_Item_Play(4,gs_DSP_GLOBAL_RAM.sc_EQ_Type,0x80);	  //tc_RefreshType always =0x80;

}


void LCM_Show_NOMP3FILE(void)
{
	U8	i;
	U8	tc_column=0;
	U8	tc_NoFile[12];

	tc_NoFile[0]=26;
	tc_NoFile[1]=27;
	tc_NoFile[2]=12;
	tc_NoFile[3]=25;
	tc_NoFile[4]=28;
	tc_NoFile[5]=03;
	tc_NoFile[6]=12;
	tc_NoFile[7]=18;
	tc_NoFile[8]=21;
	tc_NoFile[9]=24;
	tc_NoFile[10]=17;
	tc_NoFile[11]=67;
	
	for(i=0;i<12;i++)
	{
		LCM_ShowChar8x16_L2(tc_NoFile[i],tc_column);
		tc_column+=8;
	}
}


void LCM_ShowERROR(void)
{
	U8	tc_column;

	tc_column=34;
	LCM_ClearScreen();
	LCM_ShowChar12x32(15,tc_column);		
	tc_column+=12;		
	LCM_ShowChar12x32(25,tc_column);		
	tc_column+=12;		
	LCM_ShowChar12x32(25,tc_column);		
	tc_column+=12;		
	LCM_ShowChar12x32(23,tc_column);		
	tc_column+=12;		
	LCM_ShowChar12x32(25,tc_column);		
}

/*
void LCM_ShowIRNum(void)
{
	U8	i;
	U8	tc_column=30;
//	U8	tc_ShowCount=4;
//	U8	tc_offset=0;
//gc_IRNum_Count
	for(i=4;i>0;i--)
	{
		if(gc_NUMBER[i-1]!=0xFF)
		{
			LCM_ShowChar12x32(gc_NUMBER[i-1],tc_column);
			tc_column+=16;
		}
	}
}
*/
void LCM_ShowIRNum(void)
{
	U8	i;
	U8	tc_column;
	U8	tc_ShowCount=4;
	U8	tc_offset=0;

	if(gb_ChannelSet==1)	// Frequency
	{
		tc_column=16;      
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
	else if(gb_PickSong==1)	// SONG
	{
		gb_LCM_reverse=0;
		tc_offset=16;
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
	}else if(gc_Num_Flag==1)
    {
        tc_column=30;

        if(gc_NUMBER[3]!=0xFF)
    	{
    		LCM_ShowChar12x32(gc_NUMBER[3],tc_column);				
    		tc_column+=12;
    		tc_ShowCount--;
    	}
    
    	if(gc_NUMBER[2]!=0xFF)
    	{
    		LCM_ShowChar12x32(gc_NUMBER[2],tc_column);		
    		tc_column+=12;
    		tc_ShowCount--;
    	}
    
    	if(gc_NUMBER[1]!=0xFF)
    	{
    		LCM_ShowChar12x32(gc_NUMBER[1],tc_column);		
    		tc_column+=12;
    		tc_ShowCount--;
    	}   
    
    	if(gc_NUMBER[0]!=0xFF)
    	{
    		LCM_ShowChar12x32(gc_NUMBER[0],tc_column);		
    		tc_column+=12;
    		tc_ShowCount--;
    	}
    }  
}



void LCM_ShowMediaType(void)
{
	U8	i,j,k=0;

	if(gc_Media_type==1)
	{
		k=46;
	}
	else if(gc_Media_type==3)
	{
		k=92;
	}

	for(i=0;i<2;i++)
	{
		LCM_set_address(i,24);
		for(j=0;j<23;j++)
		{
			LCM_write_data(MediaTypeIcon23x16[k]);
			k++;
		}
	}
}


void LCM_ShowEQIcon(void)
{
	U8	i,j,k;

	k=gs_DSP_GLOBAL_RAM.sc_EQ_Type*30;
	for(i=0;i<2;i++)
	{
		LCM_set_address(i,81);
		for(j=0;j<15;j++)
		{
			LCM_write_data(EQIcon128x64[k]);
			k++;
		}
	}
}


void LCM_init_process(void)
{
	U8 i;

#if(SERIAL_MONO == FEATURE_ON)  // Re-#define to mine
    ui_init_spi();
#else
	LCM_Init();
#endif

	LCM_ShowCompanyLogo();  //chiayenmark for car
	for(i=0;i<=5;i++)
	{
		USER_DelayDTms(250);
	}
}

//U8 code LCM_InitCommand[]={0xe2,0xa2,0xa0,0xc8,0xa6,0xa4,0x23,0x81,0x20,0x2f,0x40,0xaf};//ST7565  128*64

//U8 code LCM_InitCommand[]={0xe2,0xa2,0xa0,0xc8,0xa6,0xa4,0x26,0x81,0x10,0x2f,0x40,0xaf};//ST7565

U8 code LCM_InitCommand[]={0xe2,0xa2,0xa0,0xc8,0xa6,0xa4,0x25,0x81,0x10,0x2f,0x40,0xaf};//ST7565
void LCM_Init(void)
{
	U8 i;

    XBYTE[0xB039] |= 0x04;//Gpio10  output enable
    XBYTE[0xB102]|=0x10;//P1_4  output

	LCM_RESET_LO;
//	LCM_RESET=0;
	USER_DelayDTms(20);
//	LCM_RESET=1;
    LCM_RESET_HI;

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
		LCM_erase_one_page(i);	
}


void LCM_clear_dir(void)  //20090216 chiayen add 
{	
	U8 i;

	for (i=2; i<8; i++)
		LCM_erase_one_page(i);	
}


void LCM_Disp_PCConnect()  //for display USB
{
	LCM_clear();
	LCM_Disp_PC();
	LCM_Disp_MyMP3Logo();
	LCM_Disp_USBLine();
}


void LCM_Disp_PC()  //for display USB
{
	U8  code X_C_PC[]={4,24,24,86,0,2};//offset3:start_column,offset5:start_page

	LCM_Disp_XBMPArray_USB(X_C_PC,LCM_BmpPC);
}


void LCM_Disp_MyMP3Logo()	//for dispale USB
{
	U8  code X_C_MyMP3Logo[]={4,38,38,8,0,2};//offset3:start_column,offset5:start_page

	LCM_Disp_XBMPArray_USB(X_C_MyMP3Logo,LCM_BmpMyMP3Logo);
}


#if 0
void LCM_ShowFMIcon(void)  //no use
{
	U8	i;
	U16	j;

	for(i=0;i<2;i++)
	{
//		if(gc_LCM_Size==0) // 96x32
		{
			LCM_set_address(i,48+(U8)gb_LCM_reverse*LCM_ReverseOffset);
			for(j=0;j<14;j++)
			{
				LCM_write_data(FM_Icon96x32[j+i*14]);
			}
		}
//		else	// 128x32  //un_finish
//		{
//		}
	}
}
#endif

void LCM_ShowWait()
{
	U8	tc_column;
	gc_LCM_line=3;
	tc_column=26;
	LCM_ShowChar8x16(35,tc_column);	// W
	tc_column+=12;
	LCM_ShowChar8x16(39,tc_column);	// a
	tc_column+=12;
	LCM_ShowChar8x16(47,tc_column);	// i
	tc_column+=12;
	LCM_ShowChar8x16(58,tc_column);	// t
	tc_column+=12;
	LCM_ShowChar8x16(65,tc_column);	// .
	tc_column+=12;
	LCM_ShowChar8x16(65,tc_column);	// .
	tc_column+=12;
	LCM_ShowChar8x16(65,tc_column);	// .
	gc_LCM_line=0;
}


void LCD_Disp_NoFile()  //20090107 chiayen add
{
	U8	tc_column;
	gc_LCM_line=3;//3
	tc_column=26;
	LCM_ShowChar8x16(26,tc_column);	// N
	tc_column+=12;
	LCM_ShowChar8x16(53,tc_column);	// o
	tc_column+=12;
	LCM_ShowChar8x16(12,tc_column);	// 
	tc_column+=12;
	LCM_ShowChar8x16(18,tc_column);	// F
	tc_column+=12;
	LCM_ShowChar8x16(47,tc_column);	// i
	tc_column+=12;
	LCM_ShowChar8x16(50,tc_column);	// l
	tc_column+=12;
	LCM_ShowChar8x16(43,tc_column);	// e
	gc_LCM_line=0;
}











