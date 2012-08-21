#include <string.h>
#include "..\header\SPDA2K.h"
#include "..\header\variables.h"


U8 DOS_ConnectFATChain(U32 tdw_StartCluster,U32 tdw_TailCluster)
{
	data	bit	tb_End;
	data	bit	tb_ChainEndFlag=0;
	data	U8	tc_Status;
	xdata	U16	tw_ValidPoint;
	xdata	U16	tw_ValidSize;
	xdata	U16	tw_ArrayNum;
	xdata	U16	tw_Offset;
	data	U8	tc_ConnectMode;
	xdata	U32	tdw_CopyStartCluster;
	xdata	U32	tdw_CopyEndCluster;
	xdata	U32 tdw_CurrentCluster;
	xdata	U32 tdw_NextCluster;
	xdata	U32	tdw_CurrentSector;
	xdata	U32 tdw_NextSector;
	xdata	U32 tdw_TempBuffer[8];
	xdata	U32	tdw_FreeClusterNum;

	//backup DOS_SearchFreeCluster's global variable to temp
	memcpy(tdw_TempBuffer,gdw_ClusterBuffer,32);
	tw_ValidPoint=gw_ClusBufValidPoint;
	tw_ValidSize=gw_ClusBufValidSize;
	tdw_FreeClusterNum=gdw_FreeClusterNum[0];

	//confirm FAT copy end address
	if(tdw_StartCluster>tdw_TailCluster)
	{
		tdw_CopyEndCluster=tdw_StartCluster;	
	}
	else
	{
		tdw_CopyEndCluster=tdw_TailCluster;	
	}
	
	if(tdw_StartCluster==tdw_TailCluster)
	{//only connect 0xffffffff to StartCluster
		tc_ConnectMode=0;
		tdw_CopyStartCluster=tdw_StartCluster;		//confirm start address of FAT Copy 
	}
	else if(DOS_GetNextCluster(tdw_StartCluster,1)==0)
	{//Create new file's FAT chain  
		tc_ConnectMode=1;
		tdw_CopyStartCluster=tdw_StartCluster;		//confirm start address of FAT Copy				
	}
	else
	{//Connect to exist file's FAT chain
		tc_ConnectMode=2;
		tdw_CopyStartCluster=2;						//confirm start address of FAT Copy
	}
	tdw_CurrentCluster=tdw_StartCluster;
	DOS_MarkFATUpdate(gdw_DOS_Fat1Addr,0xaa);			//mark FAT1 is updating now

	tb_End=0;
	tdw_CurrentSector=(tdw_CurrentCluster<<gc_DOS_FileSystemType)>>9;
	gb_ReadWriteDataArea=0;
	tc_Status=DOS_Read_LogicSector(gdw_DOS_Fat1Addr+tdw_CurrentSector);
	if(tc_Status)
	{
		return tc_Status;
	}
	//do connect chain

	while(!tb_ChainEndFlag)
	{//search free cluster initialize
		gw_ClusBufValidPoint=0;
		gw_ClusBufValidSize=1;
	    	
		if(tc_ConnectMode==2)						//(JC)Connect to exist file's FAT chain
		{//search free cluster from start
			gdw_ClusterBuffer[1]=tdw_TailCluster;
		}
		else if(tc_ConnectMode==1)					//Create new file's FAT chain 
		{
			gdw_FreeClusterNum[0]=tdw_CurrentCluster;
			while((gw_ClusBufValidSize<512)&&(gdw_FreeClusterNum[0]<(gdw_DOS_FatMaxCluster-1))) 
			{
				DOS_SearchFreeCluster(0);
			}
		}
				
		for(tw_ArrayNum=1;tw_ArrayNum<512;tw_ArrayNum++)
		{
			tw_Offset=(tdw_CurrentCluster<<gc_DOS_FileSystemType)&0x01FF;		
			if (tdw_CurrentCluster==tdw_TailCluster)
			{//final a cluster chain
				tdw_NextCluster=0xFFFFFFFF;
			}
			else
			{//get next cluster chain value
				tdw_NextCluster=gdw_ClusterBuffer[tw_ArrayNum];
			}
			//write next cluster number into previous cluster entry
			gc_PlayRecordDataBuf[tw_Offset]=((U8 *)(&tdw_NextCluster))[3];
			gc_PlayRecordDataBuf[tw_Offset+1]=((U8 *)(&tdw_NextCluster))[2];
			if(gc_DOS_FileSystemType==0x02)
			{//FAT32
				gc_PlayRecordDataBuf[tw_Offset+2]=((U8 *)(&tdw_NextCluster))[1];
				gc_PlayRecordDataBuf[tw_Offset+3]=((U8 *)(&tdw_NextCluster))[0];
			}

			if(tdw_NextCluster==0xFFFFFFFF)
			{
				tb_End=1;
				tb_ChainEndFlag=1;
				break;
			}
			tdw_NextSector=(tdw_NextCluster<<gc_DOS_FileSystemType)>>9;
			tdw_CurrentCluster=tdw_NextCluster;
		  	  	
			if(tdw_NextSector!=tdw_CurrentSector)
			{
				tb_End=0;
				gb_ReadWriteDataArea=0;
				tc_Status=DOS_Write_LogicSector(gdw_DOS_Fat1Addr+tdw_CurrentSector,3);
				if(tc_Status)
				{
					return tc_Status;
				}
				tc_Status=DOS_Write_LogicSector(gdw_DOS_Fat2Addr+tdw_CurrentSector,3);
				if(tc_Status)
				{
					return tc_Status;
				}

				tdw_CurrentSector=tdw_NextSector;
				tc_Status=DOS_Read_LogicSector(gdw_DOS_Fat1Addr+tdw_NextSector);
				if(tc_Status)
				{
					return tc_Status;
				}
			}
		}
		
		if(tb_End)
		{
			gb_ReadWriteDataArea=0;
			tc_Status=DOS_Write_LogicSector(gdw_DOS_Fat1Addr+tdw_CurrentSector,3);
			if (tc_Status)
			{
				return tc_Status;
			}
			tc_Status=DOS_Write_LogicSector(gdw_DOS_Fat2Addr+tdw_CurrentSector,3);
			if (tc_Status)
			{
				return tc_Status;
			}
		}
	}

	DOS_MarkFATUpdate(gdw_DOS_Fat1Addr,0xF8);				//mark updating is complete
	memcpy(gdw_ClusterBuffer,tdw_TempBuffer,32);
	gw_ClusBufValidPoint=tw_ValidPoint;
	gw_ClusBufValidSize=tw_ValidSize;
	gdw_FreeClusterNum[0]=tdw_FreeClusterNum;
	return tc_Status;
}

