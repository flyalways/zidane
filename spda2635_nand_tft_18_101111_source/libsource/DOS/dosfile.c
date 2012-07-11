#include <string.h>
#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "dos\dosfdb.h"
#include "dos\dosfile.h"
#include "Memalloc.h"

extern U8	gc_clock_mode;
//-------------------------------------------------------------------------------
U32 DOS_Seek_File(U8 tc_FileHandle, U32 tdw_SectorNumber)
{
	U32 tdw_ClusterNum;
	U32 tdw_NextClusterNum;

	if (gs_File_FCB[tc_FileHandle].dw_File_StartCluster==0xffffffff)
	{
		return 1;	
	}
	tdw_ClusterNum = tdw_SectorNumber/gc_DOS_SectorPerCluster;

	gs_File_FCB[tc_FileHandle].dw_File_DataPoint = tdw_SectorNumber<<9;
	if ((tdw_SectorNumber%gc_DOS_SectorPerCluster)==0)
	{
		if(tdw_ClusterNum)
			tdw_ClusterNum = tdw_ClusterNum-1;
	}
	tdw_NextClusterNum = DOS_GetNextCluster(gs_File_FCB[tc_FileHandle].dw_File_CurrentCluster,tdw_ClusterNum);
	return (tdw_NextClusterNum);
}


