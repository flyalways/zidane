#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\header\variables.h"

extern UBYTE  xdata EXT_NameC[];

void Lyric_GetTimeStamp(void);
U8 Lyric_NextReadLrcFile(U8 tc_ReadNextSector);
void Lyric_NextGetLyrics(void);
U8 Lyric_OffsetLRC_Full();

void Lyric_GetTimeStamp(void)
{
	data	U8 i=0;
	U8 Time_Char[5];
	U16 temp_gw_LrcOffset;
	U32 temp_gdw_LrcFilePointer;	
	U16	tw_Lrcoffset=0;
	U32	tw_LrcFilePointer=0;	
	U16	temp_timecon=0; 		//tangwei add these varibales 081111
	U16	temp_timevar=0;
	data	U16	temp_timelow=0;
	data	bit	setflag=1;
	U8	timevar[5];
	U8	k=0;
	
	Time_Char[0]=(gw_Disp_CurrentSec%3600/60/10)+0x30;
	Time_Char[1]=gw_Disp_CurrentSec%3600/60%10+0x30;
	Time_Char[2]=':';
	Time_Char[3]=gw_Disp_CurrentSec%60/10+0x30;
	Time_Char[4]=gw_Disp_CurrentSec%60%10+0x30;	

	gdw_LrcFileCurrentCluster=gdw_LrcFileStartCluster;

	gw_LrcOffset=0;
	gdw_LrcFilePointer=0;
	gw_LrcNextOffset=0;
	gdw_LrcNextFilePointer=0;	
	gb_LrcGetTimeOrNot=0;
	gb_LrcGetTimeNextOrNot=0;
	Lyric_ReadLrcFile(0);
	
	while(1)
	{
		if(gc_PlayRecordDataBuf[gw_LrcOffset]=='[')
		{
			
			for(i=0;i<5;i++)
			{
				if(Lyric_OffsetLRC_Full()==0)
					return;
				if(Time_Char[i]!=gc_PlayRecordDataBuf[gw_LrcOffset])
					break;
			}
			if(i>=5)//(JC)matching timestamp 
			{
				while(1)
				{
					while(gc_PlayRecordDataBuf[gw_LrcOffset]!=']')//(JC)bypass the decimal point and digits rite to it
					{	
						if(Lyric_OffsetLRC_Full()==0)
							return;	
					}
					if(Lyric_OffsetLRC_Full()==0)//(JC)Bypass ']'
						return;		
					if(gc_PlayRecordDataBuf[gw_LrcOffset]!='[')
					{	
						gb_LrcGetTimeOrNot = 1;//(JC)matching time stamp, following the lyrics
						temp_gw_LrcOffset=gw_LrcOffset;
						temp_gdw_LrcFilePointer=gdw_LrcFilePointer;
						gdw_LrcFileCurrentCluster=gdw_LrcFileStartCluster;
						gw_LrcOffset=0;				//tangwei add 081111
						gdw_LrcFilePointer=0;
						Lyric_ReadLrcFile(0);
						temp_timecon=((Time_Char[0]-0x30)*10+(Time_Char[1]-0x30))*60+(Time_Char[3]-0x30)*10+(Time_Char[4]-0x30);
						while(1)
						{	
							while(gc_PlayRecordDataBuf[gw_LrcOffset]!='[')
							{	
								if(Lyric_OffsetLRC_Full()==0)
								{
									gw_LrcOffset=temp_gw_LrcOffset;
									gdw_LrcFilePointer=temp_gdw_LrcFilePointer;
									return;
								}
							}	
   					    	for(i=0;i<5;i++)			//tangwei add all these below 081111
							{
								if(Lyric_OffsetLRC_Full()==0)
									return;	
								if(i==2)
								{	
									if(gc_PlayRecordDataBuf[gw_LrcOffset]!=':')
										break;
								}
								else
								{
									if((gc_PlayRecordDataBuf[gw_LrcOffset]>0x39)||(gc_PlayRecordDataBuf[gw_LrcOffset]<0x30))
									{
										break;
									}
								
									timevar[i]=gc_PlayRecordDataBuf[gw_LrcOffset];
								}
							}
							
							if(i!=2)
							{
								temp_timevar=((timevar[0]-0x30)*10+(timevar[1]-0x30))*60+(timevar[3]-0x30)*10+(timevar[4]-0x30);

								tw_Lrcoffset=gw_LrcOffset;
								tw_LrcFilePointer=gdw_LrcFilePointer;
								if(temp_timevar>temp_timecon)
								{   
									if(setflag)
									{   
										temp_timelow=temp_timevar;
										setflag=0;
										while(1)
										{
											while(gc_PlayRecordDataBuf[gw_LrcOffset]!=']')
											{	
												if(Lyric_OffsetLRC_Full()==0)
													return;	
											}
											if(Lyric_OffsetLRC_Full()==0)
													return;		
											if(gc_PlayRecordDataBuf[gw_LrcOffset]!='[')
											{	
												gb_LrcGetTimeNextOrNot = 1;
												gw_LrcNextOffset=gw_LrcOffset;
												gdw_LrcNextFilePointer=gdw_LrcFilePointer;
												break;
											}
											else
											{
												k++;
											}
										}
									}
								}
							
								if((temp_timevar>temp_timecon)&&(temp_timevar<temp_timelow))
								{  
									temp_timelow=temp_timevar;
									while(1)
									{
										while(gc_PlayRecordDataBuf[gw_LrcOffset]!=']')
										{	
											if(Lyric_OffsetLRC_Full()==0)
												return;	
										}
										if(Lyric_OffsetLRC_Full()==0)
												return;		
										if(gc_PlayRecordDataBuf[gw_LrcOffset]!='[')
										{	
											gb_LrcGetTimeNextOrNot = 1;
											gw_LrcNextOffset=gw_LrcOffset;
											gdw_LrcNextFilePointer=gdw_LrcFilePointer;
											break;
										}
										else
										{
											k++;
										}
									}
								}
							}
			
							 if(k!=0)
							 {
							 	k=0;   //sunzhk add 090613
							 	gw_LrcOffset=tw_Lrcoffset;
								gdw_LrcFilePointer=tw_LrcFilePointer;
							 }
						}		
					}
				}
			}
		}
		if(Lyric_OffsetLRC_Full()==0)
			return;	
	}
}

