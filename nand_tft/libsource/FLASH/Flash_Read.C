/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	flash.c
 *	\brief	
 *
 *	\author	jay
 */

#include "SPDA2K.h"

/*********************************************************************************
* NAME:                   void NAND_FlashReadSector_Large(void)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/
void NAND_FlashReadSector_Large(void) {
	data	bit	tbt_DoSendCommand=0;
	data	bit	tbt_Extend_Found,tbt_Random_Found,tbt_Find_D_SBlock;
	data	U8	tc_Extend_Count,tc_Random_Count; 
	data	U8	tc_Offset; 
   	xdata	U8	tc_Buf_InAddr;
    data	U8	tc_i;
    data	U8	tc_RealInAddr;   
    data	U8	tc_BigSector_Cnt,tc_SmallSector_Cnt;
    data	U16	tw_TotalESPage;
    data	U8	tc_Recover_Type;
    data	U8	tc_Need_Recover;
    data	U8	tc_OxFF_Cnt;
   
    USB_PlugDetect();											// 081014 Jay add for usb plug out
	if(!gbt_USB_Detected){
		return ;
	}
    DEVICE_REG[0x00]|=0x10;  //for interface switch									// 將Flash module reset
	DEVICE_REG[0x00]&=0xEF;
	DEVICE_REG[0x00]= 0x01;
	Flash_State_Initial();
	//dbprintf("\n Read LBA:%lX , CNT:%x",LBA.LW,TotalXferPageNum.WD);
	Check_LoadMap();	
	NAND_HPage_To_SBlock_SPage();
	APort_FBlock[0] = Mapping_Table[HBlock.WD];
	tbt_Extend_Found = 0;   
	tbt_Random_Found = 0;
	tc_Recover_Type  = 0;  
	tc_Need_Recover  = 0;
	
	LoadBlock_From_MAP(0);	
	for(tc_Extend_Count=0;tc_Extend_Count<Extend_NUM;tc_Extend_Count++){
		if(Mapping_Table[EINX+tc_Extend_Count]==HBlock.WD){
			tbt_Extend_Found = 1;
			Load_E_D_Block_From_MAP(0,tc_Extend_Count);
			gc_Q1_SBlock = Mapping_Table[Q1_INX+tc_Extend_Count]>>8;
			gc_E_SBlock = Mapping_Table[Q2_INX+tc_Extend_Count]>>8;
			gc_E_SPage  = (U8)Mapping_Table[ Q2_INX+tc_Extend_Count];		
			tw_TotalESPage = ((U16)gc_E_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_E_SPage;
			
			break;
		}
	}
	if(!tbt_Extend_Found){
		for(tc_Random_Count=0;tc_Random_Count<MAX_RANDOM_HBLOCK;tc_Random_Count++){
			if(Wear_Table[RINX+tc_Random_Count]==HBlock.WD){
				tbt_Random_Found= 1;				   
				break;
			}
		}
	}
	if(tbt_Extend_Found){
		if(HPage.WD>= tw_TotalESPage){
			APort_FBlock[0] = APort_LBlock[0];
			gw_TargetPage = HPage.WD;
			tc_Recover_Type =0x10;
		}		
		else{
			tc_Offset= (tc_Extend_Count* MAX_DUMMY_PAGE); 
			tbt_Find_D_SBlock =0;
			for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
				if(Mapping_Table[D_HPAGE+tc_Offset+tc_i]!=0xFFFF){
					tbt_Find_D_SBlock=1;
					break;
				}
			}
			if(tbt_Find_D_SBlock){				
				NAND_E_D_ReadSector(tc_Extend_Count);//轉成一般的RW
				if(!gbt_USB_Detected){
					goto USB_Plug_Out_R;					
				}				
				if(HPage.WD==H_INFO.wHBLOCKPAGE){//0520 Jay add 
					return;
				}				
				APort_FBlock[0] = APort_LBlock[0];
				gw_TargetPage = HPage.WD;	
				tc_Recover_Type =0x10; 	
			}		
			else{
				if(gc_SBlock<gc_Q1_SBlock){			
					APort_FBlock[0] =APort_LBlock[0];
					tc_Recover_Type =0x10;
				}
				else{			
					APort_FBlock[0] = APort_EBlock[0];
					tc_Recover_Type =0x20;
				}
				gw_TargetPage = HPage.WD;	
			}			
		}		
	}
	else if (tbt_Random_Found){		
		if(CHK_RandomPage_Exist(tc_Random_Count,HPage.WD)){
			tc_Offset =tc_Random_Count*MAX_RANDOM_HPAGE;
			gw_TargetPage =Mapping_Table[R_PAGE+tc_Offset+gc_NowIndex];
			APort_FBlock[0] = APort_RBlock[0];
			tc_Recover_Type =0x40;
		}
		else{						
			APort_FBlock[0] = APort_LBlock[0];
			gw_TargetPage = HPage.WD;
			tc_Recover_Type =0x10;
		}
		
	}
	else{
		APort_FBlock[0] =APort_LBlock[0];
		gw_TargetPage = HPage.WD;
		tc_Recover_Type =0x10;
	}
	tc_Buf_InAddr =gc_HInAddr&0x01;
	if(tc_Buf_InAddr){
		tc_RealInAddr = gc_HInAddr&0xFE;
		NAND_SBlock_SPage_To_FDBP();	
		FDBP.cFInAddr =tc_RealInAddr;
		Device_Read_SRAM_Index.BY[0] = USB_BUF_Bank0_HIGH;			// 080104 joyce add
		Device_Read_SRAM_Index.BY[1] = 0x00;
		Flash_ReadPage();
		#if  EN_RECOVER_BLOCK
		if(gc_ECC_Error_Bit>gc_MAX_ERR_BIT_NUM){			
			tc_OxFF_Cnt=0;
			for(tc_i=0;tc_i<4;tc_i++){
				if(FLASH_REG[0x60+tc_i] ==0xFF){
					tc_OxFF_Cnt++;
				}
			}
			if(tc_OxFF_Cnt<=2){
				//dbprintf("\n 1 ERR_bit:%bx ,T:%bx ",gc_ECC_Error_Bit,tc_Recover_Type);
				//dbprintf("\n BLK:%x,Page:%x,InAddr:%bx",FDBP.wFBlock,FDBP.wFPage,gc_HInAddr);
				tc_Need_Recover |= tc_Recover_Type;
				if(gc_ECC_Error_Bit ==25){
					tc_Need_Recover |=0x02;
				}
				else{
					tc_Need_Recover |=0x01;
				}
			}		
		}
		#endif 
		USB_REG[0xC2] &= 0xEF;						//Bulk-In Tx done
		USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
		USB_REG[0xE9] = 0x01;							//Clear Bulk-In(EP1) Buffer.
		USB_REG[0x2B] = 0x01;
		USB_REG[0x16] = 0x02;	
		USB_REG[0x15] = 0x00;
		USB_REG[0xA1] = 0x01;							//Allow Bulk-In Tx.
		while (USB_REG[0xA1]&0x01){								//Bulk-In Tx done
			if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
				gbt_USB_Detected = 0;
				goto USB_Plug_Out_R;
				break;
			}			
		}		
		USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
		TotalXferPageNum.WD--;	
		gc_HInAddr++;
		if(gc_HInAddr==NAND_INFO.cINADDRMAX){
			gc_HInAddr=0;
			HPage.WD++;
			NAND_HPage_To_SBlock_SPage();
			tbt_DoSendCommand=1;
		}
		if(HPage.WD==H_INFO.wHBLOCKPAGE){
			return ;
		}
		else{// chk page status
			if(tbt_Extend_Found){
				if(HPage.WD>= tw_TotalESPage){
					APort_FBlock[0] = APort_LBlock[0];
					gw_TargetPage = HPage.WD;
					tc_Recover_Type =0x10;
				}	
				else{
					if(gc_SBlock<gc_Q1_SBlock){			
						APort_FBlock[0] =APort_LBlock[0];
						tc_Recover_Type =0x10;
					}
					else{			
						APort_FBlock[0] = APort_EBlock[0];
						tc_Recover_Type =0x20;
					}				
					gw_TargetPage = HPage.WD;			
				}
			}
			else if(tbt_Random_Found){
				if(CHK_RandomPage_Exist(tc_Random_Count,HPage.WD)){
					tc_Offset =tc_Random_Count*MAX_RANDOM_HPAGE;
					gw_TargetPage =Mapping_Table[R_PAGE+tc_Offset+gc_NowIndex];
					APort_FBlock[0] = APort_RBlock[0];
					tc_Recover_Type =0x40;
				}
				else{						
					APort_FBlock[0] = APort_LBlock[0];
					gw_TargetPage = HPage.WD;
					tc_Recover_Type =0x10;
				}
			}
			else{
				APort_FBlock[0] = APort_LBlock[0];
				gw_TargetPage = HPage.WD;
				tc_Recover_Type =0x10;
			}	
				
		}
	}
	//-------- 之後都是 以1024 為單位------------//
	tc_SmallSector_Cnt= TotalXferPageNum.WD&0x01;
	tc_BigSector_Cnt= TotalXferPageNum.WD>>1;
	if(tc_BigSector_Cnt){		
		gbt_OddTemp =0;
		gc_Flash_CMD1 = 0x00;    
		gc_Flash_CMD2 = 0x30;    
		gc_Flash_CMD_Type = 0x01;									// Read -->CMD1+Addr+CMD2
		NAND_SBlock_SPage_To_FDBP();	
		FDBP.cFInAddr = gc_HInAddr;		
		TRIGGER_FLASH_CMD_COL_ADDR_CMD();	
		ENA_ECC_Decode();
		Device_Xfer_DataLength.WD = 0x03FF; 
		Device_Read_SRAM_Index.BY[0] = USB_BUF_Bank0_HIGH;			// 080104 joyce add
		Device_Read_SRAM_Index.BY[1] = 0x00;
		SET_DEVICE_READ();	
		if(gc_HInAddr==(NAND_INFO.cINADDRMAX-2)){
			FLASH_REG[0x1A] =0x11;
		}
		else{
			FLASH_REG[0x1A] =0x10;
		}
		if(tc_BigSector_Cnt==1){// last dma 
			FLASH_REG[0x1A] =0x11;
		}		
		CHK_Flash_Ready();
		FLASH_REG[0xA1]  = 0x01;								
		CHK_Flash_Ready();
		Trigger_Data_Transfer();
		tc_BigSector_Cnt--;
		TotalXferPageNum.WD-=2;
		gc_HInAddr+=2;
		tbt_DoSendCommand=0;	
		while(tc_BigSector_Cnt){
			if(gc_HInAddr==NAND_INFO.cINADDRMAX){
				gc_HInAddr=0;
				HPage.WD++;
				NAND_HPage_To_SBlock_SPage();
				tbt_DoSendCommand=1;
				
			}
			if(HPage.WD==H_INFO.wHBLOCKPAGE){
				break;
			}
			else{// chk page status				
				if(tbt_Extend_Found){
					if(HPage.WD>= tw_TotalESPage){
						APort_FBlock[0] = APort_LBlock[0];
						gw_TargetPage = HPage.WD;
					}	
					else{
						if(gc_SBlock<gc_Q1_SBlock){
							APort_FBlock[0] =APort_LBlock[0];
						}
						else{
							APort_FBlock[0] = APort_EBlock[0];
						}
						gw_TargetPage = HPage.WD;			
					}
				}
				else if(tbt_Random_Found){
					if(CHK_RandomPage_Exist(tc_Random_Count,HPage.WD)){
						tc_Offset =tc_Random_Count*MAX_RANDOM_HPAGE;
						gw_TargetPage =Mapping_Table[R_PAGE+tc_Offset+gc_NowIndex];
						APort_FBlock[0] = APort_RBlock[0];
					}
					else{						
						APort_FBlock[0] = APort_LBlock[0];
						gw_TargetPage = HPage.WD;
					}
				}
				else{
					APort_FBlock[0] = APort_LBlock[0];
					gw_TargetPage = HPage.WD;
				}	
					
			}
			CHK_Data_Transfer_Done();			
			CHK_ECC_Done();
			#if  EN_RECOVER_BLOCK
			if(gc_ECC_Error_Bit>gc_MAX_ERR_BIT_NUM){			
				tc_OxFF_Cnt=0;
				for(tc_i=0;tc_i<4;tc_i++){
					if(FLASH_REG[0x60+tc_i] ==0xFF){
						tc_OxFF_Cnt++;
					}
				}
				if(tc_OxFF_Cnt<=2){
					//dbprintf("\n 2 ERR_bit:%bx ,T:%bx ",gc_ECC_Error_Bit,tc_Recover_Type);
					//dbprintf("\n BLK:%x,Page:%x,InAddr:%bx",FDBP.wFBlock,FDBP.wFPage,gc_HInAddr);
					tc_Need_Recover |= tc_Recover_Type;
					if(gc_ECC_Error_Bit ==25){
						tc_Need_Recover |=0x02;
					}
					else{
						tc_Need_Recover |=0x01;
					}
				}		
			}
			#endif 
			
			 
			
			if(tbt_DoSendCommand){			
				gc_Flash_CMD1 = 0x00;    
				gc_Flash_CMD2 = 0x30;    
				gc_Flash_CMD_Type = 0x01;									// Read -->CMD1+Addr+CMD2
				NAND_SBlock_SPage_To_FDBP();	
				TRIGGER_FLASH_CMD_COL_ADDR_CMD();				
			}			
			if(!gbt_OddTemp) {	// transfer data to usb 	 // DMA前需確認USB 不動作			
				while (USB_REG[0xA2]&0x01){
					if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
						gbt_USB_Detected = 0;
						goto USB_Plug_Out_R;
						break;
					}				
				}			
				USB_REG[0xC2] &= 0xEF;						//Bulk-In Tx done
				USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
				USB_REG[0xE9] = 0x01;							//Clear Bulk-In(EP1) Buffer.
				USB_REG[0x2B] = 0x00;
				USB_REG[0x16] = 0x04;	
				USB_REG[0x15] = 0x00;
				USB_REG[0xA1] = 0x01;							//Allow Bulk-In Tx.
				CPU_REG[0x15] = USB_BUF_Bank1_HIGH;				// 080104 joyce add
				//if (!gbt_USBHostIsHighSpeed) { //USB 1.1
				//	while (USB_REG[0xA1]&0x01);								//Bulk-In Tx done
				//	USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
				//}
			}			
			else {
				while(USB_REG[0xA1]&0x01){
					if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
						gbt_USB_Detected = 0;
						goto USB_Plug_Out_R;
						break;
					}			
				}
				USB_REG[0xC2] &= 0xFE;	
				USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
				USB_REG[0xE9] = 0x10;							//Clear Bulk-In(EP1) Buffer.
				USB_REG[0x2b] = 0x00;
				USB_REG[0x18] = 0x04;							//1024Byte
				USB_REG[0x17] = 0x00;
				USB_REG[0xA2] = 0x01;							//Allow Bulk-In Tx.				
				CPU_REG[0x15] = USB_BUF_Bank0_HIGH;				// 080104 joyce add
				//if(!gbt_USBHostIsHighSpeed){
				//	while (USB_REG[0xA2]&0x01);							//Bulk-In Tx done
				//	USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
				//}			
			}
			if(gc_HInAddr==(NAND_INFO.cINADDRMAX-2)){	
				FLASH_REG[0x1A] =0x11;
			}
			else{				
				FLASH_REG[0x1A] =0x10;			}
			if(tc_BigSector_Cnt==1){// last dma 				
				FLASH_REG[0x1A] =0x11;
			}		
			if(tbt_DoSendCommand){// Need to  chk ready
				tbt_DoSendCommand=0;
				CHK_Flash_Ready();
			}
			if(!gbt_OddTemp){// chk usb done
				while (USB_REG[0xA1]&0x01){
					if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
						gbt_USB_Detected = 0;
						goto USB_Plug_Out_R;
						break;
					}											//Bulk-In Tx done
				}				
				USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
			}
			else{
				while (USB_REG[0xA2]&0x01){							//Bulk-In Tx done
					if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
						gbt_USB_Detected = 0;
						goto USB_Plug_Out_R;
						break;
					}	
				}
				USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
			}			
			gbt_OddTemp=!gbt_OddTemp;
			FLASH_REG[0xA1]  = 0x01;								//Start Decode RS-Code
			Trigger_Data_Transfer();
			gc_HInAddr+=2;		
			tc_BigSector_Cnt--;
			TotalXferPageNum.WD-=2;
			
		}		
		CHK_Data_Transfer_Done();			
		DMA_REG[0xC0] = 0x00;										//Clear DMA Xfer done interrupt
		CHK_ECC_Done();		
		#if  EN_RECOVER_BLOCK
		if(gc_ECC_Error_Bit>gc_MAX_ERR_BIT_NUM){			
		
			tc_OxFF_Cnt=0;
			for(tc_i=0;tc_i<4;tc_i++){
				if(FLASH_REG[0x60+tc_i] ==0xFF){
					tc_OxFF_Cnt++;
				}
			}
			if(tc_OxFF_Cnt<=2){
				//dbprintf("\n 3 ERR_bit:%bx ,T:%bx ",gc_ECC_Error_Bit,tc_Recover_Type);
				//dbprintf("\n BLK:%x,Page:%x,InAddr:%bx",FDBP.wFBlock,FDBP.wFPage,gc_HInAddr);
				tc_Need_Recover |= tc_Recover_Type;
				if(gc_ECC_Error_Bit ==25){
					tc_Need_Recover |=0x02;
				}
				else{
					tc_Need_Recover |=0x01;
				}
			}		
		}
		#endif 
		if(!gbt_OddTemp) {	// transfer data to usb 			
			while (USB_REG[0xA2]&0x01){
				if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
					gbt_USB_Detected = 0;
					goto USB_Plug_Out_R;
					break;
				}
					
			}		
			USB_REG[0xC2] &= 0xEF;						//Bulk-In Tx done
			USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
			USB_REG[0xE9] = 0x01;							//Clear Bulk-In(EP1) Buffer.
			USB_REG[0x2B] = 0x00;
			USB_REG[0x16] = 0x04;	
			USB_REG[0x15] = 0x00;
			USB_REG[0xA1] = 0x01;							//Allow Bulk-In Tx.
			while (USB_REG[0xA1]&0x01){								//Bulk-In Tx done
				if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
					gbt_USB_Detected = 0;
					goto USB_Plug_Out_R;
					break;
				}
			}			
			USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
				
		}			
		else {
			
			while (USB_REG[0xA1]&0x01){
				if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
					gbt_USB_Detected = 0;
					goto USB_Plug_Out_R;
					break;
				}
			}			
			
			USB_REG[0xC2] &= 0xFE;	
			USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
			USB_REG[0xE9] = 0x10;							//Clear Bulk-In(EP1) Buffer.
			USB_REG[0x2b] = 0x00;
			USB_REG[0x18] = 0x04;							//512Byte
			USB_REG[0x17] = 0x00;
			USB_REG[0xA2] = 0x01;							//Allow Bulk-In Tx.
			while (USB_REG[0xA2]&0x01){							//Bulk-In Tx done
				if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
					gbt_USB_Detected = 0;
					goto USB_Plug_Out_R;
					break;
				}
			}			
			USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
		}
		
	}
	//-------- 之後都是 以512 為單位------------//	
	if(HPage.WD==H_INFO.wHBLOCKPAGE){		
		return ;
	}	
	if(tc_SmallSector_Cnt){
		if(gc_HInAddr==NAND_INFO.cINADDRMAX){
			gc_HInAddr=0;
			HPage.WD++;
			NAND_SBlock_SPage_To_FDBP();				
		}
		if(HPage.WD==H_INFO.wHBLOCKPAGE){		
			return;
		}
		else{// chk page status
			if(tbt_Extend_Found){
				if(HPage.WD>= tw_TotalESPage){
					APort_FBlock[0] = APort_LBlock[0];
					gw_TargetPage = HPage.WD;
					tc_Recover_Type=0x10;
				}	
				else{
					if(gc_SBlock<gc_Q1_SBlock){
						APort_FBlock[0] =APort_LBlock[0];
						tc_Recover_Type =0x10;
					}
					else{
						APort_FBlock[0] = APort_EBlock[0];
						tc_Recover_Type =0x20;
					}
					gw_TargetPage = HPage.WD;			
				}
			}
			else if(tbt_Random_Found){			
				if(CHK_RandomPage_Exist(tc_Random_Count,HPage.WD)){
					tc_Offset =tc_Random_Count*MAX_RANDOM_HPAGE;
					gw_TargetPage =Mapping_Table[R_PAGE+tc_Offset+gc_NowIndex];
					APort_FBlock[0] = APort_RBlock[0];
					tc_Recover_Type =0x40;
				}
				else{						
					APort_FBlock[0] = APort_LBlock[0];
					gw_TargetPage = HPage.WD;
					tc_Recover_Type =0x10;
				}
			}
			else{
				APort_FBlock[0] = APort_LBlock[0];
				tc_Recover_Type =0x10;
				gw_TargetPage = HPage.WD;
			}	
		}
		tc_RealInAddr = gc_HInAddr&0xFE;
		NAND_SBlock_SPage_To_FDBP();	
		FDBP.cFInAddr =tc_RealInAddr;
		Device_Read_SRAM_Index.BY[0] = USB_BUF_Bank0_HIGH;			// 080104 joyce add
		Device_Read_SRAM_Index.BY[1] = 0x00;
		Flash_ReadPage();
		#if  EN_RECOVER_BLOCK
		if(gc_ECC_Error_Bit>gc_MAX_ERR_BIT_NUM){			
			
			tc_OxFF_Cnt=0;
			for(tc_i=0;tc_i<4;tc_i++){
				if(FLASH_REG[0x60+tc_i] ==0xFF){
					tc_OxFF_Cnt++;
				}
			}
			if(tc_OxFF_Cnt<=2){
				//dbprintf("\n 4 ERR_bit:%bx ,T:%bx ",gc_ECC_Error_Bit,tc_Recover_Type);
				//dbprintf("\n BLK:%x,Page:%x,InAddr:%bx",FDBP.wFBlock,FDBP.wFPage,gc_HInAddr);
				tc_Need_Recover |= tc_Recover_Type;
				if(gc_ECC_Error_Bit ==25){
					tc_Need_Recover |=0x02;
				}
				else{
					tc_Need_Recover |=0x01;
				}
			}		
		}
		#endif 
		USB_REG[0xC2] &= 0xEF;						//Bulk-In Tx done
		USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
		USB_REG[0xE9] = 0x01;							//Clear Bulk-In(EP1) Buffer.
		USB_REG[0x2B] = 0x00;
		USB_REG[0x16] = 0x02;	
		USB_REG[0x15] = 0x00;
		USB_REG[0xA1] = 0x01;							//Allow Bulk-In Tx.
		while (USB_REG[0xA1]&0x01){
			if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
				gbt_USB_Detected = 0;
				goto USB_Plug_Out_R;
				break;
			}
		}											//Bulk-In Tx done
		USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
		TotalXferPageNum.WD--;	
	}
	#if  EN_RECOVER_BLOCK

	if(tc_Need_Recover&0x10){
//		dbprintf("\n Recover Block H:%x RN:%bx",HBlock.WD,tc_Need_Recover);
		//P3_0=0;P3_0=1;
		NAND_Recover_Block(HBlock.WD,tc_Need_Recover);
		BMT_KEY[gw_NowBMTPoint]=1; 
	}	
	#endif 
USB_Plug_Out_R://081113 Jay add
	if(!gbt_USB_Detected){		
		DMA_REG[0xC0] =0x00;
		DEVICE_REG[0x00]|=0x10;										// 將Flash module reset
		DEVICE_REG[0x00]&=0xEF;
		DEVICE_REG[0x00]= 0x01;
		FLASH_REG[0x06] = 0xFF;
		FLASH_REG[0x04] = 0x08;
		CHK_Flash_Ready();
		FLASH_REG[0x03] = 0x01;	
	}		

}

/*********************************************************************************
* NAME:                   void NAND_E_D_ReadSector(tc_Extend_Count);
* DESCRIPTION:
*********************************************************************************/
void NAND_E_D_ReadSector(U8 tc_Extend_Count){
	data	bit		tbt_DoSendCommand=0;
	data	bit		tbt_Find_D_SBlock;
   	xdata	U8    tc_Buf_InAddr;
    data	U8    tc_RealInAddr;  
	data	U8    tc_BigSector_Cnt,tc_SmallSector_Cnt;
	data	U16   tw_TotalESPage;
	NAND_HPage_To_SBlock_SPage();
	gc_E_SBlock = Mapping_Table[Q2_INX+tc_Extend_Count]>>8;
	gc_E_SPage  = (U8)Mapping_Table[ Q2_INX+tc_Extend_Count];		
	gc_Q1_SBlock =  Mapping_Table[Q1_INX+tc_Extend_Count]>>8;
	tw_TotalESPage = ((U16)gc_E_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_E_SPage;
	if(CHK_DSBlock_Exist_Or_New(tc_Extend_Count,gc_SBlock,0)){//no dummy
		
		tbt_Find_D_SBlock=0;
		if(gc_SBlock<gc_Q1_SBlock){			
			APort_FBlock[0] =APort_LBlock[0];
		}
		else{			
			APort_FBlock[0] = APort_EBlock[0];
		}				
		gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;	
		
	}
	else{
		tbt_Find_D_SBlock=1;
		if((gc_SPage>=SQTable.BY[0])&&(gc_SPage<SQTable.BY[1])){
			
			APort_FBlock[0] =APort_DBlock[0];				
			gw_TargetPage = ((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+(gc_SPage-SQTable.BY[0]);	
		}	
		else{			
			//APort_FBlock[0] =APort_EBlock[0];				
			if(gc_SBlock<gc_Q1_SBlock){// 0524 Jay add			
				APort_FBlock[0] =APort_LBlock[0];
			}
			else{			
			APort_FBlock[0] =APort_EBlock[0];				
			}			
			gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
		}
	}	
	tc_Buf_InAddr =gc_HInAddr&0x01;
	
	if(tc_Buf_InAddr){
		tc_RealInAddr = gc_HInAddr&0xFE;
		NAND_SBlock_SPage_To_FDBP();	
		FDBP.cFInAddr =tc_RealInAddr;
		Device_Read_SRAM_Index.BY[0] = USB_BUF_Bank0_HIGH;			// 080104 joyce add
		Device_Read_SRAM_Index.BY[1] = 0x00;
		Flash_ReadPage();
		USB_REG[0xC2] &= 0xEF;						//Bulk-In Tx done
		USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
		USB_REG[0xE9] = 0x01;							//Clear Bulk-In(EP1) Buffer.
		USB_REG[0x2B] = 0x01;
		USB_REG[0x16] = 0x02;	
		USB_REG[0x15] = 0x00;
		USB_REG[0xA1] = 0x01;							//Allow Bulk-In Tx.
		while (USB_REG[0xA1]&0x01){								//Bulk-In Tx done
			if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
				gbt_USB_Detected = 0;
				return;
			}			
		}		
		USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
		TotalXferPageNum.WD--;	
		gc_HInAddr++;
		if(gc_HInAddr==NAND_INFO.cINADDRMAX){
			tbt_DoSendCommand=1;
			gc_HInAddr=0;
			HPage.WD++;
			gc_SPage++;
			if(gc_SPage==H_INFO.cSMALLPAGE_NUM){
				gc_SPage=0;
				gc_SBlock++;
				if(CHK_DSBlock_Exist_Or_New(tc_Extend_Count,gc_SBlock,0)){// no dummy 	
					tbt_Find_D_SBlock=0;
				}
				else{
					tbt_Find_D_SBlock=1;
				}
			}			
		}
		if(HPage.WD==tw_TotalESPage){
			return;
		}
		else{
			if(tbt_Find_D_SBlock){// have dummy 	
				if((gc_SPage>=SQTable.BY[0])&&(gc_SPage<SQTable.BY[1])){
					APort_FBlock[0] =APort_DBlock[0];				
					gw_TargetPage = ((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+(gc_SPage-SQTable.BY[0]);	
				}	
				else{
					//APort_FBlock[0] =APort_EBlock[0];				
					if(gc_SBlock<gc_Q1_SBlock){// 0524 Jay add			
						APort_FBlock[0] =APort_LBlock[0];
					}
					else{			
					APort_FBlock[0] =APort_EBlock[0];				
					}			
					gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
				}
			}
			else{
				if(gc_SBlock<gc_Q1_SBlock){			
					APort_FBlock[0] =APort_LBlock[0];
				}
				else{			
					APort_FBlock[0] = APort_EBlock[0];
				}						
				gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
			}		
				
		}
	
	}
	//-------- 之後都是 以1024 為單位------------//
	tc_SmallSector_Cnt= TotalXferPageNum.WD&0x01;
	tc_BigSector_Cnt= TotalXferPageNum.WD>>1;	
	if(tc_BigSector_Cnt){
		
		gbt_OddTemp =0;
		gc_Flash_CMD1 = 0x00;    
		gc_Flash_CMD2 = 0x30;    
		gc_Flash_CMD_Type = 0x01;									// Read -->CMD1+Addr+CMD2
		NAND_SBlock_SPage_To_FDBP();	
		FDBP.cFInAddr = gc_HInAddr;		
		TRIGGER_FLASH_CMD_COL_ADDR_CMD();	
		ENA_ECC_Decode();
		Device_Xfer_DataLength.WD = 0x03FF; 
		Device_Read_SRAM_Index.BY[0] = USB_BUF_Bank0_HIGH;			// 080104 joyce add
		Device_Read_SRAM_Index.BY[1] = 0x00;
		SET_DEVICE_READ();	
		if(gc_HInAddr==(NAND_INFO.cINADDRMAX-2)){
			FLASH_REG[0x1A] =0x11;
		}
		else{
			FLASH_REG[0x1A] =0x10;
		}
		if(tc_BigSector_Cnt==1){// last dma 
			FLASH_REG[0x1A] =0x11;
		}		
		CHK_Flash_Ready();
		FLASH_REG[0xA1]  = 0x01;								//Start Decode RS-Code
		CHK_Flash_Ready();
		Trigger_Data_Transfer();
		tc_BigSector_Cnt--;
		TotalXferPageNum.WD-=2;
		gc_HInAddr+=2;
		tbt_DoSendCommand=0;	
		while(tc_BigSector_Cnt){
			if(gc_HInAddr==NAND_INFO.cINADDRMAX){
				tbt_DoSendCommand=1;
				gc_HInAddr=0;
				HPage.WD++;
				gc_SPage++;
				if(gc_SPage==H_INFO.cSMALLPAGE_NUM){
					gc_SPage=0;
					gc_SBlock++;
					if(CHK_DSBlock_Exist_Or_New(tc_Extend_Count,gc_SBlock,0)){// no dummy 	
						tbt_Find_D_SBlock=0;
					}
					else{
						tbt_Find_D_SBlock=1;
					}
				}			
			}
			if(HPage.WD==tw_TotalESPage){
				break;
			}
			else{
				if(tbt_Find_D_SBlock){// have dummy 	
					if((gc_SPage>=SQTable.BY[0])&&(gc_SPage<SQTable.BY[1])){
						APort_FBlock[0] =APort_DBlock[0];				
						gw_TargetPage = ((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+(gc_SPage-SQTable.BY[0]);	
					}	
					else{
						//APort_FBlock[0] =APort_EBlock[0];				
						if(gc_SBlock<gc_Q1_SBlock){// 0524 Jay add			
							APort_FBlock[0] =APort_LBlock[0];
						}
						else{			
						APort_FBlock[0] =APort_EBlock[0];				
						}			
						gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
					}
				}
				else{
					if(gc_SBlock<gc_Q1_SBlock){			
						APort_FBlock[0] =APort_LBlock[0];
					}
					else{			
						APort_FBlock[0] = APort_EBlock[0];
					}						
					gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
				}		
				
			}
			CHK_Data_Transfer_Done();			
			CHK_ECC_Done();
			if(tbt_DoSendCommand){			
				gc_Flash_CMD1 = 0x00;    
				gc_Flash_CMD2 = 0x30;    
				gc_Flash_CMD_Type = 0x01;									// Read -->CMD1+Addr+CMD2
				NAND_SBlock_SPage_To_FDBP();	
				TRIGGER_FLASH_CMD_COL_ADDR_CMD();				
			}	
			if(!gbt_OddTemp) {	// transfer data to usb 				
				while (USB_REG[0xA2]&0x01){
					if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
						gbt_USB_Detected = 0;
						return;
					}			
				}					
				USB_REG[0xC2] &= 0xEF;						//Bulk-In Tx done
				USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
				USB_REG[0xE9] = 0x01;							//Clear Bulk-In(EP1) Buffer.
				USB_REG[0x2B] = 0x00;
				USB_REG[0x16] = 0x04;	
				USB_REG[0x15] = 0x00;
				USB_REG[0xA1] = 0x01;							//Allow Bulk-In Tx.
				CPU_REG[0x15] = USB_BUF_Bank1_HIGH;				// 080104 joyce add
				//if (!gbt_USBHostIsHighSpeed) { //USB 1.1
				//	while (USB_REG[0xA1]&0x01);								//Bulk-In Tx done
				//	USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
				//}
			}			
			else {
				
				while(USB_REG[0xA1]&0x01){
					if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
						gbt_USB_Detected = 0;
						return;
					}			
				}				
				USB_REG[0xC2] &= 0xFE;	
				USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
				USB_REG[0xE9] = 0x10;							//Clear Bulk-In(EP1) Buffer.
				USB_REG[0x2b] = 0x00;
				USB_REG[0x18] = 0x04;							//512Byte
				USB_REG[0x17] = 0x00;
				USB_REG[0xA2] = 0x01;							//Allow Bulk-In Tx.				
				CPU_REG[0x15] = USB_BUF_Bank0_HIGH;				// 080104 joyce add
				//if(!gbt_USBHostIsHighSpeed){
				//	while (USB_REG[0xA2]&0x01);							//Bulk-In Tx done
				//	USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
				//}			
			}
			if(tbt_DoSendCommand){
				tbt_DoSendCommand=0;
				CHK_Flash_Ready();
			}
				
			if(!gbt_OddTemp){// chk usb done
				while (USB_REG[0xA1]&0x01){								//Bulk-In Tx done
					if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
						gbt_USB_Detected = 0;
						return;
					}			
				}			
				USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
			}
			else{
				while (USB_REG[0xA2]&0x01){							//Bulk-In Tx done
					if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
						gbt_USB_Detected = 0;
						return;
					}			
				}		
				
				USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
			}
			if(gc_HInAddr==(NAND_INFO.cINADDRMAX-2)){	
				FLASH_REG[0x1A] =0x11;
			}
			else{				
				FLASH_REG[0x1A] =0x10;			}
			if(tc_BigSector_Cnt==1){// last dma 				
				FLASH_REG[0x1A] =0x11;
			}		
			gbt_OddTemp=!gbt_OddTemp;
			FLASH_REG[0xA1]  = 0x01;								//Start Decode RS-Code
			Trigger_Data_Transfer();		
			tc_BigSector_Cnt--;
			TotalXferPageNum.WD-=2;
			gc_HInAddr+=2;
			
		}
		
		CHK_Data_Transfer_Done();			
		DMA_REG[0xC0] = 0x00;										//Clear DMA Xfer done interrupt
		CHK_ECC_Done();		
		if(!gbt_OddTemp) {	// transfer data to usb 			
			while (USB_REG[0xA2]&0x01){
				if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
					gbt_USB_Detected = 0;
					return;
				}			
			}					
			USB_REG[0xC2] &= 0xEF;						//Bulk-In Tx done
			USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
			USB_REG[0xE9] = 0x01;							//Clear Bulk-In(EP1) Buffer.
			USB_REG[0x2B] = 0x00;
			USB_REG[0x16] = 0x04;	
			USB_REG[0x15] = 0x00;
			USB_REG[0xA1] = 0x01;							//Allow Bulk-In Tx.
			while (USB_REG[0xA1]&0x01){								//Bulk-In Tx done
				if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
					gbt_USB_Detected = 0;
					return;
				}			
			}		
			USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
				
		}			
		else {
			
			while (USB_REG[0xA1]&0x01){
				if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
					gbt_USB_Detected = 0;
					return;
				}			
			}		
			USB_REG[0xC2] &= 0xFE;	
			USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
			USB_REG[0xE9] = 0x10;							//Clear Bulk-In(EP1) Buffer.
			USB_REG[0x2b] = 0x00;
			USB_REG[0x18] = 0x04;							//512Byte
			USB_REG[0x17] = 0x00;
			USB_REG[0xA2] = 0x01;							//Allow Bulk-In Tx.
			while (USB_REG[0xA2]&0x01){							//Bulk-In Tx done
				if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
					gbt_USB_Detected = 0;
					return;
				}			
			}			
			USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
		}
		
	}
	if(HPage.WD==tw_TotalESPage){		
		return ;
	}	
	if(tc_SmallSector_Cnt){
		if(gc_HInAddr==NAND_INFO.cINADDRMAX){
			tbt_DoSendCommand=1;
			gc_HInAddr=0;
			HPage.WD++;
			gc_SPage++;
			if(gc_SPage==H_INFO.cSMALLPAGE_NUM){
				gc_SPage=0;
				gc_SBlock++;
				if(CHK_DSBlock_Exist_Or_New(tc_Extend_Count,gc_SBlock,0)){// no dummy 	
					tbt_Find_D_SBlock=0;
				}
				else{
					tbt_Find_D_SBlock=1;
				}
			}			
		}
		if(HPage.WD==tw_TotalESPage){
			return;
		}
		else{
			if(tbt_Find_D_SBlock){// have dummy 	
				if((gc_SPage>=SQTable.BY[0])&&(gc_SPage<SQTable.BY[1])){
					APort_FBlock[0] =APort_DBlock[0];				
					gw_TargetPage = ((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+(gc_SPage-SQTable.BY[0]);	
				}	
				else{
					//APort_FBlock[0] =APort_EBlock[0];				
					if(gc_SBlock<gc_Q1_SBlock){// 0524 Jay add			
						APort_FBlock[0] =APort_LBlock[0];
					}
					else{			
					APort_FBlock[0] =APort_EBlock[0];				
					}			
					gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
				}
			}
			else{
				if(gc_SBlock<gc_Q1_SBlock){			
					APort_FBlock[0] =APort_LBlock[0];
				}
				else{			
					APort_FBlock[0] = APort_EBlock[0];
				}					
				gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
			}		
				
		}
		tc_RealInAddr = gc_HInAddr&0xFE;
		NAND_SBlock_SPage_To_FDBP();	
		FDBP.cFInAddr =tc_RealInAddr;
		Device_Read_SRAM_Index.BY[0] = USB_BUF_Bank0_HIGH;			// 080104 joyce add
		Device_Read_SRAM_Index.BY[1] = 0x00;
		Flash_ReadPage();
		USB_REG[0xC2] &= 0xEF;						//Bulk-In Tx done
		USB_REG[0x10] = 0x01;							//BulkInEn nonauto turn-on
		USB_REG[0xE9] = 0x01;							//Clear Bulk-In(EP1) Buffer.
		USB_REG[0x2B] = 0x00;
		USB_REG[0x16] = 0x02;	
		USB_REG[0x15] = 0x00;
		USB_REG[0xA1] = 0x01;							//Allow Bulk-In Tx.
		while (USB_REG[0xA1]&0x01){								//Bulk-In Tx done
			if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add				
				gbt_USB_Detected = 0;
				return;
			}			
		}		
		USB_REG[0x10] = 0x00;							//BulkInEn nonauto turn-on
		TotalXferPageNum.WD--;	
	}
}
