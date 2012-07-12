#include "..\Header\SPDA2K.h"
#include "..\header\dos\fs_struct.h"
#include "..\header\Memalloc.h"
#include "Dir_process.h"
#include "..\header\variables.h"
#include "PROTOTYP.h"

extern  U8   Find_Fdb(SearchFdb *p_mp);
extern	U8	xdata EXT_NameC[];
void	DOS_Count_All_Dir(void);

U8 GetCurrentDirInfo(void)
{
	U32 tdw_Addr, tdw_TagetCluster, tdw_StartCluster;
	SearchFdb ptr;

	DOS_Count_All_Dir();
	if(gs_File_FCB[0].dw_FDB_StartCluster == gdw_DOS_RootDirClus) //root dir
	{
		gs_DIR_FCB[0].dw_FDB_Cluster = gdw_DOS_RootDirClus;
		gs_DIR_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
		gs_DIR_FCB[0].dw_File_StartCluster = gdw_DOS_RootDirClus;
		gs_DIR_FCB[0].dw_FDB_LogAdd = gdw_DOS_RootDirAddr;
		gs_DIR_FCB[0].w_FDB_Offset = 0;
		return 0;
	}

	tdw_Addr = DOS_ClusterLogicAddr(gs_File_FCB[0].dw_FDB_StartCluster); //DIR Current Cluster
	gb_ReadWriteDataArea=1;
	if(DOS_Read_LogicSector(tdw_Addr,1))	return 0xff;
	// .
	((UBYTE *)(&tdw_TagetCluster))[0]=gc_UserDataBuf[0x15];
	((UBYTE *)(&tdw_TagetCluster))[1]=gc_UserDataBuf[0x14];
	((UBYTE *)(&tdw_TagetCluster))[2]=gc_UserDataBuf[0x1b];
	((UBYTE *)(&tdw_TagetCluster))[3]=gc_UserDataBuf[0x1a];
	// ..
	((UBYTE *)(&tdw_StartCluster))[0]=gc_UserDataBuf[0x35];
	((UBYTE *)(&tdw_StartCluster))[1]=gc_UserDataBuf[0x34];
	((UBYTE *)(&tdw_StartCluster))[2]=gc_UserDataBuf[0x3b];
	((UBYTE *)(&tdw_StartCluster))[3]=gc_UserDataBuf[0x3a];
	gw_DirIndex[0]=0;
	ptr.c_Search_Mode = K_SPECIFIC_STARTCLUSTER;
	ptr.Compare.dw_StartCluster = tdw_TagetCluster;
	ptr.dw_FDB_StartCluster = tdw_StartCluster;
	ptr.c_type=0;
	ptr.c_Search_Attribute = 1;  
	ptr.c_Search_Direction = 0; //search next
	if(!Find_Fdb(&ptr))
	{
		gw_DirIndex[1] = 0;
		gs_DIR_FCB[0].dw_FDB_Cluster = ptr.dw_FDB_Cluster;
		gs_DIR_FCB[0].dw_FDB_LogAdd = ptr.dw_FDB_LogAdd;
		gs_DIR_FCB[0].w_FDB_Offset = ptr.w_FDB_Offset;
		gs_DIR_FCB[0].dw_FDB_StartCluster = tdw_TagetCluster;
		gs_DIR_FCB[0].dw_File_StartCluster = tdw_TagetCluster;
		return 0;
	}
	else
		return 1;
}