U8	DOS_Search_File(U8 tc_Mode, U8 tc_Type, U8 tc_PrevOrNext)
{
	U16	tw_FileSkipNumber;
	U8  tc_sts;
	U8 	tb_IniFlag;      
	U8  tc_FindDir,tc_FindFile;       						//whether search file or search dir flag
	U8  tc_SearchTime;
	U8  tc_clock_mode_backup;
	U32 tdw_SectorAdd,tdw_CurrentDirCluster,tdw_PrevDirCluster,tdw_NextDirCluster;
	SearchFdb ptr;
	
	tc_clock_mode_backup=gc_clock_mode;
	set_clock_mode(CLOCK_MODE_MJPEG);
	
	tw_FileSkipNumber=gw_FileSkipNumber;
	gs_File_FCB[tc_Type].dw_File_StartCluster = 0xffffffff;
	gs_File_FCB[tc_Type].dw_FDB_LogAdd = 0;

	tc_sts=1;	
	tb_IniFlag=0;
	ptr.w_FileTotalNum=0;
	gb_ReadWriteDataArea=1;
	if((tc_Mode&0x0f)==1)
	{//if search file by time, please search by order
		tc_Mode|=0x03;	
	}	
	ptr.pc_LongFileName = gc_FileLongName; 
	ptr.c_Search_Mode=(tc_Mode&0x0f);
	ptr.c_EXTSelect = ((tc_PrevOrNext & 0xf0) >> 4);		//need to compare ExtName
	tc_PrevOrNext = tc_PrevOrNext & 0x0f;
	if((tc_Mode&0x0f)==0||(tc_Mode&0x0f)==4||(tc_Mode&0x0f)==5)
	{//count filenum, find by long short filename -> find next
		tc_PrevOrNext=0;
	}

	ptr.c_Search_Direction=tc_PrevOrNext;
	ptr.Compare.dw_BubbleFlag = gdw_CurrFlag;
	ptr.dw_File_StartCluster = gdw_StartCluster2;
	ptr.c_type=tc_Type;	
	if(gb_CountDirFlag) tc_SearchTime=1; //Ching 100514
	else				tc_SearchTime=2; 
	while(tc_SearchTime)
	{
		if((tc_Mode&0x0f0)==0x20)
		{//find dir (JC)find dir overall
	 		tc_FindFile=0;
			tc_FindDir=1;
			ptr.dw_FDB_StartCluster=gs_DIR_FCB[tc_Type].dw_FDB_StartCluster;//(JC)start cluster of the dir that current file fdb is in
		}
		else
		{//find file (JC)find file in one dir or overall, find dir in one dir not included?
			tc_FindFile=1;
			tc_FindDir=0;  	
			ptr.dw_FDB_StartCluster=gs_File_FCB[tc_Type].dw_FDB_StartCluster;//(JC)start cluster of the dir that current file fdb is in 	
		}
		if(gb_FindFlag==0)//(JC)Normally for 1st time search fail
		{//have not searched 		       
			ptr.dw_File_StartCluster = gdw_DOS_RootDirClus;
			gs_DIR_FCB[tc_Type].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
			gs_DIR_FCB[tc_Type].dw_File_StartCluster = gdw_DOS_RootDirClus;
			if(ptr.c_Search_Direction)//(JC)search previous
			{
				if(((tc_Mode&0x0f)==K_TIME_FINDFDB)||((tc_Mode&0x0f)==K_NAME_FINDFDB))//(JC)Find by time or find by name
				{
					ptr.Compare.dw_BubbleFlag=0xffffffff;
				}
				tc_FindDir=1;
				
				gw_DirIndex[0]=0xffff;     	//gw_DirIndex1 used to save the index of the fdb which find just now               lizhn 040927 add
				gw_DirIndex[1]=0xffff;     //gw_DirIndex0 used to save the index of the last fdb 					
			}
			else
			{
				if(((tc_Mode&0x0f)==K_TIME_FINDFDB)||((tc_Mode&0x0f)==K_NAME_FINDFDB))
				{
					ptr.Compare.dw_BubbleFlag=0x00;
				}
				gw_DirIndex[0] = 0;
				gw_DirIndex[1] = 0;
			}	
		}
		if((tc_Mode&0x0f0)==0x10)
		{//find file in one dir
			ptr.c_Search_Attribute=0;						//(JC)look 4 file
			gb_TriggerFileSkip=1;							//Ching 081016		
			tc_sts=Find_Fdb(&ptr);	
			gb_TriggerFileSkip=0;							//Ching 081016
		}
		else
		{//find in all dir ,(JC)find file or dir in all dir w/ the same path		
			if(tc_PrevOrNext)
			{//find pre
				//dbprintf("search previous!\n");
				while(1)
				{  									
					if(tc_FindDir)
					{//not found dir in the dir in the last time but found file
						ptr.c_Search_Mode=K_ORDER_FINDFDB;
						ptr.c_Search_Attribute=1;			//(JC)look for dir
						tc_sts=Find_Fdb(&ptr);
					}				
					if(tc_sts)
					{//haven't found dir
						if(tb_IniFlag)
						{
							tb_IniFlag=0;					
						 	ptr.Compare.dw_BubbleFlag=0xffffffff;
						}
						if((tc_Mode&0x0f)==2)
						{//if find the file by name,please find dir by time
							ptr.c_Search_Mode=2;
						}
						if(tc_FindFile)
						{
							ptr.c_Search_Attribute=0;
							tc_sts=Find_Fdb(&ptr);					
						}
						if(tc_sts)
						{//not find file
							tc_FindDir=1;
							if(ptr.dw_FDB_StartCluster!=gdw_DOS_RootDirClus)
							{//current dir is not rootdir							
								tdw_SectorAdd = DOS_ClusterLogicAddr(ptr.dw_FDB_StartCluster);
								gb_ReadWriteDataArea=1;
								//dbprintf("reading out the start cluster of '..'\n");
								if(DOS_Read_LogicSector(tdw_SectorAdd,1))//(JC)for reading out the start cluster of ".."
								{
									set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
									return 0xff;
								}
								((U8 *)(&tdw_PrevDirCluster))[3]=gc_UserDataBuf[32+0x1a];
								((U8 *)(&tdw_PrevDirCluster))[2]=gc_UserDataBuf[32+0x1b];
								((U8 *)(&tdw_PrevDirCluster))[1]=gc_UserDataBuf[32+0x14];
								((U8 *)(&tdw_PrevDirCluster))[0]=gc_UserDataBuf[32+0x15]; //computer the startcluster of predir//(JC)".."upper dir 

	  							if(!(tdw_PrevDirCluster))
								{//predir is rootdir
									tdw_PrevDirCluster=gdw_DOS_RootDirClus;
								}
								ptr.dw_File_StartCluster=ptr.dw_FDB_StartCluster;//(JC)start cluster of this dir layer
								ptr.dw_FDB_StartCluster=tdw_PrevDirCluster;
								//return to predir
								tb_IniFlag=1;
								if((tc_Mode&0x0f)==0x03)
								{//find dir by order
									//modified for Fake Flash Size, Ching 090420 === S
									U32 tdw_Cluster, tdw_LogicAdd;
									bit tbt_IsFakeFDB=0, tbt_BreakFlag=0;

									do
									{
										tbt_IsFakeFDB=0;
										gw_DirIndex[0]=0xffff;
										ptr.c_Search_Mode = K_SPECIFIC_STARTCLUSTER;//(JC)search start cluster=ptr.dw_File_StartCluster from ptr.dw_FDB_StartCluster找目前所在目錄FDB是否在上層目錄
										ptr.Compare.dw_StartCluster =ptr.dw_File_StartCluster;
										ptr.c_Search_Attribute = 1;
										if(Find_Fdb(&ptr))
										{
											tbt_BreakFlag=1; //Ching 090420
											break;
										}

										if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd,1))	return 0xff;

										((U8 *)(&tdw_Cluster))[3]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1a];
										((U8 *)(&tdw_Cluster))[2]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1b];
										((U8 *)(&tdw_Cluster))[1]=gc_UserDataBuf[ptr.w_FDB_Offset+0x14];
										((U8 *)(&tdw_Cluster))[0]=gc_UserDataBuf[ptr.w_FDB_Offset+0x15];
										tdw_LogicAdd=DOS_ClusterLogicAddr(tdw_Cluster);
										if(DOS_Read_LogicSector(tdw_LogicAdd,1))	
										{
											return 0xff;
										}

										if((gc_UserDataBuf[0x00]!=0x2E) || (gc_UserDataBuf[0x0B]!=0x10) || (gc_UserDataBuf[0x20]!=0x2E) || (gc_UserDataBuf[0x21]!=0x2E) || (gc_UserDataBuf[0x2B]!=0x10))
										{
											tbt_IsFakeFDB=1;
											ptr.dw_File_StartCluster=tdw_Cluster;
										}

										if(!tbt_IsFakeFDB)
										{
											if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd,1)) //(JC)FDB sector addr, w_FDB_Offset(x32B) is the FDb found
											{
												set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
												return 0xff;
											}
	
											if((((U8 *)(&ptr.Compare.dw_StartCluster))[3]!= gc_UserDataBuf[ptr.w_FDB_Offset+0x1a])||(((U8 *)(&ptr.Compare.dw_StartCluster))[2]!= gc_UserDataBuf[ptr.w_FDB_Offset+0x1b])||
												(((U8 *)(&ptr.Compare.dw_StartCluster))[1]!= gc_UserDataBuf[ptr.w_FDB_Offset+0x14])||(((U8 *)(&ptr.Compare.dw_StartCluster))[0]!= gc_UserDataBuf[ptr.w_FDB_Offset+0x15]))
											{//(JC)previous dir exist
												((U8 *)(&ptr.dw_FDB_StartCluster))[3]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1a];//(JC)start cluster no. in FDB found
												((U8 *)(&ptr.dw_FDB_StartCluster))[2]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1b];
												((U8 *)(&ptr.dw_FDB_StartCluster))[1]=gc_UserDataBuf[ptr.w_FDB_Offset+0x14];
												((U8 *)(&ptr.dw_FDB_StartCluster))[0]=gc_UserDataBuf[ptr.w_FDB_Offset+0x15];
												ptr.Compare.dw_BubbleFlag=0xffffffff;	
												gw_DirIndex[1]=0xffff;
												if((tc_Mode&0xf0)==0x20)
												{//find dir by order
													gw_DirIndex[ptr.c_Search_Mode] = 0xffff; 											
													tc_FindFile=0;
												}
												tc_FindDir=1;
												if(((tc_Mode&0xf0)==0x20))
												{									
													if(gs_DIR_FCB[tc_Type].dw_File_StartCluster!=ptr.dw_File_StartCluster)
													{
														tc_sts=0;
														tbt_BreakFlag=1; //Ching 090420
														break;
													}
													else
													{
														gw_DirIndex[1]=0xffff;
													}									
												}
											}
											else
											{
												tc_FindDir=0;
												tc_FindFile=1;
												tc_sts=2;
												gw_DirIndex[0]=0xffff;
												ptr.c_Search_Mode=K_ORDER_FINDFDB;
												ptr.dw_FDB_StartCluster=tdw_PrevDirCluster;	
												ptr.dw_FDB_Cluster=ptr.dw_FDB_StartCluster;		
											}
										}
									}while(tbt_IsFakeFDB);

									if(tbt_BreakFlag)	break; //Ching 090420
									//modified for Fake Flash Size, Ching 090420 === E
								}
							}
							else
							{//current dir is rootdir //(JC)no upper dir			
								break;						
							}
						}
						else
						{						
							break;					
						}
					}			
					else
					{//have found dir in dir					
		 				gb_ReadWriteDataArea=1;		
						if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd,1))//(JC)FDB sector addr, w_FDB_Offset(x32B) is the FDb found
						{
							set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
							return 0xff;
						}
						tdw_CurrentDirCluster=ptr.dw_FDB_StartCluster;								//save current dir(X)
						((U8 *)(&ptr.dw_FDB_StartCluster))[3]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1a];//(JC)start cluster no. in FDB found
						((U8 *)(&ptr.dw_FDB_StartCluster))[2]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1b];
						((U8 *)(&ptr.dw_FDB_StartCluster))[1]=gc_UserDataBuf[ptr.w_FDB_Offset+0x14];
						((U8 *)(&ptr.dw_FDB_StartCluster))[0]=gc_UserDataBuf[ptr.w_FDB_Offset+0x15];//next dir cluster 
										
						tdw_SectorAdd = DOS_ClusterLogicAddr(ptr.dw_FDB_StartCluster);
						if(DOS_Read_LogicSector((tdw_SectorAdd),1))
						{
							// For dummy size
							if(gw_FileIndex[0]==1)
							{
								gw_FileIndex[0]=0;
							}
							set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
							return 0xff;						
						}
						
						((U8 *)(&tdw_PrevDirCluster))[3]=gc_UserDataBuf[32+0x1a];
						((U8 *)(&tdw_PrevDirCluster))[2]=gc_UserDataBuf[32+0x1b];
						((U8 *)(&tdw_PrevDirCluster))[1]=gc_UserDataBuf[32+0x14];
						((U8 *)(&tdw_PrevDirCluster))[0]=gc_UserDataBuf[32+0x15];
	
						((U8 *)(&tdw_NextDirCluster))[3]=gc_UserDataBuf[0x1a];
						((U8 *)(&tdw_NextDirCluster))[2]=gc_UserDataBuf[0x1b];
						((U8 *)(&tdw_NextDirCluster))[1]=gc_UserDataBuf[0x14];
						((U8 *)(&tdw_NextDirCluster))[0]=gc_UserDataBuf[0x15];		
													
						if((tdw_PrevDirCluster!=tdw_CurrentDirCluster)&&(tdw_CurrentDirCluster!=gdw_DOS_RootDirClus)||(tdw_NextDirCluster!=ptr.dw_FDB_StartCluster))
						{//dos error					
							 ptr.dw_FDB_StartCluster=tdw_CurrentDirCluster; 		
						}				
						else
						{
							ptr.Compare.dw_BubbleFlag=0xffffffff;	
							gw_DirIndex[1]=0xffff;
							if((tc_Mode&0xf0)==0x20)
							{//find dir by order
								gw_DirIndex[ptr.c_Search_Mode] = 0xffff; 											
								tc_FindFile=0;
							} 
						}
						tc_FindDir=1;		
					
					}//have found dir in dir			
				}
			}
			else
			{//find next				
				while(1)
				{
					ptr.c_Search_Mode = (tc_Mode&0x0f);						
					if(tc_FindFile)
					{								
						ptr.c_Search_Attribute=0;			//look for file			

						gb_TriggerFileSkip=1;				//Ching 080805
						tc_sts=Find_Fdb(&ptr);
						gb_TriggerFileSkip=0;				//Ching 080805
						
						if(!(tc_Mode&0x0f))//(JC)count file number							   					
						{		
							gw_FileTotalNumber[tc_Type] = ptr.w_FileTotalNum;		
						}
					}				
					if(tc_sts)
					{//have not found the specific file in one dir
						if(tc_FindFile)
						{												
							ptr.Compare.dw_BubbleFlag=0x0000;
							gw_DirIndex[1] = 0;
						}						
						ptr.c_Search_Mode=0x03;				//if count filenum now,please search dir by order
					
						if((tc_Mode&0xf0)==0x20)
						{
							ptr.c_Search_Mode=(tc_Mode&0x0f);
						}
						
						ptr.c_Search_Attribute=1;						
						tc_sts=Find_Fdb(&ptr);				//look for dir in the dir
						if(tc_sts)
						{//not find dir
							if(ptr.dw_FDB_StartCluster!=gdw_DOS_RootDirClus)//not rootdir
							{                               
								tdw_SectorAdd = DOS_ClusterLogicAddr(ptr.dw_FDB_StartCluster);
								gb_ReadWriteDataArea=1;
							 	if(DOS_Read_LogicSector(tdw_SectorAdd,1))
								{
									set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add	
									  return 0xff;
								}

								((U8 *)(&tdw_PrevDirCluster))[3]=gc_UserDataBuf[32+0x1a];
								((U8 *)(&tdw_PrevDirCluster))[2]=gc_UserDataBuf[32+0x1b];
								((U8 *)(&tdw_PrevDirCluster))[1]=gc_UserDataBuf[32+0x14];
								((U8 *)(&tdw_PrevDirCluster))[0]=gc_UserDataBuf[32+0x15];								
								if(!(tdw_PrevDirCluster))
								{//predir is rootdir												                       
									tdw_PrevDirCluster=gdw_DOS_RootDirClus;							
								}
							
								ptr.dw_File_StartCluster=ptr.dw_FDB_StartCluster;
								ptr.dw_FDB_StartCluster=tdw_PrevDirCluster;//return to predir
								tc_FindFile=0; 
								if(ptr.c_Search_Mode==0x03)
								{//find by order
									gw_DirIndex[1] = 0;
									ptr.c_Search_Mode = K_SPECIFIC_STARTCLUSTER;
									ptr.Compare.dw_StartCluster =ptr.dw_File_StartCluster;
									ptr.c_Search_Attribute=1;  
									if(Find_Fdb(&ptr))
									{
										break;
									}													
								}
							}
							else
							{//not find dir in rootdir 	
								gs_DIR_FCB[tc_Type].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;						
								break;							
							}
						}
						else
						{//have found dir						
							gb_ReadWriteDataArea=1;
							if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd,1))
							{
								set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
								return 0xff;						
							}
							tdw_CurrentDirCluster=ptr.dw_FDB_StartCluster;                                   //save current dir
							((U8 *)(&ptr.dw_FDB_StartCluster))[3]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1a];
							((U8 *)(&ptr.dw_FDB_StartCluster))[2]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1b];
							((U8 *)(&ptr.dw_FDB_StartCluster))[1]=gc_UserDataBuf[ptr.w_FDB_Offset+0x14];
							((U8 *)(&ptr.dw_FDB_StartCluster))[0]=gc_UserDataBuf[ptr.w_FDB_Offset+0x15];	//next dir cluster		
							tdw_SectorAdd = DOS_ClusterLogicAddr(ptr.dw_FDB_StartCluster);
							if(DOS_Read_LogicSector((tdw_SectorAdd),1))
							{
								// For dummy size
								if(gw_FileIndex[0]==gw_FileTotalNumber[0])
								{
									gw_FileIndex[0]=0;
									DOS_Read_LogicSector((gdw_DOS_RootDirClus),1);
								}
								else
								{
									set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
									return 0xff;							
								}
							}
							((U8 *)(&tdw_PrevDirCluster))[3]=gc_UserDataBuf[32+0x1a];
							((U8 *)(&tdw_PrevDirCluster))[2]=gc_UserDataBuf[32+0x1b];
							((U8 *)(&tdw_PrevDirCluster))[1]=gc_UserDataBuf[32+0x14];
							((U8 *)(&tdw_PrevDirCluster))[0]=gc_UserDataBuf[32+0x15];	                   //current dir
	
							((U8 *)(&tdw_NextDirCluster))[3]=gc_UserDataBuf[0x1a];
							((U8 *)(&tdw_NextDirCluster))[2]=gc_UserDataBuf[0x1b];
							((U8 *)(&tdw_NextDirCluster))[1]=gc_UserDataBuf[0x14];
							((U8 *)(&tdw_NextDirCluster))[0]=gc_UserDataBuf[0x15];				            //next dir	
							
							if((tdw_PrevDirCluster!=tdw_CurrentDirCluster)&&(tdw_CurrentDirCluster!=gdw_DOS_RootDirClus)||(tdw_NextDirCluster!=ptr.dw_FDB_StartCluster))
							{//dos error 					
								ptr.dw_FDB_StartCluster=tdw_CurrentDirCluster;       
								tc_FindFile=0;   						
							}
							else
							{
								gw_DirIndex[0] = 0;
								if((tc_Mode&0x0f0)==0x20)
								{//put the line in here is to make dw_FDB_StartCluster equal dw_File_StartCluster  	   							
								    gw_DirIndex[1] = 0;	 
									ptr.dw_File_StartCluster = ptr.dw_FDB_StartCluster;
									tc_sts=0;
									break;
								}			
								ptr.Compare.dw_BubbleFlag=0x0000;
								tc_FindFile=1; 	
							}
							tc_sts=1; 						
						}//have find dir
					}
					else
					{//have found the file 
						break;
					}
				} 		
			}		
		}
		if(!tc_sts)
		{//find the file/dir 
			gb_FindFlag=1;	
			if((tc_Mode&0x0f0)==0x20)
			{//dir
				gs_DIR_FCB[tc_Type].dw_File_StartCluster=ptr.dw_File_StartCluster;  			    
				gs_DIR_FCB[tc_Type].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
			 	gs_DIR_FCB[tc_Type].dw_LongFDB_LogAdd1=ptr.dw_LongFDB_LogAdd1;
			 	gs_DIR_FCB[tc_Type].dw_LongFDB_LogAdd0=ptr.dw_LongFDB_LogAdd0;
			 	gs_DIR_FCB[tc_Type].dw_FDB_Cluster=ptr.dw_FDB_Cluster;
				gs_DIR_FCB[tc_Type].dw_FDB_LogAdd=ptr.dw_FDB_LogAdd;
			 	gs_DIR_FCB[tc_Type].w_FDB_Offset=ptr.w_FDB_Offset;
	   		
			}
			else
			{				 			
				gs_File_FCB[tc_Type].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
				gs_File_FCB[tc_Type].dw_LongFDB_LogAdd1=ptr.dw_LongFDB_LogAdd1;
				gs_File_FCB[tc_Type].dw_LongFDB_LogAdd0=ptr.dw_LongFDB_LogAdd0;
				gs_File_FCB[tc_Type].dw_FDB_Cluster=ptr.dw_FDB_Cluster;
				gs_File_FCB[tc_Type].dw_FDB_LogAdd=ptr.dw_FDB_LogAdd;
				gs_File_FCB[tc_Type].w_FDB_Offset=ptr.w_FDB_Offset;
				gdw_CurrFlag = ptr.Compare.dw_BubbleFlag;
				gdw_StartCluster2 =	ptr.dw_File_StartCluster;
				if(tc_PrevOrNext)
				{
					if(tw_FileSkipNumber==0)
					{
						tw_FileSkipNumber=1;
					}	
					if(tw_FileSkipNumber>=gw_FileIndex[tc_Type])
					{
						gw_FileIndex[tc_Type]=gw_FileIndex[tc_Type]+gw_FileTotalNumber[tc_Type]-tw_FileSkipNumber;
					}
					else
					{
						gw_FileIndex[tc_Type]-=tw_FileSkipNumber;
					}		
				}
				else
				{
					if(tw_FileSkipNumber==0)
					{
						tw_FileSkipNumber=1;
					}
					gw_FileIndex[tc_Type]+=tw_FileSkipNumber;
					if(gw_FileIndex[tc_Type]>gw_FileTotalNumber[tc_Type])
					{
						gw_FileIndex[tc_Type]-=gw_FileTotalNumber[tc_Type];
					}
				}
			}	
			set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add					 
			return(tc_sts);		
		}
		else if(!(tc_Mode&0x0f))//(JC)Count file total number							   					
		{			
			gw_FileTotalNumber[tc_Type] = ptr.w_FileTotalNum; 			
			set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add 			
			return(1);								
		}
		else 
		{//(JC)search fail, search again if this were the 1st search 
			gs_File_FCB[tc_Type].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
			gs_DIR_FCB[tc_Type].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
			tc_SearchTime--;
			if(((tc_Mode&0x0f)==1)||((tc_Mode&0x0f)==2)||((tc_Mode&0x0f)==3))
			{
				gb_FindFlag=0;		
			}
		} 
	}
	set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
	return(1);	
}


UBYTE DOS_Open_File_r(UBYTE tc_FileHandle, UBYTE tc_OpenMode, UBYTE * tpc_FileName)
{
	WORD tw_Fdb_offset;
	LWORD	tdw_temp;
	SearchFdb *p_mp;
	SearchFdb temp;
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

		DOS_Read_LogicSector(p_mp->dw_FDB_LogAdd,1);
		
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


void DOS_Close_File_r(UBYTE tc_FileHandle)
{
		gs_File_FCB[tc_FileHandle].dw_File_StartCluster = 0xffffffff;
}

