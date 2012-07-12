/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	dsp.h
 *	\brief	DSP register and DSP-MCU interface definitions.
 *
 *	\author	
 */

#ifndef _DSP_H_
#define _DSP_H_

//////////////////////////////////////////////////////////////////////
// DSP BUF REGISTERS
//////////////////////////////////////////////////////////////////////
//#define		REG_DSPBUFDATA			0xB210
//#define		REG_DSPPMDATA			0xB211
//#define		REG_DSPBUFIDX7_0		0xB212
//#define		REG_DSPBUFIDX11_8		0xB213
#define		REG_DSPDMIDX7_0			0xB217
#define		REG_DSPDMIDX12_8		0xB218

#define     REG_DSPACCESSMODE       0xB216
#define		REG_DSPBCLR				0xB216
#define		REG_DSPBADDRS			0xB217
#define		REG_DSPMODE				0xB218
#define		REG_DSPPMIDX7_0			0xB219
#define		REG_DSPPMIDX13_8		0xB21A

#define		REG_DSPPMIDX7_0			0xB219
#define		REG_DSPPMIDX12_8		0xB21A

#define		REG_DSPIMIDX7_0			0xB21B
#define		REG_DSPIMIDX13_8		0xB21C

#define		REG_DSPDMOFFSET			0xB21D
#define		REG_DSPPMOFFSET			0xB21E
#define		REG_DSPIMOFFSET			0xB21F

#define		REG_DSPBUFREQ			0xB220
#define		REG_DSPBUFACK			0xB221

#define		REG_DSPBUFBIPI7_0		0xB2A0
#define		REG_DSPBUFBIPI11_8		0xB2A1
#define		REG_DSPBUFBIPO7_0		0xB2A2
#define		REG_DSPBUFBIPO11_8		0xB2A3

#define		REG_DSPPMIP7_0			0xB2A4
#define		REG_DSPPMIP13_8			0xB2A5
#define		REG_DSPPMOP7_0			0xB2A6
#define		REG_DSPPMOP13_8			0xB2A7
#define		REG_DSPIMIP7_0			0xB2A8
#define		REG_DSPIMIP13_8			0xB2A9
#define		REG_DSPIMOP7_0			0xB2AA
#define		REG_DSPIMOP13_8			0xB2AB


#define L2K_DSP24MIPS	0x01
#define L2K_DSP30MIPS	0x02
#define L2K_DSP36MIPS	0x03
#define L2K_DSP42MIPS	0x04
//-------------------------- SPCA759 -----------------------------------------
#define L2K_DSP16MIPS	0x05
#define L2K_DSP18MIPS	0x06
#define L2K_DSP32MIPS	0x07
#define L2K_DSP48MIPS	0x08
#define L2K_DSP03MIPS	0x09

#define L2K_MCU24MIPS	0x01		// Same as L2K_DSP24MIPS
#define L2K_MCU16MIPS	0x05
#define L2K_MCU15MIPS	0x06
#define L2K_MCU12MIPS	0x07
#define L2K_MCU08MIPS	0x08
#define L2K_MCU06MIPS	0x09

#define L2K_MCU18to06	0x00
#define L2K_MCU06to18	0x01
#define L2K_MCU15to06	0x02
#define L2K_MCU06to15	0x03
#define L2K_MCU24to06	0x04
#define L2K_MCU06to24	0x05
#define L2K_MCU24to375	0x06

/*------------- DSP Command ----------------*/
#define  DSP_STUFF_DATA                  0x00
#define  DSP_DATA_IN_CMD                 0x01
#define  DSP_DATA_OUT_CMD                0x02
#define  DSP_END_OF_FILE_CMD             0x03
#define  DSP_INITPLAY_CMD                0x04
#define  DSP_CONTINUE_PLAY_CMD           0x05
#define  DSP_PLAY_CMD                    0x06
#define  DSP_PLAY_PAUSE_CMD              0x07
#define  DSP_RECORD_PAUSE_CMD            0x08
#define  DSP_STOP_CMD                    0x09
#define  DSP_INITRECORD_CMD              0x0A
#define  DSP_RECORD_CMD                  0x0B
#define  DSP_SET_DEFAULT_EQ_CMD          0x0C
#define  DSP_USER_SET_EQ_VALUE0_CMD      0x0D
#define  DSP_USER_SET_EQ_VALUE1_CMD      0x0E
#define  DSP_USER_SET_EQ_VALUE2_CMD      0x0F
#define  DSP_USER_SET_EQ_VALUE3_CMD      0x10
#define  DSP_USER_SET_EQ_VALUE4_CMD      0x11
#define  DSP_USER_SET_EQ_VALUE5_CMD      0x12
#define  DSP_USER_SET_EQ_VALUE6_CMD      0x13
#define  DSP_USER_SET_EQ_ENABLE_CMD      0x14
#define  DSP_SET_PLAY_DPC_CMD            0x15
#define  DSP_SET_VOLUME_CMD              0x16
#define  DSP_SPECTRUM_CMD                0x17
#define  DSP_SURROUND_CMD                0x18
#define  DSP_ANTICLIPPING_CMD            0x19
#define  DSP_INITRECORD_FM_CMD           0x1A
#define  DSP_EXIT_FM_CMD                 0x1B
#define  DSP_READ_TIME_CMD               0x1C
#define  DSP_READ_BITRATE_CMD            0x1D
#define  DSP_READ_EMPTY_BUFFER_CMD       0x1E
#define  DSP_READ_REMAIN_BUFFET_CMD      0x1F

/*------------- DSP STATUSE ----------------*/
#define  DSP_SUCCESS                     0x00
#define  DSP_USER_FILE_TYPE_ERROR        0x01
#define  DSP_IMPM_INEXISTENCE_ERROR      0x02
#define  DSP_CHECK_SUNPLUSKEY_ERROR      0x03
#define  DSP_CMD_EXE_STATUS0_ERROR       0x04
#define  DSP_PLAY_COMMAND_ERROR          0x05
#define  DSP_PAUSE_COMMAND_ERROR         0x06
#define  DSP_STOP_COMMAND_ERROR          0x07
#define  DSP_RECORD_COMMAND_ERROR        0x08
#define  DSP_DECODE_STATUS_TIMEOUT_ERROR 0x09
#define  DSP_DATAIN_COMMAND_ERROR        0x0A
#define  DSP_DATAOUT_COMMAND_ERROR       0x0B
#define  DSP_RUNNING_STATUS1_ERROR       0x0C
// Added by Mark@20080617 for file end status
#define  DSP_DECODE_END                  0x0D

