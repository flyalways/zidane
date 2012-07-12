#include "..\Header\SPDA2K.h"
#include "..\header\dos\fs_struct.h"
#include "..\header\Memalloc.h"
#include "..\header\variables.h"


extern xdata UBYTE gc_RepPlayMode;
extern xdata System_Struct gs_System_State;

extern bit	gb_DirPlay_Flag;
extern xdata U8	 gc_PlayModeChange_Flag;
extern xdata U8  gc_RepPlayMode_Dir;
extern xdata U8  gc_RepPlayMode_Nor;
extern xdata U8  gc_CurrentHZK;

U8 Read_LogData(void);
U8 Write_LogData(void);
U8 Get_LogData_PageIndex(void);
U8 USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);//use reserveblock for log
void USER_FillSettingInfo(void);
void USER_GetSettingInfo(void);

extern	xdata	U8	gc_LanguageSel;
extern	xdata	U8	LanguageTable[];

U8 Get_LogData_PageIndex(void)  
{
	U8 	data tc_i;
	U8  EmptyPageExists;
	U16	data tw_j;
	
	gc_UIblock0_index=0;	//initial
	EmptyPageExists=1;		//initial, 假設有空的page		

	for(tc_i=gw_PagesPerBlock-1;tc_i>=0;tc_i--)//由後往前檢查所有的pages,找出最近一個使用的page
	{
		UserData_Block_Read(0,tc_i*gc_InAddrMax,1,gc_PlayRecordDataBufHB);	//block 0,sector tc_i*G_Card_PageSize , 1 sector(s), source address is 0xAA00
		if(gc_PlayRecordDataBuf[508] != 0xFF)
		{//check 1 byte first, 不是空的sector
			EmptyPageExists=0;	//flag : this sector is not empty		
			break;//不是空的即跳出回圈
		}
		else
		{//檢查1 byte後為空的,再確認其他bytes是否為0xFF
			for (tw_j=0;tw_j<512;tw_j++)
			{//check all bytes
				if (gc_PlayRecordDataBuf[tw_j]!=0xff)
				{//not empty sector
					EmptyPageExists=0;	//flag : this sector is not empty
					break;	//其他byte不需要再檢查
				}

			}
		}

		if (!EmptyPageExists)
		{//un-empty page found
//			dbprintf("page isn't empty\n");
			break;	//停止檢查其他pages
		}
		

		if (tc_i==0)
		{//last checked page,表示整個block都是空的
			break;	//avoid tc_i=0-1=0xFF
	}
	
	}//end,for(tc_i=gc_PagesPerBlock-1;tc_i>=0;tc_i--)

	if (EmptyPageExists)
	{//all are empty pages(tc_i=0~127 & EmptyPageExists=1)
		gc_UIblock0_index =tc_i-1;	//
	}
	else
	{//found 有資料的page(tc_i=0 & EmptyPageExists=0)
		gc_UIblock0_index =tc_i;//
	}

//	dbprintf("last LogData at page %bx \n",gc_UIblock0_index);	

	return 0;
}


U8 Read_LogData(void)  
{
	bit	tb_ReadWriteDataArea;
	
	tb_ReadWriteDataArea=gb_ReadWriteDataArea;
	gb_ReadWriteDataArea = 0;
	
	EA=0;

	if (gc_UIblock0_index==0xFF)
	{//1st page是空的
	}
	else
	{
		UserData_Block_Read(0,gc_UIblock0_index*gc_InAddrMax,1,gc_PlayRecordDataBufHB);	//block 0,sector gc_UIblock0_index*G_Card_PageSize , 1 sector(s), source address is 0xAA00
	}

	EA=1;
	gb_ReadWriteDataArea=tb_ReadWriteDataArea;	//restore
	return 0;
}	

