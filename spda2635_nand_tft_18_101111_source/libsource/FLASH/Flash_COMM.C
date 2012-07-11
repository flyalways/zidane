/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	Flash_COMM.c
 *	\brief	
 *
 *	\author	jay
 */

#include "SPDA2K.h"


extern U8	gc_clock_mode;  //20090526 chiayen add

/***************************************************************************
* NAME:         void Check_LoadMap(void)
* DESCRIPTION:
*     This subroutine is to do the Host Read Sector.
*     Input  Vars: // SPDA26xx flash  plan
***************************************************************************/
void Check_LoadMap(void){
	bit tbt_NeedChangeMap=1;
	bit tbt_Disable_Update_Table=0;
	data	U8	tc_Old_CLK_Mode;	  
	xdata	U8	tc_i;
	data	U8	gc_PreWhichGroup;
	data	U16	tw_FastPage;
	for(tc_i=0;tc_i<MAPRAM_NUM;tc_i++){
		if(MAP_Table[tc_i] ==gw_WhichGroup){
			tbt_NeedChangeMap=0;
			gw_NowBMTPoint=tc_i;
			break;
		}
	}	
	if(tbt_NeedChangeMap){
		gw_NowBMTPoint=(gw_NowBMTPoint+1)&(MAPRAM_NUM-1);
		gc_PreWhichGroup = MAP_Table[gw_NowBMTPoint]; 
		if(gc_PreWhichGroup==0xFF){//沒有使用過的的mapping ram 
			tbt_Disable_Update_Table=1;
		}		
	}
	if(gw_NowBMTPoint==1){
		Mapping_Table = MAP_1;  	// switch table pointer
		Wear_Table    = WEARTAB_1;	// switch table pointer
	}
	else{
		Mapping_Table = MAP_0;  	// switch table pointer
		Wear_Table    = WEARTAB_0;	// switch table pointer
	}	
	
	if(tbt_NeedChangeMap){ //要換表了
		
		if((FAST_BMTPage_Table[gc_PreWhichGroup]==0)||BMT_KEY[gw_NowBMTPoint]){ //把表寫回Flash
			if(!tbt_Disable_Update_Table){//預防第一次使用時誤認為之前有建過表
				NAND_ZONE_To_BMT_DBP(gc_PreWhichGroup);			
				FDBP.cFDev = BMT.cFDev;			
				FDBP.wFBlock= FAST_BMTTable[gc_PreWhichGroup];	
				tw_FastPage = FAST_BMTPage_Table[gc_PreWhichGroup];				
				if(tw_FastPage==H_INFO.wHBLOCKPAGE){
					Flash_EraseOneBlock_NO_ChkRdy();
//					dbprintf("\n Erase FAST_Blcok:%x ,Zone:%bx",FDBP.wFBlock,gc_PreWhichGroup);
					FAST_BMTPage_Table[gc_PreWhichGroup]=0x00;
					Wear_Table[FCNT]=Wear_Table[FCNT]+1;
					FDBP.wFBlock =Mapping_Table[SBK];// 將fast maping block 與 Swap 交換
					Mapping_Table[SBK] = FAST_BMTTable[gc_PreWhichGroup];
					FAST_BMTTable[gc_PreWhichGroup]=FDBP.wFBlock;
					tc_i=Wear_Table[FCNT];    // 將 erase cnt 交換
					Wear_Table[FCNT] =Wear_Table[SCNT];
					Wear_Table[SCNT] =tc_i;				
					APort_SBlock[0] = Mapping_Table[SBK];
					APort_SCNT[0] = Wear_Table[SCNT];			
				} 
				tw_FastPage=FAST_BMTPage_Table[gc_PreWhichGroup];
				FDBP.cFPlane= tw_FastPage&(NAND_INFO.cPLANE_NUM-1);
				FDBP.wFBlock= FAST_BMTTable[gc_PreWhichGroup];
				FDBP.wFPage = tw_FastPage/NAND_INFO.cPLANE_NUM;
				FDBP.cFInAddr = 0;
				Device_Write_SRAM_Index.WD = MAPRAM_PTR[gw_NowBMTPoint];
				ZSpare[0]=0xAA;	ZSpare[1] = gc_PreWhichGroup;
				ZSpare[2]=0xFF; ZSpare[3] = Wear_Table[FCNT];			
				ZSpare[4]=0xFF;	ZSpare[5] = 0xFF;
				ZSpare[6]=0xFF; ZSpare[7] = 0xFF;			
				CHK_Flash_Ready();
				Flash_Sequent_Write(2,1);//1k with tage
				FAST_BMTPage_Table[gc_PreWhichGroup]=tw_FastPage+1;
			}
		}   
		BMT_KEY[gw_NowBMTPoint] = 0; // disable BMT updatw Key
		if(FAST_BMTPage_Table[gw_WhichGroup]!=0x00){ //有把mapping寫在flash過
			NAND_ZONE_To_BMT_DBP(gw_WhichGroup);
			FDBP.cFDev = BMT.cFDev;			
			Device_Read_SRAM_Index.WD = MAPRAM_PTR[gw_NowBMTPoint];
			tw_FastPage=FAST_BMTPage_Table[gw_WhichGroup]-1;
			FDBP.cFPlane= tw_FastPage&(NAND_INFO.cPLANE_NUM-1);
			FDBP.wFBlock= FAST_BMTTable[gw_WhichGroup];	
			FDBP.wFPage = tw_FastPage/NAND_INFO.cPLANE_NUM;
			FDBP.cFInAddr = 0;			
			if(Flash_Sequent_Read(2)){//081024 Jay 萬一ECC Fail 的情況-->再次重建表
//				dbprintf("\n Read Fast BMT Fail Zone:%x ,BLK%x", gw_WhichGroup,FDBP.wFBlock);
				Flash_EraseOneBlock_NO_ChkRdy();
				FAST_BMTPage_Table[gw_WhichGroup]=0x00;
				Wear_Table[FCNT]=Wear_Table[FCNT]+1;
				FDBP.wFBlock =Mapping_Table[SBK];// 將fast maping block 與 Swap 交換
				Mapping_Table[SBK] = FAST_BMTTable[gw_WhichGroup];
				FAST_BMTTable[gw_WhichGroup]=FDBP.wFBlock;
				tc_i=Wear_Table[FCNT];    // 將 erase cnt 交換
				Wear_Table[FCNT] =Wear_Table[SCNT];
				Wear_Table[SCNT] =tc_i;	
				APort_SBlock[0] = Mapping_Table[SBK];
				APort_SCNT[0] = Wear_Table[SCNT]; 
				BMT_KEY[gw_NowBMTPoint]= 0x01; //下次需重新更新mapping table
				goto Re_Build_Mapping_Table;
			}
		
		}
		else{
Re_Build_Mapping_Table:       //081024 Jay  for mapping table fail
			tc_Old_CLK_Mode=gc_clock_mode;
			set_clock_mode(CLOCK_MODE_USB);						
			NAND_ZONE_To_BMT_DBP(gw_WhichGroup);		
			BMT_KEY[gw_NowBMTPoint]=1;
			Create_Zone_Lookup_Table();			
			set_clock_mode(tc_Old_CLK_Mode);		
		}
	}
	MAP_Table[gw_NowBMTPoint] = gw_WhichGroup;//同步Ram中的紀錄
	
}           
/***************************************************************************
* NAME:         void NAND_ZONE_To_BMT_DBP(U16 tw_ZoneNum)
* DESCRIPTION:
*          // SPDA26xx flash  plan
***************************************************************************/
void NAND_ZONE_To_BMT_DBP(U16 tw_ZoneNum){
	#ifdef HBLOCK_LINEAR_MODE
		BMT.cFDev = tw_ZoneNum/SYS_ZONE.cDEV_ZONE_NUM;
		BMT.cFPlane =0; 
		BMT.wFBlock = BMTTable[BMT.cFDev];
		BMT.wFPage = tw_ZoneNum%SYS_ZONE.cDEV_ZONE_NUM;
		BMT.cFInAddr =0;

	#else 
		BMT.cFDev = tw_ZoneNum%NAND_INFO.cDEVICE_NUM;
		BMT.cFPlane =0; 
		BMT.wFBlock = BMTTable[BMT.cFDev];
		BMT.wFPage = tw_ZoneNum/NAND_INFO.cDEVICE_NUM;
		BMT.cFInAddr =0;
	#endif  
}