/*------------- DSP STATE -----------------*/
#define  DSP_SLEEP_STATE                 0x00
#define  DSP_INIT_PLAY_STATE             0x01
#define  DSP_PLAY_STATE                  0x02
#define  DSP_PLAY_PAUSE_STATE            0x03
#define  DSP_RECORD_PAUSE_STATE          0x04
#define  DSP_INIT_RECORD_STATE           0x05
#define  DSP_RECORD_STATE                0x06

/*----------- DSP Codec Mode --------------*/
#define  DSP_MP3_FILE_TYPE               0x00
#define  DSP_WMA_FILE_TYPE               0x01
#define  DSP_WAV_FILE_TYPE               0x02
#define  DSP_ASF_FILE_TYPE               0x03
#define  DSP_JPG_FILE_TYPE               0x06		//Jimi 080225
#define  DSP_MP3D_MODE                   0x00
#define  DSP_WMAD_MODE                   0x01
#define  DSP_DVRD_WAV_MODE               0x02
/*------------ DSP Error Code -------------*/
#define C_DSP_NoError                    0x00
#define C_DSP_CheckSunplusFail           0x01
#define C_DSP_CheckReadyFail             0x02
#define C_DSP_PlayCommandFail            0x03
#define C_DSP_PauseCommandFail           0x04
#define C_DSP_StopCommandFail            0x05
#define C_DSP_DatInCommandFail           0x06
#define C_DSP_DatOutCommandFail          0x07
#define C_DSP_ReadMemError               0x08
#define C_DSP_SendCMDError               0x09
#define C_DSP_WriteMemError              0x0A
#define C_DSP_SuspendCommandFail         0x0B

//------------------------------ Periphal clk  -----------------------------------

#define PERIPH_IDLE           0x00
#define PERIPH_UINORMALDSP    0x01
#define PERIPH_PLAYWMA        0x02
#define PERIPH_USB            0x04

#define DSPLIB_RELEASE		1
#define K_DLASH /

#if DSPLIB_RELEASE
#define M_DSP_Printf;/K_DLASH
#else
#define M_DSP_Printf printf
#endif

//#include "DSPdevice.h"
//------DSP EQ Type ---
#define	K_EQType_NORMAL 	0x00
#define	K_EQType_DBB		0x01
#define	K_EQType_POP		0x02
#define	K_EQType_ROCK		0x03
#define	K_EQType_CLASSIC	0x04
#define	K_EQType_JAZZ		0x05

//--------------------------------------------------------------------------------------------
#define LDSP_SUCCESS                     0x00
#define LDSP_ERROR_GENERAL               0x01
#define LDSP_ERROR_TIMEOUT	             0x02
#define LDSP_ERROR_BUSY		             0xFE
//--------------------------------------------------------------------------------------------
//----------------------------
#define DSP_ADLSB	XBYTE[0x2E40]
#define DSP_ADMSB	XBYTE[0x2E41]

#define DSP_DBusH 	XBYTE[0x2E44]
#define DSP_DBusM 	XBYTE[0x2E43]
#define DSP_DBusL 	XBYTE[0x2E42]
#define DSP_RWCtrl  XBYTE[0x2E45]
//----------------------------
#define DSP_CMDIn	XBYTE[0x2E20]		//
//#define DSP_CMDInMSB	XBYTE[0x2E21]
#define DSP_CMDOut	XBYTE[0x2E22]
//#define DSP_CMDOutMSB	XBYTE[0x2E23]
#define DSP_CMDAck	XBYTE[0x2E24]
//-------------------------------
//#define EV_Chip_For_DSP
#ifdef EV_Chip_For_DSP
#define DSP_Dummy   XBYTE[0x2E46]
//#define DSP_Dummy   XBYTE[0x3346]
#endif
//--- Define The Register address inside DSP
//--- These Address is not directly access by MCU,but access from DSP.
#define DSP_WMACrtlReg0	0x3937			// WMA control Register

#define DSP_VersionID	0x3CF6			// Get DSP Version
#define DSP_CMDCheck	0x3CAF			// DSP Command Checker for MCU, see 0x3F40
#define DSP_InAddress	0x3E00			// DSP In Buffer Address
#define DSP_FrameSizeA	0x3E01			// DSP Frame Size Address
#define DSP_OutAddress	0x3E10			// DSP Out Buffer Address
//----------- Silence detection for Audio and Voice!
#define DSP_SilenceLV   0x3E30          // DSP Silence threshold Level, default is 0x50
#define DSP_SilenceDT   0x3E31          // DSP Silence Interval Duration, default is 0x64 ,Unit:13.5mSec
#define DSP_SilenceST   0x3E32          // DSP Silence Status Flag, bit[0]:Postprocessor, bit[1]:PreProcessor!
#define DSP_DVRDelta    0x3E37          // The Voice input differential between V(n+1)-Vn(Criterion), default is 0x30
#define DSP_UnVoxCnt    0x3E38          // The Voice input amount of differential, Need to clear for DSP
#define DSP_UnVOXState  0x3E39          // The Voice input amount of differential is larger than DSP_DVRDelta. default is 0x30(Unslience)
#define DSP_VoxCnt      0x3E3A          // The Voice input amount of differential, Need to clear for DSP
#define DSP_VOXState    0x3E3B          // The Voice input amount of differential is larger than DSP_DVRDelta, default is 63D,(silence)
//-----------
#define DSP_InPageCnt	0x3E0B			// DSP In Buffer Page Counter
#define DSP_CtrlWav		0x3E53			// Enable/Disable DSP control DVR file Header.
#define DSP_PlayDvrFrac 0x3E0C          // DSP Play DVR Frame counter
#define DSP_ResBuffer 	0x3E0D          // DSP Residual Buffer Size
#define DSP_EncDvrFrac  0x3E50          // DSP Encode DVR Frame counter
#define DSP_ErrorFrame  0x3F51          // DSP Error FrameCnt Flag
#define DSP_RampStatus  0x3E92			// DSP audio/Speech ramp status, 1:OK, 0:not yet.
//
#define DSP_CommandST	0x3F40			// DSP Command perform status
#define DSP_CMDPayload	0x3F41			// DSP Command Payload
//
#define DSP_MPEGSt		0x3F49			// DSP MPEG Decoder Status
#define DSP_FrameCnt	0x3E24			// DSP MPEG Decoder Status
#define DSP_DVRFrameCnt	0x3E50			// DSP MPEG Decoder Status

