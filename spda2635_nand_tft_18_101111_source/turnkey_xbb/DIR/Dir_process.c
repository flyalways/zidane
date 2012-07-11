#include <string.h>
#include "..\Header\SPDA2K.h"
#include "..\header\dos\fs_struct.h"
#include "Dir_process.h"
#include "..\header\Memalloc.h"
#include "..\LCM\model_define.h"//(JC)H1124
#include "..\Header\unicode_hzk.h"
#include "..\header\variables.h"
#include "..\LCM\TFT_config.h"
#include "..\LCM\TFT_display.h"
#include "..\header\host_init.h"

#define USE_TFT_disp_HZKCharBMP
#define USE_UI_StringTrigger
#define USE_UI_get_string_hzk_bitmap
#define USE_UI_render_strings

#include "..\LCM\LCM.h"
#include "..\LCM\LCM_func.h"
#include "..\LCM\UI_icon.h"

#include "..\LCM\common_LCM_func.c"
#include "..\UI_Display\common_UI_disp_func.c"
#include "..\UI_Display\unicode_hzk.c"

xdata	U8	gc_IconType;
xdata	U8	gc_IconY;
xdata	U16	gc_DispFilePage,gc_DispFilePageBak;

extern xdata System_Struct gs_System_State;
  /*
#define C_LCM_ROW_MAX   5
#define C_BUF_LENGTH	85 //512/6
#define C_BUF_STRING	80 //取小於85-5且為偶數者
   */

U8 DOS_DIRtable(void);
U8 DOS_Search_DIR(U8 tc_Mode);
U8 Count_Dir_Fdb(SearchFdb *p_mp);
void USER_MarkTable(U8 tc_MarkFlag);
void USER_GetUISetIndex(void);
U8 LCD_disp_HZK_string(U8 Page,U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 ColumnOffset,bit reverse);
void UI_DispGetDirName(U8 tc_DIRtableIndex);
void FatherDir_FDBInit(U16 tw_FatherIndex);
void Get_TotalNumber(U16 tw_DirIndex);
void UI_DispGet_FatherName(void);
void UI_DispGet_ChildName(void);
void LCD_Disp_FatherName_Dir(void);
void LCD_erase_one_page(U8 tc_page);
void UI_Disp_ChildName();
void DirManagement_Init();
void UI_DIRStoreOrRecover(bit tb_SaveOrRecover);
void DirManagement_Process();
void Dir_Task();

U8 GetCurrentDirInfo();

extern  U8   Find_Fdb(SearchFdb *p_mp);
extern 	U8   Get_LogData_PageIndex(void);
extern 	U8   USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);
extern void ir_service_menu(void);  //20090206 chiayen add for IR_DIR

extern UBYTE  xdata EXT_NameC[];
extern	data	bit	gb_TFT_refresh;
data bit gb_SourceChange_Dir=0;
bit gbt_DIRwFakeFDB=0;


