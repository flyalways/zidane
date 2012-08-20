#ifndef _EXGLOBAL_H_
#define _EXGLOBAL_H_

extern		BYTE	gc_LCD_Gobal_J;
extern		BYTE	gc_Global_I;
extern		WORD	gw_Global_J;

extern	bit gbt_SDHC_Type;
extern  bit gbt_MMC_Type;
extern	unionWORD  SDMMC_SDRCA;
extern  union	LWDataType	SD_Capacity;
extern	xdata	U32	gdw_CARD_TotalSizeMB;  


extern	bit 	gbt_BuildMapping_Process;
extern	bit 	gbt_Force_USB_FullSpeed;
extern	bit		gbt_USBHostIsHighSpeed;
extern	bit		gbt_Have_set_configuration;
extern	IBYTE	gc_Device_Address;
extern	UBYTE   dCBW_Tag[4];
extern  IBYTE   COMD_IN[10]; // 070627 jay   
extern	idata unionLONGWORD   gdw_dCBW_DataXferLength;                        // 傳輸資料的長度
extern 	idata UBYTE	gc_bCBWCBLength ;  										// 傳輸CMD的長度
extern 	idata UBYTE   gc_bCBWLUN; 	
extern	bit		gbt_bmCBWFlag;
extern	IBYTE   gc_USBStage;                                      	  // 目前Protocol之階段為何
extern	UBYTE   gc_bCSWStatus;                                       // 回填command status 用
extern	bit	gbt_USB_CMD_IN;
extern	UBYTE	gc_RequestSenseCode;
extern	bit	gbt_StallWhichEndPoint;
extern	bit	gbt_XferNormalResponseData_or_ModeSense;
extern  bit gbt_StorageMedia_has_changed;
extern	bit     gbt_USB_Connect_Flag;  // 081208 Jay add
extern	bit     gbt_USER_Remove_Flag; // 08128 Jay add dd
extern	UBYTE	gc_CardType;
extern  UBYTE   gc_CardExist;
extern  UBYTE   gc_CardDetect;
extern  UBYTE   gc_Start_Stop_Flag;
extern  UBYTE   gc_USB_ADDR;
extern  BYTE	gc_CX_Status_flag;
extern	bit	gbt_EP0SETUP_int_flag;
extern	bit	gbt_EP0RX_int_flag;
extern	bit	gbt_EP0TX_int_flag;
extern	bit	gbt_USBRST_int_flag;
extern	bit	gbt_SUSPEND_int_flag;
extern	bit	gbt_RESUME_int_flag;
extern	bit	gbt_USB_CONFIG_Chg_int_flag;
extern  bit   gbt_Support_Dummy_Capacity; // 090416 add for dummy capacity
extern 	unionWORD Start_Prog_Addr;
extern  UBYTE	gc_CardChangeStage;   // 各種Storage status; 

extern	UBYTE USB_I2C_ID;
extern  UBYTE USB_I2C_Addr;
extern  UBYTE USB_I2C_Wdata;
extern  UBYTE USB_I2C_Rdata;
extern  struct 
{
 	BYTE RequestType;
 	BYTE Direction;	// 1   Data transfer direction: IN, OUT
 	BYTE Type;		//     Request Type: Standard, Class, Vendor
 	BYTE Object;		//     Recipient: Device, Interface, Endpoint,other
 	BYTE Request;		// 2   Refer to Table 9-3
 	BYTE Value_L;          // 3
 	BYTE Value_H;          // 4
 	BYTE Index_L;          // 5
 	BYTE Index_H;          // 6
  	//union WordDataType Length;    // 7 8   .BY[1]  Low byte
 	unionWORD Length;	        //       .BY[0]  Hi  byte
} ControlCmd;
extern	bit		gbt_enableCDROM;					//09/04/24,joyce
extern	UBYTE	gc_CDROM_Size;						//09/04/24,joyce(unit:MB) 
/***********************************************************************
*			Flash
************************************************************************/
// SPDA26xx flash  plan
extern struct 
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
extern struct 
{	
	UBYTE cFDev;
	UBYTE cFPlane;
	UWORD wFBlock;
	UWORD wFPage;
	UBYTE cFInAddr;
}FDBP;
extern struct 
{	
	UBYTE cFDev;
	UBYTE cFPlane;
	UWORD wFBlock;
	UWORD wFPage;
	UBYTE cFInAddr;
}BMT;
extern struct 
{
	UBYTE cHPAGESECTOR;
	UWORD wHBLOCKSECTOR;
	UWORD wHBLOCKPAGE;
	UBYTE cSMALLBLOCK_NUM;
	UBYTE cSMALLPAGE_NUM;


}H_INFO;


