#include "SPDA2K.h"
#include "define.h"
#include "Rambase.h"
#include "DSP.h"
#include "dspphysic.h"


//extern	U8		L2_DSP_DM_Triger(U8 tc_DSP_ErrorStatus);
//extern	U8		L2_DSP_SendCommandSet(U8 tc_Cmd);
//extern	U16		DSP_Read_MMR16(U16 tw_Addr);
//extern	U16		L2_DSP_Read_DMem16(U16 tw_Addr);
//extern	U32		L2_DSP_Read_DMem24(U16 tw_Addr);
//extern	void	L2_DSP_Write_DMem16(U16 tw_Addr,U16 tw_Data);
//extern	void	DSP_Write_MMR16(U16 tw_Addr,U16 tw_Data);

//================= Private Fucntion =================
static U8 L2_DSP_DM_Triger(U8 tc_DSP_ErrorStatus)
{
	U16 TIMEOUT=0xFFFF;

	do
	{
		TIMEOUT--;
		if(TIMEOUT==0)
		{
			return tc_DSP_ErrorStatus;
		}
	}while(DSP3_REG[0x45]);

	return C_DSP_NoError;
}

//================= Public Fucntion =================
//		DM Read/Write
//===================================================
U16  L2_DSP_Read_DMem16(U16 tw_Addr)
{
	union WORDDataType mwTemp;

	DSP3_REG[0x40] = LOBYTE(tw_Addr);
	DSP3_REG[0x41] = HIBYTE(tw_Addr);
	DSP3_REG[0x45] = 0x02;     //read, after R/W, the addr will be increment

	L2_DSP_DM_Triger(C_DSP_ReadMemError);

	mwTemp.BY[0]=DSP3_REG[0x43];
    mwTemp.BY[1]=DSP3_REG[0x42];

    return(mwTemp.WD);
}

U32  L2_DSP_Read_DMem24(U16 tw_Addr)
{
	union LWDataType mdwTemp;

    DSP3_REG[0x40] = LOBYTE(tw_Addr);
  	DSP3_REG[0x41] = HIBYTE(tw_Addr);
    DSP3_REG[0x45] = 0x02;     //read, after R/W, the addr will be increment

   	L2_DSP_DM_Triger(C_DSP_ReadMemError);

    mdwTemp.BY[0] = 0;
    mdwTemp.BY[1] = DSP3_REG[0x44];
    mdwTemp.BY[2] = DSP3_REG[0x43];
    mdwTemp.BY[3] = DSP3_REG[0x42];

    return(mdwTemp.LW);
}

void L2_DSP_Write_DMem16(U16 tw_Addr,U16 tw_Data)
{
    //GPIO_7_H;
    DSP3_REG[0x42] = LOBYTE(tw_Data);
    DSP3_REG[0x43] = HIBYTE(tw_Data);
    DSP3_REG[0x44] = 0;

    DSP3_REG[0x40] = LOBYTE(tw_Addr);
    DSP3_REG[0x41] = HIBYTE(tw_Addr);

    DSP3_REG[0x45] = 0x01;        //write. after R/W, the addr will be incremented
    L2_DSP_DM_Triger(C_DSP_WriteMemError);
}


//===================================================
//		Interrupt Trigger
//===================================================
U8   L2_DSP_SendCommandSet(U8 tc_Cmd)
{
	U16 TIMEOUT=0xFFFF;
	DSP3_REG[0x20] = tc_Cmd;
   	do
  	{
		#ifdef D_DspTimeout //20090420 chiayen add
    	TIMEOUT--;
      	if(TIMEOUT==0)
		{
			DSP3_REG[0x24] = 0x00;
			return C_DSP_SendCMDError;
	   	}
		#endif
   	}while((DSP3_REG[0x24] & 0x02));	//The Command has been read by DSP

  	TIMEOUT=0xFFFF;
  	do
  	{
		#ifdef D_DspTimeout //20090420 chiayen add
    	TIMEOUT--;
      	if(TIMEOUT==0)
		{
			DSP3_REG[0x24] = 0x00;
			return C_DSP_SendCMDError;
	   	}
		#endif
   	}while(!(DSP3_REG[0x24] & 0x01));	//The Command has been response by DSP
	DSP3_REG[0x24] = 0x00;

  	return(DSP3_REG[0x22]);
}

