#include <string.h>
#include "..\header\SPDA2K.h"
#include "..\header\variables.h"

U8 DOS_Open_File_r(U8 tc_FileHandle, U8 tc_OpenMode, U8 *tpc_FileName)
{
	data	U16	tw_Fdb_offset;
	data	U32	tdw_temp;
	idata	SearchFdb *p_mp;
	xdata	SearchFdb temp;

	p_mp=&temp;

	if(!gs_File_FCB[tc_FileHandle].dw_FDB_LogAdd)
	{
		return 1;	
	}

	if(tc_OpenMode==0)//open SPECIFIC name file
	{
		if(tpc_FileName[2]==K_LongFileName)
		{
			p_mp->c_Search_Mode=K_SPECIFIC_LONG_FILENAME;
		}
		else if(tpc_FileName[2]==K_ShortFileName) 
		{
			p_mp->c_Search_Mode=K_SPECIFIC_SHORT_FILENAME;
		}
		p_mp->pc_LongFileName=tpc_FileName;
		p_mp->dw_FDB_StartCluster=gs_File_FCB[tc_FileHandle].dw_FDB_StartCluster;//(JC)start cluster of DIR that the file is in	
		Find_Fdb(p_mp);
	} 
	else 
	{//open found file
		p_mp->w_FDB_Offset =gs_File_FCB[tc_FileHandle].w_FDB_Offset;
		p_mp->dw_FDB_LogAdd =gs_File_FCB[tc_FileHandle].dw_FDB_LogAdd;
	}
	tw_Fdb_offset=p_mp->w_FDB_Offset;
	gb_ReadWriteDataArea=1;
	DOS_Read_LogicSector(p_mp->dw_FDB_LogAdd);
		
	((U8 *)(&tdw_temp))[0]=gc_UserDataBuf[tw_Fdb_offset+31];//(JC)store file size(in FDB) into tdw_temp then dw_File_TotalSize
	((U8 *)(&tdw_temp))[1]=gc_UserDataBuf[tw_Fdb_offset+30];
	((U8 *)(&tdw_temp))[2]=gc_UserDataBuf[tw_Fdb_offset+29];
	((U8 *)(&tdw_temp))[3]=gc_UserDataBuf[tw_Fdb_offset+28];
	gs_File_FCB[tc_FileHandle].dw_File_TotalSize= tdw_temp;
	if (tc_OpenMode != 2)//(JC)specific name file
	{//in mode 2 these value is got in searchfile() at first 
		gs_File_FCB[tc_FileHandle].dw_LongFDB_LogAdd1=p_mp->dw_LongFDB_LogAdd1;
		gs_File_FCB[tc_FileHandle].dw_LongFDB_LogAdd0=p_mp->dw_LongFDB_LogAdd0;
		gs_File_FCB[tc_FileHandle].dw_FDB_Cluster=p_mp->dw_FDB_Cluster;
		gs_File_FCB[tc_FileHandle].dw_FDB_LogAdd=p_mp->dw_FDB_LogAdd;
		gs_File_FCB[tc_FileHandle].w_FDB_Offset=p_mp->w_FDB_Offset;
	}
	//get the File_StartCluster
	((U8 *)(&tdw_temp))[0]=gc_UserDataBuf[tw_Fdb_offset+21];
	((U8 *)(&tdw_temp))[1]=gc_UserDataBuf[tw_Fdb_offset+20];
	((U8 *)(&tdw_temp))[2]=gc_UserDataBuf[tw_Fdb_offset+27];
	((U8 *)(&tdw_temp))[3]=gc_UserDataBuf[tw_Fdb_offset+26];
	gs_File_FCB[tc_FileHandle].dw_File_StartCluster=tdw_temp;
	gs_File_FCB[tc_FileHandle].dw_File_CurrentCluster=tdw_temp;		
	gs_File_FCB[tc_FileHandle].dw_File_DataPoint = 0;        
	if(gs_File_FCB[tc_FileHandle].dw_File_StartCluster>=2&&gs_File_FCB[tc_FileHandle].dw_File_StartCluster!=0xffffffff)
	{//(JC)data cluster is from cluster 2
		return 0;
	}
	else
	{
		return 1;
	}

	return 0;	
}