extern struct 
{
	UBYTE cTOTALZONE_NUM;
	UBYTE cDEV_ZONE_NUM;
	UWORD wDEV_DATABLK_NUM;
	UWORD wZONE_SIZE;


}SYS_ZONE;
	// default max 4 device 8192 block  
	// 當使用8192 block 最多只能支援2 ce function 	
	extern 	XWORD   DMTTable[4];
	extern 	XWORD   DMT_Page[4];
	extern 	XWORD   BMTTable[4];
	extern 	XWORD   BMTTable_Backup[4];
	extern 	XWORD   FAST_BMTTable[64];
	extern 	XWORD   FAST_BMTPage_Table[64];
	extern 	XWORD   TempBlock_Table[4];
	extern 	XWORD   ReplacementBlock_Table[4];
	extern 	XWORD	ReplacementBlock_Page[4];
	extern 	XWORD   CIS_Block[];
	extern  XWORD   *Mapping_Table;  //
    extern _XBYTE  *Wear_Table;


	//----------MAP Table 相關 ----------------------------//
	extern  DBYTE   MAP_Table[ ];// 假設切割成兩塊管理
	extern 	DBYTE   BMT_KEY[];
	extern 	DBYTE   gc_Write_EDRS ; //	
	extern  IWORD	APort_FBlock[];  	 
	extern  IWORD	APort_LBlock[]; 	  		
	extern  IWORD	APort_EBlock[];    
	extern  IWORD   APort_DBlock[];
	extern  IWORD   APort_RBlock[];
	extern  IWORD   APort_SBlock[];			
//	extern  IWORD   APort_Q1[];
//	extern  IWORD   APort_Q2[];	
	extern  IBYTE   APort_FCNT[];      	
    extern  IBYTE   APort_LCNT[];
    extern  IBYTE   APort_ECNT[];
    extern  IBYTE   APort_DCNT[];
    extern  IBYTE   APort_RCNT[];         
    extern  IBYTE   APort_SCNT[];
    extern  IBYTE   ID_Table[];


      //------- HBLCOK 相關 ---------------------------//
    extern 	DBYTE  	ZSpare[];
    extern  UBYTE   gc_HDev;
    extern  UBYTE   gc_HInAddr;    
    extern 	union	WORDDataType	HBlock;
    extern 	union	WORDDataType	HPage;
    extern 	union	WORDDataType	QTable;
    extern   union	WORDDataType	LBlock;
  	extern   union	WORDDataType	FCOL_ADDR;
  	extern  UBYTE   gc_Q1_SBlock;
  	extern 	UBYTE   gc_SBlock ;
   	extern  UBYTE   gc_SPage;
   	extern 	UBYTE   gc_D_SBlock;
   	extern  UBYTE   gc_NowIndex;  //R D page中排列的的位置   
    extern 	UBYTE   gc_E_SBlock;
    extern 	UBYTE   gc_E_SPage;
    extern 	union	WORDDataType	SQTable;//紀錄Dummy block 中Q1~Q2 page
    extern  UBYTE 	gc_T_SBlock;
    extern 	UWORD   gw_R_SPage;
    //---------- FLASH control 相關變數 -----------//
    extern  bit		gbt_newFlash;
    extern 	bit		gbt_ECC_Fail;
	extern 	bit		gbt_OddTemp;