U8 DOS_DIRtable(void)
{
	U32 tdw_LongFDB_LogAdd1;
	U16 tw_DirTabNum, tw_DirTotalNumber;
	U16 tw_TempTotalNum; //add for the sum of dir and file number in one directory
	U16 tw_DirIndex[2];
	U16 tw_FatherIndex, tw_OldFatherIndex;
	U8  tc_DirTableNo, tc_IndexInOneDir;
	U8  tc_SpanNumber; //跨越2KB Page的標誌位, 0: 1st page, 1: 2nd page...
	U8	tc_CurrentCard = gc_CurrentCard; //Ching 090306
	bit tb_HasNextDirFlag=0;

	if(tc_CurrentCard!=0)  //20090721 chiayen add for host
	{
		tc_CurrentCard=2;	
	}
	
	if(!tc_CurrentCard)
	{
		gc_DirReBuildFlag=D_Expired;//Mark DirTable is In-Valid
		USER_MarkTable(gc_DirReBuildFlag);//在保留區標誌DirTable無效，防止建立Table時突然掉電
		gc_SetIndex++; //NandFlash
	}
	else	
	{
		gc_SDSetIndex++; //SD or HOST
	}

	memset(gc_DIRtable,0,2048);	//clear buffer gc_DIRtable
	memset(gc_DirLCMtempBuf,0,512);	//clear buffer gc_DIRtable
	{//1st 32-byte is used for root dir, whose father/previous/next folder are still root dir (Offset 0)
		gs_DIRtable[0].c_occupied_flag		= 1;
		gs_DIRtable[0].dw_File_StartCluster	= gdw_DOS_RootDirClus;
		gs_DIRtable[0].dw_FDB_StartCluster	= gdw_DOS_RootDirClus;

		tw_DirIndex[0]=0;
		tw_DirIndex[1]=0;
		tdw_LongFDB_LogAdd1 = gs_DIR_FCB[0].dw_LongFDB_LogAdd1;
		gs_DIR_FCB[0].dw_FDB_StartCluster	= gs_DIRtable[0].dw_File_StartCluster;
		gs_File_FCB[0].dw_FDB_StartCluster	= gs_DIRtable[0].dw_File_StartCluster;
		gb_FindFlag = 0;
		DOS_Search_DIR(0x3b);			//count directory number in this directory, get gw_DirTotalNumber
		DOS_Search_File(0x10,0,0x10);	//count file number in this directory, get gw_FileTotalNumber[0]

		if(gw_DirTotalNumber>=0xff)
		{
			gw_DirTotalNumber=0xff;
		}

		tw_TempTotalNum=gw_DirTotalNumber+gw_FileTotalNumber[0];

		gs_DIRtable[0].w_file_folder_num	= tw_TempTotalNum;
		gs_DIRtable[0].c_file_num_hi		= ((U8 *)(&gw_FileTotalNumber[0]))[0];//save file number High 8bit of this directory
		gs_DIRtable[0].c_file_num_lo		= ((U8 *)(&gw_FileTotalNumber[0]))[1];//save file number Low 8bit of this directory
		gs_DIRtable[0].c_subdir_num			= ((U8 *)(&gw_DirTotalNumber))[1];	//save dir number of this directory

		if(gs_DIRtable[0].c_subdir_num == 0) //No Directory
 	    {
			USER_WriteReserveBlock_DIR(0, tc_CurrentCard);
//			dbprintf("NoDir\n");
			if(!tc_CurrentCard)
			{
			gc_DirReBuildFlag=0x55; //Mark DirTable is Valid, Ching 081003
			USER_MarkTable(gc_DirReBuildFlag);
			}
			return 2;
		}
		else
		{
			gs_DIRtable[0].c_index_child_folder = 1;
		}

		tc_SpanNumber = 0;//前63個目錄信息和Root目錄信息(32x64=共2KB),存在第1個2K Page
		tw_FatherIndex=0;
		tc_IndexInOneDir=1;
		tw_DirTotalNumber=gw_DirTotalNumber;
	}

	for( tw_DirTabNum=1; tw_DirTabNum<256; tw_DirTabNum++ )//擴展到256個Dir
	{
		tc_DirTableNo = tw_DirTabNum&63;
		if(tw_DirTabNum > tw_DirTotalNumber)
		{
			USER_WriteReserveBlock_DIR(tc_SpanNumber, tc_CurrentCard);
			break;
		}

		if(tc_SpanNumber)//當目錄存儲信息有跨2K Page時
		{
			if((tw_FatherIndex&0xC0)!=(tw_DirTabNum&0xC0)) //Father Dir and Child Dir aren't in the same page
				USER_Read512BReserveBlock_DIR(tw_FatherIndex, tc_CurrentCard); //gc_UserDataBuf
			else
				memcpy(&gc_UserDataBuf[0], &gc_DIRtable[((tw_FatherIndex&0x3F)<<5)&0xFE00], 512); //>>4<<9
			if(tc_IndexInOneDir <= gc_UserDataBuf[((tw_FatherIndex&0x0F)<<5)+0x1f]) //同一層目錄信息(Total Dir Num)
			{
				if(tc_IndexInOneDir == gc_UserDataBuf[((tw_FatherIndex&0x0F)<<5)+0x1f]) tb_HasNextDirFlag=0; //it's the last dir in its father dir
				else tb_HasNextDirFlag=1;

				if(tc_DirTableNo)
				{
					gs_DIR_FCB[0].dw_File_StartCluster	= gs_DIRtable[tc_DirTableNo-1].dw_File_StartCluster;
					gs_DIR_FCB[0].dw_FDB_StartCluster	= gs_DIRtable[tc_DirTableNo-1].dw_FDB_StartCluster;
					gs_DIR_FCB[0].dw_LongFDB_LogAdd0	= gs_DIRtable[tc_DirTableNo-1].dw_LongFDB_LogAdd0;
					gs_DIR_FCB[0].dw_FDB_Cluster		= gs_DIRtable[tc_DirTableNo-1].dw_FDB_Cluster;
					gs_DIR_FCB[0].dw_FDB_LogAdd			= gs_DIRtable[tc_DirTableNo-1].dw_FDB_LogAdd;
					gs_DIR_FCB[0].w_FDB_Offset			= gs_DIRtable[tc_DirTableNo-1].w_FDB_Offset;
				}
				else
				{
					USER_Read512BReserveBlock_DIR(tw_DirTabNum-1, tc_CurrentCard); //gc_UserDataBuf
					gs_DIR_FCB[0].dw_File_StartCluster	= *((U32 *)&gc_UserDataBuf[480+0x01]); 
					gs_DIR_FCB[0].dw_FDB_StartCluster	= *((U32 *)&gc_UserDataBuf[480+0x05]);
					gs_DIR_FCB[0].dw_LongFDB_LogAdd0	= *((U32 *)&gc_UserDataBuf[480+0x09]);
					gs_DIR_FCB[0].dw_FDB_Cluster		= *((U32 *)&gc_UserDataBuf[480+0x0d]);
					gs_DIR_FCB[0].dw_FDB_LogAdd			= *((U32 *)&gc_UserDataBuf[480+0x11]);
					gs_DIR_FCB[0].w_FDB_Offset			= *((U16 *)&gc_UserDataBuf[480+0x15]);
				}
				gw_DirIndex[0] = tw_DirIndex[0];
				gw_DirIndex[1] = tw_DirIndex[1];
				gs_DIR_FCB[0].dw_LongFDB_LogAdd1 = tdw_LongFDB_LogAdd1;
			}
			else 
			{
				tc_IndexInOneDir = 1;

				tw_OldFatherIndex = tw_FatherIndex;
				tw_FatherIndex++;
				if(tw_FatherIndex > tw_DirTotalNumber) 
				{
					return 1;
				}

				if((tw_FatherIndex&0x30)!=(tw_OldFatherIndex&0x30)) //not in the same sector
				{
					if((tw_FatherIndex&0xC0)!=(tw_DirTabNum&0xC0))	//need to read from reserved block
						USER_Read512BReserveBlock_DIR(tw_FatherIndex, tc_CurrentCard); //gc_UserDataBuf
					else
						memcpy(gc_UserDataBuf, &gc_DIRtable[((tw_FatherIndex&0x3F)<<5)&0xFE00], 512); //>>4<<9
				}

				while(gc_UserDataBuf[((tw_FatherIndex&0x0F)<<5)+0x1f]==0)
				{
					tw_OldFatherIndex = tw_FatherIndex; //Ching 081218
					tw_FatherIndex++;
					if(tw_FatherIndex > tw_DirTotalNumber) 
					{
						return 1;
					}

					if((tw_FatherIndex&0x30)!=(tw_OldFatherIndex&0x30)) //not in the same sector
					{
						if((tw_FatherIndex&0xC0)!=(tw_DirTabNum&0xC0))	//need to read from reserved block
							USER_Read512BReserveBlock_DIR(tw_FatherIndex, tc_CurrentCard); //gc_UserDataBuf
						else
							memcpy(gc_UserDataBuf, &gc_DIRtable[((tw_FatherIndex&0x3F)<<5)&0xFE00], 512); //>>4<<9
					}
				}
				if(tc_IndexInOneDir == gc_UserDataBuf[((tw_FatherIndex&0x0F)<<5)+0x1f]) tb_HasNextDirFlag=0; //it's the last dir in its father dir
				else tb_HasNextDirFlag=1;

				gw_DirIndex[0] = 0;//tw_DirIndex[0];
				gw_DirIndex[1] = 0;//tw_DirIndex[1];
				gs_DIR_FCB[0].dw_LongFDB_LogAdd1 = tdw_LongFDB_LogAdd1;
				gs_File_FCB[0].dw_FDB_StartCluster	= *((U32 *)&gc_UserDataBuf[((tw_FatherIndex&0x0F)<<5)+0x01]); //count which directory for file
				gs_DIR_FCB[0].dw_FDB_StartCluster	= *((U32 *)&gc_UserDataBuf[((tw_FatherIndex&0x0F)<<5)+0x01]); //from which cluster to search	
			}
		}//if(tc_SpanNumber)//當目錄存儲信息有跨2K Page時
		else
		{
			if( tc_IndexInOneDir <= gs_DIRtable[tw_FatherIndex].c_subdir_num)//同一層目錄信息(Total Dir Num)
			{
				if(tc_IndexInOneDir == gs_DIRtable[tw_FatherIndex].c_subdir_num)
					tb_HasNextDirFlag=0; //it's the last dir in its father dir
				else
					tb_HasNextDirFlag=1;

				gs_DIR_FCB[0].dw_File_StartCluster	= gs_DIRtable[tc_DirTableNo-1].dw_File_StartCluster;
				gs_DIR_FCB[0].dw_FDB_StartCluster	= gs_DIRtable[tc_DirTableNo-1].dw_FDB_StartCluster;
				gs_DIR_FCB[0].dw_LongFDB_LogAdd0	= gs_DIRtable[tc_DirTableNo-1].dw_LongFDB_LogAdd0;
				gs_DIR_FCB[0].dw_FDB_Cluster		= gs_DIRtable[tc_DirTableNo-1].dw_FDB_Cluster;
				gs_DIR_FCB[0].dw_FDB_LogAdd			= gs_DIRtable[tc_DirTableNo-1].dw_FDB_LogAdd;
				gs_DIR_FCB[0].w_FDB_Offset			= gs_DIRtable[tc_DirTableNo-1].w_FDB_Offset;

				gw_DirIndex[0] = tw_DirIndex[0];
				gw_DirIndex[1] = tw_DirIndex[1];
				gs_DIR_FCB[0].dw_LongFDB_LogAdd1 = tdw_LongFDB_LogAdd1;
			}
			else 
			{
				tc_IndexInOneDir = 1;
				tw_FatherIndex++;
				if(tw_FatherIndex > tw_DirTotalNumber) 
				{
					return 1;
				}

				while(gs_DIRtable[tw_FatherIndex].c_subdir_num==0)
				{
					tw_FatherIndex++;
					if(tw_FatherIndex > tw_DirTotalNumber) 
					{
						return 1;
					}
				}
				if(tc_IndexInOneDir == gs_DIRtable[tw_FatherIndex].c_subdir_num)
					tb_HasNextDirFlag=0; //it's the last dir in its father dir
				else
					tb_HasNextDirFlag=1;

				gw_DirIndex[0] = 0;
				gw_DirIndex[1] = 0;
				gs_DIR_FCB[0].dw_LongFDB_LogAdd1 = tdw_LongFDB_LogAdd1;

				gs_File_FCB[0].dw_FDB_StartCluster	= gs_DIRtable[tw_FatherIndex].dw_File_StartCluster; //count which directory for file
				gs_DIR_FCB[0].dw_FDB_StartCluster	= gs_DIRtable[tw_FatherIndex].dw_File_StartCluster; //from which cluster to search	
			}
		}

		if( DOS_Search_DIR(0x33) ) //search dir by order in one directory
		{
			return 1; //Read Error or no dir be found(not reasonable)
		}

		gs_DIRtable[tc_DirTableNo].c_occupied_flag		= 1;	//mark this 32Bytes is used to save the information of the found dir
		gs_DIRtable[tc_DirTableNo].dw_File_StartCluster	= gs_DIR_FCB[0].dw_File_StartCluster;
		gs_DIRtable[tc_DirTableNo].dw_FDB_StartCluster	= gs_DIR_FCB[0].dw_FDB_StartCluster;
		gs_DIRtable[tc_DirTableNo].dw_LongFDB_LogAdd0	= gs_DIR_FCB[0].dw_LongFDB_LogAdd0;
		gs_DIRtable[tc_DirTableNo].dw_FDB_Cluster		= gs_DIR_FCB[0].dw_FDB_Cluster;
		gs_DIRtable[tc_DirTableNo].dw_FDB_LogAdd		= gs_DIR_FCB[0].dw_FDB_LogAdd;
		gs_DIRtable[tc_DirTableNo].w_FDB_Offset			= gs_DIR_FCB[0].w_FDB_Offset;
       		
		tw_DirIndex[0]=gw_DirIndex[0];
		tw_DirIndex[1]=gw_DirIndex[1];
		tdw_LongFDB_LogAdd1=gs_DIR_FCB[0].dw_LongFDB_LogAdd1;
			
		gs_DIR_FCB[0].dw_FDB_StartCluster	= gs_DIRtable[tc_DirTableNo].dw_File_StartCluster; //count which directory for dir
		gs_File_FCB[0].dw_FDB_StartCluster	= gs_DIRtable[tc_DirTableNo].dw_File_StartCluster; //count which directory for file
		gb_FindFlag=0;
		DOS_Search_DIR(0x3b);			//count directory number in this directory, get gw_DirTotalNumber
		if(!gbt_DIRwFakeFDB) //Ching 090420
		{
			DOS_Search_File(0x10,0,0x10);	//count file number in this directory, get gw_FileTotalNumber[0]
			if(gw_DirTotalNumber>=0xff)	
			{
				gw_DirTotalNumber=0xff;
			}
			tw_TempTotalNum=gw_DirTotalNumber+gw_FileTotalNumber[0];
		}
		else
		{
			gbt_DIRwFakeFDB=0;
			gw_DirTotalNumber=0;
			gw_FileTotalNumber[0]=0;
			tw_TempTotalNum=0;
		}

		gs_DIRtable[tc_DirTableNo].w_file_folder_num = tw_TempTotalNum;	//save total number of dir and file in this directory
		gs_DIRtable[tc_DirTableNo].c_file_num_hi = ((U8 *)(&gw_FileTotalNumber[0]))[0];	//save file number High 8bit of this directory
		gs_DIRtable[tc_DirTableNo].c_file_num_lo = ((U8 *)(&gw_FileTotalNumber[0]))[1];	//save file number Low 8bit of this directory
		gs_DIRtable[tc_DirTableNo].c_subdir_num = ((U8 *)(&gw_DirTotalNumber))[1];		//save dir number of this directory
		gs_DIRtable[tc_DirTableNo].c_index_parent_folder = tw_FatherIndex;

		//child folder
		if(gw_DirTotalNumber>0)
		{
			if(tw_DirTotalNumber>=255)
				gs_DIRtable[tc_DirTableNo].c_index_child_folder = tw_DirTabNum; //OverSize, then set itself to child folder
			else
				gs_DIRtable[tc_DirTableNo].c_index_child_folder = tw_DirTotalNumber+1;
		}
		else
			gs_DIRtable[tc_DirTableNo].c_index_child_folder = tw_DirTabNum;

		//prev folder
		if(tc_IndexInOneDir==1) //1st dir in some dir, set itself to prev dir
			gs_DIRtable[tc_DirTableNo].c_index_prev_folder = tw_DirTabNum;
		else 
			gs_DIRtable[tc_DirTableNo].c_index_prev_folder = tw_DirTabNum-1;//prev folder

		//next folder
		if(tb_HasNextDirFlag)
			gs_DIRtable[tc_DirTableNo].c_index_next_folder = tw_DirTabNum+1;
		else
			gs_DIRtable[tc_DirTableNo].c_index_next_folder = tw_DirTabNum;

		tc_IndexInOneDir++;	
		tw_DirTotalNumber += gw_DirTotalNumber;

		if((tw_DirTabNum&0x3F)==0x3F)
		{//處理超過64個目錄
		USER_WriteReserveBlock_DIR(tc_SpanNumber, tc_CurrentCard);
			memset(gc_DIRtable,0,2048);	//clear buffer gc_DIRtable
			tc_SpanNumber++;
		}
	}//for( tw_DirTabNum=1; tw_DirTabNum<256; tw_DirTabNum++ )//擴展到256個Dir

//	dbprintf("All Dir--%x\n",tw_DirTotalNumber);

	if(!tc_CurrentCard)
	{
	gc_DirReBuildFlag=0x55; //Mark DirTable is Valid
	USER_MarkTable(gc_DirReBuildFlag);
	}

	gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;

	return 0;
}


