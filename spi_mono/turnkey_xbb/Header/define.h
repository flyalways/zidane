
#ifndef __DEFINE_H__
#define __DEFINE_H__

/*----- Common Global Variables -----*/
#define	_XBYTE	unsigned char xdata
#define	XBYTE	((unsigned char volatile xdata *) 0)

#define	U8		unsigned char
#define	U16		unsigned short
#define	U32		unsigned long

#define	P1_0	P1^0
#define	P1_1	P1^1
#define	P1_2	P1^2
#define	P1_3	P1^3
#define	P1_4	P1^4
#define	P1_5	P1^5
#define	P1_6	P1^6
#define	P1_7	P1^7
#define	P3_0	P3^0
#define	P3_1	P3^1
//0xB301
	#define	SRAM2SRAM		0x00
	#define	SRAM2PIO		0x10
	#define	SRAM2DEVICE		0x20
	#define	SRAM2DSPIM		0x30
	#define	SRAM2DSPPM		0x40
	#define	SRAM2DSPDM		0x50
	#define	SRAM2DRAM		0x60

	#define	PIO2SRAM		0x01
	#define	PIO2PIO			0x11
	#define	PIO2DEVICE		0x21
	#define	PIO2DSPIM		0x31
	#define	PIO2DSPPM		0x41
	#define	PIO2DSPDM		0x51
	#define	PIO2DRAM		0x61

	#define	DEVICE2SRAM		0x02
	#define	DEVICE2PIO		0x12
	#define	DEVICE2DEVICE	0x22
	#define	DEVICE2DSPIM	0x32
	#define	DEVICE2DSPPM	0x42
	#define	DEVICE2DSPDM	0x52
	#define	DEVICE2DRAM		0x62

	#define	DSPIM2SRAM		0x03
	#define	DSPIM2PIO		0x13
	#define	DSPIM2DEVICE	0x23
	#define	DSPIM2DSPIM		0x33
	#define	DSPIM2DSPPM		0x43
	#define	DSPIM2DSPDM		0x53
	#define	DSPIM2DRAM		0x63

	#define	DSPPM2SRAM		0x04
	#define	DSPPM2PIO		0x14
	#define	DSPPM2DEVICE	0x24
	#define	DSPPM2DSPIM		0x34
	#define	DSPPM2DSPPM		0x44
	#define	DSPPM2DSPDM		0x54
	#define	DSPPM2DRAM		0x64


	#define	DRAM2SRAM		0x06
	#define	DRAM2PIO		0x16
	#define	DRAM2DEVICE		0x26
	#define	DRAM2DSPIM		0x36
	#define	DRAM2DSPPM		0x46
	#define	DRAM2DSPDM		0x56
	#define	DRAM2DRAM		0x66


// ------------ 定義Complier參數 --------------- //

#define PseudoCard_TimeOut		1200   
#define SDMMCTimeOutLoop_1000   1000
#define SDMMCTimeOutLoop_10000  65535

//************** File System **************//
#define	DOS_CLUSTER_LINK_ERR         	0x02
#define	DOS_ERROR_DISK_FULL	 			0x03
#define	DOS_ERROR_ROOTDIR_FULL	 		0x04
#define DOS_STARTCLUSTER_ERR          	0x05
#define DOS_READSECTOR_ERR				0x06
#define DOS_WRITESECTOR_ERR             0x07
#define	DOS_END_OF_FILE              	0x08	//!< 1 to be returned on failure in FAT functions.

#define	DOS_UNKNOW_FILESYSTEM_ERR    	0x81
#define	DOS_READ_MBS_ERR             	0x82
#define	DOS_READ_PBS_ERR             	0x83
#define	DOS_MBS_MARK_ERR             	0x84
#define	DOS_PBS_MARK_ERR             	0x85
#define	DOS_FAT_ERR                  	0x86
#define	DOS_READ_FAT1_ERR            	0x87
#define	DOS_READ_FAT2_ERR            	0x88
#define	DOS_PARTITIONSIZE_ERR        	0x89