/*********************************************************************************
* NAME:              void CHK_Data_Transfer_Done(void)
* DESCRIPTION:
*********************************************************************************/
void CHK_Data_Transfer_Done(void) {
	while (!(DMA_REG[0xC0] & 0x01));							//Wait Data xfer done
	DMA_REG[0xC0] = 0x00;										//Clear DMA Xfer done interrupt
}



/*********************************************************************************
* NAME:                      void CHK_Flash_Ready(void)
* DESCRIPTION:
*********************************************************************************/
void CHK_Flash_Ready(void) {

	while (!(DEVICE_REG[0x24] & 0x01));
}



/*********************************************************************************
* NAME:              void CHK_NandAutocmp_Done(void)
* DESCRIPTION:
*********************************************************************************/
void CHK_NandAutocmp_Done(void) {
	while(!(FLASH_REG[0x0F]&0x01));								// Wait Nand Automode Done
	FLASH_REG[0x0F]=0x00;										// Clear Nand Atyomode done interrupt
}


//! Read a reserve data(DSP or HZK) from flash
/*!
 *	\param	Area		0:DSP   1: HZK
 *	\param	Offset_Addr	Read sector offset address
 *	\param	SectorCnt	Read sector number
 *	\param	Buf_Index	Target Data Buf Address
 */
void DSPHZK_Block_Read(U8 Area,U16 Offset_Addr,U8 SectorCnt,U8 Buf_Index)
{
	data	U8  tc_InAddr;
	data	U16	tw_HPage_WD;
	xdata	U16 tw_TargetInx;
	xdata	U16	tw_SourceInx;
	xdata	U16 tw_BlockSize;
	xdata	U8	tc_sts=0;
	
    tw_BlockSize = (gw_PagesPerBlock*gc_InAddrMax);
	gc_LCD_Gobal_J=FDBP.cFDev;
	FDBP.cFDev=0x00;     // CE0
   	FDBP.cFPlane = 0;	
    tw_TargetInx = (U16)Buf_Index<<8;
    while(SectorCnt){
		Device_Read_SRAM_Index.BY[0] = gc_TempFlashBufHB;//0x90
		Device_Read_SRAM_Index.BY[1] = 0x00;
		FDBP.cFDev=0x00;
		tw_HPage_WD = Offset_Addr%tw_BlockSize ;//090820  Jay add
    	gc_Flash_Data = Offset_Addr/tw_BlockSize; 
    	FDBP.cFPlane = 0;
    	if(Area==1){
    		FDBP.wFBlock = HZK_Block_A[gc_Flash_Data]; 
		}
		else{
    		FDBP.wFBlock = DSP_Block_B[gc_Flash_Data]; 		
		}
   		FDBP.wFPage = (tw_HPage_WD/gc_InAddrMax);  
   		FDBP.wFPage = (FDBP.wFPage%gw_PagesPerBlock);
    	tc_InAddr =   (tw_HPage_WD%gc_InAddrMax);    
    	FDBP.cFInAddr = (tc_InAddr&0xFE);
		tc_sts=Flash_ReadPage();	
		if(tc_InAddr&0x01){
			tw_SourceInx=(Device_Read_SRAM_Index.WD+0x200);
			Move_RAM(tw_SourceInx,tw_TargetInx,511);
		}
		else{
			tw_SourceInx=(Device_Read_SRAM_Index.WD+0);
			Move_RAM(tw_SourceInx,tw_TargetInx,511);
		}
		Offset_Addr++;
		tw_TargetInx+=0x200;
		SectorCnt--;
	}

	if(tc_sts && (!Area))//read data from backup block(only DSP)
	{
		FDBP.cFDev=0x00;     // CE0
   		FDBP.cFPlane = 0;	
    	tw_TargetInx = (U16)Buf_Index<<8;
    	while(SectorCnt){
			Device_Read_SRAM_Index.BY[0] = gc_TempFlashBufHB;//0x90
			Device_Read_SRAM_Index.BY[1] = 0x00;
			FDBP.cFDev=0x00;
			tw_HPage_WD = Offset_Addr%tw_BlockSize ;//090820  Jay add
    		gc_Flash_Data = Offset_Addr/tw_BlockSize; 
    		FDBP.cFPlane = 0;
    		FDBP.wFBlock = DSP_Block_A[gc_Flash_Data]; 		
			FDBP.wFPage = (tw_HPage_WD/gc_InAddrMax);  
   			FDBP.wFPage = (FDBP.wFPage%gw_PagesPerBlock);
    		tc_InAddr =   (tw_HPage_WD%gc_InAddrMax);    
    		FDBP.cFInAddr = (tc_InAddr&0xFE);
			tc_sts=Flash_ReadPage();	
			if(tc_InAddr&0x01){
				tw_SourceInx=(Device_Read_SRAM_Index.WD+0x200);
				Move_RAM(tw_SourceInx,tw_TargetInx,511);
			}
			else{
				tw_SourceInx=(Device_Read_SRAM_Index.WD+0);
				Move_RAM(tw_SourceInx,tw_TargetInx,511);
			}
			Offset_Addr++;
			tw_TargetInx+=0x200;
			SectorCnt--;
		}
	}
	FDBP.cFDev=gc_LCD_Gobal_J;
}

