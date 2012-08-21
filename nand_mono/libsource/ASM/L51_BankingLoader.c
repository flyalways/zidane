#include <stdio.h>
#include "SPDA2K.h"

#define SECTORS_PER_BANK	32  //1 sector = 1K Byte

U8 data gc_bank_number = 0;	//!< The current banking number in instruction SRAM.
U8 tgc_InAddr; //0602 Jay add for  
bit FLASH_MultRead_DRAM(bit tbt_Backup, U8 tc_FPage, UBYTE tc_Block1, UBYTE tc_Block0, UBYTE tc_PageNum);
void Banking_NAND_InAddr_To_Column_NAND(UBYTE tc_InAddr);
void Banking_NAND_FBlock_FPage_FBank_To_PhyAddr_NAND(U8  tc_FPage, UBYTE tc_FBank, UBYTE tc_Block1, UBYTE tc_Block0);

//! Entry of the banking loader.
void perform_nand_bank_call(void)
{
#pragma asm
Using3_R0 EQU	018H
Using3_R1 EQU	019H
Using3_R2 EQU	01AH
Using3_R3 EQU	01BH
Using3_R4 EQU	01CH
Using3_R5 EQU	01DH
Using3_R6 EQU	01EH
Using3_R7 EQU	01FH


	cjne A, gc_bank_number, ?_need_to_load_nand_bank
	push DPL
	push DPH
	ret

?_need_to_load_nand_bank:
	//Target Bank Number is in register A
	push gc_bank_number
	mov  gc_bank_number, A

	//push the bank recover function for returning use
	mov  A, #LOW return_from_nand_bank
	push ACC
	mov  A, #HIGH return_from_nand_bank
	push ACC
	//push target address
	push DPL
	push DPH
 	//push PSW and IE
	push PSW
	mov  PSW, #18	//use register bank3
	push IE
	clr  EA			//disable IRQ

	call load_nand_bank

	//recover IE and PSW
	pop  IE
	pop  PSW
	ret
#pragma endasm
}

//! Exit of the banking loader.
void return_from_nand_bank(void)
{
#pragma asm
	mov  A, gc_bank_number	//A is current bank
	pop  gc_bank_number		//gc_bank_number is return bank
	push PSW
	cjne A, gc_bank_number, ?_recover_need_load_nand
	pop PSW
	ret
?_recover_need_load_nand:
	mov  PSW, #18 			//use register bank3
	push IE
	clr  EA					//disable IRQ

	call load_nand_bank

	//pop PSW and IE
	pop  IE
	pop  PSW
	ret
#pragma endasm
}


//! NAND banking loader.
void load_nand_bank(void) using 3
{
	U8 tc_banks_per_block, tc_bank_number;
	U8  tc_FPage;
	U8  tc_InAddr;
	union WORDDataType u_FBlock;
	U8 tc_pages_per_bank;
	U8 tc_bank_block, tc_bank_offset,tc_InAddrMax;

	XBYTE[0xB400]=1;
	tc_InAddrMax=gc_InAddrMax;

	tc_pages_per_bank = SECTORS_PER_BANK/tc_InAddrMax;			// gc_InAddrMax = sectors per page
	tc_banks_per_block = gw_PagesPerBlock/tc_pages_per_bank;	// Number of 16kB banks in a block
	if (gc_bank_number == 0)
	{
		tc_bank_block = 0;
		tc_FPage = (48/tc_InAddrMax);			// commom is 20k 
		tgc_InAddr = 48%tc_InAddrMax;	//0602 Jay add
	}
	else
	{
		tc_bank_number	= gc_bank_number +2;	// Offset of the bank to the start of code region in unit of 16kB
		tc_bank_block	= tc_bank_number / tc_banks_per_block;	// in unit of 16kB
		tc_bank_offset	= tc_bank_number % tc_banks_per_block;	// Offset of the bank in its block in unit of 16kB
		tc_FPage		= tc_bank_offset * tc_pages_per_bank;	// page offset of the bank in its block
		tgc_InAddr       = 0;
	}
	tc_InAddr =tgc_InAddr;
    DEVICE_REG[0x0F] = 0x00;     // CE0
    Device_Read_SRAM_Index.WD = 0x6000;
    u_FBlock.WD = Bank_Block_B[tc_bank_block]; 
//	dbprintf("%bx ", gc_bank_number);
    if(FLASH_MultRead_DRAM(0, tc_FPage, u_FBlock.BY[1], u_FBlock.BY[0], SECTORS_PER_BANK-8))
	{
//		dbprintf("BankFail:%bx\n",gc_bank_number);
		gbt_Code_NeedRecover=1;
    	tgc_InAddr=tc_InAddr;
    	Device_Read_SRAM_Index.WD = 0x6000;
        u_FBlock.WD = Bank_Block_A[tc_bank_block];
		if (FLASH_MultRead_DRAM(1, tc_FPage, u_FBlock.BY[1], u_FBlock.BY[0], SECTORS_PER_BANK-8))
		{
			GLOBAL_REG[0x91] &= ~0x01;	// turn off LDO
			CPU_REG[0x20] = 1;	// select internal boot ROM.
			CPU_REG[0x21] = 1;	// reset MCU
		}
	}
}