void L2_DSP_Write_DMem24(U16 tw_Addr,U32 tdw_Data)
{
    DSP3_REG[0x40] = LOBYTE(tw_Addr);
    DSP3_REG[0x41] = HIBYTE(tw_Addr);
    DSP3_REG[0x42] =*((U8 *)&tdw_Data + 3);
    DSP3_REG[0x43] =*((U8 *)&tdw_Data + 2);
    DSP3_REG[0x44] =*((U8 *)&tdw_Data + 1);
    DSP3_REG[0x45]=0x01;        //write. after R/W, the addr will be incremented
    L2_DSP_DM_Triger(C_DSP_WriteMemError);
}

//===================================================
//		DMA Transfer
//===================================================
void L2_DSP_MCU_IM_DMA(U8 BufferIndex, U16 tw_Src)
{
   	U16 tw_DSPAddr;
	U16 tw_DSP_Word_Align;

	tw_DSPAddr= (U16)(BufferIndex<<9);

	// ----- DMA Reset -----
	DMA_REG[0x04] = 0x09;
	DMA_REG[0x04] = 0x08;
	DMA_REG[0xC0] = 0x00;		// clear DMA complete
	DMA_REG[0x30] = 0x01;		// clear checksum
	
	// ----- DMA initial settings for IM transfer -----
	DSP_REG[0x16] = 0x04;	// Enable IM DMA
	DMA_REG[0x01] = 0x30;	// DMA : SRAM --> IM

	// Define data size in DMA transfer =512 byte (0xB302, 0xB303)
	DMA_REG[0x02] = 0xFF;
	DMA_REG[0x03] = 0x01;

	DMA_REG[0x40] = 0x00;	// 24-bit DMA

	// Destination
	tw_DSP_Word_Align = tw_DSPAddr/3;
	DSP_REG[0x1F]=	tw_DSPAddr%3;		// offset
	DSP_REG[0x1B] = (tw_DSP_Word_Align<<8)>>8;		// DSP_REG[0x1B] = *((U8 *)&tw_DSP_Word_Align + 1);
	DSP_REG[0x1C] = (tw_DSP_Word_Align)>>8;			// DSP_REG[0x1C] = *((U8 *)&tw_DSP_Word_Align);
	// Source
	CPU_REG[0x12] = (tw_Src<<8)>>8;
	CPU_REG[0x13] = (tw_Src>>8);

	// ------------------------------------------------

	DMA_REG[0xB0] = 0x01;	// DMA Run (0xB3B0)

	// wait until DMA cycle is completed (0xB3C0)
	while(! (DMA_REG[0xC0]&0x01) );
	DMA_REG[0xC0] = 0x00;

	DSP_REG[0xC0] = 0x00;  // Clean 0xB3C0
	DSP_REG[0x16] = 0x00;  //Enable DM,IM,PM
}

void L2_DSP_MCU_PM_DMA(U8 BufferIndex, U16 tw_Src)
{
   	U16 tw_DSPAddr;
	U16 tw_DSP_Word_Align;

	tw_DSPAddr= (U16)(BufferIndex<<9);

	// ----- DMA Reset -----
	DMA_REG[0x04] = 0x09;
	DMA_REG[0x04] = 0x08;
	DMA_REG[0xC0] = 0x00;		// clear DMA complete
	DMA_REG[0x30] = 0x01;		// clear checksum

	// ----- DMA initial settings for PM transfer -----
	DSP_REG[0x16] = 0x02;	// Enable PM DMA
	DMA_REG[0x01] = 0x40;	// DMA : SRAM --> PM

	// Define data size in DMA transfer =512 byte (0xB302, 0xB303)
	DMA_REG[0x02] = 0xFF;
	DMA_REG[0x03] = 0x01;

	DMA_REG[0x40] = 0x00;	// 24-bit DMA

	// Destination
	tw_DSP_Word_Align = tw_DSPAddr/3;
	DSP_REG[0x1E]=	tw_DSPAddr%3;	// offset
	DSP_REG[0x19] = (tw_DSP_Word_Align<<8)>>8;		// DSP_REG[0x19] = *((U8 *)&tw_DSP_Word_Align);		
	DSP_REG[0x1A] = (tw_DSP_Word_Align)>>8;			// DSP_REG[0x1A] = *((U8 *)&tw_DSP_Word_Align + 1);
	// Source
	CPU_REG[0x12] = (tw_Src<<8)>>8;
	CPU_REG[0x13] = (tw_Src>>8);
		
	// ------------------------------------------------

	DMA_REG[0xB0] = 0x01;	// DMA Run (0xB3B0)

	// wait until DMA cycle is completed (0xB3C0)
	while(! (DMA_REG[0xC0]&0x01) );
	DMA_REG[0xC0] = 0x00;
	DSP_REG[0xC0] = 0x00;  // Clean 0xB3C0

	DSP_REG[0x16] = 0x00;  //Enable DM,IM,PM
}

