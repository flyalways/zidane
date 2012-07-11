#include "..\Header\SPDA2K.h"
#include "..\header\UI_config.h"
#include "..\header\variables.h"
#include "LCM_BMP.h"


void LCM_ShowVOL(void)
{
	data	U8	VOL;

	LCM_ShowChar12x32(28,28);	// V
	LCM_ShowChar12x32(23,40);	// O
	LCM_ShowChar12x32(20,52);	// L
	VOL=gs_DSP_GLOBAL_RAM.sc_Volume/2;
	LCM_ShowChar12x32(VOL/10,74);
	LCM_ShowChar12x32(VOL%10,86);
	gc_ShowTimer=67; 
}


void LCM_ShowSongNumber(void)	// 12x32
{
	LCM_ShowChar12x32(gw_FileIndex[0]/1000,12);
	LCM_ShowChar12x32((gw_FileIndex[0]%1000)/100,24);
	LCM_ShowChar12x32((gw_FileIndex[0]%100)/10,36);
	LCM_ShowChar12x32(gw_FileIndex[0]%10,48);
	LCM_ShowChar12x32(10,60);	// "/"
	LCM_ShowChar12x32(gw_FileTotalNumber[0]/1000,72);
	LCM_ShowChar12x32((gw_FileTotalNumber[0]%1000)/100,84);
	LCM_ShowChar12x32((gw_FileTotalNumber[0]%100)/10,96);
	LCM_ShowChar12x32(gw_FileTotalNumber[0]%10,108);
}

void LCM_ShowPlayModeIcon(void)	// Play mode small icon
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=PlayModeIcon11x16+gc_RepPlayMode*22;
	LCM_ReadSPIBMPdata(tw_Addr,22);

	for(i=0;i<2;i++)
	{
		LCM_set_address(i,12);
		for(j=0;j<11;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[j+i*11]);
		}
	}
}


void LCM_ShowEQ(void)
{
#if 0
	data	U8 tc_word,i;
	data	U8 tc_column;
	data	U8 tc_EQ_table[8];

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
		case 3:	// ROCK
			tc_word=4;
			tc_EQ_table[0]=25;
			tc_EQ_table[1]=23;
			tc_EQ_table[2]=13;
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
	tc_column+=16;
	for(i=0;i<tc_word;i++)
	{
		LCM_ShowChar12x32(tc_EQ_table[i],tc_column);		
		tc_column+=12;		
	}
#endif
}


void LCM_ShowERROR(void)
{
	LCM_Clear_L2_L5();
	LCM_ShowChar12x32(15,34);		
	LCM_ShowChar12x32(25,46);		
	LCM_ShowChar12x32(25,58);		
	LCM_ShowChar12x32(23,70);		
	LCM_ShowChar12x32(25,82);		
}


void LCM_ShowEQIcon(void)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=EQIcon19x16+gs_DSP_GLOBAL_RAM.sc_EQ_Type*38;
	LCM_ReadSPIBMPdata(tw_Addr,38);
	for(i=0;i<2;i++)
	{
		LCM_set_address(i,54);
		for(j=0;j<19;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[i*19+j]);
		}
	}
}

void LCM_Show_FM_CHNUM(U8 chnum)
{
	LCM_ShowChar8x16(15,30);	// C
	LCM_ShowChar8x16(20,42);	// H
	LCM_ShowChar8x16(chnum/10,54);	// num
	LCM_ShowChar8x16(chnum%10,66);	// num
}

void LCM_erase_one_page(U8 page)
{
	xdata	U8 i;

	LCM_set_address(page, 0);
	for(i=132;i;i--)
	{
		LCM_write_data(0);
	}
}


