
#ifndef __DEFINE_H__
#define __DEFINE_H__

/*----- Common Global Variables -----*/
#define BOOLEAN   unsigned char                 /* size: 1 byte  */

#define UBYTE        unsigned char              /* size: 1 byte  */
#define  BYTE        unsigned char              /* size: 1 byte  */
#define _XBYTE       unsigned char xdata
#define XBYTE ((unsigned char volatile xdata *) 0)
#define IBYTE  idata unsigned char
#define DBYTE   data unsigned char

#define UWORD        unsigned int               /* size: 2 bytes */
#define  WORD        unsigned int               /* size: 2 bytes */
#define XWORD   volatile   unsigned int  xdata
#define IWORD  idata unsigned int
#define  LWORD       unsigned long              /* size: 4 bytes */
#define XLWORD  volatile   unsigned long xdata

#define U8			unsigned char	//!< A byte type.
#define U16			unsigned short	//!< A 16-bit integer type.
#define U32			unsigned long	//!< A 32-bit integer type.

//-----------------------------------------------------------------------------
// Define a better series of macro for data type to be a standard.
//-----------------------------------------------------------------------------
#define bool        BOOLEAN
#define uint8		unsigned char
#define uint16		unsigned int

#define sint8		signed char
#define sint16		signed int

#define int8        signed char
#define int16       signed int

union LWDataType {
    	LWORD LW;
    	UWORD WD[2];
    	UBYTE BY[4];
};

union WORDDataType {
    	UWORD WD;
    	UBYTE BY[2];
};

//! union for byte access in a word
#define      unionWORD                 union  {  UWORD WD;  UBYTE BY[2]; }                // 共用命名空間
//! union for byte/word access in a long word
#define      unionLONGWORD             union  {  LWORD LW;  UWORD WD[2];  UBYTE BY[4]; }  // 共用命名空間

#define	    Power_On_Card_Reader    0


/*----------- Constant -------------*/
#define     EXTERN          extern  /* byte .. addressabel variables */
#define     TRUE            1
#define     FALSE           0

#define     LEDOn           FALSE   // [05.28]
#define     LEDOff          TRUE    // [05.28]

#define     FAIL            1
#define     SUCCESS         0

#define     FEATURE_ON      2
#define     FEATURE_OFF     1

/*------------ Card In -------------*/


#define SET_BIT0        0x01
#define SET_BIT1        0x02
#define SET_BIT2        0x04
#define SET_BIT3        0x08
#define SET_BIT4        0x10
#define SET_BIT5        0x20
#define SET_BIT6        0x40
#define SET_BIT7        0x80

#define SET_BIT8        0x0100
#define SET_BIT9        0x0200
#define SET_BIT10       0x0400
#define SET_BIT11       0x0800
#define SET_BIT12       0x1000
#define SET_BIT13       0x2000
#define SET_BIT14       0x4000
#define SET_BIT15       0x8000

#define CHK_BIT0        0x01
#define CHK_BIT1        0x02
#define CHK_BIT2        0x04
#define CHK_BIT3        0x08
#define CHK_BIT4        0x10
#define CHK_BIT5        0x20
#define CHK_BIT6        0x40
#define CHK_BIT7        0x80


#define CLR_BIT0        0xFFFE
#define CLR_BIT1        0xFFFD
#define CLR_BIT2        0xFFFB
#define CLR_BIT3        0xFFF7
#define CLR_BIT4        0xFFEF
#define CLR_BIT5        0xFFDF
#define CLR_BIT6        0xFFBF
#define CLR_BIT7        0xFF7F

#define CLR_BIT8        0xFEFF
#define CLR_BIT9        0xFDFF
#define CLR_BIT10       0xFBFF
#define CLR_BIT11       0xF7FF
#define CLR_BIT12       0xEFFF
#define CLR_BIT13       0xDFFF
#define CLR_BIT14       0xBFFF
#define CLR_BIT15       0x7FFF




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