U8 DOS_Search_DIR(U8 tc_Mode)
{
    U8 tc_sts,tc_IniFlag,tc_FindDir,tc_FindFile ;//whether search file or search dir flag
    U8 tc_SearchTime ;
	U8 tc_Type=0;
	U8 tc_PrevOrNext=0;
	U32 tdw_SectorAdd,tdw_CurrentDirCluster,tdw_PrevDirCluster,tdw_NextDirCluster;
    SearchFdb ptr ;

	gs_File_FCB[0].dw_File_StartCluster=0xffffffff ;
    gs_File_FCB[0].dw_FDB_LogAdd=0 ;
    tc_sts=1 ;
    tc_IniFlag=0 ;	
    ptr.w_DirTotalNum=0 ;
    gb_ReadWriteDataArea=1 ;

    ptr.pc_LongFileName=gc_FileLongName ;
    ptr.c_Search_Mode=tc_Mode&0x0f; //0x03 or 0x0B
    ptr.c_EXTSelect=0;			//the directory doesn't need to compare ExtName, default value
    ptr.c_Search_Direction=0 ;	//defalut: search next
	ptr.c_Search_Attribute=1 ;	//default: find dir
    ptr.c_type=0 ; 				//default
	ptr.Compare.dw_BubbleFlag=gdw_CurrFlag ;
	ptr.dw_File_StartCluster=gdw_StartCluster2 ;
    tc_SearchTime=2 ;
    while(tc_SearchTime)
    {
        {//find dir in one dir
            tc_FindFile=0 ;
            tc_FindDir=1 ;
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
                tc_sts=Count_Dir_Fdb(&ptr);
            else
			{
                tc_sts=Find_Fdb(&ptr);
	            if(DOS_Read_LogicSector(ptr.dw_FDB_LogAdd,1)) //read fail
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
												
				if(tc_FindDir==1 && ptr.c_Search_Mode==0x0b)
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
							gs_DIR_FCB[tc_Type].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
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

			gc_test=1;
            gs_File_FCB[0].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
            gs_DIR_FCB[0].dw_FDB_StartCluster=ptr.dw_FDB_StartCluster;
            tc_SearchTime--;
        }
    }
    return(1);
}


void DirManagement_Init()
{
	gc_DispFilePageBak=0xFFFF;
	gc_DispFilePage=0;
	UI_DIRStoreOrRecover(1);//save
	gc_FatherIndex = 0; 	//root
	gw_DIRtableIndex = 1;	//root's 1st child dir
	gw_Index = 1;			//1st dir
	USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex, gc_CurrentCard);	//gc_DIRtable[]

	Get_TotalNumber(gc_FatherIndex);
	FatherDir_FDBInit(gc_FatherIndex);

	gc_PhaseInx = 0;

	if(gw_TotalNumber)
	{
		UI_DispGet_ChildName();
		UI_Disp_ChildName();
	}
	else
	{
		gw_Index = 0;
		UI_Disp_ChildName();
		gc_DirNoFile_Flag=1;
	}
}


U8 Count_Dir_Fdb(SearchFdb *p_mp)
{
	bit tbt_Check1stFDB=0;
    U8  tc_SectorOffset ;
    U16 tw_FdbOffset ;
    U32 tdw_SectorAdd,tdw_S_ClusterNum ;
    U32 tdw_Find_ClusterSize,tdw_TheEndCluster ;
    
    p_mp->w_DirTotalNum=0 ;
    tdw_S_ClusterNum=p_mp->dw_FDB_StartCluster ;
    
    if(gc_DOS_FileSystemType==1)tdw_TheEndCluster=0xffff; 		//FAT16
    else 						tdw_TheEndCluster=0x0fffffff; 	//FAT32
    
	if(p_mp->dw_FDB_StartCluster != gdw_DOS_RootDirClus) tbt_Check1stFDB=1; //Ching 090420
    
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
            if(DOS_Read_LogicSector((tdw_SectorAdd+tc_SectorOffset),1)) //read fail
				return 0xff ;

			//Ching 090420
			if(tbt_Check1stFDB) //check . and ..
			{
				tbt_Check1stFDB = 0;
				if((gc_UserDataBuf[0x00]!=0x2E) || (gc_UserDataBuf[0x0B]!=0x10) || (gc_UserDataBuf[0x20]!=0x2E) || (gc_UserDataBuf[0x21]!=0x2E) || (gc_UserDataBuf[0x2B]!=0x10))
				{
					gbt_DIRwFakeFDB = 1;
					return 2;
				}
			}

            for(tw_FdbOffset=0;tw_FdbOffset<K_DOS_SectorSize;tw_FdbOffset+=32)
            {//find in one sector				
                if(gc_UserDataBuf[tw_FdbOffset]==0xE5)//deleted FDB
                {}
                else if(gc_UserDataBuf[tw_FdbOffset]==0x00)//end flag
				{
                    return(SUCCESS);
				}
                else if(gc_UserDataBuf[tw_FdbOffset+0x0B]==0x0F)//long FDB
                {}
                else if(gc_UserDataBuf[tw_FdbOffset+0x0B]&0x10)//short FDB and is DIR
                {
                 	if(gc_UserDataBuf[tw_FdbOffset]!=0x2E) //it's real folder
						p_mp->w_DirTotalNum++;
                }
            }//for(tw_FdbOffset=0;tw_FdbOffset<K_DOS_SectorSize;tw_FdbOffset+=32)------------>find in one sector
        }//for(tc_SectorOffset=0;tc_SectorOffset<tdw_Find_ClusterSize;tc_SectorOffset++)----->find in one cluster
        tdw_S_ClusterNum=DOS_GetNextCluster(tdw_S_ClusterNum,1);
    }//while(tdw_S_ClusterNum<tdw_TheEndCluster)--------------------------------------------->find in one dir

	return(SUCCESS);
}



