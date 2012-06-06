#include <string.h>
#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\header\id3.h"

extern xdata System_Struct gs_System_State;
extern xdata U16 gw_TotalSec ;
extern	data	bit	gb_HostConnect;  //20090803 chiayen add
extern xdata U32 gdw_ID3_v2_sz;		//Jimi 090612
extern xdata U16 gw_nCompressionLevel; //Jimi 091118
//extern xdata U16 gw_Random_Timer;

code U16 MP3bitRatetable_ID3[2][3][16]=
{//=-=xyq,1-8,U8-->U16
	//MPEG-1      //=-=xyq,1-8,this TAB have been modified
	0, 32, 64, 96,128,160,192,224,256,288,320,352,384,416,448,0,//layer1
	0, 32, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,384,0,//layer2
	0, 32, 40, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,0,//layer3
	//MPEG-2
	0, 32, 48, 56, 64, 80, 96,112,128,144,160,176,192,224,256,0,//layer1
	0, 8,  16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0,//layer2
	0, 8,  16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0,//layer3
};


U8 ID3_V2_Get_Idf(U32 *tpdw_DATA_LogAddr)
{	
	ID3_Read_File(ID3_V2_START_PTR, &gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster, tpdw_DATA_LogAddr);

	if(gc_PlayRecordDataBuf[0]=='I' && gc_PlayRecordDataBuf[1]=='D' && gc_PlayRecordDataBuf[2]=='3')
	{
		if((gc_PlayRecordDataBuf[3]==0x03) || (gc_PlayRecordDataBuf[3]==0x04) || (gc_PlayRecordDataBuf[4]==0x00))  //Jimi 091126
		{
			return ID3_V2_TAG_FOUND;
		}
	}
	return	ID3_V2_TAG_NOT_FOUND;
}


void ID3_Read_File(U32 tdw_FilePoint, U32 *tpdw_FileCluster, U32 *tpdw_DATA_LogAddr)
{
	data	U32 tdw_cluster_cnt;
	
	tdw_cluster_cnt = (tdw_FilePoint>>9)/gc_DOS_SectorPerCluster;

	if(tdw_cluster_cnt)
	{
		*tpdw_FileCluster=DOS_GetNextCluster(gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster, tdw_cluster_cnt);
	}
	else
	{
		*tpdw_FileCluster = gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
	}

	*tpdw_DATA_LogAddr = DOS_ClusterLogicAddr(*tpdw_FileCluster) + (tdw_FilePoint >> 9)%gc_DOS_SectorPerCluster;
	gb_ReadWriteDataArea = 0;
	DOS_Read_LogicSector((*tpdw_DATA_LogAddr),1);
}

U16 MP3_Total_Time_Parser()
{
	U16 tw_total_time;

	if(gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize==0)//(JC)I0112
	{
		return 0;
	}

	tw_total_time = (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize- gdw_ID3_v2_sz)/ gs_System_State.w_BitRate / 125;
	return tw_total_time;
}