#define		REG_EP0_EN			0xA0
#define		REG_USB_INT_EVENT		0xD0
#define		REG_BULK_EVENT			0xC0
#define		REG_BULK_ACK			0xC2
#define Libprintf		printf
#define		REG_BULK_ACK_EN			0xD2
#define		REG_USBCFG			0xB1
#define		REG_BULK_EN0			0xA1
#define K_InDataPhase                   0x81
#define K_InDataPhaseRead               0x82
#define K_InResponsePhase               0x83
#define K_InCompletionPhase             0x84
#define		REG_USB_CONFIG			0xC3
#define		REG_BMREQUESTTYPE		0xF0
#define		REG_SUSCNTRST			0x28
#define K_USB_CLASS_IN_OPEN             0x01
#define		REG_BREQUEST			0xF1
#define		REG_WINDEX7_0			0xF4
#define		REG_EP0BUFDATA			0xB500
#define		REG_BULK_BUF_CLR		0xB5E9
#define K_STDREQCODE_GET_DESCRIPTOR     0x06

#define USB_DETECT_TIMEOUT			0x1FFFF  //20081211 chiayen add
#define PseudoCard_TimeOut		1200   
#define SDMMCTimeOutLoop_1000   1000
#define SDMMCTimeOutLoop_10000  65535


/*-------- EndPoint0 Index Descriptor Manager Table -------*/

#define      Device_DescriptorTable_Begine_Base                0
#define      Device_DescriptorTable_Length                    18
#define      Configuration_DescriptorTable_Length             39
#define      iLANGID_DescriptorTable_Length                    4
#define      iManufacturer_DescriptorTable_Length             26  //0x10
#define      iProduct_DescriptorTable_Length                  48  //0x20
#define      iSerialNumber_DescriptorTable_Length             28  //0x30
#define      iConfiguraiton_DescriptorTable_Length            10  //0x40
#define      iDeviceQualifierString_DescriptorTable_Length    10  //0x50
#define      iInterface_DescriptorTable_Length                10  //0x60
#define      Other_Speed_Configuration_DescriptorTable_Length 39
#define      DeviceQualifier_DescriptorTable_Length           10
#define      InquiryTableLength				      			  36

#define      Device_DescriptorTable_Begine                     Device_DescriptorTable_Begine_Base
#define      Configuration_DescriptorTable_Begine              Device_DescriptorTable_Begine+Device_DescriptorTable_Length
#define      iLANGID_DescriptorTable_Begine                    Configuration_DescriptorTable_Begine+Configuration_DescriptorTable_Length
#define      iManufacturer_DescriptorTable_Begine              iLANGID_DescriptorTable_Begine+iLANGID_DescriptorTable_Length
#define      iProduct_DescriptorTable_Begine                   iManufacturer_DescriptorTable_Begine+iManufacturer_DescriptorTable_Length
#define      iSerialNumber_DescriptorTable_Begine              iProduct_DescriptorTable_Begine+iProduct_DescriptorTable_Length
#define      iConfiguraiton_DescriptorTable_Begine             iSerialNumber_DescriptorTable_Begine+iSerialNumber_DescriptorTable_Length
#define      iDeviceQualifierString_DescriptorTable_Begine     iConfiguraiton_DescriptorTable_Begine+iConfiguraiton_DescriptorTable_Length
#define      iInterface_DescriptorTable_Begine                 iDeviceQualifierString_DescriptorTable_Begine+iDeviceQualifierString_DescriptorTable_Length
#define      Other_Speed_Configuration_DescriptorTable_Begine  iInterface_DescriptorTable_Begine+iInterface_DescriptorTable_Length
#define      DeviceQualifier_DescriptorTable_Begine            Other_Speed_Configuration_DescriptorTable_Begine+Other_Speed_Configuration_DescriptorTable_Length
#define      End_Begine                                        DeviceQualifier_DescriptorTable_Begine+DeviceQualifier_DescriptorTable_Length
#define	     InquiryTableBegin				       DeviceQualifier_DescriptorTable_Begine+DeviceQualifier_DescriptorTable_Length