U8 Lyric_NextReadLrcFile(U8 tc_ReadNextSector)  //0:Jump to right sector  1:Continue mode for Next sector //yflin090401 
{
	U32 tdw_DataLogicAddr;
	U16 tdw_ResidualSector;
	U32 tdw_prevCluster;
	U32 tdw_Sector;

	if(gdw_LrcFilePointer >= gdw_LrcFileSize)
	{
		return 0;
	}

	tdw_Sector=gdw_LrcNextFilePointer>>9;

	if(tdw_Sector<gc_DOS_SectorPerCluster) //in start Cluster
	{
		gdw_LrcFileCurrentCluster=gdw_LrcFileStartCluster;		
	}	
	else
	{
		if(tc_ReadNextSector)
		{
			if (( (tdw_Sector&(gc_DOS_SectorPerCluster-1))==0) )
			{
				tdw_prevCluster = gdw_LrcFileCurrentCluster;
				gdw_LrcFileCurrentCluster =	DOS_GetNextCluster(gdw_LrcFileCurrentCluster, 1);
				if(tdw_prevCluster==gdw_LrcFileCurrentCluster) return 0;
			}
		}
		else  //tc_ReadNextSector=0   
		{
			tdw_prevCluster=tdw_Sector/gc_DOS_SectorPerCluster;
			gdw_LrcFileCurrentCluster =	DOS_GetNextCluster(gdw_LrcFileStartCluster, tdw_prevCluster);	
		}

		if ((gdw_LrcFileCurrentCluster==0) || (gdw_LrcFileCurrentCluster >= 0x0ffffff8))
		{
			return 0;
		}
	}

	tdw_ResidualSector = (tdw_Sector)&(gc_DOS_SectorPerCluster-1);
	tdw_DataLogicAddr = DOS_ClusterLogicAddr(gdw_LrcFileCurrentCluster)+(tdw_ResidualSector);
	
	gb_ReadWriteDataArea = 0;
	return (!DOS_Read_LogicSector(tdw_DataLogicAddr, 1));
}

void Lyric_NextGetLyrics(void)
{
	data	U8 i;

	i=0;
	Lyric_NextReadLrcFile(0);
 
	gw_LrcNextOffset=(gdw_LrcNextFilePointer&0x1ff);

	while(gc_PlayRecordDataBuf[gw_LrcNextOffset]!=0x0d && gc_PlayRecordDataBuf[gw_LrcNextOffset]!=0x0a)//(JC)not <CR> and <LF>
	{
		gc_LrcDisplayBuf[D_LrcBuffLimit/2+i]=gc_PlayRecordDataBuf[gw_LrcNextOffset];
		i++;
		if(i>=D_LrcBuffLimit/2)//(JC)max char numbers of piece of lyrics for a time stamp 
			break;

		gw_LrcNextOffset++;
		gdw_LrcNextFilePointer++;
		if(gdw_LrcNextFilePointer >= gdw_LrcFileSize)
		{
			break;
		}	
		if(gw_LrcNextOffset==512)
		{
			gw_LrcNextOffset=0;
			if(!Lyric_NextReadLrcFile(1))
				break;
		}				
	}
	gc_LrcCurrentLenNext=i;
}

U8 Lyric_OffsetLRC_Full()
{
	gw_LrcOffset++;
	gdw_LrcFilePointer++;
	if(gdw_LrcFilePointer>=gdw_LrcFileSize)
	{
		return 0;
	}	
	if(gw_LrcOffset==512)
	{
		if(gc_Play_FileType==0)
		{
			if(MP3_DataIn()==DSP_DATAIN_COMMAND_ERROR)
			{	
			 	gc_PhaseInx = 2;	//End of File for play
			 	return 0;//(JC)H0620
			}
		}
		else if(gc_Play_FileType == 0x01)//(JC)H1128
		{
			if(WMA_DataIn()==DSP_DATAIN_COMMAND_ERROR)
			{	
			 	gc_PhaseInx = 2;	//End of File for play
			 	return 0;//(JC)H0620
			}
		}
//===========================================huiling.gu add 081212
		gw_LrcOffset=0;

		if(!Lyric_ReadLrcFile(1))
		{
			return 0;
	  	}
	}
	return 1;	
}


