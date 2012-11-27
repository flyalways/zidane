#include "..\Header\SPDA2K.h"
#include "..\header\dos\fs_struct.h"
#include "..\header\Memalloc.h"
#include "Dir_process.h"
#include "..\IR\LCM_BMP.h"
#include "..\header\variables.h"
#include "PROTOTYP.h"

#define D_Expired		0xAA

#define K_COUNTER_DIRNUM  11  //20090216 chiayen add

#define C_DirIdle      	0x00
#define C_DirNext      	0x01
#define C_DirPrev      	0x02
#define C_DirBackUpper	0x03
#define C_DirEnterChild	0x04

#if (LCM_SIZE==1)
#define C_BUF_LENGTH    128     
#define C_LCM_ROW_MAX     3     
#define C_BUF_STRING    122     
#define DIR_OFFSET        0     
#define Glass_ColumnNum  96     
#endif
#if (LCM_SIZE==0)||(LCM_SIZE==2)
#define C_LCM_ROW_MAX     4     
#define C_BUF_LENGTH	128 //512/4
#define C_BUF_STRING	122 //取小於128-5且為偶數者
#define DIR_OFFSET       18     
#define Glass_ColumnNum 128     
#endif

#define SWCLASS_1	0x01
#define SWCLASS_2	0x02
#define SWCLASS_3	0x03
#define SWCLASS_4	0x04
#define SWCLASS_5	0x05
#define SWCLASS_6	0x06
#define SWCLASS_7	0x07
#define SWCLASS_8	0x08

xdata UBYTE gc_DirReBuildFlag=0;

extern data	 U8		gc_LCM_Media;
extern data	 U8		gc_DisplayEQIcon;
extern data	 U8		gc_DisplayPlayMode;

extern xdata	 U16	gw_DisplayFreq1;
extern xdata	 U16	gw_DisplayTime;
extern xdata	 U16	gw_DisplayBitRate;
extern xdata	 U16	gw_DispSongNum1;
extern xdata UBYTE  gcPlay_FileType;

extern bit   gb_FindFlag;
extern data  System_Struct gs_System_State;
extern xdata U8 gc_RepPlayMode_Dir;
extern xdata U8 gc_PlayMenu_IR;

U8 LCD_disp_HZK_string(U8 Page,U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 ColumnOffset,bit reverse);
void FatherDir_FDBInit(U16 tw_FatherIndex);
void Get_TotalNumber(U16 tw_DirIndex);
void UI_DispGet_FatherName(void);
void UI_DispGet_ChildName(void);
void LCD_Disp_FatherName_Dir(void);
void UI_Disp_ChildName();
void DirManagement_Init();
void UI_DIRStoreOrRecover(bit tb_SaveOrRecover);
void DirManagement_Process();

U8 GetCurrentDirInfo();

extern  U8   Find_Fdb(SearchFdb *p_mp);
extern 	U8   Get_LogData_PageIndex(void);
extern 	U8   USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);//use reserveblock for log
extern void LCD_Disp_NoFile();
extern void ir_service_menu(void);  //20090206 chiayen add for IR_DIR
extern void LCM_ShowFloderIcon(U8 Column);  //20090216 chiayen test
xdata U8 gc_test=0;
bit gbt_DIRwFakeFDB=0; //Ching 090420
extern U8	gc_clock_mode;  //20090817 chiayen add
xdata U16 gw_AllDirTotalNumber=0;


void USER_GetUISetIndex(void)
{
	U16 tw_SectorIndex;
	U8 tc_SetIndex;
	U8 tc_i, tc_SearchNum;

	tc_SearchNum = gc_SetNumber*2;
	tc_SetIndex = tc_SearchNum-1;

	for(tc_i=0; tc_i<tc_SearchNum; tc_i++)
	{
		tw_SectorIndex = ((tc_SetIndex & (gc_SetNumber-1)) <<2) *gc_InAddrMax;

		if(tc_SetIndex & gc_SetNumber)	//read UserDataBlock[3]
			UserData_Block_Read(3,tw_SectorIndex,1,gc_UserDataBufHB);
		else							//read UserDataBlock[2]
			UserData_Block_Read(2,tw_SectorIndex,1,gc_UserDataBufHB);

		if(gc_UserDataBuf[0]!=0xff)
			break;

		tc_SetIndex --;
	}
	if(tc_i==tc_SearchNum)
		gc_SetIndex = 0;
	else
		gc_SetIndex = tc_SetIndex;
}