#define      Device_DescriptorTable_End                        Configuration_DescriptorTable_Begine
#define      Configuration_DescriptorTable_End                 iLANGID_DescriptorTable_Begine
#define      iLANGID_DescriptorTable_End                       iManufacturer_DescriptorTable_Begine
#define      iManufacturer_DescriptorTable_End                 iProduct_DescriptorTable_Begine
#define      iProduct_DescriptorTable_End                      iSerialNumber_DescriptorTable_Begine
#define      iSerialNumber_DescriptorTable_End                 iConfiguraiton_DescriptorTable_Begine
#define      iConfiguraiton_DescriptorTable_End                iDeviceQualifierString_DescriptorTable_Begine
#define      iDeviceQualifierString_DescriptorTable_End        iInterface_DescriptorTable_Begine
#define      iInterface_DescriptorTable_End                    Other_Speed_Configuration_DescriptorTable_Begine
#define      Other_Speed_Configuration_DescriptorTable_End     DeviceQualifier_DescriptorTable_Begine
#define      DeviceQualifier_DescriptorTable_End               End_Begine


// for CX Configuration and Status Register
#define      CX_DONE			0x01        // W
#define      TST_PKDONE			0x02        // W
#define      CX_STL			0x04        // R/w
#define      CX_CLR			0x08        // W
#define      CX_FUL			0x10        // R
#define      CX_EMP			0x20        // R

/*-------- EndPint0 Index Description -------*/
#define      FiFo_CFG                      0    //0:   EndPoint1 ==> FiFo0   ==> SingalBuffer 512  ==> BulkIN   Port 0xC0
					                            //0:   EndPoint2 ==> FiFo1   ==> SingalBuffer 512  ==> BulkOut  Port 0xC4
					                            //1:   EndPoint1 ==> FiFo0/1 ==> DoubleBuffer 512  ==> BulkIN   Port 0xC0
					                            //1:   EndPoint2 ==> FiFo2/3 ==> DoubleBuffer 512  ==> BulkOut  Port 0xC8
#define      iLangeIDRequest            0x00
#define      iManufacturer              0x10
#define      iProduct                   0x20
#define      iSerialNumber              0x30
#define      iConfiguraiton             0x40
#define      iDeviceQualifierCFGString  0x50
#define      iInterface                 0x60



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

#define      NoSense_SenseKey              0
#define      RecoveredError_SenseKey       1
#define      NotReady_SenseKey             2
#define      MediumError_SenseKey          3
#define      HardwareError_SenseKey        4
#define      IllegalRequest_SenseKey       5
#define      UnitAttention_SenseKey        6
#define      DataProtect_SenseKey          7
#define      BlankCheck_SenseKey           8
#define      VendorSpecific_SenseKey       9
#define      CopyAborted_SenseKey         10
#define      AbortedCommand_SenseKey      11
#define      VolumeOverflow_SenseKey      13
#define      Miscompare_SenseKey          14


// ----------------------------- Flash Manufactur --------------------------- //
	#define	SAMSUNG					0xEC
	#define	TOSHIBA					0x98
	#define	HYNIX					0xAD
	#define	SANDISK					0x45
	#define	RENESAS					0x07
	#define	MICRON					0x2C
	#define INTEL					0x89
	#define	ST					    0x20
// ------------------------------ SMC/xD/Flash ID --------------------------- //
//8 Mbit / 1 MByte
	#define	F08M33V1				0xE8
	#define	F08M33V2				0xEC

//16 Mbit / 2 MByte
	#define	F16M33V					0xEA
	#define	F16M50V					0x64

//32 Mbit / 4 MByte
	#define	F32M33V1				0xE3
	#define	F32M33V2				0xE5
	#define	F32M50V1				0x6B
	#define	F32M50V2				0xE5

//64 Mbit / 8 MByte
	#define	F64M33V					0xE6

//128 Mbit / 16 MByte
	#define	F128M33V				0x73
	#define	F128M33V_16bit				0x53

//256 MBit / 32 MByte
	#define	F256M33V				0x75
	#define	F256M33V_16bit				0x55

//512 MBit / 64 MByte
	#define	F512M33V				0x76
	#define	F512M33V_16bit				0x56

//1024 MBit / 128 MByte
	#define	F1024M33V				0x79
	#define	F1024M33V_16bit				0x74

//1 Gbit / 128 MByte
	#define	SamsungF1024M33V			0xF1
	#define	SamsungF1024M33V_16bit			0xC1

//2 Gbit / 256 MByte
	#define	SamsungF2048M33V			0xDA
	#define SamsungF2048M33V_16bit			0xCA
	#define	F2048M33V_16KB				0x71	//xD 256M