U8 Write_LogData(void)  
{
	bit	tb_ReadWriteDataArea;

	tb_ReadWriteDataArea=gb_ReadWriteDataArea;
	gb_ReadWriteDataArea = 0;

	if(gc_UIblock0_index==(gw_PagesPerBlock-1))//already write last page,gc_UIblock0_index為上一筆資訊
	{//如果上一筆已經是最後一個page -> erase 1 block

		UserData_Block_Erase(0);	
		gc_UIblock0_index=0;	//從sector 0開始寫
	}
	else if (gc_UIblock0_index==0xFF)
	{//1st page是空的
		gc_UIblock0_index=0;	//從sector 0開始寫
	}
	else
	{
		gc_UIblock0_index=gc_UIblock0_index+1;	//從最後一page有資料的下一page開始寫
	}	

	//mark LogDATA valid
	gc_PlayRecordDataBuf[508] = 'L'	;	//0x4c
	gc_PlayRecordDataBuf[509] = 'O' ;	//0x4f
	gc_PlayRecordDataBuf[510] = 'G'	;	//0x47	
	gc_PlayRecordDataBuf[511] = 'D'	;	//0x44

	EA=0;
	UserData_Block_Write(0,gc_UIblock0_index*gc_InAddrMax,1,gc_PlayRecordDataBufHB);	//block 0,sector gc_UIblock0_index*G_Card_PageSize , 1 sector(s), source address is 0xAA00
	EA=1;
	gb_ReadWriteDataArea=tb_ReadWriteDataArea;	//restore buffer assignment
	return 0;
}


