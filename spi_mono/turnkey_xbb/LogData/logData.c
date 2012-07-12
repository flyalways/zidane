#include "..\Header\SPDA2K.h"
#include "..\header\variables.h"


void Get_LogData_PageIndex(void)  
{
	data	bit	tb_ReadWriteDataArea;
	data	U8	tc_i;
	data	U8  EmptyPageExists;
	
	gc_UIblock0_index=0;	//initial
	EmptyPageExists=0;		//initial	

	tb_ReadWriteDataArea=gb_ReadWriteDataArea;
	gb_ReadWriteDataArea = 0;
	for(tc_i=0;tc_i<128;tc_i++)//檢查所有的pages,找出最近一個使用所在的page
	{
		SPI_UserData_Block_Read(tc_i,gc_PlayRecordDataBufHB);	

		//檢查log file的標記,需滿足以下條件
//		gc_PlayRecordDataBuf[508] = 'L'	;
//		gc_PlayRecordDataBuf[509] = 'O' ;
//		gc_PlayRecordDataBuf[510] = 'G'	;	
//		gc_PlayRecordDataBuf[511] = 'F'	;

		if(gc_PlayRecordDataBuf[508] == 0xFF)
		{//0xFF表示還沒有寫入
			EmptyPageExists=1;
			break;
		}
		if (tc_i==127)
		{
			break;
		}
	}
	
	gb_ReadWriteDataArea=tb_ReadWriteDataArea;		//restore gc_ReadWriteDataArea
	if(EmptyPageExists)
	{//
		gc_UIblock0_index=tc_i-1;	//紀錄目前最後一筆log所在的page?
	}
	else
	{//都沒有空的page
		gc_UIblock0_index=tc_i;		//最後一筆log所在的page在最後一個page
	}
}