U16 MP3_Bitrate_Parser()
{
	U32 tdw_DATA_LogAddr;
	U16 tw_Buf_idx;
	U32 tdw_File_Ptr;

	U8	tc_sync_buf;
	U8	tc_temp;
	U8	tc_Version;
	U8	tc_Layer;
	U8	tc_bitrate_idx;
	U16	tw_kBitRate;
	U8	tc_sample_rate_idx;
	U16	tw_SampleRate;
	U8	tc_Padding;
	U16	tw_Frame_Size;
	U16	tw_Frame_Size_avg;
	U8  tc_Check_VBR_Done;

    code U16 SamplingRatetable[4][3]=
	{
        	44100,22050,11025,
        	48000,24000,12000,
        	32000,16000,8000,
        	0,    0,    0
	};

	gdw_ID3_v2_sz = 0;		//Jimi 090612, reset ID3 V2 size
	tc_Check_VBR_Done = 0;
//=== Open file ===
	if(DOS_Open_File_r(C_MusicFHandle, C_Open_FoundFile, C_NullFileName) || (gs_File_FCB[0].dw_File_TotalSize==0)) //Ching 090116
	{
		return 0;
	}
//=== Check ID3 v2 ===
	if(ID3_V2_Get_Idf(&tdw_DATA_LogAddr) == ID3_V2_TAG_FOUND)
	{
		gdw_ID3_v2_sz = ((((U32)gc_PlayRecordDataBuf[6]&0x7F) <<21) 	//Jimi 090612, store ID3 V2 size
	    	            +(((U32)gc_PlayRecordDataBuf[7]&0x7F) <<14)
	        	        +(((U32)gc_PlayRecordDataBuf[8]&0x7F) <<7 )
	            	    +(((U32)gc_PlayRecordDataBuf[9]&0x7F)     )
						+10);	//'cuz bit 7 are all '0' in these 4 bytes

		MP3_Parser_Seek_Dataptr(C_MusicFHandle ,gdw_ID3_v2_sz); 	//Jimi 090612, store ID3 V2 size
	}

	tdw_File_Ptr = gdw_ID3_v2_sz; 	//Jimi 090612
	tw_Buf_idx = tdw_File_Ptr & 0x1FF;


	//---blk (1)---
	while( tdw_File_Ptr  < gdw_ID3_v2_sz + 4096 )		//Jimi 090612
	{
		U8 tc_nextSector = 0;
		U32 tdw_Peek_File_Ptr = tdw_File_Ptr;

		tw_Buf_idx = tdw_Peek_File_Ptr & 0x1FF;
		if( (!tw_Buf_idx) && (tdw_Peek_File_Ptr >0))
		{	// i has beyond current sector to the next.
			MP3_Parser_Seek_Dataptr( C_MusicFHandle , tdw_Peek_File_Ptr);	//to make sure if it beyond to the next cluster or not and then read file
		}

		if((gc_CurrentCard==2) && (SD_Detect==1))  //20090803 chiayen add
		{
			break;
		}

		if((gc_CurrentCard==5) && (gb_HostConnect==0))  //20090803 chiayen add
		{			
			break;		
		}
		
		//---blk(2)---
		if(gc_PlayRecordDataBuf[tw_Buf_idx] == 0xFF)
		{	//check sync word
			tdw_Peek_File_Ptr++;
			tc_nextSector = Next_One(C_MusicFHandle, &tw_Buf_idx, tdw_Peek_File_Ptr);
			//---blk (3)---
			if( (gc_PlayRecordDataBuf[tw_Buf_idx] & 0xE0) == 0xE0)
			{	//sync word was be found
				tc_sync_buf = gc_PlayRecordDataBuf[tw_Buf_idx];
				tc_temp = tc_sync_buf & 0x18;
				if( tc_temp == 0x18)
					tc_Version = 0;		// MPEG 1
				
				else if(tc_temp == 0x10)
					tc_Version = 1;		// MPEG 2

				else
					tc_Version = 2;		// MPEG 2.5

				tc_temp = (tc_sync_buf & 0x6)>>1;
				tc_Layer = (4- tc_temp) - 1;	//layer-1
				tdw_Peek_File_Ptr++;
				tc_nextSector = Next_One(C_MusicFHandle, &tw_Buf_idx, tdw_Peek_File_Ptr);
				
				tc_sync_buf = gc_PlayRecordDataBuf[tw_Buf_idx];

				tc_bitrate_idx = ( tc_sync_buf & 0xF0 ) >> 4;
				
				tc_temp = (tc_Version)? 1:0;
		
				tw_kBitRate = MP3bitRatetable_ID3[tc_temp][tc_Layer][tc_bitrate_idx];
				//dbprintf("kBitrate = %x\n",tw_kBitRate);
				tc_sample_rate_idx = (tc_sync_buf & 0x0C) >> 2;
				tw_SampleRate = SamplingRatetable[tc_sample_rate_idx][tc_Version];

				if(tc_sample_rate_idx==1)			//20100512
				{
					GLOBAL_REG[0x46] &=0xFB; 
				}
				else
				{
					GLOBAL_REG[0x46] |=0x04; 
				}

				tc_Padding = (tc_sync_buf & 0x02) >> 1;

				// ---blk (4)---
				if( (tw_kBitRate != 0) && (tw_SampleRate != 0) )		//Reliable Frame
				{
					//Calculate frame size
					tw_Frame_Size = (U16)(144 * (U32)tw_kBitRate * 1000 / (U32)tw_SampleRate);	//MPEG 1

					if(tc_Version)				
						tw_Frame_Size = tw_Frame_Size / 2;										//MPEG2 & 2.5
					
					tw_Frame_Size += tc_Padding;

					//=== check VBR ===
					if(!tc_Check_VBR_Done)
					{
						tc_Check_VBR_Done = 1;
						tw_Frame_Size_avg = MP3_VBR_Hdr_Parser(tw_Frame_Size, tdw_Peek_File_Ptr);

						if(tw_Frame_Size_avg)
						{
							tw_kBitRate = (U16)(((U32)tw_Frame_Size_avg * (U32)tw_SampleRate) / 144000);						
							if(tc_Version)	
							{
								tw_kBitRate *= 2;	
							}

							break;	
						}			
					}
					//== Seek to Next Frame
					tdw_Peek_File_Ptr = (U32)(tdw_File_Ptr + tw_Frame_Size);
					MP3_Parser_Seek_Dataptr(C_MusicFHandle ,tdw_Peek_File_Ptr);
					tw_Buf_idx = (U16)(tdw_Peek_File_Ptr & 0x1FF);
					// ---blk (5)---
					if( gc_PlayRecordDataBuf[tw_Buf_idx] == 0xFF )
					{
						tdw_Peek_File_Ptr++;
						tc_nextSector = Next_One(C_MusicFHandle, &tw_Buf_idx, tdw_Peek_File_Ptr);

						// ---blk (6)---
						if( (gc_PlayRecordDataBuf[tw_Buf_idx] & 0xE0) == 0xE0)
						{
							U8 tw_temp;
							tdw_Peek_File_Ptr++;
							tc_nextSector = Next_One(C_MusicFHandle, &tw_Buf_idx, tdw_Peek_File_Ptr);
							tw_temp = (gc_PlayRecordDataBuf[tw_Buf_idx] & 0xF0) >> 4;
							// ---blk (7)---
							if( (tw_temp >= 8)|| (tw_temp<=384) )
							{
//								dbprintf("CBR OK~\n");
								break;	
							}//---end of (7)---

						}//---end of (6)---

					}//---end of (5)---

				}//---end of (4)---

			}//---end of (3)---

		}//---end of (2)---
		
		if( !((++tdw_File_Ptr)&0x1FF) || tc_nextSector)			//Jimi Yu 081224
			MP3_Parser_Seek_Dataptr(C_MusicFHandle ,tdw_File_Ptr);
	}//---end of (1)---

	if((tw_kBitRate<8)||(tw_kBitRate>448))			//Jimi 090623
	{     
		tw_kBitRate = 128;
	}
	
	return(tw_kBitRate);	
}