U8	DOS_Search_DIR_Next_test(U8 tc_Mode)
{
	U8 tc_sts;
	U8 tc_FindDir,tc_FindFile;      //whether search file or search dir flag
	U8 tc_SearchTime;
	U32 tdw_SectorAdd,tdw_CurrentDirCluster,tdw_PrevDirCluster,tdw_NextDirCluster;
	SearchFdb ptr;
	
	tc_sts=1;	
	ptr.w_DirTotalNum=0;
	gb_ReadWriteDataArea=1;
	if((tc_Mode&0x0f)==1)
	{//if search file by time, please search by order
		tc_Mode|=0x03;	
	}	
	ptr.pc_LongFileName = gc_FileLongName;
	ptr.c_Search_Mode=(tc_Mode&0x0f);
	ptr.c_EXTSelect = 0; //the directory doesn't need to compare ExtName, default value

	if((tc_Mode&0x0f)==0x0b)
		gw_DirTotalNumber=0;

	ptr.c_Search_Direction=0; //Next
	ptr.Compare.dw_BubbleFlag = gdw_CurrFlag;
	ptr.dw_File_StartCluster = gdw_StartCluster2;
	ptr.c_type=0;
	tc_SearchTime=2; 
	while(tc_SearchTime)
	{
		if((tc_Mode&0x0f0)==0x20 || (tc_Mode&0x0f0)==0x30)
		{//find dir 
		 	tc_FindFile=0;
			tc_FindDir=1;
			ptr.dw_FDB_StartCluster=gs_DIR_FCB[0].dw_FDB_StartCluster;
		}
		else
		{//find file
			return(1); //this function isn't used to find file
		}
		
		if(!(gb_FindFlag))
		{//have not searched 		       
			ptr.dw_File_StartCluster = gdw_DOS_RootDirClus;
			gs_DIR_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
			gs_DIR_FCB[0].dw_File_StartCluster = gdw_DOS_RootDirClus;
			if(ptr.c_Search_Direction) //find pre
			{
				if(((tc_Mode&0x0f)==K_NAME_FINDFDB)||((tc_Mode&0x0f)==K_TIME_FINDFDB)) //modify by Ching
				{
					ptr.Compare.dw_BubbleFlag=0xffffffff;
				}
				tc_FindDir=1;
				gw_DirIndex[0]=0xffff;     //gw_DirIndex1 used to save the index of the fdb which find just now     lizhn 040927 add
				gw_DirIndex[1]=0xffff;     //gw_DirIndex0 used to save the index of the last fdb  					
			}
			else //find next
			{
				if(((tc_Mode&0x0f)==K_NAME_FINDFDB)||((tc_Mode&0x0f)==K_TIME_FINDFDB)) //modify by Ching
				{
					ptr.Compare.dw_BubbleFlag=0x00;
				}
				gw_DirIndex[0] = 0;    
				gw_DirIndex[1] = 0;
			}	
		}

		if((tc_Mode&0x0f0)==0x30)
		{//find dir in one dir
			ptr.c_Search_Attribute=1; //find dir
			if((tc_Mode&0x0f)==0x0b)
				tc_sts=Count_Dir_Fdb(&ptr);
			else
				tc_sts=Find_Fdb(&ptr);
			
			gb_ReadWriteDataArea=1;
			if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd,1))
				return 0xff;

			((U8 *)(&ptr.dw_File_StartCluster))[3]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1a];
			((U8 *)(&ptr.dw_File_StartCluster))[2]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1b];
			((U8 *)(&ptr.dw_File_StartCluster))[1]=gc_UserDataBuf[ptr.w_FDB_Offset+0x14];
			((U8 *)(&ptr.dw_File_StartCluster))[0]=gc_UserDataBuf[ptr.w_FDB_Offset+0x15];
		}
		else //(tc_Mode&0x0f0)==0x20
		{//find in all dir
			{//find next
				while(1)
				{ 
					ptr.c_Search_Mode = (tc_Mode&0x0f);
					if(tc_FindDir==1 && ptr.c_Search_Mode==0x0b)
					{//count dir number in all directories
						//ptr.c_Search_Attribute=1;//find dir
						tc_sts=Count_Dir_Fdb(&ptr);
						gw_DirTotalNumber += ptr.w_DirTotalNum; //Ching 090204
						tc_sts=1; //Ching 090204
					}
					if(tc_sts) //Here, tc_sts=1
					{//have not found the specific file in one dir or count dir number in one dir
						ptr.c_Search_Mode=0x03;  //if count filenum now,please search dir by order

						if((tc_Mode&0xf0)==0x20 && (tc_Mode&0x0f)!=0x0b) //for tc_Mode=0x28, 
							ptr.c_Search_Mode=(tc_Mode&0x0f);                     //it has counted in this dir. Then it must be searching new dir to count overall directories

						ptr.c_Search_Attribute=1;
						tc_sts=Find_Fdb(&ptr);   //look for dir in the dir
						if(tc_sts)
						{//not find dir
							if(ptr.dw_FDB_StartCluster!=gdw_DOS_RootDirClus)                 //not rootdir
							{
								tdw_SectorAdd = DOS_ClusterLogicAddr(ptr.dw_FDB_StartCluster);
								gb_ReadWriteDataArea=1;
								if(DOS_Read_LogicSector(tdw_SectorAdd,1))
									return 0xff;

								((U8 *)(&tdw_PrevDirCluster))[3]=gc_UserDataBuf[32+0x1a];
								((U8 *)(&tdw_PrevDirCluster))[2]=gc_UserDataBuf[32+0x1b];
								((U8 *)(&tdw_PrevDirCluster))[1]=gc_UserDataBuf[32+0x14];
								((U8 *)(&tdw_PrevDirCluster))[0]=gc_UserDataBuf[32+0x15];
							    if(!(tdw_PrevDirCluster))
								{//predir is rootdir
									tdw_PrevDirCluster=gdw_DOS_RootDirClus;
								}
								ptr.dw_File_StartCluster=ptr.dw_FDB_StartCluster;
								ptr.dw_FDB_StartCluster=tdw_PrevDirCluster;	              //return to predir

								if((tc_Mode&0x0f)==0x0b) //add by Ching 051220
									tc_FindDir=0;
								
								tc_FindFile=0;

								if(ptr.c_Search_Mode==0x03)
								{//find by order
									gw_DirIndex[1] = 0;
									ptr.c_Search_Mode = K_SPECIFIC_STARTCLUSTER;
									ptr.Compare.dw_StartCluster =ptr.dw_File_StartCluster;
									ptr.c_Search_Attribute=1;
									if(Find_Fdb(&ptr))
										break;
								}
							}
							else
							{//not find dir in rootdir
								gs_DIR_FCB[0].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
								break; 
							}
						}
						else
						{//have found dir
							gb_ReadWriteDataArea=1;
							if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd,1))
								return 0xff;

							tdw_CurrentDirCluster=ptr.dw_FDB_StartCluster; //save current dir
							((U8 *)(&ptr.dw_FDB_StartCluster))[3]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1a];
							((U8 *)(&ptr.dw_FDB_StartCluster))[2]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1b];
							((U8 *)(&ptr.dw_FDB_StartCluster))[1]=gc_UserDataBuf[ptr.w_FDB_Offset+0x14];
							((U8 *)(&ptr.dw_FDB_StartCluster))[0]=gc_UserDataBuf[ptr.w_FDB_Offset+0x15];	//next dir cluster

							tdw_SectorAdd = DOS_ClusterLogicAddr(ptr.dw_FDB_StartCluster);
							if(DOS_Read_LogicSector((tdw_SectorAdd),1))
								return 0xff; 

							((U8 *)(&tdw_PrevDirCluster))[3]=gc_UserDataBuf[32+0x1a];
							((U8 *)(&tdw_PrevDirCluster))[2]=gc_UserDataBuf[32+0x1b];
							((U8 *)(&tdw_PrevDirCluster))[1]=gc_UserDataBuf[32+0x14];
							((U8 *)(&tdw_PrevDirCluster))[0]=gc_UserDataBuf[32+0x15];	//current dir
	
							((U8 *)(&tdw_NextDirCluster))[3]=gc_UserDataBuf[0x1a];
							((U8 *)(&tdw_NextDirCluster))[2]=gc_UserDataBuf[0x1b];
							((U8 *)(&tdw_NextDirCluster))[1]=gc_UserDataBuf[0x14];
							((U8 *)(&tdw_NextDirCluster))[0]=gc_UserDataBuf[0x15];	//next dir

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
									tc_FindDir=1; //for tc_Mode=0x28,it has been changed. Here it's to re-changed
								    gw_DirIndex[1] = 0; 
									ptr.dw_File_StartCluster = ptr.dw_FDB_StartCluster;
									tc_sts=0;
									if((tc_Mode&0x0f)!=0x0b)  //for tc_Mode=0x2b, it must continus counting
										break;									
								} 
								ptr.Compare.dw_BubbleFlag=0x0000;
								tc_FindFile=1; 
							}
							tc_sts=1; 
						}//have find dir 
					}
					else
					{//have find the file --> this line does not occur Here!
						break;
					}
				}//whileB 
			}//find next 
		}//find in all dir
		if(!tc_sts)
		{//find the file/dir
			gb_FindFlag=1;
			if((tc_Mode&0x0f0)==0x20 || (tc_Mode&0x0f0)==0x30)
			{//dir
				gs_DIR_FCB[0].dw_File_StartCluster=ptr.dw_File_StartCluster; 
			    gs_DIR_FCB[0].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
			 	gs_DIR_FCB[0].dw_LongFDB_LogAdd1=ptr.dw_LongFDB_LogAdd1;
			 	gs_DIR_FCB[0].dw_LongFDB_LogAdd0=ptr.dw_LongFDB_LogAdd0;
			 	gs_DIR_FCB[0].dw_FDB_Cluster=ptr.dw_FDB_Cluster;
				gs_DIR_FCB[0].dw_FDB_LogAdd=ptr.dw_FDB_LogAdd;
			 	gs_DIR_FCB[0].w_FDB_Offset=ptr.w_FDB_Offset;
			}
			return(tc_sts); 
		}
		else if((tc_Mode&0x0f)==0x0b)
		{//Count dir total number
			gw_DirTotalNumber += ptr.w_DirTotalNum; //Ching 090204
			//dbprintf("Search----gw_DirTotalNumber=%x\n",gw_DirTotalNumber);
			return(1);
		}
		else
		{ 
			gs_File_FCB[0].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
			gs_DIR_FCB[0].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
			tc_SearchTime--;
			if(((tc_Mode&0x0f)==1)||((tc_Mode&0x0f)==2)||((tc_Mode&0x0f)==3))
				gb_FindFlag=0; 
		} 
	}//while
	return(1); 
}