U8 DOS_Open_File_w(void)
{
	data	U32	tdw_ClusterCnt;
	idata	SearchFdb *p_mp;
	idata	SearchFdb temp;

	p_mp=&temp;
	p_mp->c_Search_Mode=K_FIND_FREE_FDB;
	p_mp->dw_FDB_StartCluster=gs_File_FCB[1].dw_FDB_StartCluster;
	p_mp->c_Search_Attribute=0;
	//(JC)start to find a free FDB
	if(Find_Fdb(p_mp))	//no free fdb
	{		
		if(gs_File_FCB[1].dw_FDB_StartCluster<2)	//(JC)find no free FDB in root dir entry(FAT16)
		{//if root dir is full
			return 1;
		}
		tdw_ClusterCnt=DOS_GetAFreeCluster();				//get a free cluster for FDB(FAT32)
		if(!tdw_ClusterCnt)
		{
			return 1;
		}
		else
		{
			gs_File_FCB[1].dw_FDB_Cluster=tdw_ClusterCnt;//(JC)cluster no.
			gs_File_FCB[1].dw_FDB_LogAdd=((tdw_ClusterCnt-2)*gc_DOS_SectorPerCluster)+gdw_DOS_DataAddr;//xyq040811
			gs_File_FCB[1].w_FDB_Offset=0;
		}

		tdw_ClusterCnt=DOS_GetAFreeCluster();				//(JC)Get a free cluster for file content
		if(!tdw_ClusterCnt)
		{
			return 1;
		}
		else
		{
			gs_File_FCB[1].dw_File_TotalSize=0;
			gs_File_FCB[1].dw_File_DataPoint=0;
			gs_File_FCB[1].dw_File_StartCluster=tdw_ClusterCnt;
			gs_File_FCB[1].dw_File_CurrentCluster=tdw_ClusterCnt;
		}
	}
	else 
	{//(JC)Get a free FDB, info stored in p_mp
		tdw_ClusterCnt=0;
		gs_File_FCB[1].dw_FDB_Cluster=p_mp->dw_FDB_Cluster;//(JC)File FDB start cluster
		tdw_ClusterCnt=DOS_GetAFreeCluster();				//(JC)Get a free cluster for file content
		if(!tdw_ClusterCnt)
		{
			return 1;
		}
		else
		{
			gs_File_FCB[1].dw_File_TotalSize=0;
			gs_File_FCB[1].dw_File_DataPoint=0;
			gs_File_FCB[1].dw_File_StartCluster=tdw_ClusterCnt;
			gs_File_FCB[1].dw_File_CurrentCluster=tdw_ClusterCnt;
			gs_File_FCB[1].dw_FDB_LogAdd=p_mp->dw_FDB_LogAdd;
			gs_File_FCB[1].w_FDB_Offset=p_mp->w_FDB_Offset;
		}
	}
	return 0;	
}