void L2_DSP_MCU_DM_DMA(U16 BufferIndex,U8 tc_Dest_Addr_High, U16 tw_Src, U8 tc_Codec_Mode)
{
   	U16 tw_DSPAddr;
	U16 tw_DSP_Word_Align;
	U8  tc_DSPAddr_High;
	code U16 gw_DSP_Addr_Table[47] = 
	{
		0x0, 0x80aa, 0x4155, 0x200, 0x82aa, 0x4355, 0x400, 0x84aa, 
		0x4555, 0x600, 0x86aa, 0x4755, 0x800, 0x88aa, 0x4955, 0xa00, 
		0x8aaa, 0x4b55, 0xc00, 0x8caa, 0x4d55, 0xe00, 0x8eaa, 0x4f55, 
		0x1000, 0x90aa, 0x5155, 0x1200, 0x92aa, 0x5355, 0x1400, 0x94aa, 
		0x5555, 0x1600, 0x96aa, 0x5755, 0x1800, 0x98aa, 0x5955, 0x1a00, 
		0x9aaa, 0x5b55, 0x1c00, 0x9caa, 0x5d55, 0x1e00, 0x9eaa
	};

	// ----- DMA Reset -----
	DMA_REG[0x04] = 0x09;
	DMA_REG[0x04] = 0x08;
	DMA_REG[0xC0] = 0x00;		// clear DMA complete
	DMA_REG[0x30] = 0x01;		// clear checksum

	// ----- DMA initial settings for DM transfer -----
	// DSP_REG[0x16] = 0x01;	// Enable DM DMA (Error)
	DSP_REG[0x16] = 0x00;	// Remain DSP mode.  When you do MCU -> DM, MCU will stop DSP automaticlly
	DMA_REG[0x01] = 0x50;	// DMA : SRAM --> DM

	// Define data size in DMA (assume 512 bytes)
	DMA_REG[0x02] = 0xFF;
	DMA_REG[0x03] = 0x01;

	// Destination
	switch (tc_Codec_Mode) {
		case DMA_24BIT_MODE:
			DMA_REG[0x40] = 0x00;	// 24-bit DMA

			tw_DSP_Word_Align = gw_DSP_Addr_Table[BufferIndex];
			tc_DSPAddr_High = *((U8 *)&tw_DSP_Word_Align);
			DSP_REG[0x1D]=	(U8)(tc_DSPAddr_High >> 6);	//(U8)((BufferIndexTmp)%3);

			tc_DSPAddr_High = *((U8 *)&tw_DSP_Word_Align);
			tc_DSPAddr_High = tc_DSPAddr_High & 0x3F;
			tc_DSPAddr_High += tc_Dest_Addr_High;

			DSP_REG[0x18] = tc_DSPAddr_High;
			DSP_REG[0x17] = *((U8 *)&tw_DSP_Word_Align + 1);
			break;
		case DMA_16BIT_MODE:
			DMA_REG[0x40] = 0x01;	// 16-bit DMA

			*((U8 *)&tw_DSPAddr) = ((*((U8 *)&BufferIndex + 1)) << 1);
			*((U8 *)&tw_DSPAddr + 1) = 0;

			tw_DSP_Word_Align = tw_DSPAddr >> 1;
			DSP_REG[0x1D]=	0x01;	// offset

			tc_DSPAddr_High = *((U8 *)&tw_DSP_Word_Align);
			tc_DSPAddr_High += tc_Dest_Addr_High;

			DSP_REG[0x18] = tc_DSPAddr_High;
			DSP_REG[0x17] = *((U8 *)&tw_DSP_Word_Align + 1);
			break;
	}
	// Source
	CPU_REG[0x12] = (tw_Src<<8)>>8;
	CPU_REG[0x13] = (tw_Src>>8);
		
	// ------------------------------------------------

	DMA_REG[0xB0] = 0x01;	// DMA Run (0xB3B0)
	// wait until DMA cycle is completed (0xB3C0)
	while((DMA_REG[0xC0]&0x01)==0);
	DMA_REG[0xC0] = 0x00;
	DSP_REG[0xC0] = 0x00;  // Clean 0xB3C0
}