/*********************************************************************************
* NAME:                    bit  FLASH_MultRead_DRAM(void)
* DESCRIPTION:
*   If ECC is Uncorrectable, return TRUE; else return FALSE. 
*********************************************************************************/
bit FLASH_MultRead_DRAM(bit tbt_Backup, U8 tc_FPage, UBYTE tc_Block1, UBYTE tc_Block0, UBYTE tc_PageNum)
{
	bit	tbt_DoSendCommand=FALSE;
	union WORDDataType u_FBlock1;
	UBYTE tc_InAddr = 0, tc_FBank = 0,tc_InAddrMax;
	tc_InAddrMax=gc_InAddrMax;
	//if(gbt_newFlash)					//09/02/10,joyce
	//{
	//	tc_InAddrMax=tc_InAddrMax>>1;
	//}
	tc_InAddr = tgc_InAddr;//0602 Jay add
	u_FBlock1.BY[1] = tc_Block1;
	u_FBlock1.BY[0] = tc_Block0;
	Reset_DMA();	
	Device_Xfer_DataLength.WD = 0x03FF;   
	SET_DEVICE_READ_TO_DRAM();
	DEVICE_REG[0xA0] = 0x01;									// Reset ECC  
	FLASH_REG[0xA3]  = 0x01;									// Enable ECC 
	FLASH_REG[0xA0]  = 0x00;									// Disable Encode RS-Code
	FLASH_REG[0xA1]  = 0x01;									// Start Decode RS-Code
	FLASH_REG[0xA2] &= 0xF3;									// Clear ECC fail register
	FLASH_REG[0x06]  = 0x00;									// cmd1
	FLASH_REG[0x07]  = 0x30;									// cmd2	
	Banking_NAND_InAddr_To_Column_NAND(tc_InAddr);				// set Column
	Banking_NAND_FBlock_FPage_FBank_To_PhyAddr_NAND(tc_FPage, tc_FBank, u_FBlock1.BY[1], u_FBlock1.BY[0]); // set FBank Fblock Fpage	
	CHK_Flash_Ready();
	FLASH_REG[0x04] = 0x01;										// CMD0+ADDR+CMD1
	TRIGGER_FLASH_CMD();
	CHK_Flash_Ready();
	FLASH_REG[0x1A] = 0x10;
	while(tc_PageNum){
        if(tbt_DoSendCommand ){
            FLASH_REG[0x06]  = 0x00;							// cmd1
	        FLASH_REG[0x07]  = 0x30;							// cmd2	
	        Banking_NAND_InAddr_To_Column_NAND(tc_InAddr);		// set Column
        	Banking_NAND_FBlock_FPage_FBank_To_PhyAddr_NAND(tc_FPage, tc_FBank, u_FBlock1.BY[1], u_FBlock1.BY[0]); // set FBank Fblock Fpage
	        FLASH_REG[0x04] = 0x01;								// CMD0+ADDR+CMD1
	        TRIGGER_FLASH_CMD(); 
	        CHK_Flash_Ready();
	        tbt_DoSendCommand= 0;
        }                 
        FLASH_REG[0xA1]  = 0x01;								// Start Decode RS-Code
	    FLASH_REG[0xA2] &= 0xF3;
        FLASH_REG[0x1A]	= 0x10; 
        if((tc_InAddr==(tc_InAddrMax-2))||(tc_PageNum==0x02)){
	        FLASH_REG[0x1A]	= 0x11;								// last frag = 1
	    }        
        Trigger_Data_Transfer();        
		Device_Read_SRAM_Index.BY[0] += 0x04 ;					// 080411 Joyce add for auotcount function fail
		Device_Read_SRAM_Index.BY[1]  = 0x00 ;					// 080411 Joyce add for auotcount function fail          
       
        tc_InAddr+=2;
        if(tc_InAddr==tc_InAddrMax){
            tbt_DoSendCommand = TRUE;
            tc_InAddr = 0x00;
            tc_FPage++;
            if(tc_FPage==gw_PagesPerBlock){
                tc_FPage = 0x00;
                tc_FBank++;
                gc_Flash_Data++;
                //if(tc_FBank==gc_BanksPerDevice){
                //    tc_FBank =0x00;
                //}
                if(tbt_Backup){
                    u_FBlock1.WD = Bank_Block_A[gc_Flash_Data];
                }
                else{
                    u_FBlock1.WD = Bank_Block_B[gc_Flash_Data]; 
                }
            }
        }
        CHK_Data_Transfer_Done();
        if(CHK_ECC_Done()){
//			dbprintf("\nECC-PN:%bx %bx ",tc_PageNum,tc_FPage);
            if(!tbt_Backup){
               DEVICE_REG[0x00] |=0x10;							// reset nand module because un-complete flash read process
               return 1;
            }
	    }
        CPU_REG[0x19] = Device_Read_SRAM_Index.BY[0];			// 080411 Joyce add for auotcount function fail
		CPU_REG[0x18] = Device_Read_SRAM_Index.BY[1];			// 080411 Joyce add for auotcount function fail	
        tc_PageNum -=2; 
    }
    CHK_NandAutocmp_Done();
	FLASH_REG[0xA1] = 0x00;
	return 0;	    
} 