//4 Gbit / 512 MByte
	#define	SamsungF4096M33V			0xDC
	#define	SamsungF4096M33V_16bit			0xCC
		#define   Samsung_4G_MLC_ID_8Bit		0x14
		#define   Toshiba_4G_MLC_ID_8Bit		0x84
	#define	F4096M33V_16KB				0xDC	//xD 512M

//8 Gbit / 1 GByte
	#define	SamsungF8192M33V			0xD3
	#define	SamsungF8192M33V_16bit			0xC3
		#define   Samsung_8G_MLC_ID_8Bit		0x55
		#define   Toshiba_8G_MLC_ID_8Bit		0x84
	#define	F8192M33V_16KB				0xD3	//xD 1G

//16 Gbit / 2 GByte
	#define	SamsungF16384M33V			0xD5
	#define SamsungF16384M33V_16bit		0xC5
// 32 Gbit/ 4GByte
    #define	SamsungF32768M33V			0xD7
	#define SamsungF32768M33V_16bit		0xC7  
// 64 Gbit/ 8GByte
    #define	SamsungF65536M33V			0xDE
	#define SamsungF65536M33V_16bit		0xCE    	
	  

//AG-AND 256Mbit / 32 MByte
	#define	AG_AND256M     				0x08

//AG-AND 1Gbit / 128MByte
	#define	AG_AND1G       				0x01

//AG-AND 2Gbit / 256MByte
	#define	AG_AND2G       				0x20

//AG-AND 4Gbit / 512MByte
	#define	AG_AND4G       				0x29
	#define	AG_AND4G_16bit				0x28

// --------------------------- SMC/xD/Flash Command ------------------------- //
// Flash Reset Command
	#define	FResetCmd				0xFF

// Flash Read ID Command
	#define	FReadIDCmd				0x90

// Flash Erase Command
	#define	FBlockEraseCmd1				0x60
	#define	FBlockEraseCmd2				0xD0

// Flash Read Page Command
	#define	FRead1Cmd_1st				0x00
	#define	FRead1Cmd				    0x00
	#define	FRead1Cmd_2nd				0x01
	#define	FReadSpareCmd				0x50
	#define	FReadCmd_End				0x30

// Flash Program Page Command
	#define	FSeqDataInCmd				0x80
	#define	FProgramCmd				    0x10

// Flash Read Status Command
	#define	FReadStatusCmd				0x70




#define	MULTI_FLASH					0
#define	PhyDevStart					0

#define	MaxDevSize					4	//先假設可以Support 4顆Flash




#define	ID_NOT_FND					0x10

// ------------ 定義Complier參數 --------------- //
#define Extend_NUM              2
#define Dummy_NUM               2
#define RANDOM_NUM              1
#define SWAP_NUM                1
#define FAST_NUM                1 
#define RESERVE_NUM            	1
#define ZONE_TOTAL_OTHER        (Extend_NUM+ Dummy_NUM+RANDOM_NUM +SWAP_NUM+FAST_NUM) 
#define ZONE_EDRSF_NUM      	(Extend_NUM+ Dummy_NUM+RANDOM_NUM +SWAP_NUM+FAST_NUM +RESERVE_NUM)// E:2 D:2 R :1  S:1  F:1  reserve :1  
#define MAX_RANDOM_HBLOCK       4 // random block 最多支援幾個Hblock 
#define MAX_RANDOM_HPAGE        8 //每個Random 支援幾個Hblock page
#define MAX_DUMMY_PAGE          16 //每個dummy 支援EXT page 數
#define MAPRAM_NUM          	2
#define SWAP_HBLOCK_SIZE        2 
#define EXT_SMALL_BLOCK_SIZE  	4  // 定義small block  大小單位是page  // 0520 jay change kb to page
#define ZSPARE_DATA_SIZE        8  
#define RANDOM_SBLK_SIZE        1   // 定義大小  Random  small block size 單位 page 
#define RANDOM_MAX_SECTORCNT    8  //unit sector(設小可以提高小檔速度，但是影響極速反之提高極速，可能可以分LBA來設定）
#define MAX_BUF_SIZE            16  //定義可是用BUF 大小
#define COPY_OR_PASS            4   //決定當超過多少small block 後不搬  
#define HBLOCK_LINEAR_MODE      
#define	DEBUG					0
#define	NEW_ECC					1
#define	WEAR_LEVELING			0
#define Mul_LUN               	1	// 決定是否支援Mul-LUN
#define Bat160mAh
#define	BankingCode_Recover		1  //08/11/04,joyce add 