void LCM_disp_HZKCharBMP(U8 Page, U8 Column,U8 *BMPdataBuf,U8 CharOrWord,U8 reverse)
{
	data	U8	i;
	data	U8	j;

	i=0;
	if(CharOrWord)//char(8x16)--------word(12x16)
	{
		j=31;
	}
	else
	{
		j=15;
	}
	LCM_set_address(Page,Column);

	for(;i<j;i=i+2)
	{
		if(reverse)
		{
			LCM_write_data(~*(BMPdataBuf+(i)));
		}
		else
		{
			LCM_write_data(*(BMPdataBuf+(i)));
		}
	}

	i=1;
	if(CharOrWord)
	{
		j=32;
	}
	else
	{
		j=16;
	}

	LCM_set_address(Page+1,Column);
	
	for(;i<j;i=i+2)
	{
		if(reverse)
		{
			LCM_write_data(~*(BMPdataBuf+(i)));
		}
		else
		{
			LCM_write_data(*(BMPdataBuf+(i)));
		}
	}
}


void LCM_DisplayString(U8 *tpc_DataBuf, U16 tw_nByte,U8 tc_RowAddr)
{	
	data	U8	tc_ColumnEnd;
	data	U8	temp_RowAddr;
	data	U8	tc_ColumnAddr;
	data	U8	tc_ColumnNum;
	data	U8	tc_temp;
	data	U16 tw_Loop;
	xdata	U8	Tmp_DataBuf[32];

	temp_RowAddr=tc_RowAddr;
	tc_ColumnEnd=128;
	tc_ColumnAddr=0;
	tc_temp=0;
	tw_Loop=0;
  memset(Tmp_DataBuf,0,32);
	while(tw_Loop<tw_nByte)
	{
		tc_ColumnNum=LCM_UNICODE_HZK_GET_BMP(*(tpc_DataBuf+tw_Loop),*(tpc_DataBuf+tw_Loop+1),Tmp_DataBuf,0);

		if(*(tpc_DataBuf+tw_Loop)&0x80)
		{
			tc_temp=16;
		}
		else
		{
			tc_temp=8;
		}

		if((tc_ColumnAddr+tc_temp)>tc_ColumnEnd)
		{
			LCM_FillColumn(tc_RowAddr/8,tc_ColumnAddr,tc_ColumnEnd);
			if(tc_RowAddr+16>=temp_RowAddr+32) 
			{
				break;
			}
			tc_ColumnAddr=0;
			tc_RowAddr+=16;
		}
		if((tc_ColumnNum&0x7f)>8)
		{
			LCM_disp_HZKCharBMP(tc_RowAddr/8,tc_ColumnAddr,Tmp_DataBuf,1,0);
		}
		else
		{
			LCM_disp_HZKCharBMP(tc_RowAddr/8,tc_ColumnAddr,Tmp_DataBuf,0,0);
			if(tc_temp==16)
			{
				LCM_FillColumn(tc_RowAddr/8,tc_ColumnAddr+8,tc_ColumnAddr+16);
			}
		}
		tc_ColumnAddr+=tc_temp;
		tw_Loop++;
		if(tc_ColumnNum&0x80)
		{
			tw_Loop++;
		}
	}

	if(tc_ColumnAddr<tc_ColumnEnd)
	{
		LCM_FillColumn(tc_RowAddr/8,tc_ColumnAddr,tc_ColumnEnd);
	}
	tc_RowAddr+=16;
	while(tc_RowAddr<temp_RowAddr+32)
	{
		LCM_FillColumn(tc_RowAddr/8,0,tc_ColumnEnd);
		tc_RowAddr+=16;
	}
}


void LCM_FillColumn(U8 tc_Page,U8 tc_CurrentColumn,U8 tc_TotalColumn)
{
	data	U8	tc_Width;
	xdata	U8	i;
	data	U8	j;

	tc_Width=tc_TotalColumn-tc_CurrentColumn;
	for(j=0;j<2;j++)
	{
		LCM_set_address(tc_Page+j,tc_CurrentColumn);
		for(i=0;i<tc_Width;i++)
		{		
			LCM_write_data(0);	
		}
	}
}