/*********************************************************************************
* NAME:                      bit Flash_ChkStatus(void)
* DESCRIPTION:
*********************************************************************************/
bit Flash_ChkStatus(void) {
	gc_FlashStatus     = 0x00;
	CHK_Flash_Ready();
	FLASH_REG [0x00] = 0x00;		//強迫進IO Mode     
	DEVICE_REG[0x23] = 0x0A;		//CLE = '1', ALE = '0', nWP = '1', nCE = '0'
	DEVICE_REG[0x20] = 0x70;
	DEVICE_REG[0x23] = 0x02;		//CLE = '0', ALE = '0', nWP = '1', nCE = '0'
	gc_FlashStatus = DEVICE_REG[0x20];
	DEVICE_REG[0x23] = 0x03;		//CLE = '0', ALE = '0', nWP = '1', nCE = '1'
	FLASH_REG [0x00] = 0x01;		//進auto Mode     
	if (gc_FlashStatus & 0x01) {
		return 1;
	}
	else {
		return 0;
	}
}

/*********************************************************************************
* NAME:                   bit Flash_EraseOneBlock(void)
* DESCRIPTION:
*********************************************************************************/
U8 Flash_EraseOneBlock(void) {	
	CHK_Flash_Ready();
	FLASH_REG[0x06]  = FERASE_CMD1;									// cmd1
	FLASH_REG[0x07]  = FERASE_CMD2;									// cmd2	
	FLASH_REG[0x04]  = 0x05;
	NAND_FBlock_FPage_FBank_To_PhyAddr_NAND();
	FLASH_REG[0x1D] = 0x02;									// skip FA0 FA1
	CHK_Flash_Ready();
	TRIGGER_FLASH_CMD();
	CHK_NandAutocmp_Done();
	CHK_Flash_Ready();  
	FLASH_REG[0x1D] = 0x00; 
	if (Flash_ChkStatus()& 0x01) {
		return 1;
	}
	else {
		return 0;
	}
}

/*********************************************************************************
* NAME:                   bit Flash_EraseOneBlock(void)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/
void Flash_EraseOneBlock_NO_ChkRdy(void) {	
	
	FLASH_REG[0x06]  = FERASE_CMD1;									// cmd1
	FLASH_REG[0x07]  = FERASE_CMD2;									// cmd2	
	FLASH_REG[0x04]  = 0x05;
	NAND_FBlock_FPage_FBank_To_PhyAddr_NAND();
	FLASH_REG[0x1D] = 0x02;									// skip FA0 FA1
	CHK_Flash_Ready();
	TRIGGER_FLASH_CMD();
	CHK_NandAutocmp_Done();
	FLASH_REG[0x1D] = 0x00; 
	
}
/*********************************************************************************
* NAME:          void Flash_MultiBank_Erase(void)
* DESCRIPTION:
*********************************************************************************/
U8 Flash_MultiBank_Erase(U8 tbt_CHK_Rdy)
{
	FDBP.wFPage =0;			
	FDBP.cFPlane =0;		
	Flash_EraseOneBlock_NO_ChkRdy();
	if(tbt_CHK_Rdy)
	{
		CHK_Flash_Ready();  
		CHK_NandAutocmp_Done();
		FLASH_REG[0x1D] = 0x00; 
		if (Flash_ChkStatus()& 0x01) 
		{
			return 1;
		}
		else 
		{
			return 0;
		}
	}	
	else
	{
		return 0;
	}
}


/*********************************************************************************
* NAME:                   void Flash_ReadSpare(U8 cSbyte)  
* DESCRIPTION:// SPDA26xx flash  plan  4byte or 8 byte
*********************************************************************************/
void Flash_ReadSpare(U8 cSbyte) {
    xdata	U8	i;
    data	U8	j;
    data	U8	tc_EndInAddr; 
    Device_Xfer_DataLength.WD = 0x03FF;
	SET_DEVICE_READ();
	ENA_ECC_Decode();
	FLASH_REG[0x06]  = 0x00;                                   // cmd1
	FLASH_REG[0x07]  = 0x30;                                   // cmd2	
	NAND_InAddr_To_Column_NAND();                              // set Column
	NAND_FBlock_FPage_FBank_To_PhyAddr_NAND();                 // set FBank Fblock Fpage
	FLASH_REG[0x1A]	= 0x10;                                    // last frag = 1
	FLASH_REG[0x04] = 0x01;	                                   // CMD0+ADDR+CMD1
	CHK_Flash_Ready();
	TRIGGER_FLASH_CMD();                                       //               
	CHK_Flash_Ready();
	tc_EndInAddr=cSbyte>>2;
	for(j=0;j<tc_EndInAddr;j++){
		if(j==(tc_EndInAddr-1)){
			FLASH_REG[0x1A]	= 0x11; 
		}		
		else{
			FLASH_REG[0x1A]	= 0x10;
		} 
		FLASH_REG[0xA1] = 0x01;
		Trigger_Data_Transfer_Done();
		CHK_NandAutocmp_Done();
		CHK_ECC_Done();	
		for(i=0;i<4;i++){
       		ZSpare[(j*4)+i] = FLASH_REG[0x60+i];  //每1k有4 byte 的spare byte
        }
		
	}	
}

