/******************************************************************************
* How to access the hzk file.
*
* Based on this file and some guess, the dot matrix data is made by a modulo:
*       vertical, negative
* 
* But we are using a LCM which displays by:
*       horizonal, positive
*
* So we probably need to cook the dot matrix data read from the hzk file.
******************************************************************************/
#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\IR\LCM_BMP.h"

xdata U8 gc_CurrentHZK; //20090107 chiayen add

void LCM_UNICODE_READ_HZK_SECTOR(U16 tw_SectorCount)
{
	DSPHZK_Block_Read(1,tw_SectorCount,1,gc_PlayRecordDataBufHB);
}
//=============================================================================================================================
//UNICODE_HZK_GET_BMP()
//PARA:
//		tc_HighByte: high byte of unicode or ISN			
//		tc_LowByte:  low byte of unicode or ISN 
//		tc_BmpBuf:	 the buffer point of the Bmp data
//		tbt_UnicodeOrISN: indicate the low and high byte is ISN or unicde,0: ISN; 1: unicode
//return value:
//		bit7: indicate the ISN is one or two bytes.0:one byte; 1: two bytes;
//		bit6~bit0: the valid column number of the BmpData
//=============================================================================================================================
U8 LCM_UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tbt_UnicodeOrISN)
{	
	U8 tc_Loop;
	U8 tc_MaxLoop;
	U8 tc_ValidColumn;
	U8 tc_BmpAttribute;
	U8 tc_Lookup_TotalNum;
	U8 tc_LookupAttribute;
	U8 tc_TwoByteFlag=0x80;
	U16 tw_OffsetSector;
	U16 tw_ByteOffset;
	U16 tw_UnicodeTabOffset;
	U32 tdw_UnicodeZKOffset;
	U32 tdw_UnicodeZKStartAddr;
	U32 tdw_LookupStartAddr;	
	//-------------------------------------------------------------------
	//1.read unicode header information 
	//------------------------------------------------------------------
	LCM_UNICODE_READ_HZK_SECTOR(1);
	tc_BmpAttribute = gc_PlayRecordDataBuf[0];

	((U8 *)(&tdw_UnicodeZKStartAddr))[0] = gc_PlayRecordDataBuf[1];
 	((U8 *)(&tdw_UnicodeZKStartAddr))[1] = gc_PlayRecordDataBuf[2];
	((U8 *)(&tdw_UnicodeZKStartAddr))[2] = gc_PlayRecordDataBuf[3];
	((U8 *)(&tdw_UnicodeZKStartAddr))[3] = gc_PlayRecordDataBuf[4];	
	//-------------------------------------------------------------------
	//2.if it is ISN and high byte's bit7 is 1, must convert it to Unicode
	//-------------------------------------------------------------------
	if ((!tbt_UnicodeOrISN) && (tc_HighByte & 0x80))
	{//if it is ISN,try to convert to unicode
		tc_Lookup_TotalNum = gc_PlayRecordDataBuf[5];//get total number
		
		for (tc_Loop = 0; tc_Loop < tc_Lookup_TotalNum; tc_Loop ++)
		{
			tw_ByteOffset = (U16)tc_Loop * 6;
			if (gc_PlayRecordDataBuf[6 + tw_ByteOffset]	== gc_CurrentHZK)  //20090107 chiayen modify
			{//if find the special ID
				tc_LookupAttribute = gc_PlayRecordDataBuf[6 + tw_ByteOffset + 1];//get the attribute
 				
				((U8 *)(&tdw_LookupStartAddr))[0] = gc_PlayRecordDataBuf[6 + tw_ByteOffset + 2];//get lookup table start address
 				((U8 *)(&tdw_LookupStartAddr))[1] = gc_PlayRecordDataBuf[6 + tw_ByteOffset + 3];
				((U8 *)(&tdw_LookupStartAddr))[2] = gc_PlayRecordDataBuf[6 + tw_ByteOffset + 4];
				((U8 *)(&tdw_LookupStartAddr))[3] = gc_PlayRecordDataBuf[6 + tw_ByteOffset + 5];

				if (gc_CurrentHZK == LCM_ID3HZK_JAP)//Language_Japanese)  //20090107 chiayen modify
				{//if it is not japanese
					if ((tc_HighByte >= 0xA1) && (tc_HighByte <= 0xDF))
					{//if it is in single byte area,pad 0 to low byte
						tc_LowByte = 0;
						tc_TwoByteFlag = 0x00;// next onebyte
					}
					else
					{
						//it is two byte ISN to two byte Unicode
						tc_TwoByteFlag = 0x80;
					}
					tc_LookupAttribute = 0x01;
				}
				else
				{//if it is not japanese
					if (tc_LookupAttribute == 0x00)
					{//if attribute is 0x00, ISN is single byte
						tc_TwoByteFlag = 0;
					}
					else
					{//if attribute is 0x01, ISN is double bytes
						tc_TwoByteFlag = 0x80;
					}
				}
				
				if (tc_LookupAttribute)
				{//if attribute is 0x01, ISN is double bytes
					tw_UnicodeTabOffset = (((U16)(tc_HighByte & 0x7f) << 8) | tc_LowByte) << 1;
				}
				else
				{//if attribute is 0x00, ISN is single byte
					tw_UnicodeTabOffset = ((tc_HighByte & 0x7f) << 1);
				}
				//---------------- read unicode of the ISN word in table --------------------------------------------------------------------------
				tw_OffsetSector = (tdw_LookupStartAddr + tw_UnicodeTabOffset) >> 9;//get the start offset sector 
				tw_ByteOffset = (tdw_LookupStartAddr + tw_UnicodeTabOffset) & 0x1ff;//get the start offset byte in secotr
						
				LCM_UNICODE_READ_HZK_SECTOR(tw_OffsetSector+1);//read the start sector
	
				tc_HighByte = gc_PlayRecordDataBuf[tw_ByteOffset];
				tw_ByteOffset ++;

				if (tw_OffsetSector >= 512)
				{
					LCM_UNICODE_READ_HZK_SECTOR(tw_OffsetSector + 2);//read next sector
					tw_OffsetSector = 0;
				}				
				tc_LowByte = gc_PlayRecordDataBuf[tw_ByteOffset];
				break;
			}
		}		
	}
	else if (!tbt_UnicodeOrISN)
	{//if it is ASCII,convert high byte to unicode
		tc_LowByte = tc_HighByte;
		tc_HighByte = 0;
		tc_TwoByteFlag = 0;
	}
	//--------------------------------------------------------------------
	//3.according to unicode,find the zk bmp data
	//--------------------------------------------------------------------
	tdw_UnicodeZKOffset = ((U16)tc_HighByte << 8) + tc_LowByte;//calculate the offset of the word

	if (tc_BmpAttribute == 0x01)
	{//if bmp attribute is 0x01,per word bmp include 25byte(24 bmp byte + 1 flag byte) 
		tdw_UnicodeZKOffset = tdw_UnicodeZKOffset * 25;//calculate the offset of the byte	
	    tc_MaxLoop = 24;
  	}
	else if (tc_BmpAttribute == 0x00)
	{//if bmp attribute is 0x00,per word bmp include 33byte(32 bmp byte + 1 flag byte) 
		tdw_UnicodeZKOffset = tdw_UnicodeZKOffset * 33;//calculate the offset of the byte		
 	    tc_MaxLoop = 32;
	}
	else
	{//other value 
		goto HZK_END;	 
	}

	tw_OffsetSector = (tdw_UnicodeZKStartAddr + tdw_UnicodeZKOffset) >> 9;//get the start offset sector 
	tw_ByteOffset = (tdw_UnicodeZKStartAddr + tdw_UnicodeZKOffset) & 0x1ff;//get the start offset byte in secotr

	LCM_UNICODE_READ_HZK_SECTOR(tw_OffsetSector+1);//read start sector of the word bmp
	
	tc_ValidColumn = gc_PlayRecordDataBuf[tw_ByteOffset];//record the valid column in the bmp display
	tw_ByteOffset ++;

	for (tc_Loop = 0; tc_Loop < tc_MaxLoop;tc_Loop ++)
	{//get bmp data to buffer
		if (tw_ByteOffset >= 512)
		{//if the bmp datacover one sector, must read next sector and set "byte offset" to 0
			LCM_UNICODE_READ_HZK_SECTOR(tw_OffsetSector + 2);//read next sector
			tw_ByteOffset = 0;//get data from first byte of the sector
		}

		tc_BmpBuf[tc_Loop] = gc_PlayRecordDataBuf[tw_ByteOffset];//get one byte
		tw_ByteOffset ++;
	}			
	tc_TwoByteFlag=tc_TwoByteFlag|tc_ValidColumn;  
HZK_END:
	return tc_TwoByteFlag;
}