bit DOS_Close_File_w(U8 *tpc_FileName)
{
	data	U8	i;
	data	U16	tw_Addr;
	xdata	U8	tac_FileFdb[32];
	xdata	U32	tdw_TempCluster;
	xdata	U32	tdw_PreviousCluster;
	xdata	U32	tdw_TempLogicAddr;

	if(gs_File_FCB[1].dw_File_StartCluster==0xFFFFFFFF)
	{		
		return 1;	
	}

	if(gc_CurrentCard==2)	// SD card must issue CMD12(stop command)
	{
	    if(gc_DOS_SectorPerCluster!=1) 
		SD_Write_LBA_Dos(++gdw_LogicSectorAddr,4);	// SD stop command only
	}

	if(gs_File_FCB[1].dw_File_TotalSize!=0)
	{
		// Fill FDB
		DOS_FillFdb(tac_FileFdb,tpc_FileName,gs_File_FCB[1].dw_File_StartCluster,gs_File_FCB[1].dw_File_TotalSize);
		tac_FileFdb[0x0B]=0x20;//attribute is archive		
		tdw_TempLogicAddr=DOS_ClusterLogicAddr(gs_File_FCB[1].dw_FDB_Cluster);
		if((tdw_TempLogicAddr==gs_File_FCB[1].dw_FDB_LogAdd)&&(gs_File_FCB[1].w_FDB_Offset==0)&&(gs_File_FCB[1].dw_FDB_Cluster!=gdw_DOS_RootDirClus))
		{//directory area is full,must add fdb in a new free cluster 					
			memcpy(gc_PlayRecordDataBuf,tac_FileFdb,32);
			memset(&gc_PlayRecordDataBuf[32],0,480);
			gb_ReadWriteDataArea=0;
			DOS_Write_LogicSector(tdw_TempLogicAddr,0);
			//clear other area of the cluster to 0
			memset(gc_PlayRecordDataBuf,0,32);		
			for(i=1;i<gc_DOS_SectorPerCluster-1;i++)
			{
	  			DOS_Write_LogicSector((tdw_TempLogicAddr+i),1);
			}
			DOS_Write_LogicSector((tdw_TempLogicAddr+i),2);

			//find the last cluster of the directory
			tdw_TempCluster=gs_File_FCB[1].dw_FDB_StartCluster;
			while(tdw_TempCluster<gdw_DOS_FatMaxCluster)
			{
				tdw_PreviousCluster=tdw_TempCluster;
				tdw_TempCluster=DOS_GetNextCluster(tdw_TempCluster,1);
			}
			//connect the free cluster after the last cluster of the directory
			DOS_ConnectFATChain(tdw_PreviousCluster,gs_File_FCB[1].dw_FDB_Cluster);	
		}
		else
		{//add fdb in directory area
			gb_ReadWriteDataArea=0;
			DOS_Read_LogicSector(gs_File_FCB[1].dw_FDB_LogAdd);
			memcpy(&gc_PlayRecordDataBuf[gs_File_FCB[1].w_FDB_Offset],tac_FileFdb,32);
			DOS_Write_LogicSector(gs_File_FCB[1].dw_FDB_LogAdd,3);
		}

		gb_ReadWriteDataArea=1;
		tdw_TempLogicAddr=gs_File_FCB[1].dw_File_CurrentCluster>>(9-gc_DOS_FileSystemType);
		// gc_FATTempBuffer[] DMA to gc_UserDataBuf[]
		XBYTE[0xB301]=0x00;
		XBYTE[0xB112]=0x00;
		XBYTE[0xB113]=gc_FATTempBufferHB;
		XBYTE[0xB114]=0x00;
		XBYTE[0xB115]=gc_UserDataBufHB;
		XBYTE[0xB3B0]=0x01;
		while(!(XBYTE[0xB3C0]&0x01));
		XBYTE[0xB3C0]=0;

		if(gc_DOS_FileSystemType==1) //FAT16
 	    {
			tw_Addr=(gs_File_FCB[1].dw_File_CurrentCluster&0xFF)<<1; 
		}
		else        //FAT32 
		{
			tw_Addr=(gs_File_FCB[1].dw_File_CurrentCluster&0x7F)<<2;
 		}
		gc_UserDataBuf[tw_Addr+0]=0xFF;
		gc_UserDataBuf[tw_Addr+1]=0xFF;
		if(gc_DOS_FileSystemType==2)
		{
			gc_UserDataBuf[tw_Addr+2]=0xFF;
			gc_UserDataBuf[tw_Addr+3]=0xFF;
		}
		DOS_Write_LogicSector(gdw_DOS_Fat1Addr+tdw_TempLogicAddr,3);
		DOS_Write_LogicSector(gdw_DOS_Fat2Addr+tdw_TempLogicAddr,3);
	}

	return 0;
}