void USER_MarkTable(U8 tc_MarkFlag)
{
	gc_PlayRecordDataBuf[400] = tc_MarkFlag; //0xAA: Invalid, 0x55: Valid
	if(tc_MarkFlag==D_Expired)//disable track memory //yflin081007
	{
		gc_PlayRecordDataBuf[401]=D_Expired;
		gc_PlayRecordDataBuf[402]=D_Expired;
	}
}


void USER_GetUISetIndex(void)
{
	U16 tw_SectorIndex;
	U8 tc_SetIndex;
	U8 tc_i, tc_SearchNum, tc_j, tc_loop=2;

	tc_SearchNum = gc_SetNumber*2;
	tc_SetIndex = tc_SearchNum-1;

	//0:NandFlash - [2][3]; 1:SD - [4][5], Ching 090306
	for(tc_j=0; tc_j<tc_loop; tc_j++)
	{
		for(tc_i=0; tc_i<tc_SearchNum; tc_i++)
		{
			tw_SectorIndex = ((tc_SetIndex & (gc_SetNumber-1)) <<2) *gc_InAddrMax;
	
			if(tc_SetIndex & gc_SetNumber)	//read UserDataBlock[3]
				UserData_Block_Read(3+tc_j*2,tw_SectorIndex,1,gc_DIRtableHB);
			else							//read UserDataBlock[2]
				UserData_Block_Read(2+tc_j*2,tw_SectorIndex,1,gc_DIRtableHB);
	
			if(gc_UserDataBuf[0]!=0xff)
				break;
		
			tc_SetIndex --;
		}
		if(!tc_j) //NandFlash
		{
			if(tc_i==tc_SearchNum)	gc_SetIndex = 0;
			else					gc_SetIndex = tc_SetIndex;
		}
		else //SD
		{
			if(tc_i==tc_SearchNum)	gc_SDSetIndex = 0;
			else					gc_SDSetIndex = tc_SetIndex;
		}
	}
}



void UI_DispGetDirName(U8 tc_DIRtableIndex)
{
	U32 tdw_bk_LongFDB_LogAdd0, tdw_FDB_LogAdd;
	U16 tw_FDB_Offset;

	if(gs_System_State.c_FileHandle == 2) //back up, Ching 090311
	{
		tdw_bk_LongFDB_LogAdd0 = gs_File_FCB[2].dw_LongFDB_LogAdd0;
		tdw_FDB_LogAdd = gs_File_FCB[2].dw_FDB_LogAdd;
		tw_FDB_Offset = gs_File_FCB[2].w_FDB_Offset;
	}

	gs_File_FCB[2].dw_LongFDB_LogAdd0	= gs_DIRtable[tc_DIRtableIndex].dw_LongFDB_LogAdd0;
	gs_File_FCB[2].dw_FDB_LogAdd		= gs_DIRtable[tc_DIRtableIndex].dw_FDB_LogAdd;
	gs_File_FCB[2].w_FDB_Offset			= gs_DIRtable[tc_DIRtableIndex].w_FDB_Offset;
    DOS_GetLongFileName(2,gc_FileLongName);

	if(gs_System_State.c_FileHandle == 2) //re-cover, Ching 090311
	{
		gs_File_FCB[2].dw_LongFDB_LogAdd0 = tdw_bk_LongFDB_LogAdd0;
		gs_File_FCB[2].dw_FDB_LogAdd = tdw_FDB_LogAdd;
		gs_File_FCB[2].w_FDB_Offset = tw_FDB_Offset;
	}
}


void FatherDir_FDBInit(U16 tw_FatherIndex)
{
	tw_FatherIndex &= 0x3F;  //20090407 add
	gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster = gs_DIRtable[tw_FatherIndex].dw_File_StartCluster;
	gb_FindFlag = 0;
	gw_FileIndex[gs_System_State.c_FileHandle] = 0;
}


void Get_TotalNumber(U16 tw_DirIndex)
{
	tw_DirIndex &= 0x3F;
	gw_DirTotalNumber					= gs_DIRtable[tw_DirIndex].c_subdir_num;
	if(gs_System_State.c_FileHandle == 2) //Ching 090311
	{
		DOS_Search_File(C_File_OneDir|C_Cnt_FileNo,C_OtherFileType,C_CmpExtName|C_Next);
		gw_TotalNumber						= gw_DirTotalNumber + gw_FileTotalNumber[2];
		gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster = gs_DIRtable[tw_DirIndex].dw_File_StartCluster;
		gw_DirIndex[0] = 0;
		gw_DirIndex[1] = 0;
	}
	else
	{
		gw_TotalNumber						= gs_DIRtable[tw_DirIndex].w_file_folder_num;
		((U8 *)(&gw_FileTotalNumber[0]))[0] = gs_DIRtable[tw_DirIndex].c_file_num_hi;
		((U8 *)(&gw_FileTotalNumber[0]))[1] = gs_DIRtable[tw_DirIndex].c_file_num_lo;
	}
}


void UI_Disp_ChildName()//tc_mode for color project
{
#ifdef TFT_18V
    struct string_view_t disp_param = { 20, 25, 108, 20 };
#else
	struct string_view_t disp_param = { 20, 25, 136, 20 };
	
#endif
	struct string_disp_t string_attr;
	U16 tw_TempIndex;
	U8 	tc_i, tc_ReversePage;

	tc_ReversePage =gw_Index % C_LCM_ROW_MAX;
	gc_DispFilePage=gw_Index / C_LCM_ROW_MAX;

	if(gc_DispFilePageBak!=gc_DispFilePage)
	{
		//TFT_FileSelect();
		Menu_Disp_Item_Other(60,0,REFRESH_ALL);//sunzhk add
	}

	for(tc_i=0; tc_i<C_LCM_ROW_MAX; tc_i++)
	{
		tw_TempIndex = (U16)C_BUF_LENGTH*tc_i;
		gc_IconY=tc_i;

		if(gc_DispFilePageBak!=gc_DispFilePage)
		{
			if(gc_DirLCMtempBuf[tw_TempIndex+4])
			{
				gc_IconType=gc_DirLCMtempBuf[tw_TempIndex+1];
				TFT_FileIcon();
			}
		}

		UI_get_string_hzk_bitmap(&disp_param, //*
								 &(string_attr),
								 &(gc_DirLCMtempBuf[tw_TempIndex+5]), //* string to display
								 gc_DirLCMtempBuf[tw_TempIndex+4], //* string size in bytes
								 gc_DirLCMtempBuf[tw_TempIndex+2], //* unicode or ansi flag
								 tc_i, //* line number
								 0/*truncate on char or not*/, //* 1
								 0, // rolling factor
								 &(gc_DirLCMtempBuf[512]));  //luke090311 fix  bug

		if(tc_i==tc_ReversePage)
		{
			string_attr.c_reverse_flag = 1;
		}
		UI_render_strings(&disp_param, &string_attr);
	}
	gc_DispFilePageBak=gc_DispFilePage;
}


void OtherManagement_Init() //Ching 090311
{
//dbprintf("OtherManagement_Init\n");
	gc_FatherIndex = 0; 	//root
	gw_DIRtableIndex = 1;	//root's 1st child dir
	gw_Index = 1;			//1st dir
	USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex, gc_CurrentCard);	//gc_DIRtable[]
	FatherDir_FDBInit(gc_FatherIndex);
	Get_TotalNumber(gc_FatherIndex);
	gc_PhaseInx = 0;

	if(gw_TotalNumber)
	{
		UI_DispGet_ChildName();
		UI_Disp_ChildName();
	}
	else
	{
		gw_Index = 0;
		UI_Disp_ChildName();
	}
}