#define	K_COUNTER_FILENUM           	0
#define	K_TIME_FINDFDB              	1
#define	K_NAME_FINDFDB              	2
#define	K_ORDER_FINDFDB             	3
#define	K_SPECIFIC_LONG_FILENAME    	4
#define	K_SPECIFIC_SHORT_FILENAME   	5
#define	K_SPECIFIC_STARTCLUSTER     	6
#define	K_SPECIFIC_FILESIZE         	7
#define	K_FIND_FREE_FDB             	8
#define	K_FIND_LAST_FDB					9
#define	K_FIND_DIR_EXTNAME				10
#define	K_LongFileName					0x00
#define	K_ShortFileName					0x01
#define	K_CREATENEWFILE_MODE			0x01	

#define	C_COUNTER_FILENUM				0
#define	C_TIME_FINDFDB              	1
#define	C_NAME_FINDFDB              	2
#define	C_ORDER_FINDFDB             	3
#define	C_SPECIFIC_LONG_FILENAME    	4
#define	C_SPECIFIC_SHORT_FILENAME   	5
#define	C_SPECIFIC_STARTCLUSTER     	6
#define	C_SPECIFIC_FILESIZE         	7
#define	C_FIND_FREE_FDB             	8
#define	C_FIND_LAST_FDB	         	 	9
#define	C_FIND_DIR_EXTNAME				10
#define C_COUNTER_DIRNUM			    11

//==========File System Arguments===========
#define C_File_All						0x00
#define C_File_OneDir					0x10
#define C_Dir_All						0x20
#define C_Dir_OneDir					0x30

#define C_Cnt_FileNo					0x00
#define C_By_Time						0x01
#define C_By_Name						0x02
#define C_By_FDB						0x03
#define C_By_LName						0x04
#define C_By_SName						0x05
#define C_Cnt_DirNo						0x0B

#define C_MusicFileType					0x00
#define C_RecordFileType				0x01
#define C_OtherFileType					0x02

#define C_NotCmpExtName					0x00					
#define C_CmpExtName					0x10

#define C_Next							0x00
#define C_Prev							0x01

#define C_MusicFHandle                  0x00
#define C_RecordFHandle					0x01
#define C_OtherFHandle					0x02

#define C_NullFileName                  0x00

#define C_Open_SpecFile					0x00
#define C_Open_NewFile					0x01
#define C_Open_FoundFile				0x02

//==========================================
#define FAIL 						    1
#define SUCCESS    						0

#define K_USER_DIR_NUM					1
#define GO_ON							2

//------------------------------------------UI
#define C_Key_Play                  0x01
#define C_Key_Mode                  0x02
#define C_Key_Next					0x03
#define C_Key_Prev               	0x04
//======================================================
#define C_LKey_Play                 0x11
#define C_LKey_Mode                 0x12
#define C_LKey_Next                 0x13
#define C_LKey_Prev                 0x14
#define C_LKey_Volup                0x15
#define C_LKey_Voldn                0x16
#define C_LKey_Rec                  0x17
#define C_LKey_Vol	 				0x18

#define C_Key_None                  0x00

#define REPEATKEY_DELAYVALUE 		12
#define MIN_REPEATKEYDELAY 			12  //12
#define MAX_REPEATKEYDELAY 			48  //16
#define	IRKEY_REPEATTIMES			10	//IR 長按鍵時間設定

#define	C_Task_Idle				0
#define	C_Task_Suspend			1
#define	C_Task_FuncOption		2
#define C_Task_MusicPlay		3
#define C_Task_FM				4
#define	C_Task_PlayWAV			5
#define C_Task_Calendar	  		6
#define C_Task_Setup	  		7
#define	C_Task_SetClock			8
#define	C_Task_SetBacklight		9
#define	C_Task_SetLanguage		10
#define	C_Task_SetAlarm			11
#define	C_Task_SetOFFTimer		12
#define	C_Task_AlarmON			13
#define	C_Task_LineIn			14


#define C_Task_Menu     	7
#define C_Task_Setting  	8