U32 GetValidClusterCnt(void)
{
	U32 tdw_ClusterCnt;

	tdw_ClusterCnt=gdw_ClusterBuffer[gw_ClusBufValidPoint];//get a cluster content
	gw_ClusBufValidPoint++;
	if(gw_ClusBufValidPoint>=512)
	{				
		gw_ClusBufValidPoint=0;							//if array full,reset the point
	}
	gw_ClusBufValidSize--;								//the number of valid cluster minus 1
	return 	tdw_ClusterCnt;
}


U32 DOS_GetAFreeCluster(void)
{
	if((gw_ClusBufValidSize<512)&&(gdw_FreeClusterNum[0]<(gdw_DOS_FatMaxCluster-1))) 
	{
		DOS_SearchFreeCluster(0);//search cluster from flash fat table
	}
		  	
	if(gw_ClusBufValidSize==0)//(JC)search none
	{
		return 0;
	}
	else
	{
		return GetValidClusterCnt();
	}
}


//---------------------------------------------------------------
//! Mark the 1st byte of FAT
/*! Mark the 1st byte of FAT sector while updating.
 *	\param	tdw_MarkFATAddr		The FAT LBA.
 *	\param	tc_MarkValue		The byte mark to be written.
 *	\return	DOS_SUCCESS(0) or DOS_FALSE(1)
 */
