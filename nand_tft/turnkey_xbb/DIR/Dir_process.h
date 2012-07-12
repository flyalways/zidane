/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	Dir_process.h
 *	\brief	Dir Management related variable and function declarations.
 *
 *	\author trista
 */

#ifndef	_Dir_process_
#define _Dir_process_

//========= Dir_Process =========
extern U8 Count_Dir_Fdb(SearchFdb *p_mp);
extern U8 DOS_Search_DIR(U8 tc_Mode);
extern U8 DOS_DIRtable(void);
extern	U8 GetCurrentDirInfo(void);
extern	U8	DOS_Search_DIR_Next_test(U8 tc_Mode);
extern	U8	DOS_Search_DIR_Prev_test(U8 tc_Mode);

//======== Dir_ReadWrite ========
extern void USER_GetUISetIndex(void);
extern void USER_WriteReserveBlock_DIR(U8 tc_PageNum, U8 tc_CurrentCard);
extern void USER_Read512BReserveBlock_DIR(U16 DirIndex, U8 tc_CurrentCard);
extern void USER_Read2KBReserveBlock_DIR(U16 DirIndex, U8 tc_CurrentCard);
extern void USER_MarkTable(U8 tc_MarkFlag);
extern void USER_GetReBuildFlag(void);

//=========== Dir_LCM ===========
extern void LCD_DisplayIcon_Dir(U16 tw_IconIDNumber,U8 tc_CurrentOffset,U8 tc_SetPosition,U16 tw_Star_page);
extern void LCD_GetDirName(U8 tc_DIRtableIndex);
extern void LCD_Get_FatherName(void);
extern void LCD_Get_ChildName(void);
extern void LCD_Disp_FatherName_Dir(void);
extern void LCD_Disp_ChildName(void);
extern void UI_DIRStoreOrRecover(bit tb_SaveOrRecover);

#endif