void L2_DSP_DM_MCU_DMA(U16 BufferIndex,U16 tw_Src)
{
	U8  tc_DSPAddr_High;
	U16 tw_MoveSize;

	// ----- DMA Reset -----
	DMA_REG[0x04] = 0x09;
	DMA_REG[0x04] = 0x08;
	DMA_REG[0xC0] = 0x00;		// clear DMA complete
	DMA_REG[0x30] = 0x01;		// clear checksum		
		
	// ----- DMA initial settings for DM transfer -----		
	// DSP_REG[0x16] = 0x01;		//Enable DM DMA	(Error)
	DSP_REG[0x16] = 0x00;	// Remain DSP mode.  When you do MCU -> DM, MCU will stop DSP automaticlly
	DMA_REG[0x01] = 0x05;		// DM -> SRAM

	// Define data size in DMA		
	tw_MoveSize = (1 << tc_PowerOf2) - 1;
	DMA_REG[0x02] = ((U8 *)(&tw_MoveSize))[1];
	DMA_REG[0x03] = ((U8 *)(&tw_MoveSize))[0];

	switch(tc_Codec_Mode)
	{
		case DMA_24BIT_MODE:
			DMA_REG[0x40] = 0x00;	// 24-bit DMA

			DSP_REG[0x1D] = (U8)((BufferIndex<<tc_PowerOf2)%3);
			tc_DSPAddr_High = (U8)( ((BufferIndex<<tc_PowerOf2)/3) >>8 );
			tc_DSPAddr_High += 0x20;	// bitstream start address : 0x2000
			DSP_REG[0x18] = tc_DSPAddr_High;
			DSP_REG[0x17] = (U8)((BufferIndex<<tc_PowerOf2)/3);
			break;
		case DMA_16BIT_MODE:
			DMA_REG[0x40] = 0x01;	// 16-bit DMA

			DSP_REG[0x1D] = 0x01;
			tc_DSPAddr_High = (U8)( ((BufferIndex<<tc_PowerOf2)>>1) >>8 );
			tc_DSPAddr_High += 0x20;	// bitstream start address : 0x2000
			DSP_REG[0x18] = tc_DSPAddr_High;
			DSP_REG[0x17] = (U8)((BufferIndex<<tc_PowerOf2)>>1);
			break;
	}
	// Source
	CPU_REG[0x14] = (U8)( tw_Src);
	CPU_REG[0x15] = (U8)( ( tw_Src) >>8 );

	// ------------------------------------------------

	DMA_REG[0xB0] = 0x01;	// DMA1 start
	// wait until DMA cycle is completed (0xB3C0)		
	while(! (DMA_REG[0xC0]&0x01) );
	DMA_REG[0xC0] = 0x00;
}
//===================================================================
void DSP_Write_MMR16(U16 tw_Addr,U16 tw_Data)
{
	L2_DSP_Write_DMem16(0x3F01, tw_Addr);
	L2_DSP_Write_DMem16(0x3F08, tw_Data);
	L2_DSP_SendCommandSet(0xFD);

}


U16 DSP_Read_MMR16(U16 tw_Addr)
{
	L2_DSP_Write_DMem16(0x3F01, tw_Addr);
	L2_DSP_SendCommandSet(0xF9); 
	return(L2_DSP_Read_DMem16(0x3F08));
}
