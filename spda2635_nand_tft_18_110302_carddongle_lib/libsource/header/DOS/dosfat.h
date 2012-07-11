
#ifndef	_DOSFAT_
#define _DOSFAT_
//-------------------------------------------------------------------------------
extern	U32 DOS_ClusterLogicAddr(U32 tdw_Cluster);
extern	U32 DOS_GetNextCluster(U32 tdw_CurrentCLuster, U32 tdw_ClusterNumber);

#endif