#define DSP_MPEGst		0x3F49			// The status of MPEG
#define DSP_PFST0		0x3F50			// The Program flow Status #0
#define DSP_PFST1		0x3F51			// The Program flow Status #1
#define DSP_PFST2		0x3F52			// The Program flow Status #2
#define DSP_SilenceCtrl	0x3F53			// DSP Silence Control Register
//--Audio Control
//==== APE Decoder====
#define		DSP_APECurBlockH		0x3F51		//APE Decoder current block H
#define 	DSP_APECurBlockL		0x3F52		//APE Decoder current block L
#define		DSP_APESampleRate		0x3F53		//APE Decoder sample rate
#define		DSP_APETotalBlockL		0x3F5B		//APE Decoder total block L
#define		DSP_APETotalBlockH		0x3F5C		//APE Decoder total block H
#define		DSP_APETotalFrmNum		0x3F59		//APE Decoder total frame number
#define		DSP_APERandomFlag		0x3F92 		//APE random flag
#define		DSP_APESeekDataPtrL		0x3F93 		//APE seek data ptr L
#define		DSP_APESeekDataPtrH		0x3F94 		//APE seek data ptr H
#define		DSP_APESeekFrmNum		0x3F95 		//APE seek table idx
#define		DSP_APEEOFFlag			0x3F96		//APE end of file flag


#define DSP_AudioCtrl	0x3F55			// Audio Control
#define DSP_CMDFlg		0x3F57
#define DSP_BufferCtrl	0x3F58
#define DSP_RepeatCtrl	0x3F59
#define DSP_DPCCtrl		0x3F5D
#define DSP_PlaySkipCnt	0x3F5E			// Skip Frame count for Fast Forward
#define DSP_PlayCFrCnt	0x3F5F			// Play Frame Count for Fast Forward
#define DSP_w_EntropyMode		0x3F60	// WMA Entropy mode,use to choose what huffman and run level table to download(Jimi 080523)
#define DSP_ModeReady	0x3F61			// DSP Initialize Mode for Ready
//
#define DSP_IDStrAdd	0x3F70			// DSP ID String Address
#define DSP_ChipIDAdd	0x3F7D			// DSP Chip ID String Address
#define DSP_CmdReady	0x3F7B			// DSP Command Ready
#define DSP_WMAPageCnt	0x3F98			// DSP WMA Page Counter of random access
//#define DSP_WMARndAcc	0x3F99			// DSP WMA random access flag
#define DSP_W_Total_Time_H		0x3F99	// DSP WMA total time H
#define DSP_W_Total_Time_L		0x3F9A	// DSP WMA total time L
#define DSP_WMACurrenTimeHigh	0x3F9B	// WMA play current time high byte
#define DSP_WMACurrenTimeLow	0x3F9C	// WMA play current time low byte

#define DSP_W_asf_parse_mode	0x3FA4	// WMA ASF parser mode, 0: normal wma dec, 1: asf parser only(Jimi 080523)
#define DSP_W_asf_parse_ok		0x3FA5	// WMA ASF parse OK, 1: ok(Jimi 080523)
#define	DSP_WMAhasJanusDRM		0x3FA6  //0: no DRM, 1: has DRM(Jimi 080523)
#define DSP_MP3BitRate	0x3FA5			// The Bit rate of current MP3 file
#define DSP_EnableCMD	0x3FBE			// DSP Command Decoder Enable
//------756 Register
// decoder status registers
#define ST0_ADDR		0x3F50 			// ESP mem status
#define ST1_ADDR		0x3F51 			// ESP operating status
#define ST2_ADDR		0x3F52 			// DRAM mem valid write
#define ST3_ADDR		0x3F53 			// DRAM mem write
#define ST4_ADDR		0x3F54 			// DRAM mem read
#define ST5_ADDR		0x3F55 			// DRAM mem valid data counter

#define ST0_INT_MASK	0x3F4E // ST0 interrupt mask; 1: enable interrupt, 0: disable interrupt
#define ST0_STOPBO_MASK 0x3F57 // STO DRAM buffer out mask; 1: not moving from DRAM, 0: moving from DRAM
#define ST0_STOPBI_MASK 0x3F58 // STO DRAM buffer in mask; 1: not moving to DRAM, 0: moving to DRAM
//---- End of 756 Registers
//--------------------------------------------------------------------------------------------
//--- Define CommandSet between with MCU and DSP
#define DCMD_SysClkCfg	0x05			// System Phase Lock loop configuartion
#define DCMD_WMEM		0x07			// Write DSP memory mapping registers
#define DCMD_RMEM		0x0B			// Read DSP memory mapping registers
#define DCMD_AudTxCfg	0x0A			// Audio Interface in transmit configuration
#define DCMD_DecMode	0x0D			// Decoder Mode setting
#define DCMD_VolCtrl	0x51			// Digital volume Control
#define DCMD_EqCtrl		0x59			// Equalizer Control
#define DCMD_Restart    0x60            // force DSP jump 0 without hardware reset, ycc081031  //20090420 chiayen add
#define DCMD_DVRCfg		0x79			// DVR operation configuration
#define DCMD_RepCtrl	0x7D			// mp3 repeater control
#define DCMD_SilCtrl	0x7F			// Auto silence detection Control
//---
#define DCMD_Play		0x80			// Play
#define DCMD_ClearBuf	0x81			// Clear Buffer and Reset the Buffer Address
#define DCMD_Pause		0x84			// Pause
#define DCMD_Mute		0x88			// Mute
#define DCMD_Stop		0x8C			// Stop
#define DCMD_Sleep		0x98			// Sleep
#define DCMD_AskBitrate	0xC0			// Get the bit rate of current file
#define DCMD_DatIn		0xD0			// Receive Data End
#define DCMD_DatOut		0xD4			// Transmit Data End
#define DCMD_ChkBuf		0xDC			// Check the status of Buffer
#define DCMD_EOF		0xF0			// Command for EOF
#define DCMD_Reset		0xF2			// Software reset command
#define DCMD_Multiply   0xFC            // DSP do multiply
//--
/**********************
* DECODER COMMAND     *
**********************/

