#include "..\Header\SPDA2K.h"
#include "..\LCM\LCM_BMP.h"
#include "..\header\variables.h"

#define K_COUNTER_DIRNUM  11
#define C_DirIdle      	0x00
#define C_DirNext      	0x01
#define C_DirPrev      	0x02
#define C_DirBackUpper	0x03
#define C_DirEnterChild	0x04


#define C_BUF_LENGTH    128     
#define C_LCM_ROW_MAX     4     
#define C_BUF_STRING    122     
#define DIR_OFFSET       18     
#define Glass_ColumnNum 128     



U8 Count_Dir_Fdb(SearchFdb *p_mp)
{
	data	bit	tb_Check1stFDB;
	data	U8  tc_SectorOffset;
	xdata	U16 tw_FdbOffset;
	xdata	U32	tdw_SectorAdd;
	xdata	U32	tdw_S_ClusterNum;
	xdata	U32	tdw_Find_ClusterSize;
	xdata	U32	tdw_TheEndCluster;
	
	p_mp->w_DirTotalNum=0;
	tdw_S_ClusterNum=p_mp->dw_FDB_StartCluster ;
	
	if(gc_DOS_FileSystemType==1)
	{
		tdw_TheEndCluster=0xffff; 		//FAT16
	}
    else
	{
		tdw_TheEndCluster=0x0fffffff; 	//FAT32
	}

	tb_Check1stFDB=0;
	if(p_mp->dw_FDB_StartCluster != gdw_DOS_RootDirClus) 
	{
		tb_Check1stFDB=1;
	}
    
    while(tdw_S_ClusterNum<tdw_TheEndCluster)
    {//find in one dir
        if(tdw_S_ClusterNum)
        {
            tdw_Find_ClusterSize=gc_DOS_SectorPerCluster;
            tdw_SectorAdd=DOS_ClusterLogicAddr(tdw_S_ClusterNum);
        }
        else //FAT16's rootdir
        {
            tdw_Find_ClusterSize=(gdw_DOS_DataAddr-gdw_DOS_RootDirAddr);
            tdw_SectorAdd=gdw_DOS_RootDirAddr;
        }
        for(tc_SectorOffset=0;tc_SectorOffset<tdw_Find_ClusterSize;tc_SectorOffset++)
        {//find in one cluster 
            gb_ReadWriteDataArea=1;
            if(DOS_Read_LogicSector(tdw_SectorAdd+tc_SectorOffset)) //read fail
			{
				return 0xff ;
			}

			//Ching 090420
			if(tb_Check1stFDB) //check . and ..
			{
				tb_Check1stFDB=0;
				if((gc_UserDataBuf[0x00]!=0x2E) || (gc_UserDataBuf[0x0B]!=0x10) || (gc_UserDataBuf[0x20]!=0x2E) || (gc_UserDataBuf[0x21]!=0x2E) || (gc_UserDataBuf[0x2B]!=0x10))
				{
					return 2;
				}
			}

            for(tw_FdbOffset=0;tw_FdbOffset<512;tw_FdbOffset+=32)
            {//find in one sector				
                if(gc_UserDataBuf[tw_FdbOffset]==0xE5)//deleted FDB
                {
				}
                else if(gc_UserDataBuf[tw_FdbOffset+0x0B]==0x0F)//long FDB
                {
				}
                else if(gc_UserDataBuf[tw_FdbOffset+0x0B]&0x10)//short FDB and is DIR
                {
                 	if(gc_UserDataBuf[tw_FdbOffset]!=0x2E) //it's real folder
					{
						p_mp->w_DirTotalNum++;
					}
                }
                else if(gc_UserDataBuf[tw_FdbOffset]==0x00)//end flag
				{
                    return 0;
				}
            }
        }
        tdw_S_ClusterNum=DOS_GetNextCluster(tdw_S_ClusterNum,1);
    }//while(tdw_S_ClusterNum<tdw_TheEndCluster)--------------------------------------------->find in one dir

   	return 0;
}