U8 LCM_Disp_FileName(U8 *DataBuf,U8 tc_ISNOrUnicode,U8 nByte,U8 DispOnOff)
{
	data	U8	i;
	data	U8	Column;
	data	U8	Sts;
	data	U8	tc_ColumnNum;
	data	U8	tc_FirstWorldColumnNum;
	xdata	U8	Tmp_DataBuf[32];
  memset(Tmp_DataBuf,0,32);
	i=0;
	Sts=0;
	Column=0;
	tc_FirstWorldColumnNum=0;

	while(i<nByte)
	{
		if(DispOnOff)
		{
			if(!tc_ISNOrUnicode)
			{
				tc_ColumnNum=LCM_UNICODE_HZK_GET_BMP(*(DataBuf+i+1),*(DataBuf+i),Tmp_DataBuf,1);
			}
			else
			{
				tc_ColumnNum=LCM_UNICODE_HZK_GET_BMP(*(DataBuf+i),*(DataBuf+i+1),Tmp_DataBuf,0);
			}

			if(i==0)
			{
				tc_FirstWorldColumnNum=tc_ColumnNum;
			}
		}			
		if((Column+(tc_ColumnNum&0x7f))>128)
		{
			Sts=1;
			goto DispOver;
		}
		if(DispOnOff)
		{	
			if((tc_ColumnNum&0x7f)>8)
			{
				LCM_disp_HZKCharBMP(4,Column,Tmp_DataBuf,LCM_IsWord,0);
			}
			else
			{
				LCM_disp_HZKCharBMP(4,Column,Tmp_DataBuf,LCM_IsChar,0);
			}
		}
		Column+=(tc_ColumnNum&0x7f);

		i++;
		if(tc_ColumnNum&0x80)
		{
			i++;
		}
	}
	Sts=0;
DispOver:
	while(Column<128 && DispOnOff)
	{
		LCM_set_address(4,Column);
		LCM_write_data(0x00);
		LCM_set_address(4+1,Column);
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
	}
	else
	{
		gw_DispFileName_ByteOffset=0;
	}
	return Sts;//overstep display area
}


void LCM_ShowClock(void)
{
	xdata	U32	tdw_RTC_Second;

	tdw_RTC_Second=gdw_RTC_Second%86400;
	//gdw_RTC_Second
	LCM_ShowChar16x32((tdw_RTC_Second%60)/10,91);	// 10S
	LCM_ShowChar16x32(tdw_RTC_Second%10,108);		// 1S

	if(((tdw_RTC_Second%60)==0)||(gb_LCM_ForceShow==1))
	{
		LCM_ShowChar16x32((((tdw_RTC_Second/60)%60)/10),48);// 10M
		LCM_ShowChar16x32((((tdw_RTC_Second/60)%60)%10),64);// 1M
	}

	if(((tdw_RTC_Second%3600)==0)||(gb_LCM_ForceShow==1))
	{
		LCM_ShowChar16x32(tdw_RTC_Second/36000,4);		// 10H
		LCM_ShowChar16x32((tdw_RTC_Second/3600)%10,21);	// 1H
	}
	gb_LCM_ForceShow=0;
}


void LCM_DisplayMenuString(U8 *tpc_DataBuf, U16 tw_nByte,U8 tc_ColumnAddr,U8 tc_RowAddr,bit tb_Fill)
{	
	data	U8	temp_RowAddr;
	data	U8	tc_ColumnNum;
	data	U8	tc_temp;
	data	U16 tw_Loop;
	xdata	U8	Tmp_DataBuf[32];
 	  memset(Tmp_DataBuf,0,32);
	temp_RowAddr=tc_RowAddr;
	tc_temp=0;
	tw_Loop=0;

	while(tw_Loop<tw_nByte)
	{
#ifdef SPI_1M
		tc_ColumnNum=LCM_UNICODE_HZK_GET_BMP(*(tpc_DataBuf+tw_Loop),*(tpc_DataBuf+tw_Loop+1),Tmp_DataBuf,1);
#else
		tc_ColumnNum=LCM_UNICODE_HZK_GET_BMP(*(tpc_DataBuf+tw_Loop+1),*(tpc_DataBuf+tw_Loop),Tmp_DataBuf,1);
#endif
		tc_temp=tc_ColumnNum&~0x80;

		if(tc_temp>8)
		{
			LCM_disp_HZKCharBMP(tc_RowAddr/8,tc_ColumnAddr,Tmp_DataBuf,1,0);
		}
		else
		{
			LCM_disp_HZKCharBMP(tc_RowAddr/8,tc_ColumnAddr,Tmp_DataBuf,0,0);
		}
		tc_ColumnAddr+=tc_temp;
		tw_Loop+=2;
#ifdef SPI_1M			  //防止显示过长导致该行显示异常的问题  --sunzk add110324
		if(tc_ColumnAddr>=118)
		{
			break;
		}
#else
		if(tc_ColumnAddr>=128)
		{
			break;
		}
#endif
	}
	if(tb_Fill)
	{
		LCM_FillColumn(tc_RowAddr/8,tc_ColumnAddr,128);
	}
}

