#ifndef	_STORAGE_
#define _STORAGE_
//--------------------------------------------------------------------------------
extern xdata U32 gdw_DOS_SectorPerFAT;
extern xdata U32 gdw_DOS_Fat1Addr;
extern xdata U32 gdw_DOS_Fat2Addr;
//--------------------------------------------------------------------------------
extern U8 Card_Detect(void);
extern U8 DOS_Initialize(void);
extern U8 SMC_STORAGE_Initialize(void);
extern bit USR_Check_Character(U8 *SourceArray,U8 offset,U8 *CompareArray,U8 NumU8);
//--------------------------------------------------------------------------------
#endif