#define DECODER_SYS_CLK_CONFIG          0x05 // system clock config.
#define DECODER_RESET_CDROM_MODE        0x08 // reset cdrom mode
#define DECODER_AUD_CLK_CONFIG			0x0A // audio output clock config.
#define DECODER_DECODER_MODE            0x0D // decoder mode setting
#define DECODER_DRAM_CONFIG             0x15 // DRAM type and size config.
#define DECODER_CDDSP_CONFIG            0x22 // CD-DSP i/f config.
#define DECODER_SHOCK_FLAG_CONFIG       0x35 // shock flag config.

#define DECODER_DMAR1024                0x0b // DMA read 1024 word.
#define DECODER_DMAR16                  0x0f // DMA read 16 word.
#define DECODER_DATA_COMPARE_START		0x20 // Data compare_connect start
#define DECODER_DATA_COMPARE_STOP		0x24 // Data compare_connect stop
#define DECODER_DMAR32                  0x27 // DMA read 32 word.
#define DECODER_DMAW01                  0x13 // DMA write 1 word.
#define DECODER_DMAW02                  0x17 // DMA write 2 word.
#define DECODER_DMAW04                  0x1b // DMA write 4 word.
#define DECODER_DMAW08                  0x1f // DMA write 8 word.
#define DECODER_DMAW16                  0x23 // DMA write 16 word.
#define DECODER_DMAW32                  0x37 // DMA write 32 word.

#define DECODER_CDDA_SHOCK				0x18 // Issue the CDDA shock to SPCA756A

#define DECODER_CDFMIN					0x2B // CDFmIn indicates the first frm addr. of input data
#define DECODER_CDFMINLEN				0x30 // CDFmEnd indicates the last frm addr. of input data
#define DECODER_CDFILELEN               0x33 // Length of file(sector)
#define DECODER_DMA_TRANS_SIZE_CONFIG   0x39 // DMA transaction size config. (reserved)
#define DECODER_READ_CDFM             	0x3b // reads CDROM disc data and put on internal SRAM
#define DECODER_ECC_MODE				0x3D // ECC mode setting. 1: Mode 1 disc; 0: Mode 2 disc
#define DECODER_WRITE_DM_ADDR			0x3E // set the internal SRAM write address
#define DECODER_WRITE_DM				0x42 // continuously write SRAM
#define DECODER_READ_DM_ADDR            0x4A // set the internal SRAM read address
#define DECODER_READ_DM					0x4C // continuously read SRAM
#define DECODER_VOL						0x51 // volume control
#define DECODER_SKIP_SECTOR				0x55 // skip sector
#define DECODER_EQ                      0x59 // equalizer control
#define DECODER_ADPCM_COMP_CTRL         0x65 // CDDA ADPCM compression control/repeater control
#define DECODER_DATA_COMPARE_CONNECT	0x69 // CDDA data compare and connect setting
#define DECODER_DRAM_ADDR_OFST          0x6A // DRAM addr. offset from the base addr.
#define DECODER_READ_DRAM               0x6C // continuously read DRAM data
#define DECODER_WRITE_DRAM              0x6E // continuously write DRAM data
#define DECODER_DATA_ERR_CTRL			0x71 // CDROM data error control
#define DECODER_UPDATE_VWP				0x74 // update DRAM valid write pointer
#define DECODER_DVR_CONFIG              0x79 // config DVR rec/play mode
#define DECODER_REP_CONTROL             0x7D // Repeater mode control
#define DECODER_SILENCE_CTRL            0x7F // auto-silence-detection control

// Type 3 Command
#define DECODER_PLAY                    0x80 // play
#define DECODER_PAUSE                   0x84 // pause
#define DECODER_MUTE                    0x88 // mute
#define DECODER_STOP                    0x8C // stop

// Decode mode
#define DECODER_DAUDIO_MODE			0x00
#define DECODER_CDDA_ESP_MODE  		0x01
#define DECODER_CDDA_BUF_MODE		0x02
#define DECODER_DVR_ENC_MODE		0x04
#define DECODER_DVR_DEC_MODE		0x05


// ST0 flags
#define ST0_SHF	    	0x0001
#define ST0_DBOV		0x0002
#define ST0_DBUN		0x0004
#define ST0_PEF			0x0008
#define ST0_SEF     	0x0010
#define ST0_PUF     	0x0020
#define ST0_HFND    	0x0040
#define ST0_CRCE    	0x0080
#define ST0_C3E1    	0x0100
#define ST0_C3E2    	0x0200
#define ST0_MSFD    	0x0400
#define ST0_MUTEF   	0x0800
#define ST0_DVRF    	0x2000

// ST1 flags
#define ST1_IBACT		0x0001
#define ST1_DCOMP		0x0002
#define ST1_ADPEN		0x0004
#define ST1_RPEF		0x0004
#define ST1_ADPDE		0x0008
#define ST1_RPDE		0x0008
#define ST1_ENSTOP		0x0010
#define ST1_DESTOP		0x0020
#define ST1_CMPOK		0x0040
#define ST1_DSPRDY		0x0080
#define ST1_UNMUTE  	0x0200
#define ST1_RPOK    	0x0800
#define ST1_PSEF		0x1000
#define ST1_SILENCE 	0x2000