void UI_DIRStoreOrRecover(bit tb_SaveOrRecover)
{
	if(tb_SaveOrRecover) //save
	{ 
		Save_gw_MusicFileTotalNum 	= gw_FileTotalNumber[0]; 
		Save_dw_FDB_StartCluster[gs_System_State.c_FileHandle]		= gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster;
		Save_dw_FDB_Cluster[gs_System_State.c_FileHandle]	   		= gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_Cluster;	
		Save_dw_FDB_LogAdd[gs_System_State.c_FileHandle]	   		= gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_LogAdd;
		Save_dw_LongFDB_LogAdd0[gs_System_State.c_FileHandle] 		= gs_File_FCB[gs_System_State.c_FileHandle].dw_LongFDB_LogAdd0;
		Save_dw_LongFDB_LogAdd1[gs_System_State.c_FileHandle] 		= gs_File_FCB[gs_System_State.c_FileHandle].dw_LongFDB_LogAdd1;
		Save_w_FDB_Offset[gs_System_State.c_FileHandle]	   			= gs_File_FCB[gs_System_State.c_FileHandle].w_FDB_Offset;
		Save_gw_FileIndex[gs_System_State.c_FileHandle]	   			= gw_FileIndex[gs_System_State.c_FileHandle];
		Save_c_FileType[gs_System_State.c_FileHandle]		   		= gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;
		Save_dw_File_StartCluster[gs_System_State.c_FileHandle]		= gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;//lijian.peng 081011 for return to music
		Save_gw_FileDirIndex[gs_System_State.c_FileHandle]=gw_FileTotalNumber[gs_System_State.c_FileHandle];//yflin081007

		if(gs_System_State.c_FileHandle==0)
		{
			gw_DirIndexTest[0]=gw_DirIndex[0];//for PLAY yflin081015
			gw_DirIndexTest[1]=gw_DirIndex[1];
		}
	}
	else  //recover
	{
		gw_FileTotalNumber[0] = Save_gw_MusicFileTotalNum;
		gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster 	= Save_dw_FDB_StartCluster[gs_System_State.c_FileHandle];
		gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_Cluster	   	= Save_dw_FDB_Cluster[gs_System_State.c_FileHandle];	
		gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_LogAdd	   		= Save_dw_FDB_LogAdd[gs_System_State.c_FileHandle];
		gs_File_FCB[gs_System_State.c_FileHandle].dw_LongFDB_LogAdd0  	= Save_dw_LongFDB_LogAdd0[gs_System_State.c_FileHandle];
		gs_File_FCB[gs_System_State.c_FileHandle].dw_LongFDB_LogAdd1  	= Save_dw_LongFDB_LogAdd1[gs_System_State.c_FileHandle];
		gs_File_FCB[gs_System_State.c_FileHandle].w_FDB_Offset 	   		= Save_w_FDB_Offset[gs_System_State.c_FileHandle];
		gw_FileIndex[gs_System_State.c_FileHandle] 				   		= Save_gw_FileIndex[gs_System_State.c_FileHandle];
		gs_File_FCB[gs_System_State.c_FileHandle].c_FileType		   	= Save_c_FileType[gs_System_State.c_FileHandle];
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster	= Save_dw_File_StartCluster[gs_System_State.c_FileHandle];//lijian.peng 081011 for return to music
        gw_FileTotalNumber[gs_System_State.c_FileHandle]=Save_gw_FileDirIndex[gs_System_State.c_FileHandle];//yflin081007
			
		if(gs_System_State.c_FileHandle==0)	
		{
			gw_DirIndex[0]=gw_DirIndexTest[0];//for PLAY yflin081015
			gw_DirIndex[1]=gw_DirIndexTest[1];
		}
		if(gw_FileTotalNumber[gs_System_State.c_FileHandle])DOS_GetLongFileName(gs_System_State.c_FileHandle,gc_FileLongName);//yflin081023
	}
}


void DirManagement_Process() //adjust code to match both music and voice case, Ching 080926
{
	UBYTE tc_MenuItem, tc_LCMRowMinus1, tc_i;
	WORD  tw_BottomIndex, tw_Index, tw_DIRtableIndex;

	if(gc_PhaseInx)
	{
		switch(gc_PhaseInx)
		{
			case C_PlayMenu:
				gc_DispFilePageBak=0xFFFF;
				if(gw_Index)	//enter child dir or confirm selected item
				{
					tc_MenuItem = 3; 
				}
				else			//back to upper level
				{
					if(gc_FatherIndex)	
					{
						tc_MenuItem = 2;
					}
					else
					{
						return; //root dir
					}
				}
            break;

            case C_PlayNext:
				tc_MenuItem = 0;
            break;

            case C_PlayPrev:
				tc_MenuItem = 1;
            break;

			default:		// other-key , no action lijian.peng 081024
				gc_PhaseInx=0;
				return ;
			break;
        }

		tc_LCMRowMinus1 = C_LCM_ROW_MAX-1;
		
		if(tc_MenuItem==0) //Next
		{
			if(gw_TotalNumber)
			{
				if(gw_Index==gw_TotalNumber)//last->1st page
				{
					gw_Index=0;
					//Ching 081218
					if(gw_DirTotalNumber)	
					{
						tw_DIRtableIndex = gw_DIRtableIndex;
						gw_DIRtableIndex -= (gw_DirTotalNumber-1);
						if((gw_DIRtableIndex&0xc0) != (tw_DIRtableIndex&0xc0))
							USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex, gc_CurrentCard);
					}
					UI_DispGet_ChildName();
				}
				else
				{
					gw_Index ++;
					if(gw_Index<=gw_DirTotalNumber) 
					{
						tw_DIRtableIndex = gw_DIRtableIndex; //Ching 081218
						if(gw_Index!=1) //0->1
						gw_DIRtableIndex++;
					}

					if(!(gw_Index%C_LCM_ROW_MAX)) //Next page
					{
						if(gw_DirTotalNumber) //Ching 081218
						{
							if((gw_DIRtableIndex&0xc0) != (tw_DIRtableIndex&0xc0))
								USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex, gc_CurrentCard);
						}
						UI_DispGet_ChildName();
					}
				}
			}
		}
		else if(tc_MenuItem==1) //Prev
		{
			if(gw_TotalNumber)
			{
				gc_PhaseInx = C_DirPrev;
				gb_TriggerFileSkip = 1;
				if(gw_Index==0)//1st->last page
				{
					gw_Index=gw_TotalNumber;
					if(gw_DirTotalNumber)
					{
						tw_DIRtableIndex = gw_DIRtableIndex;
						gw_DIRtableIndex += (gw_DirTotalNumber-1);
						if((gw_DIRtableIndex&0xc0) != (tw_DIRtableIndex&0xc0))
							USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex, gc_CurrentCard);
					}
					if(gw_FileTotalNumber[gs_System_State.c_FileHandle])
					{
						tw_Index = gw_Index - (gw_Index%C_LCM_ROW_MAX); 
						if(tw_Index > gw_DirTotalNumber)	gw_FileSkipNumber = tw_Index - gw_DirTotalNumber;
						else								gw_FileSkipNumber = 1;
						gb_FindFlag = 0;
						gw_FileIndex[gs_System_State.c_FileHandle] = 0;
					}
					UI_DispGet_ChildName();
				}
				else
				{
					gw_Index --;
					if((gw_Index<gw_DirTotalNumber) && gw_Index)
					{
						tw_DIRtableIndex = gw_DIRtableIndex;
						gw_DIRtableIndex--;
					}
					if((gw_Index%C_LCM_ROW_MAX)==tc_LCMRowMinus1) //Prev page
					{
						if(gw_Index < C_LCM_ROW_MAX) //2nd->1st page
							gw_FileSkipNumber = 0;
						else
						{
							tw_Index = gw_Index - (gw_Index%C_LCM_ROW_MAX); 
							if((gw_Index-tc_LCMRowMinus1)>gw_DirTotalNumber) //it has some file in this page
								gw_FileSkipNumber = tw_Index - gw_DirTotalNumber;
						}

						gb_FindFlag = 0;
						gw_FileIndex[gs_System_State.c_FileHandle] = 0;

						if((gw_DirTotalNumber) && gw_Index)
						{
							if((gw_DIRtableIndex&0xc0) != (tw_DIRtableIndex&0xc0))
								USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex, gc_CurrentCard);
						}
						UI_DispGet_ChildName();
					}
				}
				gb_TriggerFileSkip = 0;
			}
		} 
		else if(tc_MenuItem==2) //back to upper level
		{
			//if(gc_FatherIndex!=0) //not Root
			{
				//Check if gc_DIRtable[] is in which page we need
				tw_Index = gw_Index + (tc_LCMRowMinus1 - (gw_Index%C_LCM_ROW_MAX)); //last item in the page
				if(tw_Index>gw_DirTotalNumber)	tw_BottomIndex = gw_DIRtableIndex + (gw_DirTotalNumber - gw_Index);
				else							tw_BottomIndex = gw_DIRtableIndex + (tw_Index - gw_Index);

				if((tw_BottomIndex&0xc0) != (gc_FatherIndex&0xc0))
				{
					USER_Read2KBReserveBlock_DIR(gc_FatherIndex, gc_CurrentCard);	//gc_DIRtable[]
				}
				gw_DIRtableIndex = gc_FatherIndex;
				gc_FatherIndex = gs_DIRtable[gw_DIRtableIndex&0x3F].c_index_parent_folder;  //20090407 chiayen modify

				if((gw_DIRtableIndex&0xc0) != (gc_FatherIndex&0xc0))
				{
					USER_Read2KBReserveBlock_DIR(gc_FatherIndex, gc_CurrentCard);	//gc_DIRtable[]
					tw_Index = gs_DIRtable[gc_FatherIndex&0x3F].c_index_child_folder;  //20090407 chiayen modify
					FatherDir_FDBInit(gc_FatherIndex);
					Get_TotalNumber(gc_FatherIndex);
					USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex, gc_CurrentCard);	//gc_DIRtable[]
				}
				else
				{
					tw_Index = gs_DIRtable[gc_FatherIndex].c_index_child_folder;
					FatherDir_FDBInit(gc_FatherIndex);
					Get_TotalNumber(gc_FatherIndex);
				}
				gw_Index = gw_DIRtableIndex - tw_Index + 1;

				UI_DispGet_ChildName();
			}
		}
		else if(tc_MenuItem==3) //enter child dir or confirm selected item
		{
			if(gw_Index>gw_DirTotalNumber) //file
			{
				if(gw_FileTotalNumber[gs_System_State.c_FileHandle]) //Ching 081106
				{
					if((gs_System_State.c_FileHandle==0) || (gs_System_State.c_FileHandle==2)) //music or jpg/mjpg/txt, Ching 090311
					{
						gw_FileSkipNumber = gw_Index-gw_DirTotalNumber;
						gb_FindFlag = 0;
						gw_FileIndex[gs_System_State.c_FileHandle] = 0;
						gb_TriggerFileSkip = 1;
						DOS_Search_File(C_File_OneDir|C_By_FDB, gs_System_State.c_FileHandle, C_CmpExtName|C_Next);
						gb_TriggerFileSkip = 0;
					}
	
					DOS_GetLongFileName(gs_System_State.c_FileHandle,gc_FileLongName);
					gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;//lijian.peng modify 081016 for play error from pause to DIR

					if(gs_System_State.c_FileHandle!=2)
					{
						if(!gs_System_State.c_FileHandle) //music
						{		
							gc_Task_Next=C_Task_Play;
							gc_PhaseInx = 0;								
						}
						gw_init_needed |= SET_BIT15;//(JC)H1007
	
						return;
					}
					else
					{
						if(gc_TaskMode_BkUp == C_Task_Jpeg)
						{
							gc_Task_Next=C_Task_Jpeg;			
							//gw_init_needed |= SET_BIT4;
							gc_PhaseInx=1;
						}
						else if(gc_TaskMode_BkUp == C_Task_Mjpeg)
						{
							gc_Task_Next=C_Task_Mjpeg;			
							gw_init_needed |= SET_BIT5;
							gc_PhaseInx=1;
						}
						return;
					}
				}
			}
			else //dir
			{
				tw_Index = (((gw_Index-1)/tc_LCMRowMinus1)+1)*tc_LCMRowMinus1;
				if(tw_Index>gw_DirTotalNumber)
				{
					tw_BottomIndex = gw_DIRtableIndex + (gw_DirTotalNumber - gw_Index);
				}
				else
				{
					tw_BottomIndex = gw_DIRtableIndex + (tw_Index - gw_Index);
				}

				if((tw_BottomIndex&0xc0) != (gw_DIRtableIndex&0xc0))
				{
					USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex, gc_CurrentCard);	//gc_DIRtable[]
				}

				gc_FatherIndex = gw_DIRtableIndex;
				FatherDir_FDBInit(gc_FatherIndex); //Ching 090311
				Get_TotalNumber(gc_FatherIndex);
				tc_i = (gw_Index-1)%tc_LCMRowMinus1 + 1;
				memcpy(&gc_DirLCMtempBuf[0], &gc_DirLCMtempBuf[C_BUF_LENGTH*tc_i], C_BUF_LENGTH);
//				dbprintf("gw_TotalNumber=%x \n",gw_TotalNumber);
				if(gw_TotalNumber)
				{
					gw_DIRtableIndex = gs_DIRtable[gc_FatherIndex&0x3F].c_index_child_folder; //1st child  //20090407 chiayen modify
					if((gc_FatherIndex&0xc0) != (gw_DIRtableIndex&0xc0))
					{
						USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex, gc_CurrentCard);	//gc_DIRtable[]
					}
					gw_Index = 1;

					UI_DispGet_ChildName();
				}
				else
				{
					gw_Index = 0;
					UI_DispGet_ChildName();
				}
			}
		}

		if((gc_PhaseInx != 0) && (gc_DirNoFile_Flag==0))
		{
			UI_Disp_ChildName(); //5sec
		} 
	}
	else
	{
	}
}