//======================================================================================================
/*
DOS_Search_File()/DOS_Search_DIR():
tc_Mode, 
	  High 4 bits present searching overall or just in one dir,
					       	0:Search File overall
			     		   	1:Search File in one dir
							2:Search Dir overall
							3:Search Dir in one dir 		//DOS_Search_DIR()
      Low 4 bits present Search Mode,
         					0: Count File Total number
                            1: Find by Time
                            2: Find by Name
                            3: Find by FDB
					 		4: Find by Long File Name
							5: Find by Short File Name
                            B: Count Dir Number		 //DOS_Search_DIR()
                              		
tc_PrevOrNext, presents the searching direction,
							0: Search Next
                       		1: Search Pre
	 (CountFileTotalNum and FindByFDB defaults searching next)
*/
//======================================================================================================
U8	DOS_Search_DIR_Next_test(U8 tc_Mode)
{
	data	bit	tb_FindDir;
	data	U8	tc_sts;
	data	U8	tc_SearchTime;
	xdata	U32 tdw_SectorAdd;
	xdata	U32	tdw_CurrentDirCluster;
	xdata	U32	tdw_PrevDirCluster;
	xdata	U32	tdw_NextDirCluster;
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
	{
		gw_DirTotalNumber=0;
	}

	ptr.c_Search_Direction=0; //Next
	ptr.Compare.dw_BubbleFlag = gdw_CurrFlag;
	ptr.dw_File_StartCluster = gdw_StartCluster2;
	ptr.c_type=0;
	tc_SearchTime=2; 
	while(tc_SearchTime)
	{
		if((tc_Mode&0x0f0)==0x20 || (tc_Mode&0x0f0)==0x30)
		{//find dir 
			tb_FindDir=1;
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
				tb_FindDir=1;
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
			{
				tc_sts=Count_Dir_Fdb(&ptr);
			}
			else
			{
				tc_sts=Find_Fdb(&ptr);
			}			

			gb_ReadWriteDataArea=1;
			if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd))
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
					if(tb_FindDir==1 && ptr.c_Search_Mode==0x0b)
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
								if(DOS_Read_LogicSector(tdw_SectorAdd))
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
									tb_FindDir=0;
								
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
							if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd))
								return 0xff;

							tdw_CurrentDirCluster=ptr.dw_FDB_StartCluster; //save current dir
							((U8 *)(&ptr.dw_FDB_StartCluster))[3]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1a];
							((U8 *)(&ptr.dw_FDB_StartCluster))[2]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1b];
							((U8 *)(&ptr.dw_FDB_StartCluster))[1]=gc_UserDataBuf[ptr.w_FDB_Offset+0x14];
							((U8 *)(&ptr.dw_FDB_StartCluster))[0]=gc_UserDataBuf[ptr.w_FDB_Offset+0x15];	//next dir cluster

							tdw_SectorAdd = DOS_ClusterLogicAddr(ptr.dw_FDB_StartCluster);
							if(DOS_Read_LogicSector((tdw_SectorAdd)))
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
							}
							else
							{
								gw_DirIndex[0] = 0;
								if((tc_Mode&0x0f0)==0x20)
								{//put the line in here is to make dw_FDB_StartCluster equal dw_File_StartCluster 
									tb_FindDir=1; //for tc_Mode=0x28,it has been changed. Here it's to re-changed
								    gw_DirIndex[1] = 0; 
									ptr.dw_File_StartCluster = ptr.dw_FDB_StartCluster;
									tc_sts=0;
									if((tc_Mode&0x0f)!=0x0b)  //for tc_Mode=0x2b, it must continus counting
										break;									
								} 
								ptr.Compare.dw_BubbleFlag=0x0000;
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
			{
				gb_FindFlag=0; 
			}
		} 
	}//while
	//Libprintf("no file to find\n"); 
	return(1); 
}


