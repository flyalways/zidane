#include "..\header\SPDA2K.h"
#include "..\header\define.h"
#include "..\header\Rambase.h"
#include "..\header\DSP.h"
#include "dspphysic.h"


void L2_DSP_Write_DMem24(U16 tw_Addr,U32 tdw_Data)
{
    XBYTE[0xBE40]=LOBYTE(tw_Addr);
    XBYTE[0xBE41]=HIBYTE(tw_Addr);
    XBYTE[0xBE42]=*((U8 *)&tdw_Data+3);
    XBYTE[0xBE43]=*((U8 *)&tdw_Data+2);
    XBYTE[0xBE44]=*((U8 *)&tdw_Data+1);
    XBYTE[0xBE45]=0x01;        //write. after R/W, the addr will be incremented
    L2_DSP_DM_Triger(C_DSP_WriteMemError);
}


void L2_DSP_MCU_IM_DMA(U8 BufferIndex, U16 tw_Src)
{
   	xdata	U16 tw_DSPAddr;
	data	U16 tw_DSP_Word_Align;

	tw_DSPAddr= (U16)(BufferIndex<<9);

	// ----- DMA Reset -----
	XBYTE[0xB304]=0x09;
	XBYTE[0xB304]=0x08;
	XBYTE[0xB3C0]=0x00;		// clear DMA complete
	XBYTE[0xB330]=0x01;		// clear checksum
	
	// ----- DMA initial settings for IM transfer -----
	XBYTE[0xB216]=0x04;	// Enable IM DMA
	XBYTE[0xB301]=0x30;	// DMA : SRAM --> IM

	// Define data size in DMA transfer =512 byte (0xB302, 0xB303)
	XBYTE[0xB302]=0xFF;
	XBYTE[0xB303]=0x01;

	XBYTE[0xB340]=0x00;	// 24-bit DMA

	// Destination
	tw_DSP_Word_Align = tw_DSPAddr/3;
	XBYTE[0xB21F]=tw_DSPAddr%3;		// offset
	XBYTE[0xB21B]=(tw_DSP_Word_Align<<8)>>8;
	XBYTE[0xB21C]=(tw_DSP_Word_Align)>>8;
	// Source
	XBYTE[0xB112]=(tw_Src<<8)>>8;
	XBYTE[0xB113]=(tw_Src>>8);

	// ------------------------------------------------

	XBYTE[0xB3B0]=0x01;
	while(!(XBYTE[0xB3C0]&0x01));
	XBYTE[0xB3C0]=0x00;
	XBYTE[0xB216]=0x00;  //Enable DM,IM,PM
}

void L2_DSP_MCU_PM_DMA(U8 BufferIndex, U16 tw_Src)
{
   	xdata	U16 tw_DSPAddr;
	data	U16 tw_DSP_Word_Align;

	tw_DSPAddr= (U16)(BufferIndex<<9);

	// ----- DMA Reset -----
	XBYTE[0xB304]=0x09;
	XBYTE[0xB304]=0x08;
	XBYTE[0xB3C0]=0x00;		// clear DMA complete
	XBYTE[0xB330]=0x01;		// clear checksum

	// ----- DMA initial settings for PM transfer -----
	XBYTE[0xB216]=0x02;	// Enable PM DMA
	XBYTE[0xB301]=0x40;	// DMA : SRAM --> PM

	// Define data size in DMA transfer=512 byte (0xB302, 0xB303)
	XBYTE[0xB302]=0xFF;
	XBYTE[0xB303]=0x01;

	XBYTE[0xB340]=0x00;	// 24-bit DMA

	// Destination
	tw_DSP_Word_Align = tw_DSPAddr/3;
	XBYTE[0xB21E]=tw_DSPAddr%3;	// offset
	XBYTE[0xB219]=(tw_DSP_Word_Align<<8)>>8;
	XBYTE[0xB21A]=(tw_DSP_Word_Align)>>8;
	// Source
	XBYTE[0xB112]=(tw_Src<<8)>>8;
	XBYTE[0xB113]=(tw_Src>>8);
		
	XBYTE[0xB3B0]=0x01;	// DMA Run (0xB3B0)
	while(!(XBYTE[0xB3C0]&0x01));
	XBYTE[0xB3C0]=0x00;
	XBYTE[0xB216]=0x00;  //Enable DM,IM,PM
}