U8 DOS_Write_File(U8 tc_AppendWr)
{
	data	U8	tc_Status;
	data	U16	tw_Addr;
	data	U32	tw_temp;
	data	U32	tdw_FreeCluster;
	data	U32	tdw_CurrentFATSector;
	
	if(gs_File_FCB[1].dw_File_StartCluster==0xFFFFFFFF)
	{
		return 1;	
	}

	if((gs_File_FCB[1].dw_File_DataPoint>>9)==gc_DOS_SectorPerCluster)
	{	
		// Get Next free cluster
		tdw_FreeCluster=DOS_GetAFreeCluster();
		if(tdw_FreeCluster==0)
		{
			// disk is full
			return 1;
		}
		else
		{
			// If Sectot Fill Full, Write into FAT1 & FAT2
			tdw_CurrentFATSector=gs_File_FCB[1].dw_File_CurrentCluster>>(9-gc_DOS_FileSystemType);
			if(gdw_FATBufferSectorNum!=tdw_CurrentFATSector)
			{
				gdw_FATBufferSectorNum=tdw_CurrentFATSector;
				DOS_Read_LogicSector(gdw_DOS_Fat1Addr+tdw_CurrentFATSector);
				// gc_UserDataBuf[] DMA to gc_FATTempBuffer[]
				XBYTE[0xB301]=0x00;
				XBYTE[0xB112]=0x00;
				XBYTE[0xB113]=gc_UserDataBufHB;
				XBYTE[0xB114]=0x00;
				XBYTE[0xB115]=gc_FATTempBufferHB;
				XBYTE[0xB3B0]=0x01;
				while(!(XBYTE[0xB3C0]&0x01));
				XBYTE[0xB3C0]=0x00;
			}

			if(gc_DOS_FileSystemType==2)
			{
				tw_temp=(gs_File_FCB[1].dw_File_CurrentCluster&~0x7F)+0x80;
				tw_Addr=(gs_File_FCB[1].dw_File_CurrentCluster&0x7F)<<gc_DOS_FileSystemType;
				gc_FATTempBuffer[tw_Addr+0]=((U8 *)(&tdw_FreeCluster))[3];
				gc_FATTempBuffer[tw_Addr+1]=((U8 *)(&tdw_FreeCluster))[2];
				gc_FATTempBuffer[tw_Addr+2]=((U8 *)(&tdw_FreeCluster))[1];
				gc_FATTempBuffer[tw_Addr+3]=((U8 *)(&tdw_FreeCluster))[0];
			}
			else
			{
				tw_temp=(gs_File_FCB[1].dw_File_CurrentCluster&~0xFF)+0x100;
				tw_Addr=(gs_File_FCB[1].dw_File_CurrentCluster&0xFF)<<gc_DOS_FileSystemType;
				gc_FATTempBuffer[tw_Addr+0]=((U8 *)(&tdw_FreeCluster))[3];
				gc_FATTempBuffer[tw_Addr+1]=((U8 *)(&tdw_FreeCluster))[2];
			}

			if(tdw_FreeCluster>=tw_temp)
			{
				// gc_FATTempBuffer[] DMA to gc_UserDataBuf[]
				XBYTE[0xB301]=0x00;
				XBYTE[0xB112]=0x00;
				XBYTE[0xB113]=gc_FATTempBufferHB;
				XBYTE[0xB114]=0x00;
				XBYTE[0xB115]=gc_UserDataBufHB;
				XBYTE[0xB3B0]=0x01;
				while(!(XBYTE[0xB3C0]&0x01));
				XBYTE[0xB3C0]=0x00;
				DOS_Write_LogicSector(gdw_DOS_Fat1Addr+tdw_CurrentFATSector,3);
				DOS_Write_LogicSector(gdw_DOS_Fat2Addr+tdw_CurrentFATSector,3);
			}

			// update file current cluster and file pointer
			gs_File_FCB[1].dw_File_CurrentCluster=tdw_FreeCluster;
			gs_File_FCB[1].dw_File_DataPoint=0;
		}
	}	
	//calculate logic address of current file pointer
	gdw_LogicSectorAddr=DOS_ClusterLogicAddr(gs_File_FCB[1].dw_File_CurrentCluster);
	gdw_LogicSectorAddr=gdw_LogicSectorAddr+(gs_File_FCB[1].dw_File_DataPoint>>9);

	//write a sector to disk
  	gb_ReadWriteDataArea=0;
	tc_Status=DOS_Write_LogicSector(gdw_LogicSectorAddr,tc_AppendWr);
	gs_File_FCB[1].dw_File_DataPoint+=512;	//file pointer add 
	gs_File_FCB[1].dw_File_TotalSize+=512;	//file size add
	return tc_Status;
}