#define Enhance_BMT_Num         2  //090820  Jay add
#define pEnhanceBMT             128 //090820  Jay add
#define pFastBMTBackup          128 //090820  Jay add
#define pEnhance_Fast_BMT       144 //090820  Jay add
#define pEnhance_User_Backup    96  //090820  Jay add
#define MAX_BMT_ECCbit_Num      7  //090820  Jay add

#define DEBUG_VERSION
#define EN_RECOVER_BLOCK        1	//0614 Jay add 
#define MAX_ERRBIT_OFFSET       3

#define EN_WEAR_LEVELING        1


//************** File System **************//
#define	DOS_SUCCESS         			0x00	//!< 0 to be returned on sucess in FAT functions.
#define	DOS_FALSE           			0x01	//!< 1 to be returned on failure in FAT functions.

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
#define K_DOS_SectorSize				512

#define MCU_12MHz               		1 

//-----------------------------------------------------------------------------
// Key pressed. This info means which key is pressed we detect. This is 1st
// level of key info. Value's range is 0x00-0x0F.
//-----------------------------------------------------------------------------
#define C_Key_Play                  0x01
#define C_Key_Mode                  0x02
#define C_Key_Next					0x03
#define C_Key_Prev               	0x04
#define C_Key_Volup             	0x05
#define C_Key_Voldn                	0x06
#define C_Key_Rec                   0x07
#define C_Key_Vol	 				0x08

//-----------------------------------------------------------------------------
// Long keys. 0x10 | (key pressed)
//-----------------------------------------------------------------------------
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

#define C_Task_Idle                 0
#define C_Task_Play					1
#define C_Task_Rec                  2
#define C_Task_Voice                3
#define C_Task_Jpeg                 4
#define C_Task_Mjpeg				5
#define C_Task_FMRecv               6
#define C_Task_Menu                 7
#define C_Task_Setting              8

#define C_Task_USB					9
#define C_Task_LoBatt				10
#define C_Task_FSErr				11
#define C_Task_DiscFull             12

#define C_Task_Dir                  13
#define C_Task_PlayMenu_IR          14  //20090107 chiayen add
#define C_Task_PlayMenu             15  //20090107 chiayen add
#define	C_Task_PowerOff				16
#define C_Task_RPTMenu				17

#define TASK_PHASE_PLAYACT          0x00
#define TASK_PHASE_PAUSE			0x01
#define TASK_PHASE_STOP				0x02
#define TASK_PHASE_RECACT           0x03
#define TASK_PHASE_FASTFWD          0x04
#define TASK_PHASE_FASTREV          0x05

//define for audio
#define AUDIO_FILE_TYPE_MP3         0x00
#define AUDIO_FILE_TYPE_WMA         0x01
#define AUDIO_FILE_TYPE_WMV         0x02
#define AUDIO_FILE_TYPE_APE         0x03

//define for voice
#define AUDIO_FILE_TYPE_WAV         0x00
#define AUDIO_FILE_TYPE_ASF         0x01
//=========DSP i/f==========
#define DSP_RESET 0   // ycc081117 

#define D_DspTimeout		
//=========DSP i/f==========

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
#define C_TuneVol       0x10  

#define C_FmFreqChgUp	0x11  //chiayen0807
#define C_FmFreqChgDown	0x12  //chiayen0807
#define C_PlayModeChg	0x13  //chiayen0807
#define C_FmFreqChg		0x14  //chiayen0807
#define C_PowerOff		0x15

#define C_RecIdle       0x00
#define C_RecInit       0x01
#define C_RecProc       0x02
#define C_RecEndfile    0x03
#define C_RecPause      0x04
#define C_RecStop       0x05
#define C_RecUpLevel	0x06
#define C_RecLRec_Stop	0x07
#define C_RecDiskFull	0x08//sq added for full disk@081030