U8	DOS_Search_DIR_Prev_test(U8 tc_Mode)
{
	U8 tc_sts;
	U8 tc_FindDir,tc_FindFile;      //whether search file or search dir flag
	SearchFdb ptr;
	bit tbt_checkAddr=0;
	
	tc_sts=1;	
	//tbt_IniFlag=0;
	ptr.w_DirTotalNum=0;
	gb_ReadWriteDataArea=1;
	if((tc_Mode&0x0f)==1)
	{//if search file by time, please search by order
		tc_Mode|=0x03;	
	}	
	ptr.pc_LongFileName = gc_FileLongName;
	ptr.c_Search_Mode=(tc_Mode&0x0f);
	//ptr.c_EXTSelect = (tc_PrevOrNext >> 4);//need to compare ExtName;
	ptr.c_EXTSelect = 0; //the directory doesn't need to compare ExtName, default value
	if((tc_Mode&0x0f)==0||(tc_Mode&0x0f)==4||(tc_Mode&0x0f)==5 ||(tc_Mode&0x0f)==0x0b)
	{//count filenum/dirnum , find next
		tc_sts = DOS_Search_DIR_Next_test(tc_Mode);
		return tc_sts;
	}						
	ptr.c_Search_Direction=1;
	ptr.Compare.dw_BubbleFlag = gdw_CurrFlag;
	ptr.dw_File_StartCluster = gdw_StartCluster2;
	ptr.c_type=0;
	//tc_SearchTime=1; 
	//while(tc_SearchTime)
	{
		if((tc_Mode&0x020)==0x20)
		{//find dir 
		 	tc_FindFile=0;
			tc_FindDir=1;
			ptr.dw_FDB_StartCluster=gs_DIR_FCB[0].dw_FDB_StartCluster;
		}
		else
		{//find file
			return(1); //this function isn't used to find file
		}

		//if(tc_PrevOrNext && ((tc_Mode&0x0f)==0x03)) //find prev dir
		{
			U32 tdw_StartCluster, tdw_TagetCluster, tdw_CurrentCluster, tdw_TempCluster=0xffffffff, tdw_TempCluster1=0xffffffff, tdw_MomFDB_StartClus;
			U32 tdw_1stSectorOfCluster, tdw_LogAddr;
			U16 tw_FdbOffset;

			if(gs_DIR_FCB[0].w_FDB_Offset == 0)
			{
				if(gs_DIR_FCB[0].dw_FDB_LogAdd != gdw_DOS_RootDirAddr)
				{
					tw_FdbOffset = 0x1E0;
					tbt_checkAddr = 1;
				}
				else
				{
					if((gs_DIR_FCB[0].dw_FDB_Cluster == gdw_DOS_RootDirClus) && (gs_DIR_FCB[0].dw_File_StartCluster != gdw_DOS_RootDirClus)) //1st dir
					{//1st dir of root dir
						if((tc_Mode&0x0f0)==0x30) //find dir in one dir
						{
							ptr.c_Search_Mode = K_COUNTER_DIRNUM;
							ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_FDB_Cluster;
							ptr.c_Search_Attribute = 1; //find dir
							ptr.c_Search_Direction = 0; //search next
							Count_Dir_Fdb(&ptr);
										
							gw_FileSkipNumber = ptr.w_DirTotalNum;
		
							if(gw_FileSkipNumber>1)
							{
								ptr.c_Search_Mode = K_ORDER_FINDFDB;
								ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_FDB_Cluster;
								ptr.c_Search_Attribute = 1; //find dir
								ptr.c_Search_Direction = 0; //search next
								gw_DirIndex[1] = 0;
		
								gb_TriggerFileSkip = 1;
								tc_sts = Find_Fdb(&ptr);
								gb_TriggerFileSkip = 0;
								if(!tc_sts)
								{
							    		//gs_DIR_FCB[0].dw_FDB_StartCluster = ptr.dw_FDB_StartCluster;
							 		gs_DIR_FCB[0].dw_LongFDB_LogAdd1 = ptr.dw_LongFDB_LogAdd1;
							 		gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = ptr.dw_LongFDB_LogAdd0;
							 		gs_DIR_FCB[0].dw_FDB_Cluster = ptr.dw_FDB_Cluster;
									gs_DIR_FCB[0].dw_FDB_LogAdd = ptr.dw_FDB_LogAdd;
								 	gs_DIR_FCB[0].w_FDB_Offset = ptr.w_FDB_Offset;
		
									if(DOS_Read_LogicSector(gs_DIR_FCB[0].dw_FDB_LogAdd,1))	return 0xff;
									((UBYTE *)(&tdw_StartCluster))[0] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x15];
									((UBYTE *)(&tdw_StartCluster))[1] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x14];
									((UBYTE *)(&tdw_StartCluster))[2] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1b];
									((UBYTE *)(&tdw_StartCluster))[3] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1a];
		
									gs_DIR_FCB[0].dw_File_StartCluster = tdw_StartCluster;
									gs_DIR_FCB[0].dw_FDB_StartCluster  = tdw_StartCluster;

									return 0;
								}
								else	return 1;
							}
							else if(gw_FileSkipNumber==1)	return 0; //the dir itself is what we want to get
							else	return 1;
						}
						else //find in all dir
						{
							gs_DIR_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
							gs_DIR_FCB[0].dw_File_StartCluster = gdw_DOS_RootDirClus;
							gs_DIR_FCB[0].dw_FDB_Cluster = gdw_DOS_RootDirClus;
							gs_DIR_FCB[0].dw_FDB_LogAdd = gdw_DOS_RootDirAddr;
							gs_DIR_FCB[0].w_FDB_Offset = 0;
							gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = 0;
							return 0;
						}
					}
					tw_FdbOffset = 0x1E0;
					tbt_checkAddr = 1;
				}
			}
			else
				tw_FdbOffset = gs_DIR_FCB[0].w_FDB_Offset-32;

			if((gs_DIR_FCB[0].dw_FDB_Cluster <= 2) && (gs_DIR_FCB[0].dw_File_StartCluster <= 2))
			{//root dir
				if((tc_Mode&0x0f0)==0x30) //find dir in one dir -- no dir to get
				{
					return 1;
				}
				else //find in all dir -- the last dir is what we want to get
				{
					U16 tw_count;

					ptr.c_Search_Mode = K_ORDER_FINDFDB;
					ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_File_StartCluster;
					ptr.c_Search_Attribute = 1; //find dir
					ptr.c_Search_Direction = 0; //search next
					gw_DirIndex[1] = 0;
					gb_FindFlag = 0;
			
					for(tw_count=0; tw_count<gw_DirOverallNumber; tw_count++)
					{
						tc_sts = DOS_Search_DIR_Next_test(0x23);
						if(tc_sts) return 1;
					}
					return 0;
				}
			}
			//tdw_MomFDB_StartClus
			tdw_TempCluster = gs_DIR_FCB[0].dw_File_StartCluster;
			tdw_1stSectorOfCluster = DOS_ClusterLogicAddr(tdw_TempCluster);
			if(DOS_Read_LogicSector(tdw_1stSectorOfCluster,1))	return 0xff;
			((UBYTE *)(&tdw_MomFDB_StartClus))[0] = gc_UserDataBuf[0x35];
			((UBYTE *)(&tdw_MomFDB_StartClus))[1] = gc_UserDataBuf[0x34];
			((UBYTE *)(&tdw_MomFDB_StartClus))[2] = gc_UserDataBuf[0x3b];
			((UBYTE *)(&tdw_MomFDB_StartClus))[3] = gc_UserDataBuf[0x3a];
			if(tdw_MomFDB_StartClus == 0) tdw_MomFDB_StartClus = gdw_DOS_RootDirClus;

			tdw_TempCluster = gs_DIR_FCB[0].dw_FDB_Cluster;
			tdw_LogAddr = gs_DIR_FCB[0].dw_FDB_LogAdd;
			tdw_1stSectorOfCluster = DOS_ClusterLogicAddr(tdw_TempCluster);

			if(tbt_checkAddr) //Ching 090213
			{
				if(tdw_LogAddr != tdw_1stSectorOfCluster)
					tdw_LogAddr --;
				else
				{
					tdw_CurrentCluster = tdw_TempCluster;
					tdw_TempCluster1 = tdw_MomFDB_StartClus;
					tdw_TempCluster = tdw_TempCluster1;
					while(tdw_TempCluster1!=tdw_CurrentCluster)
					{
						tdw_TempCluster1 = DOS_GetNextCluster(tdw_TempCluster1,1);
						if(tdw_TempCluster1!=tdw_CurrentCluster)
							tdw_TempCluster = tdw_TempCluster1;
					}
					tdw_1stSectorOfCluster = DOS_ClusterLogicAddr(tdw_TempCluster);
					tdw_LogAddr = tdw_1stSectorOfCluster + gc_DOS_SectorPerCluster - 1;
				}
			}

			if((tc_Mode&0x0f0)==0x30) //find dir in one dir
			{
				do //search in one directory
				{
					tdw_CurrentCluster = tdw_TempCluster;
					for(tdw_LogAddr; tdw_LogAddr >= tdw_1stSectorOfCluster; tdw_LogAddr--) //search in one cluster
					{
						gb_ReadWriteDataArea=1;
						if(DOS_Read_LogicSector(tdw_LogAddr,1))	return 0xff;
	
						while(1) //search in one sector
						{
							if(gc_UserDataBuf[tw_FdbOffset+0x0b] & 0x10) //is dir (short FDB)
							{
								if(gc_UserDataBuf[tw_FdbOffset]!=0xE5)
								{
									if(gc_UserDataBuf[tw_FdbOffset]==0x2E) //jump to the last dir in this dir
									{	// .
										ptr.c_Search_Mode = K_COUNTER_DIRNUM;
										ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_FDB_Cluster;
										ptr.c_Search_Attribute = 1; //find dir
										ptr.c_Search_Direction = 0; //search next
										Count_Dir_Fdb(&ptr);
														
										gw_FileSkipNumber = ptr.w_DirTotalNum;
						
										if(gw_FileSkipNumber>1)
										{
											ptr.c_Search_Mode = K_ORDER_FINDFDB;
											ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_FDB_Cluster;
											ptr.c_Search_Attribute = 1; //find dir
											ptr.c_Search_Direction = 0; //search next
											gw_DirIndex[1] = 0;

											gb_TriggerFileSkip = 1;
											tc_sts = Find_Fdb(&ptr);
											gb_TriggerFileSkip = 0;
											if(!tc_sts)
											{
											    gs_DIR_FCB[0].dw_FDB_StartCluster = ptr.dw_FDB_StartCluster;
											 	gs_DIR_FCB[0].dw_LongFDB_LogAdd1 = ptr.dw_LongFDB_LogAdd1;
											 	gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = ptr.dw_LongFDB_LogAdd0;
											 	gs_DIR_FCB[0].dw_FDB_Cluster = ptr.dw_FDB_Cluster;
												gs_DIR_FCB[0].dw_FDB_LogAdd = ptr.dw_FDB_LogAdd;
											 	gs_DIR_FCB[0].w_FDB_Offset = ptr.w_FDB_Offset;
						
												if(DOS_Read_LogicSector(gs_DIR_FCB[0].dw_FDB_LogAdd,1))	return 0xff;
												((UBYTE *)(&tdw_StartCluster))[0] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x15];
												((UBYTE *)(&tdw_StartCluster))[1] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x14];
												((UBYTE *)(&tdw_StartCluster))[2] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1b];
												((UBYTE *)(&tdw_StartCluster))[3] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1a];
						
												gs_DIR_FCB[0].dw_File_StartCluster = tdw_StartCluster; 

												return 0;
											}
											else if(gw_FileSkipNumber==1)	return 0; //the dir itself is what we want to get
											else	return 1;
										}
									}
									else //found dir
									{
										((UBYTE *)(&tdw_StartCluster))[0] = gc_UserDataBuf[tw_FdbOffset+0x15];
										((UBYTE *)(&tdw_StartCluster))[1] = gc_UserDataBuf[tw_FdbOffset+0x14];
										((UBYTE *)(&tdw_StartCluster))[2] = gc_UserDataBuf[tw_FdbOffset+0x1b];
										((UBYTE *)(&tdw_StartCluster))[3] = gc_UserDataBuf[tw_FdbOffset+0x1a];
		
										gs_DIR_FCB[0].dw_FDB_Cluster = tdw_CurrentCluster;//gs_DIR_FCB[0].dw_FDB_Cluster;
										gs_DIR_FCB[0].dw_FDB_LogAdd = tdw_LogAddr;//gs_DIR_FCB[0].dw_FDB_LogAdd;
										gs_DIR_FCB[0].w_FDB_Offset = tw_FdbOffset;
										gs_DIR_FCB[0].dw_FDB_StartCluster = tdw_StartCluster;
										gs_DIR_FCB[0].dw_File_StartCluster = tdw_StartCluster;
										//Ching 090212
										if(tdw_LogAddr > tdw_1stSectorOfCluster) 
											gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = tdw_LogAddr - 1;
										else //Prev Cluster's last sector
										{
											if(tdw_CurrentCluster == tdw_MomFDB_StartClus) //no prev cluster
												gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = 0;
											else
											{//search prev cluster
												tdw_TempCluster1 = tdw_MomFDB_StartClus;
												tdw_TempCluster = tdw_TempCluster1;
	
												while(tdw_TempCluster1!=tdw_CurrentCluster)
												{
													tdw_TempCluster1 = DOS_GetNextCluster(tdw_TempCluster1,1);
													if(tdw_TempCluster1!=tdw_CurrentCluster)
														tdw_TempCluster = tdw_TempCluster1;
												}
												tdw_1stSectorOfCluster = DOS_ClusterLogicAddr(tdw_TempCluster);
												gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = tdw_1stSectorOfCluster + gc_DOS_SectorPerCluster - 1;
											}
										}
										return 0;
									}
								}
							}
		
							if(tw_FdbOffset==0) break; //break while(1);
							tw_FdbOffset -= 32;
						}//while(1) //search in one sector
						tw_FdbOffset = 0x1E0;
					}//for(tdw_LogAddr) //search in one cluster

					tdw_TempCluster1 = tdw_MomFDB_StartClus;
					tdw_TempCluster = tdw_TempCluster1;
					while(tdw_TempCluster1!=tdw_CurrentCluster)
					{
						tdw_TempCluster1 = DOS_GetNextCluster(tdw_TempCluster1,1);
						if(tdw_TempCluster1!=tdw_CurrentCluster)
							tdw_TempCluster = tdw_TempCluster1;
					}
					tdw_1stSectorOfCluster = DOS_ClusterLogicAddr(tdw_TempCluster);
					tdw_LogAddr = tdw_1stSectorOfCluster + gc_DOS_SectorPerCluster - 1;
				}while(tdw_CurrentCluster != tdw_MomFDB_StartClus); //search in one directory

				if(gs_DIR_FCB[0].dw_FDB_Cluster == gdw_DOS_RootDirClus) //no prev dir in root dir, then last dir in root diris what we want to get
				{
					ptr.c_Search_Mode = K_COUNTER_DIRNUM;
					ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_FDB_Cluster;
					ptr.c_Search_Attribute=1; //find dir
					ptr.c_Search_Direction = 0; //search next
					Count_Dir_Fdb(&ptr);
										
					gw_FileSkipNumber=ptr.w_DirTotalNum;
					if(gw_FileSkipNumber>1)
					{
						ptr.c_Search_Mode = K_ORDER_FINDFDB;
						ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_FDB_Cluster;
						ptr.c_Search_Attribute = 1; //find dir
						ptr.c_Search_Direction = 0; //search next
						gw_DirIndex[1] = 0;
		
						gb_TriggerFileSkip = 1;
						tc_sts = Find_Fdb(&ptr);
						gb_TriggerFileSkip = 0;
						if(!tc_sts)
						{
						    gs_DIR_FCB[0].dw_FDB_StartCluster = ptr.dw_FDB_StartCluster;
						 	gs_DIR_FCB[0].dw_LongFDB_LogAdd1 = ptr.dw_LongFDB_LogAdd1;
						 	gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = ptr.dw_LongFDB_LogAdd0;
						 	gs_DIR_FCB[0].dw_FDB_Cluster = ptr.dw_FDB_Cluster;
							gs_DIR_FCB[0].dw_FDB_LogAdd = ptr.dw_FDB_LogAdd;
						 	gs_DIR_FCB[0].w_FDB_Offset = ptr.w_FDB_Offset;
		
							if(DOS_Read_LogicSector(gs_DIR_FCB[0].dw_FDB_LogAdd,1))	return 0xff;
							((UBYTE *)(&tdw_StartCluster))[0] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x15];
							((UBYTE *)(&tdw_StartCluster))[1] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x14];
							((UBYTE *)(&tdw_StartCluster))[2] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1b];
							((UBYTE *)(&tdw_StartCluster))[3] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1a];
		
							gs_DIR_FCB[0].dw_File_StartCluster = tdw_StartCluster; 

							return 0;
						}
						else	return 1;
					}
					else if(gw_FileSkipNumber==1)	return 0; //the dir itself is what we want to get
					else	return 1;
				}
			}
			else //(tc_Mode&0x0f0)==0x20 //find in all dir
			{
				do //search in one directory
				{
					tdw_CurrentCluster = tdw_TempCluster;
					for(tdw_LogAddr; tdw_LogAddr >= tdw_1stSectorOfCluster; tdw_LogAddr--) //search in one cluster
					{
						gb_ReadWriteDataArea=1;
						if(DOS_Read_LogicSector(tdw_LogAddr,1))	return 0xff;
	
						while(1) //search in one sector
						{
							if(gc_UserDataBuf[tw_FdbOffset+0x0b] & 0x10) //is dir (short FDB)
							{
								if(gc_UserDataBuf[tw_FdbOffset]!=0xE5)
								{
									if(gc_UserDataBuf[tw_FdbOffset]==0x2E) //back to upper dir (upper dir is what we want to get)
									{	// .
										((UBYTE *)(&tdw_TagetCluster))[0]=gc_UserDataBuf[tw_FdbOffset-11]; //-32+0x15
										((UBYTE *)(&tdw_TagetCluster))[1]=gc_UserDataBuf[tw_FdbOffset-12]; //-32+0x14
										((UBYTE *)(&tdw_TagetCluster))[2]=gc_UserDataBuf[tw_FdbOffset-5]; //-32+0x1b
										((UBYTE *)(&tdw_TagetCluster))[3]=gc_UserDataBuf[tw_FdbOffset-6]; //-32+0x1a
										// ..
										((UBYTE *)(&tdw_StartCluster))[0]=gc_UserDataBuf[tw_FdbOffset+0x15];
										((UBYTE *)(&tdw_StartCluster))[1]=gc_UserDataBuf[tw_FdbOffset+0x14];
										((UBYTE *)(&tdw_StartCluster))[2]=gc_UserDataBuf[tw_FdbOffset+0x1b];
										((UBYTE *)(&tdw_StartCluster))[3]=gc_UserDataBuf[tw_FdbOffset+0x1a];
										gw_DirIndex[0]=0;
										ptr.c_Search_Mode = K_SPECIFIC_STARTCLUSTER;
										ptr.Compare.dw_StartCluster = tdw_TagetCluster;
										ptr.dw_FDB_StartCluster = tdw_StartCluster;
										ptr.c_Search_Attribute = 1;  
										ptr.c_Search_Direction = 0; //search next
										if(!Find_Fdb(&ptr))
										{
											gw_DirIndex[1] = 0;
											gs_DIR_FCB[0].dw_FDB_Cluster = ptr.dw_FDB_Cluster;
											gs_DIR_FCB[0].dw_FDB_LogAdd = ptr.dw_FDB_LogAdd;
											gs_DIR_FCB[0].w_FDB_Offset = ptr.w_FDB_Offset;
											gs_DIR_FCB[0].dw_FDB_StartCluster = tdw_TagetCluster;
											gs_DIR_FCB[0].dw_File_StartCluster = tdw_TagetCluster;
											gs_DIR_FCB[0].dw_LongFDB_LogAdd1 = ptr.dw_LongFDB_LogAdd1;
						 					gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = ptr.dw_LongFDB_LogAdd0;

											return 0;
										}
										else
											return 1;
									}
									else //found dir -- check if it has dir
									{
										((UBYTE *)(&tdw_StartCluster))[0] = gc_UserDataBuf[tw_FdbOffset+0x15];
										((UBYTE *)(&tdw_StartCluster))[1] = gc_UserDataBuf[tw_FdbOffset+0x14];
										((UBYTE *)(&tdw_StartCluster))[2] = gc_UserDataBuf[tw_FdbOffset+0x1b];
										((UBYTE *)(&tdw_StartCluster))[3] = gc_UserDataBuf[tw_FdbOffset+0x1a];
		
										gs_DIR_FCB[0].dw_FDB_Cluster = tdw_CurrentCluster;//gs_DIR_FCB[0].dw_FDB_Cluster;
										gs_DIR_FCB[0].dw_FDB_LogAdd = tdw_LogAddr;//gs_DIR_FCB[0].dw_FDB_LogAdd;
										gs_DIR_FCB[0].w_FDB_Offset = tw_FdbOffset;
										gs_DIR_FCB[0].dw_FDB_StartCluster = tdw_StartCluster;
										gs_DIR_FCB[0].dw_File_StartCluster = tdw_StartCluster;

										//check if it has dir
										ptr.c_Search_Mode = K_COUNTER_DIRNUM;
										ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_File_StartCluster;
										ptr.c_Search_Attribute = 1; //find dir
										ptr.c_Search_Direction = 0; //search next
										Count_Dir_Fdb(&ptr);
										
										gw_FileSkipNumber = ptr.w_DirTotalNum;

										if(!gw_FileSkipNumber) //Ching 090212
										{
											if(tdw_LogAddr > tdw_1stSectorOfCluster) 
												gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = tdw_LogAddr - 1;
											else //Prev Cluster's last sector
											{
												if(tdw_CurrentCluster == tdw_MomFDB_StartClus) //no prev cluster
													gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = 0;
												else
												{//search prev cluster
													tdw_TempCluster1 = tdw_MomFDB_StartClus;
													tdw_TempCluster = tdw_TempCluster1;
		
													while(tdw_TempCluster1!=tdw_CurrentCluster)
													{
														tdw_TempCluster1 = DOS_GetNextCluster(tdw_TempCluster1,1);
														if(tdw_TempCluster1!=tdw_CurrentCluster)
															tdw_TempCluster = tdw_TempCluster1;
													}
													tdw_1stSectorOfCluster = DOS_ClusterLogicAddr(tdw_TempCluster);
													gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = tdw_1stSectorOfCluster + gc_DOS_SectorPerCluster - 1;
												}
											}
											return 0;
										}

										while(gw_FileSkipNumber)//if(gw_FileSkipNumber)
										{
											ptr.c_Search_Mode = K_ORDER_FINDFDB;
											ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_File_StartCluster;
											ptr.c_Search_Attribute=1; //find dir
											ptr.c_Search_Direction = 0; //search next
											gw_DirIndex[1] = 0;
		
											gb_TriggerFileSkip = 1;
											tc_sts = Find_Fdb(&ptr);
											gb_TriggerFileSkip = 0;
											if(!tc_sts)
											{
											    gs_DIR_FCB[0].dw_FDB_StartCluster = ptr.dw_FDB_StartCluster;
											 	gs_DIR_FCB[0].dw_LongFDB_LogAdd1 = ptr.dw_LongFDB_LogAdd1;
											 	gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = ptr.dw_LongFDB_LogAdd0;
											 	gs_DIR_FCB[0].dw_FDB_Cluster = ptr.dw_FDB_Cluster;
												gs_DIR_FCB[0].dw_FDB_LogAdd = ptr.dw_FDB_LogAdd;
											 	gs_DIR_FCB[0].w_FDB_Offset = ptr.w_FDB_Offset;
		
												if(DOS_Read_LogicSector(gs_DIR_FCB[0].dw_FDB_LogAdd,1))	return 0xff;
												((UBYTE *)(&tdw_StartCluster))[0] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x15];
												((UBYTE *)(&tdw_StartCluster))[1] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x14];
												((UBYTE *)(&tdw_StartCluster))[2] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1b];
												((UBYTE *)(&tdw_StartCluster))[3] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1a];
		
												gs_DIR_FCB[0].dw_File_StartCluster = tdw_StartCluster; 
												ptr.c_Search_Mode = K_COUNTER_DIRNUM;
												ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_File_StartCluster;
												ptr.c_Search_Attribute = 1; //find dir
												ptr.c_Search_Direction = 0; //search next
												Count_Dir_Fdb(&ptr);
												gw_FileSkipNumber = ptr.w_DirTotalNum;
											}
											else	return 1;
										}		
										return 0;
									}
								}
							}		
							if(tw_FdbOffset==0) break; //break while(1);
							tw_FdbOffset -= 32;
						}//while(1) //search in one sector
						tw_FdbOffset = 0x1E0;
					}//for(tdw_LogAddr) //search in one cluster

					tdw_TempCluster1 = tdw_MomFDB_StartClus;
					tdw_TempCluster = tdw_TempCluster1;
					while(tdw_TempCluster1 != tdw_CurrentCluster)
					{
						tdw_TempCluster1 = DOS_GetNextCluster(tdw_TempCluster1,1);
						if(tdw_TempCluster1 != tdw_CurrentCluster)
							tdw_TempCluster = tdw_TempCluster1;
					}
					tdw_1stSectorOfCluster = DOS_ClusterLogicAddr(tdw_TempCluster);
					tdw_LogAddr = tdw_1stSectorOfCluster + gc_DOS_SectorPerCluster - 1;
				}while(tdw_CurrentCluster != tdw_MomFDB_StartClus); //search in one directory

				if(gs_DIR_FCB[0].dw_FDB_Cluster == gdw_DOS_RootDirClus) //no prev dir in root dir, then root dir is what we want to get
				{
					gs_DIR_FCB[0].dw_FDB_Cluster = gdw_DOS_RootDirClus;
					gs_DIR_FCB[0].dw_FDB_LogAdd = gdw_DOS_RootDirAddr;
					gs_DIR_FCB[0].w_FDB_Offset = 0;
					gs_DIR_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
					gs_DIR_FCB[0].dw_File_StartCluster = gdw_DOS_RootDirClus;
					gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = 0;
					return 0;
				}
			}
		}
 	}//while
	return(1); 
}


