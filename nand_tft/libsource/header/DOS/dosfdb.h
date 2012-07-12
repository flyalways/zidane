#ifndef	_DOSFDB_
#define _DOSFDB_
//-------------------------------------------------------------------------------
extern xdata U32 gdw_CurrFlag;
extern xdata U32 gdw_StartCluster2;

extern U8  code  Unicode_Offset[];
extern U8  code  EXT_NameA[];
extern U8  code  EXT_NameB[];
extern U8  xdata EXT_NameC[];
extern U8 *code  EXT_Name[];
//-------------------------------------------------------------------------------
extern U8 DOS_AddNewFdb(U8 * tpc_FdbEntry, U32 tdw_DirStartCluster);
extern void DOS_FillFdb(U8 * tpc_Fdb, U8 * tpc_Name, U32 tdw_StartCluster, U32 tdw_FileSize);
extern U8 DOS_GetFileLongName(U8 tc_FileHandle,U8 tb_UicodeToISN, U8 * tpc_LongName);
extern U32 DOS_Make_Dir(U8 * tpc_DirName, U32 tdw_DirStartCluster);
extern U8 DOS_WriteDirInitData1(U32 tdw_CurrentStart, U32 tdw_ParentStart);
extern void USER_Create_Dir(U8 tc_UserDirNum, U32 tdw_DirStartCluster);
extern U8 USER_FindDir(U8 tc_UserDirNum, U32 tdw_DirStartCluster);
extern U8 Find_Fdb(SearchFdb *p_mp);
//-------------------------------------------------------------------------------
#endif