/*********************************************************************************
* NAME:                    Banking_NAND_InAddr_To_Column_NAND(UBYTE tc_InAddr)
* DESCRIPTION:
*********************************************************************************/
void Banking_NAND_InAddr_To_Column_NAND(UBYTE tc_InAddr) {
	
	unionWORD tw_FColAddr;
    if((FLASH_REG[0xA5]&0x03)==0x02){ //24bit
        tw_FColAddr.WD =  (U16)(tc_InAddr>>1)*1070;//(1024+46)        
    }
    else{                             //16bit
       tw_FColAddr.WD =  (U16)(tc_InAddr>>1)*1056;//(1024+32)
    }   
	FLASH_REG[0x0C] = tw_FColAddr.BY[1];
	FLASH_REG[0x0B] = tw_FColAddr.BY[0]; 
	
	
	
	
	
}

/*****************************************************************************************************************************
* NAME: Banking_NAND_FBlock_FPage_FBank_To_PhyAddr_NAND(UBYTE tc_FPage, UBYTE tc_FBank, UBYTE tc_Block1, UBYTE tc_Block0)
* DESCRIPTION:
*******************************************************************************************************************************/
void Banking_NAND_FBlock_FPage_FBank_To_PhyAddr_NAND(UBYTE tc_FPage, UBYTE tc_FBank, UBYTE tc_Block1, UBYTE tc_Block0) {
	U8 tc_FMAP;
	tc_FBank=0;
	DEVICE_REG[0x0F] =0x00;   
	if(NAND_INFO.cFMAP&0xF0){// By MCU 
    	tc_FMAP = (NAND_INFO.cFMAP&0xF0)>>4;
    	switch(tc_FMAP){
    		case 1:    	// 256 page  block 
        		FLASH_REG[0x0A]= tc_FPage;  
        		FLASH_REG[0x09]= tc_Block1;
        		FLASH_REG[0x08]= tc_Block0;        		
				break;
			default: 
				break;
		}	              
    }
    else{				// by HW    	
    	FLASH_REG[0x0A] = tc_FPage;                                             // FPage
	    FLASH_REG[0x09] = tc_Block1;                                      // FBlock_L 
	    FLASH_REG[0x08] = tc_Block0;                                      // FBlock_H
		FLASH_REG[0x1C] = 0;                                             // FBank  
	}
	if(NAND_INFO.cADDR_CYCLE==0x04){	// 4 cycle addr
		FLASH_REG[0x01] = 0x30|0x01;
	}
	else{
		FLASH_REG[0x01] = (0x40|0x01); 	// 5 cycle addr
	}	  
	

}