void Play_SDFlash_DosInit_Dir()
{
	gb_FindFlag = 0;
	gc_PhaseInx=0;
	gw_init_needed=0xFFFF;
	if (DOS_Initialize())
	{
	}
	else
	{
		gc_ShowTimer=0;
		gc_DispWallpaper=1;
		gc_TuneVolFreqStatus=0;
		gb_FindFlag = 0;
		gc_PhaseInx=0;
		gw_init_needed=0xFFFF;

		gc_TaskMode_BkUp=C_Task_Play;//(JC)I0304 for back to rite TASK from Dir-list
		gc_Task_Next=C_Task_Play;
		gb_SourceChange_Dir=1;

		gs_System_State.c_FileHandle=0;
		gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;           
		DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);//(JC)count music file no. in root
		gc_PhaseInx=0;
		gb_TFT_refresh=1;
		gc_DispWallpaper=0xFF;
	}
}


void UI_DispGet_ChildName(void)
{
	U8 	tc_i, tc_TableOffset;
	U16 tw_Index, tw_TempIndex, tw_CurrentTableIndex, tw_Length;

	tw_Index = gw_Index - (gw_Index%C_LCM_ROW_MAX); //initialize @ Line0
	if((tw_Index<=gw_DirTotalNumber) && (gw_DirTotalNumber))
	{
		if(tw_Index!=0)
	{
		if((gw_Index-tw_Index) > (gw_DirTotalNumber-tw_Index))
		{
			tw_CurrentTableIndex = gw_DIRtableIndex - (gw_DirTotalNumber-tw_Index);
		}
		else
		{
			tw_CurrentTableIndex = gw_DIRtableIndex - (gw_Index-tw_Index);
		}
		}
		else //Ching 090304
		{
			if(!gw_Index)
			{
				tw_CurrentTableIndex = gw_DIRtableIndex;
			}
			else if(gw_Index > gw_DirTotalNumber)
			{
				tw_CurrentTableIndex = gw_DIRtableIndex - (gw_DirTotalNumber-1);
			}
			else
			{
				tw_CurrentTableIndex = gw_DIRtableIndex - (gw_Index-1);
			}
		}

		if((tw_CurrentTableIndex&0xc0) != (gw_DIRtableIndex&0xc0)) //different pages
		{
			USER_Read2KBReserveBlock_DIR(tw_CurrentTableIndex, gc_CurrentCard);	//gc_DIRtable[]
		}
	}

	for(tc_i=0; tc_i<C_LCM_ROW_MAX; tc_i++)
	{
		tw_TempIndex = (U16)C_BUF_LENGTH*tc_i;

		if(!tw_Index) //Father DIR
		{
			gc_DirLCMtempBuf[0] = 0;
			gc_DirLCMtempBuf[1] = D_SmallIcon_Dir;	//dir
			gc_DirLCMtempBuf[2] = 1;
			gc_DirLCMtempBuf[3] = 0;
			if(!gc_FatherIndex && (gs_System_State.c_FileHandle!=1)) //not DVR(DVR's FatherIndex =0)
			{
				gc_DirLCMtempBuf[4] = 4;
				gc_DirLCMtempBuf[5] = 'R';
				gc_DirLCMtempBuf[6] = 'O';
				gc_DirLCMtempBuf[7] = 'O';
				gc_DirLCMtempBuf[8] = 'T';
			}
			else
			{
				gc_DirLCMtempBuf[4] = 1;
				gc_DirLCMtempBuf[5] = 0x5C; // '\'
			}
		}
		else if(tw_Index <= gw_TotalNumber)
		{
			if(tw_Index <= gw_DirTotalNumber) //Dir
			{
				tc_TableOffset = (U8)tw_CurrentTableIndex & 0x3F;
					UI_DispGetDirName(tc_TableOffset);
	
				memcpy(&tw_Length,&gc_FileLongName[3],2);

				if(tw_Length>C_BUF_STRING) 
				{
					memcpy(&gc_DirLCMtempBuf[tw_TempIndex], &gc_FileLongName[0],C_BUF_STRING+5);
					gc_DirLCMtempBuf[tw_TempIndex+1] = D_SmallIcon_Dir;	//dir
					gc_DirLCMtempBuf[tw_TempIndex+3] = 0;	//high byte of length
					gc_DirLCMtempBuf[tw_TempIndex+4] = C_BUF_STRING;//low byte of length
				}
				else
				{
					tc_TableOffset = (U8)(tw_Length+5);
					memcpy(&gc_DirLCMtempBuf[tw_TempIndex], &gc_FileLongName[0],tc_TableOffset);
					gc_DirLCMtempBuf[tw_TempIndex+1] = D_SmallIcon_Dir;	//dir
				}
	
				if((tw_Index < gw_DirTotalNumber) && (tc_i!=(C_LCM_ROW_MAX-1))) //it has the next dir, Ching 081218
				{
					if(((tw_CurrentTableIndex+1)&0xc0) != (tw_CurrentTableIndex&0xc0)) //next dir and current dir are in different pages
					{
						USER_Read2KBReserveBlock_DIR(tw_CurrentTableIndex+1, gc_CurrentCard);	//gc_DIRtable[]
					}
					tw_CurrentTableIndex++;
				}
			}
			else //File
			{//adjust code to match both music and voice case, Ching 080926
				//Ching 081105
				if(gs_System_State.c_FileHandle!=1)
				{
					DOS_Search_File(C_File_OneDir|C_By_FDB, gs_System_State.c_FileHandle, C_CmpExtName|C_Next);
				}

				DOS_GetLongFileName(gs_System_State.c_FileHandle,gc_FileLongName);
				tw_Length = *((U16 *)&gc_FileLongName[3]);

				if(tw_Length>C_BUF_STRING) 
				{
					memcpy(&gc_DirLCMtempBuf[tw_TempIndex], &gc_FileLongName[0],C_BUF_STRING+5);
					if(gs_System_State.c_FileHandle < 2) //Ching 090311
						gc_DirLCMtempBuf[tw_TempIndex+1] = gs_System_State.c_FileHandle;	//file, Ching 090304
					else
					{
						if(gc_TaskMode_BkUp == C_Task_Jpeg)			
						{
							gc_DirLCMtempBuf[tw_TempIndex+1] = D_SmallIcon_Photo;
						}
						else if(gc_TaskMode_BkUp == C_Task_Mjpeg)	
						{
							gc_DirLCMtempBuf[tw_TempIndex+1] = D_SmallIcon_Movie;
						}
						else if(gc_TaskMode_BkUp == C_Task_TXT)		
						{
							gc_DirLCMtempBuf[tw_TempIndex+1] = D_SmallIcon_EBook;
						}
					}
					gc_DirLCMtempBuf[tw_TempIndex+3] = 0;	//high byte of length
					gc_DirLCMtempBuf[tw_TempIndex+4] = C_BUF_STRING;//low byte of length
				}
				else
				{
					tc_TableOffset = (U8)(tw_Length+5);
					memcpy(&gc_DirLCMtempBuf[tw_TempIndex], &gc_FileLongName[0],tc_TableOffset);
					if(gs_System_State.c_FileHandle < 2) //Ching 090311
					{
						gc_DirLCMtempBuf[tw_TempIndex+1] = gs_System_State.c_FileHandle;	//file, Ching 090304
					}
					else
					{
						if(gc_TaskMode_BkUp == C_Task_Jpeg)			
						{
							gc_DirLCMtempBuf[tw_TempIndex+1] = D_SmallIcon_Photo;
						}
						else if(gc_TaskMode_BkUp == C_Task_Mjpeg)	
						{
							gc_DirLCMtempBuf[tw_TempIndex+1] = D_SmallIcon_Movie;
						}
						else if(gc_TaskMode_BkUp == C_Task_TXT)		
						{
							gc_DirLCMtempBuf[tw_TempIndex+1] = D_SmallIcon_EBook;
						}
					}
				}
			}
		}
		else
		{
			gc_DirLCMtempBuf[tw_TempIndex+4] = 0x00;//low byte of length
		}
		tw_Index ++;
	}
}

