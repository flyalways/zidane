#include <string.h>
#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "dos\dosfat.h"
#include "dos\dosfdb.h"
#include "dos\dosrw.h"
#include "Memalloc.h"
#include "Rambase.h"
#include "PROTOTYP.h"

extern	U8		HOST_Read_Logicsector(U32 LBAaddr);
extern	data	bit	gb_HostConnect;
extern	xdata	U8	gc_HostSectorUnit;

U32 DOS_GetNextCluster(U32 tdw_CurrentCluster, U32 tdw_ClusterNumber)
{
	data	U16 tw_WordOffset;
	xdata	U8  tc_status;											//Ching 081111
	xdata	U32 tdw_ClusterNumCounter=0;
	xdata	U32 tdw_fatOffset, tdw_DOS_FatSize;
	
	gb_ReadWriteDataArea=1;
	if(tdw_CurrentCluster<2)
	{//for rootdir
			return(0xffffffff);	
	}

	if (tdw_ClusterNumber == 0) 
	{
		return tdw_CurrentCluster;
	}
				
	tdw_fatOffset = tdw_CurrentCluster<< gc_DOS_FileSystemType;//fat16 or fat32	
	for(tdw_DOS_FatSize=(tdw_fatOffset>>9);tdw_DOS_FatSize<gdw_DOS_SectorPerFAT;tdw_DOS_FatSize++)//(JC)search sectors in FAT from current cluster sector to FAT end
	{//find in fat
		tc_status = DOS_Read_LogicSector((gdw_DOS_Fat1Addr+tdw_DOS_FatSize),1);
		if(tc_status)										//read fail
			tc_status = DOS_Read_LogicSector((gdw_DOS_Fat2Addr+tdw_DOS_FatSize), 1);

		if(tc_status == 0)
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
						if(SD_Detect)
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
			tdw_DOS_FatSize=(tdw_fatOffset>>9) - 1; 		//-1 是因為 for loop 會再 +1, Ching 081216
		}
		else
		{
			return(0xffffffff);
		}	
	} 
}


