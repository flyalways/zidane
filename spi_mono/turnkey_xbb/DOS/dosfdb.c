#include <string.h>
#include "..\header\SPDA2K.h"
#include "..\header\Rambase.h"
#include "..\header\variables.h"


U8 Find_Fdb(SearchFdb *p_mp)
{
	data	bit	tc_RecFlag;
	data	bit	tb_flag=0;
	data	bit	tb_SortFlag=0;
	data	bit	tb_SameFlag=0;
	data	bit	tb_not_1stFDB=0;
	data	bit	tb_DiffFlag;
	data	U8	tc_sts;
	data	U8	tc_SectorOffset;
	data	U8	tc_Temp_SectorOffset;
	data	U8	tc_PlayMode;
	data	U8	tc_ExtNum;
	data	U16	tw_FdbOffset;
	data	U16	tw_Temp_FdbOffset;
	data	U16	tw_FileLongName;
	data	U16	tw_ByteOffset;
	data	U16	tw_PrevFDB_Offset;
	data	U16	tw_DirIndex=0;
	data	U16	tw_DirIndex1=0;
	xdata	U32 tdw_SectorAdd,tdw_StartCluster,tdw_S_ClusterNum,tdw_FDBClusterNum;
	xdata	U32 tdw_Find_ClusterSize,tdw_TheEndCluster,tdw_StartCluster1,tdw_StartCluster3,tdw_PrevFDB_Cluster, tdw_PrevFDB_LogAdd;
	xdata	U32 tdw_FDB_LogAdd0,tdw_FDB_LogAdd1;
	xdata	U32 tdw_Temp_flag=0;
	xdata	U32 tdw_Tag0,tdw_Tag1;
	data	U16	tw_FileSkipNumber;
	code	U8	REC[]={'R','E','C','_'};

	tw_FileSkipNumber=gw_FileSkipNumber;
	tdw_Tag1=p_mp->Compare.dw_BubbleFlag;
	if(p_mp->c_Search_Direction) //find pre
	{
		tdw_Tag0=0x0000;		
	}
	else
	{//find next
		tdw_Tag0=0xffffffff;
	}

	if((p_mp->dw_FDB_StartCluster==0)&&(gdw_DOS_RootDirClus==2)) //FAT32, 20090216 chiayen add
	{
		p_mp->dw_FDB_StartCluster=2;
	}

	tdw_FDBClusterNum=p_mp->dw_FDB_StartCluster;
	p_mp->dw_LongFDB_LogAdd1=0;
	p_mp->dw_LongFDB_LogAdd0=0;	
	tw_Temp_FdbOffset=0x00;
	tc_Temp_SectorOffset=0x00;
	tdw_FDB_LogAdd0=0;
	tdw_FDB_LogAdd1=0;
	tdw_S_ClusterNum=p_mp->dw_FDB_StartCluster;	
	tc_sts=GO_ON;

	if(gc_DOS_FileSystemType==1)
	{
		tdw_TheEndCluster=0xffff;
	}
	else
	{
		tdw_TheEndCluster=0x0fffffff; 
	}

	while(tdw_S_ClusterNum<tdw_TheEndCluster)
	{ //find in one	dir
		if((gc_CurrentCard==2)&&(gb_SD_pin==1))
		{
			return 1;
		}

		if((gc_CurrentCard==5)&&(gb_HostConnect==0))
		{			
			return 1;		
		}

		if(!tdw_S_ClusterNum)		 					     //Is rootdir
		{
			tdw_Find_ClusterSize=(gdw_DOS_DataAddr-gdw_DOS_RootDirAddr);
			tdw_SectorAdd=gdw_DOS_RootDirAddr;		
		}
		else
		{
			tdw_Find_ClusterSize=gc_DOS_SectorPerCluster;	
			tdw_SectorAdd=DOS_ClusterLogicAddr(tdw_S_ClusterNum);		
		}

		if(tdw_S_ClusterNum!=p_mp->dw_FDB_Cluster)
		{
			tc_SectorOffset=0;	// when one cluster is(tw_FdbOffset)finished,fdboffset should be 0
		}

		for(tc_SectorOffset=tc_Temp_SectorOffset;tc_SectorOffset<tdw_Find_ClusterSize*gc_HostSectorUnit;tc_SectorOffset++)
		{//find	in one cluster 	
			gb_ReadWriteDataArea=1;
			if(DOS_Read_LogicSector(tdw_SectorAdd+tc_SectorOffset)!=0)
			{//read sector error(1 is fail)
				break;
			}

			if(tc_SectorOffset!=tc_Temp_SectorOffset)//when one Sector is(tw_FdbOffset)finished,fdboffset should be 0
			{
				tw_Temp_FdbOffset=0;
			}

			for(tw_FdbOffset=tw_Temp_FdbOffset;tw_FdbOffset<512;tw_FdbOffset+=32)
			{//Find in one sector			
				tc_PlayMode=0;				
				tc_RecFlag=1;
				tw_DirIndex++;
				if(gc_UserDataBuf[tw_FdbOffset]==0xE5)
				{
					if(p_mp->c_Search_Mode==K_FIND_FREE_FDB)
					{
						tc_sts=SUCCESS;
					} 				
				}
				else if(gc_UserDataBuf[tw_FdbOffset]==0x00)
				{
					if((p_mp->c_Search_Mode==K_FIND_FREE_FDB)||(p_mp->c_Search_Mode==K_FIND_LAST_FDB))
					{
						tc_sts=SUCCESS;
					}
					else
					{					
						tc_sts=FAIL;						//The dir is end flag
					}			
				}
				else if(gc_UserDataBuf[tw_FdbOffset+0x0b]==0x0f)//is longfdb
				{
					if(p_mp->c_Search_Mode==K_SPECIFIC_LONG_FILENAME)//find by long filename
					{
						if(gc_UserDataBuf[tw_FdbOffset]&0x40)//IS it the last longfdb?
						{						
							if(gc_UserDataBuf[tw_FdbOffset]==p_mp->pc_LongFileName[0])
							{					
								if (*(p_mp->pc_LongFileName+1) != 0)
								{
									if((gc_UserDataBuf[tw_FdbOffset+Unicode_Offset[*(p_mp->pc_LongFileName+1)-1]]==0x00) && (gc_UserDataBuf[tw_FdbOffset+Unicode_Offset[*(p_mp->pc_LongFileName+1)-2]]==0x00))
									{
										goto MatchEXTName;
									}
								}
								else
								{//character NUM is equal
MatchEXTName:
									((U8 *)(&tw_FileLongName))[0] = *(p_mp->pc_LongFileName+3);
									((U8 *)(&tw_FileLongName))[1] = *(p_mp->pc_LongFileName+4);
									tw_FileLongName+=5;
							
									for(tw_ByteOffset=0;tw_ByteOffset<(26-(*(p_mp->pc_LongFileName+1)));tw_ByteOffset++)
									{//compare the last longfdb
										if(gc_UserDataBuf[tw_FdbOffset+Unicode_Offset[(*(p_mp->pc_LongFileName+1))+tw_ByteOffset]]!=p_mp->pc_LongFileName[tw_FileLongName-tw_ByteOffset])  //last longfdb is not equal
										{
											if ((gc_UserDataBuf[tw_FdbOffset+Unicode_Offset[(*(p_mp->pc_LongFileName+1))+tw_ByteOffset]] >= 'a') && (gc_UserDataBuf[tw_FdbOffset+Unicode_Offset[(*(p_mp->pc_LongFileName+1))+tw_ByteOffset]] <= 'z'))
											{//if current char is lower letter,try to compare after converting to upper letter
												if (gc_UserDataBuf[tw_FdbOffset+Unicode_Offset[(*(p_mp->pc_LongFileName+1))+tw_ByteOffset]]!=(p_mp->pc_LongFileName[tw_FileLongName-tw_ByteOffset] + 0x20))
												{
													break;
												}
											}
											else
											{
												break;
											}										
										}
									}
									if (tw_ByteOffset==26-*(p_mp->pc_LongFileName+1))//the last longfdb is equal
									{
										if((gc_UserDataBuf[tw_FdbOffset]&0x3f)==1)//current longfdb is the first 
										{
											tc_sts=SUCCESS;
										}
										else
										{
											tw_FileLongName-=tw_ByteOffset;
										}
									}
								}
							}
						}
						else
						{//not the	"last" longfdb
							for(tw_ByteOffset=0;tw_ByteOffset<26;tw_ByteOffset++)
							{//compare the longfdb which is not the last
								if(gc_UserDataBuf[tw_FdbOffset+Unicode_Offset[tw_ByteOffset]]!=p_mp->pc_LongFileName[tw_FileLongName-tw_ByteOffset])
								{
									if ((gc_UserDataBuf[tw_FdbOffset+Unicode_Offset[tw_ByteOffset]] >= 'a') && (gc_UserDataBuf[tw_FdbOffset+Unicode_Offset[tw_ByteOffset]] <= 'z'))
									{//if current char is lower letter,try to compare after converting to upper letter
										if (gc_UserDataBuf[tw_FdbOffset+Unicode_Offset[tw_ByteOffset]]!=(p_mp->pc_LongFileName[tw_FileLongName-tw_ByteOffset] + 0x20))
										{
											break;
										}
									}
									else
									{
										break;
									}		
								}
							}
							if(tw_ByteOffset==26)//equal  			
							{
								tw_FileLongName-=tw_ByteOffset;//the length of gc_FileLongName[] minus 26
								if((gc_UserDataBuf[tw_FdbOffset]&0x3f)==1)
								{//current longfdb is the first									
									tc_sts=SUCCESS;
								} 						
							}
						}
					}
				}
				else
				{//short_fdb
					tw_FileLongName=*(p_mp->pc_LongFileName+4);
					if(gc_UserDataBuf[tw_FdbOffset+0x0b]&0x10)//2005/05/17 modify
					{//is dir
						if(p_mp->c_Search_Attribute)
						{//find dir
							if(gc_UserDataBuf[tw_FdbOffset]!=0x2e)
							{
								if(((p_mp->c_Search_Mode==K_FIND_DIR_EXTNAME)||(p_mp->c_Search_Mode==K_SPECIFIC_SHORT_FILENAME)))
								{
									goto EXTNAMECOMPARE;
							   	}
								else
								{
									tb_flag=1; //the flag which determine if implement the flowing program
							   	}
							}						
						}
						else if(gb_CountDirFlag && (p_mp->c_Search_Mode != K_SPECIFIC_STARTCLUSTER))
						{
							if(gc_UserDataBuf[tw_FdbOffset]!=0x2e) //. & ..
								gw_DirTotalNumber++; 
						}
					}
					else if(!(p_mp->c_Search_Attribute))
					{//not to find dir
						if((gc_UserDataBuf[tw_FdbOffset+0x0b]&0x08)!=0x08)//2005/05/17 modify
						{
EXTNAMECOMPARE:
							if(p_mp->c_EXTSelect==1)
							{//need compare EXTName						
								for(tc_ExtNum=0;tc_ExtNum<EXT_Name[p_mp->c_type][0];tc_ExtNum++)
								{//compare the ext_name
									for(tw_ByteOffset=0;tw_ByteOffset<3;tw_ByteOffset++)
									{//the max length of ext_name is 3 now 	
										if(gc_UserDataBuf[tw_FdbOffset+0x08+tw_ByteOffset]!=EXT_Name[p_mp->c_type][tw_ByteOffset+tc_PlayMode*3+1])
										{
											tc_PlayMode++;
											break;
										}
									}
									if(tw_ByteOffset>=3)
									{//Extension name is equal
										tb_flag=1;
										tw_FileLongName-=3;
									  	if(p_mp->c_type==1)
									  	{
											tc_PlayMode+=EXT_Name[0][0];
									  	}
										break;
									}
								}
							}
							else if(p_mp->c_EXTSelect==0) 
							{//no need compare EXTName
								tb_flag=1;	                     //the flag determines if implement the flowing programe
								tw_FileLongName-=3;
							}
														
							if((tb_flag==1)&&(p_mp->c_type==1))							
							{//voice file
								for(tw_ByteOffset=0;tw_ByteOffset<4;tw_ByteOffset++)
							  	{//the max length of ext_name is 3 now 									
									if(gc_UserDataBuf[tw_FdbOffset+tw_ByteOffset]!=REC[tw_ByteOffset])
									{
										tb_flag=0;
										break;
									}
								}
							}				
						}
					}//not and not find dir

					if(tb_flag)
					{
						tb_flag=0;	
						if(p_mp->c_type==1)
						{
							for(tw_ByteOffset=0;tw_ByteOffset<0x04;tw_ByteOffset++)
							{
								if((gc_UserDataBuf[tw_FdbOffset+0x04+tw_ByteOffset]<0x30)||(gc_UserDataBuf [tw_FdbOffset+0x04+tw_ByteOffset]>0x39))
								{
									tc_RecFlag=0;
								}		
								tdw_Temp_flag=tdw_Temp_flag<<8;			
								tdw_Temp_flag|=gc_UserDataBuf [tw_FdbOffset+0x04+tw_ByteOffset];  //sorts the name of current fdb
							}
						}

						if(p_mp->c_Search_Mode==K_COUNTER_FILENUM)
						{																		
							if(tc_RecFlag)
							{					
								p_mp->w_FileTotalNum++;
							}
						}
						else if((p_mp->c_Search_Mode==K_TIME_FINDFDB)||p_mp->c_Search_Mode==K_NAME_FINDFDB)		  //find by create time
						{				
							((U8 *)(&tdw_StartCluster1))[0]=gc_UserDataBuf[tw_FdbOffset+0x15];
							((U8 *)(&tdw_StartCluster1))[1]=gc_UserDataBuf[tw_FdbOffset+0x14];
							((U8 *)(&tdw_StartCluster1))[2]=gc_UserDataBuf[tw_FdbOffset+0x1B];
							((U8 *)(&tdw_StartCluster1))[3]=gc_UserDataBuf[tw_FdbOffset+0x1A];
						
							if(tc_RecFlag)
							{			
								if(p_mp->c_Search_Direction)  
								{//find pre						
									if((tdw_Tag0<=tdw_Temp_flag)&&(tdw_Temp_flag<tdw_Tag1))
									{					
										tdw_Tag0= tdw_Temp_flag;
										p_mp->dw_FDB_Cluster=tdw_S_ClusterNum;
										p_mp->dw_FDB_LogAdd=tc_SectorOffset+tdw_SectorAdd;
										p_mp->w_FDB_Offset=tw_FdbOffset;
										tdw_StartCluster3=tdw_StartCluster1;
										p_mp->dw_LongFDB_LogAdd1=tdw_FDB_LogAdd1;
										p_mp->dw_LongFDB_LogAdd0=tdw_FDB_LogAdd0;
										tb_SortFlag=1;	
										gs_File_FCB[p_mp->c_type].c_FileType=tc_PlayMode;								
									}							
									else if(tdw_Temp_flag==tdw_Tag1)
									{								
										if(tdw_StartCluster1!=p_mp->dw_File_StartCluster)
										{//startcluster is not equal
											if(!tb_SameFlag)
											{//current fdb is  before pre fdb which we stored										
												tdw_Tag0= tdw_Temp_flag;
												p_mp->dw_FDB_Cluster=tdw_S_ClusterNum;
												p_mp->dw_FDB_LogAdd=tc_SectorOffset+tdw_SectorAdd;
												p_mp->w_FDB_Offset=tw_FdbOffset;
												tdw_StartCluster3=tdw_StartCluster1;
												p_mp->dw_LongFDB_LogAdd1=tdw_FDB_LogAdd1;
										        p_mp->dw_LongFDB_LogAdd0=tdw_FDB_LogAdd0;
												tb_SortFlag=1;
												gs_File_FCB[p_mp->c_type].c_FileType=tc_PlayMode;																					
											}									
										}
										else
										{
											tb_SameFlag=1;	
										}								
									}														
								}
								else
								{//find next
									if((tdw_Tag1<tdw_Temp_flag)&&(tdw_Temp_flag<tdw_Tag0))
									{																
										tdw_Tag0= tdw_Temp_flag;
										p_mp->dw_FDB_Cluster=tdw_S_ClusterNum;
										p_mp->dw_FDB_LogAdd=tc_SectorOffset+tdw_SectorAdd;
										p_mp->w_FDB_Offset=tw_FdbOffset;
										tdw_StartCluster3=tdw_StartCluster1;
										p_mp->dw_LongFDB_LogAdd1=tdw_FDB_LogAdd1;
										p_mp->dw_LongFDB_LogAdd0=tdw_FDB_LogAdd0;									
										tb_SortFlag=1;	
										gs_File_FCB[p_mp->c_type].c_FileType=tc_PlayMode;						
									}
									if(tdw_Temp_flag!=tdw_Tag0)
									{
										if(tdw_Temp_flag==tdw_Tag1)	
										{							
											if(tdw_StartCluster1!=p_mp->dw_File_StartCluster)
											{//startcluster is not equal
												if(tb_SameFlag)
												{//current fdb is after pre fdb which we stored											
													tdw_Tag0=tdw_Temp_flag;
													p_mp->dw_FDB_Cluster=tdw_S_ClusterNum;
													p_mp->dw_FDB_LogAdd=tc_SectorOffset+tdw_SectorAdd;
													p_mp->w_FDB_Offset=tw_FdbOffset;
													tdw_StartCluster3=tdw_StartCluster1;
													p_mp->dw_LongFDB_LogAdd1=tdw_FDB_LogAdd1;
										           	p_mp->dw_LongFDB_LogAdd0=tdw_FDB_LogAdd0;												
													tb_SortFlag=1;
													gs_File_FCB[p_mp->c_type].c_FileType = tc_PlayMode;												
												}
											}
											else
											{
												tb_SameFlag=1;	
											}		
										}
									}								
								}
							}
						}							
						else if(p_mp->c_Search_Mode==K_ORDER_FINDFDB || p_mp->c_Search_Mode==K_FIND_DIR_EXTNAME) //find by	order
						{ 							
							if(p_mp->c_Search_Direction) 
							{//previous
								if(tw_DirIndex<gw_DirIndex[p_mp->c_Search_Attribute])
								{
								 	tw_DirIndex1=tw_DirIndex;
									p_mp->dw_FDB_Cluster=tdw_S_ClusterNum;
									p_mp->dw_FDB_LogAdd=tc_SectorOffset+tdw_SectorAdd;
									p_mp->w_FDB_Offset=tw_FdbOffset; 									
									p_mp->dw_LongFDB_LogAdd1=tdw_FDB_LogAdd1;
									p_mp->dw_LongFDB_LogAdd0=tdw_FDB_LogAdd0;
									gs_File_FCB[p_mp->c_type].c_FileType = tc_PlayMode;	
									tb_SortFlag=1;
								}
								else//(JC)
								{
									tc_sts=FAIL;					
								}
							}
							else
							{ //next 												 
								if(tw_DirIndex>(gw_DirIndex[p_mp->c_Search_Attribute]))
								{
									p_mp->dw_LongFDB_LogAdd1=tdw_FDB_LogAdd1;
								 	p_mp->dw_LongFDB_LogAdd0=tdw_FDB_LogAdd0;
									gw_DirIndex[p_mp->c_Search_Attribute] = tw_DirIndex;
									tc_sts=SUCCESS;	 																									
									gb_FindFile=1;//Ching 080805																								
								}			
							}
						}
						else if(p_mp->c_Search_Mode==K_SPECIFIC_STARTCLUSTER)	//find by StartCluster
						{						
							((U8 *)(&tdw_StartCluster))[0]=gc_UserDataBuf[tw_FdbOffset+0x15];
							((U8 *)(&tdw_StartCluster))[1]=gc_UserDataBuf[tw_FdbOffset+0x14];
							((U8 *)(&tdw_StartCluster))[2]=gc_UserDataBuf[tw_FdbOffset+0x1b];
							((U8 *)(&tdw_StartCluster))[3]=gc_UserDataBuf[tw_FdbOffset+0x1a];
							if(p_mp->c_Search_Direction)
							{
								p_mp->dw_FDB_Cluster=tdw_S_ClusterNum;
								p_mp->dw_FDB_LogAdd=tc_SectorOffset+tdw_SectorAdd;
								p_mp->w_FDB_Offset=tw_FdbOffset;
								
								if(tdw_StartCluster==p_mp->Compare.dw_StartCluster)
								{
									if(tb_not_1stFDB)
									{
										p_mp->dw_FDB_Cluster=tdw_PrevFDB_Cluster;
										p_mp->dw_FDB_LogAdd=tdw_PrevFDB_LogAdd;
										p_mp->w_FDB_Offset=tw_PrevFDB_Offset;
										tb_not_1stFDB=0;		
									}
									return 0;
								}
								else
								{
									tb_not_1stFDB=1;
									if(p_mp->c_Search_Direction && p_mp->c_Search_Attribute) 
									{
										tdw_PrevFDB_Cluster=tdw_S_ClusterNum;
										tdw_PrevFDB_LogAdd=tc_SectorOffset+tdw_SectorAdd;
										tw_PrevFDB_Offset=tw_FdbOffset; 									
									}	
								}
							}
							else
							{
								if(tdw_StartCluster==p_mp->Compare.dw_StartCluster)
								{
									tc_sts=0;
								}
							}	 						
						}
						else if(p_mp->c_Search_Mode==K_SPECIFIC_SHORT_FILENAME)
						{//find by short file name
							tw_FileLongName=5;	// Only for Search Folder-VOICE
							for(tw_ByteOffset=0;tw_ByteOffset<tw_FileLongName;tw_ByteOffset++)
							{//Compare shortfdb
								if(gc_UserDataBuf[tw_FdbOffset+tw_ByteOffset]!=p_mp->pc_LongFileName[5+tw_ByteOffset])
								{
									break;
								}
							}//compare shortfdb
							if(tw_ByteOffset==tw_FileLongName)
							{//short file name is equal						
								//debug: ex. search 01.txt, but find 012.txt (012's FDB in front of 01's FDB), Ching 100224
								tb_DiffFlag=0;
								if(tw_FileLongName<8)
								{
									if(gc_UserDataBuf[tw_FdbOffset+tw_FileLongName]!=0x20)
									{
										tb_DiffFlag=1;
									}
								}

								if(tb_DiffFlag==0)
								{
									p_mp->dw_LongFDB_LogAdd1=tdw_FDB_LogAdd1;//add for DRM
									p_mp->dw_LongFDB_LogAdd0=tdw_FDB_LogAdd0;//add for DRM
									if(p_mp->c_EXTSelect==1)//hao.yang 090314
									{
										gs_File_FCB[p_mp->c_type].c_FileType = tc_PlayMode;
									}
									tc_sts=SUCCESS;
								}
							}						
						}					
					}
			 	}//short_fdb		
				if(!tc_sts) //Get what we want to get
				{
					if((gb_TriggerFileSkip==0) || (tw_FileSkipNumber==0))	//Ching 080805
					{
NextFIND:   //Ching 080805
						if(p_mp->c_Search_Attribute)
						{
							gw_DirIndex[p_mp->c_Search_Attribute] = tw_DirIndex;
						}
	
						if (p_mp->c_Search_Mode < K_SPECIFIC_LONG_FILENAME)
						{
							gs_File_FCB[p_mp->c_type].c_FileType = tc_PlayMode;							
						}
						else if (p_mp->c_Search_Mode == K_SPECIFIC_LONG_FILENAME)
						{
							tw_FdbOffset+=32;
							if (tw_FdbOffset == 0x200)
							{
								tw_FdbOffset = 0;
								p_mp->dw_LongFDB_LogAdd1 = tdw_FDB_LogAdd0;//add for DRM	
								p_mp->dw_LongFDB_LogAdd0 = tc_SectorOffset+tdw_SectorAdd;//add for DRM	
								tc_SectorOffset ++;
								if (tc_SectorOffset == gc_DOS_SectorPerCluster)
								{
									tdw_S_ClusterNum = DOS_GetNextCluster(tdw_S_ClusterNum,1);
									tdw_SectorAdd=DOS_ClusterLogicAddr(tdw_S_ClusterNum);
									tc_SectorOffset = 0;
								}
								DOS_Read_LogicSector(tdw_SectorAdd+tc_SectorOffset);//tangwei add 090314 for RTC
							}
							else
							{
								p_mp->dw_LongFDB_LogAdd1=tdw_FDB_LogAdd1;//add for DRM
								p_mp->dw_LongFDB_LogAdd0=tdw_FDB_LogAdd0;//add for DRM
							}
							//================================tangwei add for RTC 090314 start=================
							if(p_mp->c_EXTSelect==1)
							{//need compare EXTName						
								for(tc_ExtNum=0;tc_ExtNum<EXT_Name[p_mp->c_type][0];tc_ExtNum++)
								{//compare the ext_name
									for(tw_ByteOffset=0;tw_ByteOffset<3;tw_ByteOffset++)
									{//the max length of ext_name is 3 now 									
										if(gc_UserDataBuf[tw_FdbOffset+0x08+tw_ByteOffset]!=EXT_Name[p_mp->c_type][tw_ByteOffset+tc_PlayMode*3+1])
										{
											tc_PlayMode++;
											break;
										}
									}
									if(tw_ByteOffset>=3)
									{//Extension name is equal
									  	if(p_mp->c_type==1)
									  	{
											tc_PlayMode+=EXT_Name[0][0];
									  	}
										gs_File_FCB[p_mp->c_type].c_FileType = tc_PlayMode;
										break;
									}
								}
							}
							//==================================tangwei add for RTC 090314 end===================
						}
	
						p_mp->dw_FDB_Cluster=tdw_S_ClusterNum;
						p_mp->dw_FDB_LogAdd=tdw_SectorAdd+tc_SectorOffset;
						p_mp->w_FDB_Offset=tw_FdbOffset;
						p_mp->dw_FDB_StartCluster=tdw_FDBClusterNum;
						return(SUCCESS);
					}
					else	//Ching 080805
					{
						if(gb_FindFile==1)
						{
							gb_FindFile=0;
							gw_FileSkipNumber--;
							if(gw_FileSkipNumber==0)
							{
								goto NextFIND;
							}
						}
					}// end of else Ching 080805
				}
				else
				{//not get what we want directly
					if(tc_sts==FAIL)
					{//the	fdb is end or the mode oversteps  
						if(tb_SortFlag)
						{//sort success 
							tb_SortFlag=0;	
							tb_SameFlag=0;
							p_mp->dw_File_StartCluster=tdw_StartCluster3;//save startcluster of current fdb
							p_mp->Compare.dw_BubbleFlag=tdw_Tag0;
							p_mp->dw_FDB_StartCluster=tdw_FDBClusterNum;
							gw_DirIndex[p_mp->c_Search_Attribute] = tw_DirIndex1;
							return(SUCCESS);
						}
						else
						{
							p_mp->dw_FDB_Cluster = tdw_S_ClusterNum;
							p_mp->dw_FDB_LogAdd = tdw_SectorAdd+tc_SectorOffset;
							p_mp->w_FDB_Offset = tw_FdbOffset;
							p_mp->dw_FDB_StartCluster = tdw_FDBClusterNum;
							return(FAIL); 			
						}
					}					
				}
			}
			tdw_FDB_LogAdd1=tdw_FDB_LogAdd0;
			tdw_FDB_LogAdd0=tdw_SectorAdd+tc_SectorOffset;						
		}//for(tc_SectorOffset=tc_Temp_SectorOffset;tc_SectorOffset<tdw_Find_ClusterSize;tc_SectorOffset++)--->find in one cluster
		tdw_S_ClusterNum=DOS_GetNextCluster(tdw_S_ClusterNum,1); 		
	}//while(tdw_S_ClusterNum<0xffff)------>find in one dir
	if(tb_SortFlag)
	{//sort success 											
		tb_SortFlag=0;	
		tb_SameFlag=0;
		p_mp->dw_File_StartCluster=tdw_StartCluster3;          //save startcluster of current fdb
		p_mp->Compare.dw_BubbleFlag=tdw_Tag0;						
		p_mp->dw_FDB_StartCluster=tdw_FDBClusterNum;						
		gw_DirIndex[p_mp->c_Search_Attribute] = tw_DirIndex1;					
		return 0;
	}
	else
	{		
		p_mp->dw_FDB_Cluster = tdw_S_ClusterNum;
		p_mp->dw_FDB_LogAdd = tdw_SectorAdd+tc_SectorOffset;
		p_mp->w_FDB_Offset = tw_FdbOffset;
		p_mp->dw_FDB_StartCluster=tdw_FDBClusterNum;	
		return 1;			
	}
}