void UI_DispGetDirName(U8 tc_DIRtableIndex)
{
	U16 tw_DIRtableOffset;

	tw_DIRtableOffset = tc_DIRtableIndex << 5;

	memcpy(&gs_File_FCB[2].dw_LongFDB_LogAdd0,&gc_DIRtable[tw_DIRtableOffset+0x09],4);//it's useful when the LFN FDB is at two sectors
   	memcpy(&gs_File_FCB[2].dw_FDB_LogAdd,&gc_DIRtable[tw_DIRtableOffset+0x11],4);
    memcpy(&gs_File_FCB[2].w_FDB_Offset,&gc_DIRtable[tw_DIRtableOffset+0x15],2);
    DOS_GetLongFileName(2,gc_FileLongName);
}





void LCD_Disp_FatherName_Dir(void)
{
	LCM_clear();
	LCD_disp_HZK_string(0, &gc_DirLCMtempBuf[5], gc_DirLCMtempBuf[2], gc_DirLCMtempBuf[4], DIR_OFFSET, 0); //Ching 080926
}


void LCD_erase_one_page(U8 tc_page)
{
	U8 i;
	LCM_set_address(tc_page, 0);
	for(i=0;i<128;i++)
	{
		LCM_write_data(0x00);
	}
}



void IR_Service_Process_Dir()  //20090206 chiayen add for IR_DIR
{
    if(gc_IRCmdStatus==1)
    {
        ir_service_menu();
    }
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



//=========================================================================
//=========================================================================
//=========================================================================
U8 DOS_DIRtable(void)
{
	U32 tdw_LongFDB_LogAdd1;
	U16 tw_DirTabNum, tw_DirTotalNumber;
	U16 tw_TempTotalNum; //add for the sum of dir and file number in one directory
	U16 tw_DirIndex[2];
	U16 tw_FatherIndex, tw_OldFatherIndex;
	U8  tc_DirTableNo, tc_IndexInOneDir;
	U8  tc_SpanNumber; //跨越2KB Page的標誌位, 0: 1st page, 1: 2nd page...
	bit tb_HasNextDirFlag=0;

	gc_DirReBuildFlag=D_Expired;//Mark DirTable is In-Valid
	USER_MarkTable(gc_DirReBuildFlag);//在保留區標誌DirTable無效，防止建立Table時突然掉電

	gc_SetIndex ++;
	memset(gc_DIRtable,0,2048);	//clear buffer gc_DIRtable
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

		if(gw_DirTotalNumber>=0xff)gw_DirTotalNumber=0xff;

		tw_TempTotalNum=gw_DirTotalNumber+gw_FileTotalNumber[0];

		gs_DIRtable[0].w_file_folder_num	= tw_TempTotalNum;
		gs_DIRtable[0].c_file_num_hi		= ((U8 *)(&gw_FileTotalNumber[0]))[0];//save file number High 8bit of this directory
		gs_DIRtable[0].c_file_num_lo		= ((U8 *)(&gw_FileTotalNumber[0]))[1];//save file number Low 8bit of this directory
		gs_DIRtable[0].c_subdir_num			= ((U8 *)(&gw_DirTotalNumber))[1];	//save dir number of this directory
	
		if(gs_DIRtable[0].c_subdir_num == 0) //No Directory
 	    {
			USER_WriteReserveBlock_DIR(0);
//			dbprintf("NoDir\n");
			gc_DirReBuildFlag=0x55; //Mark DirTable is Valid, Ching 081003
			USER_MarkTable(gc_DirReBuildFlag);
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
			USER_WriteReserveBlock_DIR(tc_SpanNumber);
			break;
		}

		if(tc_SpanNumber)//當目錄存儲信息有跨2K Page時
		{
			if((tw_FatherIndex&0xC0)!=(tw_DirTabNum&0xC0)) //Father Dir and Child Dir aren't in the same page
				USER_Read512BReserveBlock_DIR(tw_FatherIndex); //gc_UserDataBuf
			else
				memcpy(&gc_UserDataBuf[0], &gc_DIRtable[((tw_FatherIndex&0x3F)<<5)&0xFE1F], 512); //>>4<<9
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
					USER_Read512BReserveBlock_DIR(tw_DirTabNum-1); //gc_UserDataBuf
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
//					dbprintf("return 1\n");
					return 1;
				}

				if((tw_FatherIndex&0x30)!=(tw_OldFatherIndex&0x30)) //not in the same sector
				{
					if((tw_FatherIndex&0xC0)!=(tw_DirTabNum&0xC0))	//need to read from reserved block
						USER_Read512BReserveBlock_DIR(tw_FatherIndex); //gc_UserDataBuf
					else
						memcpy(gc_UserDataBuf, &gc_DIRtable[((tw_FatherIndex&0x3F)<<5)&0xFE1F], 512); //>>4<<9
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
							USER_Read512BReserveBlock_DIR(tw_FatherIndex); //gc_UserDataBuf
						else
							memcpy(gc_UserDataBuf, &gc_DIRtable[((tw_FatherIndex&0x3F)<<5)&0xFE1F], 512); //>>4<<9
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
			if(gw_DirTotalNumber>=0xff)	gw_DirTotalNumber=0xff;
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
			USER_WriteReserveBlock_DIR(tc_SpanNumber);
			memset(gc_DIRtable,0,2048);	//clear buffer gc_DIRtable
			tc_SpanNumber++;
		}
	}//for( tw_DirTabNum=1; tw_DirTabNum<256; tw_DirTabNum++ )//擴展到256個Dir

//	dbprintf("All Dir--%x\n",tw_DirTotalNumber);

    gw_AllDirTotalNumber = tw_DirTotalNumber;
	gc_DirReBuildFlag=0x55; //Mark DirTable is Valid
	USER_MarkTable(gc_DirReBuildFlag);

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
//        if(((tc_Mode&0x0f0)==0x30)/*|| (tc_Mode&0x0f0)==0x20)*/)
        {//find dir in one dir
            tc_FindFile=0 ;
            tc_FindDir=1 ;
            ptr.dw_FDB_StartCluster=gs_DIR_FCB[0].dw_FDB_StartCluster ;
        }
//        else //find file or search in all dir
//            return(1);//this function isn't used to find file and search in all dir


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
	UI_DIRStoreOrRecover(1);//save
	gc_FatherIndex = 0; 	//root
	gw_DIRtableIndex = 1;   //root's 1st child dir
	gw_Index = 1;			//1st dir
	gc_DirNoFile_Flag=0;

	USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex);	//gc_DIRtable[]

	Get_TotalNumber(gc_FatherIndex);
	FatherDir_FDBInit(gc_FatherIndex);

	gc_PhaseInx = 0;

	if(gw_TotalNumber)
	{
		UI_DispGet_ChildName();
	}

	#if (LCM_SIZE==0)||(LCM_SIZE==2)
	LCM_ShowFloderIcon(0);  //20090216 chiayen test
	#endif
	if(gw_TotalNumber)
	{
		UI_Disp_ChildName();
	}
	else
	{
		LCD_Disp_NoFile();
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


void Get_TotalNumber(U16 tw_DirIndex)
{
	tw_DirIndex &= 0x3F;
	gw_TotalNumber						= gs_DIRtable[tw_DirIndex].w_file_folder_num;
	((U8 *)(&gw_FileTotalNumber[0]))[0] = gs_DIRtable[tw_DirIndex].c_file_num_hi;
	((U8 *)(&gw_FileTotalNumber[0]))[1] = gs_DIRtable[tw_DirIndex].c_file_num_lo;
	gw_DirTotalNumber					= gs_DIRtable[tw_DirIndex].c_subdir_num;
}


void FatherDir_FDBInit(U16 tw_FatherIndex)
{
	tw_FatherIndex &= 0x3F;
	gs_File_FCB[0].dw_FDB_StartCluster = gs_DIRtable[tw_FatherIndex].dw_File_StartCluster;

	gb_FindFlag = 0;
	gw_FileIndex[0] = 0;
}


void DirManagement_Process() //adjust code to match both music and voice case, Ching 080926
{
	UBYTE tc_MenuItem, tc_LCMRowMinus1, tc_i;
	WORD  tw_BottomIndex, tw_Index, tw_DIRtableIndex;

	if(gc_PhaseInx)
	{
        switch(gc_PhaseInx)
        {
            case C_FmFreqChg:
				tc_MenuItem = 3;
            break;

            case C_PlayNext:
				tc_MenuItem = 0;
				gc_PlayMenu_Timeout=160;
            break;

            case C_PlayPrev:
				tc_MenuItem = 1;
				gc_PlayMenu_Timeout=160;
            break;
		/*
            case C_FmFreqChg://C_FmFreqChgUp:
				gc_DirNoFile_Flag=0;
				if(gc_FatherIndex!=0) //Ching 081009
				{
					tc_MenuItem = 2;
					break;
				}
				UI_DIRStoreOrRecover(0);//recover
				gc_DIRTimeOut_Flag=1;
				gb_FindFlag = 1;
				gc_Task_Next=C_Task_PlayMenu_IR;//C_Task_PlayMenu;			
				return;
            break;
		*/
			default:		// other-key , no action lijian.peng 081024
				gc_PhaseInx=0;
				return ;
			break;
        }
		gc_PlayMenu_Timeout=160;			

		tc_LCMRowMinus1 = C_LCM_ROW_MAX-1;
		
		if(tc_MenuItem==0) //Next
		{
			if(gw_TotalNumber)
			{
				if(gw_Index==gw_TotalNumber)//last->1st page
				{
					gw_Index=1;
					if(gw_DirTotalNumber)	
					{
						tw_DIRtableIndex = gw_DIRtableIndex;
						gw_DIRtableIndex -= (gw_DirTotalNumber-1);
						if((gw_DIRtableIndex&0xc0) != (tw_DIRtableIndex&0xc0))
							USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex);
					}
					UI_DispGet_ChildName();
				}
				else
				{
					gw_Index ++;
					if(gw_Index<=gw_DirTotalNumber) 
					{
						tw_DIRtableIndex = gw_DIRtableIndex; //Ching 081218
						gw_DIRtableIndex++;
					}
					if(((gw_Index-1)%tc_LCMRowMinus1)==0) //Next page
					{
						if(gw_DirTotalNumber) //Ching 081218
						{
							if((gw_DIRtableIndex&0xc0) != (tw_DIRtableIndex&0xc0))
								USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex);
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
				if(gw_Index==1)//1st->last page
				{
					gw_Index=gw_TotalNumber;
					if(gw_DirTotalNumber)
					{
						tw_DIRtableIndex = gw_DIRtableIndex;
						gw_DIRtableIndex += (gw_DirTotalNumber-1);
						if((gw_DIRtableIndex&0xc0) != (tw_DIRtableIndex&0xc0))
							USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex);
					}
					if(gw_FileTotalNumber[gs_System_State.c_FileHandle])
					{
						if((((gw_Index-1)/tc_LCMRowMinus1)*tc_LCMRowMinus1) > gw_DirTotalNumber) 
							gw_FileSkipNumber = (((gw_Index-1)/tc_LCMRowMinus1)*tc_LCMRowMinus1 - gw_DirTotalNumber) + 1;
						else
							gw_FileSkipNumber = 1;
						gb_FindFlag = 0;
						gw_FileIndex[gs_System_State.c_FileHandle] = 0;
					}
					UI_DispGet_ChildName();
				}
				else
				{
					gw_Index --;
					if(gw_Index<gw_DirTotalNumber) 
					{
						tw_DIRtableIndex = gw_DIRtableIndex;
						gw_DIRtableIndex--;
					}
					if((gw_Index%tc_LCMRowMinus1)==0) //Prev page
					{
						if(gw_Index <= tc_LCMRowMinus1) //2nd->1st page, Ching 080926
							gw_FileSkipNumber = 0;
						else
						{
							if((gw_Index-tc_LCMRowMinus1+1)>gw_DirTotalNumber) //it has some file in this page
							gw_FileSkipNumber = (((gw_Index-1)/tc_LCMRowMinus1)*tc_LCMRowMinus1 - gw_DirTotalNumber) + 1;
						}

						gb_FindFlag = 0;
						gw_FileIndex[gs_System_State.c_FileHandle] = 0;

						if(gw_DirTotalNumber) //Ching 081218
						{
							if((gw_DIRtableIndex&0xc0) != (tw_DIRtableIndex&0xc0))
								USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex);
						}
						UI_DispGet_ChildName();
					}
				}
				gb_TriggerFileSkip = 0;
			}
		} 
		else if(tc_MenuItem==2) //back to upper level
		{
			if(gc_FatherIndex!=0) //not Root
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

				if((tw_BottomIndex&0xc0) != (gc_FatherIndex&0xc0))
				{
					USER_Read2KBReserveBlock_DIR(gc_FatherIndex);	//gc_DIRtable[]
				}

				gw_DIRtableIndex = gc_FatherIndex;
				gc_FatherIndex = gs_DIRtable[gw_DIRtableIndex&0x3F].c_index_parent_folder;
				if((gw_DIRtableIndex&0xc0) != (gc_FatherIndex&0xc0))
				{
					USER_Read2KBReserveBlock_DIR(gc_FatherIndex);	//gc_DIRtable[]
					tw_Index = gs_DIRtable[gc_FatherIndex&0x3F].c_index_child_folder;
					Get_TotalNumber(gc_FatherIndex);
					FatherDir_FDBInit(gc_FatherIndex);
					if(gc_FatherIndex)
				 	{
						#if (LCM_SIZE==0)||(LCM_SIZE==2)
						UI_DispGet_FatherName();
						#endif
					}
					USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex);	//gc_DIRtable[]
				}
				else
				{
					tw_Index = gs_DIRtable[gc_FatherIndex&0x3F].c_index_child_folder;
					Get_TotalNumber(gc_FatherIndex);
					FatherDir_FDBInit(gc_FatherIndex);
					if(gc_FatherIndex)
				 	{
						#if (LCM_SIZE==0)||(LCM_SIZE==2)
						UI_DispGet_FatherName();
						#endif
					}
				}
				gw_Index = gw_DIRtableIndex - tw_Index + 1;

				UI_DispGet_ChildName();
				if(!gc_FatherIndex)
				{
					#if (LCM_SIZE==0)||(LCM_SIZE==2)
					LCM_ShowFloderIcon(0);  //20090216 chiayen test
					#endif
				}
				else
				{
					LCD_Disp_FatherName_Dir();
				}
			}
			else//(JC)H1007 Root
			{
				if(!gs_System_State.c_FileHandle) //music
				{	
					gc_Task_Next=C_Task_Play;	
				}

				UI_DIRStoreOrRecover(0);//recover
				gw_init_needed |= SET_BIT15;//(JC)H1007
				return;
			}
		}
		else if(tc_MenuItem==3) //enter child dir or confirm selected item
		{
			if(gw_Index>gw_DirTotalNumber) //file
			{
				if(gw_FileTotalNumber[gs_System_State.c_FileHandle]) //Ching 081106
				{
					if(gs_System_State.c_FileHandle==0) //music
					{
						gw_FileSkipNumber = gw_Index-gw_DirTotalNumber;
						gb_FindFlag = 0;
						gw_FileIndex[gs_System_State.c_FileHandle] = 0;
						gb_TriggerFileSkip = 1;
						DOS_Search_File(C_File_OneDir|C_By_FDB, gs_System_State.c_FileHandle, C_CmpExtName|C_Next);
						gb_TriggerFileSkip = 0;
					}
	
					DOS_GetLongFileName(gs_System_State.c_FileHandle,gc_FileLongName);
					gcPlay_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;//lijian.peng modify 081016 for play error from pause to DIR

					if(!gs_System_State.c_FileHandle) //music
					{		
						gc_Task_Next=C_Task_Play;
						gc_PhaseInx = 0;								
					}
					gw_init_needed |= SET_BIT15;//(JC)H1007
					return;	
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
					USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex);	//gc_DIRtable[]
				}

				gc_FatherIndex = gw_DIRtableIndex;
				Get_TotalNumber(gc_FatherIndex);
				tc_i = (gw_Index-1)%tc_LCMRowMinus1 + 1;
				memcpy(&gc_DirLCMtempBuf[0], &gc_DirLCMtempBuf[C_BUF_LENGTH*tc_i], C_BUF_LENGTH);

				if(gw_TotalNumber)
				{
					FatherDir_FDBInit(gc_FatherIndex);
					gw_DIRtableIndex = gc_DIRtable[((gc_FatherIndex & 0x3F) <<5)+0x18]; //1st child
					if((gc_FatherIndex&0xc0) != (gw_DIRtableIndex&0xc0))
					{
						USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex);	//gc_DIRtable[]
					}
					gw_Index = 1;

					UI_DispGet_ChildName();
					LCD_Disp_FatherName_Dir();									
				}
				else
				{
					LCM_clear();
					LCD_Disp_NoFile();
					gc_DirNoFile_Flag=1;
					return;
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
		if(gc_PlayMenu_Timeout==0)  //TimeOut
		{
			UI_DIRStoreOrRecover(0);//recover
			gc_DIRTimeOut_Flag=1;
			gb_FindFlag = 0;  //20090817 chiayen modify
			gc_PhaseInx=0;//20090817 chiayen add
			gw_init_needed=0xFFFF;//20090817 chiayen add
            gw_DispSongNum1=0xFF;
			gc_Task_Next=C_Task_Play;
			return;
		}
	}
}


