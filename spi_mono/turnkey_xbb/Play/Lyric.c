#include "..\header\SPDA2K.h"
#include "..\header\variables.h"


U8 Lyric_OffsetLRC()
{
	gw_LrcOffset++;
	gdw_LrcFilePointer++;
	if(gdw_LrcFilePointer>=gdw_LrcFileSize)
	{
		return 0;
	}	
	if(gw_LrcOffset==512)
	{
		gw_LrcOffset=0;
		if(!Lyric_ReadLrcFile(1))
		{
			return 0;
	  	}
	}
	return 1;	
}

void Lyric_GetLyrics(void)
{
	data	U8 i;

	i=0;
	Lyric_ReadLrcFile(0);

	gw_LrcOffset=(gdw_LrcFilePointer&0x1ff);
	while(gc_PlayRecordDataBuf[gw_LrcOffset]!=0x0a)//(JC)not <LF>, fill 0x0d to lyric buf, it will cause carriage return
	{
		gc_LrcDisplayBuf[i]=gc_PlayRecordDataBuf[gw_LrcOffset];
		i++;
		if(i>=LRC_DISPLAY_LYRICS_CHARNUMBER)//(JC)max char numbers of piece of lyrics for a time stamp 
		{
			break;
		}

		gw_LrcOffset++;
		gdw_LrcFilePointer++;
		if(gdw_LrcFilePointer >= gdw_LrcFileSize)
		{
			break;
		}	
		if(gw_LrcOffset==512)
		{
			gw_LrcOffset=0;
			if(!Lyric_ReadLrcFile(1))
			{
				break;
			}
		}				
	}
	gc_LrcCurrentLen=i;
}


void Lyric_GetTimeStamp(void)
{
	data	U8 i;
	data	U8 Time_Char[5];

	Time_Char[0]=(gw_Disp_CurrentSec/60)/10+0x30;
	Time_Char[1]=((gw_Disp_CurrentSec/60)%10)+0x30;
	Time_Char[2]=':';
	Time_Char[3]=(gw_Disp_CurrentSec%60)/10+0x30;
	Time_Char[4]=(gw_Disp_CurrentSec%10)+0x30;
	gdw_LrcFileCurrentCluster=gdw_LrcFileStartCluster;
	gw_LrcOffset=0;
	gdw_LrcFilePointer=0;	
	gb_LrcGetTimeOrNot=0;
	Lyric_ReadLrcFile(0);
	
	while(1)
	{
		if(gc_PlayRecordDataBuf[gw_LrcOffset]=='[')
		{
			for(i=0;i<5;i++)
			{
				if(Lyric_OffsetLRC()==0)
				{
					return;
				}

				if(Time_Char[i]!=gc_PlayRecordDataBuf[gw_LrcOffset])
				{
					break;
				}
			}
			if(i>=5)//(JC)matching timestamp 
			{
				while(1)
				{
					while(gc_PlayRecordDataBuf[gw_LrcOffset]!=']')//(JC)bypass the decimal point and digits rite to it
					{	
						if(Lyric_OffsetLRC()==0)
						{
							return;	
						}
					}

					if(Lyric_OffsetLRC()==0)//(JC)Bypass ']'
					{
						return;		
					}

					if(gc_PlayRecordDataBuf[gw_LrcOffset]!='[')
					{	
						gb_LrcGetTimeOrNot=1;//(JC)matching time stamp, following the lyrics
						return;	
					}
				}
			}
		}

		if(Lyric_OffsetLRC()==0)
		{
			return;	
		}
	}
}


U8 Lyric_FileSearch()
{
	data	U8	tc_Status;
 	data	U8	tc_TempBuff[3];
	data	U16	tw_Lenth;

	{
		gs_File_FCB[2].dw_FDB_StartCluster=gs_File_FCB[0].dw_FDB_StartCluster;
		if (!gc_FileLongName[2])
		{//0:long			
			EXT_NameC[0]=1;		
			((U8 *)(&tw_Lenth))[0] = gc_FileLongName[3];
			((U8 *)(&tw_Lenth))[1] = gc_FileLongName[4];
			tw_Lenth+=5;	
				
			tc_TempBuff[0] = gc_FileLongName[tw_Lenth-1];
			tc_TempBuff[1] = gc_FileLongName[tw_Lenth-3];			
			tc_TempBuff[2]=gc_FileLongName[tw_Lenth-5];			
			gc_FileLongName[tw_Lenth-5]='L';
			gc_FileLongName[tw_Lenth-3]='R';
			gc_FileLongName[tw_Lenth-1]='C';
			tc_Status=DOS_Search_File(C_File_OneDir|C_By_LName,C_OtherFileType,C_CmpExtName|C_Next);			
		}
		else
		{		
			EXT_NameC[0]=1;
			EXT_NameC[1]='L';
			EXT_NameC[2]='R';
    		EXT_NameC[3]='C';
			tc_Status=DOS_Search_File(C_File_OneDir|C_By_SName,C_OtherFileType,C_CmpExtName|C_Next);
		}	
		
		if (!gc_FileLongName[2])//(JC)finish searching, restore file name
		{
			gc_FileLongName[tw_Lenth-1]=tc_TempBuff[0];
			gc_FileLongName[tw_Lenth-3]=tc_TempBuff[1]; 
			gc_FileLongName[tw_Lenth-5]=tc_TempBuff[2];
		}	
		
		if (tc_Status)
		{//no LRC file
			return 0;
		}
		else
		{//LRC file is exist	
			DOS_Open_File_r(C_OtherFHandle, C_Open_FoundFile, C_NullFileName);//get file_totalsize and startcluster
			gdw_LrcFileStartCluster = gs_File_FCB[2].dw_File_StartCluster;
			gdw_LrcFileSize = gs_File_FCB[2].dw_File_TotalSize;
			return 1;
		}
  	}
}

U8 Lyric_ReadLrcFile(U8 tc_ReadNextSector)
{
	data	U16 tdw_ResidualSector;
	xdata	U32 tdw_DataLogicAddr;
	xdata	U32 tdw_prevCluster;

	if(gdw_LrcFilePointer >= gdw_LrcFileSize)
	{
		return 0;
	}
	if((gdw_LrcFilePointer>>9)!=0)
	{
		if ((((gdw_LrcFilePointer>>9)&(gc_DOS_SectorPerCluster-1))==0) && tc_ReadNextSector)
		{
			tdw_prevCluster = gdw_LrcFileCurrentCluster;
			gdw_LrcFileCurrentCluster =	DOS_GetNextCluster(gdw_LrcFileCurrentCluster, 1);
			if (tdw_prevCluster	== gdw_LrcFileCurrentCluster || gdw_LrcFileCurrentCluster == 0 || gdw_LrcFileCurrentCluster >= 0x0ffffff8)
			{
				return 0;
			}
		}
	}
	tdw_ResidualSector = ((gdw_LrcFilePointer>>9))&(gc_DOS_SectorPerCluster-1);
	tdw_DataLogicAddr = DOS_ClusterLogicAddr(gdw_LrcFileCurrentCluster)+(tdw_ResidualSector);
	
	gb_ReadWriteDataArea = 0;
	return (!DOS_Read_LogicSector(tdw_DataLogicAddr));
}