//------DSP DVR MODDE ------
#define KDVR_ADPCM_4bit     	0x00
#define KDVR_ADPCM_5bit     	0x01
#define KDVR_ADPCM_6bit     	0x02
#define KDVR_PCM				0x03
#define KDVR_uLAW				0x04
#define KDVR_G723_5K        	0x05
#define KDVR_G723_6K        	0x06
#define KDVR_NONE				0xFF

//-------DSP Operation Mode ---
#define KDSP_MP3_MODE       	0x00
#define KDSP_WMA_MODE       	0x03
#define KDSP_DVRE_MODE      	0x04    // DVR Encode Mode
#define KDSP_DVRD_MODE      	0x05    // DVR Decode Mode
#define KDSP_AAC_MODE       	0x06

//-------DSP Operation Mode Under Play---
#define KDSP_PlayFast       	0x00    // None
#define KDSP_PlayVolume     	0x01    // Set Volume
#define KDSP_PlayDPC        	0x02    // Set DPC
#define KDSP_PlayEQ				0x03    // Set Fast Forware/Backward

//======================================================================================================
#define EQ_Normalmode 0
#define EQ_DBBmode    1
#define EQ_JAZZmode   2//(JC)exchange with rock mode H0703
#define EQ_ROCKmode   3
#define EQ_POPmode    4
#define EQ_LIVEmode   5
#define EQ_SSRmode	  6
#define EQ_USERDEFINE 7
//======================================================================================================

//============================================================
// For MCU DMA to DSP DM
// DM24bit: For MP3 encode,MP3 Decode,WMA Encode,WMA Decode
// DM16bit: For ADPCM Encode,ADPCM Decode
//============================================================
#define         DM24bit						0x00
#define         DM16bit                     0x01
//============================================================
//DSP Code Address define
//============================================================
//============================================================
//DSP Register define
//============================================================
#define			DSP_spectrumBand0			0x3E40
#define         DSP_spectrumBand1			0x3E41
#define         DSP_spectrumBand2			0x3E42
#define         DSP_spectrumBand3			0x3E43
#define         DSP_spectrumBand4			0x3E44
#define         DSP_spectrumBand5			0x3E45
#define         DSP_spectrumBand6			0x3E46

#define			DSP_SurroundDPCFLAG			0x3E54

#define         DSP_EQBand0					0x3EE0 //range 0~24
#define         DSP_EQBand1					0x3EE1 //range 0~24
#define         DSP_EQBand2					0x3EE2 //range 0~24
#define         DSP_EQBand3					0x3EE3 //range 0~24
#define         DSP_EQBand4					0x3EE4 //range 0~24
#define         DSP_EQBand5				    0x3EE5 //range 0~24
#define         DSP_EQBand6                 0x3EE6 //range 0~24

#define         DSP_SurroundMixGain         0x3E4F // mix gain 0~7
#define         DSP_SurroundGainUp          0x3EBF // gain 0~5
#define         DSP_VolumeControl           0x3EEF //0~63 WMA,MP3,ADPCM included
                                                                                                  //This value must be set again when
                                                   //IM,PM reloaded
#define         DSP_CMDExeStatus            0x3F0B //DSP Command executing Status
                                                   //bit0:initial complete flag
                                                   //bit1:command executing flag

#define         DSP_RunningStatus           0x3F0A
#define         DSP_EffectControl           0x3F0F //3F0F[8] EQ change
                                                   //3F0F[9] EQ enable
                                                   //3F0F[10] spectrum enable
                                                   //3F0F[11] surround enable
                                                   //3F0F[12] anti-clipping enable
                                                   //3F0F[13] surround change

#define         DSP_GoControl				0x3F10 //Control the DSP starting to go
                                                   //0: DSP waiting MCU starting DSP go.
                                                   //0x1234: DSP enter to TEST mode
                                                   //0x1235: DSP starting and clear all buffer
                                                   //others: DSP starting to go but not clear all buffer.

#define         DSP_SunplusKeyAddr          0x3F11 //3F11~3F14 Sunplus Key Address

#define         DSP_RestartFlag        		0x3F1F // restart DSP,ycc081031  //20090420 chiayen add

#define         DSP_RemainBuffer            0x3F21 //DSP Remain Buffer size
                                                   //default 0
#define         DSP_EmptyBuffer             0x3F22 //DSP empty buffer size
                                                   //default value:
                                                   //WMA Decode 0x4800
                                                   //MP3 Encode 0x4800
                                                   //ADPCM Encode 0x4800
                                                   //ADPCM Decode 0x4800
                                                   //JPEG Decode 0x600

#define			DSP_PostProcessSelect       0x3F27 //bit  0: Surround bypass select bit after rampdown  //20090420 chiayen add
                                                   //bit  1: Anti-clipping bypass select bit after rampdown
                                                   //bit  2: Volume bypass select bit after rampdown
                                                   //bit  3: EQ bypass select bit after rampdown

                                                   //bit  5: Volume initial setting complete bit, to handshake with DSP
                                                   //        1: to notice DSP to do the volume process after MCU set the volume index,
                                                   //           after DSP complete the process, this bit will be cleared by DSP. 
                                                   //bit 15: First play bit, set this bit as first play.
                                                   //        Clear this bit after first play to avoid turn on/off EQ repeatly when restart DSP.
/****** Jimi 091126 ******/
#define			DSP_MP3_dec_bsbuf_rem		0x386F
#define			DSP_MP3_file_end_flag		0x3870
#define			DSP_MP3_frame_size			0x34BF
/*************************/
/****** Mark@080225 ******/
#define         DSP_SOutBufRemData          0x3F2C //Remain output Buffer size
                                                   //default 0

#define         DSP_SOutBufEmptyData		0x3F2D //empty output buffer size
                                                   //default value: 
                                                   //JPG Decode 0x1800
/*************************/
#define			DSP_WMABitrate			    0x3F31 //WMA avg bitrate
#define			DSP_MP3Bitrate			    0x3F31 //MP3 bitrate

#define         DSP_DPCControl				0x3F32 //Playback speed control register (DPC speed).
                                                   //range:1~24
                                                                                                  //This value must be set again when
                                                   //IM,PM reloaded                                                                                //This value must be set again when


#define         DSP_DecodeStatus            0x3F8C //MP3/WMA decode status
                                                   //3F8C[6:7]: 11 file end
                                                   //00 normal
                                                   //10 WMA error bit stream
                                                   //01 fatal error
