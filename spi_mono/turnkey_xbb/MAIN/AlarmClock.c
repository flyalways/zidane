#include "..\header\SPDA2K.h"
#include "..\header\UI_config.h"
#include "..\header\variables.h"

void AlarmMusic(void)
{
	data	U8	tc_timeout;

	gb_AlarmTrigger=0;
	gw_AlarmClockTimer=300;	// Alarm clock sound duration(Unit:Sec)
	MP3_Download();	// Download DSP code
	L2_DSP_Write_DMem16(DSP_PostProcessSelect,L2_DSP_Read_DMem16(DSP_PostProcessSelect)|0x0020);
	tc_timeout=0xFF;
	while((L2_DSP_Read_DMem16(DSP_PostProcessSelect)&0x0020))  //Handshake with DSP to make sure that DSP has ramp digital volume up.
	{
		if(!(tc_timeout--))
		{
			break;
		}
	}

	Music_PlayCmd();
	L2_DSP_Write_DMem16(DSP_VolumeControl,25);	// Set Volume
	gc_GetSPIdataSectorNumber=0;
	while(1)
	{
		if(L2_DSP_Read_DMem16(DSP_EmptyBuffer)>511)
		{
			AlarmMusic_DataIn();
		}

		Key_Detect();
		if(gc_KeyEvent==2||gw_AlarmClockTimer==0)
		{
			Music_StopCmd(0);
			gb_Mp3FileEnd=1;
			gc_KeyEvent=0;
			break;
		}
	}
	gc_Task_Current=gc_Task_Last;
}

void AlarmMusic_DataIn(void)
{
	xdata	U8  tc_DSPAddr_High;
	data	U16 tw_DSP_Word_Align;
	xdata	U16	tw_SmpRateIdx;
	code	U16	gw_DSP_Addr_Table[3]={0x0000,0x80aa,0x4155};

	GetAlarmData();
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
	XBYTE[0xB340]=0x00;	// 24-bit DMA
	tw_DSP_Word_Align=gw_DSP_Addr_Table[gs_DSP_GLOBAL_RAM.sc_DM_Index%3]+0x200*(gs_DSP_GLOBAL_RAM.sc_DM_Index/3);
	tc_DSPAddr_High=*((U8 *)&tw_DSP_Word_Align);
	XBYTE[0xB21D]=(U8)(tc_DSPAddr_High>>6);
	tc_DSPAddr_High=*((U8 *)&tw_DSP_Word_Align);
	tc_DSPAddr_High=tc_DSPAddr_High&0x3F;
	tc_DSPAddr_High+=0x20;
	XBYTE[0xB217]=*((U8 *)&tw_DSP_Word_Align+1);
	XBYTE[0xB218]=tc_DSPAddr_High;
	// Source
	XBYTE[0xB112]=(U8)gc_PlayRecordDataBuf;
	XBYTE[0xB113]=gc_PlayRecordDataBufHB;
	Trigger_Data_Transfer_Done();

	L2_DSP_SendCommandSet(DCMD_DatIn);
	gs_DSP_GLOBAL_RAM.sc_DM_Index++;
	if(gs_DSP_GLOBAL_RAM.sc_DM_Index==24)
	{
		gs_DSP_GLOBAL_RAM.sc_DM_Index=0;
	}

	gc_GetSPIdataSectorNumber++;
	if(gc_GetSPIdataSectorNumber==C_AlarmDataSize)
	{
		gc_GetSPIdataSectorNumber=0;
	}

    tw_SmpRateIdx=L2_DSP_Read_DMem16(0x3F8E);	//0x3F8E is an index of mp3 sampling rate within DSP
                                                                                                                //0x0:44.1k;  0x1:48k;  0x2:32k;  0x3:22.1k;  0x4:24k;  0x5:16k;  0x6:11.025k;  0x7:12k;  0x8:8k
    if(tw_SmpRateIdx==0||tw_SmpRateIdx==3||tw_SmpRateIdx==6)
	{
		GLOBAL_REG[0x46]|=0x04;
	}
    else
	{
		GLOBAL_REG[0x46]&=0xFB; 
	}
}

void GetAlarmData(void)
{
	unionU16	tdw_SPIAddr;
#ifdef SPI_1M
	Device_Read_SRAM_Index.BY[0]=gc_PlayRecordDataBufHB;
	Device_Read_SRAM_Index.BY[1]=0x00;
	tdw_SPIAddr.BY[0]=(0xC2+gc_GetSPIdataSectorNumber*2)%256; // 1-page=256-byte
	tdw_SPIAddr.BY[1]=0x0B+(0xC2+gc_GetSPIdataSectorNumber*2)/256;
	SPI_Read_Data(tdw_SPIAddr);	
#else
	Device_Read_SRAM_Index.BY[0]=gc_PlayRecordDataBufHB;
	Device_Read_SRAM_Index.BY[1]=0x00;
	tdw_SPIAddr.BY[0]=(0x2E+gc_GetSPIdataSectorNumber*2)%256; // 1-page=256-byte
	tdw_SPIAddr.BY[1]=0x1F+(0x2E+gc_GetSPIdataSectorNumber*2)/256;
	SPI_Read_Data(tdw_SPIAddr);
#endif
}