U8 DOS_MarkFATUpdate(U32 tdw_MarkFATAddr,U8 tc_MarkValue)
{
	xdata	U8	tc_Status;
		
	gb_ReadWriteDataArea=0;							//select gc_PlayRecordDataBuf[]
	tc_Status=DOS_Read_LogicSector(tdw_MarkFATAddr);//read sector
	if(tc_Status==0)
	{
		gc_PlayRecordDataBuf[0]=tc_MarkValue;			//mark first byte
		tc_Status=DOS_Write_LogicSector(tdw_MarkFATAddr,3);//write sector	
	}
	return tc_Status;
}

bit CreateFolder_VOICE(void)
{
	data	U32	tdw_ClusterCnt;
	data	U32	tdw_ClusterCnt1;
	data	U32	tdw_SectorCnt;
	xdata	U8	i;
	xdata	U16	tw_Addr;
	xdata	U32	tdw_FATValue;
	idata	SearchFdb *p_mp;
	idata	SearchFdb temp;

	p_mp=&temp;
	p_mp->c_Search_Mode=K_FIND_FREE_FDB;
	p_mp->dw_FDB_StartCluster=gdw_DOS_RootDirClus;
	p_mp->c_Search_Attribute=0;
	if(Find_Fdb(p_mp))	//no free fdb
	{
		if(gs_File_FCB[1].dw_FDB_StartCluster<2&&gc_DOS_FileSystemType==1)	// find no free FDB in root dir entry(FAT16)
		{//if root dir is full
			return 1;
		}

		tdw_ClusterCnt=DOS_GetAFreeCluster();	//get a free cluster for FDB(FAT32)
		if(!tdw_ClusterCnt)
		{
			return 1;
		}
		else
		{
			tdw_ClusterCnt1=gdw_DOS_RootDirClus;
			// Connect FAT chain for Add new cluster of Root DIR
			while(1)
			{
				tdw_SectorCnt=gdw_DOS_Fat1Addr+tdw_ClusterCnt1/(512/(1<<gc_DOS_FileSystemType));
				DOS_Read_LogicSector(tdw_SectorCnt);
				tw_Addr=(tdw_ClusterCnt1%(512/(1<<gc_DOS_FileSystemType)))*(1<<gc_DOS_FileSystemType);
				tdw_FATValue=gc_UserDataBuf[tw_Addr]+(gc_UserDataBuf[tw_Addr+1]<<8);

				if(gc_DOS_FileSystemType==1)
				{
					tdw_FATValue=gc_UserDataBuf[tw_Addr+1];
					tdw_FATValue=(tdw_FATValue<<8)+gc_UserDataBuf[tw_Addr+0];
					if(tdw_FATValue==0xFFFF)
					{
						gc_UserDataBuf[tw_Addr+0]=tdw_ClusterCnt;
						gc_UserDataBuf[tw_Addr+1]=tdw_ClusterCnt>>8;
						break;
					}
				}
				else
				{
					tdw_FATValue=gc_UserDataBuf[tw_Addr+3];
					tdw_FATValue=(tdw_FATValue<<8)+gc_UserDataBuf[tw_Addr+2];
					tdw_FATValue=(tdw_FATValue<<8)+gc_UserDataBuf[tw_Addr+1];
					tdw_FATValue=(tdw_FATValue<<8)+gc_UserDataBuf[tw_Addr+0];
					if(tdw_FATValue>=0x0FFFFFFF)
					{
						gc_UserDataBuf[tw_Addr+0]=tdw_ClusterCnt;
						gc_UserDataBuf[tw_Addr+1]=tdw_ClusterCnt>>8;
						gc_UserDataBuf[tw_Addr+2]=tdw_ClusterCnt>>16;
						gc_UserDataBuf[tw_Addr+3]=tdw_ClusterCnt>>24;
						break;
					}
				}
			}

			// Write FAT1 & FAT2(Root DIR)
			DOS_Write_LogicSector(tdw_SectorCnt,3);
			DOS_Write_LogicSector(tdw_SectorCnt-gdw_DOS_Fat1Addr+gdw_DOS_Fat2Addr,3);
			tdw_SectorCnt=gdw_DOS_Fat1Addr+tdw_ClusterCnt/(512/(1<<gc_DOS_FileSystemType));
			DOS_Read_LogicSector(tdw_SectorCnt);
			tw_Addr=(tdw_ClusterCnt%(512/(1<<gc_DOS_FileSystemType)))*(1<<gc_DOS_FileSystemType);
			gc_UserDataBuf[tw_Addr+0]=0xFF;
			gc_UserDataBuf[tw_Addr+1]=0xFF;
			if(gc_DOS_FileSystemType==2)
			{
				gc_UserDataBuf[tw_Addr+2]=0xFF;
				gc_UserDataBuf[tw_Addr+3]=0x0F;
			}
			DOS_Write_LogicSector(tdw_SectorCnt,3);
			DOS_Write_LogicSector(tdw_SectorCnt-gdw_DOS_Fat1Addr+gdw_DOS_Fat2Addr,3);

			// Clear New Root FDB Area & Fill "VOICE" folder data into Root FDB
			tdw_SectorCnt=(tdw_ClusterCnt-2)*gc_DOS_SectorPerCluster+gdw_DOS_DataAddr;
			tdw_ClusterCnt1=DOS_GetAFreeCluster();
			for(i=0;i<gc_DOS_SectorPerCluster;i++)
			{
				if(i==0)
				{
					memset(&gc_UserDataBuf[0],0,512);
					gc_UserDataBuf[ 0]='V';	gc_UserDataBuf[ 1]='O';	gc_UserDataBuf[ 2]='I';
					gc_UserDataBuf[ 3]='C';	gc_UserDataBuf[ 4]='E';	gc_UserDataBuf[ 5]=' ';
					gc_UserDataBuf[ 6]=' ';	gc_UserDataBuf[ 7]=' ';	gc_UserDataBuf[ 8]=' ';
					gc_UserDataBuf[ 9]=' ';	gc_UserDataBuf[10]=' ';	gc_UserDataBuf[11]=0x10;
					gc_UserDataBuf[14]=gw_DOS_CreateTime;
					gc_UserDataBuf[15]=gw_DOS_CreateTime>>8;
					gc_UserDataBuf[16]=gw_DOS_CreateDate;
					gc_UserDataBuf[17]=gw_DOS_CreateDate>>8;
					gc_UserDataBuf[18]=gw_DOS_CreateDate;
					gc_UserDataBuf[19]=gw_DOS_CreateDate>>8;
					gc_UserDataBuf[20]=tdw_ClusterCnt1>>16;
					gc_UserDataBuf[21]=tdw_ClusterCnt1>>24;
					gc_UserDataBuf[22]=gw_DOS_CreateTime;
					gc_UserDataBuf[23]=gw_DOS_CreateTime>>8;
					gc_UserDataBuf[24]=gw_DOS_CreateDate;
					gc_UserDataBuf[25]=gw_DOS_CreateDate>>8;
					gc_UserDataBuf[26]=tdw_ClusterCnt1;
					gc_UserDataBuf[27]=tdw_ClusterCnt1>>8;
					DOS_Write_LogicSector(tdw_SectorCnt,0);
					memset(&gc_UserDataBuf[0],0,32);

					if(gc_DOS_SectorPerCluster==1)
						DOS_Write_LogicSector(tdw_SectorCnt,4);
				}
				else if(i==gc_DOS_SectorPerCluster-1)
				{
					DOS_Write_LogicSector(tdw_SectorCnt+i,2);
				}
				else
				{
					DOS_Write_LogicSector(tdw_SectorCnt+i,1);
				}
			}

			// Clear "VOICE" folder FDB area
			tdw_SectorCnt=(tdw_ClusterCnt1-2)*gc_DOS_SectorPerCluster+gdw_DOS_DataAddr;
			for(i=0;i<gc_DOS_SectorPerCluster;i++)
			{
				if(i==0)
				{
					gs_File_FCB[1].dw_FDB_Cluster=tdw_ClusterCnt1;
					memset(&gc_UserDataBuf[0],0,512);
					FillFDBHeader();	// . & ..

					if(gc_DOS_SectorPerCluster==1)
						DOS_Write_LogicSector(tdw_SectorCnt,3);
					else
					{
						DOS_Write_LogicSector(tdw_SectorCnt,0);
						memset(&gc_UserDataBuf[0],0,64);
					}
				}
				else if(i==gc_DOS_SectorPerCluster-1)
				{
					DOS_Write_LogicSector(tdw_SectorCnt+i,2);
				}
				else
				{
					DOS_Write_LogicSector(tdw_SectorCnt+i,1);
				}
			}

			// Fill FAT of "VOICE" folder
			tdw_SectorCnt=gdw_DOS_Fat1Addr+tdw_ClusterCnt1/(512/(1<<gc_DOS_FileSystemType));
			tw_Addr=(tdw_ClusterCnt1%(512/(1<<gc_DOS_FileSystemType)))*(1<<gc_DOS_FileSystemType);
			DOS_Read_LogicSector(tdw_SectorCnt);
			gc_UserDataBuf[tw_Addr+0]=0xFF;
			gc_UserDataBuf[tw_Addr+1]=0xFF;
			if(gc_DOS_FileSystemType==2)
			{
				gc_UserDataBuf[tw_Addr+2]=0xFF;
				gc_UserDataBuf[tw_Addr+3]=0x0F;
			}
			DOS_Write_LogicSector(tdw_SectorCnt,3);
			DOS_Write_LogicSector(tdw_SectorCnt-gdw_DOS_Fat1Addr+gdw_DOS_Fat2Addr,3);
			return 0;
		}
	}
	else 
	{//(JC)Get a free FDB, info stored in p_mp
		gs_File_FCB[1].dw_FDB_Cluster=p_mp->dw_FDB_Cluster;//(JC)File FDB start cluster
		DOS_SearchFreeCluster(0);//search cluster from flash fat table
		tdw_ClusterCnt=DOS_GetAFreeCluster();	//(JC)Get a free cluster for file content
		if(!tdw_ClusterCnt)
		{
			return 1;
		}
		else
		{
			gs_File_FCB[1].dw_File_StartCluster=tdw_ClusterCnt;
			gs_File_FCB[1].dw_FDB_LogAdd=p_mp->dw_FDB_LogAdd;
			gs_File_FCB[1].w_FDB_Offset=p_mp->w_FDB_Offset;
			DOS_Read_LogicSector(p_mp->dw_FDB_LogAdd);	
			gc_UserDataBuf[p_mp->w_FDB_Offset+ 0]='V';
			gc_UserDataBuf[p_mp->w_FDB_Offset+ 1]='O';
			gc_UserDataBuf[p_mp->w_FDB_Offset+ 2]='I';
			gc_UserDataBuf[p_mp->w_FDB_Offset+ 3]='C';
			gc_UserDataBuf[p_mp->w_FDB_Offset+ 4]='E';
			gc_UserDataBuf[p_mp->w_FDB_Offset+ 5]=' ';
			gc_UserDataBuf[p_mp->w_FDB_Offset+ 6]=' ';
			gc_UserDataBuf[p_mp->w_FDB_Offset+ 7]=' ';
			gc_UserDataBuf[p_mp->w_FDB_Offset+ 8]=' ';
			gc_UserDataBuf[p_mp->w_FDB_Offset+ 9]=' ';
			gc_UserDataBuf[p_mp->w_FDB_Offset+10]=' ';
			gc_UserDataBuf[p_mp->w_FDB_Offset+11]=0x10;
			gc_UserDataBuf[p_mp->w_FDB_Offset+12]=0x00;
			gc_UserDataBuf[p_mp->w_FDB_Offset+13]=0x00;
			gc_UserDataBuf[p_mp->w_FDB_Offset+14]=gw_DOS_CreateTime;
			gc_UserDataBuf[p_mp->w_FDB_Offset+15]=gw_DOS_CreateTime>>8;
			gc_UserDataBuf[p_mp->w_FDB_Offset+16]=gw_DOS_CreateDate;
			gc_UserDataBuf[p_mp->w_FDB_Offset+17]=gw_DOS_CreateDate>>8;
			gc_UserDataBuf[p_mp->w_FDB_Offset+18]=gw_DOS_CreateDate;
			gc_UserDataBuf[p_mp->w_FDB_Offset+19]=gw_DOS_CreateDate>>8;
			gc_UserDataBuf[p_mp->w_FDB_Offset+20]=tdw_ClusterCnt>>16;
			gc_UserDataBuf[p_mp->w_FDB_Offset+21]=tdw_ClusterCnt>>24;
			gc_UserDataBuf[p_mp->w_FDB_Offset+22]=gw_DOS_CreateTime;
			gc_UserDataBuf[p_mp->w_FDB_Offset+23]=gw_DOS_CreateTime>>8;
			gc_UserDataBuf[p_mp->w_FDB_Offset+24]=gw_DOS_CreateDate;
			gc_UserDataBuf[p_mp->w_FDB_Offset+25]=gw_DOS_CreateDate>>8;
			gc_UserDataBuf[p_mp->w_FDB_Offset+26]=tdw_ClusterCnt;
			gc_UserDataBuf[p_mp->w_FDB_Offset+27]=tdw_ClusterCnt>>8;
			gc_UserDataBuf[p_mp->w_FDB_Offset+28]=0x00;
			gc_UserDataBuf[p_mp->w_FDB_Offset+29]=0x00;
			gc_UserDataBuf[p_mp->w_FDB_Offset+30]=0x00;
			gc_UserDataBuf[p_mp->w_FDB_Offset+31]=0x00;
			DOS_Write_LogicSector(p_mp->dw_FDB_LogAdd,3);

			tdw_SectorCnt=tdw_ClusterCnt*(1<<gc_DOS_FileSystemType)/512;
			DOS_Read_LogicSector(gdw_DOS_Fat1Addr+tdw_SectorCnt);
			tw_Addr=(tdw_ClusterCnt%(512/(1<<gc_DOS_FileSystemType)))*(1<<gc_DOS_FileSystemType);
			gc_UserDataBuf[tw_Addr+0]=0xFF;
			gc_UserDataBuf[tw_Addr+1]=0xFF;	
			if(gc_DOS_FileSystemType==2)
			{
				gc_UserDataBuf[tw_Addr+2]=0xFF;
				gc_UserDataBuf[tw_Addr+3]=0x0F;
			}
			DOS_Write_LogicSector(gdw_DOS_Fat1Addr+tdw_SectorCnt,3);
			DOS_Write_LogicSector(gdw_DOS_Fat2Addr+tdw_SectorCnt,3);
			// Clear FDB area
			tdw_SectorCnt=(tdw_ClusterCnt-2)*gc_DOS_SectorPerCluster+gdw_DOS_DataAddr;

			for(i=0;i<gc_DOS_SectorPerCluster;i++)
			{
				if(i==0)
				{
					gs_File_FCB[1].dw_FDB_Cluster=tdw_ClusterCnt;
					memset(&gc_UserDataBuf[0],0,512);
					FillFDBHeader();	// . & ..
					if(gc_DOS_SectorPerCluster==1)
						DOS_Write_LogicSector(tdw_SectorCnt,3);
					else
					{
						DOS_Write_LogicSector(tdw_SectorCnt,0);
						memset(&gc_UserDataBuf[0],0,64);
					}
				}
				else if(i==gc_DOS_SectorPerCluster-1)
				{
					DOS_Write_LogicSector(tdw_SectorCnt+i,2);
				}
				else
				{
					DOS_Write_LogicSector(tdw_SectorCnt+i,1);
				}
			}
		}
	}
	return 0;
}