/*********************************************************************************
* NAME:                    bit Flash_Sequent_Read(UBYTE Secount)
* DESCRIPTION:
*   If ECC is Uncorrectable, return TRUE; else return FALSE. 
*********************************************************************************/	
bit Flash_Sequent_Read(UBYTE Secount){
	bit tbt_ECC_Fail=0;
	Device_Xfer_DataLength.WD = 0x03FF;   
	SET_DEVICE_READ();
	ENA_ECC_Decode();
	FLASH_REG[0x06]  = 0x00;									// cmd1
	FLASH_REG[0x07]  = 0x30;									// cmd2	
	NAND_InAddr_To_Column_NAND();								// set Column
	NAND_FBlock_FPage_FBank_To_PhyAddr_NAND();					// set FBank Fblock FPage	
	FLASH_REG[0x04] = 0x01;										// CMD0+ADDR+CMD1
	CHK_Flash_Ready();
	TRIGGER_FLASH_CMD();               
	CHK_Flash_Ready();
	Secount-=2;
	while(Secount){
		FLASH_REG[0xA1] = 0x01;									// last flag = 0
		FLASH_REG[0xA2] &= 0xF3;
		FLASH_REG[0x1A] = 0x10;
		Trigger_Data_Transfer_Done();    
		if(CHK_ECC_Done()){
			tbt_ECC_Fail=1;
		}
		CHK_NandAutocmp_Done();
		Device_Read_SRAM_Index.BY[0] += 0x04;  
		Device_Read_SRAM_Index.BY[1] =  0x00;
		SET_DEVICE_READ();		
		Secount-=2;
	}
	FLASH_REG[0xA1] = 0x01;
	FLASH_REG[0xA2] &= 0xF3;
	FLASH_REG[0x1A] = 0x11;										// last flag = 1
	Trigger_Data_Transfer_Done();    
	if(CHK_ECC_Done()){
		tbt_ECC_Fail=1;
	}
	CHK_NandAutocmp_Done();
	FLASH_REG[0xA1] = 0x00;	
	return tbt_ECC_Fail;
	
	
	
}
/*********************************************************************************
* NAME:               void Flash_Sequent_Write(UBYTE Secount,bit spare )
* DESCRIPTION://以page為單位
*********************************************************************************/
void Flash_Sequent_Write(UBYTE Secount,bit spare ) {	
	data	U8 i;	
	Device_Xfer_DataLength.WD = 0x03FF;	
	DMA_REG[0xB1] = 0x01 ;										// Enable DMA Auto address account
	SET_DEVICE_WRITE();
	ENA_ECC_Encode();
	FLASH_REG[0x06]  = 0x80;									// cmd1
	NAND_InAddr_To_Column_NAND();								// set Column
	NAND_FBlock_FPage_FBank_To_PhyAddr_NAND();					// set FBank Fblock FPage
	CHK_Flash_Ready();
	FLASH_REG[0x04] = 0x0F;										// Radom write phase
	TRIGGER_FLASH_CMD();
	Secount-=2 ;
	if(spare){
		FLASH_REG[0x60]=ZSpare[0];
		FLASH_REG[0x61]=ZSpare[1];
		FLASH_REG[0x62]=ZSpare[2];
		FLASH_REG[0x63]=ZSpare[3];   		
	}
	while(Secount){             
		if(spare){
			for(i=0;i<4; i++){
	        	FLASH_REG[0x60+i]=ZSpare[(FDBP.cFInAddr&0x03)*2+i];
	    	} 
	      
		}				
		FLASH_REG[0x1A]	= 0x10;										// last flag = 1
		FLASH_REG[0xA0] = 0x01;									//Start Encode RS-Code                
		Trigger_Data_Transfer_Done();
		while (FLASH_REG[0xA2] & 0x40);							// check Ecc done
   	    CHK_NandAutocmp_Done();
		Secount-=2;
		FDBP.cFInAddr+=2;
	}
	if(spare){
		for(i=0;i<4; i++){
	       	FLASH_REG[0x60+i]=ZSpare[(FDBP.cFInAddr&0x03)*2+i];
	   	} 
	      
	}
	FLASH_REG[0x1A]	= 0x11;										// last flag = 1
	FLASH_REG[0xA0] = 0x01;										//Start Encode RS-Code
	Trigger_Data_Transfer_Done();
	while (FLASH_REG[0xA2] & 0x40);
	CHK_NandAutocmp_Done();
	FLASH_REG[0x06]  = 0x10;									// CMD1
	FLASH_REG[0x04] = 0x08;										// CMD phase
	TRIGGER_FLASH_CMD();   	
	FLASH_REG[0xA0]  = 0x00; 
	DMA_REG[0xB1] = 0x00 ;										// Disable DMA Auto address account
}


/******************************************************* 
* NAME:            void Flash_State_Initial(void)
* DESCRIPTION:// SPDA26xx flash  plan
********************************************************/
void Flash_State_Initial(void){
	xdata	U8 tc_FMAP;
	DEVICE_REG[0x00] = 0x01;				// 0x00=none ; 0x01= Flash ; 0x03 = CF; 0x04 =SPI ; 0x06 = SD ; 0x07 = MS
    //DEVICE_REG[0x21]    = 0x12;				// [3..0]Low flash strobe; [7..4] High flash strobe     
	FLASH_REG[0x00]  = 0x01;				// Enable Flash auto mode
	FLASH_REG[0x02]  = 0x52;				// Flash signal strobe counter[3..0]; Check flash ready delay[7..4] 
	FLASH_REG[0xA3]  = 0x01;				// Enable ECC
	
	if(!NAND_INFO.cECC_MODE){    	//0-->24 bit 1-->16bit
	    FLASH_REG[0xA5] = 0x0A;     //24 bit ECC    
	}
	else{
	    FLASH_REG[0xA5] = 0x08;     // 16 bit ECC	    
	}	
    if(NAND_INFO.cFMAP&0xF0){// 
        FLASH_REG[0x0D] =0xF0;  // disable auot Fmap
    }
    else{      
        tc_FMAP = NAND_INFO.cFMAP&0x0F;
        FLASH_REG[0x0D]  = (tc_FMAP<<4)|0x03;              // Enable Fmap
    }
    DEVICE_REG[0x21]    = gc_RW_Strobe;
    FLASH_REG[0x00] =0x01;

}

