/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	Flash_Init.c
 *	\brief	
 *
 *	\author	jay
 */

#include "SPDA2K.h"
static U16 Search_Last_Page(U16 TBlock);
static U8 CHK_ECCData_FF(void);
/*********************************************************************************
* NAME:                        void Chk_infor(void)
* DESCRIPTION:
*********************************************************************************/
void Chk_infor(void) {// 放在 CIS_Block[0]  的地方	
#if 1		
	Device_Read_SRAM_Index.WD = 0x9200;
	DEVICE_REG[0x0F] =0;
	FDBP.cFPlane =0;
	FDBP.wFBlock = CIS_Block[0]; 
	FDBP.wFPage = 0;
	FDBP.cFInAddr =0;
	for( FDBP.wFPage=0; FDBP.wFPage< NAND_INFO.wPAGE_NUM; FDBP.wFPage++){
		FDBP.cFInAddr = 0;
	    if(!Flash_ReadPage()){
			if(XWORD_9200[255]==0xDC54){
				if (XBYTE_9200[0] == 'M') {
					//gbt_Multi_Block = 1;
				}
				if (XBYTE_9200[1] == 'G') {
					SYS_ZONE.wZONE_SIZE = XBYTE_9200[2];
					SYS_ZONE.wZONE_SIZE = (SYS_ZONE.wZONE_SIZE<<8)+ XBYTE_9200[3];			
				}
				if (XBYTE_9200[4] == 'C') {
//					gbt_Copy_Back = 1;//08/11/14,joyce close copyback function
				}
				if (XBYTE_9200[5] == 'R') {
					// gbt_Enable_Replacement   =1;
				}
				if (XBYTE_9200[6] == 'R') {
					//gc_RW_Strobe=XBYTE_9200[7];
				}		
				if (XBYTE_9200[8] == 'W') {
					//gc_Write_T = XBYTE_9200[9];
				}
				if (XBYTE_9200[10] == 'R') {
				}
				if (XBYTE_9200[11] == 'D') {
       	    		gbt_Support_Dummy_Capacity=1;  //090416 add for dummy capacity
       	   			 Dummy_Capacity.BY[0] = XBYTE_9200[12];		
				    Dummy_Capacity.BY[1] = XBYTE_9200[13];	
				    Dummy_Capacity.BY[2] = XBYTE_9200[14];	
				    Dummy_Capacity.BY[3] = XBYTE_9200[15];
//				    dbprintf("\nDummy_Capacity.LW=%lX \n",Dummy_Capacity.LW);	
       			}
				//XBYTE_9200[16] XBYTE_9200[17] for MP use
				if (XBYTE_9200[18] == 'C' && XBYTE_9200[19] == 'D' ) {
					gbt_enableCDROM   =1;
					gc_CDROM_Size=XBYTE_9200[20];
//					dbprintf("CD-ROM 0x%bx MB\n",gc_CDROM_Size);
       		    }
			}				
			break;
		}
	}
	#endif
	
	
	
	
	
	
	
	
	
	
}



/*********************************************************************************
* NAME:         bit FlashReadID(void)
* DESCRIPTION:// SPDA26xx flash  plan
*      
*********************************************************************************/
#define	ID_1	XBYTE_8000[0]		//Maker Code
#define	ID_2	XBYTE_8000[1]		//Device Code
#define	ID_3	XBYTE_8000[2]		
#define	ID_4	XBYTE_8000[3]		
#define	ID_5	XBYTE_8000[4]		