void LCM_Clear_L0_L7(void)
{	
	data	U8	i;

	for(i=0;i<8;i++)
	{
		LCM_erase_one_page(i);	
	}
}

void LCM_Clear_L0_L1(void)
{
    data	U8 i;

    for(i=0;i<2;i++)
    {
		LCM_erase_one_page(i);
    }
}

void LCM_Clear_L2_L5(void)
{
    data	U8 i;

	for(i=2;i<6;i++)
    {
        LCM_erase_one_page(i);
	}
}

void LCM_Clear_L6_L7(void)
{
    data	U8 i;

	for(i=6;i<8;i++)
    {
        LCM_erase_one_page(i);
	}
}

void LCM_Clear_L2_L7(void)
{
    data	U8 i;

	for(i=2;i<8;i++)
    {
        LCM_erase_one_page(i);
	}
}

void LCM_ShowChar16x32(U8 Num,U8 tc_column)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=DigitIcon16x32+(U16)Num*64;
	LCM_ReadSPIBMPdata(tw_Addr,64);

	for(i=0;i<4;i++)
	{
		LCM_set_address(i+2,tc_column);
		for(j=0;j<16;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[j+i*16]);
		}
	}
}

void LCM_ShowChar12x32(U8 Num,U8 tc_column)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=CharIcon12x32+(U16)Num*48;
	LCM_ReadSPIBMPdata(tw_Addr,48);

	for(i=0;i<4;i++)
	{
		LCM_set_address(i+2,tc_column);
		for(j=0;j<12;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[j+i*12]);
		}
	}
}

void LCM_ShowChar10x16(U8 Num,U8 tc_column)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=DigitIcon10x16+(U16)Num*20;
	LCM_ReadSPIBMPdata(tw_Addr,20);
	for(i=0;i<2;i++)
	{
		LCM_set_address(i+gc_LCM_line,tc_column);
		for(j=0;j<10;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[j+i*10]);
		}
	}
}

void LCM_ShowChar5x8(U8 Num,U8 tc_column)
{
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=BitrateNum5x8+(U16)Num*5;
	LCM_ReadSPIBMPdata(tw_Addr,5);
	LCM_set_address(0,tc_column);
	for(j=0;j<5;j++)
	{
		LCM_write_data(gc_DirLCMtempBuf[j]);
	}
}

void LCM_ShowChar8x16(U8 Num,U8 tc_column)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=CharIcon8x16+(U16)Num*16;
	LCM_ReadSPIBMPdata(tw_Addr,16);
	for(i=0;i<2;i++)
	{
		LCM_set_address(i+gc_LCM_line,tc_column);
		for(j=0;j<8;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[j+i*8]);
		}
	}
}

void LCM_ShowChar6x8(U8 Num,U8 tc_column)
{
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=SongNum6x8+Num*6;
	LCM_ReadSPIBMPdata(tw_Addr,6);
	LCM_set_address(gc_LCM_line,tc_column);
	for(j=0;j<6;j++)
	{
		LCM_write_data(gc_DirLCMtempBuf[j]);
	}
}


void LCM_ShowBattIcon(void)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=BatteryIcon16x16+gc_BatteryLevel*32;
	LCM_ReadSPIBMPdata(tw_Addr,32);
	for(i=0;i<2;i++)
	{
		LCM_set_address(i,111);
		for(j=0;j<16;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[j+i*16]);
		}
	}
}