void Play_SourceDetect_Process_Dir()
{
	gb_SD_Exist_pre=gb_SD_Exist;
	if(!SD_Detect)  //SD exist
    {
        gb_SD_Exist=1;
    }
	else
	{
		gb_SD_Exist=0;
		gb_SDNoFileflag=0;
	}

	gb_Host_Exist_pre=gb_Host_Exist;
	if(!Host_DetectDevice())
	{
		gb_Host_Exist=1;	//host_exist		
	}
	else
	{
		gb_Host_Exist=0;
		gc_HostNoFileflag=0;
	}

	if((gc_CurrentCard==0) && ((gb_Host_Exist_pre!=gb_Host_Exist)||(gb_SD_Exist_pre!=gb_SD_Exist)))
	{
		if((gb_Host_Exist_pre!=gb_Host_Exist) && (gb_Host_Exist==1))
		{
			gc_CurrentCard=5;
		}
		else if((gb_SD_Exist_pre!=gb_SD_Exist) && (gb_SD_Exist==1))
		{
			gc_CurrentCard=2;	
		}
		else
		{
			gc_CurrentCard=0;	
		}
	}
	else
	{
		if(gc_CurrentCard==2)
		{
			if(gc_Dosinitfail==0)  //20090803 chiayen add
			{
				if((gb_Host_Exist_pre!=gb_Host_Exist) && (gb_Host_Exist==1))
				{
					gc_CurrentCard=5;
				}
				else if(((gb_SD_Exist==0) || (gb_SDNoFileflag==1)) && (gb_Host_Exist==1))
				{
					gc_CurrentCard=5;
					if(gc_HostNoFileflag==1)
					{
						gc_CurrentCard=0;	
					}
				}
				else if((gb_SD_Exist==0) || (gb_SDNoFileflag==1))
				{
					gc_CurrentCard=0;	
				}
			}
			else //20090803 chiayen add for SD Dos initial fail
			{
				if((gb_Host_Exist==1) && (gc_HostNoFileflag==0))
				{
					gc_CurrentCard=5;
				}
				else
				{
					gc_CurrentCard=0;
				}
				gc_Dosinitfail=0;				
			}
		}
		if(gc_CurrentCard==5)
		{
			if(gc_Dosinitfail==0)  //20090803 chiayen add
			{
				if((gb_SD_Exist_pre!=gb_SD_Exist) && (gb_SD_Exist==1))
				{
					gc_CurrentCard=2;
				}
				else if(((gb_Host_Exist==0) || (gc_HostNoFileflag==1)) && (gb_SD_Exist==1))
				{
					gc_CurrentCard=2;
					if(gb_SDNoFileflag==1)
					{
						gc_CurrentCard=0;	
					}
				}
				else if((gb_Host_Exist==0) || (gc_HostNoFileflag==1))
				{
					gc_CurrentCard=0;	
				}
			}
			else //host dos initial fail  //20090803 chiayen add
			{
				if((gb_SD_Exist==1) && (gb_SDNoFileflag==0))
				{
					gc_CurrentCard=2;
				}
				else
				{
					gc_CurrentCard=0;
				}
				gc_Dosinitfail=0;
			}
		}
	}

    if(gc_CurrentCard_backup!=gc_CurrentCard)
    {
		gc_CurrentCard_backup=gc_CurrentCard;

		set_clock_mode(CLOCK_MODE_MP3); //20090803 chiayen add for media change

		if(gc_CurrentCard==2)
        {           
            if(SD_Identification_Flow())
            {
                gc_CardExist |=0x02;
            }
            else if((gb_Host_Exist==1) && (gc_HostNoFileflag==0)) //20090730 chiayen modify
            {
				DEVICE_REG[0x00]= 0x01;  //20090730 chiayen add
				if(!Host_Initial())
				{
					gc_CurrentCard = CURRENT_MEDIA_HOST;
					gb_FindFlag = 0;
            	}
	            else
	            {
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;
					InitFlash();				    
	            }
        	}
			else 
        	{
				DEVICE_REG[0x00]= 0x01;  //20090730 chiayen add
				gc_CurrentCard=0;
				gc_CardExist &=0xFD;
				gb_FindFlag = 0;
				InitFlash();
			}
			Play_SDFlash_DosInit_Dir();
        }
		else if(gc_CurrentCard==CURRENT_MEDIA_HOST)
		{
			DEVICE_REG[0x00]= 0x01;  //20090803 chiayen add			
			if(!Host_Initial())
			{
				gc_CurrentCard = CURRENT_MEDIA_HOST;
				gb_FindFlag = 0;
			}
			else if((gb_SD_Exist==1) && (gb_SDNoFileflag==0)) //20090730 chiayen add
			{
	            if(SD_Identification_Flow())
	            { 
	                gc_CardExist |=0x02;
					gc_CurrentCard=2;      
				}
				else
				{
					DEVICE_REG[0x00]= 0x01;
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;
					InitFlash();
				}
			}
			else 
        	{
				DEVICE_REG[0x00]= 0x01;
				gc_CurrentCard=0;
				gb_FindFlag = 0;
				InitFlash();
			}
			Play_SDFlash_DosInit_Dir();
		}
		else if(gc_CurrentCard==0)
        {
			DEVICE_REG[0x00]= 0x01;
			gc_CardExist &=0xFD;
			InitFlash();
			Play_SDFlash_DosInit_Dir();
		}
    }
}