void Read_LogData(void)  
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
		SPI_UserData_Block_Read(gc_UIblock0_index,gc_PlayRecordDataBufHB);	//block 0,sector gc_UIblock0_index*G_Card_PageSize , 1 sector(s), source address is 0xAA00
	}
	EA=1;
	gb_ReadWriteDataArea=tb_ReadWriteDataArea;	//restore
}	
	
	
void Write_LogData(void)  
{
	bit	tb_ReadWriteDataArea;

	tb_ReadWriteDataArea=gb_ReadWriteDataArea;
	gb_ReadWriteDataArea=0;

	if(gc_UIblock0_index==127)//already write last page,gc_UIblock0_index為上一筆資訊
	{//如果上一筆已經是最後一個page -> erase 1 block
		SPI_UserData_Block_Erase();	
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
	gc_PlayRecordDataBuf[508]='L';
	gc_PlayRecordDataBuf[509]='O';
	gc_PlayRecordDataBuf[510]='G';
	gc_PlayRecordDataBuf[511]='D';

	EA=0;
	SPI_UserData_Block_Write(gc_UIblock0_index,gc_PlayRecordDataBufHB);
	EA=1;
	gb_ReadWriteDataArea=tb_ReadWriteDataArea;	//restore buffer assignment
}


void USER_GetSettingInfo(void)
{
	xdata	U8 i;

	if(gc_PlayRecordDataBuf[249]!='M'||gc_PlayRecordDataBuf[250]!='A'||gc_PlayRecordDataBuf[251]!='X')
	{
		gs_DSP_GLOBAL_RAM.sc_Volume=C_VolumeDefault;
		gs_DSP_GLOBAL_RAM.sc_EQ_Type=0;
		gc_RepPlayMode=0;
		gc_LCMBacklightTime=0;
		gc_RecordQuality=2;
		gc_LastCard=gc_CurrentCard;
		gb_AlarmON=0;
		gb_AlarmRepeatON=0;
		gc_AlarmDaysSelect=0x1F;
		gb_OrderRecordConfirm=0;
		gc_CurrentHZK=HZK_LANGUAGE;
		gw_OrderRecDuration=1800;
		gdw_OrderRecordTime=43200;	// 12:00
		gdw_AlarmTime=43200;		// 12:00
		gc_FM_CHAmount=0;
		gc_FM_CH_COUNT=0;
		gw_FM_CH[0]=875;
		gw_FM_frequency=875;
		gw_FileIndex[0]=1;
		gw_FileTotalNumber[0]=1;
		gw_CurrentFolderNum=1;
		gs_File_FCB[0].dw_FDB_StartCluster=gdw_DOS_RootDirClus;
		return;
	}
	gs_DSP_GLOBAL_RAM.sc_Volume=gc_PlayRecordDataBuf[0];	// Vol
	gs_DSP_GLOBAL_RAM.sc_EQ_Type=gc_PlayRecordDataBuf[1];	// EQ
	gc_RepPlayMode=gc_PlayRecordDataBuf[2];					// Play Mode
	gc_LCMBacklightTime=gc_PlayRecordDataBuf[3];			// LCM Backlight Time
	gc_RecordQuality=gc_PlayRecordDataBuf[4];				// Record Quality
	if(gc_LastCard==0)	// Only Power-On is available
	{
		gc_LastCard=gc_PlayRecordDataBuf[5];				// Last Card Of Play
	}
	gb_AlarmON=(bit)gc_PlayRecordDataBuf[6];				// Alarm Clock ON/OFF
	gb_AlarmRepeatON=(bit)gc_PlayRecordDataBuf[7];			// Alarm Clcok Repeat ON/OFF
	gc_AlarmDaysSelect=gc_PlayRecordDataBuf[8];				// Alarm Clock Repeat Days Option
	gb_OrderRecordConfirm=(bit)gc_PlayRecordDataBuf[9];		// Order Record ON/OFF
	gc_CurrentHZK=gc_PlayRecordDataBuf[10];

	((U8*)(&gw_FM_frequency))[0]=gc_PlayRecordDataBuf[11];		// FM Frequency
	((U8*)(&gw_FM_frequency))[1]=gc_PlayRecordDataBuf[12];
	((U8 *)(&gw_OrderRecDuration))[0]=gc_PlayRecordDataBuf[13];	// Order Record Duration
	((U8 *)(&gw_OrderRecDuration))[1]=gc_PlayRecordDataBuf[14];
	((U8 *)(&gdw_OrderRecordTime))[0]=gc_PlayRecordDataBuf[15];	// Order Record Time
	((U8 *)(&gdw_OrderRecordTime))[1]=gc_PlayRecordDataBuf[16];
	((U8 *)(&gdw_OrderRecordTime))[2]=gc_PlayRecordDataBuf[17];
	((U8 *)(&gdw_OrderRecordTime))[3]=gc_PlayRecordDataBuf[18];
	((U8 *)(&gdw_AlarmTime))[0]=gc_PlayRecordDataBuf[19];		// Alarm Clock Time
	((U8 *)(&gdw_AlarmTime))[1]=gc_PlayRecordDataBuf[20];
	((U8 *)(&gdw_AlarmTime))[2]=gc_PlayRecordDataBuf[21];
	((U8 *)(&gdw_AlarmTime))[3]=gc_PlayRecordDataBuf[22];

	gc_FM_CHAmount=gc_PlayRecordDataBuf[23];
	gc_FM_CH_COUNT=gc_PlayRecordDataBuf[24];

	((U8 *)(&gdw_SDDIRFDB_StartCluster))[0]=gc_PlayRecordDataBuf[30];
	((U8 *)(&gdw_SDDIRFDB_StartCluster))[1]=gc_PlayRecordDataBuf[31];
	((U8 *)(&gdw_SDDIRFDB_StartCluster))[2]=gc_PlayRecordDataBuf[32];
	((U8 *)(&gdw_SDDIRFDB_StartCluster))[3]=gc_PlayRecordDataBuf[33];
	((U8 *)(&gw_SDDIRFileIndex))[0]=gc_PlayRecordDataBuf[34];
	((U8 *)(&gw_SDDIRFileIndex))[1]=gc_PlayRecordDataBuf[35];
	((U8 *)(&gw_SDDIRFileTotalNumber))[0]=gc_PlayRecordDataBuf[36];
	((U8 *)(&gw_SDDIRFileTotalNumber))[1]=gc_PlayRecordDataBuf[37];
	((U8 *)(&gw_SDFolderIndex))[0]=gc_PlayRecordDataBuf[38];
	((U8 *)(&gw_SDFolderIndex))[1]=gc_PlayRecordDataBuf[39];

	((U8 *)(&gdw_HostDIRFDB_StartCluster))[0]=gc_PlayRecordDataBuf[40];
	((U8 *)(&gdw_HostDIRFDB_StartCluster))[1]=gc_PlayRecordDataBuf[41];
	((U8 *)(&gdw_HostDIRFDB_StartCluster))[2]=gc_PlayRecordDataBuf[42];
	((U8 *)(&gdw_HostDIRFDB_StartCluster))[3]=gc_PlayRecordDataBuf[43];
	((U8 *)(&gw_HostDIRFileIndex))[0]=gc_PlayRecordDataBuf[44];
	((U8 *)(&gw_HostDIRFileIndex))[1]=gc_PlayRecordDataBuf[45];
	((U8 *)(&gw_HostDIRFileTotalNumber))[0]=gc_PlayRecordDataBuf[46];
	((U8 *)(&gw_HostDIRFileTotalNumber))[1]=gc_PlayRecordDataBuf[47];
	((U8 *)(&gw_HostFolderIndex))[0]=gc_PlayRecordDataBuf[48];
	((U8 *)(&gw_HostFolderIndex))[1]=gc_PlayRecordDataBuf[49];

	if(gc_CurrentCard==2) //SD CARD
	{
		((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[0]=gc_PlayRecordDataBuf[30];
		((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[1]=gc_PlayRecordDataBuf[31];
		((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[2]=gc_PlayRecordDataBuf[32];
		((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[3]=gc_PlayRecordDataBuf[33];
		((U8 *)(&gw_FileIndex[0]))[0]=gc_PlayRecordDataBuf[34];
		((U8 *)(&gw_FileIndex[0]))[1]=gc_PlayRecordDataBuf[35];		
		((U8 *)(&gw_FileTotalNumber[0]))[0]=gc_PlayRecordDataBuf[36];
		((U8 *)(&gw_FileTotalNumber[0]))[1]=gc_PlayRecordDataBuf[37];
		((U8 *)(&gw_CurrentFolderNum))[0]=(U16)gc_PlayRecordDataBuf[38];
		((U8 *)(&gw_CurrentFolderNum))[1]=(U16)gc_PlayRecordDataBuf[39];
	}
	else if(gc_CurrentCard==5)  //HOST
	{
		((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[0]=gc_PlayRecordDataBuf[40];
		((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[1]=gc_PlayRecordDataBuf[41];
		((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[2]=gc_PlayRecordDataBuf[42];
		((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[3]=gc_PlayRecordDataBuf[43];
		((U8 *)(&gw_FileIndex[0]))[0]=gc_PlayRecordDataBuf[44];
		((U8 *)(&gw_FileIndex[0]))[1]=gc_PlayRecordDataBuf[45];
		((U8 *)(&gw_FileTotalNumber[0]))[0]=gc_PlayRecordDataBuf[46];
		((U8 *)(&gw_FileTotalNumber[0]))[1]=gc_PlayRecordDataBuf[47];
		((U8 *)(&gw_CurrentFolderNum))[0]=(U16)gc_PlayRecordDataBuf[48];
		((U8 *)(&gw_CurrentFolderNum))[1]=(U16)gc_PlayRecordDataBuf[49];
	}

	for(i=0;i<30;i++)
	{
		((U8 *)(&gw_FM_CH[i]))[0]=gc_PlayRecordDataBuf[60+i*2];
		((U8 *)(&gw_FM_CH[i]))[1]=gc_PlayRecordDataBuf[61+i*2];
	}
}


void USER_FillSettingInfo(void)
{
	xdata	U8 i;

	gc_PlayRecordDataBuf[ 0]=gs_DSP_GLOBAL_RAM.sc_Volume;		// Vol
	gc_PlayRecordDataBuf[ 1]=gs_DSP_GLOBAL_RAM.sc_EQ_Type;		// EQ
	gc_PlayRecordDataBuf[ 2]=gc_RepPlayMode;					// Play Mode
	gc_PlayRecordDataBuf[ 3]=gc_LCMBacklightTime;				// LCM Backlight
	gc_PlayRecordDataBuf[ 4]=gc_RecordQuality;					// Record Quality
	gc_PlayRecordDataBuf[ 5]=gc_LastCard;						// Last Card Of Play
	gc_PlayRecordDataBuf[ 6]=(U8)gb_AlarmON;					// Alarm Clock ON/OFF
	gc_PlayRecordDataBuf[ 7]=(U8)gb_AlarmRepeatON;				// Alarm Clcok Repeat ON/OFF
	gc_PlayRecordDataBuf[ 8]=gc_AlarmDaysSelect;				// Alarm Clock Repeat Days Option
	gc_PlayRecordDataBuf[ 9]=(U8)gb_OrderRecordConfirm;			// Order Record ON/OFF
	gc_PlayRecordDataBuf[10]=gc_CurrentHZK;						// Language Option

	gc_PlayRecordDataBuf[11]=((U8 *)(&gw_FM_frequency))[0];		// FM Frequency
	gc_PlayRecordDataBuf[12]=((U8 *)(&gw_FM_frequency))[1];
	gc_PlayRecordDataBuf[13]=((U8 *)(&gw_OrderRecDuration))[0];	// Order Record Duration
	gc_PlayRecordDataBuf[14]=((U8 *)(&gw_OrderRecDuration))[1];
	gc_PlayRecordDataBuf[15]=((U8 *)(&gdw_OrderRecordTime))[0];	// Order Record Time
	gc_PlayRecordDataBuf[16]=((U8 *)(&gdw_OrderRecordTime))[1];
	gc_PlayRecordDataBuf[17]=((U8 *)(&gdw_OrderRecordTime))[2];
	gc_PlayRecordDataBuf[18]=((U8 *)(&gdw_OrderRecordTime))[3];
	gc_PlayRecordDataBuf[19]=((U8 *)(&gdw_AlarmTime))[0];		// Alarm Clock Time
	gc_PlayRecordDataBuf[20]=((U8 *)(&gdw_AlarmTime))[1];
	gc_PlayRecordDataBuf[21]=((U8 *)(&gdw_AlarmTime))[2];
	gc_PlayRecordDataBuf[22]=((U8 *)(&gdw_AlarmTime))[3];

	gc_PlayRecordDataBuf[23]=gc_FM_CHAmount;					// FM Station Memory Counter
	gc_PlayRecordDataBuf[24]=gc_FM_CH_COUNT;					// FM Playing Station Number

	gc_PlayRecordDataBuf[30]=((U8 *)(&gdw_SDDIRFDB_StartCluster))[0];
	gc_PlayRecordDataBuf[31]=((U8 *)(&gdw_SDDIRFDB_StartCluster))[1];
	gc_PlayRecordDataBuf[32]=((U8 *)(&gdw_SDDIRFDB_StartCluster))[2];
	gc_PlayRecordDataBuf[33]=((U8 *)(&gdw_SDDIRFDB_StartCluster))[3];
	gc_PlayRecordDataBuf[34]=((U8 *)(&gw_SDDIRFileIndex))[0];
	gc_PlayRecordDataBuf[35]=((U8 *)(&gw_SDDIRFileIndex))[1];
	gc_PlayRecordDataBuf[36]=((U8 *)(&gw_SDDIRFileTotalNumber))[0];
	gc_PlayRecordDataBuf[37]=((U8 *)(&gw_SDDIRFileTotalNumber))[1];
	gc_PlayRecordDataBuf[38]=((U8 *)(&gw_SDFolderIndex))[0];
	gc_PlayRecordDataBuf[39]=((U8 *)(&gw_SDFolderIndex))[1];

	gc_PlayRecordDataBuf[40]=((U8 *)(&gdw_HostDIRFDB_StartCluster))[0];
	gc_PlayRecordDataBuf[41]=((U8 *)(&gdw_HostDIRFDB_StartCluster))[1];
	gc_PlayRecordDataBuf[42]=((U8 *)(&gdw_HostDIRFDB_StartCluster))[2];
	gc_PlayRecordDataBuf[43]=((U8 *)(&gdw_HostDIRFDB_StartCluster))[3];
	gc_PlayRecordDataBuf[44]=((U8 *)(&gw_HostDIRFileIndex))[0];
	gc_PlayRecordDataBuf[45]=((U8 *)(&gw_HostDIRFileIndex))[1];
	gc_PlayRecordDataBuf[46]=((U8 *)(&gw_HostDIRFileTotalNumber))[0];
	gc_PlayRecordDataBuf[47]=((U8 *)(&gw_HostDIRFileTotalNumber))[1];
	gc_PlayRecordDataBuf[48]=((U8 *)(&gw_HostFolderIndex))[0];
	gc_PlayRecordDataBuf[49]=((U8 *)(&gw_HostFolderIndex))[1];

	if(gc_CurrentCard==2)  // SD
	{
		gc_PlayRecordDataBuf[30]=((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[0];
		gc_PlayRecordDataBuf[31]=((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[1];
		gc_PlayRecordDataBuf[32]=((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[2];
		gc_PlayRecordDataBuf[33]=((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[3];
		gc_PlayRecordDataBuf[34]=((U8 *)(&gw_FileIndex[0]))[0];
		gc_PlayRecordDataBuf[35]=((U8 *)(&gw_FileIndex[0]))[1];
		gc_PlayRecordDataBuf[36]=((U8 *)(&gw_FileTotalNumber[0]))[0];
		gc_PlayRecordDataBuf[37]=((U8 *)(&gw_FileTotalNumber[0]))[1];	
		gc_PlayRecordDataBuf[38]=((U8 *)(&gw_CurrentFolderNum))[0];
		gc_PlayRecordDataBuf[39]=((U8 *)(&gw_CurrentFolderNum))[1];
		gdw_SDDIRFDB_StartCluster=gs_File_FCB[0].dw_FDB_StartCluster;
		gw_SDDIRFileIndex=gw_FileIndex[0];
		gw_SDDIRFileTotalNumber=gw_FileTotalNumber[0];
		gw_SDFolderIndex=gw_CurrentFolderNum;
	}
	else if(gc_CurrentCard==5)  // HOST
	{
		gc_PlayRecordDataBuf[40]=((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[0];
		gc_PlayRecordDataBuf[41]=((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[1];
		gc_PlayRecordDataBuf[42]=((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[2];
		gc_PlayRecordDataBuf[43]=((U8 *)(&gs_File_FCB[0].dw_FDB_StartCluster))[3];
		gc_PlayRecordDataBuf[44]=((U8 *)(&gw_FileIndex[0]))[0];
		gc_PlayRecordDataBuf[45]=((U8 *)(&gw_FileIndex[0]))[1];
		gc_PlayRecordDataBuf[46]=((U8 *)(&gw_FileTotalNumber[0]))[0];
		gc_PlayRecordDataBuf[47]=((U8 *)(&gw_FileTotalNumber[0]))[1];
		gc_PlayRecordDataBuf[48]=((U8 *)(&gw_CurrentFolderNum))[0];
		gc_PlayRecordDataBuf[49]=((U8 *)(&gw_CurrentFolderNum))[1];
		gdw_HostDIRFDB_StartCluster=gs_File_FCB[0].dw_FDB_StartCluster;
		gw_HostDIRFileIndex=gw_FileIndex[0];
		gw_HostDIRFileTotalNumber=gw_FileTotalNumber[0];
		gw_HostFolderIndex=gw_CurrentFolderNum;
	}

	for(i=0;i<30;i++)
	{
	 	gc_PlayRecordDataBuf[60+i*2]=((U8 *)(&gw_FM_CH[i]))[0];
		gc_PlayRecordDataBuf[61+i*2]=((U8 *)(&gw_FM_CH[i]))[1];
	}

	gc_PlayRecordDataBuf[249]='M';
	gc_PlayRecordDataBuf[250]='A';
	gc_PlayRecordDataBuf[251]='X';
}


void USER_LogFile_ReadWrite(bit tb_ReadOrWrite)	//0=Read 1=Write
{
	if(tb_ReadOrWrite==0)//read log
	{
		Read_LogData();
		USER_GetSettingInfo();
	}
	else
	{
		USER_FillSettingInfo();
		Write_LogData();
	}
}