/*********************************************************************************
* NAME:                      void FlashReset(void)
* DESCRIPTION:
*********************************************************************************/
void FlashReset(void) {
    CHK_Flash_Ready();
	FLASH_REG[0x06] = 0xFF;
	FLASH_REG[0x04] = 0x08;
	FLASH_REG[0x03] = 0x01;
	CHK_NandAutocmp_Done();	
	CHK_Flash_Ready();
	DEVICE_REG[0x23] = 0x03;	//CLE = '0', ALE = '0', nWP = '1', nCE = '1'
}

/*********************************************************************************
* NAME: void LBA2DBP (void)
* DESCRIPTION:
*       LBA -> gw_CylTemp, HPage(轉換的過程與Flash本身的gc_BanksPerDevice, gc_PagesPerBlock有關)
*    
*     
*********************************************************************************/
void LBA2DBP(void) {

	if(gbt_enableCDROM && (gc_bCBWLUN==2)){//09/04/24,joyce for CD-ROM
		LBA.LW=LBA.LW+Capacity_Remove.LW; 
	}

	HBlock.WD  = LBA.LW/H_INFO.wHBLOCKSECTOR;
	HPage.WD   = LBA.LW%H_INFO.wHBLOCKSECTOR;
	gc_HInAddr = HPage.WD&(NAND_INFO.cINADDRMAX-1);	
	HPage.WD   = HPage.WD/NAND_INFO.cINADDRMAX;			
	LBlock.WD  = HBlock.WD;// for swap mode	
}

/*********************************************************************************
* NAME:           void NAND_FBlock_FPage_FBank_To_PhyAddr_NAND(void)
* DESCRIPTION:
*********************************************************************************/
void NAND_FBlock_FPage_FBank_To_PhyAddr_NAND(void) {
	unionWORD tw_FBlock;
    xdata	U8 tc_FMAP;
	DEVICE_REG[0x0F] =FDBP.cFDev;   
	if(NAND_INFO.cFMAP&0xF0){
    	tc_FMAP = (NAND_INFO.cFMAP&0xF0)>>4;
    	switch(tc_FMAP){
    		case 1:    	// 256 page  block 
        		//tw_FBlock.WD = FDBP.wFBlock+FDBP.cFPlane;// support 2 plane mode       		
        		tw_FBlock.WD = FDBP.wFBlock;//only 1 plane 
        		FLASH_REG[0x0A]= FDBP.wFPage;  
        		FLASH_REG[0x09]= tw_FBlock.BY[1];
        		FLASH_REG[0x08]= tw_FBlock.BY[0];        		
				break;
			default: 
				break;
		}	              
    }
    else{
    	//tw_FBlock.WD = FDBP.wFBlock+FDBP.cFPlane;
    	tw_FBlock.WD = FDBP.wFBlock;//only 1 plane
    	FLASH_REG[0x0A] = FDBP.wFPage;                                             // FPage
	    FLASH_REG[0x09] = tw_FBlock.BY[1];                                      // FBlock_L 
	    FLASH_REG[0x08] = tw_FBlock.BY[0];                                      // FBlock_H
		FLASH_REG[0x1C] = 0;                                             // FBank  
	}
	if(NAND_INFO.cADDR_CYCLE==0x04){	// 4 cycle addr
		FLASH_REG[0x01] = 0x30|0x01;
	}
	else{
		FLASH_REG[0x01] = (0x40|0x01); 	// 5 cycle addr
	}	  
}

/*********************************************************************************
* NAME:           void NAND_InAddr_To_Column_NAND(void)
* DESCRIPTION:
*********************************************************************************/
void NAND_InAddr_To_Column_NAND(void) {
	 unionWORD tw_FColAddr;
    if((FLASH_REG[0xA5]&0x03)==0x02){ //24bit
        tw_FColAddr.WD =  (U16)(FDBP.cFInAddr>>1)*1070;//(1024+46)        
    }
    else{                             //16bit
       tw_FColAddr.WD =  (U16)(FDBP.cFInAddr>>1)*1056;//(1024+32)
    }   
	FLASH_REG[0x0C] = tw_FColAddr.BY[1];
	FLASH_REG[0x0B] = tw_FColAddr.BY[0]; 
}


/*********************************************************************************
* NAME:                     void SET_DEVICE_READ(void)
* DESCRIPTION:
* Global Variable: Device_Xfer_DataLength.WD, Device_Read_SRAM_Index.WD
*********************************************************************************/
void SET_DEVICE_READ(void) {
	DMA_REG[0x01] = DEVICE2SRAM;								//FLASH to SRAM
	DMA_REG[0x03] = Device_Xfer_DataLength.BY[0];				//傳送長度.
	DMA_REG[0x02] = Device_Xfer_DataLength.BY[1];
	CPU_REG[0x15] = Device_Read_SRAM_Index.BY[0];				//SRAM Index
	CPU_REG[0x14] = Device_Read_SRAM_Index.BY[1];
}

/*********************************************************************************
* NAME:                     void SET_DEVICE_READ_TO_DRAM(void)
* DESCRIPTION:
* Global Variable: Device_Xfer_DataLength.WD, Device_Read_SRAM_Index.WD
*********************************************************************************/
void SET_DEVICE_READ_TO_DRAM(void){
	DMA_REG[0x01] = DEVICE2DRAM;						        //FLASH to DERAM
	DMA_REG[0x03] = Device_Xfer_DataLength.BY[0];				//傳送長度.
	DMA_REG[0x02] = Device_Xfer_DataLength.BY[1];               //
	CPU_REG[0x19] = Device_Read_SRAM_Index.BY[0];               //DRAM Index
	CPU_REG[0x18] = Device_Read_SRAM_Index.BY[1];               //	
		
}

/*********************************************************************************
* NAME:                     void SET_DEVICE_WRITE(void)
* DESCRIPTION:
* Global Variable: Device_Xfer_DataLength.WD, Device_Write_SRAM_Index.WD
*********************************************************************************/
void SET_DEVICE_WRITE(void) {
	DMA_REG[0x01] = SRAM2DEVICE;								//SRAM to FLASH
	DMA_REG[0x03] = Device_Xfer_DataLength.BY[0];				//傳送長度.
	DMA_REG[0x02] = Device_Xfer_DataLength.BY[1];
	CPU_REG[0x13] = Device_Write_SRAM_Index.BY[0];				//SRAM Index
	CPU_REG[0x12] = Device_Write_SRAM_Index.BY[1];
}  