bit FlashReadID(void){
	U8 tbt_newFlash=0;
    DEVICE_REG[0x21] = 0x55;
	CHK_Flash_Ready();
	FLASH_REG [0x00] = 0x00;		//強迫進IO Mode     
	DEVICE_REG[0x23] = 0x0A;	//CLE = '1', ALE = '0', nWP = '1', nCE = '0'
	DEVICE_REG[0x20] = 0xFF;
	DEVICE_REG[0x23] = 0x02;	//CLE = '0', ALE = '0', nWP = '1', nCE = '0'
	CHK_Flash_Ready();
	DEVICE_REG[0x23] = 0x0A;	//CLE = '1', ALE = '0', nWP = '1', nCE = '0'
	DEVICE_REG[0x20] = 0x90;
	DEVICE_REG[0x23] = 0x06;	//CLE = '0', ALE = '1', nWP = '1', nCE = '0'
	DEVICE_REG[0x20] = 0x00;
	DEVICE_REG[0x23] = 0x02;	//CLE = '0', ALE = '0', nWP = '1', nCE = '0'
	CHK_Flash_Ready();
	ID_1 = DEVICE_REG[0x20];
	gc_Global_I = 0;
	ID_2 = DEVICE_REG[0x20];
	gc_Global_I = 0;
	ID_3 = DEVICE_REG[0x20];
	gc_Global_I = 0;
	ID_4 = DEVICE_REG[0x20];	
	gc_Global_I = 0;
	ID_5 = DEVICE_REG[0x20];
	DEVICE_REG[0x23] = 0x03;	//CLE = '0', ALE = '0', nWP = '1', nCE = '1'	
	FLASH_REG [0x00] = 0x01;     
	
	if(FDBP.cFDev==0){
		for(gc_Global_I=0; gc_Global_I<5;gc_Global_I++){
			ID_Table[gc_Global_I] =XBYTE_8000[gc_Global_I];
		}
	}
	else{
		if((ID_Table[0]==ID_1)&&(ID_Table[1]==ID_2)){
			return FALSE;
		}
		else{
			return 1;
		}
	}	
	if((ID_1==0xec)&&(ID_4&0x4c)==0x08)//09/02/06,joyce add for new flash
	{
		
		tbt_newFlash=1;
	}
    if((ID_1==0xAD)&&(ID_4==0x25))//090924 JAY Add
	{
		
		tbt_newFlash=1;
	}
	NAND_INFO.cADDR_CYCLE = 5;  //default 5 cycle 
	NAND_INFO.cSUPPORT_MULT_PLANE = 0;
	NAND_INFO.cDEVICE_NUM	= 1;
	NAND_INFO.cPLANE_NUM	= 1;
	NAND_INFO.cSUPPORT_CACHE_RW =0;
	switch (ID_2) {
		case	SamsungF1024M33V:				//0xF1			
		case	0xD1:	// 128MB(1Gb)
			NAND_INFO.wBLOCK_NUM	= 1024;
			NAND_INFO.wPAGE_NUM		= 64;
			NAND_INFO.cINADDRMAX	= 4;
			NAND_INFO.cFMAP			= 0x01;
			NAND_INFO.cADDR_CYCLE 	= 4;  // 4 cycle
			NAND_INFO.cFLASH_TYPE	= 0;//SLC
			NAND_INFO.cECC_MODE		= 1;// 0-->24 1-->16
		
			break;
		case	SamsungF2048M33V:				//0xDA
			
			NAND_INFO.wBLOCK_NUM	= 2048;
			NAND_INFO.wPAGE_NUM		= 64;
			NAND_INFO.cINADDRMAX	= 4;
			NAND_INFO.cFMAP			= 0x01;
			NAND_INFO.cFLASH_TYPE	= 0;//SLC
			NAND_INFO.cECC_MODE		= 1;// 0-->24 1-->16
					
			if (((ID_3 & 0x0C) == 0x04)) {	//ID_4: Block Size = 256KB
				
				NAND_INFO.wBLOCK_NUM	= 1024;
				NAND_INFO.wPAGE_NUM		= 128;
				NAND_INFO.cFMAP			= 0x03;
				NAND_INFO.cFLASH_TYPE	= 1;//MLC
							
			}
			break;

		case	SamsungF4096M33V:				//0xDC
			
			NAND_INFO.wBLOCK_NUM	= 4096;
			NAND_INFO.wPAGE_NUM		= 64;
			NAND_INFO.cINADDRMAX	= 4;
			NAND_INFO.cFMAP			= 0x01;
			NAND_INFO.cFLASH_TYPE	= 0;//SLC
			NAND_INFO.cECC_MODE		= 1;// 0-->24 1-->16
					
			if (((ID_3 & 0x0C) == 0x04)) {	//ID_4: Block Size = 256KB
			
				NAND_INFO.wBLOCK_NUM	= 2048;
				NAND_INFO.wPAGE_NUM		= 128;
				NAND_INFO.cFMAP			= 0x03;
				NAND_INFO.cFLASH_TYPE	= 1;//MLC
							
			}
			break;
		case	SamsungF8192M33V:				//0xD3
			if (((ID_3 & 0x0C) == 0x00)){//SLC
				NAND_INFO.wBLOCK_NUM	= 8192;
				NAND_INFO.wPAGE_NUM		= 64;
				NAND_INFO.cINADDRMAX	= 4;
				NAND_INFO.cFMAP			= 0x01;
				NAND_INFO.cFLASH_TYPE	= 0;//SLC
				NAND_INFO.cECC_MODE		= 1;// 0-->24 1-->16
				if(((ID_3&0x03)==0x00)&(ID_1!=0xAD)){
					NAND_INFO.wBLOCK_NUM	= 4096;
					NAND_INFO.wPAGE_NUM		= 64;
					NAND_INFO.cINADDRMAX	= 8;
				}
			}
			else{
				
				NAND_INFO.wBLOCK_NUM	= 4096;
				NAND_INFO.wPAGE_NUM		= 128;
				NAND_INFO.cINADDRMAX	= 4;
				NAND_INFO.cFMAP			= 0x03;
				NAND_INFO.cFLASH_TYPE	= 1;//MLC
				NAND_INFO.cECC_MODE		= 1;// 0-->24 1-->16
				if((ID_4&0x03)==0x02){//H27U8G8T2BTR , TC58NVG3D1DTGIO
					NAND_INFO.wBLOCK_NUM	= 2048;
					NAND_INFO.cINADDRMAX	= 8;			
				}				
				if((ID_1==0xEC)&&(ID_4==0x72)){// 8KB
					if(ID_3&0x80){//0603 Jay fix
						NAND_INFO.cSUPPORT_CACHE_RW =1;
					}
					NAND_INFO.wBLOCK_NUM	= 1038;
					NAND_INFO.wPAGE_NUM		= 128;
					NAND_INFO.cINADDRMAX	= 16;
					NAND_INFO.cFMAP			= 0x03;
					NAND_INFO.cFLASH_TYPE	= 1;//MLC
					NAND_INFO.cECC_MODE		= 0;// 0-->24 1-->16
						
				}
			}
			break;
		case	SamsungF16384M33V:				//0xD5 090512Jay 變成以MLC為主
			if (((ID_3 & 0x0C) == 0x00)){//SLC
				NAND_INFO.wBLOCK_NUM	= 8192;
				NAND_INFO.wPAGE_NUM		= 64;
				NAND_INFO.cINADDRMAX	= 8;
				NAND_INFO.cFMAP			= 0x01;
				NAND_INFO.cFLASH_TYPE	= 0;//SLC
				NAND_INFO.cECC_MODE		= 1;// 0-->24 1-->16		
			}
			else{			
				NAND_INFO.wBLOCK_NUM	= 4096;
				NAND_INFO.wPAGE_NUM		= 128;
				NAND_INFO.cINADDRMAX	= 8;
				NAND_INFO.cFMAP			= 0x03;
				NAND_INFO.cFLASH_TYPE	= 1;//MLC
				NAND_INFO.cECC_MODE		= 1;// 0-->24 1-->16
				if(ID_3&0x80){//0603 Jay fix
					NAND_INFO.cSUPPORT_CACHE_RW =1;
				}
				if((ID_3&0x03)==0x01){//2 k page 2_Die 
					NAND_INFO.wBLOCK_NUM	= 8192;
					NAND_INFO.cINADDRMAX	= 4;
					NAND_INFO.cSUPPORT_CACHE_RW =0;
				}
				if((ID_1==0xEC)&&(ID_4==0x72)){// 8KB
					NAND_INFO.wBLOCK_NUM	= 2076;
					NAND_INFO.wPAGE_NUM		= 128;
					NAND_INFO.cINADDRMAX	= 16;
					NAND_INFO.cFMAP			= 0x03;
					NAND_INFO.cFLASH_TYPE	= 1;//MLC
					NAND_INFO.cECC_MODE		= 0;// 0-->24 1-->16
				
				}
				if((ID_1==0x98)&&((ID_4&0xB3)==0x32)){// toshiba 8KB
					NAND_INFO.wBLOCK_NUM	= 2076;// 因為TC58NVG4D2ETA00 :2084 但是 TC58NVG4D2FTA00：2076
					NAND_INFO.wPAGE_NUM		= 128;	//但ID table完全相同故取最小值  2076
					NAND_INFO.cINADDRMAX	= 16;
					NAND_INFO.cFMAP			= 0x03;
					NAND_INFO.cFLASH_TYPE	= 1;//MLC
					NAND_INFO.cECC_MODE		= 0;// 0-->24 1-->16
				}
				if(tbt_newFlash){
					NAND_INFO.cECC_MODE		= 0;// 0-->24 1-->16
					
				}
			}
			break;
		case	SamsungF32768M33V:				//0xD7 應該會使用新的ID table
		
			NAND_INFO.wBLOCK_NUM	= 8192;// L63a
			NAND_INFO.wPAGE_NUM		= 128;
			NAND_INFO.cINADDRMAX	= 8;
			NAND_INFO.cFMAP			= 0x03;
			NAND_INFO.cFLASH_TYPE	= 1;//MLC
			NAND_INFO.cECC_MODE		= 0;// 0-->24 1-->16		
			if(ID_3&0x80){//0603 Jay fix
				NAND_INFO.cSUPPORT_CACHE_RW =1;
			}			
			if(NAND_INFO.cSUPPORT_CACHE_RW ==0){//M Die 1j不支持cach 
				NAND_INFO.cECC_MODE		= 1;// 0-->24 1-->16				
			}
			else if((ID_1!=0x89)&&(ID_1!=0x2C)){//非L63a
				if((ID_3&0x03)==0x01){//4 k page 2_Die // toshiba
					NAND_INFO.wBLOCK_NUM	= 8192;// TH58NVG6D1DTG20
					NAND_INFO.cINADDRMAX	= 8;
				}
				else if((ID_4&0x03)==0x02){//8KB 					
					NAND_INFO.cINADDRMAX	= 16;
					NAND_INFO.cFMAP			= 0x03;
					NAND_INFO.cFLASH_TYPE	= 1;//MLC
					NAND_INFO.cECC_MODE		= 0;// 0-->24 1-->16					
					if(ID_1==0xAD){
						NAND_INFO.cFMAP			= 0x10;//Bit 4~7 表示HW不支持需要使用SW方式轉換
						NAND_INFO.wBLOCK_NUM	= 2048;
						NAND_INFO.wPAGE_NUM		= 256;				
					}
					if(ID_1==0xEC){
						NAND_INFO.wBLOCK_NUM	= 4152;
						NAND_INFO.wPAGE_NUM		= 128;		
					}
					if(ID_1==0x98){
						NAND_INFO.wBLOCK_NUM	= 4096;
						NAND_INFO.wPAGE_NUM		= 128;		
					}
				}			
			}
			break;
		case	SamsungF65536M33V:				//0xDE
		case	0xD9:     // Micron 64G Device code   	

			NAND_INFO.cINADDRMAX	= 16;
			NAND_INFO.cFMAP			= 0x03;
			NAND_INFO.cFLASH_TYPE	= 1;//MLC
			NAND_INFO.cECC_MODE		= 0;// 0-->24 1-->16
			if(ID_3&0x80){//0603 Jay fix
				NAND_INFO.cSUPPORT_CACHE_RW =1;
			}
			if(ID_1==0xAD){
				NAND_INFO.cFMAP			= 0x10;//Bit 4~7 表示HW不支持需要使用SW方式轉換
				NAND_INFO.wBLOCK_NUM	= 4096;
				NAND_INFO.wPAGE_NUM		= 256;				
			}
			if(ID_1==0xEC){
				NAND_INFO.wBLOCK_NUM	= 8304;
				NAND_INFO.wPAGE_NUM		= 128;		
			}
			if(ID_1==0x98){
				NAND_INFO.wBLOCK_NUM	= 8192;
				NAND_INFO.wPAGE_NUM		= 128;
									
			}
			break;	
		case 0x48: // 16Gb Micron 12bit flash  //090820  Jay add 
			
			NAND_INFO.wBLOCK_NUM	= 2048;
			NAND_INFO.wPAGE_NUM		= 256;
			NAND_INFO.cINADDRMAX	= 8;
			NAND_INFO.cFMAP			= 0x10;//Bit 4~7 表示HW不支持需要使用SW方式轉換
			NAND_INFO.cFLASH_TYPE	= 1;//0:SLC 1:MLC, 2:TLC
			NAND_INFO.cECC_MODE		= 0;// 0-->24 1-->16
		
			NAND_INFO.cSUPPORT_CACHE_RW =1;
			break;
		case 0x68 : // 32 Gb Micron 12 bit flash //090820  Jay add
			
			NAND_INFO.wBLOCK_NUM	= 4096;
			NAND_INFO.wPAGE_NUM		= 256;
			NAND_INFO.cINADDRMAX	= 8;
			NAND_INFO.cFMAP			= 0x10;//Bit 4~7 表示HW不支持需要使用SW方式轉換
			NAND_INFO.cFLASH_TYPE	= 1;//0:SLC 1:MLC, 2:TLC
			NAND_INFO.cECC_MODE		= 0;// 0-->24 1-->16
				    
			NAND_INFO.cSUPPORT_CACHE_RW =1;
			break; 
		case 0x88:  // 64 Gb Micron 12 bit flash  //090820  Jay add
		
			NAND_INFO.wBLOCK_NUM	= 4096;
			NAND_INFO.wPAGE_NUM		= 256;
			NAND_INFO.cINADDRMAX	= 16;
			NAND_INFO.cFMAP			= 0x10;//Bit 4~7 表示HW不支持需要使用SW方式轉換
			NAND_INFO.cFLASH_TYPE	= 1;//0:SLC 1:MLC, 2:TLC
			NAND_INFO.cECC_MODE		= 0;// 0-->24 1-->16
				
			NAND_INFO.cSUPPORT_CACHE_RW =1;
			if((ID_3&0x03)==0x01){//4 k page 2_Die 
				NAND_INFO.wBLOCK_NUM	= 8192;
				NAND_INFO.cINADDRMAX	= 8;
			}


			break;
		default:
			
			break;
			
	}
	if(NAND_INFO.cECC_MODE== 1){// 0-->24 1-->16
		NAND_INFO.cECC_NUM	= 32;
		gc_MAX_ERR_BIT_NUM = (16-MAX_ERRBIT_OFFSET);
		
	}
	else{
		NAND_INFO.cECC_NUM	= 46;
		gc_MAX_ERR_BIT_NUM = (24-MAX_ERRBIT_OFFSET);
	}
	if(NAND_INFO.cINADDRMAX>=8){
		gc_RW_Strobe =0x12;
	}
		
	
	
	return FALSE;
}
/*********************************************************************************
* NAME:                     void Init_Flash_Reg(void)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/
void Init_Flash_Reg(void) {
	DEVICE_REG[0x05] = 0x04;
	DEVICE_REG[0x01] = 0x65;
	DEVICE_REG[0x00] = 0x01;
	DEVICE_REG[0x22] = 0x00;
	DEVICE_REG[0x23] = 0x03;

	if(MaxDevSize ==0x02){
		DEVICE_REG[0x0D]=0x03;
	}
	else{
		DEVICE_REG[0x0D] = 0x0F;
	}									//Enable CE0   
	FLASH_REG[0x02]  = 0x55;									//FLASH CMD/ADDR 50ns
}
/*********************************************************************************
* NAME:                     void Init_Flash_Variable(void)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/
void Init_Flash_Variable(void) {  //Jay default  always disable
	gc_CardType			    = 0x00;
	gbt_Code_NeedRecover 	= 0;	
	gc_WriteProof 			= 0x00; 
	gbt_Support_Dummy_Capacity =0;   //090416 add for initial value
	gbt_enableCDROM =0;					//09/04/24,joyce
	gc_CDROM_Size=0 ;					//09/04/24,joyce(unit:MB)
	SYS_ZONE.wZONE_SIZE =MAX_ZONE_SIZE;			// default 240
    gc_RW_Strobe =0x13;
    gbt_USB_Data_In_Buf=0;
}
/*********************************************************************************
* NAME:                         void InitFlash(void)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/
void InitFlash(void) {

	bit tbt_ReadIDFail =0;
	U8 tc_DeviceCnt=0;
	for (FDBP.cFDev = PhyDevStart; FDBP.cFDev< MaxDevSize; FDBP.cFDev++) {
		DMTTable[FDBP.cFDev] =0xFFFF;	
		DEVICE_REG[0x0F] =FDBP.cFDev;
		tbt_ReadIDFail = FlashReadID();
		if (!tbt_ReadIDFail) {
			Flash_State_Initial();			
			SearchBMT();
			tc_DeviceCnt++;			
		}
		else {
			FLASH_REG[0x00] =0x01; 
			Flash_State_Initial();   
			break;
		}
	}
	NAND_INFO.cDEVICE_NUM=tc_DeviceCnt;// updata NAND info	
	
	if (!NAND_INFO.cDEVICE_NUM) {
		Capacity.LW = 1;
		//CardExist = 0x00;
	}
	else {
		// 0nly for User& HZK & DSP use 
		gw_PagesPerBlock  = NAND_INFO.wPAGE_NUM;
		gc_InAddrMax = NAND_INFO.cINADDRMAX;
		gw_DataBlockSize  = (gw_PagesPerBlock*gc_InAddrMax);
		//-----------------------------------------------------//
		
		
		H_INFO.cHPAGESECTOR= NAND_INFO.cINADDRMAX*NAND_INFO.cPLANE_NUM;  // Hblock 中page sector 
		H_INFO.wHBLOCKSECTOR = NAND_INFO.wPAGE_NUM*H_INFO.cHPAGESECTOR; // Hblock 中total sector num
		H_INFO.wHBLOCKPAGE= NAND_INFO.wPAGE_NUM*NAND_INFO.cPLANE_NUM;	//Hblock  total page num	
		//H_INFO.cSMALLPAGE_NUM = (EXT_SMALL_BLOCK_SIZE/NAND_INFO.cINADDRMAX)<<1;//轉換成small block page num  // 0520 Jay mark  
		H_INFO.cSMALLPAGE_NUM = EXT_SMALL_BLOCK_SIZE;//0520 Jay add
		H_INFO.cSMALLBLOCK_NUM = H_INFO.wHBLOCKPAGE/H_INFO.cSMALLPAGE_NUM;// 計算EX block 中的small block num 	
		
		
		SYS_ZONE.cDEV_ZONE_NUM = NAND_INFO.wBLOCK_NUM>>8;
		SYS_ZONE.wDEV_DATABLK_NUM = ((U16)SYS_ZONE.cDEV_ZONE_NUM*SYS_ZONE.wZONE_SIZE);
		SYS_ZONE.cTOTALZONE_NUM = SYS_ZONE.cDEV_ZONE_NUM *NAND_INFO.cDEVICE_NUM;				
		#if 0//only test  
		Capacity.LW = ((LWORD)SYS_ZONE.wZONE_SIZE * H_INFO.wHBLOCKSECTOR)*3;	
		#else
		Capacity.LW = ((LWORD)SYS_ZONE.wDEV_DATABLK_NUM * NAND_INFO.wPAGE_NUM)*NAND_INFO.cINADDRMAX;	
		Capacity.LW = Capacity.LW * NAND_INFO.cDEVICE_NUM;	
		#endif

        if(gbt_Support_Dummy_Capacity){//090416 add for error dummy capacity
            if(Dummy_Capacity.LW<Capacity.LW){
//              dbprintf("\n Error Dummy_Capacity:%lX",Dummy_Capacity.LW);
                Dummy_Capacity.LW = Capacity.LW;
            }       
        }
		//CardExist = 0xFF;
		Capacity_CDROM.LW=((U32)gc_CDROM_Size<<11);//09/04/15,joyce
		Capacity_Remove.LW=Capacity.LW-Capacity_CDROM.LW;//09/04/15,joyce
	}
	if (!NAND_INFO.cDEVICE_NUM || gc_WriteProof) {
		gc_CardExist &=0xFE;  // 081014 Jay Flash initial fail  disable flash system 
//	        dbprintf("\n Decice Not Ready"); 
	}
	else {		
//		dbprintf("\n Capacity.lw:%lx",Capacity.LW);
		gc_CardType = 0x01; //  081014 Jay now flash interface enable
		gc_CardExist|=0x01;
		LoadFirstTable();
	}
	Flash_State_Initial();
}
/*********************************************************************************
* NAME:                    void LoadFirstTable(void)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/

void LoadFirstTable(void) {
   gw_WhichGroup =0;
	gw_NowBMTPoint = 0;
	Mapping_Table = MAP_0;  	// switch table pointer
	Wear_Table    = WEARTAB_0;	// switch table pointer
	MAP_Table[0] = gw_WhichGroup;   	//先將BMT 0  load 到map ram 0
	MAP_Table[1] = 0xFF;	// oxFF表示 map ram 1 是空的	
	BMT_KEY[0]   = 0;
	BMT_KEY[1]   = 0;
	BMT.cFDev  	 = 0;
	BMT.cFPlane  = 0;
	BMT.wFBlock  = BMTTable[BMT.cFDev]; 
	BMT.wFPage 	 = 0;
	BMT.cFInAddr = 0;
	Create_Zone_Lookup_Table();	
	
}




/*********************************************************************************
* NAME:                  bit Search_Bank_Block(void)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/
bit Search_BankCode_Block(void){
	 bit tbt_Search_Fail=1;
    U8  tc_Index=0;
    gc_CodeBlock_Num =4096/(NAND_INFO.wPAGE_NUM*NAND_INFO.cINADDRMAX);
	gc_DSPBlock_Num =4096/(NAND_INFO.wPAGE_NUM*NAND_INFO.cINADDRMAX);
	//dbprintf("\ngc_CodeBlock_Num %bx",gc_CodeBlock_Num);
	//dbprintf("\ngc_DSPBlock_Num %bx",gc_DSPBlock_Num);	
	//gc_HZKBlock_Num =16384/(gc_PagesPerBlock*gc_InAddrMax);//不用計算因為直接記錄在table 中
    FLASH_REG[0xA5]= 0x0A;     //強制切換成24 bit ECC  
	FlashReset();
	FDBP.cFDev    = 0;
	FDBP.cFPlane  = 0;
	FDBP.wFBlock  = 0;
	FDBP.wFPage   = 0;
	FDBP.cFInAddr = 0;
	Device_Read_SRAM_Index.BY[0] = 0x80;
	Device_Read_SRAM_Index.BY[1] = 0x00;
	while(1){
		for(FDBP.wFPage=0; FDBP.wFPage<NAND_INFO.wPAGE_NUM;FDBP.wFPage++){            
	        if(!Flash_ReadPage()){ // 理論上這個Page應該是ECC pass ，如果ECC fail 只好試試看下一個page，如果不行只有在找下一個code info block                
	            if((FLASH_REG[0x60]=='C')&& (FLASH_REG[0x61] == 'O')&&(FLASH_REG[0x62]=='D')&&(FLASH_REG[0x63] == 'E')){
		 	    	Bank_Info_Block[tc_Index]= FDBP.wFBlock;
					tbt_Search_Fail =0;
					tc_Index++;
					break;
				}
			}
		}
	 	FDBP.wFBlock++;    
	 	if((FDBP.wFBlock>20)||(tc_Index==0x02)){	// 當超過1/16後直接跳出		
			
			break;		
		}
	}
	if(!tbt_Search_Fail ){ //有找到Code info block
        if((XBYTE_8000[508] == 'S' )&&(XBYTE_8000[509] == 'U' )&&(XBYTE_8000[510] == 'N')){
            gc_Load_Code_Size = XBYTE_8000[100];    

            for(tc_Index =0; tc_Index<gc_CodeBlock_Num;tc_Index++){
                Bank_Block_A[tc_Index] =  XWORD_8000[(60+tc_Index)];    // 080328 Jay fix 2KB slc flash
                Bank_Block_B[tc_Index] =  XWORD_8000[(80+tc_Index)];     // 080328 Jay fix 2KB slc flash                     
//            	dbprintf("\n CodeA: %x , B:%x",Bank_Block_A[tc_Index],Bank_Block_B[tc_Index]);
            }
   
			for(tc_Index =0; tc_Index<gc_DSPBlock_Num;tc_Index++){
				DSP_Block_A[tc_Index] =  XWORD_8000[(112+tc_Index)];
				DSP_Block_B[tc_Index] =  XWORD_8000[(128+tc_Index)];  
//				dbprintf("\n DSP A: %x , B:%x",DSP_Block_A[tc_Index],DSP_Block_B[tc_Index]);
			}
			
			gc_HZKBlock_Num =(U8)(((U32)(XBYTE_8000[114])<<10)/((NAND_INFO.wPAGE_NUM*NAND_INFO.cINADDRMAX)>>1));		
			if(gc_HZKBlock_Num>64){
//				dbprintf("\nHZK_Block_Num %bx out of ram size \n",gc_HZKBlock_Num);
			}
			for(tc_Index =0; tc_Index<gc_HZKBlock_Num;tc_Index++){
				HZK_Block_A[tc_Index] =  XWORD_8000[(256+tc_Index)];
//				dbprintf("\n HZK:%x" ,HZK_Block_A[tc_Index]);
			}

            Flash_State_Initial(); // 切換回default Ecc bit 
            return 0;
        } 
		
	}
	Flash_State_Initial(); // 切換回default Ecc bit 
	return 1;
}

/*********************************************************************************
* NAME:                  void SearchBMT(void)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/
void SearchBMT(void) {
	U8 tc_DevZoneNum; 
	
	DEVICE_REG[0x0F] =FDBP.cFDev;
	FlashReset();	
	FDBP.cFPlane  = 0;
	FDBP.cFInAddr = 0;
	FDBP.wFBlock  = NAND_INFO.wBLOCK_NUM - 2;
	Device_Read_SRAM_Index.BY[0] = 0x80;
	Device_Read_SRAM_Index.BY[1] = 0x00;
	gc_WriteProof =1;// 1 : system busy or fail 0: system ready	 
	while(1){    // first  Search DMT Block  																			//  只要三個Tag中有一個符合即可
		for( FDBP.wFPage=0; FDBP.wFPage< NAND_INFO.wPAGE_NUM; FDBP.wFPage++){
			FDBP.cFInAddr = 0;
			if(!Flash_ReadPage()){// ECC OK 
		    	if((FLASH_REG[0x60]==0x3E)&&(FLASH_REG[0x61]=='D')&&(FLASH_REG[0x62]=='M')&&(FLASH_REG[0x63]=='T')){
 	    	    	DMTTable[FDBP.cFDev] = FDBP.wFBlock;
			    	DMT_Page[FDBP.cFDev] = FDBP.wFPage;
			    	gc_WriteProof =0;
			    }
		    }
		    if(!gc_WriteProof){
				break;
			}  
		}
		if(!gc_WriteProof){
			break;
		}
		FDBP.wFBlock--;    
	 	if (FDBP.wFBlock<(NAND_INFO.wBLOCK_NUM-15)) {
			gc_WriteProof = 1;
			DMTTable[FDBP.cFDev] = 0xFFFF;
//			dbprintf("\n Search BMT Fail");
			return;			
		}
	}
	FDBP.wFBlock = DMTTable[FDBP.cFDev];
	FDBP.cFPlane =0;
	FDBP.wFPage  = DMT_Page[FDBP.cFDev];
	FDBP.cFInAddr =0;
	Device_Read_SRAM_Index.BY[0] = 0x90;
	Device_Read_SRAM_Index.BY[1] = 0x00;
	Flash_ReadPage();// 已經不需要再判斷了～之前已經判斷過一次了～
	BMTTable[FDBP.cFDev]= XWORD_9000[0];  
	BMTTable_Backup[FDBP.cFDev] =  XWORD_9000[1];
	ReplacementBlock_Table[FDBP.cFDev] = XWORD_9000[2];
	//--------  100617 Jay fix -----------//
	gw_SourceBlock = ReplacementBlock_Table[FDBP.cFDev];
	ReplacementBlock_Page[FDBP.cFDev] =Search_Last_Page(gw_SourceBlock);	
	TempBlock_Table[FDBP.cFDev] = XWORD_9000[3];
	//-----------------------------------//	
	if(!FDBP.cFDev){ // 只有第一顆 flash 保留CIS block 
		for(gc_Global_I =0; gc_Global_I<CIS_Block_Num;gc_Global_I++) {
			CIS_Block[gc_Global_I] = XWORD_9000[(BMTBLK_NUM+TEMPBLK_NUM+REPLACEBLK_NUM)+gc_Global_I];
			
		}
		Chk_infor();  // Jay 放CIS[0] 的位置
	}
	if(!FDBP.cFDev){ // 只有第一顆 flash 保留Userdata block 
		for(gc_Global_I =0; gc_Global_I<User_Block_Num;gc_Global_I++) {
			UserData_Block[gc_Global_I]=XWORD_9000[(BMTBLK_NUM+TEMPBLK_NUM+REPLACEBLK_NUM+CIS_Block_Num)+gc_Global_I];
			
		}	
	}
	tc_DevZoneNum = NAND_INFO.wBLOCK_NUM>>8;
	tc_DevZoneNum = tc_DevZoneNum>>1;
	if(!FDBP.cFDev){
		for(gc_Global_I =0; gc_Global_I<64;gc_Global_I++) {  // initial fastbmt table
			FAST_BMTTable[gc_Global_I]=0xFFFF;
	    	FAST_BMTPage_Table[gc_Global_I]=0;
	   
		}
	}
	
	  
	
}
#if 0
/*********************************************************************************      
* NAME:                  void Flash_Set_Feature(U8 cAddr,U8 cdata )                              
* DESCRIPTION:                                             
*********************************************************************************/   
void Flash_Set_Feature(U8 cAddr,U8 cdata ){                                            
    	DEVICE_REG[0x21] = 0x55;                                                                             
    	CHK_Flash_Ready();                                                                                   
    	FLASH_REG [0x00] = 0x00;	                       
    	CHK_Flash_Ready();                                 
    	DEVICE_REG[0x23] = 0x0A;	                       
    	DEVICE_REG[0x20] = 0xEF;                      
    	DEVICE_REG[0x23] = 0x06;	                       
    	DEVICE_REG[0x20] = cAddr;                         
    	DEVICE_REG[0x23] = 0x02;	                       
    	                                                                                                     
    	DEVICE_REG[0x20]=cdata;                                                              
    	DEVICE_REG[0x20]=0x00;                                                                               
    	DEVICE_REG[0x20]=0x00;                                                                               
    	DEVICE_REG[0x20]=0x00;                                                                               
    	                                                                                                     
    	CHK_Flash_Ready();                                                                                   
    	
                                                                                                         
    
}    
#endif
/*********************************************************************************
* NAME:                  U16 Search_Last_Page(U16 TBlock)                         
* DESCRIPTION: 找尋最後一個有資料 page  // SPDA26xx flash  plan                                          
*********************************************************************************/
static U16 Search_Last_Page(U16 TBlock){                                               
    U8 	tbt_Data_0xFF=0;                                                        
    FDBP.wFBlock = TBlock;
    gw_TargetPage=(NAND_INFO.wPAGE_NUM-1);//
    while(gw_TargetPage){    
    
    	
    	FDBP.cFPlane =gw_TargetPage%NAND_INFO.cPLANE_NUM;
    	FDBP.wFPage = gw_TargetPage/NAND_INFO.cPLANE_NUM;
    	if(CHK_ECCData_FF()){// 再次檢查是否為真的全部都是FF
			tbt_Data_0xFF =1;//ECC 全部都是0xFF			
		}
		else{
			tbt_Data_0xFF=0;  // 非全部都是0xFF;
			break;
		}

		gw_TargetPage--;
    }  
    return gw_TargetPage;            

}
/*********************************************************************************
* NAME:                bit CHK_ECCData_FF(void)
* DESCRIPTION:
*********************************************************************************/
static U8 CHK_ECCData_FF(void){ 
	bit tbt_Data_0xFF;
	U8 tc_i;	
	Device_Read_SRAM_Index.BY[0] = 0x9C;//0601 Jay fix 0x80-->0x9C
	Device_Read_SRAM_Index.BY[1] = 0x00;
	Device_Xfer_DataLength.BY[0] = 0x00;
	Device_Xfer_DataLength.BY[1] = NAND_INFO.cECC_NUM;
	tbt_Data_0xFF      = 0;
	FLASH_REG[0xA3]  = 0x00;									// disable ECC 	
	DEVICE_REG[0x0E] = 0x10;									// Enable Falsh data check oxFF
	SET_DEVICE_READ();
	FLASH_REG[0x06]  = 0x00;									// cmd1
	FLASH_REG[0x07]  = 0x30;									// cmd2	
	FLASH_REG[0x0C] =  0x00;									//column_L
	FLASH_REG[0x0B] =  0x04;									//column_H
	NAND_FBlock_FPage_FBank_To_PhyAddr_NAND();					// set FBank Fblock FPage	
	FLASH_REG[0x04]  = 0x01;									// CMD0+ADDR+CMD1	
	FLASH_REG[0x01] &= 0xF0;									// don't atuo read redunt
	CHK_Flash_Ready();
	TRIGGER_FLASH_CMD();	
	FLASH_REG[0x1A]  = 0x11; 
	CHK_Flash_Ready();
	Trigger_Data_Transfer_Done();    	
	if(DEVICE_REG[0x0E]&0x20){ // Flash Data is not 0xFF  
		gc_Global_I =0;
		tbt_Data_0xFF= 0;
		for(tc_i =0; tc_i<NAND_INFO.cECC_NUM;tc_i++){ // Double check 
			if(XBYTE_9C00[tc_i] ==0xFF){//0601 Jay fix 0x80-->0x9c
				gc_Global_I ++;
			}
		}
		if(gc_Global_I>(NAND_INFO.cECC_NUM>>1)){ //有可能只是flash bit error 誤判
			tbt_Data_0xFF =1;
		}
	}
	else{
		tbt_Data_0xFF =1;
	}
	return tbt_Data_0xFF;
}  