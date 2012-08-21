#include "..\UI_header\UI_FuncInclude.h"
//#include "..\LCM\coordinate.h"
#include "..\header\variables.h"
//extern xdata U16 gw_font_sector;

static U8 code char_set_bytes[] = 
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
    SBCS,	// Turkey
};

static U16 code gwa_trans_table_sector[] =
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
    CP1254_TAB_SECT,    // Turkey

};

extern xdata U8  gc_BmpString[];
extern xdata U16 gw_BmpInx;
extern xdata U8 gc_MenuHZK;
extern xdata U8 gb_BuffTxt;//yflin080930 Buffer address 0:0xA800  1:0x7500

extern void LCM_set_view(U8 view_type, U16 x_start, U16 y_start, U16 x_size, U16 y_size);
extern void TFT_disp_HZKCharBMP(U8 tc_Width,U8 reverse,U8 xdata *BMPdataBuf);
extern void MenuString2BMP(U8 reverse,U8 *tpc_DataBuf,U16 *tw_StrOut);
extern void LCD_DisplayIcon(U8 tc_IconIDNumber,U8 tc_CurrentOffset);

//#ifdef GENERATE_SHARED_CODE
static void UNICODE_READ_HZK_SECTOR(U16 tw_SectorCount)
{
	DSPHZK_Block_Read(1,tw_SectorCount,1,gc_PlayRecordDataBufHB);
}

//! Get font bitmap according the character code.
/*!	Get font bitmap.
 *	\param	tc_HighByte		High byte of unicode or ISN.
 *	\param	tc_LowByte		Low byte of unicode or ISN
 *	\param	tc_BmpBuf		The buffer point of the Bmp data
 *	\param	tc_ANSI			To indicate the low and high byte is ISN or unicode,1: ANSI ASCII; 0: unicode
 *	\return	Bits to indicate:
 *			- bit7: indicate the ISN is one or two bytes.0:one byte; 1: two bytes;
 *			- bit6~bit0: the valid column number of the BmpData
 */
static U8 UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tc_ANSI)
{
	U8 tc_Loop;
	U8 tc_Byte;
	U8 tc_ValidColumn;
	U8 tc_LookupAttribute;
	U8 tc_TwoByteFlag;
	U16 tw_OffsetSector;
	U16 tw_ByteOffset;
	U16 tw_UnicodeTabOffset;
	U16 tw_unicode;
	U8 xdata * p;

	tc_TwoByteFlag = 0x80;
	//-------------------------------------------------------------------
	// if it is ISN and high byte's bit7 is 1, must convert it to Unicode
	//-------------------------------------------------------------------
	if(tc_ANSI)
	{
		if(tc_HighByte & 0x80)
		{//if it is ISN,try to convert to unicode
			tc_LookupAttribute = char_set_bytes[gc_MenuHZK];
			if (gc_MenuHZK == LANG_JAP)//Language_Japanese)
			{//if it is not japanese
				if ((tc_HighByte >= 0xA1) && (tc_HighByte <= 0xDF))
				{//if it is in single byte area,pad 0 to low byte
					tc_LowByte = 0;
					tc_TwoByteFlag = 0x00;// next onebyte
				}
			}
			else
			{//if it is not japanese
				if (tc_LookupAttribute == 0x00)
				{//if attribute is 0x00, ISN is single byte
					tc_TwoByteFlag = 0;
				}
			}
				
			if (tc_LookupAttribute)
			{//if attribute is 0x01, ISN is double bytes
				tw_UnicodeTabOffset = (((U16)(tc_HighByte & 0x7f) << 8) | tc_LowByte);
			}
			else
			{//if attribute is 0x00, ISN is single byte
				tw_UnicodeTabOffset = (tc_HighByte);
			}
			//---------------- read unicode of the ISN word in table --------------------------------------------------------------------------
			// tw_UnicodeTabOffset is the index in word to look up unicode.
			tw_OffsetSector = (tw_UnicodeTabOffset >> 8) + gwa_trans_table_sector[gc_MenuHZK];//get the start offset sector 
			UNICODE_READ_HZK_SECTOR(tw_OffsetSector);//read the start sector
	
			tw_ByteOffset = (U16)((U8)tw_UnicodeTabOffset) * 2;//get the start offset byte in secotr
			tc_HighByte = gc_PlayRecordDataBuf[tw_ByteOffset];
			tc_LowByte = gc_PlayRecordDataBuf[tw_ByteOffset+1];
		}
		else /*if(tc_HighByte & 0x80)*/
		{//if it is ASCII,convert high byte to unicode
			tc_LowByte = tc_HighByte;
			tc_HighByte = 0;
			tc_TwoByteFlag = 0;
		}
	}//end of if(tc_ANSI)
	//--------------------------------------------------------------------
	// according to unicode,find the zk bmp data
	//--------------------------------------------------------------------

	tw_unicode = (tc_HighByte<<8) | tc_LowByte;
	tw_OffsetSector = (tw_unicode/15) + gw_font_sector;
	UNICODE_READ_HZK_SECTOR(tw_OffsetSector);

	tw_ByteOffset = ((U8)(tw_unicode%15))*33;
	tc_ValidColumn = gc_PlayRecordDataBuf[tw_ByteOffset];//record the valid column in the bmp display
	tc_Byte=tc_ValidColumn+tc_ValidColumn;
	gw_BmpInx += tc_Byte;
	tc_Loop = tc_Byte;
	p = &(gc_PlayRecordDataBuf[tw_ByteOffset+1]);

	do {
		*tc_BmpBuf++ = *p++;//get one byte
	} while (--tc_Loop);

	return tc_TwoByteFlag|tc_ValidColumn;
}