U32 DOS_ClusterLogicAddr(U32 tdw_Cluster)
{ 	
	U32 tdw_LogicAddr;

	if (tdw_Cluster == 0)
	{	//maybe Root dir
	  	if (gc_DOS_FileSystemType == 2)
		{//FAT32 cluster can not be 0
			tdw_LogicAddr = 0;//mark error
		}
		else
		{//FAT12&FAT16 root dir
			tdw_LogicAddr = gdw_DOS_RootDirAddr;
		}
	}
	else if (tdw_Cluster == 1)
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


U8 DOS_Read_LogicSector(U32 tdw_LogicSectorAddr, U8 tc_ReadSectorNumber)
{
	data	U8	tc_Status = DOS_FALSE;
	data	U8	tc_BufIndex;
	xdata	U32 tdw_tempLogicSectorAddr;

	if(gb_ReadWriteDataArea==0)
	{
		tc_BufIndex=gc_PlayRecordDataBufHB;
	}
	else
	{
		tc_BufIndex=gc_UserDataBufHB;
	}
	switch(gc_CurrentCard)
	{
		case 0:
			tdw_tempLogicSectorAddr=tdw_LogicSectorAddr;			
			while(tc_ReadSectorNumber)
			{
				tc_Status = (U8)Dos_Read_LBA(tdw_tempLogicSectorAddr, tc_BufIndex); //Ching 081111
				tc_ReadSectorNumber--;
				tdw_tempLogicSectorAddr++;
			}
			break;

		case 2:
			tdw_tempLogicSectorAddr=tdw_LogicSectorAddr;			
			while(tc_ReadSectorNumber)
			{
				SD_Read_LBA_Dos(tdw_tempLogicSectorAddr ,tc_BufIndex);
				tc_ReadSectorNumber--;
				tdw_tempLogicSectorAddr++;
			}

			if(SD_Detect)  //if SD not exist return false  //chiayen0813  
			{
				tc_Status=DOS_FALSE;
			}
			else
			{
			tc_Status=DOS_SUCCESS;
			}
		break;

    	case 5:
			HOST_Read_Logicsector(tdw_LogicSectorAddr);
			if(gb_HostConnect == 0x00)
			{
				tc_Status=DOS_FALSE;		
			}
			else
			{
				tc_Status=DOS_SUCCESS;	
			}
			break;
	}
	return tc_Status;
}


//! Write some logical sectors from a given LBA.
/*!	
 *	\param	tdw_LogicSectorAddr		The starting LBA to be written.
 *	\param	tc_WriteSectorNumber	Number of sectors to be written.
 *	\param	tc_AppendW				The append write flag.
 *	\return	DOS_SUCCESS or DOS_FALSE
 */
U8 DOS_Write_LogicSector(U32 tdw_LogicSectorAddr, U8 tc_WriteSectorNumber, U8 tc_AppendW)
{
	data	U8	tc_BufIndex;
	data	U8	tc_WrSectorNum;
	xdata	U32 tdw_tempLogicSectorAddr;
	
	tdw_tempLogicSectorAddr=tdw_LogicSectorAddr;
	tc_WrSectorNum=tc_WriteSectorNumber;

	if(gb_ReadWriteDataArea == 0)
	{
		tc_BufIndex=gc_PlayRecordDataBufHB;
	}
	else
	{
		tc_BufIndex=gc_UserDataBufHB;
	}

	switch(gc_CurrentCard)
	{
		case 0:
			if(!tc_WrSectorNum)
			{
				return (DOS_FALSE);
			}			
		
			if(tc_AppendW)
			{
				//============================================================
				if(gc_NeedNewBlock==1)
				{
					if(gb_NeedEraseUpdateBlock==1)
					{
						//dbprintf("appdw-S+C+E\n");
						Dos_Write_LBA(tdw_tempLogicSectorAddr, tc_BufIndex, 0x03);//(JC)start+continue+end
						gb_NeedEraseUpdateBlock=0;
						gc_NeedNewBlock=1;
					}
					else
					{
						//dbprintf("appdw-S+C\n");
						Dos_Write_LBA(tdw_tempLogicSectorAddr, tc_BufIndex, 0x00);//(JC)start+continue
						gc_NeedNewBlock=0;
					}
				}
				else if(gc_NeedNewBlock==0)
				{
					if(gb_NeedEraseUpdateBlock==1)
					{
						while(tc_WrSectorNum>1)
						{
							//dbprintf("appdw-C\n");
							Dos_Write_LBA(tdw_tempLogicSectorAddr, tc_BufIndex, 0x01);//(JC)continue
							tc_WrSectorNum--;
							tdw_tempLogicSectorAddr++;
						}
						//dbprintf("appdw-C+E\n");
						Dos_Write_LBA(tdw_tempLogicSectorAddr, tc_BufIndex, 0x02);//(JC)continue+end
						gb_NeedEraseUpdateBlock=0;
						gc_NeedNewBlock=1;
					}
					else
					{
						//dbprintf("appdw-C\n");
						Dos_Write_LBA(tdw_tempLogicSectorAddr, tc_BufIndex, 0x01);//(JC)continue
					}
				}
			}
			else
			{
				while(tc_WrSectorNum)//(JC)mostly normal write 1 sector
				{
					//dbprintf("Normw-S+C+E\n");
					Dos_Write_LBA(tdw_tempLogicSectorAddr, tc_BufIndex, 0x03);
					tc_WrSectorNum--;
					tdw_tempLogicSectorAddr++;	
				}
			}
			break;

		case 2:
			if(!tc_WrSectorNum)
			{
				return 1;
			}
			
			tdw_tempLogicSectorAddr=tdw_LogicSectorAddr;
			if(tc_WrSectorNum==0x01)
			{
				SD_Write_LBA_Dos(tdw_tempLogicSectorAddr,tc_BufIndex, 0x03);				
			}
			else
			{					
				SD_Write_LBA_Dos(tdw_tempLogicSectorAddr,tc_BufIndex, 0x00);
				tc_WrSectorNum--;
				tdw_tempLogicSectorAddr++;
				while(tc_WrSectorNum)
				{
					SD_Write_LBA_Dos(tdw_tempLogicSectorAddr,tc_BufIndex, 0x01);
					tc_WrSectorNum--;
					tdw_tempLogicSectorAddr++;
				}
				SD_Write_LBA_Dos(tdw_tempLogicSectorAddr,tc_BufIndex, 0x04);
			}
			break;
	}
	return 0;
}