U16	MP3_VBR_Hdr_Parser(U16 tw_Frame_Size, U32 tdw_DataPoint)
{
	
    code U8 VBRHeader[3][4]={	'V','B','R','I',
								'X','i','n','g',
							 	'I','n','f','0',  }; 													

	bit tc_VBR_flag;

	U8	tc_i;
	U8	tc_VBR_type;
	U16 tw_Frm_Sz_Avg;
	U16 tw_VBR_Check_End;

	union
	{
		U8 Frm_Info[8];

		struct
		{
			U32 Byte_Num;
			U32 Frm_Num;
		}VBRI_Format;

		struct
		{
			U32 Frm_Num;
			U32 Byte_Num;
		}XING_Format;

	}VBR_Info;
	
	tw_Frm_Sz_Avg = 0;
	tw_VBR_Check_End = (U16)tdw_DataPoint + tw_Frame_Size - 3;

	while(tdw_DataPoint < tw_VBR_Check_End)
	{
		U16 tw_buf_idx;

		tw_buf_idx = (U16)tdw_DataPoint & 0x1FF;
		tdw_DataPoint ++;				//Jimi 081202
		Next_One(C_MusicFHandle, &tw_buf_idx, tdw_DataPoint);

		if((gc_CurrentCard==2) && (SD_Detect==1))  //20090803 chiayen add
		{
			break;
		}

		if((gc_CurrentCard==5) && (gb_HostConnect==0))  //20090803 chiayen add
		{			
			break;		
		}

		if(gc_PlayRecordDataBuf[tw_buf_idx] == VBRHeader[0][0] || gc_PlayRecordDataBuf[tw_buf_idx] == VBRHeader[1][0] || gc_PlayRecordDataBuf[tw_buf_idx] == VBRHeader[2][0]) 
		{
			//byte[0] now
			tc_VBR_flag = 1;	// VBR tag has been found
			
			if(gc_PlayRecordDataBuf[tw_buf_idx] == VBRHeader[0][0])
				tc_VBR_type = 0;	// VBRI

			else if(gc_PlayRecordDataBuf[tw_buf_idx] == VBRHeader[1][0])
				tc_VBR_type = 1;	// XING

			else 
				tc_VBR_type = 2;
	
			tdw_DataPoint++;
			Next_One(C_MusicFHandle, &tw_buf_idx, tdw_DataPoint);
			//tdw_DataPoint++;			//Jimi 081202 remove	

			//--- byte[1] now
			for( tc_i = 1; tc_i < 4; tc_i++ )
			{

				if(gc_PlayRecordDataBuf[tw_buf_idx] != VBRHeader[tc_VBR_type][tc_i])
				{
					tc_VBR_flag = 0;
					tc_i = 4;

					break;
				}
				tdw_DataPoint++;			//Jimi 081202
				Next_One(C_MusicFHandle, &tw_buf_idx, tdw_DataPoint);
			}

			//--- byte[4] now
			if(tc_VBR_flag)
			{
				//--- skip 4 bytes following vbr tag
				for( tc_i = 0; tc_i < 4; tc_i++)
				{
					tdw_DataPoint++;		//Jimi 081202
					Next_One(C_MusicFHandle, &tw_buf_idx, tdw_DataPoint);
				}
				
				//--- byte[8] now				
				if( !tc_VBR_type )
				{	//VBRI must skip 2 more byte
					tdw_DataPoint++;		//Jimi 081202
					Next_One(C_MusicFHandle, &tw_buf_idx, tdw_DataPoint);
					tdw_DataPoint++;		//Jimi 081202
					Next_One(C_MusicFHandle, &tw_buf_idx, tdw_DataPoint);
					//tdw_DataPoint += 2;	//Jimi 081202 remove
				}

				//--- byte[8](XING format) (or byte[10] for VBRI format) now: 
				//    to fetch the frame and byte number
				for( tc_i = 0; tc_i <8; tc_i++)
				{
					VBR_Info.Frm_Info[tc_i]= gc_PlayRecordDataBuf[tw_buf_idx];
					tdw_DataPoint++;		//Jimi 081202
					Next_One(C_MusicFHandle, &tw_buf_idx, tdw_DataPoint);
				}
				tw_Frm_Sz_Avg = 
					(tc_VBR_type)? (U16)(VBR_Info.XING_Format.Byte_Num / VBR_Info.XING_Format.Frm_Num) :
					               (U16)(VBR_Info.VBRI_Format.Byte_Num / VBR_Info.VBRI_Format.Frm_Num);				
				
				break;
			}
		}
	}

	return(tw_Frm_Sz_Avg);	
}