void DOS_FillFdb(U8 *tpc_Fdb,U8 *tpc_Name,U32 tdw_StartCluster,U32 tdw_FileSize)
{
	// update new name
	memcpy(tpc_Fdb,tpc_Name,0x0C);					  				
	gw_DOS_CreateDate=(U16)(gc_Year+30)*0x0200+(U16)gc_Month*0x0020+gc_Day;
	gw_DOS_CreateTime=(U16)gc_Hour*0x0800+(U16)gc_Min*0x0020+gc_Sec;

	tpc_Fdb[0x0C]=0;	// reserved 
	tpc_Fdb[0x0D]=0;	// creation time in millisecond 
		
	//creation time
	tpc_Fdb[0x0E]=gw_DOS_CreateTime;
	tpc_Fdb[0x0F]=gw_DOS_CreateTime>>8;
	
	//creation date
	tpc_Fdb[0x10]=gw_DOS_CreateDate;		
	tpc_Fdb[0x11]=gw_DOS_CreateDate>>8;	
	
	//last access date
	tpc_Fdb[0x12]=gw_DOS_CreateDate;
	tpc_Fdb[0x13]=gw_DOS_CreateDate>>8;	
	
	//high word of start cluster number
	tpc_Fdb[0x14]=tdw_StartCluster>>16;		
	tpc_Fdb[0x15]=tdw_StartCluster>>24;

	//last modify time
	tpc_Fdb[0x16]=gw_DOS_CreateTime;
	tpc_Fdb[0x17]=gw_DOS_CreateTime>>8;
	
	//last modify date
	tpc_Fdb[0x18]=gw_DOS_CreateDate;		
	tpc_Fdb[0x19]=gw_DOS_CreateDate>>8;

	tpc_Fdb[0x1A]=tdw_StartCluster;
	tpc_Fdb[0x1B]=tdw_StartCluster>>8;

	tpc_Fdb[0x1C]=tdw_FileSize;
	tpc_Fdb[0x1D]=tdw_FileSize>>8;
	tpc_Fdb[0x1E]=tdw_FileSize>>16;
	tpc_Fdb[0x1F]=tdw_FileSize>>24;
}