#define C_VocIdle  	    0x00
#define C_VocInit   	0x01
#define C_VocProc		0x02
#define C_VocEndfile    0x03
#define C_VocPause  	0x04
#define C_VocStop       0x05
#define C_VocNext		0x06
#define C_VocPrev       0x07
#define C_VocFF         0x08
#define C_VocFR         0x09
#define C_VocFfFrEnd    0x0A
#define C_VocSetRepAB   0x0B
#define C_VocVolUp		0x0C
#define C_VocVolDn		0x0D
#define C_VocUpLevel    0x0E
#define C_VocMenu       0x0F 
#define C_VocTuneVol   	0x10 

#define C_JpegIdle 	    0x00
#define C_JpegProc		0x01
#define C_JpegPause  	0x02
#define C_JpegStop      0x03
#define C_JpegEndfile   0x04
#define C_JpegNext		0x05
#define C_JpegPrev      0x06
#define C_JpegUpLevel   0x07

#define C_MjpegIdle 	0x00
#define C_MjpegInit     0x01
#define C_MjpegProc		0x02
#define C_MjpegPause  	0x03
#define C_MjpegStop     0x04
#define C_MjpegEndfile  0x05
#define C_MjpegNext		0x06
#define C_MjpegPrev     0x07
#define C_MjpegFF       0x08
#define C_MjpegFR       0x09
#define C_MjpegFfFrEnd  0x0A
#define C_MjpegVolUp    0x0B
#define C_MjpegVolDn    0x0C
#define C_MjpegUpLevel  0x0D

#define C_FMRIdle       0x00
#define C_FMRProc       0x01
#define C_FMRStop		0x02
#define C_FMRNextCH		0x03
#define C_FMRPrevCH		0x04
#define C_FMRVolUp		0x05
#define C_FMRVolDn		0x06
#define C_FMRUpLevel    0x07


#define C_MenuInit      0x00
#define C_MenuProc      0x01
#define C_MenuIdle      0x02

#define C_PhoneBookInit	0x00
#define C_PhoneBookProc	0x01

#define C_SettingInit	0x00
#define C_SettingProc	0x01
//***************************

// We use definitions here to access gac_ratio_table[].  Please make sure the right mapping.
#define CLOCK_MODE_XTL			0
#define CLOCK_MODE_MP3		1
#define CLOCK_MODE_MP3L		2
#define CLOCK_MODE_WMA	4//	3
#define CLOCK_MODE_DOS		4
#define	CLOCK_MODE_USB		4

#define C_T0_06MhzMSB				(unsigned char)(-3750>>8)
#define C_T0_06MhzLSB				(unsigned char)(-3750)

#define C_T0_12MhzMSB				(unsigned char)(-7500>>8)
#define C_T0_12MhzLSB				(unsigned char)(-7500)

#define C_T0_24MhzMSB				(unsigned char)(-15000>>8)
#define C_T0_24MhzLSB				(unsigned char)(-15000)

#define C_T0_36MhzMSB				(unsigned char)(-22500>>8)
#define C_T0_36MhzLSB				(unsigned char)(-22500)

#define C_T0_39MhzMSB				(unsigned char)(-24375>>8)
#define C_T0_39MhzLSB				(unsigned char)(-24375)

#define	C_T0_42MhzMSB				(unsigned char)(-26250>>8)		//Jimi 081222
#define C_T0_42MhzLSB				(unsigned char)(-26250)			//Jimi 081222

#define	C_T0_45MhzMSB				(unsigned char)(-28125>>8)		//Jimi 081222
#define C_T0_45MhzLSB				(unsigned char)(-28125)			//Jimi 081222

#define C_T0_48MhzMSB				(unsigned char)(-30000>>8)
#define C_T0_48MhzLSB				(unsigned char)(-30000)

#define C_T0_72MhzMSB				(unsigned char)(-45000>>8)
#define C_T0_72MhzLSB				(unsigned char)(-45000)

#define	C_T0_78MhzMSB				(unsigned char)(-48750>>8)
#define C_T0_78MhzLSB				(unsigned char)(-48750)