void MP3_Parser_Seek_Dataptr(tc_Handle, U32 tdw_DataPoint)
{
	gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
	gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = DOS_Seek_File(gs_System_State.c_FileHandle, tdw_DataPoint >> 9);	
	gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint = tdw_DataPoint & 0xFFFFFE00;
	DOS_Read_File(tc_Handle);			
}


U8 Next_One(U8 tc_Handle, U16 *tpw_Buf_idx, U32 tdw_fileptr)
{
	(*tpw_Buf_idx)++;
	if( !((*tpw_Buf_idx) & 0x1FF ))
	{
		*tpw_Buf_idx = 0;
		MP3_Parser_Seek_Dataptr(tc_Handle, tdw_fileptr);
		
		return 1;
	}
	return 0;
}


U8 WMA_ASF_Parser()
{
	code U8 ASF_GUID[4][16] =
	{
		0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C,	//Header Obj 
		0xA1, 0xDC, 0xAB, 0x8C, 0x47, 0xA9, 0xCF, 0x11, 0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65,	//File Properties Obj
		0x91, 0x07, 0xDC, 0xB7, 0xB7, 0xA9, 0xCF, 0x11, 0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65,	//Stream Properties Obj	
		0x40, 0x9E, 0x69, 0xF8, 0x4D, 0x5B, 0xCF, 0x11, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B	//Audio Media Obj
	};

	U32	tdw_File_Ptr;
	U16 tw_Buf_idx;
	U8 	tc_GUID[16];
	U32	tdw_Obj_Sz;
	U32	tdw_Hdr_Obj_Sz;
	U16 tw_read_byte;
	U8	tc_ASF_State_Flag;	
	U16 tw_ASF_Sample_Rate;
	U32 tdw_ASF_PlayDuration_H;
	U32 tdw_ASF_PlayDuration_L;
    	U32 tdw_ASF_PlayDuration;  //081015 yc add  for wma
	U32 tdw_ASF_Preroll_L;
	U32 tdw_ASF_Preroll_H;

	gdw_ID3_v2_sz = 0;
//=== Open file ===
	if(DOS_Open_File_r(C_MusicFHandle, C_Open_FoundFile, C_NullFileName) || (gs_File_FCB[0].dw_File_TotalSize==0)) //Ching 090116
	{
		gs_System_State.w_BitRate=0;
		gw_TotalSec=0;
		return APE_PARSE_FAIL;		
	}
	gb_ReadWriteDataArea = 0;
	DOS_Read_File(C_MusicFHandle);

	tw_Buf_idx = 0;
	tdw_File_Ptr = 0;
	tw_read_byte = 0;
	tc_ASF_State_Flag = 0;

//=== find Header Obj GUID
	ASF_Get_GUID(&tw_Buf_idx, &tdw_File_Ptr, tc_GUID);
	tw_read_byte += 16;	

	if( memcmp(tc_GUID, &ASF_GUID[HDR_OBJ_GUID][0], 16))
		return ASF_FORMAT_ERROR;
	
	tdw_Hdr_Obj_Sz = ASF_Get_Obj_Size(&tw_Buf_idx, &tdw_File_Ptr);
	tw_read_byte += 8;

	ASF_Flush_Byte(&tw_Buf_idx, &tdw_File_Ptr, 6);
	tw_read_byte = 0;

	while( tdw_File_Ptr < tdw_Hdr_Obj_Sz )
	{
		ASF_Get_GUID(&tw_Buf_idx, &tdw_File_Ptr, tc_GUID);
		tw_read_byte += 16;

		tdw_Obj_Sz = ASF_Get_Obj_Size(&tw_Buf_idx, &tdw_File_Ptr);
		tw_read_byte += 8;

		if( !memcmp(tc_GUID, &ASF_GUID[FILE_PROPS_OBJ_GUID][0], 16) )
		{
			ASF_Flush_Byte(&tw_Buf_idx, &tdw_File_Ptr, 40);
			tw_read_byte += 40;

			tdw_ASF_PlayDuration_L = ASF_Get_DWord(&tw_Buf_idx, &tdw_File_Ptr);
			tdw_ASF_PlayDuration_H = ASF_Get_DWord(&tw_Buf_idx, &tdw_File_Ptr);
			tw_read_byte += 8;			

			ASF_Flush_Byte(&tw_Buf_idx, &tdw_File_Ptr, 8);
			tw_read_byte += 8;
			
			tdw_ASF_Preroll_L = ASF_Get_DWord(&tw_Buf_idx, &tdw_File_Ptr);
			tdw_ASF_Preroll_H = ASF_Get_DWord(&tw_Buf_idx, &tdw_File_Ptr);
			tw_read_byte += 8;   // 081015 YC add
			tdw_ASF_PlayDuration = (tdw_ASF_PlayDuration_H << 24) | (tdw_ASF_PlayDuration_L >> 8); // ycc 081015
			gw_TotalSec = (U16)((tdw_ASF_PlayDuration / 78125)*2) - (U16)(tdw_ASF_Preroll_L / 1000);// ycc 081015
			
			tc_ASF_State_Flag++;
		}

		else if( !memcmp(tc_GUID, &ASF_GUID[STRM_PROPS_OBJ_GUID][0], 16) )
		{			
			
			ASF_Get_GUID(&tw_Buf_idx, &tdw_File_Ptr, tc_GUID);
			tw_read_byte += 8;

			if( !memcmp(tc_GUID, &ASF_GUID[AUDIO_MEDIA_OBJ_GUID][0], 16) )
			{
				ASF_Flush_Byte(&tw_Buf_idx, &tdw_File_Ptr, 42);
				tw_read_byte += 42;

				tw_ASF_Sample_Rate = ASF_Get_DWord(&tw_Buf_idx, &tdw_File_Ptr);
				tw_read_byte += 4;
				
				gs_System_State.w_BitRate = ASF_Get_DWord(&tw_Buf_idx, &tdw_File_Ptr) / 125;
				tw_read_byte += 4;			

				tc_ASF_State_Flag++;
			}
		}

		if(tw_ASF_Sample_Rate==44100||tw_ASF_Sample_Rate==22050||tw_ASF_Sample_Rate==11025)//(Jimi 091027)select Audio PLL freq
			GLOBAL_REG[0x46] |=0x04; 
		else
			GLOBAL_REG[0x46] &=0xFB; 

		if(tc_ASF_State_Flag >= 2)
			break;

		ASF_Flush_Byte(&tw_Buf_idx, &tdw_File_Ptr, tdw_Obj_Sz-(U32)tw_read_byte);
		tw_read_byte = 0;

		if((gc_CurrentCard==2) && (SD_Detect==1))  //20090803 chiayen add
		{
			break;
		}

		if((gc_CurrentCard==5) && (gb_HostConnect==0))  //20090803 chiayen add
		{			
			break;		
		}	

	}	

	return ASF_PARSE_DONE;		//Jimi 081112
}