/*********************************************************************************
* NAME:                   void SetHBlockToHDev(void)
* DESCRIPTION:

*********************************************************************************/
void SetHBlockToHDev(void) {
	#ifdef HBLOCK_LINEAR_MODE //HBlock  linear mode
	gw_WhichGroup=HBlock.WD/SYS_ZONE.wZONE_SIZE;
	HBlock.WD = HBlock.WD%SYS_ZONE.wZONE_SIZE;
	gc_HDev=gw_WhichGroup/SYS_ZONE.cDEV_ZONE_NUM;
	if( gc_HDev >= NAND_INFO.cDEVICE_NUM) {
	    gc_ErrorCode = ID_NOT_FND;
	}
	#else  // HBlock swap mode
	U16 tw_Big_HBlock =0;
	U8  tc_Small_HBlock;
	U8  tc_Big_HPage;
	
	tw_Big_HBlock = LBlock.WD/SWAP_HBLOCK_SIZE;//   先轉換成基本swap mode 單位
	tc_Small_HBlock = LBlock.WD%SWAP_HBLOCK_SIZE;
	tc_Big_HPage = tw_Big_HBlock/SYS_ZONE.cTOTALZONE_NUM;
	
	gw_WhichGroup=tw_Big_HBlock%SYS_ZONE.cTOTALZONE_NUM;
	HBlock.WD= (tc_Big_HPage*SWAP_HBLOCK_SIZE)+tc_Small_HBlock;
	gc_HDev=gw_WhichGroup%NAND_INFO.cDEVICE_NUM;
	if( HBlock.WD >= SYS_ZONE.wZONE_SIZE) {
	    gc_ErrorCode = ID_NOT_FND;
	}
	#endif 
	
	
	
}

/*********************************************************************************
* NAME:                void SetHBlock_To_WhichGroup(void)
* DESCRIPTION:
*********************************************************************************/
void SetHBlock_To_WhichGroup(void) {
	#ifdef HBLOCK_LINEAR_MODE //Hblock Linear mode 
	
	if(HBlock.WD == SYS_ZONE.wZONE_SIZE) {	//必須更換Zone
		HBlock.WD = 0;			//重新歸零
	    gw_WhichGroup++;			//下一個Zone
	    gc_HDev=gw_WhichGroup/SYS_ZONE.cDEV_ZONE_NUM;
		if( gc_HDev >= NAND_INFO.cDEVICE_NUM) {
	    	gc_ErrorCode = ID_NOT_FND;
		}
	}
	#else
	 //HBlock Swap mode
	U16 tw_Big_HBlock =0;
	U8  tc_Small_HBlock;
	U8  tc_Big_HPage;
	
	tw_Big_HBlock = LBlock.WD/SWAP_HBLOCK_SIZE;//   先轉換成基本swap mode 單位
	tc_Small_HBlock = LBlock.WD%SWAP_HBLOCK_SIZE;
	tc_Big_HPage = tw_Big_HBlock/SYS_ZONE.cTOTALZONE_NUM;
	
	gw_WhichGroup=tw_Big_HBlock%SYS_ZONE.cTOTALZONE_NUM;
	HBlock.WD= (tc_Big_HPage*SWAP_HBLOCK_SIZE)+tc_Small_HBlock;
	gc_HDev=gw_WhichGroup%NAND_INFO.cDEVICE_NUM;
	if( HBlock.WD >= SYS_ZONE.wZONE_SIZE) {
	    gc_ErrorCode = ID_NOT_FND;
	}
	#endif

}


/*********************************************************************************
* NAME:                 void Trigger_Data_Transfer(void)
* DESCRIPTION:
*********************************************************************************/
void Trigger_Data_Transfer(void) {
	DMA_REG[0xC0] = 0x00;										//Clear DMA Xfer done interrupt
	DMA_REG[0xB0] = 0x01;										//Trigger Data Xfer
}


/*********************************************************************************
* NAME:                  void TRIGGER_FLASH_CMD(void)
* DESCRIPTION:
* Global Variable: gc_Flash_CMD2
*********************************************************************************/
void TRIGGER_FLASH_CMD(void) {
	FLASH_REG[0x03] = 0x01;	
	while(FLASH_REG[0x03]);
}


/*********************************************************************************
* NAME:            void TRIGGER_FLASH_CMD_COL_ADDR_CMD(void)
* DESCRIPTION:
* Global Variable: gc_Flash_CMD1, FCOL_ADDR.WD, gc_Flash_CMD2
*********************************************************************************/
void TRIGGER_FLASH_CMD_COL_ADDR_CMD(void) {
	FLASH_REG[0x06]  = gc_Flash_CMD1;							// cmd1
	FLASH_REG[0x07]  = gc_Flash_CMD2;							// cmd2	
	NAND_InAddr_To_Column_NAND();								// set Column
	NAND_FBlock_FPage_FBank_To_PhyAddr_NAND();					// set FBank Fblock FPage	
	FLASH_REG[0x04] = gc_Flash_CMD_Type;						// CMD0+ADDR+CMD1	
	CHK_Flash_Ready();
	FLASH_REG[0x03] = 0x01;						
}

//! erase UserData block
/*!
 *	\param	Block_index	index of userdata block
 */
U8 UserData_Block_Erase(U8 Block_index)
{
	if(Block_index >= User_Block_Num)
	{
		//dbprintf("over \n");
		return 1;
	}
	gc_LCD_Gobal_J=FDBP.cFDev;
	FDBP.cFDev=0x00;
   	FDBP.cFPlane =0;
   	FDBP.wFBlock = UserData_Block[Block_index];
	FDBP.wFPage =0;
	if(FDBP.wFBlock==0x00 || FDBP.wFBlock==0xffff)
	{
//		dbprintf("error user block \n");
		FDBP.cFDev=gc_LCD_Gobal_J;
		return 1;
	} 
	Flash_EraseOneBlock();

	FDBP.cFDev=gc_LCD_Gobal_J;
	return 0;
}