U8	DOS_Search_DIR_Prev_test(U8 tc_Mode)
{
	data	bit	tb_checkAddr;
	data	U8	tc_sts;
	SearchFdb ptr;
	
	tc_sts=1;	
	tb_checkAddr=0;
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
					tb_checkAddr = 1;
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
		
									if(DOS_Read_LogicSector(gs_DIR_FCB[0].dw_FDB_LogAdd))	return 0xff;
									((U8 *)(&tdw_StartCluster))[0] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x15];
									((U8 *)(&tdw_StartCluster))[1] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x14];
									((U8 *)(&tdw_StartCluster))[2] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1b];
									((U8 *)(&tdw_StartCluster))[3] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1a];
		
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
					tb_checkAddr = 1;
				}
			}
			else
				tw_FdbOffset = gs_DIR_FCB[0].w_FDB_Offset-32;

			if((gs_DIR_FCB[0].dw_FDB_Cluster <= 2) && (gs_DIR_FCB[0].dw_File_StartCluster <= 2))
			{//root dir
				if((tc_Mode&0x0f0)==0x30) //find dir in one dir -- no dir to get
					return 1;
				else //find in all dir -- the last dir is what we want to get
				{
					U16 tw_count;

					ptr.c_Search_Mode = K_ORDER_FINDFDB;
					ptr.dw_FDB_StartCluster = gs_DIR_FCB[0].dw_File_StartCluster;
					ptr.c_Search_Attribute = 1; //find dir
					ptr.c_Search_Direction = 0; //search next
					gw_DirIndex[1] = 0;
					gb_FindFlag = 0;
			
					for(tw_count=0; tw_count<gw_DirTotalNumber; tw_count++)
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
			if(DOS_Read_LogicSector(tdw_1stSectorOfCluster))	return 0xff;
			((U8 *)(&tdw_MomFDB_StartClus))[0] = gc_UserDataBuf[0x35];
			((U8 *)(&tdw_MomFDB_StartClus))[1] = gc_UserDataBuf[0x34];
			((U8 *)(&tdw_MomFDB_StartClus))[2] = gc_UserDataBuf[0x3b];
			((U8 *)(&tdw_MomFDB_StartClus))[3] = gc_UserDataBuf[0x3a];
			if(tdw_MomFDB_StartClus == 0) tdw_MomFDB_StartClus = gdw_DOS_RootDirClus;

			tdw_TempCluster = gs_DIR_FCB[0].dw_FDB_Cluster;
			tdw_LogAddr = gs_DIR_FCB[0].dw_FDB_LogAdd;
			tdw_1stSectorOfCluster = DOS_ClusterLogicAddr(tdw_TempCluster);

			if(tb_checkAddr) //Ching 090213
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
						if(DOS_Read_LogicSector(tdw_LogAddr))	return 0xff;
	
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
						
												if(DOS_Read_LogicSector(gs_DIR_FCB[0].dw_FDB_LogAdd))	return 0xff;
												((U8 *)(&tdw_StartCluster))[0] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x15];
												((U8 *)(&tdw_StartCluster))[1] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x14];
												((U8 *)(&tdw_StartCluster))[2] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1b];
												((U8 *)(&tdw_StartCluster))[3] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1a];
						
												gs_DIR_FCB[0].dw_File_StartCluster = tdw_StartCluster; 

												return 0;
											}
											else if(gw_FileSkipNumber==1)	return 0; //the dir itself is what we want to get
											else	return 1;
										}
									}
									else //found dir
									{
										((U8 *)(&tdw_StartCluster))[0] = gc_UserDataBuf[tw_FdbOffset+0x15];
										((U8 *)(&tdw_StartCluster))[1] = gc_UserDataBuf[tw_FdbOffset+0x14];
										((U8 *)(&tdw_StartCluster))[2] = gc_UserDataBuf[tw_FdbOffset+0x1b];
										((U8 *)(&tdw_StartCluster))[3] = gc_UserDataBuf[tw_FdbOffset+0x1a];
		
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
		
							if(DOS_Read_LogicSector(gs_DIR_FCB[0].dw_FDB_LogAdd))	return 0xff;
							((U8 *)(&tdw_StartCluster))[0] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x15];
							((U8 *)(&tdw_StartCluster))[1] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x14];
							((U8 *)(&tdw_StartCluster))[2] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1b];
							((U8 *)(&tdw_StartCluster))[3] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1a];
		
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
						if(DOS_Read_LogicSector(tdw_LogAddr))	return 0xff;
	
						while(1) //search in one sector
						{
							if(gc_UserDataBuf[tw_FdbOffset+0x0b] & 0x10) //is dir (short FDB)
							{
								if(gc_UserDataBuf[tw_FdbOffset]!=0xE5)
								{
									if(gc_UserDataBuf[tw_FdbOffset]==0x2E) //back to upper dir (upper dir is what we want to get)
									{	// .
										((U8 *)(&tdw_TagetCluster))[0]=gc_UserDataBuf[tw_FdbOffset-11]; //-32+0x15
										((U8 *)(&tdw_TagetCluster))[1]=gc_UserDataBuf[tw_FdbOffset-12]; //-32+0x14
										((U8 *)(&tdw_TagetCluster))[2]=gc_UserDataBuf[tw_FdbOffset-5]; //-32+0x1b
										((U8 *)(&tdw_TagetCluster))[3]=gc_UserDataBuf[tw_FdbOffset-6]; //-32+0x1a
										// ..
										((U8 *)(&tdw_StartCluster))[0]=gc_UserDataBuf[tw_FdbOffset+0x15];
										((U8 *)(&tdw_StartCluster))[1]=gc_UserDataBuf[tw_FdbOffset+0x14];
										((U8 *)(&tdw_StartCluster))[2]=gc_UserDataBuf[tw_FdbOffset+0x1b];
										((U8 *)(&tdw_StartCluster))[3]=gc_UserDataBuf[tw_FdbOffset+0x1a];
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
										((U8 *)(&tdw_StartCluster))[0] = gc_UserDataBuf[tw_FdbOffset+0x15];
										((U8 *)(&tdw_StartCluster))[1] = gc_UserDataBuf[tw_FdbOffset+0x14];
										((U8 *)(&tdw_StartCluster))[2] = gc_UserDataBuf[tw_FdbOffset+0x1b];
										((U8 *)(&tdw_StartCluster))[3] = gc_UserDataBuf[tw_FdbOffset+0x1a];
		
										gs_DIR_FCB[0].dw_FDB_Cluster = tdw_CurrentCluster;//gs_DIR_FCB[0].dw_FDB_Cluster;
										gs_DIR_FCB[0].dw_FDB_LogAdd = tdw_LogAddr;//gs_DIR_FCB[0].dw_FDB_LogAdd;
										gs_DIR_FCB[0].w_FDB_Offset = tw_FdbOffset;
										gs_DIR_FCB[0].dw_FDB_StartCluster = tdw_StartCluster;
										gs_DIR_FCB[0].dw_File_StartCluster = tdw_StartCluster;

										//check if it has dir
										ptr.c_Search_Mode=K_COUNTER_DIRNUM;
										ptr.dw_FDB_StartCluster=gs_DIR_FCB[0].dw_File_StartCluster;
										ptr.c_Search_Attribute=1; //find dir
										ptr.c_Search_Direction=0; //search next
										Count_Dir_Fdb(&ptr);
										
										gw_FileSkipNumber=ptr.w_DirTotalNum;

										if(!gw_FileSkipNumber) //Ching 090212
										{
											if(tdw_LogAddr>tdw_1stSectorOfCluster) 
											{
												gs_DIR_FCB[0].dw_LongFDB_LogAdd0=tdw_LogAddr-1;
											}
											else //Prev Cluster's last sector
											{
												if(tdw_CurrentCluster==tdw_MomFDB_StartClus) //no prev cluster
													gs_DIR_FCB[0].dw_LongFDB_LogAdd0=0;
												else
												{//search prev cluster
													tdw_TempCluster1=tdw_MomFDB_StartClus;
													tdw_TempCluster=tdw_TempCluster1;
		
													while(tdw_TempCluster1!=tdw_CurrentCluster)
													{
														tdw_TempCluster1=DOS_GetNextCluster(tdw_TempCluster1,1);
														if(tdw_TempCluster1!=tdw_CurrentCluster)
														{
															tdw_TempCluster=tdw_TempCluster1;
														}
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
		
												if(DOS_Read_LogicSector(gs_DIR_FCB[0].dw_FDB_LogAdd))	return 0xff;
												((U8 *)(&tdw_StartCluster))[0] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x15];
												((U8 *)(&tdw_StartCluster))[1] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x14];
												((U8 *)(&tdw_StartCluster))[2] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1b];
												((U8 *)(&tdw_StartCluster))[3] = gc_UserDataBuf[gs_DIR_FCB[0].w_FDB_Offset+0x1a];
		
												gs_DIR_FCB[0].dw_File_StartCluster = tdw_StartCluster; 
												ptr.c_Search_Mode=K_COUNTER_DIRNUM;
												ptr.dw_FDB_StartCluster=gs_DIR_FCB[0].dw_File_StartCluster;
												ptr.c_Search_Attribute=1; //find dir
												ptr.c_Search_Direction=0; //search next
												Count_Dir_Fdb(&ptr);
												gw_FileSkipNumber=ptr.w_DirTotalNum;
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


U8 GetCurrentDirInfo()
{
	U32 tdw_Addr, tdw_TagetCluster, tdw_StartCluster;
	SearchFdb ptr;

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
	if(DOS_Read_LogicSector(tdw_Addr))	return 0xff;
	// .
	((U8 *)(&tdw_TagetCluster))[0]=gc_UserDataBuf[0x15];
	((U8 *)(&tdw_TagetCluster))[1]=gc_UserDataBuf[0x14];
	((U8 *)(&tdw_TagetCluster))[2]=gc_UserDataBuf[0x1b];
	((U8 *)(&tdw_TagetCluster))[3]=gc_UserDataBuf[0x1a];
	// ..
	((U8 *)(&tdw_StartCluster))[0]=gc_UserDataBuf[0x35];
	((U8 *)(&tdw_StartCluster))[1]=gc_UserDataBuf[0x34];
	((U8 *)(&tdw_StartCluster))[2]=gc_UserDataBuf[0x3b];
	((U8 *)(&tdw_StartCluster))[3]=gc_UserDataBuf[0x3a];
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
	{
		return 1;
	}
}


U8 DOS_Search_DIR(U8 tc_Mode)
{
	data	bit	tb_FindDir;
	data	U8	tc_sts;
	data	U8	tc_SearchTime;
	xdata	U32 tdw_SectorAdd,tdw_CurrentDirCluster,tdw_PrevDirCluster,tdw_NextDirCluster;
	SearchFdb ptr;

   	gs_File_FCB[0].dw_File_StartCluster=0xffffffff ;
    gs_File_FCB[0].dw_FDB_LogAdd=0 ;
    tc_sts=1 ;

    ptr.w_DirTotalNum=0 ;
    gb_ReadWriteDataArea=1 ;

    ptr.pc_LongFileName=gc_FileLongName ;
    ptr.c_Search_Mode=tc_Mode&0x0f ; //0x03 or 0x0B
    ptr.c_EXTSelect=0 ;			//the directory doesn't need to compare ExtName, default value
    ptr.c_Search_Direction=0 ;	//defalut: search next
	ptr.c_Search_Attribute=1 ;	//default: find dir
    ptr.c_type=0 ; 				//default
	ptr.Compare.dw_BubbleFlag=gdw_CurrFlag ;
	ptr.dw_File_StartCluster=gdw_StartCluster2 ;
    tc_SearchTime=2 ;
    while(tc_SearchTime)
    {
        {//find dir in one dir
            tb_FindDir=1 ;
            ptr.dw_FDB_StartCluster=gs_DIR_FCB[0].dw_FDB_StartCluster ;
        }

        if(!(gb_FindFlag))
        {//have not searched             		       
			gb_FindFlag=1 ;
            //first search next: default
            gw_DirIndex[0]=0 ;
            gw_DirIndex[1]=0 ;
        }
        if((tc_Mode&0x0f0)==0x30)//find dir in one dir
        {            
            if((tc_Mode&0x0f)==0x0b)//count dir total number
			{
				tc_sts=Count_Dir_Fdb(&ptr);
			}
            else
			{
                tc_sts=Find_Fdb(&ptr);
	            if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd)) //read fail
	            	return 0xff ;
	            
	            ((U8*)(&ptr.dw_File_StartCluster))[3]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1a];
	            ((U8*)(&ptr.dw_File_StartCluster))[2]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1b];
	            ((U8*)(&ptr.dw_File_StartCluster))[1]=gc_UserDataBuf[ptr.w_FDB_Offset+0x14];
	            ((U8*)(&ptr.dw_File_StartCluster))[0]=gc_UserDataBuf[ptr.w_FDB_Offset+0x15];
			}
        }
		else //(tc_Mode&0x0f0)==0x20
		{//find in all dir
			while(1)
			{ 
				ptr.c_Search_Mode = (tc_Mode&0x0f);
												
				if(tb_FindDir==1 && ptr.c_Search_Mode==0x0b)
				{//count dir number in all directories
					tc_sts=Count_Dir_Fdb(&ptr);
					gw_DirTotalNumber = ptr.w_DirTotalNum;
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
							if(DOS_Read_LogicSector(tdw_SectorAdd))
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
								tb_FindDir=0;
							
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
						if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd))
							return 0xff;

						tdw_CurrentDirCluster=ptr.dw_FDB_StartCluster; //save current dir
						((U8 *)(&ptr.dw_FDB_StartCluster))[3]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1a];
						((U8 *)(&ptr.dw_FDB_StartCluster))[2]=gc_UserDataBuf[ptr.w_FDB_Offset+0x1b];
						((U8 *)(&ptr.dw_FDB_StartCluster))[1]=gc_UserDataBuf[ptr.w_FDB_Offset+0x14];
						((U8 *)(&ptr.dw_FDB_StartCluster))[0]=gc_UserDataBuf[ptr.w_FDB_Offset+0x15];	//next dir cluster

						tdw_SectorAdd = DOS_ClusterLogicAddr(ptr.dw_FDB_StartCluster);
						if(DOS_Read_LogicSector(tdw_SectorAdd))
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
						}
						else
						{
							gw_DirIndex[0] = 0;
							if((tc_Mode&0x0f0)==0x20)
							{//put the line in here is to make dw_FDB_StartCluster equal dw_File_StartCluster 
								tb_FindDir=1; //for tc_Mode=0x28,it has been changed. Here it's to re-changed
							    gw_DirIndex[1] = 0; 
								ptr.dw_File_StartCluster = ptr.dw_FDB_StartCluster;
								tc_sts=0;
								if((tc_Mode&0x0f)!=0x0b)  //for tc_Mode=0x2b, it must continus counting
									break;									
							} 
							ptr.Compare.dw_BubbleFlag=0x0000;
						}
						tc_sts=1; 
					}//have find dir 
				}
				else
				{//have find the file --> this line does not occur Here!
					break;
				}
			}//whileB 
		}//find in all dir

        if(!tc_sts)
        {//found the file/dir or counted dir total number successfully
			if((tc_Mode&0x0f)==0x0b)//count dir total number
	            gw_DirTotalNumber=ptr.w_DirTotalNum ;
            else //if((tc_Mode&0x0f)==0x03)
            {
	            gb_FindFlag=1 ;
                //dir
                gs_DIR_FCB[0].dw_File_StartCluster=ptr.dw_File_StartCluster ;
                gs_DIR_FCB[0].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster ;
                gs_DIR_FCB[0].dw_LongFDB_LogAdd1=ptr.dw_LongFDB_LogAdd1 ;
                gs_DIR_FCB[0].dw_LongFDB_LogAdd0=ptr.dw_LongFDB_LogAdd0 ;
                gs_DIR_FCB[0].dw_FDB_Cluster=ptr.dw_FDB_Cluster ;
                gs_DIR_FCB[0].dw_FDB_LogAdd=ptr.dw_FDB_LogAdd ;
                gs_DIR_FCB[0].w_FDB_Offset=ptr.w_FDB_Offset ;	
            }
            return tc_sts;
        }
        else 
        {
			if((tc_Mode&0x0f)==0x0b)
				return tc_sts;
			else //if((tc_Mode&0x0f)==3)
				gb_FindFlag=0 ;

			gb_LastFolder=1;
            gs_File_FCB[0].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
            gs_DIR_FCB[0].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;

            tc_SearchTime--;
        }
    }  
    return(1);
}