void ASF_Get_GUID(U16 *tpw_Buf_idx, U32 *tpdw_File_Ptr, U8 tc_GUID[])
{
	U16 tw_Buf_Remain_Data;

	tw_Buf_Remain_Data = 512 - *tpw_Buf_idx;

	if( tw_Buf_Remain_Data > 16 )
	{
		memcpy(&tc_GUID[0], &gc_PlayRecordDataBuf[*tpw_Buf_idx], 16);
		*tpw_Buf_idx += 16;
	}

	else
	{
		memcpy(&tc_GUID[0], &gc_PlayRecordDataBuf[*tpw_Buf_idx], tw_Buf_Remain_Data);
		DOS_Read_File(C_MusicFHandle);
		*tpw_Buf_idx = 0;
		(*tpdw_File_Ptr) += 512;
		memcpy(&tc_GUID[tw_Buf_Remain_Data], &gc_PlayRecordDataBuf[*tpw_Buf_idx], 16 - tw_Buf_Remain_Data);//(JC)H1020 the line above overwrite object GUID		
		(*tpw_Buf_idx) += 16 - tw_Buf_Remain_Data;	
	}

}


U32 ASF_Get_Obj_Size(U16 *tpw_Buf_idx, U32 *tpdw_File_Ptr)
{
	U16 tw_Buf_Remain_Data;
	U32 tdw_Obj_Sz;
	U8 tc_i;
	
	tw_Buf_Remain_Data = 512 - *tpw_Buf_idx;
	tdw_Obj_Sz = 0;

	for( tc_i = 0; tc_i < 8; tc_i++ )
	{
		if(tc_i < 4)
			tdw_Obj_Sz = (tdw_Obj_Sz >> 8) + ((U32)gc_PlayRecordDataBuf[*tpw_Buf_idx]<<24);
	
		(*tpw_Buf_idx)++;
		if(*tpw_Buf_idx>=512)
		{
			DOS_Read_File(C_MusicFHandle);
			*tpw_Buf_idx = 0;
			*tpdw_File_Ptr += 512;
		}
	}
	
	return tdw_Obj_Sz;
}