//! Read UserData data from flash
/*!
 *	\param	Block_index	index of userdata block
 *	\param	Sector_Addr	Read sector address
 *	\param	SectorCnt	Read sector number
 *	\param	Buf_Index	Target Data Buf Address
 */
U8 UserData_Block_Read(U8 Block_index,U16 Sector_Addr,U8 SectorCnt,U8 Buf_Index)
{
	data	U8	tc_sts=0;
	data	U8  tc_InAddr;
	xdata	U16 tw_TargetInx;
	data	U16	tw_SourceInx;
	data	U16	tw_HPage_WD;
	data	U16	tw_SectCnt = 0;
	if( Block_index >= User_Block_Num)
	{
		//dbprintf("over \n");
		return 1;
	}
	gc_LCD_Gobal_J=FDBP.cFDev;
	tw_TargetInx = (U16)Buf_Index<<8;
	while(SectorCnt)
	{
		if(Buf_Index>=0xAC)
		{
			Device_Read_SRAM_Index.BY[0] = gc_TempFlashBufHB;	// 0x90
		}
		else
		{
			Device_Read_SRAM_Index.BY[0] = 0xAC;
		}
		Device_Read_SRAM_Index.BY[1] = 0x00;
		FDBP.cFDev=0x00;
		tw_HPage_WD = Sector_Addr;
    	gc_Flash_Data = Sector_Addr/gw_DataBlockSize; 
    	FDBP.cFPlane = 0;
   		FDBP.wFBlock = UserData_Block[Block_index]; 
    	FDBP.wFPage = (tw_HPage_WD/gc_InAddrMax);  
   		FDBP.wFPage = (FDBP.wFPage%gw_PagesPerBlock);
    	tc_InAddr =   (tw_HPage_WD%gc_InAddrMax);    
    	FDBP.cFInAddr = (tc_InAddr&0xFE);
		tc_sts=Flash_ReadPage();	
		if(tc_InAddr&0x01){
			tw_SourceInx=(Device_Read_SRAM_Index.WD+0x200);
			Move_RAM(tw_SourceInx,tw_TargetInx,511);
		}
		else
		{
			tw_SourceInx=(Device_Read_SRAM_Index.WD+0);
			Move_RAM(tw_SourceInx,tw_TargetInx,511);
		}

		Sector_Addr++;
		tw_TargetInx+=0x200;
		SectorCnt--;
	}
	
	FDBP.cFDev=gc_LCD_Gobal_J;
	if(tc_sts)
	{
		//dbprintf("ECC \n");
		return 1;
	}
	else
	{
		return 0;
	}
}

//! Write UserData data from flash
/*!
 *	\param	Block_index	index of userdata block
 *	\param	Sector_Addr	write sector address
 *	\param	SectorCnt	write sector number
 *	\param	Buf_Index	Target Data Buf Address
 */
U8 UserData_Block_Write(U8 Block_index,U16 Sector_Addr,U8 SectorCnt,U8 Buf_Index)
{
	xdata	U8  tc_InAddr;
	data	U8 tc_XferCnt;
	data	U16 tw_HPage_WD;
	gc_LCD_Gobal_J=FDBP.cFDev;
	if(Block_index >= User_Block_Num)
	{
		//dbprintf("over \n");
		return 1;
	}
	if(SectorCnt&0x01){
		tc_XferCnt = SectorCnt+1;
	}
		
	else{
		tc_XferCnt =SectorCnt;
	}
	Device_Write_SRAM_Index.BY[0] = Buf_Index;
	Device_Write_SRAM_Index.BY[1] = 0x00;
	FDBP.cFDev=0x00;
	tw_HPage_WD = Sector_Addr;
    gc_Flash_Data = Sector_Addr/gw_DataBlockSize; 
    FDBP.cFPlane = 0;
   	FDBP.wFBlock = UserData_Block[Block_index]; 
    FDBP.wFPage = (tw_HPage_WD/gc_InAddrMax);  
   	FDBP.wFPage = (FDBP.wFPage%gw_PagesPerBlock);
    tc_InAddr = (tw_HPage_WD%gc_InAddrMax);
    FDBP.cFInAddr =(tc_InAddr&0xFE);
    Flash_Sequent_Write(tc_XferCnt,0);
	
	FDBP.cFDev=gc_LCD_Gobal_J;
	return 0;
}

/*********************************************************************************
* NAME:                      bit CHK_ECC_Done(void)
* DESCRIPTION:
*********************************************************************************/
bit CHK_ECC_Done(void) {
	 gc_ECC_Error_Bit =0;
    while (FLASH_REG[0xA2] & 0x01);                            // check Ecc done
	if (FLASH_REG[0xA2] & 0x04) {                              // Error Bit Occur, correct it
		if (FLASH_REG[0xA2] & 0x08) {                          // Un-Correctable Error
			gbt_ECC_Fail = 1;
			gc_ECC_Error_Bit = 25;               
		}
		else{
			gbt_ECC_Fail = 0;
			gc_ECC_Error_Bit = FLASH_REG[0xA7];
		}
				
		FLASH_REG[0xA2] &= 0xF3;  
	}
	else {
		gbt_ECC_Fail = 0;
		gc_ECC_Error_Bit =0;
	}
	return gbt_ECC_Fail;	
}    
    

    
/*********************************************************************************
* NAME:                    void  ENA_ECC_Decode(void)
* DESCRIPTION:*    // SPDA26xx flash  plan
*********************************************************************************/
void  ENA_ECC_Decode(void){
	DEVICE_REG[0xA0] = 0x01;                                   // Reset ECC  
	FLASH_REG[0xA3]  = 0x01;                                   // Enable ECC 
	FLASH_REG[0xA0]  = 0x00;	                               // Disable Encode RS-Code
	FLASH_REG[0xA1]  = 0x01;                                   // Start Decode RS-Code
	FLASH_REG[0xA2] &= 0xF3;                                   // Clear ECC fail register
}
/*********************************************************************************
* NAME:                    void  ENA_ECC_Encode(void)
* DESCRIPTION:* // SPDA26xx flash  plan    
/*********************************************************************************/
void  ENA_ECC_Encode(void){
	DEVICE_REG[0xA0] = 0x01;                                   // Reset ECC  
	FLASH_REG[0xA3]  = 0x01;                                   // Enable ECC 
	FLASH_REG[0xA0]  = 0x01;	                               // Disable Encode RS-Code
	FLASH_REG[0xA1]  = 0x00;                                   // Start Decode RS-Code
	FLASH_REG[0xA2] &= 0xF3;                                   // Clear ECC fail register
}
    
