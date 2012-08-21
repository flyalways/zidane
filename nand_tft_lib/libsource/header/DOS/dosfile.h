/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	dosfile.h
 *	\brief	Prototype of file operation functions.
 *
 *	\author trista, jason
 */
#ifndef	_DOSFILE_
#define _DOSFILE_

extern xdata DIR_FCB   gs_DIR_FCB[];
//extern U8     DOS_tc_AppendWrite_File(U8 tc_FileHandle);
extern U8     DOS_Close_File(UBYTE tc_FileHandle, UBYTE tc_CloseMode, UBYTE * tpc_FileName);
extern void   DOS_Init_gc_PlayRecordDataBuf(UBYTE Value);
extern U8     DOS_Open_File(UBYTE tc_FileHandle, UBYTE tc_OpenMode, UBYTE * tpc_FileName);
extern U8     DOS_Read_File(UBYTE tc_FileHandle);
extern U32    DOS_Seek_File(UBYTE tc_FileHandle, LWORD tdw_SectorNumber);
//extern U8     DOS_Write_File(U8 tc_FileHandle);
//extern U8     DOS_Write_FileCore(U8 tc_FileHandle, U8 tbt_WriteMode);
extern U8     DOS_Write_File(UBYTE tc_FileHandle, UBYTE tc_tc_AppendW);
extern U8	  DOS_Search_File(UBYTE tc_Mode, UBYTE tc_Type, UBYTE tc_PrevOrNext);

#endif