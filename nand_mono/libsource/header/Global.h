	BYTE	gc_LCD_Gobal_J;
	BYTE	gc_Global_I;
	WORD	gw_Global_J;



/************************************************************************
*				SD
************************************************************************/
	bit gbt_SDHC_Type;
	bit gbt_MMC_Type;
	unionWORD  SDMMC_SDRCA;	
union	LWDataType		SD_Capacity;
	xdata	U32	gdw_CARD_TotalSizeMB;  


/************************************************************************
*				USB
************************************************************************/
idata	unionLONGWORD	gdw_dCBW_DataXferLength;				// 傳輸資料的長度
idata 	UBYTE	gc_bCBWCBLength ;  										// 傳輸CMD的長度
idata	UBYTE   gc_bCBWLUN;
		UBYTE	gc_CardChangeStage;   // 各種Storage status
	bit  gbt_StorageMedia_has_changed;		
struct 
{
	BYTE RequestType;
	BYTE Direction;			//	1	Data transfer direction: IN, OUT
	BYTE Type;				//		Request Type: Standard, Class, Vendor
	BYTE Object;			//		Recipient: Device, Interface, Endpoint,other
	BYTE Request;			//	2	Refer to Table 9-3
	BYTE Value_L;			//	3
	BYTE Value_H;			//	4
	BYTE Index_L;			//	5
	BYTE Index_H;			//	6
	//union WordDataType Length;    // 7 8   .BY[1]  Low byte
	unionWORD Length;		//		.BY[0]  Hi  byte
} ControlCmd;

	bit 	gbt_BuildMapping_Process;
	bit		gbt_Force_USB_FullSpeed;
	bit		gbt_USBHostIsHighSpeed;
	bit		gbt_Have_set_configuration;	
	bit		gbt_bmCBWFlag;
	bit		gbt_USB_CMD_IN;	
	bit		gbt_StallWhichEndPoint;
	bit		gbt_XferNormalResponseData_or_ModeSense;
	bit		gbt_EP0SETUP_int_flag;
	bit		gbt_EP0RX_int_flag;
	bit		gbt_EP0TX_int_flag;
	bit		gbt_USBRST_int_flag;
	bit		gbt_SUSPEND_int_flag;
	bit		gbt_RESUME_int_flag;
	bit		gbt_USB_CONFIG_Chg_int_flag;
	bit     gbt_USB_Connect_Flag;
	bit     gbt_USER_Remove_Flag;
	bit     gbt_Support_Dummy_Capacity; // 090416 add for dummy capacity 
	bit		gbt_enableCDROM;					//09/04/24,joyce
	UBYTE	gc_CDROM_Size;						//09/04/24,joyce(unit:MB) 
	UBYTE	gc_bCSWStatus;                                       // 回填command status 用
	UBYTE	gc_RequestSenseCode;
	UBYTE	gc_CardType;
	UBYTE   gc_CardExist;
	UBYTE   gc_CardDetect; 
	UBYTE   gc_Start_Stop_Flag;
	UBYTE   gc_USB_ADDR;
	UBYTE	gc_CX_Status_flag;		
	IBYTE	gc_Device_Address;
	IBYTE	gc_USBStage;										// 目前Protocol之階段為何
	UBYTE	dCBW_Tag[4];
	IBYTE	COMD_IN[10];										// 070627 jay
	unionWORD Start_Prog_Addr;
	UBYTE 	USB_I2C_ID;         	
	UBYTE 	USB_I2C_Addr;       	
	UBYTE 	USB_I2C_Wdata;      	
	UBYTE 	USB_I2C_Rdata; 
/***********************************************************************
*			Flash
************************************************************************/
// SPDA26xx flash  plan
struct  
{
	UBYTE cDEVICE_NUM;
	UBYTE cPLANE_NUM;
	UWORD wBLOCK_NUM;
	UWORD wPAGE_NUM;
	UBYTE cINADDRMAX;

	UBYTE cFMAP;
	UBYTE cADDR_CYCLE;
	UBYTE cFLASH_TYPE; //0-->SLC ,1-->MLC ,2-->TLC
	UBYTE cECC_MODE ;// 0-->24 1-->16
	UBYTE cECC_NUM;	
	UBYTE cSUPPORT_MULT_PLANE;
	UBYTE cSUPPORT_CACHE_RW; 
}NAND_INFO;

struct 
{	
	UBYTE cFDev;
	UBYTE cFPlane;
	UWORD wFBlock;
	UWORD wFPage;
	UBYTE cFInAddr;
	
}FDBP;

struct 
{	
	UBYTE cFDev;
	UBYTE cFPlane;
	UWORD wFBlock;
	UWORD wFPage;
	UBYTE cFInAddr;
}BMT;
struct 
{
	UBYTE cTOTALZONE_NUM;
	UBYTE cDEV_ZONE_NUM;
	UWORD wDEV_DATABLK_NUM;
	UWORD wZONE_SIZE;
	
}SYS_ZONE;

struct 
{
	UBYTE cHPAGESECTOR;
	UWORD wHBLOCKSECTOR;
	UWORD wHBLOCKPAGE;
	UBYTE cSMALLBLOCK_NUM;
	UBYTE cSMALLPAGE_NUM;
}H_INFO;

	//---------  DMT Table  相關 ---------------------------//
	// default max 4 device 8192 block  
	// 當使用8192 block 最多只能支援2 ce function 	
	
	XWORD   DMTTable[4];
	XWORD   DMT_Page[4];
	XWORD   BMTTable[4];
	XWORD   BMTTable_Backup[4];
	XWORD   FAST_BMTTable[64];
	XWORD   FAST_BMTPage_Table[64];
	XWORD   TempBlock_Table[4];
	XWORD   ReplacementBlock_Table[4];
	XWORD	ReplacementBlock_Page[4];
	XWORD   CIS_Block[6]; 

	
	
	//----------MAP Table 相關 ----------------------------//
	DBYTE   MAP_Table[MAPRAM_NUM];// 假設切割成兩塊管理
	DBYTE   BMT_KEY[MAPRAM_NUM];
	IWORD	APort_FBlock[2]; //use idata  	 
	IWORD	APort_LBlock[1]; 	  		
	IWORD	APort_EBlock[1];    
	IWORD   APort_DBlock[1];
	IWORD   APort_RBlock[1];
	IWORD   APort_SBlock[1];			
