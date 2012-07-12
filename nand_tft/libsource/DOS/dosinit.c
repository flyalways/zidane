#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"

extern	xdata	U8	gc_HostSectorUnit;

U8 JudgeBootSector(void)
{
	U32 tdw_DiskSize;
	U32	tdw_PBRAddr;
	U32	tdw_PBRAddr1;

	if(gc_CurrentCard==0)
	{
		if(gbt_Support_Dummy_Capacity==1)
		{
			gdw_CARD_TotalSizeMB = Dummy_Capacity.LW;	
		}
		else
		{
			gdw_CARD_TotalSizeMB = Capacity.LW; 
		}
	}

	((UBYTE *)(&tdw_PBRAddr))[0]=gc_PlayRecordDataBuf[0x1C9];
	((UBYTE *)(&tdw_PBRAddr))[1]=gc_PlayRecordDataBuf[0x1C8];
	((UBYTE *)(&tdw_PBRAddr))[2]=gc_PlayRecordDataBuf[0x1C7];
	((UBYTE *)(&tdw_PBRAddr))[3]=gc_PlayRecordDataBuf[0x1C6];

	((UBYTE *)(&tdw_DiskSize))[0]=gc_PlayRecordDataBuf[0x1CD];
	((UBYTE *)(&tdw_DiskSize))[1]=gc_PlayRecordDataBuf[0x1CC];
	((UBYTE *)(&tdw_DiskSize))[2]=gc_PlayRecordDataBuf[0x1CB];
	((UBYTE *)(&tdw_DiskSize))[3]=gc_PlayRecordDataBuf[0x1CA];

	((UBYTE *)(&tdw_PBRAddr1))[0]=gc_PlayRecordDataBuf[0x1D9];
	((UBYTE *)(&tdw_PBRAddr1))[1]=gc_PlayRecordDataBuf[0x1D8];
	((UBYTE *)(&tdw_PBRAddr1))[2]=gc_PlayRecordDataBuf[0x1D7];
	((UBYTE *)(&tdw_PBRAddr1))[3]=gc_PlayRecordDataBuf[0x1D6];

	if((tdw_DiskSize+tdw_PBRAddr-1)==gdw_CARD_TotalSizeMB)
	{
		return	1;	// MBR
	}
	if((tdw_DiskSize+tdw_PBRAddr)==gdw_CARD_TotalSizeMB)
	{
		return	1;	// MBR
	}

	if((tdw_DiskSize+tdw_PBRAddr)<gdw_CARD_TotalSizeMB)
	{
		if(((tdw_DiskSize+tdw_PBRAddr)*100/98)>gdw_CARD_TotalSizeMB)
		{
			return	1;	// MBR
		}
	}

	if((tdw_DiskSize+tdw_PBRAddr+gc_PlayRecordDataBuf[0x1D0]-1)==tdw_PBRAddr1)
	{
//		dbprintf(">>>iPod\n");
		return 1;
	}	

	if((gc_PlayRecordDataBuf[54]=='F')&&(gc_PlayRecordDataBuf[55]=='A')&&(gc_PlayRecordDataBuf[56]=='T'))
	{
		return 0;
	}

	if((gc_PlayRecordDataBuf[82]=='F')&&(gc_PlayRecordDataBuf[83]=='A')&&(gc_PlayRecordDataBuf[84]=='T'))
	{
		return 0;
	}
	return 2;
}	


