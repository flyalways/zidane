#include "..\header\SPDA2K.h"
#include "..\header\variables.h"


U8 JudgeBootSector(void)
{
	U32 tdw_DiskSize;
	U32	tdw_PBRAddr;
	U32	tdw_PBRAddr1;

	((U8 *)(&tdw_PBRAddr))[0]=gc_PlayRecordDataBuf[0x1C9];
	((U8 *)(&tdw_PBRAddr))[1]=gc_PlayRecordDataBuf[0x1C8];
	((U8 *)(&tdw_PBRAddr))[2]=gc_PlayRecordDataBuf[0x1C7];
	((U8 *)(&tdw_PBRAddr))[3]=gc_PlayRecordDataBuf[0x1C6];

	((U8 *)(&tdw_DiskSize))[0]=gc_PlayRecordDataBuf[0x1CD];
	((U8 *)(&tdw_DiskSize))[1]=gc_PlayRecordDataBuf[0x1CC];
	((U8 *)(&tdw_DiskSize))[2]=gc_PlayRecordDataBuf[0x1CB];
	((U8 *)(&tdw_DiskSize))[3]=gc_PlayRecordDataBuf[0x1CA];

	((U8 *)(&tdw_PBRAddr1))[0]=gc_PlayRecordDataBuf[0x1D9];
	((U8 *)(&tdw_PBRAddr1))[1]=gc_PlayRecordDataBuf[0x1D8];
	((U8 *)(&tdw_PBRAddr1))[2]=gc_PlayRecordDataBuf[0x1D7];
	((U8 *)(&tdw_PBRAddr1))[3]=gc_PlayRecordDataBuf[0x1D6];

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

	if ((gc_PlayRecordDataBuf[0x1fe]!=0x55)||(gc_PlayRecordDataBuf[0x1ff]!=0xAA)) return 2;
	else if((gc_PlayRecordDataBuf[0]!=0xEB)&&(gc_PlayRecordDataBuf[0]!=0xE9)) return 1;
	else return 0;
}	

