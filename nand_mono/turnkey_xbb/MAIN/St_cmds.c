
#include "SPDA2K.h"
/***************************************************************************
* NAME:         void SelfTestCmd (void) - 0xFE
* DESCRIPTION:
*       This routine is to Execute the LAA006's Self-Test Command.
*
***************************************************************************/
void SelfTestCmd(void) {
	gc_bCSWStatus = 0x00;
	gc_ST_ErrorCode = 0;
	switch (COMD_IN[7]) {
		case	ASK_PARA:   //0x02
			USB_BUF_Bank0[0x00] = 'L';
			USB_BUF_Bank0[0x01] = 'A';
			USB_BUF_Bank0[0x02] = 'A';
			USB_BUF_Bank0[0x03] = '0';
			USB_BUF_Bank0[0x04] = '2';
			USB_BUF_Bank0[0x05] = '6';
			USB_BUF_Bank0[0x06] = '-';
			USB_BUF_Bank0[0x07] = ' ';
			USB_BUF_Bank0[0x08] = 'B';
			USB_BUF_Bank0[0x09] = 'a';
			USB_BUF_Bank0[0x0A] = 'n';
			USB_BUF_Bank0[0x0B] = 'k';
			USB_BUF_Bank0[0x0C] = ' ';
			USB_BUF_Bank0[0x0D] = '1';
			USB_BUF_Bank0[0x0E] = '0';
			USB_BUF_Bank0[0x0F] = '0';
			USB_BUF_Bank0[0x10] = '5';
			USB_BUF_Bank0[0x11] = '1';
			USB_BUF_Bank0[0x12] = '6';
			USB_BUF_Bank0[0x13] = ' ';
			USB_BUF_Bank0[0x14] = NAND_INFO.cDEVICE_NUM;
			USB_BUF_Bank0[0x15] = (UBYTE)NAND_INFO.wPAGE_NUM;
			USB_BUF_Bank0[0x16] = (UBYTE)(NAND_INFO.wBLOCK_NUM>> 8);
			USB_BUF_Bank0[0x17] = (UBYTE)(NAND_INFO.wBLOCK_NUM);
			USB_BUF_Bank0[0x18] = NAND_INFO.cPLANE_NUM;
			USB_BUF_Bank0[0x19] = NAND_INFO.cINADDRMAX;			
			FlashReset();
			DEVICE_REG[0x0F] =0x00;
			CHK_Flash_Ready();
			FLASH_REG [0x00]    = 0x00;    //強迫進IO Mode 			
			DEVICE_REG[0x23]    = 0x0A;	//CLE = '1', ALE = '0', nWP = '1', nCE = '0'
			DEVICE_REG[0x20]    = 0x90;
			DEVICE_REG[0x23]    = 0x06;	//CLE = '0', ALE = '1', nWP = '1', nCE = '0'
			DEVICE_REG[0x20]    = 0x00;
			DEVICE_REG[0x23]    = 0x02;	//CLE = '0', ALE = '0', nWP = '1', nCE = '0'
			USB_BUF_Bank0[0x1A] = DEVICE_REG[0x20];	//Maker Code
			gc_Global_I = 0;
			USB_BUF_Bank0[0x1B] = DEVICE_REG[0x20];	//Device Code
			gc_Global_I = 0;
			USB_BUF_Bank0[0x1C] = DEVICE_REG[0x20];	//多取一些預防新的Flash
			gc_Global_I = 0;
			USB_BUF_Bank0[0x1D] = DEVICE_REG[0x20];
			gc_Global_I = 0;
			USB_BUF_Bank0[0x1E] = DEVICE_REG[0x20];
			FLASH_REG [0x00]    = 0x01;    //進ACC Mode 
			USB_BUF_Bank0[0x20] = 1;
			USB_BUF_Bank0[0X21] = (UBYTE)(SYS_ZONE.wZONE_SIZE>>8);
			USB_BUF_Bank0[0x22] = (UBYTE)(SYS_ZONE.wZONE_SIZE);
			USB_BUF_Bank0[0x23] = NAND_INFO.cFMAP;	//Bit 4~7 表示HW不支持需要使用SW方式轉換		
	        USB_BUF_Bank0[0x24]	= 0;
	        USB_BUF_Bank0[0x25] = NAND_INFO.cSUPPORT_MULT_PLANE;
	        USB_BUF_Bank0[0x26] = NAND_INFO.cFLASH_TYPE;//0:SLC 1:MLC, 2:TLC
			USB_BUF_Bank0[0x27] = gbt_newFlash;		// 當ID table 中沒有支援
	        //-------------------------------------//  081212 Jay for flash capacity         
            USB_BUF_Bank0[0x30] = Capacity.BY[0];
            USB_BUF_Bank0[0x31] = Capacity.BY[1];
            USB_BUF_Bank0[0x32] = Capacity.BY[2];
            USB_BUF_Bank0[0x33] = Capacity.BY[3];        	
			//-- 2010 03 new LDZ039  parameter  --//
			USB_BUF_Bank0[0x40] = (UBYTE)(NAND_INFO.wPAGE_NUM>>8);
			USB_BUF_Bank0[0X41] = NAND_INFO.cECC_MODE;// 0-->24 1-->16
			USB_BUF_Bank0[0x42] = NAND_INFO.cECC_NUM;
			USB_BUF_Bank0[0x43] = 0xFF;//temp			
	        USB_BUF_Bank0[0x44]	= 0xFF;//temp
	        USB_BUF_Bank0[0x45] = 0xFF;//temp
	        USB_BUF_Bank0[0x46] = 0xFF;//temp
			USB_BUF_Bank0[0x47] = 0xFF;//temp		
			
	        //-------------------------------------//
			USB_REG[0x10]       = 0x01;	//BulkInEn auto turn-on(dma over)
			USB_REG[0x16]       = 0x02;
			USB_REG[0x15]       = 0x00;
			USB_REG[0xA1]       = 0x01;
			while (USB_REG[0xA1] & 0x01);
			USB_REG[0x10]       = 0x00;	//BulkInEn auto turn-off(dma over)
			USB_REG[0xE9]       = 0x01;
			break;

		default:
			gc_bCSWStatus = 0x01;
			gc_RequestSenseCode = 0x03;		//參數不支援
			break;
	}
	if (gc_ST_ErrorCode) {
		gc_bCSWStatus = 0x01;
		gc_RequestSenseCode = 0x88;
	}
}