U32 ASF_Get_DWord(U16 *tpw_Buf_idx, U32 *tpdw_File_Ptr)
{
	U32 tdw_Get_DWord;	
	U16 tw_Buf_Remain_Data;
	U8 tc_i;

	tw_Buf_Remain_Data = 512 - *tpw_Buf_idx;
	tdw_Get_DWord = 0;

	for( tc_i = 0; tc_i < 4; tc_i++ )
	{
		tdw_Get_DWord = (tdw_Get_DWord >> 8) + ((U32)gc_PlayRecordDataBuf[*tpw_Buf_idx]<<24);
			
		(*tpw_Buf_idx)++;
		if(*tpw_Buf_idx>=512)
		{
			DOS_Read_File(C_MusicFHandle);
			*tpw_Buf_idx = 0;
			*tpdw_File_Ptr += 512;
		}
	}
	
	return tdw_Get_DWord;
}


void ASF_Flush_Byte(U16 *tpw_Buf_idx, U32 *tpdw_File_Ptr, U32 tdw_flush_byte)
{
	
	U16 tw_Buf_Remain_Data;

	tw_Buf_Remain_Data = 512 - *tpw_Buf_idx;

	while( tdw_flush_byte >=512)
	{
		DOS_Read_File(C_MusicFHandle);
		tdw_flush_byte -= 512;
		*tpdw_File_Ptr += 512;
	}

	if(tdw_flush_byte + *tpw_Buf_idx >= 512)
	{
		tdw_flush_byte -= (U32)tw_Buf_Remain_Data;
		DOS_Read_File(C_MusicFHandle);
		*tpdw_File_Ptr += 512;
		*tpw_Buf_idx = 0;
	}

	(*tpw_Buf_idx) += (U16)tdw_flush_byte;
	
}
/*
void RandomGetFileIndex(void)
{
    xdata	U16 tw_Num;

    tw_Num=(gw_Random_Timer%gw_FileTotalNumber[gs_System_State.c_FileHandle])+1;

    if(gw_FileIndex[gs_System_State.c_FileHandle]==tw_Num)
    {
        gw_FileSkipNumber = 0;
    }
    else
    {
        if (gw_FileIndex[gs_System_State.c_FileHandle] > tw_Num)
        {
            gw_FileSkipNumber=gw_FileTotalNumber[gs_System_State.c_FileHandle] - (gw_FileIndex[gs_System_State.c_FileHandle] - tw_Num);
        }
        else
        {
            gw_FileSkipNumber=(tw_Num - gw_FileIndex[gs_System_State.c_FileHandle]);
        }
    }
}*/