#define         DSP_MP3MPGIndex				0x3F92 // MP3 mode: MPG index
                                                   // WMA mode: bit0==>random Flag
#define         DSP_WMARandomFlag           0x3F92 // WMA random flag

#define         DSP_MP3LayerIndex           0x3F93 //MP3 mode:layer index
                                                   //WMA mode:sector offset
#define         DSP_WMASectorOffset         0x3F93

#define         DSP_MP3BitRateIndex         0x3F94 //MP3 bit rate index

	#define         DSP_ADCInputSelect          0x3E53 //ADC input select
                                                   // [1:0]=00 MIC in
                                                   // [1:0]=01 Line in
                                                   // [1:0]=10 FM in


#define         DSP_SRBRControl				0x3F0E //DSP All algorithm bit rate/sample rate control
#define         DSP_ADPCMModeControl		0x3F0E // 0x3F0E[5:4]=00 8K sampling rate
                                                   //             01 16K sampline rate

#define         DSP_MP3EncodeMode			0x3F0E //MP3 encode mode select
//#define         DSP_WMACurrenTimeHigh		0x3F9B //WMA play current time high byte
//#define         DSP_WMACurrenTimeLow		0x3F9C //WMA play current time low byte

#define         DSP_WMAMediaStreamInfo      0x3F9D //Information of WMA media stream

#define         DSP_DecodeFrameCounter      0x3FB3 //ADPCM & MP3 decode frame counter

#define         DSP_RampDownComplete        0x3FB8 //Ramp-down complete flag  //20090803 chiayen add