extern	bit	gbt_Need_Up_Mapping;
extern	bit	gbt_Need_Write_Mapping;
	extern 	bit		gbt_BMT_Key;
	extern	bit     gbt_Need_Build_Mapping;
	extern  bit     gbt_USB_Data_In_Buf;
	extern 	UBYTE   gc_ECC_Error_Bit;
extern	UBYTE	gc_WriteProof;
extern	UBYTE	gc_Flash_CMD1;
extern	UBYTE	gc_Flash_CMD2;
extern  UBYTE   gc_Flash_CMD_Type; 
extern	UBYTE	gc_ErrorCode;
	extern	UBYTE   gc_Load_Code_Size ;
	extern  UBYTE	gc_RW_Strobe; 

extern	unionWORD	TotalXferPageNum;
    extern  UWORD	gw_WhichGroup; 
    extern  UWORD	gw_NowBMTPoint;
    extern  UWORD   gw_DataBlockSize;
    extern	UWORD   gw_SourceBlock;
    extern  UWORD   gw_TargetBlock;
    //~~~ 0415~~~   
   	extern 	bit		gbt_Marked ;
    extern 	bit     gbt_Send_10_11_CMD;
    extern 	bit     gbt_Send_80_81_CMD;
    extern 	bit		gbt_Data_In_Buf;
//    extern	bit		gbt_Need_Keep_Data;
    extern	bit 	gbt_Program_Erase_Fail;
    extern  bit     gbt_Force_SeqWrite_Mode;
    extern   bit    gbt_NeedMask_DummyPage;
    extern  UBYTE   gc_MAX_ERR_BIT_NUM;
    //--------  SYS 相關的變數--------------------//
    extern	union	LWDataType		Capacity;
	extern	union   LWDataType      Dummy_Capacity;// 090416  add for dummy capacity
	extern	union	LWDataType		Capacity_CDROM;		//09/04/24,joyce for CD-ROM
	extern	union	LWDataType		Capacity_Remove;	//09/04/24,joyce for CD-ROM
	extern	union	LWDataType		LBA;
	extern	union	LWDataType		Dos_LBA;
	extern	union	WORDDataType	Dos_Sec_Cnt;
extern	unionWORD	Device_Read_SRAM_Index;
extern	unionWORD	Device_Write_SRAM_Index;
extern	unionWORD	Device_Xfer_DataLength;
    extern 	bit     gbt_RW_UsbBuf;
    extern   bit 	gbt_RW_CD_ROM;
    extern 	IBYTE	gc_ST_ErrorCode;
    extern	UWORD   gw_MultPalne_BLK;
    extern  UWORD   gw_Single_BLK;
    extern  UWORD   gw_SourcePage;
    extern 	UWORD   gw_TargetPage;
    extern  U8      gc_FlashStatus;


    
    //--------- CODE  HZK  相關變數 -------------------//
extern  UBYTE   gc_CodeBlock_Num;
extern	UBYTE   gc_DSPBlock_Num;
extern	UBYTE   gc_HZKBlock_Num;
	extern	UBYTE	gc_HZKsize;
    extern	XWORD	DSP_Block_A[16];
	extern	XWORD	DSP_Block_B[16];
	extern	XWORD	HZK_Block_A[64];  
	extern	XWORD	UserData_Block[User_Block_Num];  
	extern  XWORD 	Bank_Info_Block[2];
	extern  XWORD	Bank_Block_A[16];			
	extern 	XWORD 	Bank_Block_B[16];
	
	
	
	extern  bit		gbt_Enable_Replacement;
	extern 	bit		gbt_Enable_ReadRecover;
	extern	bit		gbt_Read_NeedRecover;
	extern	bit		gbt_Code_NeedRecover;
	extern  U8      gc_Flash_Data;
	extern 	U16     gw_PagesPerBlock;  // 相容於SPDA23XX
	extern 	U8  	gc_InAddrMax;      // 1k base 
	
	
	
  	extern	bit		gbt_USB_Detected;
	extern	bit		gbt_USB_Detected_backUp;
 
#endif	