U8 Lyric_ReadLrcFile(U8 tc_ReadNextSector)  //0:Jump to right sector  1:Continue mode for Next sector//yflin090401
{
	U32 tdw_DataLogicAddr;
	U16 tdw_ResidualSector;
	U32 tdw_prevCluster;
	U32 tdw_Sector;

	if(gdw_LrcFilePointer >= gdw_LrcFileSize)
	{
		return 0;
	}

	tdw_Sector=gdw_LrcFilePointer>>9;

	if(tdw_Sector<gc_DOS_SectorPerCluster) //in start Cluster
	{
		gdw_LrcFileCurrentCluster=gdw_LrcFileStartCluster;		
	}	
	else
	{
		if(tc_ReadNextSector)
		{
			if (( (tdw_Sector&(gc_DOS_SectorPerCluster-1))==0) )
			{
				tdw_prevCluster = gdw_LrcFileCurrentCluster;
				gdw_LrcFileCurrentCluster =	DOS_GetNextCluster(gdw_LrcFileCurrentCluster, 1);
				if(tdw_prevCluster==gdw_LrcFileCurrentCluster) return 0;
			}
		}
		else  //tc_ReadNextSector=0   
		{
			tdw_prevCluster=tdw_Sector/gc_DOS_SectorPerCluster;
			gdw_LrcFileCurrentCluster =	DOS_GetNextCluster(gdw_LrcFileStartCluster, tdw_prevCluster);	
		}

		if ((gdw_LrcFileCurrentCluster==0) || (gdw_LrcFileCurrentCluster >= 0x0ffffff8))
		{
			return 0;
		}
	}

	tdw_ResidualSector = (tdw_Sector)&(gc_DOS_SectorPerCluster-1);
	tdw_DataLogicAddr = DOS_ClusterLogicAddr(gdw_LrcFileCurrentCluster)+(tdw_ResidualSector);
	
	gb_ReadWriteDataArea = 0;
	return (!DOS_Read_LogicSector(tdw_DataLogicAddr, 1));
}


void Lyric_GetLyrics(void)
{
	data	U8 i;

	i=0;
	Lyric_ReadLrcFile(0);

	gw_LrcOffset=(gdw_LrcFilePointer&0x1ff);
	while(gc_PlayRecordDataBuf[gw_LrcOffset]!=0x0d && gc_PlayRecordDataBuf[gw_LrcOffset]!=0x0a)//(JC)not <CR> and <LF>
	{
		gc_LrcDisplayBuf[i]=gc_PlayRecordDataBuf[gw_LrcOffset];
		i++;
		if(i>=D_LrcBuffLimit/2)//(JC)max char numbers of piece of lyrics for a time stamp 
			break;

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
				break;
		}				
	}
	gc_LrcCurrentLen=i;
}



U8 Lyric_FileSearch()
{
	data	U8 tc_Status;
 	data	U8 tc_TempBuff[3];
	data	U16 tw_Lenth;

	{
		EXT_NameC[0]=1;
		gs_File_FCB[2].dw_FDB_StartCluster=gs_File_FCB[0].dw_FDB_StartCluster;
		if (!gc_FileLongName[2])
		{//0:long
			((U8 *)(&tw_Lenth))[0] = gc_FileLongName[3];
			((U8 *)(&tw_Lenth))[1] = gc_FileLongName[4];
			tw_Lenth+=5;	
				
			tc_TempBuff[0] = gc_FileLongName[tw_Lenth-2];
			tc_TempBuff[1] = gc_FileLongName[tw_Lenth-4];			
			tc_TempBuff[2] = gc_FileLongName[tw_Lenth-6];			
			gc_FileLongName[tw_Lenth-6] = 'L';
			gc_FileLongName[tw_Lenth-4] = 'R';
			gc_FileLongName[tw_Lenth-2] = 'C';			
			tc_Status = DOS_Search_File(C_File_OneDir|C_By_LName, C_OtherFileType, C_CmpExtName|C_Next);			
		}
		else
		{
			EXT_NameC[0]=1;		
			EXT_NameC[1]='L';
			EXT_NameC[2]='R';
    		EXT_NameC[3]='C';
		
			tc_Status = DOS_Search_File(C_File_OneDir|C_By_SName, C_OtherFileType, C_CmpExtName|C_Next);
		}	
		
		if (!gc_FileLongName[2])//(JC)finish searching, restore file name
		{
			gc_FileLongName[tw_Lenth-2] = tc_TempBuff[0];
			gc_FileLongName[tw_Lenth-4] = tc_TempBuff[1]; 
			gc_FileLongName[tw_Lenth-6] = tc_TempBuff[2];
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