void USER_GetSettingInfo(void)
{
	gs_DSP_GLOBAL_RAM.sw_Volume = gc_PlayRecordDataBuf[0];
	gs_DSP_GLOBAL_RAM.sc_EQ_Type = gc_PlayRecordDataBuf[1];
	gc_RepPlayMode=gc_PlayRecordDataBuf[2];
//	gc_RepPlayMode = gc_RepPlayMode;
	gc_LanguageSel=gc_PlayRecordDataBuf[22];
	gc_Contrast = gc_PlayRecordDataBuf[39];
	if(gc_LanguageSel>17)
	{
		gc_LanguageSel=HZK_LANGUAGE;
	}
	gc_MenuHZK=gc_LanguageSel;//LanguageTable[gc_LanguageSel];//sunzhk modify

	gw_FM_frequency=((U16)(gc_PlayRecordDataBuf[3])<<8) + (U16)gc_PlayRecordDataBuf[4];

	if(gc_PlayModeChange_Flag==0)
	{
		if(gc_PlayRecordDataBuf[20]<2)
		{
			gb_DirPlay_Flag = gc_PlayRecordDataBuf[20];
		}
		else
		{
			gb_DirPlay_Flag=0;	
		}
	}

	gw_LogFileIndex=((U16)(gc_PlayRecordDataBuf[5])<<8) + (U16)gc_PlayRecordDataBuf[6];
	gw_SDLogFileIndex=((U16)(gc_PlayRecordDataBuf[7])<<8) + (U16)gc_PlayRecordDataBuf[8];
	gw_USBLogFileIndex=((U16)(gc_PlayRecordDataBuf[9])<<8) + (U16)gc_PlayRecordDataBuf[10];
	gw_DIRFileIndex=((U16)(gc_PlayRecordDataBuf[15])<<8) + (U16)gc_PlayRecordDataBuf[16];
	gdw_DIRFDB_StartCluster=((U32)(gc_PlayRecordDataBuf[11]) <<24) + ((U32)gc_PlayRecordDataBuf[12] <<16) + ((U32)gc_PlayRecordDataBuf[13] << 8) + (U32)gc_PlayRecordDataBuf[14];
	gw_DIRFileTotalNumber=((U16)(gc_PlayRecordDataBuf[18])<<8) + (U16)gc_PlayRecordDataBuf[19];

	gw_SDDIRFileIndex=((U16)(gc_PlayRecordDataBuf[27])<<8) + (U16)gc_PlayRecordDataBuf[28];  //20090216 chiayen add
	gdw_SDDIRFDB_StartCluster=((U32)(gc_PlayRecordDataBuf[23]) <<24) + ((U32)gc_PlayRecordDataBuf[24] <<16) + ((U32)gc_PlayRecordDataBuf[25] << 8) + (U32)gc_PlayRecordDataBuf[26];
	gw_SDDIRFileTotalNumber=((U16)(gc_PlayRecordDataBuf[29])<<8) + (U16)gc_PlayRecordDataBuf[30];

	gw_HostDIRFileIndex=((U16)(gc_PlayRecordDataBuf[35])<<8) + (U16)gc_PlayRecordDataBuf[36];  //20090216 chiayen add
	gdw_HostDIRFDB_StartCluster=((U32)(gc_PlayRecordDataBuf[31]) <<24) + ((U32)gc_PlayRecordDataBuf[32] <<16) + ((U32)gc_PlayRecordDataBuf[33] << 8) + (U32)gc_PlayRecordDataBuf[34];
	gw_HostDIRFileTotalNumber=((U16)(gc_PlayRecordDataBuf[37])<<8) + (U16)gc_PlayRecordDataBuf[38];

	if(gb_DirPlay_Flag==1)
	{
		if(gc_CurrentCard==0)  //20090216 chiayen add
		{
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[0] = gc_PlayRecordDataBuf[11];
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[1] = gc_PlayRecordDataBuf[12];
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[2] = gc_PlayRecordDataBuf[13];
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[3] = gc_PlayRecordDataBuf[14];

			((U8 *)(&gw_FileIndex[0]))[0] = gc_PlayRecordDataBuf[15];
			((U8 *)(&gw_FileIndex[0]))[1] = gc_PlayRecordDataBuf[16];
	
			((U8 *)(&gw_FileTotalNumber[0]))[0] = gc_PlayRecordDataBuf[18];
			((U8 *)(&gw_FileTotalNumber[0]))[1] = gc_PlayRecordDataBuf[19];
		}
		else if(gc_CurrentCard==2) //SD CARD
		{
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[0] = gc_PlayRecordDataBuf[23];
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[1] = gc_PlayRecordDataBuf[24];
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[2] = gc_PlayRecordDataBuf[25];
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[3] = gc_PlayRecordDataBuf[26];
			((U8 *)(&gw_FileIndex[0]))[0] = gc_PlayRecordDataBuf[27];
			((U8 *)(&gw_FileIndex[0]))[1] = gc_PlayRecordDataBuf[28];
			((U8 *)(&gw_FileTotalNumber[0]))[0] = gc_PlayRecordDataBuf[29];
			((U8 *)(&gw_FileTotalNumber[0]))[1] = gc_PlayRecordDataBuf[30];			
		}
		else if(gc_CurrentCard==5)  //HOST
		{
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[0] = gc_PlayRecordDataBuf[31];
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[1] = gc_PlayRecordDataBuf[32];
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[2] = gc_PlayRecordDataBuf[33];
			((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[3] = gc_PlayRecordDataBuf[34];
			((U8 *)(&gw_FileIndex[0]))[0] = gc_PlayRecordDataBuf[35];
			((U8 *)(&gw_FileIndex[0]))[1] = gc_PlayRecordDataBuf[36];
			((U8 *)(&gw_FileTotalNumber[0]))[0] = gc_PlayRecordDataBuf[37];
			((U8 *)(&gw_FileTotalNumber[0]))[1] = gc_PlayRecordDataBuf[38];
		}
	}
	gc_PlayModeChange_Flag=0;
}


void USER_FillSettingInfo(void)
{
	if(gb_DirPlay_Flag==0)
	{
		if(gc_CurrentCard==0)
		{
			gw_LogFileIndex=gw_FileIndex[0];	
		}
		else if(gc_CurrentCard==2)
		{
			gw_SDLogFileIndex=gw_FileIndex[0];		
		}
		else if(gc_CurrentCard==5)
		{
			gw_USBLogFileIndex=gw_FileIndex[0];		
		}
		gc_PlayRecordDataBuf[5] = (U8)(gw_LogFileIndex >>8);//Nand
		gc_PlayRecordDataBuf[6] = (U8) gw_LogFileIndex;//Nand		
		gc_PlayRecordDataBuf[7] = (U8)(gw_SDLogFileIndex >>8);//SD
		gc_PlayRecordDataBuf[8] = (U8) gw_SDLogFileIndex;//SD
		gc_PlayRecordDataBuf[9] = (U8)(gw_USBLogFileIndex >>8);//USB
		gc_PlayRecordDataBuf[10] = (U8)gw_USBLogFileIndex;//USB

		gc_PlayRecordDataBuf[11] = (U8)(gdw_DIRFDB_StartCluster >>24);
		gc_PlayRecordDataBuf[12] = (U8)(gdw_DIRFDB_StartCluster>>16);
		gc_PlayRecordDataBuf[13] = (U8)(gdw_DIRFDB_StartCluster>>8);
		gc_PlayRecordDataBuf[14] = (U8) gdw_DIRFDB_StartCluster;

		gc_PlayRecordDataBuf[15] = (U8)(gw_DIRFileIndex >>8);
		gc_PlayRecordDataBuf[16] = (U8) gw_DIRFileIndex ;

		gc_PlayRecordDataBuf[18] = (U8)(gw_DIRFileTotalNumber >>8);
		gc_PlayRecordDataBuf[19] = (U8) gw_DIRFileTotalNumber ;

		gc_PlayRecordDataBuf[23] = (U8)(gdw_SDDIRFDB_StartCluster >>24);  //20090216 chiayen add
		gc_PlayRecordDataBuf[24] = (U8)(gdw_SDDIRFDB_StartCluster>>16);
		gc_PlayRecordDataBuf[25] = (U8)(gdw_SDDIRFDB_StartCluster>>8);
		gc_PlayRecordDataBuf[26] = (U8) gdw_SDDIRFDB_StartCluster;
		gc_PlayRecordDataBuf[27] = (U8)(gw_SDDIRFileIndex >>8);
		gc_PlayRecordDataBuf[28] = (U8) gw_SDDIRFileIndex;
		gc_PlayRecordDataBuf[29] = (U8)(gw_SDDIRFileTotalNumber >>8);
		gc_PlayRecordDataBuf[30] = (U8) gw_SDDIRFileTotalNumber;

		gc_PlayRecordDataBuf[31] = (U8)(gdw_HostDIRFDB_StartCluster >>24);
		gc_PlayRecordDataBuf[32] = (U8)(gdw_HostDIRFDB_StartCluster>>16);
		gc_PlayRecordDataBuf[33] = (U8)(gdw_HostDIRFDB_StartCluster>>8);
		gc_PlayRecordDataBuf[34] = (U8) gdw_HostDIRFDB_StartCluster;
		gc_PlayRecordDataBuf[35] = (U8)(gw_HostDIRFileIndex >>8);
		gc_PlayRecordDataBuf[36] = (U8) gw_HostDIRFileIndex;
		gc_PlayRecordDataBuf[37] = (U8)(gw_HostDIRFileTotalNumber >>8);
		gc_PlayRecordDataBuf[38] = (U8) gw_HostDIRFileTotalNumber;  //20090216 chiayen add
	}
	else	// Folder
	{
		gc_PlayRecordDataBuf[5] = (U8)(gw_LogFileIndex >>8);//Nand
		gc_PlayRecordDataBuf[6] = (U8) gw_LogFileIndex;//Nand		
		gc_PlayRecordDataBuf[7] = (U8)(gw_SDLogFileIndex >>8);//SD
		gc_PlayRecordDataBuf[8] = (U8) gw_SDLogFileIndex;//SD
		gc_PlayRecordDataBuf[9] = (U8)(gw_USBLogFileIndex >>8);//USB
		gc_PlayRecordDataBuf[10] = (U8)gw_USBLogFileIndex;//USB	

		gc_PlayRecordDataBuf[11] = (U8)(gdw_DIRFDB_StartCluster >>24);  //20090216 chiayen add
		gc_PlayRecordDataBuf[12] = (U8)(gdw_DIRFDB_StartCluster>>16);
		gc_PlayRecordDataBuf[13] = (U8)(gdw_DIRFDB_StartCluster>>8);
		gc_PlayRecordDataBuf[14] = (U8) gdw_DIRFDB_StartCluster;

		gc_PlayRecordDataBuf[15] = (U8)(gw_DIRFileIndex >>8);
		gc_PlayRecordDataBuf[16] = (U8) gw_DIRFileIndex ;

		gc_PlayRecordDataBuf[18] = (U8)(gw_DIRFileTotalNumber >>8);
		gc_PlayRecordDataBuf[19] = (U8) gw_DIRFileTotalNumber ;
		gc_PlayRecordDataBuf[23] = (U8)(gdw_SDDIRFDB_StartCluster >>24);
		gc_PlayRecordDataBuf[24] = (U8)(gdw_SDDIRFDB_StartCluster>>16);
		gc_PlayRecordDataBuf[25] = (U8)(gdw_SDDIRFDB_StartCluster>>8);
		gc_PlayRecordDataBuf[26] = (U8) gdw_SDDIRFDB_StartCluster;
		gc_PlayRecordDataBuf[27] = (U8)(gw_SDDIRFileIndex >>8);
		gc_PlayRecordDataBuf[28] = (U8) gw_SDDIRFileIndex;
		gc_PlayRecordDataBuf[29] = (U8)(gw_SDDIRFileTotalNumber >>8);
		gc_PlayRecordDataBuf[30] = (U8) gw_SDDIRFileTotalNumber;

		gc_PlayRecordDataBuf[31] = (U8)(gdw_HostDIRFDB_StartCluster >>24);
		gc_PlayRecordDataBuf[32] = (U8)(gdw_HostDIRFDB_StartCluster>>16);
		gc_PlayRecordDataBuf[33] = (U8)(gdw_HostDIRFDB_StartCluster>>8);
		gc_PlayRecordDataBuf[34] = (U8) gdw_HostDIRFDB_StartCluster;
		gc_PlayRecordDataBuf[35] = (U8)(gw_HostDIRFileIndex >>8);
		gc_PlayRecordDataBuf[36] = (U8) gw_HostDIRFileIndex;
		gc_PlayRecordDataBuf[37] = (U8)(gw_HostDIRFileTotalNumber >>8);
		gc_PlayRecordDataBuf[38] = (U8) gw_HostDIRFileTotalNumber;  //20090216 chiayen add

		if(gc_CurrentCard==0)  //20090216 chiayen add
		{
			gc_PlayRecordDataBuf[11] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[0];
			gc_PlayRecordDataBuf[12] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[1];
			gc_PlayRecordDataBuf[13] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[2];
			gc_PlayRecordDataBuf[14] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[3];

			gc_PlayRecordDataBuf[15] = ((U8 *)(&gw_FileIndex[0]))[0];
			gc_PlayRecordDataBuf[16] = ((U8 *)(&gw_FileIndex[0]))[1];
		
			gc_PlayRecordDataBuf[17] = gs_File_FCB[0].c_FileType;
			gc_PlayRecordDataBuf[18] = ((U8 *)(&gw_FileTotalNumber[0]))[0];
			gc_PlayRecordDataBuf[19] = ((U8 *)(&gw_FileTotalNumber[0]))[1];	
		}
		else if(gc_CurrentCard==2)  //20090216 chiayen add
		{
			gc_PlayRecordDataBuf[23] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[0];
			gc_PlayRecordDataBuf[24] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[1];
			gc_PlayRecordDataBuf[25] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[2];
			gc_PlayRecordDataBuf[26] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[3];
			gc_PlayRecordDataBuf[27] = ((U8 *)(&gw_FileIndex[0]))[0];
			gc_PlayRecordDataBuf[28] = ((U8 *)(&gw_FileIndex[0]))[1];
			gc_PlayRecordDataBuf[29] = ((U8 *)(&gw_FileTotalNumber[0]))[0];
			gc_PlayRecordDataBuf[30] = ((U8 *)(&gw_FileTotalNumber[0]))[1];		
		}
		else if(gc_CurrentCard==5)  //20090216 chiayen add
		{
			gc_PlayRecordDataBuf[31] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[0];
			gc_PlayRecordDataBuf[32] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[1];
			gc_PlayRecordDataBuf[33] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[2];
			gc_PlayRecordDataBuf[34] = ((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[3];
			gc_PlayRecordDataBuf[35] = ((U8 *)(&gw_FileIndex[0]))[0];
			gc_PlayRecordDataBuf[36] = ((U8 *)(&gw_FileIndex[0]))[1];
			gc_PlayRecordDataBuf[37] = ((U8 *)(&gw_FileTotalNumber[0]))[0];
			gc_PlayRecordDataBuf[38] = ((U8 *)(&gw_FileTotalNumber[0]))[1];			
		}	
	}
	gc_PlayRecordDataBuf[0] = gs_DSP_GLOBAL_RAM.sw_Volume;//Vol
	gc_PlayRecordDataBuf[1] = gs_DSP_GLOBAL_RAM.sc_EQ_Type;//EQ
	gc_PlayRecordDataBuf[2] = gc_RepPlayMode;
	gc_PlayRecordDataBuf[3] = (U8)(gw_FM_frequency >>8);
	gc_PlayRecordDataBuf[4] = (U8) gw_FM_frequency;

	gc_PlayRecordDataBuf[20] = gb_DirPlay_Flag;
	gc_PlayRecordDataBuf[22] = gc_LanguageSel;
	gc_PlayRecordDataBuf[39] = gc_Contrast;
}


U8 USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite)//use reserveblock for log
{
	if(tbt_ReadOrWrite==0)//read log
	{
		Read_LogData();
		USER_GetSettingInfo();
	}
	else
	{
		USER_FillSettingInfo();
		Write_LogData();
	}
	return 0;
}