void DOS_Count_All_Dir(void)
{
	gs_DIR_FCB[C_OtherFileType].dw_FDB_StartCluster	= gdw_DOS_RootDirClus;
	gs_File_FCB[C_OtherFileType].dw_FDB_StartCluster= gdw_DOS_RootDirClus;
	gb_FindFlag = 0;
	EXT_NameC[0]=1;
	EXT_NameC[1]=0xFE;
	EXT_NameC[2]=0x5A;
	EXT_NameC[3]=0xA5;
	gb_CountDirFlag=1;
	gw_DirTotalNumber=0;
	DOS_Search_File(C_File_All|C_By_FDB, C_OtherFileType, C_CmpExtName|C_Next);
	gb_CountDirFlag=0;
//	dbprintf("======= DIR num:%x\n\n\n\n",gw_DirTotalNumber);//gw_DirTotalNumber
}

 extern xdata 	System_Struct gs_System_State;
UBYTE DOS_GetDIRName(UBYTE tc_FileHandle, UBYTE tb_UicodeToISN, UBYTE * tpc_LongName);

extern	U8	fdb_check_sum(U8 xdata * p);
extern xdata	U8	gc_UIblock0_index;
extern U8  code Unicode_Offset2[26];
extern U16	FDB_OFFSET;
UBYTE DOS_GetDIRName(UBYTE tc_FileHandle, UBYTE tb_UicodeToISN, UBYTE * tpc_LongName)
{

	WORD	i;
	UBYTE	j;
	UBYTE	tc_Chksum=0;
	UBYTE	tc_EndSign=0;
	UBYTE	tc_LogAddrNum=1;
	UBYTE  tc_Array[26];
	WORD	tw_FDB_Offset,tw_LFN_Offset,tw_FileNameLen;
	LWORD	tdw_Addr;
	tb_UicodeToISN = tb_UicodeToISN;

	#if 1
	if(gs_File_FCB[0].dw_FDB_StartCluster==gdw_DOS_RootDirClus)
	{
		j=0;
		tpc_LongName[0]=0;
		tpc_LongName[1]=0;
		tpc_LongName[3]=0;
		tpc_LongName[4]=3;
		//tpc_LongName[4]=0;
		tpc_LongName[2]=K_ShortFileName;
		tpc_LongName[5]=0x46;
		tpc_LongName[6]=0x3A;
		tpc_LongName[7]=0x5C;

		return 0;
	}
	#endif
	tdw_Addr=gs_DIR_FCB[tc_FileHandle].dw_FDB_LogAdd;
	tw_FDB_Offset=gs_DIR_FCB[tc_FileHandle].w_FDB_Offset;


//t	gc_ReadWriteDataArea=1;//use buf gc_UserDataBuf[512]

	DOS_Read_LogicSector(tdw_Addr, 1);

	memcpy(tc_Array,&gc_UserDataBuf[tw_FDB_Offset],11);
	tc_Chksum = fdb_check_sum(tc_Array);
	tw_LFN_Offset=tw_FDB_Offset;
	i=5;//fill the UNICODE from the offset 3! 
	while(i<512)
	{	 
		U8 xdata * p;
		if(tw_LFN_Offset>=32)
		{	
			tw_LFN_Offset-=32;
		}
		else
		{
			if(tc_LogAddrNum==1)
			{
				//tdw_Addr=gs_File_FCB[tc_FileHandle].dw_LongFDB_LogAdd0;
				tdw_Addr=gs_DIR_FCB[tc_FileHandle].dw_LongFDB_LogAdd0;
				tc_LogAddrNum=2;
			} else if(tc_LogAddrNum==2) {
		   		//tdw_Addr=gs_File_FCB[tc_FileHandle].dw_LongFDB_LogAdd1;
				tdw_Addr=gs_DIR_FCB[tc_FileHandle].dw_LongFDB_LogAdd1;

			}
//t			gc_ReadWriteDataArea=1;//use buf gc_UserDataBuf[512]
			DOS_Read_LogicSector(tdw_Addr, 1);
			tw_LFN_Offset=512-32;
		}
		p = &(gc_UserDataBuf[tw_LFN_Offset]);
		//if((gc_UserDataBuf[tw_LFN_Offset]&0x40)==0x40)
		if((*p&0x40)==0x40)
		{
			tc_EndSign=1;
		}	

		//if(((gc_UserDataBuf[tw_LFN_Offset+11]==0x0f)||(gc_UserDataBuf[tw_LFN_Offset+11]==0x3f))&&(gc_UserDataBuf[tw_LFN_Offset+12]==0x00)&&(gc_UserDataBuf[tw_LFN_Offset+13]==tc_Chksum))
		if(((p[11]==0x0f)||(p[11]==0x3f))&&(p[12]==0x00)&&(p[13]==tc_Chksum))
		{	 
			for (j=0; j<26; j++)
				tpc_LongName[i++]=p[Unicode_Offset2[j]];//fill	the long file name into the array
//--------------------------------------------------------------------------------------------------
//if  tc_Chksum is not right,or if sign number is not right,fill the short file name into the array
//----------------------------------------------------------------------------------------------------		
		} 
		else 
		{ 
			j=0;
//t			gc_ReadWriteDataArea=1;//use buf gc_UserDataBuf[512]
			//DOS_Read_LogicSector(gs_File_FCB[tc_FileHandle].dw_FDB_LogAdd, 1);
			DOS_Read_LogicSector(gs_DIR_FCB[tc_FileHandle].dw_FDB_LogAdd, 1);

			tpc_LongName[0]=0;
			tpc_LongName[1]=0;
			tpc_LongName[3]=0;
			tpc_LongName[2]=K_ShortFileName;
			while(j<8)
			{
				tpc_LongName[5+j]=tc_Array[j];
				if(tc_Array[j]==0x20)
				{
					break;
				}	
				j++;
			}
			if (tc_Array[8] != 0x20)
			{
				tpc_LongName[5+j]=0x2e;
				j+=1;
			}
			if (tc_Array[8] != 0x20)
			{
				tpc_LongName[5+j]=tc_Array[8];
				j++;
				if (tc_Array[9] != 0x20)
				{
					tpc_LongName[5+j]=tc_Array[9];
					j++;
					if (tc_Array[10] != 0x20)
					{
						tpc_LongName[5+j]=tc_Array[10];
						j++;
				   	}
			  	}
			}
			tpc_LongName[4]=j;
			return 0;		
		}
//-----------------------------------------------------------------------------------------------------------
   		
		if(tc_EndSign) //when end,fill the first 3 bytes indicate the information of long file name!
		{
			//tpc_LongName[0]=gc_UserDataBuf[tw_LFN_Offset];
			tpc_LongName[0]=*p;
			tpc_LongName[2]=K_LongFileName;

#if 0
			for (j = 26; j > 0; j=j-2)
			{
				//if((gc_UserDataBuf[tw_LFN_Offset+Unicode_Offset[j-1]]==0)&&(gc_UserDataBuf[tw_LFN_Offset+Unicode_Offset[j-2]]==0))
				if((p[Unicode_Offset[j-1]]==0)&&(p[Unicode_Offset[j-2]]==0))
				{
					break; 
				}
			}
#else
			for (j = 0; j < 26; j+=2)
			{
				if((p[Unicode_Offset2[j]]==0)&&(p[Unicode_Offset2[j+1]]==0))
//				if (*((U16 xdata *)&(p[Unicode_Offset2[j]])) == 0)
				{
					break; 
				}
			}
			j = 26-j;
#endif
			tpc_LongName[1] = j;
			//tw_FileNameLen=(((tpc_LongName[0]&0x3f)-1)*26)+(26-tpc_LongName[1]-1);
			tw_FileNameLen=(((tpc_LongName[0]&0x3f)-1)*26)+(26-tpc_LongName[1]);//(JC)H0305
			tpc_LongName[3] = ((UBYTE *)(&tw_FileNameLen))[0];            //save the high 4bit 
			tpc_LongName[4] = ((UBYTE *)(&tw_FileNameLen))[1];          //save the low 4bit
//			dbprintf("tw_FileNameLen=%x\n",tw_FileNameLen);
			return 0;	
			
		}
	}

	return 0;

}