/*********************************************************************************
* NAME:                   void LoadBlock_From_MAP(U8 tc_ptr)
* DESCRIPTION:
*********************************************************************************/
void LoadBlock_From_MAP(U8 tc_ptr){
	APort_LBlock[tc_ptr] = Mapping_Table[HBlock.WD];
	APort_SBlock[tc_ptr] = Mapping_Table[SBK];	
	APort_RBlock[tc_ptr] = Mapping_Table[RBK];
    APort_LCNT[tc_ptr]=Wear_Table[HBlock.WD];
    APort_RCNT[tc_ptr]=Wear_Table[RCNT];         
    APort_SCNT[tc_ptr]=Wear_Table[SCNT];
}
    
/*********************************************************************************
* NAME:        void Load_E_D_Block_From_MAP(U8 tc_ptr,U8 tc_Inx);
* DESCRIPTION:
*********************************************************************************/
void Load_E_D_Block_From_MAP(U8 tc_ptr,U8 tc_Inx){
    
 	APort_EBlock[tc_ptr] = Mapping_Table[EBK+tc_Inx];
	APort_DBlock[tc_ptr] = Mapping_Table[DBK+tc_Inx];

	APort_ECNT[tc_ptr]=Wear_Table[ECNT+tc_Inx];
    APort_DCNT[tc_ptr]=Wear_Table[DCNT+tc_Inx];

}
/*********************************************************************************
* NAME:    bit CHK_DSBlock_Exist_Or_New(U8 tc_Which_D,U8 tc_ExtLBLK, bit tbt_Need_New)
* DESCRIPTION:
*********************************************************************************/
U8 CHK_DSBlock_Exist_Or_New(U8 tc_Which_D,U8 tc_ExtLBLK, bit tbt_Need_New){
	data	bit	tbt_FindPage=0;
	xdata	U8	tc_i;
	xdata	U8	tc_Offset;
	tc_Offset= (tc_Which_D* MAX_DUMMY_PAGE); 
	for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
		QTable.WD=Mapping_Table[D_HPAGE+tc_Offset+tc_i];
		if(tc_ExtLBLK==QTable.BY[0]){
			SQTable.WD = Mapping_Table[D_PAGE+tc_Offset+tc_i];			
			gc_D_SBlock = QTable.BY[1];
			gc_NowIndex =tc_i;
			tbt_FindPage=1;
			break;
		}
	}
	if(tbt_FindPage){
		return 0;
	}
	else if(tbt_Need_New){		
		tbt_FindPage=0;
		for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
			if(Mapping_Table[D_HPAGE+tc_Offset+tc_i]==0xFFFF){
				Mapping_Table[D_PAGE+tc_Offset+tc_i]=0x0000;
				gc_NowIndex =tc_i;
				tbt_FindPage=1;
				SQTable.WD= 0x0000;
				break;
			}
		}
		if(tbt_FindPage){
			return 0;
		}
		else{
			return 1;
		}
		
	}
	else{
		return 1;
	}
	
}
/*********************************************************************************
* NAME:               void NAND_HPage_To_SBlock_SPage(void)
* DESCRIPTION:
*********************************************************************************/
void NAND_HPage_To_SBlock_SPage(void){
	gc_SBlock= HPage.WD/H_INFO.cSMALLPAGE_NUM;
	gc_SPage = HPage.WD%H_INFO.cSMALLPAGE_NUM;
}    
/*********************************************************************************
* NAME:               bit CHK_RandomPage_Exist(U8 tc_Inx);
* DESCRIPTION:
*********************************************************************************/
U8 CHK_RandomPage_Exist(U8 tc_Inx,U16 tw_HPage){
	xdata	U8 tc_Offset ;
	data	U8	tc_i;
	data	bit tbt_FindPage=0;

	tc_Offset = tc_Inx*MAX_RANDOM_HPAGE;
	for(tc_i=0; tc_i<MAX_RANDOM_HPAGE;tc_i++){
		if(Mapping_Table[R_HPAGE+tc_Offset+tc_i] ==tw_HPage){
			gc_NowIndex =tc_i;
			tbt_FindPage=1;
			break;
		}	
	}	
	return tbt_FindPage;
}    
    
/*********************************************************************************
* NAME:               void NAND_SBlock_SPage_To_FDBP(void)
* DESCRIPTION:
*********************************************************************************/
void NAND_SBlock_SPage_To_FDBP(void){
	
	FDBP.cFDev =gc_HDev;
	FDBP.cFPlane = gw_TargetPage&(NAND_INFO.cPLANE_NUM-1);
	FDBP.wFBlock = APort_FBlock[0];
	FDBP.wFPage = gw_TargetPage/NAND_INFO.cPLANE_NUM;
	FDBP.cFInAddr =0;
}    
    
/*********************************************************************************
* NAME:                    bit Flash_ReadPage(void)
* DESCRIPTION:// SPDA26xx flash  plan
*   If ECC is Uncorrectable, return TRUE; else return FALSE. 
*********************************************************************************/
U8 Flash_ReadPage(void) {
	XBYTE[0xB400]=1;
	Device_Xfer_DataLength.WD = 0x03FF;
	SET_DEVICE_READ();
	ENA_ECC_Decode();
	FLASH_REG[0x06]  = 0x00;                                   // cmd1
	FLASH_REG[0x07]  = 0x30;                                   // cmd2
	NAND_InAddr_To_Column_NAND();                              // set Column
	NAND_FBlock_FPage_FBank_To_PhyAddr_NAND();                 // set FBank Fblock Fpage
	FLASH_REG[0x1A]	= 0x11;                                    // last frag = 1
	FLASH_REG[0x04] = 0x01;	                                   // CMD0+ADDR+CMD1
	CHK_Flash_Ready();
	TRIGGER_FLASH_CMD();                                       //               
	CHK_Flash_Ready();
	Trigger_Data_Transfer_Done();
	CHK_NandAutocmp_Done();
	CHK_ECC_Done();	
	return gbt_ECC_Fail;
}    
    
    
    
    
    
    
    
                    