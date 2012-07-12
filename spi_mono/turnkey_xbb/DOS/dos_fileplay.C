#include <string.h>
#include "..\header\SPDA2K.h"
#include "..\header\variables.h"

#ifdef SPI_1M

void UNICODE_READ_HZK_SECTOR_ISN(U16 tw_SectorCount)
{
	SPI_DSPHZK_Block_Read(1,tw_SectorCount,gc_PlayRecordDataBufHB);
}

void X_UnicodeConvGB2312BIGSJIS(unsigned char *Unicode1,unsigned char *Unicode2)
{	
	bit hit;
	U8	aa=11,counter=2,number=0,len=22;
	U16	tw_hzkbyteoffest = 0;
	U16	tw_hzksectoroffest = 0;
	U32	(*UtoX_Table_Offset)[3];
	U32	InputOffset;

	U32 code UtoG_table_offset[][3]=
	{
		{ 322,  406,  322},
		{ 924,  42,  518},
		{ 1414,  14,  966},
		{ 1820,  126,  1358},
		{ 2044,  168,  1456},
		{ 16422,  84,  15666},
		{ 16898,  56,  16058},
		{ 17080,  112,  16184},
		{ 17416,  224,  16408},
		{ 17710,  42,  16478},
		{ 17948,  14,  16674},
		{ 18620,  126,  17332},
		{ 18942,  238,  17528},
		{ 19264,  98,  17612},
		{ 19460,  28,  17710},
		{ 19572,  28,  17794},
		{ 24570,  602,  22764},
		{ 25662,  28,  23254},
		{ 39928,  41818,  37492},
		{ 128016,  14,  83762},
		{ 130550,  210,  86282},
		{ 130998,  28,  86520}
	};



	InputOffset = (((U16)(*Unicode1) << 8) + *Unicode2);
	hit=0;

	InputOffset=InputOffset*2;

	UtoX_Table_Offset=UtoG_table_offset;

	while(!hit)
	{
		counter*=2;
		if( (InputOffset>=UtoX_Table_Offset[aa][0]) && ( InputOffset <= UtoX_Table_Offset[aa+1][0] ))
		{
			if((InputOffset>=UtoX_Table_Offset[aa][0]) && (InputOffset <= (U32)UtoX_Table_Offset[aa][0]+ (U32)UtoX_Table_Offset[aa][1] ))
			{
				hit=1;
			}
			else
			{ 
				goto CantFindUnicode;
			}
		}
		else
		{
			number=len/counter;
			if(number<1) number=1;
			if(InputOffset>UtoX_Table_Offset[aa][0])
			{
				aa=aa+number; 
			}
			else
			{
				aa=aa-number; 
				if (aa<0)
				{
					goto CantFindUnicode;
				}				
			}
		}
	}

	InputOffset = InputOffset-UtoX_Table_Offset[aa][2];

	tw_hzksectoroffest = InputOffset/512;
	tw_hzkbyteoffest = InputOffset%512;
	UNICODE_READ_HZK_SECTOR_ISN(390+tw_hzksectoroffest);

	*Unicode1 = gc_PlayRecordDataBuf[tw_hzkbyteoffest];
	*Unicode2 = gc_PlayRecordDataBuf[tw_hzkbyteoffest+1];
	
	return;

CantFindUnicode:
	*Unicode1 = 0x20;
	*Unicode2 = 0x20;
	return;
}


U8 X_Unicode2ISN(U8 *UnicodeArray)
{
	U16	ISNByteNum,i,UnicodeLenth;
	
	if(!UnicodeArray[2])
	{//long
		U8 tc_Temp;	
		((U8 *)(&UnicodeLenth))[0] = UnicodeArray[3];
		((U8 *)(&UnicodeLenth))[1] = UnicodeArray[4];
		for(i=5;i < UnicodeLenth+5;i+=2)
		{
			tc_Temp = UnicodeArray[i];
			UnicodeArray[i] = UnicodeArray[i+1];
			UnicodeArray[i+1] = tc_Temp;
		}
		ISNByteNum = 5;
		for(i=5;i < UnicodeLenth+5;i+=2)
		{
			if(*(UnicodeArray+i)==0x00)
			{
				if(*(UnicodeArray+i+1)==0x00)
					break;
				*(UnicodeArray+ISNByteNum)=*(UnicodeArray+i+1);
				ISNByteNum++;
			}
			else
			{
				X_UnicodeConvGB2312BIGSJIS( (UnicodeArray+i), (UnicodeArray+i+1) );
				*(UnicodeArray+ISNByteNum)=*(UnicodeArray+i);
				*(UnicodeArray+ISNByteNum+1)=*(UnicodeArray+i+1);
				ISNByteNum+=2;
			}
		}
		ISNByteNum -= 5;
		UnicodeArray[3] = ((U8 *)(&ISNByteNum))[0];
		UnicodeArray[4] = ((U8 *)(&ISNByteNum))[1];	
		UnicodeArray[2] = 0x01;
	}
	return ISNByteNum;
}
#endif