U8 DOS_Initialize(void)
{
	U8	tc_Status;
	U16	tw_RootDirSectors=0;
	U32	tdw_PBRAddr;
	U16	tw_DOS_ReserveSector;
	U32	tdw_PartitionSectorSize;
	U8	tc_FATperDisc;

	tdw_PBRAddr=0;
	gb_ReadWriteDataArea = 0;
	gw_FileIndex[0]=0;
	gw_FileIndex[1]=0;
	gw_FileIndex[2]=0;
	
	gw_FileTotalNumber[0]=0;
	gw_FileTotalNumber[1]=0;

	gdw_TotalFreeClusNumber=0;
	gdw_FreeClusterNum[0]=0; 
	gdw_FreeClusterNum[1]=0;    //lizhn for search free cluster
	gc_ClusBufValidPoint = 0;
	gc_ClusBufValidSize = 0;
	
	gs_File_FCB[0].dw_FDB_LogAdd = 0;
	gs_File_FCB[1].dw_FDB_LogAdd = 0;
	gs_File_FCB[2].dw_FDB_LogAdd = 0;

	gs_File_FCB[0].dw_File_StartCluster = 0xffffffff;
	gs_File_FCB[1].dw_File_StartCluster = 0xffffffff;
	gs_File_FCB[2].dw_File_StartCluster = 0xffffffff;

	gc_NeedNewBlock=1;
	gb_NeedEraseUpdateBlock=0;

	if(!DOS_Read_LogicSector(0x00000000, 1))//(JC)Read sector 0
	{ 
		tc_Status=JudgeBootSector();
		if(tc_Status==2)//(JC)neither PBR nor MBR 
		{
			return 1;
		}
		else if(tc_Status==1)//(JC)MBR
		{
			if(gc_PlayRecordDataBuf[0x1C2]!=0)
			{
				((UBYTE *)(&tdw_PBRAddr))[0] = gc_PlayRecordDataBuf[0x1c9];
				((UBYTE *)(&tdw_PBRAddr))[1] = gc_PlayRecordDataBuf[0x1c8];
				((UBYTE *)(&tdw_PBRAddr))[2] = gc_PlayRecordDataBuf[0x1c7];
				((UBYTE *)(&tdw_PBRAddr))[3] = gc_PlayRecordDataBuf[0x1c6];
			}
			else
			{
				((UBYTE *)(&tdw_PBRAddr))[0] = gc_PlayRecordDataBuf[0x1D9];
				((UBYTE *)(&tdw_PBRAddr))[1] = gc_PlayRecordDataBuf[0x1D8];
				((UBYTE *)(&tdw_PBRAddr))[2] = gc_PlayRecordDataBuf[0x1D7];
				((UBYTE *)(&tdw_PBRAddr))[3] = gc_PlayRecordDataBuf[0x1D6];
			}

			if(DOS_Read_LogicSector(tdw_PBRAddr*gc_HostSectorUnit, 1))
			{//read PBR sector error.
				return DOS_READ_PBS_ERR;
			} 
		} 

		if(((gc_PlayRecordDataBuf[54]=='F')&&(gc_PlayRecordDataBuf[55]=='A')&&(gc_PlayRecordDataBuf[56]=='T'))
		 ||((gc_PlayRecordDataBuf[82]=='F')&&(gc_PlayRecordDataBuf[83]=='A')&&(gc_PlayRecordDataBuf[84]=='T')))
		{
			gc_DOS_SectorPerCluster = gc_PlayRecordDataBuf[0x0d];
			tc_FATperDisc = gc_PlayRecordDataBuf[0x10];
			((UBYTE *)(&tw_DOS_ReserveSector))[0] = gc_PlayRecordDataBuf[0x0f];
			((UBYTE *)(&tw_DOS_ReserveSector))[1] = gc_PlayRecordDataBuf[0x0e];
			gdw_DOS_Fat1Addr = tdw_PBRAddr + tw_DOS_ReserveSector;
			((UBYTE *)(&gdw_DOS_SectorPerFAT))[0] = 0;
			((UBYTE *)(&gdw_DOS_SectorPerFAT))[1] = 0;
			((UBYTE *)(&gdw_DOS_SectorPerFAT))[2] = gc_PlayRecordDataBuf[0x17];
			((UBYTE *)(&gdw_DOS_SectorPerFAT))[3] = gc_PlayRecordDataBuf[0x16];
			if(gdw_DOS_SectorPerFAT)		//(JC)check FATSize16/12
	       	{
    			((UBYTE *)(&tw_RootDirSectors))[0] = gc_PlayRecordDataBuf[0x12];
				((UBYTE *)(&tw_RootDirSectors))[1] = gc_PlayRecordDataBuf[0x11];
				tw_RootDirSectors = tw_RootDirSectors >>4;		//(JC)*32/512
        	}
        	else//FAT32
        	{
       			((UBYTE *)(&gdw_DOS_SectorPerFAT))[0] = gc_PlayRecordDataBuf[0x27];
				((UBYTE *)(&gdw_DOS_SectorPerFAT))[1] = gc_PlayRecordDataBuf[0x26];
				((UBYTE *)(&gdw_DOS_SectorPerFAT))[2] = gc_PlayRecordDataBuf[0x25];
				((UBYTE *)(&gdw_DOS_SectorPerFAT))[3] = gc_PlayRecordDataBuf[0x24];
        	}
        	gdw_DOS_Fat2Addr=gdw_DOS_Fat1Addr;
        	if(tc_FATperDisc==2)
			{
      	    	gdw_DOS_Fat2Addr+=gdw_DOS_SectorPerFAT;
			}
        	
        	((UBYTE *)(&tdw_PartitionSectorSize))[0] = 0;
			((UBYTE *)(&tdw_PartitionSectorSize))[1] = 0;
			((UBYTE *)(&tdw_PartitionSectorSize))[2] = gc_PlayRecordDataBuf[0x014];
			((UBYTE *)(&tdw_PartitionSectorSize))[3] = gc_PlayRecordDataBuf[0x013];
        	if(tdw_PartitionSectorSize==0)
        	{//(JC)FAT32 or FAT12/16 size more than 32M
       			((UBYTE *)(&tdw_PartitionSectorSize))[0] = gc_PlayRecordDataBuf[0x23];
				((UBYTE *)(&tdw_PartitionSectorSize))[1] = gc_PlayRecordDataBuf[0x22];
				((UBYTE *)(&tdw_PartitionSectorSize))[2] = gc_PlayRecordDataBuf[0x21];
				((UBYTE *)(&tdw_PartitionSectorSize))[3] = gc_PlayRecordDataBuf[0x20];
        	}	

	    	if (tdw_PartitionSectorSize != 0)
			{				
				gdw_DOS_FatMaxCluster=tdw_PartitionSectorSize-tw_DOS_ReserveSector-tw_RootDirSectors-(gdw_DOS_SectorPerFAT<<(tc_FATperDisc-1));
				gdw_DOS_FatMaxCluster = gdw_DOS_FatMaxCluster / gc_DOS_SectorPerCluster;
					
       			if(gdw_DOS_FatMaxCluster<65525)
       			{
   					gc_DOS_FileSystemType=1;
   					//dbprintf("FAT16\n");
	   			}
       			else
       			{
    				gc_DOS_FileSystemType=2;
        			//dbprintf("FAT32\n");
   				}

       			gdw_DOS_FatMaxCluster+=2;	//(JC)the maximum cluster no
       			if (gdw_DOS_FatMaxCluster == 0 || gdw_DOS_FatMaxCluster > 0x0fffffff)
				{//0x0fffffff means last cluster in chain in FAT32.
					gdw_DOS_FatMaxCluster = 0x0fffffff;
				}
				if(gc_DOS_FileSystemType == 1)
				{// FAT 16
					gdw_DOS_RootDirAddr = gdw_DOS_Fat2Addr + gdw_DOS_SectorPerFAT;
					gdw_DOS_DataAddr = gdw_DOS_RootDirAddr + tw_RootDirSectors/gc_HostSectorUnit;
					//FAT12/16 have no root dir cluster. starting cluster '2' is in the beginning of data area
					gdw_DOS_RootDirClus = 0;
				}
				else if(gc_DOS_FileSystemType == 2)					
				{// FAT 32
					gdw_DOS_DataAddr = gdw_DOS_Fat2Addr + gdw_DOS_SectorPerFAT;
					//(JC)BPB_RootClus[3:0]				
					((UBYTE *)(&gdw_DOS_RootDirClus))[0] = gc_PlayRecordDataBuf[0x2f];
					((UBYTE *)(&gdw_DOS_RootDirClus))[1] = gc_PlayRecordDataBuf[0x2e];
					((UBYTE *)(&gdw_DOS_RootDirClus))[2] = gc_PlayRecordDataBuf[0x2d];
					((UBYTE *)(&gdw_DOS_RootDirClus))[3] = gc_PlayRecordDataBuf[0x2c];
					gdw_DOS_RootDirAddr = gdw_DOS_DataAddr + ((gdw_DOS_RootDirClus-2) * (LWORD)gc_DOS_SectorPerCluster);//start from cluster 2
				}
				gdw_DOS_Fat1Addr=gdw_DOS_Fat1Addr*gc_HostSectorUnit;
				gdw_DOS_Fat2Addr=gdw_DOS_Fat2Addr*gc_HostSectorUnit;
				gdw_DOS_DataAddr=gdw_DOS_DataAddr*gc_HostSectorUnit;
				gdw_DOS_RootDirAddr=gdw_DOS_RootDirAddr*gc_HostSectorUnit;
							
				if(gc_CurrentCard<2)
				{
					if(!DOS_Read_LogicSector(gdw_DOS_Fat1Addr, 1))
					{//check FAT1
						if (gc_PlayRecordDataBuf[0] != 0xf8 || gc_PlayRecordDataBuf[1] != 0xff)
						{
							if(!DOS_Read_LogicSector(gdw_DOS_Fat2Addr,1))
							{
								if(gc_PlayRecordDataBuf[0] !=	0xf8 ||	gc_PlayRecordDataBuf[1] != 0xff)
								{//(JC)Both FAT1 and FAT2 r error
									return DOS_FAT_ERR;
								}
								else
								{// copy fat2 to fat1
									DOS_CopyFATtoFAT(0x02, 0x0fffffff, 0);
								}
							}
						}
						else
						{// check FAT2
							if(!DOS_Read_LogicSector(gdw_DOS_Fat2Addr,1))
							{
								if (gc_PlayRecordDataBuf[0] !=	0xf8 ||	gc_PlayRecordDataBuf[1] != 0xff)
								{// copy fat1 to fat2
									DOS_CopyFATtoFAT(0x02, 0x0fffffff, 1);
								}
							}
							else
							{
								//dbprintf("read fat2 err!!!\n");
								return DOS_READ_FAT2_ERR;
							}
						}
					}
					else
					{
						//dbprintf("read fat1 read func err!!!\n");
						return DOS_READ_FAT1_ERR;
					}
				}
			}
			else
			{//tdw_PartitionSectorSize == 0, not FAT12/16/32
				return DOS_PARTITIONSIZE_ERR;
			}
		}
	}
	else
	{
		return DOS_READ_MBS_ERR;	
	}
	return DOS_SUCCESS;  
}



