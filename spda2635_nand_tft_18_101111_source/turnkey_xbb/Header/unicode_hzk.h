
#ifndef	_UNICODE_HZK_H_
#define _UNICODE_HZK_H_



//==============================================================
// Function Prototype
//==============================================================

#ifdef PLAY_PROCESS
#define NEW_NAME(name)		name##_PLAY
#endif
#ifdef DECODE_MP3
#define NEW_NAME(name)		name##_MP3
#endif
#ifdef DECODE_WMA
#define NEW_NAME(name)		name##_WMA
#endif
#ifdef DECODE_APE
#define NEW_NAME(name)		name##_APE
#endif
#ifdef DECODE_FLAC
#define NEW_NAME(name)		name##_FLAC
#endif
#ifdef ENCODE_ADPCM
#define NEW_NAME(name)		name##_REC
#endif
#ifdef DECODE_ADPCM
#define NEW_NAME(name)		name##_VOICE
#endif
#ifdef DECODE_JPEG
#define NEW_NAME(name)		name##_JPEG
#endif
#ifdef DECODE_MJPEG
#define NEW_NAME(name)		name##_JPEG
#endif
#ifdef IDLE_PROCESS
#define NEW_NAME(name)		name##_IDLE
#endif
#ifdef MENU_PROCESS
#define NEW_NAME(name)		name##_MENU
#endif
#ifdef PLAY_FM
#define NEW_NAME(name)		name##_FM
#endif
#ifdef PHONE_BOOK
#define NEW_NAME(name)		name##_TEL
#endif
#ifdef TEXT_READER
#define NEW_NAME(name)		name##_TXT
#endif
#ifdef SETTING
#define NEW_NAME(name)		name##_SET
#endif
#ifdef LRC_FULLDisplay
#define NEW_NAME(name)		name##_FULL_LRC
#endif
#ifdef COLOR_TEST
#define NEW_NAME(name)		name##_TEST
#endif
#ifdef DIR_MENU_PROCESS
#define NEW_NAME(name)		name##_DIR
#endif
#ifdef Other_PROCESS
#define NEW_NAME(name)		name##_OTHER
#endif

#ifdef NEW_NAME
#define GENERATE_SHARED_CODE
#endif

#ifdef GENERATE_SHARED_CODE

#define	UNICODE_READ_HZK_SECTOR		NEW_NAME(UNICODE_READ_HZK_SECTOR)
#define	UNICODE_HZK_GET_BMP			NEW_NAME(UNICODE_HZK_GET_BMP)

void UNICODE_READ_HZK_SECTOR(U16 tw_SectorCount);
U8 UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tbt_UnicodeOrISN);

#endif // GENERATE_SHARED_CODE

#endif	// _DSPPHYSIC_H  
