#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\LCM\LCM_BMP.h"
#include "..\header\variables.h"

#ifdef SPI_1M
////////////2M-spi hzk
#define D_font_sector		0x6  //Font BMP sector
void LCM_UNICODE_READ_HZK_SECTOR(U16 tw_SectorCount)
{
	SPI_DSPHZK_Block_Read(1,tw_SectorCount,gc_PlayRecordDataBufHB);
}
//U8 UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tbt_UnicodeOrISN)
U8 LCM_UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tbt_UnicodeOrISN)
{
	U16	tw_hzkbyteoffest = 0,i;
	U16	tw_hzksectoroffest = 0;
	U32	ShowPointer;

	if(tc_HighByte&0x80)
	{
		if(tc_HighByte<0xa1 || tc_LowByte<0xa1 || tc_HighByte>=0xf8)
		{
			goto CantFindHZ;
		}

		ShowPointer = (((U32)tc_HighByte-0xa1)*94+((U32)tc_LowByte-0xa1))*24;
		
		tw_hzksectoroffest = (U16)(ShowPointer/512);
		tw_hzkbyteoffest = (U16)(ShowPointer%512);

		LCM_UNICODE_READ_HZK_SECTOR(D_font_sector+tw_hzksectoroffest);
		
		if(tw_hzkbyteoffest<(512-24))
		{
			for(i=0;i<24;i++)
			{
				*(tc_BmpBuf+i) = gc_PlayRecordDataBuf[tw_hzkbyteoffest+i];
			}
		}
		else
		{
			for(i=0;i<(512-tw_hzkbyteoffest);i++)
			{
				*(tc_BmpBuf+i) = gc_PlayRecordDataBuf[tw_hzkbyteoffest+i];
			}

			LCM_UNICODE_READ_HZK_SECTOR(D_font_sector+tw_hzksectoroffest+1);

			for(i=0;i<(24-(512-tw_hzkbyteoffest));i++)
			{
				*(tc_BmpBuf+((512-tw_hzkbyteoffest)+i)) = gc_PlayRecordDataBuf[i];
			}
		}
//		gw_BmpInx += 24;
		return 0x80|12;
	}
	else
	{
		if(!tc_HighByte) goto CantFindHZ;			
	
		ShowPointer=tc_HighByte*24;

		tw_hzksectoroffest = (U16)(ShowPointer/512);
		tw_hzkbyteoffest = (U16)(ShowPointer%512);

		LCM_UNICODE_READ_HZK_SECTOR(tw_hzksectoroffest);
		
		if(tw_hzkbyteoffest<(512-24))
		{
			for(i=0;i<24;i++)
			{
				*(tc_BmpBuf+i) = gc_PlayRecordDataBuf[tw_hzkbyteoffest+i];
			}
		}
		else
		{
			for(i=0;i<(512-tw_hzkbyteoffest);i++)
			{
				*(tc_BmpBuf+i) = gc_PlayRecordDataBuf[tw_hzkbyteoffest+i];
			}

			LCM_UNICODE_READ_HZK_SECTOR(tw_hzksectoroffest+1);

			for(i=0;i<(24-(512-tw_hzkbyteoffest));i++)
			{
				*(tc_BmpBuf+((512-tw_hzkbyteoffest)+i)) = gc_PlayRecordDataBuf[i];
			}
		}
//		gw_BmpInx += 24;
		return 0x00|10;	 //英文字母不显示最后两列，以便美观些。-sunzhk add
	}

CantFindHZ:
	for(tc_HighByte = 0; tc_HighByte < 24; tc_HighByte++)
	{
		*(tc_BmpBuf+tc_HighByte) = 0;
	}

//	gw_BmpInx += 24;
	return 0x80|12;
}
////////////4M-spi hzk
#else
void LCM_UNICODE_READ_HZK_SECTOR(U16 tw_SectorCount)
{
	SPI_DSPHZK_Block_Read(1,tw_SectorCount,gc_PlayRecordDataBufHB);
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
	#define D_font_sector		0x125  //Font BMP sector

	#define SBCS	0
	#define DBCS	1

	#define CP1250_TAB_SECT			1
	#define CP1251_TAB_SECT			2
	#define CP1252_TAB_SECT			3
	#define CP1253_TAB_SECT			4
	#define CP1254_TAB_SECT			5
	#define CP1255_TAB_SECT			6
	#define CP1256_TAB_SECT			7
	#define CP1257_TAB_SECT			8
	#define CP1258_TAB_SECT			9
	#define CP874_TAB_SECT			10
	#define CP932_TAB_SECT			11	// Shift JIS
	#define CP936_TAB_SECT			44	// GBK
	#define CP949_TAB_SECT			134 // for Korean
	#define CP950_TAB_SECT			227	// Big5

U8 code char_set_bytes[] = 
{
	DBCS, 	// Simplified Chinese
	DBCS, 	// Traditional Chinese
	SBCS,	// English
	DBCS,	// Japense
	DBCS,	// Korean
	SBCS,	// German
	SBCS,	// French
	SBCS,	// Italian
	SBCS,	// Spanish
	SBCS,	// Swedish
	SBCS,	// Czech
	SBCS,	// Dutch
	SBCS,	// Portuguese
	SBCS,	// Danish
	SBCS,	// Polish
	SBCS,	// Russian	
};

U8 code gwa_trans_table_sector[] =
{
	CP936_TAB_SECT, 	// Simplified Chinese
	CP950_TAB_SECT, 	// Traditional Chinese
	CP1252_TAB_SECT,	// English
	CP932_TAB_SECT,		// Japense
	CP949_TAB_SECT,		// Korean
	CP1252_TAB_SECT,	// German
	CP1252_TAB_SECT,	// French
	CP1252_TAB_SECT,	// Italian
	CP1252_TAB_SECT,	// Spanish
	CP1252_TAB_SECT,	// Swedish
	CP1250_TAB_SECT,	// Czech
	CP1252_TAB_SECT,	// Dutch
	CP1252_TAB_SECT,	// Portuguese
	CP1252_TAB_SECT,	// Danish
	CP1250_TAB_SECT,	// Polish
	CP1251_TAB_SECT,	// Russian
};


U8 LCM_UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tbt_UnicodeOrISN)
{
	data	U8	tc_Loop;
	data	U8	tc_ValidColumn;
	data	U8	tc_LookupAttribute;
	data	U8	tc_TwoByteFlag;
	data	U16	tw_OffsetSector;
	data	U16	tw_ByteOffset;
	data	U16	tw_UnicodeTabOffset;
	data	U16	tw_unicode;
	xdata	U16	tw_tmp;
	xdata	U8	xdata * p;

	tc_TwoByteFlag=0x80;
	tw_OffsetSector=0;
	tc_Loop=0;
	
	//-------------------------------------------------------------------
	// if it is ISN and high byte's bit7 is 1, must convert it to Unicode
	//-------------------------------------------------------------------
	if(!tbt_UnicodeOrISN)
	{		
		if(tc_HighByte&0x80)
		{//if it is ISN,try to convert to unicode
			tc_LookupAttribute=char_set_bytes[gc_CurrentHZK];
			if(gc_CurrentHZK==LANG_JAP)
			{//if it is not japanese
				if((tc_HighByte>=0xA1)&&(tc_HighByte<=0xDF))
				{//if it is in single byte area,pad 0 to low byte
					tc_LowByte=0;
					tc_TwoByteFlag=0x00;// next onebyte
				}
			}
			else
			{//if it is not japanese
				if(tc_LookupAttribute==0x00)
				{//if attribute is 0x00, ISN is single byte
					tc_TwoByteFlag=0;
				}
			}
		
			//read table first 
			LCM_UNICODE_READ_HZK_SECTOR(gwa_trans_table_sector[gc_CurrentHZK]);	
			tw_ByteOffset=(U16)(tc_HighByte)*2;	//get the start offset byte in secotr 
														//tc_LookupAttribute=1: information is sector 
														//tc_LookupAttribute=0: information is Unicode			
			if(tc_LookupAttribute)
			{//if attribute is 0x01, ISN is double bytes
				//---------------- read unicode Table sector -------------------
				tw_UnicodeTabOffset=*(U16 *)(&gc_PlayRecordDataBuf[tw_ByteOffset]);
				tw_UnicodeTabOffset=tw_UnicodeTabOffset+(U16)(tc_LowByte-0x40)*2 ;

				tw_OffsetSector=(tw_UnicodeTabOffset>>9)+gwa_trans_table_sector[gc_CurrentHZK];//get the start offset sector
 				tw_ByteOffset=tw_UnicodeTabOffset&0x1FF;
				LCM_UNICODE_READ_HZK_SECTOR(tw_OffsetSector);//read the corect sector
			}

			tc_HighByte=gc_PlayRecordDataBuf[tw_ByteOffset];
			tc_LowByte=gc_PlayRecordDataBuf[tw_ByteOffset+1];
		}
		else /*if(tc_HighByte & 0x80)*/
		{//if it is ASCII,convert high byte to unicode
			tc_LowByte=tc_HighByte;
			tc_HighByte=0;
			tc_TwoByteFlag=0;
		}
	}//end of if(tbt_UnicodeOrISN)
	//--------------------------------------------------------------------
	// according to unicode,find the zk bmp data
	//--------------------------------------------------------------------

	tw_unicode=(tc_HighByte<<8)|tc_LowByte;
	//read table 
	LCM_UNICODE_READ_HZK_SECTOR(D_font_sector/*gw_font_sector*/);
	
	tw_tmp=(U16)(tc_HighByte)<<1;
	tw_ByteOffset=*(U16 *)(&gc_PlayRecordDataBuf[tw_tmp]);
	tw_OffsetSector=tw_ByteOffset+D_font_sector+(tc_LowByte/16); //538;
	LCM_UNICODE_READ_HZK_SECTOR(tw_OffsetSector);
	tw_ByteOffset=((U16)(tc_LowByte&0x0F))<<5;	
	p=&(gc_PlayRecordDataBuf[tw_ByteOffset]); 

	if((p[16]==0xAA)&&(p[17]==0xBB))
	{
		tc_ValidColumn=8;
		tc_Loop=16;
	}
	else
	{
		tc_ValidColumn=16;
		tc_Loop=32;	
	}

	do 
	{
		*tc_BmpBuf++=*p++;//get one byte
	} while (--tc_Loop);


	return tc_TwoByteFlag|tc_ValidColumn;
}
#endif