#define C_Task_Dir			13
#define C_Task_PlayMenu_IR          14 
#define C_Task_PlayMenu             15 

#define TASK_PHASE_PLAYACT	0x00
#define TASK_PHASE_PAUSE	0x01
#define TASK_PHASE_STOP		0x02
#define TASK_PHASE_REC		0x03

#define TASK_PHASE_FASTFWD          0x04
#define TASK_PHASE_FASTREV          0x05

//define for audio
#define AUDIO_FILE_TYPE_MP3         0x00
#define AUDIO_FILE_TYPE_WMA         0x01
#define AUDIO_FILE_TYPE_WMV         0x02

//define for voice
#define AUDIO_FILE_TYPE_WAV         0x00
#define AUDIO_FILE_TYPE_ASF         0x01


//=========key=========
#define C_NULLPHASE    	0xff

#define C_ModeNext     	0x01
#define C_ModePrev		0x02	


#define C_PlayInit   	0x00
#define C_PlayProc		0x01			
#define C_PlayEndfile   0x02
#define C_PlayPause  	0x03
#define C_PlayStop      0x04
#define C_PlayNext		0x05
#define C_PlayPrev      0x06
#define C_PlayFF        0x07
#define C_PlayFR        0x08
#define C_PlayIdle  	0x09
#define C_PlayFfFrEnd   0x0A
#define C_PlaySetRepAB  0x0B
#define C_PlayVolUp		0x0C
#define C_PlayVolDn		0x0D
#define C_PlayUpLevel   0x0E
#define C_PlayMenu      0x0F
#define C_PlayLanguage 	0x10
#define C_PowerOff		0x11   

#define C_FmFreqChgUp	0x11
#define C_FmFreqChgDown	0x12
#define C_PlayModeChg	0x13
#define C_MediaChg		0x16



//***************************

// We use definitions here to access gac_ratio_table[].  Please make sure the right mapping.
#define CLOCK_MODE_XTL		0
#define CLOCK_MODE_MP3		1
#define CLOCK_MODE_MP3L		2
#define CLOCK_MODE_WMA		3
#define CLOCK_MODE_DOS		4
#define	CLOCK_MODE_USB		4
#define	CLOCK_MODE_SPI		5
#define	CLOCK_MODE_REC		6

#define C_T0_12MhzMSB				(unsigned char)(-7500>>8)
#define C_T0_12MhzLSB				(unsigned char)(-7500)
#define C_T0_24MhzMSB				(unsigned char)(-15000>>8)
#define C_T0_24MhzLSB				(unsigned char)(-15000)
#define C_T0_48MhzMSB				(unsigned char)(-30000>>8)
#define C_T0_48MhzLSB				(unsigned char)(-30000)
#define C_T0_72MhzMSB				(unsigned char)(-45000>>8)
#define C_T0_72MhzLSB				(unsigned char)(-45000)

#define C_RepeatAll		0x00
#define	C_PlayAllOnce	0x01
#define C_RepeatOne		0x02
#define C_RandomPlay	0x03
#define C_InDirRepeat	0x04
#define C_InDirOnce		0x05


//******************************
enum 
{
	ONE_SECTOR = 512
};

#define	REPEAT_AB_NULL	0
#define	REPEAT_A		1
#define	REPEAT_B		2
#define	REPEAT_AB		3

typedef struct DSP_PLAY_GLOBAL_ {
	U8 sc_RepeatMode;
}DSP_PLAY_GLOBAL;

typedef struct DSP_GLOBAL_ 
{
	U8	sc_Volume;
	U8  sc_EQ_Type;
	U8	sc_DM_Index;
	U32 sdw_File_ACluster;
	U32 sdw_File_ADataPoint;
	U32 sdw_File_BCluster;
	U32 sdw_File_BDataPoint;

	U32 sdw_sector_num; // Added by Mark@080605
	U16 sw_sector_cnt;
	U16 sw_data_start;
	U8  sc_Pixel_Index;
	U8  sc_End_Of_File;

	DSP_PLAY_GLOBAL s_DSP_RAM;
}DSP_GLOBAL; 