//	IWORD   APort_Q1[1];
//	IWORD   APort_Q2[1];	
	IBYTE   APort_FCNT[1];      	
    IBYTE   APort_LCNT[1];
    IBYTE   APort_ECNT[1];
    IBYTE   APort_DCNT[1];
    IBYTE   APort_RCNT[1];         
    IBYTE   APort_SCNT[1];
    IBYTE   ID_Table[5];
    XWORD   *Mapping_Table;  //
    _XBYTE  *Wear_Table;
    
    //------- HBLCOK 相關 ---------------------------//
    DBYTE  	ZSpare[8];   // use data type
    DBYTE   gc_Write_EDRS ; //
    UBYTE   gc_HDev;
    UBYTE   gc_HInAddr;   
    union	WORDDataType	HBlock;
    union	WORDDataType	HPage;
    union	WORDDataType	QTable;
    union	WORDDataType	LBlock;
    union	WORDDataType	FCOL_ADDR;
    UBYTE   gc_Q1_SBlock;
   	UBYTE   gc_SBlock ;
   	UBYTE   gc_SPage;
    UBYTE   gc_D_SBlock;
    UBYTE   gc_NowIndex; 
    UBYTE   gc_E_SBlock;
    UBYTE   gc_E_SPage;
    UBYTE   gc_T_SBlock;
    union	WORDDataType	SQTable;//紀錄Dummy block 中Q1~Q2 page
   	UWORD   gw_R_SPage;
   
	
								
    //---------- FLASH control 相關變數 -----------//
    bit		gbt_newFlash;
	bit		gbt_ECC_Fail;
	bit		gbt_OddTemp;
	bit		gbt_Need_Up_Mapping;
	bit		gbt_Need_Write_Mapping;
	bit     gbt_Need_Build_Mapping;
	bit     gbt_USB_Data_In_Buf;
	UBYTE   gc_ECC_Error_Bit;
	UBYTE	gc_WriteProof;
	UBYTE	gc_Flash_CMD1;
	UBYTE	gc_Flash_CMD2;
	UBYTE	gc_Flash_CMD_Type; 
	UBYTE	gc_ErrorCode;
    UBYTE   gc_Load_Code_Size ; 
    UBYTE	gc_RW_Strobe;

    unionWORD	TotalXferPageNum; 
	UWORD	gw_WhichGroup;
    UWORD	gw_NowBMTPoint;
	UWORD	gw_DataBlockSize;
    UWORD   gw_SourceBlock;
    UWORD   gw_TargetBlock;
    //0415 ~~
    bit		gbt_Marked ;
    bit     gbt_Send_10_11_CMD;
    bit     gbt_Send_80_81_CMD;
    bit		gbt_Data_In_Buf;
//    bit		gbt_Need_Keep_Data;
    bit 	gbt_Program_Erase_Fail;
    bit     gbt_Force_SeqWrite_Mode;
    bit     gbt_NeedMask_DummyPage;
    UBYTE   gc_MAX_ERR_BIT_NUM;
    //--------  SYS 相關的變數--------------------//
    union	LWDataType		Capacity;
	union   LWDataType      Dummy_Capacity;// 090416  add for dummy capacity
	union	LWDataType		Capacity_CDROM;		//09/04/24,joyce for CD-ROM
	union	LWDataType		Capacity_Remove;	//09/04/24,joyce for CD-ROM
	union	LWDataType		LBA;
	union	LWDataType		Dos_LBA;
	union	WORDDataType	Dos_Sec_Cnt;

	unionWORD	Device_Read_SRAM_Index;
	unionWORD	Device_Write_SRAM_Index;
	unionWORD	Device_Xfer_DataLength;
    bit     gbt_RW_UsbBuf;
    bit 	gbt_RW_CD_ROM;
    IBYTE	gc_ST_ErrorCode;
    UWORD   gw_MultPalne_BLK;
    UWORD   gw_Single_BLK;
    UWORD   gw_SourcePage;
    UWORD   gw_TargetPage;
   	U8      gc_FlashStatus;
    //--------- CODE  HZK  相關變數 -------------------//
    UBYTE   gc_CodeBlock_Num;
    UBYTE   gc_DSPBlock_Num;
    UBYTE   gc_HZKBlock_Num; 
	UBYTE	gc_HZKsize;
	XWORD	DSP_Block_A[16];
	XWORD	DSP_Block_B[16];
	XWORD	HZK_Block_A[64];  
	XWORD	UserData_Block[User_Block_Num];
	XWORD 	Bank_Info_Block[2];
	XWORD	Bank_Block_A[16];			
	XWORD 	Bank_Block_B[16];	
	
	bit		gbt_Enable_Replacement;
	bit		gbt_Enable_ReadRecover;
	bit		gbt_Read_NeedRecover;
	bit		gbt_Code_NeedRecover;
	
	
  	bit		gbt_USB_Detected;
	bit		gbt_USB_Detected_backUp;
	U8      gc_Flash_Data;
	U16     gw_PagesPerBlock;  // 相容於SPDA23XX
	U8  	gc_InAddrMax;      // 1k base 
	
	

	