#define C_NoRepeat          0x00
#define C_RepeatOne         0x01
#define C_RepeatAll         0x02
#define C_RandomPlay		0x03
#define C_IntroPlay         0x04
#define C_InDirPlay         0x05
#define C_InDirRepeat       0x06

//******************************
enum {
	ONE_SECTOR = 512
};

enum MP3 {
	REPEAT_AB = 0,
	REPEAT_A = 1,
	REPEAT_B = 2,
	REPEAT_AB_START = 3,
	REPEAT_AB_NULL = 4
};

typedef struct DSP_PLAY_GLOBAL_ {
	U8 sc_RepeatMode;
}DSP_PLAY_GLOBAL;

typedef struct DSP_GLOBAL_ {
	
	U16 sw_Volume;
	U8  sc_EQ_Type;//(JC)H0604
	U8  sc_DM_Index;
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
//===================(JC)TaskEventFlag====================
#define C_PollingEventNo    0x05


#define M_SetFlag_USB		gc_TaskEvent_bFlag |= SET_BIT0
#define M_ChkFlag_USB		gc_TaskEvent_bFlag & SET_BIT0
#define M_ClrFlag_USB		gc_TaskEvent_bFlag &= CLR_BIT0

#define M_SetFlag_LoBatt	gc_TaskEvent_bFlag |= SET_BIT1
#define M_ChkFlag_LoBatt	gc_TaskEvent_bFlag & SET_BIT1
#define M_ClrFlag_LoBatt	gc_TaskEvent_bFlag &= CLR_BIT1

#define M_SetFlag_FSErr	    gc_TaskEvent_bFlag |= SET_BIT2
#define M_ChkFlag_FSErr 	gc_TaskEvent_bFlag & SET_BIT2
#define M_ClrFlag_FSErr 	gc_TaskEvent_bFlag &= CLR_BIT2

#define M_SetFlag_DiscFull  gc_TaskEvent_bFlag |= SET_BIT3
#define M_ChkFlag_DiscFull 	gc_TaskEvent_bFlag & SET_BIT3
#define M_ClrFlag_DiscFull 	gc_TaskEvent_bFlag &= CLR_BIT3

#define	LRC_DISPLAY_LYRICS_CHARNUMBER	100

// For RTC interrupt flags.
#define RTC_SECOND_INTERRUPT	0x01
#define RTC_ALARM_INTERRUPT		0x02

//for ADPCM
#define C_8kSampleRate    0x08
#define C_16kSampleRate   0x10

#define C_ChgSpectrumTimerCnt	0x04

#define D_Valid			0x55
#define D_Valid2USB		0x80
#define D_Expired		0xAA

#define D_LRCOffsetLimit	5632  

#define D_DspTimeout			//yflin081208	

#define META_HEADER_SIZE 4
#define SEEK_SIZE 18

/*
#if 1	//Open Debug message 
	#define Dprint	dbprintf
#else
	#define Dprint  /Kprint 
	#define Kprint  /dbprintf
#endif
*/
	
#define D_DemoVol				24
//------------------------------//
// SPDA26xx flash  plan
#define FERASE_CMD1    		0x60
#define FERASE_CMD2     	0xD0
#define FREAD_CMD1      	0x00
#define FREAD_CMD2       	0x30
#define FCACHE_READ       	0x31
#define FCACHE_READ_LAST  	0x3F
#define FWRITE_CMD1         0x80 
#define FWRITE_CMD2      	0x10
#define FCACHE_WRITE1       0x80
#define FCACHE_WRITE2       0x15
#define FRANDOM_INPUT       0x85
#define FRANDOM_OUTPUT1     0x05
#define FRANDOM_OUTPUT2     0xE0


//-----  Table 相關--------------- //
#define CIS_Block_Num       6
#define	User_Block_Num		16
#define DMTBLK_NUM          1
#define BMTBLK_NUM          2	// BMT ＆BMT backup   
#define TEMPBLK_NUM        	1
#define REPLACEBLK_NUM      1

#define TOTAL_MAPBLK_NUM    (CIS_Block_Num+User_Block_Num+DMTBLK_NUM+BMTBLK_NUM+ TEMPBLK_NUM +REPLACEBLK_NUM)


#define MAX_ZONE_SIZE      240	
#endif // !__DEFINE_H__