void LCM_ShowBitRate(void)
{
	LCM_ShowChar5x8(gs_System_State.w_SampleRate/100,77);
	LCM_ShowChar5x8(gs_System_State.w_SampleRate%100/10,83);
	LCM_ShowChar5x8(gs_System_State.w_SampleRate%10,89);
}

void LCM_ShowMusicType(void)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=MusicTypeIcon23x16+gc_Play_FileType*46;
	LCM_ReadSPIBMPdata(tw_Addr,46);
	for(i=0;i<2;i++)
	{
		LCM_set_address(i,74);
		for(j=0;j<23;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[i*23+j]);
		}
	}
}

void LCM_ShowSongNum(void)
{
	gc_LCM_line=0;
	LCM_ShowChar6x8(gw_FileIndex[0]/1000,25);
	LCM_ShowChar6x8((gw_FileIndex[0]/100)%10,32);
	LCM_ShowChar6x8((gw_FileIndex[0]/10)%10,39);
	LCM_ShowChar6x8(gw_FileIndex[0]%10,46);
}

void LCM_ShowSongTotalNum(void)
{
	gc_LCM_line=1;
	LCM_ShowChar6x8(gw_FileTotalNumber[0]/1000,25);
	LCM_ShowChar6x8((gw_FileTotalNumber[0]/100)%10,32);
	LCM_ShowChar6x8((gw_FileTotalNumber[0]/10)%10,39);
	LCM_ShowChar6x8(gw_FileTotalNumber[0]%10,46);
}

void LCM_ShowSongTime(void)
{
	gc_LCM_line=2;
	LCM_ShowChar8x16(10,37);	// :
	LCM_ShowChar8x16(68,60);	// /
	if(gw_TotalSec>=3600)
	{
		LCM_ShowChar8x16(10,16);	// :
		LCM_ShowChar8x16(10,79);	// :
		LCM_ShowChar8x16(10,101);	// :
		LCM_ShowChar8x16(gw_TotalSec/3600,70);		// 1H
		LCM_ShowChar8x16((gw_TotalSec%3600)/600,84);// 10M
		LCM_ShowChar8x16((gw_TotalSec/60)%10,92);	// 1M
		LCM_ShowChar8x16((gw_TotalSec%60)/10,106);	// 10S
		LCM_ShowChar8x16(gw_TotalSec%10,114);		// 1S
	}
	else
	{
		LCM_ShowChar8x16(10,88);	// :
		LCM_ShowChar8x16((gw_TotalSec%3600)/600,70);// 10M
		LCM_ShowChar8x16((gw_TotalSec/60)%10,78);	// 1M
		LCM_ShowChar8x16((gw_TotalSec%60)/10,92);	// 10S
		LCM_ShowChar8x16(gw_TotalSec%10,100);		// 1S	
	}
}

void LCM_ShowPlayTime(U16 time)
{
	gc_LCM_line=2;
	if(gw_TotalSec>=3600)
	{
		LCM_ShowChar8x16(time/3600,6);	// 1H
	}
	LCM_ShowChar8x16((time/600)%10,19);	// 10M
	LCM_ShowChar8x16((time/60)%10,28);	// 1M
	LCM_ShowChar8x16((time%60)/10,42);	// 10S
	LCM_ShowChar8x16(time%10,50);		// 1S
}

void LCM_ShowRECTime(U16 time)
{
	data	U8	i;

	gc_LCM_line=2;
	LCM_ShowChar8x16(10,51);
	LCM_ShowChar8x16(10,75);
	i=time/36000;
	LCM_ShowChar8x16(i,32);	// 10H
	time=time-i*36000;
	i=time/3600;
	LCM_ShowChar8x16(i,40);	// 1H
	time=time-i*3600;
	i=time/600;
	LCM_ShowChar8x16(i,56);	// 10M
	time=time-i*600;
	i=time/60;
	LCM_ShowChar8x16(i,64);	// 1M
	time=time-i*60;
	i=time/10;
	LCM_ShowChar8x16(i,80);	// 10S
	i=time%10;
	LCM_ShowChar8x16(i,88);	// 1S
}