U8 DOS_GetLongFileName(U8 tc_FileHandle, U8 * tpc_LongName)
{
	data	bit	tb_EndSign=0;
	data	U8	tc_Chksum=0;
	data	U8	tc_LogAddrNum=1;
	data	U8	j;
	data	U16	i;
	data	U16	tw_FDB_Offset;
	data	U16	tw_LFN_Offset;
	data	U16	tw_FileNameLen;
	xdata	U8  tc_Array[26];
	xdata	U32	tdw_Addr;

	tdw_Addr=gs_File_FCB[tc_FileHandle].dw_FDB_LogAdd;
	tw_FDB_Offset=gs_File_FCB[tc_FileHandle].w_FDB_Offset;

	gb_ReadWriteDataArea=1;
	DOS_Read_LogicSector(tdw_Addr);

	memcpy(tc_Array,&gc_UserDataBuf[tw_FDB_Offset],11);

	for(j=0;j<11;j++)
	{
		tc_Chksum = ((tc_Chksum & 1) ? 0x80 : 0) + (tc_Chksum >> 1) + gc_UserDataBuf[tw_FDB_Offset+j];
	}//compute the Chksum!
	
	tw_LFN_Offset=tw_FDB_Offset;
	i=5;//fill the UNICODE from the offset 3! 
	while(i<512)
	{	 
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
			if(gc_CurrentCard==9)
			{
				return 1;
			}
			gb_ReadWriteDataArea=1;
			DOS_Read_LogicSector(tdw_Addr);
			tw_LFN_Offset=512-32;
		}
		
		if((gc_UserDataBuf[tw_LFN_Offset]&0x40)==0x40)
		{
			tb_EndSign=1;
		}	

		if(((gc_UserDataBuf[tw_LFN_Offset+11]==0x0f)||(gc_UserDataBuf[tw_LFN_Offset+11]==0x3f))&&(gc_UserDataBuf[tw_LFN_Offset+12]==0x00)&&(gc_UserDataBuf[tw_LFN_Offset+13]==tc_Chksum))
		{	 
			j=26;
			while(j>0)
			{
				tpc_LongName[i]=gc_UserDataBuf[tw_LFN_Offset+Unicode_Offset[j-1]];//fill	the long file name into the array
				tpc_LongName[i+1]=gc_UserDataBuf[tw_LFN_Offset+Unicode_Offset[j-2]];
				i+=2;
				j-=2;
			}
		} 
		else 
		{ 
			j=0;
			gb_ReadWriteDataArea=1;
			DOS_Read_LogicSector(gs_File_FCB[tc_FileHandle].dw_FDB_LogAdd);
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
   		
		if(tb_EndSign) //when end,fill the first 3 bytes indicate the information of long file name!
		{
			tpc_LongName[0]=gc_UserDataBuf[tw_LFN_Offset];
			tpc_LongName[2]=K_LongFileName;


			for (j = 26; j > 0; j=j-2)
			{
				if((gc_UserDataBuf[tw_LFN_Offset+Unicode_Offset[j-1]]==0)&&(gc_UserDataBuf[tw_LFN_Offset+Unicode_Offset[j-2]]==0))
				{
					break; 
				}
			}
			tpc_LongName[1] = j;
			tw_FileNameLen=(((tpc_LongName[0]&0x3f)-1)*26)+(26-tpc_LongName[1]-1);
			tpc_LongName[3]=((U8 *)(&tw_FileNameLen))[0];            //save the high 4bit 
			tpc_LongName[4]=((U8 *)(&tw_FileNameLen))[1];          //save the low 4bit
			return 0;
		}
	}
	return 0;
}