#define	LRC_DISPLAY_LYRICS_CHARNUMBER	100

// For RTC interrupt flags.
#define RTC_SECOND_INTERRUPT	0x01
#define RTC_ALARM_INTERRUPT		0x02


//----------- SPI flash define-------//
#define SPI_ENABLE()        do { SPI_REG[0x18] &= ~1; } while(0)
#define SPI_DISABLE()       do { SPI_REG[0x18] |= 1; } while(0)

#define SPI_CLK_HI()        (DEVICE_REG[0x02]|=0x80)
#define SPI_CLK_LO()        (DEVICE_REG[0x02]&=~0x80)

#define SPI_SO	            ((DEVICE_REG[0x0A] >> 6) & 0x01)

#define SPI_SI_HI	        (DEVICE_REG[0x02] |= 0x20)
#define SPI_SI_LO	        (DEVICE_REG[0x02] &= ~0x20)

#define SPI_BANK_NUM       3
#define SPI_DSP_NUM        3
#define SPI_USER_NUM       2
typedef struct _SPI_ADDR_
{
    U8 SBlock;       // 16 page /block 
    U8 SPage;       // 8 sector / page
    U8 SInAddr;     // 512 byte/sector 
    
} sSPI_ADDR;


#define NO_USB				0x00
#define USB_PHY_CONNECT			0x01
#define USB_REMOVE 			0x02
#define USB_STAND_BY		0x03
#define USB_READ			0x04
#define USB_WRITE			0x05
#define USB_SUSPEND			0x06

#define K_CommandPhase                  0x00
#define K_OutDataPhase                  0x01
#define K_OutDataPhaseWrite             0x02
#define K_OutResponsePhase              0x03

#define K_USBMSDC_BULK_ONLY_MASS_STORAGE_RESET  0xff
#define K_USBMSDC_GET_MAX_LUN                   0xfe
#define K_USBMSDC_CommandPassed                 0x00
#define K_USBMSDC_CommandFailed                 0x01
#define K_USBMSDC_PhaseError                    0x02
#define K_USBMSDC_NoMedium                      0x03
#define K_USBMSDC_CBW_SIZE              0x1f
#define K_USBMSDC_CSW_SIZE              0x0d


#define		REG_BULK_ACK_EN			0xD2
#define		REG_USBCFG			0xB1
#define		REG_BULK_EN0			0xA1
#define K_InDataPhase                   0x81
#define K_InDataPhaseRead               0x82
#define K_InResponsePhase               0x83
#define K_InCompletionPhase             0x84


#define USB_DETECT_TIMEOUT			0x1FFFF  //20081211 chiayen add


/*-------- EndPoint0 Index Descriptor Manager Table -------*/


// for CX Configuration and Status Register
#define      CX_DONE			0x01        // W
#define      TST_PKDONE			0x02        // W
#define      CX_STL			0x04        // R/w
#define      CX_CLR			0x08        // W
#define      CX_FUL			0x10        // R
#define      CX_EMP			0x20        // R


/*-------- USB Definition ------------------*/
#define      StallEndPoint1                0
#define      StallEndPoint2                1
#define      StepOverAllStage              0
#define      CBWStage                      1
#define      BulkOutStage                  2
#define      BulkOutWriteSectorsStage      3
#define      BulkInResponseStage           4
#define      BulkInReadSectorsStage        5
#define      BulkStallStage                6
#define      CSWStage                      7
#define      VendorCommandStage            8
#define      BulkOutWriteBufferStage       9
#define      BulkInReadBufferStage        10 

#define ID_NOT_FND      0x10
#define      NormalResponseData            1
#define      ModeSenseResponseData         0

#define	SPI_BANK_AREA		0	// 192K
#define	SPI_DSP_AREA		3	// 192K
#define SPI_USERdata_AREA	6	// 128K
#define	SPI_HZK_AREA		8	// 192K


#define	unionU16	union{ U16 WD; U8 BY[2]; }
#define	unionU32	union{ U32 LW; U16 WD[2]; U8 BY[4]; }

#endif // !__DEFINE_H__