void LCM_ShowRECRemainTime(U32 time)
{
	data	U8	i;

	gc_LCM_line=4;
	LCM_ShowChar8x16(10,51);
	LCM_ShowChar8x16(10,75);
	i=time/360000;
	LCM_ShowChar8x16(i,24);	// 100H
	time=time-i*360000;
	i=time/36000;
	LCM_ShowChar8x16(i,32);	// 10H
	time=time-i*36000;
	i=time/3600;
	LCM_ShowChar8x16(i,40);	// 1H
	time=time-i*3600;
	i=time/600;
	LCM_ShowChar8x16(i,56);	// 10M
	time=time-i*600;
	i=time/60;
	LCM_ShowChar8x16(i,64);	// 1M
	time=time-i*60;
	i=time/10;
	LCM_ShowChar8x16(i,80);	// 10S
	i=time%10;
	LCM_ShowChar8x16(i,88);	// 1S
}


void LCM_ShowPinPu(void)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	if(gb_LrcFileName_Exist==1)
	{
		return;
	}
	tw_Addr=PinPu128x16+(U16)gc_PinPuCount*256;
	LCM_ReadSPIBMPdata(tw_Addr,256);
	gc_PinPuTimer=8;
	for(i=0;i<2;i++)
	{
		LCM_set_address(i+6,0);
		for(j=0;j<128;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[i*128+j]);
		}
	}	
}

void LCM_ShowPlayPauseIcon(void)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=PlayPauseIcon10x16;
	if(gs_System_State.c_Phase==TASK_PHASE_PAUSE||gs_System_State.c_Phase==TASK_PHASE_STOP)
	{
		tw_Addr+=22;
	}
	LCM_ReadSPIBMPdata(tw_Addr,22);
	for(i=0;i<2;i++)
	{
		LCM_set_address(i,0);
		for(j=0;j<11;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[i*11+j]);
		}
	}
}

void LCM_ShowRepeatIcon(void)
{
	data	U8	i;
	xdata	U8	j;
	xdata	U16	tw_Addr;

	tw_Addr=RepeatIcon19x16;
	if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_AB_NULL)
	{
		tw_Addr+=38*3;
	}
	else if(gb_FlashRepeatIcon==0)
	{
		tw_Addr+=38*2;
	}
	else
	{
		if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_A)
		{
			tw_Addr+=38;
		}
		else if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode==REPEAT_AB)
		{
			return;
		}
	}

	LCM_ReadSPIBMPdata(tw_Addr,38);
	for(i=0;i<2;i++)
	{
		LCM_set_address(i,29);
		for(j=0;j<19;j++)
		{
			LCM_write_data(gc_DirLCMtempBuf[i*19+j]);
		}
	}
}

void LCM_ShowRecordStartTime_Hour(void)
{
	gc_LCM_line=2;
	LCM_ShowChar8x16(gdw_OrderRecordTime/36000,70);			// 10H
	LCM_ShowChar8x16((gdw_OrderRecordTime/3600)%10,78);		// 1H
}

void LCM_ShowRecordStartTime_Minute(void)
{
	gc_LCM_line=2;
	LCM_ShowChar8x16((((gdw_OrderRecordTime/60)%60)/10),90);// 10M
	LCM_ShowChar8x16((((gdw_OrderRecordTime/60)%60)%10),98);// 1M
}

void LCM_ShowAlarmTime_Hour(void)
{
	gc_LCM_line=2;
	LCM_ShowChar8x16(gdw_AlarmTime/36000,70);			// 10H
	LCM_ShowChar8x16((gdw_AlarmTime/3600)%10,78);		// 1H
}

void LCM_ShowAlarmTime_Minute(void)
{
	gc_LCM_line=2;
	LCM_ShowChar8x16((((gdw_AlarmTime/60)%60)/10),90);// 10M
	LCM_ShowChar8x16((((gdw_AlarmTime/60)%60)%10),98);// 1M
}

void LCM_ShowDuration(void)
{
	gc_LCM_line=4;
	LCM_ShowChar8x16(gw_OrderRecDuration/6000,70);// 100M
	LCM_ShowChar8x16((gw_OrderRecDuration%6000)/600,78);// 10M
	LCM_ShowChar8x16(0,86);// 1M
}