void UI_DispGet_FatherName(void)
{
	WORD tw_Length;
	UBYTE tc_TableOffset;

	UI_DispGetDirName(gc_FatherIndex&0x3F);
	memcpy(&tw_Length,&gc_FileLongName[3],2);

	if(tw_Length>C_BUF_STRING) 
	{
		memcpy(&gc_DirLCMtempBuf[0], &gc_FileLongName[0],C_BUF_STRING+6);
		gc_DirLCMtempBuf[1] = 2;	//dir
		gc_DirLCMtempBuf[3] = 0;	//high byte of length
		gc_DirLCMtempBuf[4] = C_BUF_STRING;//low byte of length
	}
	else
	{
		tc_TableOffset = (U8)(tw_Length+6);
		memcpy(&gc_DirLCMtempBuf[0], &gc_FileLongName[0],tc_TableOffset);
		gc_DirLCMtempBuf[1] = 2;	//dir
	}
}


void UI_DispGet_ChildName(void)
{
	U8 	tc_i, tc_TableOffset;
	U16 tw_Index, tw_TempIndex, tw_CurrentTableIndex, tw_Length;

	tw_Index = ((gw_Index-1)/(C_LCM_ROW_MAX-1))*(C_LCM_ROW_MAX-1) + 1;
	if(tw_Index<=gw_DirTotalNumber)
	{
		if((gw_Index-tw_Index) > (gw_DirTotalNumber-tw_Index))
		{
			tw_CurrentTableIndex = gw_DIRtableIndex - (gw_DirTotalNumber-tw_Index);
		}
		else
		{
			tw_CurrentTableIndex = gw_DIRtableIndex - (gw_Index-tw_Index);
		}

		if((tw_CurrentTableIndex&0xc0) != (gw_DIRtableIndex&0xc0)) //different pages
		{
			USER_Read2KBReserveBlock_DIR(tw_CurrentTableIndex);
		}
	}

	for(tc_i=1; tc_i<C_LCM_ROW_MAX; tc_i++)
	{
		tw_TempIndex = (U16)C_BUF_LENGTH*tc_i;

		if(tw_Index <= gw_TotalNumber)
		{
			if(tw_Index <= gw_DirTotalNumber) //Dir
			{
				tc_TableOffset = (U8)tw_CurrentTableIndex & 0x3F;

				if(tw_CurrentTableIndex!=0)  //20081204 chiayen test
				{
					UI_DispGetDirName(tc_TableOffset);
				}
	
				memcpy(&tw_Length,&gc_FileLongName[3],2);

				if(tw_Length>C_BUF_STRING) 
				{
					memcpy(&gc_DirLCMtempBuf[tw_TempIndex], &gc_FileLongName[0],C_BUF_STRING+6);
					gc_DirLCMtempBuf[tw_TempIndex+1] = 2;	//dir
					gc_DirLCMtempBuf[tw_TempIndex+3] = 0;	//high byte of length
					gc_DirLCMtempBuf[tw_TempIndex+4] = C_BUF_STRING;//low byte of length
				}
				else
				{
					tc_TableOffset = (U8)(tw_Length+6);
					memcpy(&gc_DirLCMtempBuf[tw_TempIndex], &gc_FileLongName[0],tc_TableOffset);
					gc_DirLCMtempBuf[tw_TempIndex+1] = 2;	//dir
				}
	
				if((tw_Index < gw_DirTotalNumber) && (tc_i!=(C_LCM_ROW_MAX-1))) //it has the next dir, Ching 081218
				{
					if(((tw_CurrentTableIndex+1)&0xc0) != (tw_CurrentTableIndex&0xc0)) //next dir and current dir are in different pages
						USER_Read2KBReserveBlock_DIR(tw_CurrentTableIndex+1);	//gc_DIRtable[]
					tw_CurrentTableIndex++;
				}
			}
			else //File
			{//adjust code to match both music and voice case, Ching 080926
				if(gs_System_State.c_FileHandle==0) 	//music
				{
					DOS_Search_File(C_File_OneDir|C_By_FDB, gs_System_State.c_FileHandle, C_CmpExtName|C_Next);
				}

				DOS_GetLongFileName(gs_System_State.c_FileHandle,gc_FileLongName);
				memcpy(&tw_Length,&gc_FileLongName[3],2);

				if(tw_Length>C_BUF_STRING) 
				{
					memcpy(&gc_DirLCMtempBuf[tw_TempIndex], &gc_FileLongName[0],C_BUF_STRING+6);
					gc_DirLCMtempBuf[tw_TempIndex+1] = 3;	//file
					gc_DirLCMtempBuf[tw_TempIndex+3] = 0;	//high byte of length
					gc_DirLCMtempBuf[tw_TempIndex+4] = C_BUF_STRING;//low byte of length
				}
				else
				{
					tc_TableOffset = (U8)(tw_Length+6);
					memcpy(&gc_DirLCMtempBuf[tw_TempIndex], &gc_FileLongName[0],tc_TableOffset);
					gc_DirLCMtempBuf[tw_TempIndex+1] = 3;	//file
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


void USER_Read2KBReserveBlock_DIR(U16 DirIndex)
{
	U16 tw_SectorIndex;
	U8  tc_PageNum;

	tc_PageNum = (DirIndex >> 6) & 0x03; //Ching 081121
	tw_SectorIndex = ((((gc_SetIndex & (gc_SetNumber-1)) <<2)+tc_PageNum) * gc_InAddrMax);
	if(gc_SetIndex & gc_SetNumber)	//read UserDataBlock[3]
	{
		UserData_Block_Read(3,tw_SectorIndex,4,gc_DIRtableHB); //gc_DIRtable[]
	}
	else							//read UserDataBlock[2]
	{
		UserData_Block_Read(2,tw_SectorIndex,4,gc_DIRtableHB);
	}
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


void USER_Read512BReserveBlock_DIR(U16 DirIndex)
{
	U16 tw_SectorIndex;
	U8  tc_PageNum, tc_SectorNum;

	tc_PageNum = (DirIndex >> 6) & 0x03;
	tc_SectorNum = (DirIndex & 0x3F) >> 4;

	tw_SectorIndex = ((((gc_SetIndex & (gc_SetNumber-1)) <<2)+tc_PageNum) * gc_InAddrMax) + tc_SectorNum;

	if(gc_SetIndex & gc_SetNumber)	//read UserDataBlock[3]
		UserData_Block_Read(3,tw_SectorIndex,1,gc_UserDataBufHB); //gc_UserDataBuf[]
	else							//read UserDataBlock[2]
		UserData_Block_Read(2,tw_SectorIndex,1,gc_UserDataBufHB);
}


void Dir_Task(void)
{
	gc_PlayMenu_Timeout=160;
	LCM_clear();
	DirManagement_Init();

	while(1)
	{
		DirManagement_Process();
		gc_PhaseInx=0;

//		if(gc_PlayMenu_IR==0)
		{
			Polling_TaskEvents();
		}
//		else
		{
			IR_Service_Process_Dir();  //20090206 chiayen add for IR_DIR
		}

       	if(gc_Task_Current!=gc_Task_Next)
        {
			gc_RepPlayMode = gc_RepPlayMode_Dir;
			LCM_clear();
			gc_LCM_Media=0xff;
			gc_DisplayEQIcon=0xFF;
			gw_DisplayBitRate=0xFFFF;
			gc_DisplayPlayMode=0xFF;
			gw_DispSongNum1=0xFF;
			#if (LCM_SIZE==1)
			gc_LCM_line=0;
			gw_DisplayTime=0xFFFF;
			gw_DisplayFreq1=0xFFFF;
			#endif

			if(gc_DIRTimeOut_Flag==0)
			{
				gb_DirPlay_Flag=1;
			}
			gw_DispFileName_ByteOffset=0;
            gc_Task_Current=gc_Task_Next;
            break;  
        }
	}
}


void USER_WriteReserveBlock_DIR(U8 tc_PageNum)
{
	U16 tw_SectorIndex;

	if(gc_SetIndex == (gc_SetNumber*2))
	{
		UserData_Block_Erase(3);
		gc_SetIndex = 0;
	}
	else if(gc_SetIndex == gc_SetNumber)
	{
		UserData_Block_Erase(2);
	}

	tw_SectorIndex = (((gc_SetIndex & (gc_SetNumber-1)) <<2)+tc_PageNum) * gc_InAddrMax;
	if(gc_SetIndex & gc_SetNumber)	//write to UserDataBlock[3]
	{
		UserData_Block_Write(3,tw_SectorIndex,4,gc_DIRtableHB);
	}
	else							//write to UserDataBlock[2]
	{
		UserData_Block_Write(2,tw_SectorIndex,4,gc_DIRtableHB);
	}
}


void UI_Disp_ChildName(void)//tc_mode for color project
{
	U8 	tc_i, tc_ReversePage;
	U16 tw_TempIndex;

	tc_ReversePage = gw_Index % (C_LCM_ROW_MAX-1);
	if(!tc_ReversePage)
 	{
		tc_ReversePage=(C_LCM_ROW_MAX-1);
	}

	#if (LCM_SIZE==1)
	for(tc_i=0; tc_i<C_LCM_ROW_MAX; tc_i++)
	#endif
	#if (LCM_SIZE==0)||(LCM_SIZE==2)
	for(tc_i=1; tc_i<C_LCM_ROW_MAX; tc_i++)
	#endif
	{
		LCD_erase_one_page(tc_i*2);
		LCD_erase_one_page(tc_i*2+1);
	}
	
	for(tc_i=1; tc_i<C_LCM_ROW_MAX; tc_i++)
	{
		tw_TempIndex = (U16)C_BUF_LENGTH*tc_i;

		if(gc_DirLCMtempBuf[tw_TempIndex+4])
		{
			if(tc_i != tc_ReversePage)
			{
				#if (LCM_SIZE==1)
				LCD_disp_HZK_string((tc_i-1)*2, &gc_DirLCMtempBuf[tw_TempIndex+5], gc_DirLCMtempBuf[tw_TempIndex+2], gc_DirLCMtempBuf[tw_TempIndex+4], DIR_OFFSET, 0);
				#endif
				#if (LCM_SIZE==0)||(LCM_SIZE==2)
				LCD_disp_HZK_string(tc_i*2, &gc_DirLCMtempBuf[tw_TempIndex+5], gc_DirLCMtempBuf[tw_TempIndex+2], gc_DirLCMtempBuf[tw_TempIndex+4], DIR_OFFSET, 0);
				#endif
			}
			else //reverse
			{
				#if (LCM_SIZE==1)
				LCD_disp_HZK_string((tc_i-1)*2, &gc_DirLCMtempBuf[tw_TempIndex+5], gc_DirLCMtempBuf[tw_TempIndex+2], gc_DirLCMtempBuf[tw_TempIndex+4], DIR_OFFSET, 1);
				#endif
				#if (LCM_SIZE==0)||(LCM_SIZE==2)
				LCD_disp_HZK_string(tc_i*2, &gc_DirLCMtempBuf[tw_TempIndex+5], gc_DirLCMtempBuf[tw_TempIndex+2], gc_DirLCMtempBuf[tw_TempIndex+4], DIR_OFFSET, 1);
				#endif
			}
		}
	}
}


U8 LCD_disp_HZK_string(U8 Page,U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 ColumnOffset,bit reverse)
{
	U8 i,Column;
	U8 Tmp_DataBuf[32];
	U8 tc_ColumnNum,tc_FirstWorldColumnNum;
	U8 Sts;

	tc_FirstWorldColumnNum = 0x00;
	i=0;
	Column=ColumnOffset;
	Sts = 0;

	while(i<nByte)
	{
		if(!tc_ISNOrUnicode)
		{
			tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(DataBuf+i+1),*(DataBuf+i),Tmp_DataBuf,1);
		}else{
			tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(DataBuf+i),*(DataBuf+i+1),Tmp_DataBuf,0);
		}
		if(i==0)
		{
			tc_FirstWorldColumnNum = tc_ColumnNum;
		}
		if((Column+(tc_ColumnNum&0x7f))>Glass_ColumnNum)
		{
			Sts=1;
			break;
		}
		if((tc_ColumnNum&0x7f) > 8)
			LCM_disp_HZKCharBMP(Page,Column,Tmp_DataBuf,LCM_IsWord,reverse);
		else
			LCM_disp_HZKCharBMP(Page,Column,Tmp_DataBuf,LCM_IsChar,reverse);
		Column+=(tc_ColumnNum&0x7f);

		if(tc_ColumnNum&0x80)
			i+=2;
		else
			i+=1;
	}

	while(Column<Glass_ColumnNum)
	{
		LCM_set_address(Page,Column);
		LCM_write_data(0x00);
		LCM_set_address(Page+1,Column);
		LCM_write_data(0x00);
		Column++;
	}

	if(Sts)
	{
		if(tc_FirstWorldColumnNum&0x80)
		{
			gw_DispFileName_ByteOffset++;
		}
		gw_DispFileName_ByteOffset+=1;
	}else{
		gw_DispFileName_ByteOffset=0;
	}
	return Sts;//overstep display area
}

void SwitchClass(U8 tClass)
{
    U8 tIndex;
    switch(tClass)
    {
       case SWCLASS_1:
       tIndex=1;
       break;
       case SWCLASS_2:
       tIndex=2;
       break;
       case SWCLASS_3:
       tIndex=3;
       break;
       case SWCLASS_4:
       tIndex=4;
       break;
       case SWCLASS_5:
       tIndex=5;
       break;
       case SWCLASS_6:
       tIndex=6;
       break;
       case SWCLASS_7:
       tIndex=7;
       break;
       case SWCLASS_8:
       tIndex=8;
       break;

       // Just quit if passed into an invalid dir index.
       default:
           return;
           break;
    }

    //gs_DIR_FCB[0].dw_FDB_StartCluster    = gs_DIRtable[tIndex].dw_FDB_StartCluster;
    //gs_DIR_FCB[0].dw_LongFDB_LogAdd0 = gs_DIRtable[tIndex].dw_LongFDB_LogAdd0;
    //gs_DIR_FCB[0].dw_FDB_Cluster     = gs_DIRtable[tIndex].dw_FDB_Cluster;
    //gs_DIR_FCB[0].dw_FDB_LogAdd          = gs_DIRtable[tIndex].dw_FDB_LogAdd;
    //gs_DIR_FCB[0].w_FDB_Offset           = gs_DIRtable[tIndex].w_FDB_Offset;
 
    gs_File_FCB[0].dw_FDB_StartCluster = gs_DIRtable[tIndex].dw_File_StartCluster;

    // REVISIT!!!
    // Update the FCB?

    // Update the index in the dir table.
    gw_DIRtableIndex = tIndex;

	//USER_Read2KBReserveBlock_DIR(gw_DIRtableIndex);   
    gc_RepPlayMode = C_InDirPlay;
    gc_PhaseInx = C_PlayNext;
    //play_next();
}

