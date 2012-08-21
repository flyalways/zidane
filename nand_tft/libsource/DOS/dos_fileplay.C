#include <string.h>
#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "dos\dosfat.h"
#include "dos\dosfdb.h"
#include "dos\dosrw.h"
#include "Memalloc.h"
#include "Rambase.h"
#include "PROTOTYP.h"

extern U8  code Unicode_Offset[];
extern U8  code Unicode_Offset2[];
extern	U8	fdb_check_sum(U8 xdata * p);
extern	xdata	U8	gc_HostSectorUnit;
//! Read 512 bytes form a file.
/*!
 *	\param	tc_FileHandle	The file handle to be read.
 *	\return	
 *			- DOS_STARTCLUSTER_ERR
 *			- DOS_CLUSTER_LINK_ERR
 *			- DOS_END_OF_FILE
 *			- DOS_SUCCESS
 *			- DOS_FALSE
 */
U8 DOS_Read_File(U8 tc_FileHandle)
{
	U8	tc_ResidueSector;
	U32	tdw_DataPoint;

	if(gs_File_FCB[tc_FileHandle].dw_File_StartCluster==0xffffffff)
	{
		return DOS_STARTCLUSTER_ERR;
	}

	if(gs_File_FCB[tc_FileHandle].dw_File_DataPoint < gs_File_FCB[tc_FileHandle].dw_File_TotalSize)
	{//not at file end.
		tc_ResidueSector=(gs_File_FCB[tc_FileHandle].dw_File_DataPoint>>9)&(gc_DOS_SectorPerCluster*gc_HostSectorUnit-1);
		if((tc_ResidueSector==0)&&(gs_File_FCB[tc_FileHandle].dw_File_DataPoint!=0))
		{//next cluster.
			gs_File_FCB[tc_FileHandle].dw_File_CurrentCluster = DOS_GetNextCluster(gs_File_FCB[tc_FileHandle].dw_File_CurrentCluster,1);
			if(gs_File_FCB[tc_FileHandle].dw_File_CurrentCluster == 0xffffffff)
			{
				return DOS_CLUSTER_LINK_ERR;
			}
		}
		tdw_DataPoint = DOS_ClusterLogicAddr(gs_File_FCB[tc_FileHandle].dw_File_CurrentCluster) + tc_ResidueSector;
		gb_ReadWriteDataArea = 0;
		gs_File_FCB[tc_FileHandle].dw_File_DataPoint += 512;
		return (DOS_Read_LogicSector(tdw_DataPoint, 1));
	}
	else
	{
		return DOS_END_OF_FILE;
	}
}


U8 DOS_GetLongFileName(U8 tc_FileHandle, U8 * tpc_LongName)
{
	xdata	U16	i;
	xdata	U8  tc_Array[26];
	data	bit	tc_EndSign=0;
	data	U8	j;
	data	U8	tc_Chksum=0;
	data	U8	tc_LogAddrNum=1;
	data	U16	tw_FDB_Offset;
	data	U16	tw_LFN_Offset;
	data	U16	tw_FileNameLen;
	xdata	U32	tdw_Addr;
	
	tdw_Addr=gs_File_FCB[tc_FileHandle].dw_FDB_LogAdd;
	tw_FDB_Offset=gs_File_FCB[tc_FileHandle].w_FDB_Offset;
	gb_ReadWriteDataArea=1;
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
				tdw_Addr=gs_File_FCB[tc_FileHandle].dw_LongFDB_LogAdd0;
				tc_LogAddrNum=2;
			}
			else if(tc_LogAddrNum==2) 
			{
		   		tdw_Addr=gs_File_FCB[tc_FileHandle].dw_LongFDB_LogAdd1;
			}
			gb_ReadWriteDataArea=1;
			DOS_Read_LogicSector(tdw_Addr, 1);
			tw_LFN_Offset=512-32;
		}
		p = &(gc_UserDataBuf[tw_LFN_Offset]);
		
		if((*p&0x40)==0x40)
		{
			tc_EndSign=1;
		}	

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
			gb_ReadWriteDataArea=1;
			DOS_Read_LogicSector(gs_File_FCB[tc_FileHandle].dw_FDB_LogAdd, 1);
			tpc_LongName[0]=0;
			tpc_LongName[1]=0;
			tpc_LongName[2]=K_ShortFileName;
			tpc_LongName[3]=0;
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
   		
		if(tc_EndSign) //when end,fill the first 3 bytes indicate the information of long file name!
		{
			tpc_LongName[0]=*p;
			tpc_LongName[2]=K_LongFileName;

			for (j = 0; j < 26; j+=2)
			{
				if((p[Unicode_Offset2[j]]==0)&&(p[Unicode_Offset2[j+1]]==0))
				{
					break; 
				}
			}
			j = 26-j;
			tpc_LongName[1] = j;
			tw_FileNameLen=(((tpc_LongName[0]&0x3f)-1)*26)+(26-tpc_LongName[1]);//(JC)H0305
			tpc_LongName[3] = ((UBYTE *)(&tw_FileNameLen))[0];            //save the high 4bit 
			tpc_LongName[4] = ((UBYTE *)(&tw_FileNameLen))[1];          //save the low 4bit
			return 0;	
		}
	}

	return 0;
}
