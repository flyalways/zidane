/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	dspphysic.h
 *	\brief	Functions for MCU-DSP communication channels.
 *
 *	\author joseph.hsieh
 */

#ifndef	_DSPPHYSIC_H
#define _DSPPHYSIC_H


//============================================================
// Useful Macro
//============================================================
#define HIBYTE(var)	(*(unsigned char*)&var)
#define LOBYTE(var)	(*((unsigned char*)&var+1))

//============================================================
//Constant Define
//============================================================
enum {
	DMA_24BIT_MODE = 0,
	DMA_16BIT_MODE = 1
};

//==============================================================
// Function Prototype
//==============================================================
U16  L2_DSP_Read_DMem16(U16 tw_Addr);
U32  L2_DSP_Read_DMem24(U16 tw_Addr);
void L2_DSP_Write_DMem16(U16 tw_Addr,U16 tw_Data);
void L2_DSP_Write_DMem24(U16 tw_Addr,U32 tdw_Data);
U8   L2_DSP_SendCommandSet(U8 tc_Cmd);
void L2_DSP_MCU_IM_DMA(U8 BufferIndex, U16 tw_Src);
void L2_DSP_MCU_PM_DMA(U8 BufferIndex, U16 tw_Src);
void L2_DSP_MCU_DM_DMA(U16 BufferIndex,U8 tc_Dest_Addr_High, U16 tw_Src, U8 tc_Codec_Mode);

void L2_DSP_DM_MCU_DMA(U16 BufferIndex,U8 tc_PowerOf2, U16 tw_Src, U8 tc_Codec_Mode);

void DSP_Write_MMR16(U16 tw_Addr,U16 tw_Data); 
U16 DSP_Read_MMR16(U16 tw_Addr); 
#endif	// _DSPPHYSIC_H