void PlayPrevFolder(void)
{
	data	U8	tc_SearchDirTimeout;
	data	U8	tc_clock_mode_backup;
	xdata	U32	dw_File_StartCluster_Backup;
	xdata	U32	dw_FDB_StartCluster_Backup;

	if(gw_FileTotalNumber[0]!=0)
	{										
		gb_DirPlay_Flag=1;

		play_stop();
		gb_LrcFileName_Exist=0;
		tc_clock_mode_backup=gc_clock_mode;
		set_clock_mode(CLOCK_MODE_DOS);
		GetCurrentDirInfo();

		for(tc_SearchDirTimeout=0;tc_SearchDirTimeout<255;tc_SearchDirTimeout++)
		{
			DOS_Search_DIR_Prev_test(0x23);

			gb_FindFlag=0;

			if(gw_CurrentFolderNum==0)
			{
				gw_CurrentFolderNum=gw_DirTotalNumber;
			}
			else
			{
				gw_CurrentFolderNum--;
			}
			
		//	dbprintf("gw_CurrentFolderNum=%x\n",gw_CurrentFolderNum);

			gs_File_FCB[0].dw_FDB_StartCluster = gs_DIR_FCB[0].dw_File_StartCluster;

			if(gc_CurrentCard==9)
			{
				return;	
			}
			else if(gc_CurrentCard==2&&gb_SD_pin==1)	// SD remove
			{
				return;
			}
			else if(gc_CurrentCard==5&&Host_DetectDevice()==1)	// Host remove
			{
				return;
			}

			dw_File_StartCluster_Backup=gs_DIR_FCB[0].dw_File_StartCluster;
			dw_FDB_StartCluster_Backup=gs_DIR_FCB[0].dw_FDB_StartCluster;
			gb_LastFolder=0;
			DOS_Search_File(C_File_OneDir|C_Cnt_FileNo,C_MusicFHandle,C_CmpExtName|C_Next);
			if(gw_FileTotalNumber[0])
			{
				DOS_Search_File(C_File_OneDir|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
				gw_FileIndex[0]=1;
				break;
			}
			gs_DIR_FCB[0].dw_File_StartCluster=dw_File_StartCluster_Backup;
			gs_DIR_FCB[0].dw_FDB_StartCluster=dw_FDB_StartCluster_Backup;
		}
		set_clock_mode(tc_clock_mode_backup);
		LCM_ShowSongNumber();
		gb_FindFlag=1;
		gb_Mp3FileEnd=1;
		gb_LogDataFlag=1;
		gb_DirPlay_Flag=1;
	}
}


void PlayNextFolder(void)
{
	data	U8	tc_SearchDirTimeout;
	data	U8	tc_clock_mode_backup;

//	if(gw_FileTotalNumber[0]!=0)
	{
		play_stop();
		gb_LrcFileName_Exist=0;
		tc_clock_mode_backup=gc_clock_mode;
		set_clock_mode(CLOCK_MODE_DOS);

		for(tc_SearchDirTimeout=0;tc_SearchDirTimeout<255;tc_SearchDirTimeout++)
		{
			gs_DIR_FCB[0].dw_FDB_StartCluster=gs_File_FCB[0].dw_FDB_StartCluster;							
			DOS_Search_DIR(0x23);

			if(gw_CurrentFolderNum==gw_DirTotalNumber)
			{
				gw_CurrentFolderNum=0;
			}
			else
			{
				gw_CurrentFolderNum++;
			}

			gb_FindFlag=0;
			if(gb_LastFolder==0)
			{
				gs_File_FCB[0].dw_FDB_StartCluster=gs_DIR_FCB[0].dw_File_StartCluster;
			}
			else
			{
				gs_File_FCB[0].dw_FDB_StartCluster=gdw_DOS_RootDirClus;
				gb_LastFolder=0;
			}

			if(gc_CurrentCard==9)
			{
				return;	
			}
			else if(gc_CurrentCard==2&&gb_SD_pin==1)	// SD remove
			{
				return;
			}
			else if(gc_CurrentCard==5&&Host_DetectDevice()==1)	// Host remove
			{
				return;
			}

			DOS_Search_File(C_File_OneDir|C_Cnt_FileNo,C_MusicFHandle,C_CmpExtName|C_Next);
			if(gw_FileTotalNumber[0])
			{
				DOS_Search_File(C_File_All|C_By_FDB,C_MusicFileType,C_CmpExtName|C_Next);
				gw_FileIndex[0]=1;
				break;
			}														
		}
		set_clock_mode(tc_clock_mode_backup);
		LCM_ShowSongNumber();
		gb_FindFlag=1;
		gb_Mp3FileEnd=1;
		gb_LogDataFlag=1;
		gb_DirPlay_Flag=1;
	}				
}



void DOS_Count_All_Dir(void) //Ching 100514
{
	gs_DIR_FCB[C_OtherFileType].dw_FDB_StartCluster=gdw_DOS_RootDirClus;
	gs_File_FCB[C_OtherFileType].dw_FDB_StartCluster=gdw_DOS_RootDirClus;
	gb_FindFlag=0;
	EXT_NameC[0]=1;
	EXT_NameC[1]=0xFE;
	EXT_NameC[2]=0x5A;
	EXT_NameC[3]=0xA5;
	gb_CountDirFlag=1;
	gw_DirTotalNumber=0;
	DOS_Search_File(C_File_All|C_By_FDB,C_OtherFileType,C_CmpExtName|C_Next);
	gb_CountDirFlag=0;
}