U8 DOS_Initialize(void)
{
	xdata	U8	tc_Status;
	data	U8	tc_FATperDisc;
	data	U16	tw_RootDirSectors=0;
	data	U16	tw_DOS_ReserveSector;
	data	U32	tdw_PBRAddr;
	xdata	U32	tdw_PartitionSectorSize;

	tdw_PBRAddr=0;
	gb_ReadWriteDataArea=0;
	
	gw_FileIndex[0]=0;
	gw_FileIndex[1]=0;
	gw_FileIndex[2]=0;
	
	gw_FileTotalNumber[0]=0;
	gw_FileTotalNumber[1]=0;

	gs_File_FCB[0].dw_FDB_LogAdd=0;
	gs_File_FCB[1].dw_FDB_LogAdd=0;
	gs_File_FCB[2].dw_FDB_LogAdd=0;

	gs_File_FCB[0].dw_File_StartCluster=0xffffffff;
	gs_File_FCB[1].dw_File_StartCluster=0xffffffff;
	gs_File_FCB[2].dw_File_StartCluster=0xffffffff;

	if(!DOS_Read_LogicSector(0))
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
				((U8 *)(&tdw_PBRAddr))[0]=gc_PlayRecordDataBuf[0x1C9];
				((U8 *)(&tdw_PBRAddr))[1]=gc_PlayRecordDataBuf[0x1C8];
				((U8 *)(&tdw_PBRAddr))[2]=gc_PlayRecordDataBuf[0x1C7];
				((U8 *)(&tdw_PBRAddr))[3]=gc_PlayRecordDataBuf[0x1C6];
			}
			else
			{
				((U8 *)(&tdw_PBRAddr))[0]=gc_PlayRecordDataBuf[0x1D9];
				((U8 *)(&tdw_PBRAddr))[1]=gc_PlayRecordDataBuf[0x1D8];
				((U8 *)(&tdw_PBRAddr))[2]=gc_PlayRecordDataBuf[0x1D7];
				((U8 *)(&tdw_PBRAddr))[3]=gc_PlayRecordDataBuf[0x1D6];
			}
			gdw_DOS_PBRAddr=tdw_PBRAddr;
			if(DOS_Read_LogicSector(tdw_PBRAddr*gc_HostSectorUnit))
			{//read PBR sector error.
				return DOS_READ_PBS_ERR;
			} 
		} 

		if(((gc_PlayRecordDataBuf[54]=='F')&&(gc_PlayRecordDataBuf[55]=='A')&&(gc_PlayRecordDataBuf[56]=='T'))
		 ||((gc_PlayRecordDataBuf[82]=='F')&&(gc_PlayRecordDataBuf[83]=='A')&&(gc_PlayRecordDataBuf[84]=='T')))
		{
			gc_DOS_SectorPerCluster=gc_PlayRecordDataBuf[0x0D];
			tc_FATperDisc=gc_PlayRecordDataBuf[0x10];
			((U8 *)(&tw_DOS_ReserveSector))[0]=gc_PlayRecordDataBuf[0x0F];
			((U8 *)(&tw_DOS_ReserveSector))[1]=gc_PlayRecordDataBuf[0x0E];
			gdw_DOS_Fat1Addr=tdw_PBRAddr+tw_DOS_ReserveSector;
			((U8 *)(&gdw_DOS_SectorPerFAT))[0]=0;
			((U8 *)(&gdw_DOS_SectorPerFAT))[1]=0;
			((U8 *)(&gdw_DOS_SectorPerFAT))[2]=gc_PlayRecordDataBuf[0x17];
			((U8 *)(&gdw_DOS_SectorPerFAT))[3]=gc_PlayRecordDataBuf[0x16];
			if(gdw_DOS_SectorPerFAT)		//(JC)check FATSize16/12
	       	{
    			((U8 *)(&tw_RootDirSectors))[0]=gc_PlayRecordDataBuf[0x12];
				((U8 *)(&tw_RootDirSectors))[1]=gc_PlayRecordDataBuf[0x11];
				tw_RootDirSectors=tw_RootDirSectors>>4;
        	}
        	else//FAT32
        	{
       			((U8 *)(&gdw_DOS_SectorPerFAT))[0] = gc_PlayRecordDataBuf[0x27];
				((U8 *)(&gdw_DOS_SectorPerFAT))[1] = gc_PlayRecordDataBuf[0x26];
				((U8 *)(&gdw_DOS_SectorPerFAT))[2] = gc_PlayRecordDataBuf[0x25];
				((U8 *)(&gdw_DOS_SectorPerFAT))[3] = gc_PlayRecordDataBuf[0x24];
        	}
        	gdw_DOS_Fat2Addr=gdw_DOS_Fat1Addr;
        	if(tc_FATperDisc==2)
			{
      	    	gdw_DOS_Fat2Addr+=gdw_DOS_SectorPerFAT;
			}
               	
        	((U8 *)(&tdw_PartitionSectorSize))[0]=0;
			((U8 *)(&tdw_PartitionSectorSize))[1]=0;
			((U8 *)(&tdw_PartitionSectorSize))[2]=gc_PlayRecordDataBuf[0x14];
			((U8 *)(&tdw_PartitionSectorSize))[3]=gc_PlayRecordDataBuf[0x13];

        	if(tdw_PartitionSectorSize==0)
        	{//(JC)FAT32 or FAT12/16 size more than 32M
       			((U8 *)(&tdw_PartitionSectorSize))[0]=gc_PlayRecordDataBuf[0x23];
				((U8 *)(&tdw_PartitionSectorSize))[1]=gc_PlayRecordDataBuf[0x22];
				((U8 *)(&tdw_PartitionSectorSize))[2]=gc_PlayRecordDataBuf[0x21];
				((U8 *)(&tdw_PartitionSectorSize))[3]=gc_PlayRecordDataBuf[0x20];
        	}	

	    	if (tdw_PartitionSectorSize!=0)
			{				
				gdw_DOS_FatMaxCluster=tdw_PartitionSectorSize-tw_DOS_ReserveSector-tw_RootDirSectors-(gdw_DOS_SectorPerFAT<<(tc_FATperDisc-1));
				gdw_DOS_FatMaxCluster=gdw_DOS_FatMaxCluster/gc_DOS_SectorPerCluster;
					
       			if(gdw_DOS_FatMaxCluster<65525)
       			{
   					gc_DOS_FileSystemType=1;	// FAT16
	   			}
       			else
       			{
    				gc_DOS_FileSystemType=2;	// FAT32
   				}

//				gdw_DOS_FatMaxCluster+=2;	//(JC)the maximum cluster no
				if ((gdw_DOS_FatMaxCluster==0)||(gdw_DOS_FatMaxCluster>0x0FFFFFFF))
				{//0x0fffffff means last cluster in chain in FAT32.
					gdw_DOS_FatMaxCluster=0x0FFFFFFF;
				}

				if(gc_DOS_FileSystemType==1)
				{// FAT 16
					gdw_DOS_RootDirAddr = gdw_DOS_Fat2Addr + gdw_DOS_SectorPerFAT;
					gdw_DOS_DataAddr = gdw_DOS_RootDirAddr + tw_RootDirSectors/gc_HostSectorUnit;
					//FAT12/16 have no root dir cluster. starting cluster '2' is in the beginning of data area
					gdw_DOS_RootDirClus=0;
				}
				else if(gc_DOS_FileSystemType==2)					
				{// FAT 32
					gdw_DOS_DataAddr = gdw_DOS_Fat2Addr + gdw_DOS_SectorPerFAT;
					//(JC)BPB_RootClus[3:0]				
					((U8 *)(&gdw_DOS_RootDirClus))[0]=gc_PlayRecordDataBuf[0x2F];
					((U8 *)(&gdw_DOS_RootDirClus))[1]=gc_PlayRecordDataBuf[0x2E];
					((U8 *)(&gdw_DOS_RootDirClus))[2]=gc_PlayRecordDataBuf[0x2D];
					((U8 *)(&gdw_DOS_RootDirClus))[3]=gc_PlayRecordDataBuf[0x2C];
					gdw_DOS_RootDirAddr=gdw_DOS_DataAddr+((gdw_DOS_RootDirClus-2)*(U32)gc_DOS_SectorPerCluster);//start from cluster 2
				}
				gdw_DOS_Fat1Addr=gdw_DOS_Fat1Addr*gc_HostSectorUnit;
				gdw_DOS_Fat2Addr=gdw_DOS_Fat2Addr*gc_HostSectorUnit;
				gdw_DOS_DataAddr=gdw_DOS_DataAddr*gc_HostSectorUnit;
				gdw_DOS_RootDirAddr=gdw_DOS_RootDirAddr*gc_HostSectorUnit;
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
	return 0;  
}