#define         DSP_MPEGSt                  0x3F49
//#define			DSP_WMAhasJanusDRM          0x3FA6  //0: no DRM, 1: has DRM 
/****************************************************************************
MP3 Encode mode 0x3F0E[8:0]
8 7 6 5 4 3 2 1 0    samplerate bitrate mono/stereo   Date
-----------------------------------------------------------
1 0 0 0 0 0 0 0 0             Not support            0x0100
1 0 0 0 0 0 0 0 1     44.1K      32K     stereo      0x0101
1 0 0 0 0 0 0 1 0     44.1K      40K     stereo      0x0102
1 0 0 0 0 0 0 1 1     44.1K      48K     stereo      0x0103
1 0 0 0 0 0 1 0 0     44.1K      56K     stereo      0x0104
1 0 0 0 0 0 1 0 1     44.1K      64K     stereo      0x0105
1 0 0 0 0 0 1 1 0     44.1K      80K     stereo      0x0106
1 0 0 0 0 0 1 1 1     44.1K      96K     stereo      0x0107
1 0 0 0 0 1 0 0 0     44.1K     112K     stereo      0x0108
1 0 0 0 0 1 0 0 1     44.1K     128K     stereo      0x0109
1 0 0 0 0 1 0 1 0     44.1K     160K     stereo      0x010A
1 0 0 0 0 1 0 1 1     44.1K     192K     stereo      0x010B
1 0 0 0 0 1 1 0 0     44.1K     224K     stereo      0x010C
1 0 0 0 0 1 1 0 1     44.1K     256K     stereo      0x010D
1 0 0 0 0 1 1 1 0     44.1K     320K     stereo      0x010E
1 0 0 0 0 1 1 1 1            Not support             0x010F

1 0 0 1 0 0 0 0 0             Not support            0x0120
1 0 0 1 0 0 0 0 1       48K      32K     stereo      0x0121
1 0 0 1 0 0 0 1 0       48K      40K     stereo      0x0122
1 0 0 1 0 0 0 1 1       48K      48K     stereo      0x0123
1 0 0 1 0 0 1 0 0       48K      56K     stereo      0x0124
1 0 0 1 0 0 1 0 1       48K      64K     stereo      0x0125
1 0 0 1 0 0 1 1 0       48K      80K     stereo      0x0126
1 0 0 1 0 0 1 1 1       48K      96K     stereo      0x0127
1 0 0 1 0 1 0 0 0       48K     112K     stereo      0x0128
1 0 0 1 0 1 0 0 1       48K     128K     stereo      0x0129
1 0 0 1 0 1 0 1 0       48K     160K     stereo      0x012A
1 0 0 1 0 1 0 1 1       48K     192K     stereo      0x012B
1 0 0 1 0 1 1 0 0       48K     224K     stereo      0x012C
1 0 0 1 0 1 1 0 1       48K     256K     stereo      0x012D
1 0 0 1 0 1 1 1 0       48K     320K     stereo      0x012E
1 0 0 1 0 1 1 1 1            Not support             0x012F

1 0 1 0 0 0 0 0 0             Not support            0x0140
1 0 1 0 0 0 0 0 1       32K      32K     stereo      0x0141
1 0 1 0 0 0 0 1 0       32K      40K     stereo      0x0142
1 0 1 0 0 0 0 1 1       32K      48K     stereo      0x0143
1 0 1 0 0 0 1 0 0       32K      56K     stereo      0x0144
1 0 1 0 0 0 1 0 1       32K      64K     stereo      0x0145
1 0 1 0 0 0 1 1 0       32K      80K     stereo      0x0146
1 0 1 0 0 0 1 1 1       32K      96K     stereo      0x0147
1 0 1 0 0 1 0 0 0       32K     112K     stereo      0x0148
1 0 1 0 0 1 0 0 1       32K     128K     stereo      0x0149
1 0 1 0 0 1 0 1 0       32K     160K     stereo      0x014A
1 0 1 0 0 1 0 1 1       32K     192K     stereo      0x014B
1 0 1 0 0 1 1 0 0       32K     224K     stereo      0x014C
1 0 1 0 0 1 1 0 1       32K     256K     stereo      0x014D
1 0 1 0 0 1 1 1 0       32K     320K     stereo      0x014E
1 0 1 0 0 1 1 1 1            Not support             0x014F

1 1 0 0 1 0 0 0 0             Not support             0x0190
1 1 0 0 1 0 0 0 1     22.05K       8K     stereo      0x0191
1 1 0 0 1 0 0 1 0     22.05K      16K     stereo      0x0192
1 1 0 0 1 0 0 1 1     22.05K      24K     stereo      0x0193
1 1 0 0 1 0 1 0 0     22.05K      32K     stereo      0x0194
1 1 0 0 1 0 1 0 1     22.05K      40K     stereo      0x0195
1 1 0 0 1 0 1 1 0     22.05K      48K     stereo      0x0196
1 1 0 0 1 0 1 1 1     22.05K      56K     stereo      0x0197
1 1 0 0 1 1 0 0 0     22.05K      64K     stereo      0x0198
1 1 0 0 1 1 0 0 1     22.05K      80K     stereo      0x0199
1 1 0 0 1 1 0 1 0     22.05K      96K     stereo      0x019A
1 1 0 0 1 1 0 1 1     22.05K     112K     stereo      0x019B
1 1 0 0 1 1 1 0 0     22.05K     128K     stereo      0x019C
1 1 0 0 1 1 1 0 1     22.05K     144K     stereo      0x019D
1 1 0 0 1 1 1 1 0     22.05K     160K     stereo      0x019E
1 1 0 0 1 1 1 1 1            Not support              0x019F

1 1 0 1 1 0 0 0 0             Not support             0x01B0
1 1 0 1 1 0 0 0 1       24K       8K     stereo       0x01B1
1 1 0 1 1 0 0 1 0       24K      16K     stereo       0x01B2
1 1 0 1 1 0 0 1 1       24K      24K     stereo       0x01B3
1 1 0 1 1 0 1 0 0       24K      32K     stereo       0x01B4
1 1 0 1 1 0 1 0 1       24K      40K     stereo       0x01B5
1 1 0 1 1 0 1 1 0       24K      48K     stereo       0x01B6
1 1 0 1 1 0 1 1 1       24K      56K     stereo       0x01B7
1 1 0 1 1 1 0 0 0       24K      64K     stereo       0x01B8
1 1 0 1 1 1 0 0 1       24K      80K     stereo       0x01B9
1 1 0 1 1 1 0 1 0       24K      96K     stereo       0x01BA
1 1 0 1 1 1 0 1 1       24K     112K     stereo       0x01BB
1 1 0 1 1 1 1 0 0       24K     128K     stereo       0x01BC
1 1 0 1 1 1 1 0 1       24K     144K     stereo       0x01BD
1 1 0 1 1 1 1 1 0       24K     160K     stereo       0x01BE
1 1 0 1 1 1 1 1 1           Not support               0x01BF

1 1 1 0 1 0 0 0 0             Not support             0x01D0
1 1 1 0 1 0 0 0 1       16K       8K     stereo       0x01D1
1 1 1 0 1 0 0 1 0       16K      16K     stereo       0x01D2
1 1 1 0 1 0 0 1 1       16K      24K     stereo       0x01D3
1 1 1 0 1 0 1 0 0       16K      32K     stereo       0x01D4
1 1 1 0 1 0 1 0 1       16K      40K     stereo       0x01D5
1 1 1 0 1 0 1 1 0       16K      48K     stereo       0x01D6
1 1 1 0 1 0 1 1 1       16K      56K     stereo       0x01D7
1 1 1 0 1 1 0 0 0       16K      64K     stereo       0x01D8
1 1 1 0 1 1 0 0 1       16K      80K     stereo       0x01D9
1 1 1 0 1 1 0 1 0       16K      96K     stereo       0x01DA
1 1 1 0 1 1 0 1 1       16K     112K     stereo       0x01DB
1 1 1 0 1 1 1 0 0       16K     128K     stereo       0x01DC
1 1 1 0 1 1 1 0 1       16K     144K     stereo       0x01DD
1 1 1 0 1 1 1 1 0       16K     160K     stereo       0x01DE
1 1 1 0 1 1 1 1 1           Not support               0x01DF

0 0 0 0 0 0 0 0 0             Not support            0x0000
0 0 0 0 0 0 0 0 1     44.1K      32K       mono      0x0001
0 0 0 0 0 0 0 1 0     44.1K      40K       mono      0x0002
0 0 0 0 0 0 0 1 1     44.1K      48K       mono      0x0003
0 0 0 0 0 0 1 0 0     44.1K      56K       mono      0x0004
0 0 0 0 0 0 1 0 1     44.1K      64K       mono      0x0005
0 0 0 0 0 0 1 1 0     44.1K      80K       mono      0x0006
0 0 0 0 0 0 1 1 1     44.1K      96K       mono      0x0007
0 0 0 0 0 1 0 0 0     44.1K     112K       mono      0x0008
0 0 0 0 0 1 0 0 1     44.1K     128K       mono      0x0009-->default
0 0 0 0 0 1 0 1 0     44.1K     160K       mono      0x000A
0 0 0 0 0 1 0 1 1     44.1K     192K       mono      0x000B
0 0 0 0 0 1 1 0 0     44.1K     224K       mono      0x000C
0 0 0 0 0 1 1 0 1     44.1K     256K       mono      0x000D
0 0 0 0 0 1 1 1 0     44.1K     320K       mono      0x000E
0 0 0 0 0 1 1 1 1            Not support             0x000F

0 0 0 1 0 0 0 0 0             Not support            0x0020
0 0 0 1 0 0 0 0 1       48K      32K       mono      0x0021
0 0 0 1 0 0 0 1 0       48K      40K       mono      0x0022
0 0 0 1 0 0 0 1 1       48K      48K       mono      0x0023
0 0 0 1 0 0 1 0 0       48K      56K       mono      0x0024
0 0 0 1 0 0 1 0 1       48K      64K       mono      0x0025
0 0 0 1 0 0 1 1 0       48K      80K       mono      0x0026
0 0 0 1 0 0 1 1 1       48K      96K       mono      0x0027
0 0 0 1 0 1 0 0 0       48K     112K       mono      0x0028
0 0 0 1 0 1 0 0 1       48K     128K       mono      0x0029
0 0 0 1 0 1 0 1 0       48K     160K       mono      0x002A
0 0 0 1 0 1 0 1 1       48K     192K       mono      0x002B
0 0 0 1 0 1 1 0 0       48K     224K       mono      0x002C
0 0 0 1 0 1 1 0 1       48K     256K       mono      0x002D
0 0 0 1 0 1 1 1 0       48K     320K       mono      0x002E
0 0 0 1 0 1 1 1 1            Not support             0x002F

0 0 1 0 0 0 0 0 0             Not support            0x0040
0 0 1 0 0 0 0 0 1       32K      32K       mono      0x0041
0 0 1 0 0 0 0 1 0       32K      40K       mono      0x0042
0 0 1 0 0 0 0 1 1       32K      48K       mono      0x0043
0 0 1 0 0 0 1 0 0       32K      56K       mono      0x0044
0 0 1 0 0 0 1 0 1       32K      64K       mono      0x0045
0 0 1 0 0 0 1 1 0       32K      80K       mono      0x0046
0 0 1 0 0 0 1 1 1       32K      96K       mono      0x0047
0 0 1 0 0 1 0 0 0       32K     112K       mono      0x0048
0 0 1 0 0 1 0 0 1       32K     128K       mono      0x0049
0 0 1 0 0 1 0 1 0       32K     160K       mono      0x004A
0 0 1 0 0 1 0 1 1       32K     192K       mono      0x004B
0 0 1 0 0 1 1 0 0       32K     224K       mono      0x004C
0 0 1 0 0 1 1 0 1       32K     256K       mono      0x004D
0 0 1 0 0 1 1 1 0       32K     320K       mono      0x004E
0 0 1 0 0 1 1 1 1            Not support             0x004F

0 1 0 0 1 0 0 0 0             Not support            0x0090
0 1 0 0 1 0 0 0 1     22.05K       8K      mono      0x0091
0 1 0 0 1 0 0 1 0     22.05K      16K      mono      0x0092
0 1 0 0 1 0 0 1 1     22.05K      24K      mono      0x0093
0 1 0 0 1 0 1 0 0     22.05K      32K      mono      0x0094
0 1 0 0 1 0 1 0 1     22.05K      40K      mono      0x0095
0 1 0 0 1 0 1 1 0     22.05K      48K      mono      0x0096
0 1 0 0 1 0 1 1 1     22.05K      56K      mono      0x0097
0 1 0 0 1 1 0 0 0     22.05K      64K      mono      0x0098
0 1 0 0 1 1 0 0 1     22.05K      80K      mono      0x0099
0 1 0 0 1 1 0 1 0     22.05K      96K      mono      0x009A
0 1 0 0 1 1 0 1 1     22.05K     112K      mono      0x009B
0 1 0 0 1 1 1 0 0     22.05K     128K      mono      0x009C
0 1 0 0 1 1 1 0 1     22.05K     144K      mono      0x009D
0 1 0 0 1 1 1 1 0     22.05K     160K      mono      0x009E
0 1 0 0 1 1 1 1 1            Not support             0x009F

0 1 0 1 1 0 0 0 0             Not support            0x00B0
0 1 0 1 1 0 0 0 1       24K       8K      mono       0x00B1
0 1 0 1 1 0 0 1 0       24K      16K      mono       0x00B2
0 1 0 1 1 0 0 1 1       24K      24K      mono       0x00B3
0 1 0 1 1 0 1 0 0       24K      32K      mono       0x00B4
0 1 0 1 1 0 1 0 1       24K      40K      mono       0x00B5
0 1 0 1 1 0 1 1 0       24K      48K      mono       0x00B6
0 1 0 1 1 0 1 1 1       24K      56K      mono       0x00B7
0 1 0 1 1 1 0 0 0       24K      64K      mono       0x00B8
0 1 0 1 1 1 0 0 1       24K      80K      mono       0x00B9
0 1 0 1 1 1 0 1 0       24K      96K      mono       0x00BA
0 1 0 1 1 1 0 1 1       24K     112K      mono       0x00BB
0 1 0 1 1 1 1 0 0       24K     128K      mono       0x00BC
0 1 0 1 1 1 1 0 1       24K     144K      mono       0x00BD
0 1 0 1 1 1 1 1 0       24K     160K      mono       0x00BE
0 1 0 1 1 1 1 1 1           Not support              0x00BF

0 1 1 0 1 0 0 0 0             Not support           0x00D0
0 1 1 0 1 0 0 0 1       16K       8K     mono       0x00D1
0 1 1 0 1 0 0 1 0       16K      16K     mono       0x00D2
0 1 1 0 1 0 0 1 1       16K      24K     mono       0x00D3
0 1 1 0 1 0 1 0 0       16K      32K     mono       0x00D4
0 1 1 0 1 0 1 0 1       16K      40K     mono       0x00D5
0 1 1 0 1 0 1 1 0       16K      48K     mono       0x00D6
0 1 1 0 1 0 1 1 1       16K      56K     mono       0x00D7
0 1 1 0 1 1 0 0 0       16K      64K     mono       0x00D8
0 1 1 0 1 1 0 0 1       16K      80K     mono       0x00D9
0 1 1 0 1 1 0 1 0       16K      96K     mono       0x00DA
0 1 1 0 1 1 0 1 1       16K     112K     mono       0x00DB
0 1 1 0 1 1 1 0 0       16K     128K     mono       0x00DC
0 1 1 0 1 1 1 0 1       16K     144K     mono       0x00DD
0 1 1 0 1 1 1 1 0       16K     160K     mono       0x00DE
0 1 1 0 1 1 1 1 1           Not support             0x00DF
***************************************************************************/
//********************************************************************************************

//==================================== SYSTEM STATE STRUCT ========================================


#define gbBitRate     ((Mpeg_Status>>(7+8))&0x0f)
#define gbSampleRate  ((Mpeg_Status>>(5+8))&0x03)
#define gbMPEGType    ((Mpeg_Status>>(14+8))&0x03)
#define gbLayer       ((Mpeg_Status>>(12+8))&0x03)

#define  DSP_DISABLE                     0x00
#define  DSP_ENABLE                      0x01

#define  DSP_B2APOINT_CONTINUEPLAY       0x00
#define  DSP_FFFR_CONTINUEPLAY           0x01


#endif	/* DSP.H */
