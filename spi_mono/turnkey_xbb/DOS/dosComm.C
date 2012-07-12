#include <string.h>
#include "..\header\SPDA2K.h"
#include "..\header\Rambase.h"
#include "..\header\variables.h"


U32 DOS_GetNextCluster(U32 tdw_CurrentCluster,U32 tdw_ClusterNumber)
{
	data	U16	tw_WordOffset;
	xdata	U32	tdw_ClusterNumCounter;
	xdata	U32	tdw_fatOffset;
	xdata	U32	tdw_DOS_FatSize;
	
	tdw_ClusterNumCounter=0;
	gb_ReadWriteDataArea=1;
	if(tdw_CurrentCluster<2)
	{//for rootdir
		return(0xffffffff);	
	}
	
	if (tdw_ClusterNumber == 0) 
	{
		return tdw_CurrentCluster;
	}
				
	tdw_fatOffset=tdw_CurrentCluster<<gc_DOS_FileSystemType;//fat16 or fat32	
	for(tdw_DOS_FatSize=(tdw_fatOffset>>9);tdw_DOS_FatSize<(gdw_DOS_SectorPerFAT*gc_HostSectorUnit);tdw_DOS_FatSize++)//(JC)search sectors in FAT from current cluster sector to FAT end
	{//find in fat
		if(DOS_Read_LogicSector(gdw_DOS_Fat1Addr+tdw_DOS_FatSize)==0)
		{			
			while((tdw_fatOffset>>9)==tdw_DOS_FatSize)
			{
				tw_WordOffset=tdw_fatOffset&0x1FF;
				if(gc_DOS_FileSystemType==1)
				{//FAT16
					if(tdw_CurrentCluster>=0xfff8)
					{						
						return(0xffffffff);
					}					
					tdw_CurrentCluster=(((U32)gc_UserDataBuf[tw_WordOffset+1]<<8))|(U32)gc_UserDataBuf[tw_WordOffset];
				}
				else if(gc_DOS_FileSystemType==2)
				{//fat32				
					if(tdw_CurrentCluster>=0x0ffffff8)
					{			
						return(0xffffffff);
					}
	
					((U8 *)(&tdw_CurrentCluster))[0]=gc_UserDataBuf[tw_WordOffset+3];
					((U8 *)(&tdw_CurrentCluster))[1]=gc_UserDataBuf[tw_WordOffset+2];
					((U8 *)(&tdw_CurrentCluster))[2]=gc_UserDataBuf[tw_WordOffset+1];
					((U8 *)(&tdw_CurrentCluster))[3]=gc_UserDataBuf[tw_WordOffset];	
					tdw_CurrentCluster &= 0x0FFFFFFF;
				}
				tdw_fatOffset = tdw_CurrentCluster<< gc_DOS_FileSystemType;
				tdw_ClusterNumCounter++;

				if(gc_CurrentCard==2)  //if SD not exist return DOS_CLUSTER_LINK_ERR chiayen0813
				{
					if(gb_SD_pin)
					{
						return(0xffffffff);		
					}
				}

				if((gc_CurrentCard==5) && (gb_HostConnect == 0))  //if Host not exist return DOS_CLUSTER_LINK_ERR chiayen 20081017
				{
					return(0xffffffff);		
				}

				if(tdw_ClusterNumCounter==tdw_ClusterNumber)
				{		
					return(tdw_CurrentCluster);
				}
			}
		}
		else
		{
			return(0xffffffff);
		}	
	}
}


U32 DOS_ClusterLogicAddr(U32 tdw_Cluster)
{ 	
	data	U32 tdw_LogicAddr;

	if(tdw_Cluster==0)
	{	//maybe Root dir
	  	if(gc_DOS_FileSystemType==2)
		{//FAT32 cluster can not be 0
			tdw_LogicAddr=0;//mark error
		}
		else
		{//FAT12&FAT16 root dir
			tdw_LogicAddr=gdw_DOS_RootDirAddr;
		}
	}
	else if(tdw_Cluster==1)
	{	//cluster is disable to be 1
	   	//when cluster is 1,error is occur	
		tdw_LogicAddr = 0;
	}
	else
	{//normal state
		tdw_LogicAddr=gdw_DOS_DataAddr+((tdw_Cluster-2)*gc_DOS_SectorPerCluster*gc_HostSectorUnit);
	}

  	return tdw_LogicAddr;
}


U8 DOS_Read_LogicSector(U32 tdw_LogicSectorAddr)
{
	xdata	U8	tc_Status;
	xdata	U8	tc_BufIndex;
	xdata	U32 tdw_tempLogicSectorAddr;

	switch(gc_CurrentCard)
	{
		case 2:
    		if(gb_ReadWriteDataArea==0)
			{
				tc_BufIndex=gc_PlayRecordDataBufHB;
			}
			else
			{
				tc_BufIndex=gc_UserDataBufHB;
			}
			
			tdw_tempLogicSectorAddr=tdw_LogicSectorAddr;			
			SD_Read_LBA_Dos(tdw_LogicSectorAddr,tc_BufIndex);

			if(gb_SD_pin)  //if SD not exist return false
			{
				tc_Status=1;
			}
			else
			{
				tc_Status=0;
			}
		break;

    	case 5:
			HOST_Read_Logicsector(tdw_LogicSectorAddr);
			if(gb_HostConnect==0)
			{
				tc_Status=1;		
			}
			else
			{
				tc_Status=0;	
			}
		break;
	}
	return tc_Status;
}


U8 DOS_Read_File(U8 tc_FileHandle)
{
	xdata	U8	tc_ResidueSector;
	xdata	U32	tdw_DataPoint;

	if(gs_File_FCB[tc_FileHandle].dw_File_StartCluster==0xffffffff)
	{
		return DOS_STARTCLUSTER_ERR;
	}

	if(gs_File_FCB[tc_FileHandle].dw_File_DataPoint<gs_File_FCB[tc_FileHandle].dw_File_TotalSize)
	{//not at file end.
		tc_ResidueSector=(gs_File_FCB[tc_FileHandle].dw_File_DataPoint>>9)&(gc_DOS_SectorPerCluster*gc_HostSectorUnit-1);
		if((tc_ResidueSector==0)&&(gs_File_FCB[tc_FileHandle].dw_File_DataPoint!=0))
		{//next cluster.
			gs_File_FCB[tc_FileHandle].dw_File_CurrentCluster=DOS_GetNextCluster(gs_File_FCB[tc_FileHandle].dw_File_CurrentCluster,1);
			if(gs_File_FCB[tc_FileHandle].dw_File_CurrentCluster==0xffffffff)
			{
				return DOS_CLUSTER_LINK_ERR;
			}
		}
		tdw_DataPoint=DOS_ClusterLogicAddr(gs_File_FCB[tc_FileHandle].dw_File_CurrentCluster)+tc_ResidueSector;
		gb_ReadWriteDataArea=0;
		gs_File_FCB[tc_FileHandle].dw_File_DataPoint+=512;
		return (DOS_Read_LogicSector(tdw_DataPoint));
	}
	else
	{
		return DOS_END_OF_FILE;
	}
}

U8 DOS_Write_LogicSector(U32 tdw_LogicSectorAddr,U8 tc_AppendW)
{
	if(gc_CurrentCard==2)
	{
		SD_Write_LBA_Dos(tdw_LogicSectorAddr,tc_AppendW);
	}
	else if(gc_CurrentCard==5)
	{
		Host_Write_LBA_Dos(tdw_LogicSectorAddr,tc_AppendW);
	}

	return 0;
}