void USER_Read2KBReserveBlock_DIR(U16 DirIndex, U8 tc_CurrentCard) //Ching 090306
{
	U16 tw_SectorIndex;
	U8  tc_PageNum;
	U8	tc_SetIndex;

	if(tc_CurrentCard!=0)  //20090721 chiayen add for host
	{
		tc_CurrentCard=2;	
	}

	if(!tc_CurrentCard) tc_SetIndex = gc_SetIndex; //NandFlash
	else				tc_SetIndex=gc_SDSetIndex; //SD

	tc_PageNum = (DirIndex >> 6) & 0x03; //0x11; //Ching 081121
	tw_SectorIndex = ((((tc_SetIndex & (gc_SetNumber-1)) <<2)+tc_PageNum) * gc_InAddrMax);

	if(tc_SetIndex & gc_SetNumber)	//read UserDataBlock[3]
		UserData_Block_Read(3+tc_CurrentCard,tw_SectorIndex,4,/*D_BuffDIRtable*/gc_DIRtableHB); //gc_DIRtable[]
	else							//read UserDataBlock[2]
		UserData_Block_Read(2+tc_CurrentCard,tw_SectorIndex,4,/*D_BuffDIRtable*/gc_DIRtableHB);
}


void USER_Read512BReserveBlock_DIR(U16 DirIndex, U8 tc_CurrentCard) //Ching 090306
{
	U16 tw_SectorIndex;
	U8  tc_PageNum, tc_SectorNum;
	U8	tc_SetIndex;

	if(!tc_CurrentCard) tc_SetIndex = gc_SetIndex; //NandFlash
	else				tc_SetIndex=gc_SDSetIndex; //SD

	tc_SetIndex = gc_SetIndex;
	tc_CurrentCard = 0;

	tc_PageNum = (DirIndex >> 6) & 0x03; //0x11; //Ching 081121
	tc_SectorNum = (DirIndex & 0x3F) >> 4;

	tw_SectorIndex = ((((tc_SetIndex & (gc_SetNumber-1)) <<2)+tc_PageNum) * gc_InAddrMax) + tc_SectorNum;

	if(tc_SetIndex & gc_SetNumber)	//read UserDataBlock[3]/[5]
		UserData_Block_Read(3+tc_CurrentCard,tw_SectorIndex,1,0xA8); //gc_UserDataBuf[]
	else							//read UserDataBlock[2]/[4]
		UserData_Block_Read(2+tc_CurrentCard,tw_SectorIndex,1,0xA8);
}


void IR_Service_Process_DIR(void)
{
    if(gc_IRCmdStatus==1)
    {
        ir_service_menu();
    }
}


void Dir_Task(void)
{
	gc_PlayMenu_Timeout=160;

	if(gc_TaskMode_BkUp > 3) //C_Task_Jpeg/C_Task_Mjpeg/C_Task_TXT (not C_Task_Play/C_Task_Voice)
	{
		gs_System_State.c_FileHandle = 2;
		gb_FindFlag = 0;
		gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
		if(gc_TaskMode_BkUp == C_Task_Jpeg)
		{
			EXT_NameC[0] = 3;
			EXT_NameC[1] = 'J';
			EXT_NameC[2] = 'P';
			EXT_NameC[3] = 'G';
			EXT_NameC[4] = 'B';
			EXT_NameC[5] = 'M';
			EXT_NameC[6] = 'P';
			EXT_NameC[7] = 'G';
			EXT_NameC[8] = 'I';
			EXT_NameC[9] = 'F';
			DOS_Search_File(C_File_All|C_Cnt_FileNo,C_OtherFileType,C_CmpExtName|C_Next);
			if(gw_FileTotalNumber[2]==0)
			{
				gc_Task_Current = C_Task_Jpeg;
				return;
			}
		}
		else if(gc_TaskMode_BkUp == C_Task_Mjpeg)
		{
			EXT_NameC[0] = 2;
			EXT_NameC[1] = 'S';
			EXT_NameC[2] = 'C';
			EXT_NameC[3] = 'V';
			EXT_NameC[4] = 'A';
			EXT_NameC[5] = 'M';
			EXT_NameC[6] = 'V';
			DOS_Search_File(C_File_All|C_Cnt_FileNo,C_OtherFileType,C_CmpExtName|C_Next);
			if(gw_FileTotalNumber[2]==0)
			{
				gc_Task_Current = C_Task_Mjpeg;
				return;
			}
		}
		else //error, back to Main Menu
		{
			gs_System_State.c_FileHandle = 0;
			gc_PhaseInx = 0;
			gc_Task_Current = C_Task_Menu;
			return;
		}
	}

	if((!gs_System_State.c_FileHandle) || (gs_System_State.c_FileHandle == 2)) //music or jpg/mjpg/txt, Ching 090311
	{
		//check if DIRtable is valid
//		dbprintf("================== DOS_DIRtable!! ================== %bx\n", gc_CurrentCard);
		if(!gc_CurrentCard) //NandFlash
		{
			Get_LogData_PageIndex();
			if(gc_DirReBuildFlag != D_Valid)
			{
//				dbprintf("Build Dir Table\n");
				if(DOS_DIRtable())
				{
//					dbprintf("Build Dir Table Fail!!\n");
				}
				else
				{
//					dbprintf("================== DOS_DIRtable Done!! ==================\n");
				}
			}
			else
			{
//				dbprintf("Dir Table Is Valid. No Need to ReBuild!!\n");
			}
		}
		else //SD
		{
			if(!gbt_SD_BuildDIRFlag)
			{
//				dbprintf("Build SD's Dir Table\n");
				if(DOS_DIRtable())
				{
//					dbprintf("Build SD's Dir Table Fail!!\n");
				}
				else
				{
//					dbprintf("================== SD's DOS_DIRtable Done!! ==================\n");
				}
			}
			else
			{
//				dbprintf("SD's Dir Table Is Valid. No Need to ReBuild!!\n");
			}
		}
	}

	if(!gs_System_State.c_FileHandle) //music
		DirManagement_Init();
	else if(gs_System_State.c_FileHandle == 2) //jpg/mjpg/txt, Ching 090311
		OtherManagement_Init();

	while(1)
	{
		DirManagement_Process();
		gc_PhaseInx=0;

//		if(gc_PlayMenu_IR==0)
		{
			Polling_TaskEvents();
            IR_Service_Process_DIR();   
		}

	    if(gc_PhaseInx==C_PowerOff)
		{
			gc_Task_Next=C_Task_PowerOff;
		}  					    	

		Play_SourceDetect_Process_Dir();

       	if(gc_Task_Current!=gc_Task_Next)
        {
			gc_DispWallpaper=1;  //InitDisp variable
			if(gc_TaskMode_BkUp==C_Task_Jpeg)
			{
				gc_PhaseInx=1;	
			}
			else if(gc_TaskMode_BkUp==C_Task_Mjpeg)  //20090331
			{
				gc_PhaseInx=3;	
			}

			if(gb_SourceChange_Dir==0)
			{
				gb_DirPlay_Flag=1;
			}
			else
			{
				gb_SourceChange_Dir=0;
			}
            gc_Task_Current=gc_Task_Next;
            break;  
        }
	}
}


void USER_WriteReserveBlock_DIR(U8 tc_PageNum, U8 tc_CurrentCard) //Ching 090306
{
	U16 tw_SectorIndex;
	U8	tc_SetIndex;
	U8  tc_1stBlock, tc_2ndBlock;

	tc_1stBlock = 2 + tc_CurrentCard;
	tc_2ndBlock = 3 + tc_CurrentCard;

	if(!tc_CurrentCard) tc_SetIndex = gc_SetIndex; //NandFlash
	else				tc_SetIndex=gc_SDSetIndex; //SD

	if(tc_SetIndex == (gc_SetNumber*2))
	{
		UserData_Block_Erase(tc_2ndBlock); //[3]/[5]
		tc_SetIndex = 0;

		if(!tc_CurrentCard) gc_SetIndex = 0; //NandFlash
		else				gc_SDSetIndex=0; //SD
	}
	else if(tc_SetIndex == gc_SetNumber)
		UserData_Block_Erase(tc_1stBlock); //[2]/[4]

	tw_SectorIndex = (((tc_SetIndex & (gc_SetNumber-1)) <<2)+tc_PageNum) * gc_InAddrMax;
	//dbprintf("Write - gc_SetIndex:%bx, tw_SectorIndex:%x %bx %bx\n",gc_SetIndex,tw_SectorIndex,gc_SetIndex,gc_SetNumber);

	if(tc_SetIndex & gc_SetNumber)	//write to UserDataBlock[3]/[5]
	{
		UserData_Block_Write(tc_2ndBlock,tw_SectorIndex,4,gc_DIRtableHB);
	}
	else							//write to UserDataBlock[2]/[4]
	{
		UserData_Block_Write(tc_1stBlock,tw_SectorIndex,4,gc_DIRtableHB);
	}
}


