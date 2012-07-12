/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	dosrw.h
 *	\brief	Logical sector RW function prototypes.
 *  
 *	\author 
 */

#ifndef	_DOSRW_
#define _DOSRW_
//------------------------------------------------------------------------------------
//extern U8 DOS_tc_AppendWrite_LogicSector(U32 tdw_LogicSectorAddr, U8 tc_WriteSectorNumber);
//extern U8 DOS_Kernel_Read_LogicSector(U32 tdw_LogicSectorAddr, U8 tc_ReadSectorNumber);
extern U8 DOS_Read_LogicSector(U32 tdw_LogicSectorAddr, U8 tc_ReadSectorNumber);
extern U8 DOS_Write_LogicSector(U32 tdw_LogicSectorAddr, U8 tc_WriteSectorNumber, UBYTE tc_AppendW);

#ifdef	BONDING_OPTION_ENABLE
extern xdata U8 gc_CustomerIDNumber; 	
extern U8   ID_Compare(U8 * tc_Table);
extern U16  CheckOption(void);
extern void CUSTOMER_IDRead(void);
extern void CUSTOMER_IDCheck(void);
#endif
//-------------------------------------------------------------------------------
#endif