void FillFDBHeader(void)
{
	gc_UserDataBuf[ 0]='.';	gc_UserDataBuf[ 1]=' ';	gc_UserDataBuf[ 2]=' ';	gc_UserDataBuf[ 3]=' ';
	gc_UserDataBuf[ 4]=' ';	gc_UserDataBuf[ 5]=' ';	gc_UserDataBuf[ 6]=' ';	gc_UserDataBuf[ 7]=' ';
	gc_UserDataBuf[ 8]=' ';	gc_UserDataBuf[ 9]=' ';	gc_UserDataBuf[10]=' ';	gc_UserDataBuf[11]=0x10;
	gc_UserDataBuf[14]=gw_DOS_CreateTime;	gc_UserDataBuf[15]=gw_DOS_CreateTime>>8;
	gc_UserDataBuf[16]=gw_DOS_CreateDate;	gc_UserDataBuf[17]=gw_DOS_CreateDate>>8;
	gc_UserDataBuf[18]=gw_DOS_CreateDate;	gc_UserDataBuf[19]=gw_DOS_CreateDate>>8;
	gc_UserDataBuf[20]=gs_File_FCB[1].dw_File_StartCluster>>16;
	gc_UserDataBuf[21]=gs_File_FCB[1].dw_File_StartCluster>>24;
	gc_UserDataBuf[22]=gw_DOS_CreateTime;	gc_UserDataBuf[23]=gw_DOS_CreateTime>>8;
	gc_UserDataBuf[24]=gw_DOS_CreateDate;	gc_UserDataBuf[25]=gw_DOS_CreateDate>>8;
	gc_UserDataBuf[26]=gs_File_FCB[1].dw_File_StartCluster;
	gc_UserDataBuf[27]=gs_File_FCB[1].dw_File_StartCluster>>8;
	
	gc_UserDataBuf[32]='.';	gc_UserDataBuf[33]='.';	gc_UserDataBuf[34]=' ';	gc_UserDataBuf[35]=' ';
	gc_UserDataBuf[36]=' ';	gc_UserDataBuf[37]=' ';	gc_UserDataBuf[38]=' ';	gc_UserDataBuf[39]=' ';
	gc_UserDataBuf[40]=' ';	gc_UserDataBuf[41]=' ';	gc_UserDataBuf[42]=' ';	gc_UserDataBuf[43]=0x10;
	gc_UserDataBuf[46]=gw_DOS_CreateTime;	gc_UserDataBuf[47]=gw_DOS_CreateTime>>8;
	gc_UserDataBuf[48]=gw_DOS_CreateDate;	gc_UserDataBuf[49]=gw_DOS_CreateDate>>8;
	gc_UserDataBuf[50]=gw_DOS_CreateDate;	gc_UserDataBuf[51]=gw_DOS_CreateDate>>8;
	gc_UserDataBuf[54]=gw_DOS_CreateTime;	gc_UserDataBuf[55]=gw_DOS_CreateTime>>8;
	gc_UserDataBuf[56]=gw_DOS_CreateDate;	gc_UserDataBuf[57]=gw_DOS_CreateDate>>8;	
}
