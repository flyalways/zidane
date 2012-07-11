#include <string.h>
#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "dos\dosfat.h"
#include "Memalloc.h"


U8 DOS_CopyFATtoFAT(U32 tdw_StartCluster, U32 tdw_TailCluster, U8 tbt_fat1tofat2)
{
	U8	tc_Status = DOS_SUCCESS;
	U32 tdw_StartSector;
	U32 tdw_EndSector;
	U32 tdw_CopySecSize;
	U32 tdw_CopySourceAddr;
	U32 tdw_CopyTargetAddr;
	U32 tdw_LoopCount;
	U32 tdw_MarkAddr;
	
	if (tdw_TailCluster >= gdw_DOS_FatMaxCluster)//exceed max cluster
	{
		tdw_TailCluster = gdw_DOS_FatMaxCluster - 1;
	}
	
	//calculate start and end address of copying
	tdw_StartSector = (tdw_StartCluster << gc_DOS_FileSystemType)>>9;//(JC)Byte/512
	tdw_EndSector = (tdw_TailCluster << gc_DOS_FileSystemType)>>9;//(JC)Byte/512
	
	tdw_CopySecSize = (tdw_EndSector - tdw_StartSector)+1;
	//confirm the direction of the Copy operation
	if (tbt_fat1tofat2)
	{//FAT1 to FAT2
		tdw_CopySourceAddr = gdw_DOS_Fat1Addr + tdw_StartSector;//copy source address
		tdw_CopyTargetAddr = gdw_DOS_Fat2Addr + tdw_StartSector;//copy target address
		tdw_MarkAddr = gdw_DOS_Fat2Addr;//mark updating address
	}
	else
	{//FAT2 to FAT1
		tdw_CopySourceAddr = gdw_DOS_Fat2Addr + tdw_StartSector;
		tdw_CopyTargetAddr = gdw_DOS_Fat1Addr + tdw_StartSector;
		tdw_MarkAddr = gdw_DOS_Fat1Addr;
	}
	if (tdw_CopyTargetAddr != tdw_MarkAddr)
	{//mark updating flag 0xf8 ---> 0xaa
		DOS_MarkFATUpdate(tdw_MarkAddr,0xaa);
	}

	for(tdw_LoopCount = 0; tdw_LoopCount < tdw_CopySecSize; tdw_LoopCount++)
	{
		gb_ReadWriteDataArea = 0;//select gc_PlayRecordDataBuf[]
		tc_Status=DOS_Read_LogicSector(tdw_CopySourceAddr, 1);
		if(tc_Status)
		{
			return tc_Status;
		}
		//not close new block
		if (tdw_LoopCount == 0 && tdw_CopyTargetAddr == tdw_MarkAddr)
		{//first write sector is FAT1's start sector
			gc_PlayRecordDataBuf[0] = 0xaa;
		}
		tc_Status=DOS_Write_LogicSector(tdw_CopyTargetAddr, 1, 0);
		if(tc_Status)
		{
			return tc_Status;
		}
		tdw_CopySourceAddr++;//next sector
		tdw_CopyTargetAddr++;//next sector
	}
	DOS_MarkFATUpdate(tdw_MarkAddr,0xf8);//mark updating is complete
	
	return tc_Status;
}


//---------------------------------------------------------------
//! Mark the 1st byte of FAT
/*! Mark the 1st byte of FAT sector while updating.
 *	\param	tdw_MarkFATAddr		The FAT LBA.
 *	\param	tc_MarkValue		The byte mark to be written.
 *	\return	DOS_SUCCESS or DOS_FALSE.
 */
U8 DOS_MarkFATUpdate(U32 tdw_MarkFATAddr, U8 tc_MarkValue)
{
	U8	tc_Status;
		
	gb_ReadWriteDataArea = 0;//select gc_PlayRecordDataBuf[]
	tc_Status = DOS_Read_LogicSector(tdw_MarkFATAddr, 1);//read sector
	if (tc_Status == DOS_SUCCESS)
	{
		gc_PlayRecordDataBuf[0] = tc_MarkValue;//mark first byte
		tc_Status = DOS_Write_LogicSector(tdw_MarkFATAddr, 1, 0);//write sector	
	}
	return tc_Status;
}

