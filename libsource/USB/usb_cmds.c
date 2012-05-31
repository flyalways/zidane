#include "SPDA2K.h"

void ReadSectors(void) 
{
	gc_ErrorCode = 0;
	Check_LBA_ID();
	USB_REG[0xE9] = 0x01;				//Clear EP1
	gbt_OddTemp = 0;				    //Device Buffer Pin-pon use~~
	
	if (gc_ErrorCode||gbt_RW_UsbBuf) 
	{  //090520  Jay add
		NoCard_ReadSectors();
	}
	else 
	{
		switch(gc_CardType)
		{
			case 1:
				ReadATABlock();
			break;

			case 2:	
				SD_ReadSectors_USB();
			break;

			default:				   
				NoCard_ReadSectors();
			break;
    	}
  	}
	USB_REG[0x2B] =0x00; //0529 Jay add
	gc_USBStage = CSWStage;                // 資料都傳完了,進入下一stage==>Comand Status Wrapper
}

 
void NoCard_ReadSectors(void) 
{
	USB_REG[0x10] = 0x01;	
	USB_REG[0x2B] = 0x00; //0529 Jay add
	memset(USB_BUF_Bank0 ,0x00,512);
	do {
		USB_REG[0x15]  = 0x00;
		USB_REG[0x16]  = 0x02;
		USB_REG[0xA1] |= 0x01;
		while (USB_REG[0xA1] & 0x01){
			USB_PlugDetect();								// 081014 Jay add for usb plug out
			if(!gbt_USB_Detected){
				return ;
			}
		}
		TotalXferPageNum.WD--;
		LBA.LW++;
	} while (TotalXferPageNum.WD);
	USB_REG[0x10] = 0x00;
}


void ReadATABlock(void) 
{
	bit  tbt_RW_Buf;
	bit  tbt_ChangeHBlock;
	tbt_RW_Buf =0;
	tbt_ChangeHBlock =0;
	LBA2DBP();	
	SetHBlockToHDev();
	do {		 
		if(tbt_ChangeHBlock){
			SetHBlock_To_WhichGroup();
		}
		if(gc_ErrorCode && gbt_Support_Dummy_Capacity){// 090416 add for dummy capacity		    
//		    dbprintf("\n RataBlock _RW");
			gc_ErrorCode =0; 
			tbt_RW_Buf =1;                           // 若其已經超過Dummy capacity 只能等下一個CMD將其回Error status
		}
		if(gbt_enableCDROM & (!gc_bCBWLUN)){//09/05/06,joyce for remove device
			if(LBA.LW>=Capacity.LW){				//防止超過remove size,會讀到CD-ROM area
				gc_ErrorCode =0; 
				tbt_RW_Buf =1;				
			}
		}
		if (gc_ErrorCode||tbt_RW_Buf) {
			NoCard_ReadSectors();
			break;
		}
		NAND_FlashReadSector_Large();
		HPage.WD = 0;
		LBlock.WD++ ;
		HBlock.WD++;
		gbt_OddTemp = 0;
		gc_HInAddr =0;
		tbt_ChangeHBlock =1;
		#if 0 //20100330 Jay mark 
		HPage.WD = 0;
		gw_CylTemp++;
		HBlock.WD++;
		LBA.LW++;											//09/06/02,joyce
		gbt_OddTemp = 0;
		#endif
		USB_PlugDetect();									// 081014 Jay add for usb plug out
		if(!gbt_USB_Detected){
			break;
		}			
	} while (TotalXferPageNum.WD);
}


void WriteSectors(void) 
{
	gbt_OddTemp = 0;
	gc_ErrorCode = 0;

	Check_LBA_ID();
	if (gc_ErrorCode || gc_WriteProof||gbt_RW_UsbBuf) {
		NoCard_WriteSectors();
	}
	else if((gc_CardType==0x01)&& gc_WriteProof)
	{
		NoCard_WriteSectors();
	}
	else 
	{
		switch(gc_CardType)
		{
			case 1:
				WriteATABlock();
			break;

			case 2:
				SD_WriteSectors_USB();
			break;

			default:
				NoCard_WriteSectors();
			break;
		}
	}
	gc_USBStage = CSWStage;
}


void NoCard_WriteSectors(void) 
{
	USB_REG[0x11]  = 0x01;									//Turn on BulkOutEn, Turn off by data xfer done.
	USB_REG[0x19]  = 0x00;
	USB_REG[0x16]  = 0x02;
	USB_REG[0x15]  = 0x00;
	USB_REG[0xA1] |= 0x02;
	while ((USB_REG[0xA1] & 0x02));
	USB_REG[0xC2] &= 0xFD;
	USB_REG[0xE9]  = 0x02;
	USB_REG[0x11]  = 0x00;									//Turn-off BulkOutEn
	TotalXferPageNum.WD--;
	while (TotalXferPageNum.WD) 
	{
		USB_REG[0x11]  = 0x01;							//Turn on BulkOutEn, Turn off by data xfer done.
		USB_REG[0x19]  = 0x00;
		USB_REG[0x16]  = 0x02;
		USB_REG[0x15]  = 0x00;
		USB_REG[0xA1] |= 0x02;
		while (USB_REG[0xA1] & 0x02){
			USB_PlugDetect();								// 081014 Jay add for usb plug out
			if(!gbt_USB_Detected){
				return ;
			}
		}
		USB_REG[0xC2] &= 0xFD;
		USB_REG[0x11]  = 0x00;							//Turn-off BulkOutEn
		USB_REG[0xE9]  = 0x02;
		TotalXferPageNum.WD--;
	}
}


void WriteATABlock(void) 
{
	bit  tbt_RW_Buf;
	bit  tbt_ChangeHBlock;
	tbt_RW_Buf =0;
	tbt_ChangeHBlock =0;
	LBA2DBP();
	gbt_Force_SeqWrite_Mode =0;
	SetHBlockToHDev();
	do {     
		if(tbt_ChangeHBlock){
			SetHBlock_To_WhichGroup();
     	}
 
		if(gc_ErrorCode && gbt_Support_Dummy_Capacity){// 090416 add for dummy capacity		    
		    gc_ErrorCode =0;                            // 若其已經超過Dummy capacity 只能等下一個CMD將其回Error status
			tbt_RW_Buf =1;
		}

		if(gbt_enableCDROM & (!gc_bCBWLUN)){//09/05/06,joyce for remove device
			if(LBA.LW>=Capacity.LW){					//防止超過remove size,會寫到CD-ROM area
				gc_ErrorCode =0;
				tbt_RW_Buf =1;
			}
		}
		if (gc_ErrorCode||tbt_RW_Buf) {
			NoCard_WriteSectors();
			break;
		}
		NAND_FlashWriteSector_Large();
		if(HPage.WD==H_INFO.wHBLOCKPAGE){
			HPage.WD = 0;
			HBlock.WD++;
			gc_HInAddr =0;
			gbt_OddTemp = 0;	
			tbt_ChangeHBlock =1;
			LBlock.WD++ ;
			//dbprintf("\n Change to New HBlock :%x",HBlock.WD);
		}
		else{
			gbt_OddTemp = 0;	
			tbt_ChangeHBlock =0;
		}
		#if 0 //20100330 Jay mark 
		HPage.WD = 0x0000;
		gw_CylTemp++;
		HBlock.WD++;
		LBA.LW++;
		gbt_OddTemp = 0;		
		#endif
		USB_PlugDetect();									// 081014 Jay add for usb plug out
		if(!gbt_USB_Detected){
			break;
		}				
	} while (TotalXferPageNum.WD);
}



void  NAND_FlashWriteSector_Large(void) {
	data	bit	tbt_SmartLastBI,tbt_Extend_Found, tbt_Random_Found,tbt_SmartRandom;    
	data	bit	tbt_HPage_Euq_Q2;                                                                                                                                                                                                                                                                  
	data	U8	tc_Extend_Count,tc_Random_Count;                                                                                                                                                                                                                                                                   
	data	bit	tbt_DoSendCommand;	
	data	bit	tbt_PreLoadData;
	data	U8	tc_Operate_Mode;  
	data	U8	tc_XferPageCnt;
	data	U8	tc_Pre_XferCnt;
	data	U8	tc_Offset;
	//dbprintf("\n W:%lX , CNT:%x",LBA.LW,TotalXferPageNum.WD);
	USB_PlugDetect();
	if(!gbt_USB_Detected){
		return ;
	}
	DEVICE_REG[0x00]|=0x10;  
	DEVICE_REG[0x00]&=0xEF;
	DEVICE_REG[0x00]= 0x01;
	#if 0 
	Flash_State_Initial();
	if (gbt_USBHostIsHighSpeed) {
		USB_REG[0x12]=0x01;
	}
	else{
		USB_REG[0x12]=0x00;
	}	
	if((TotalXferPageNum.WD==1)||(gc_HInAddr==(NAND_INFO.cINADDRMAX-1))){
		USB_Bank0_1_BulkOut(0,0);		
		gbt_USB_Data_In_Buf=1;
	}
	else{
		USB_Bank0_1_BulkOut(0,1);		
		gbt_USB_Data_In_Buf=1;
	}	
	USB_CHK_Bank0_1_BulkOut_Done(0);
	#endif
	if(!gbt_USB_Detected){
		return;
	}		
	Check_LoadMap();	
	tbt_SmartLastBI = 0;
	tbt_SmartRandom =0;
	tbt_Extend_Found = 0;
	tbt_Random_Found =0;
	tbt_PreLoadData =0;
	tbt_DoSendCommand = 1;
	tbt_HPage_Euq_Q2 = 0;
	gbt_Send_80_81_CMD =0;
	gbt_Data_In_Buf    = 0;  		
	gc_Write_EDRS     = 0; 
	gbt_OddTemp      = 0;
	NAND_HPage_To_SBlock_SPage(); 
	for(tc_Extend_Count=0;tc_Extend_Count<Extend_NUM;tc_Extend_Count++){
		if(Mapping_Table[EINX+tc_Extend_Count]==HBlock.WD){
			tbt_SmartLastBI = 1;			
			gc_E_SBlock = (Mapping_Table[Q2_INX+tc_Extend_Count]>>8);
			gc_E_SPage  =(U8)Mapping_Table[Q2_INX+tc_Extend_Count];	
			gc_Q1_SBlock = (Mapping_Table[Q1_INX+tc_Extend_Count]>>8);
			QTable.WD =Mapping_Table[INVERTINX]; 
			QTable.BY[0] =tc_Extend_Count;
			Mapping_Table[INVERTINX] = QTable.WD;			
			if(gc_SBlock ==gc_E_SBlock){				 
				tbt_HPage_Euq_Q2 = 1;
				if(gc_SPage==gc_E_SPage){
					   
					tc_Operate_Mode=0x00;
				}   
				else if(gc_SPage<gc_E_SPage){
					tc_Operate_Mode = 0x01;
				}
				else{
					tc_Operate_Mode =0x02; 
				}				  								
			}
			else if(gc_SBlock<gc_E_SBlock){ 
				tc_Operate_Mode=0x01;     				
			}      
			else{ 									
				tc_Operate_Mode =0x02;	
			}
			break;
		 
		}
	}
	if(!tbt_SmartLastBI){ 

			for(tc_Random_Count=0;tc_Random_Count<MAX_RANDOM_HBLOCK;tc_Random_Count++){
				if(Wear_Table[RINX+tc_Random_Count]==HBlock.WD){
					tbt_SmartRandom= 1;						
					break;
				}
			}
			
			if(tbt_SmartRandom){
				//if((TotalXferPageNum.WD>=RANDOM_MAX_SECTORCNT)||gbt_Force_SeqWrite_Mode){  
				if((TotalXferPageNum.WD>RANDOM_MAX_SECTORCNT)||gbt_Force_SeqWrite_Mode){
					gbt_Force_SeqWrite_Mode=0;
					for(tc_Extend_Count=0;tc_Extend_Count<Extend_NUM;tc_Extend_Count++){
						if(Mapping_Table[EINX+tc_Extend_Count]==0xFFFF){
							tbt_Extend_Found = 1;
							QTable.WD =Mapping_Table[INVERTINX]; 
							QTable.BY[0] =tc_Extend_Count;
							Mapping_Table[INVERTINX] = QTable.WD;					
							break;
						}
					}
					tc_Operate_Mode = 0x03;  			
				}
				else{
					tc_Operate_Mode=0x04;   
					QTable.WD =Mapping_Table[INVERTINX]; 
					QTable.BY[1] =tc_Random_Count;
					Mapping_Table[INVERTINX] = QTable.WD;		
				}		
			}
			else{
				//if((TotalXferPageNum.WD>=RANDOM_MAX_SECTORCNT)||gbt_Force_SeqWrite_Mode){
				if((TotalXferPageNum.WD>RANDOM_MAX_SECTORCNT)||gbt_Force_SeqWrite_Mode){	  
					gbt_Force_SeqWrite_Mode=0;
					for(tc_Extend_Count=0;tc_Extend_Count<Extend_NUM;tc_Extend_Count++){
						if(Mapping_Table[EINX+tc_Extend_Count]==0xFFFF){
							tbt_Extend_Found = 1;
							QTable.WD =Mapping_Table[INVERTINX]; 
							QTable.BY[0] =tc_Extend_Count;
							Mapping_Table[INVERTINX] = QTable.WD;							
							break;
						}
					}
					tc_Operate_Mode = 0x05 ;  	
				}
				else{
					for(tc_Random_Count=0;tc_Random_Count<MAX_RANDOM_HBLOCK;tc_Random_Count++){
						if(Wear_Table[RINX+tc_Random_Count]==0xFF){
							tbt_Random_Found = 1;
							QTable.WD =Mapping_Table[INVERTINX]; 
							QTable.BY[1] =tc_Random_Count;
							Mapping_Table[INVERTINX] = QTable.WD;
							break;
						}
					}		
					tc_Operate_Mode =0x06;
				}
			}
		
			
	}                                                                                                                                                                                                                                                         
	
	
	DEVICE_REG[0x0F] =gc_HDev;
	CHK_Flash_Ready();  	
	FlashReset();
	LoadBlock_From_MAP(0);  
	CHK_Flash_Ready();
	Flash_State_Initial();
	#if 1
	if (gbt_USBHostIsHighSpeed) {
		USB_REG[0x12]=0x01;
	}
	else{
		USB_REG[0x12]=0x00;
	}	
	if((TotalXferPageNum.WD==1)||(gc_HInAddr==(NAND_INFO.cINADDRMAX-1))){
		USB_Bank0_1_BulkOut(0,0);		
		gbt_USB_Data_In_Buf=1;
	}
	else{
		USB_Bank0_1_BulkOut(0,1);		
		gbt_USB_Data_In_Buf=1;
	}	
	USB_CHK_Bank0_1_BulkOut_Done(0);
	#endif
	//--- chk pre_load data ----//
	if((gc_HInAddr+ TotalXferPageNum.WD)<NAND_INFO.cINADDRMAX){	
		tbt_PreLoadData =1;		
	}	
	
	if(tbt_SmartLastBI){ //EXT  mode  
		Load_E_D_Block_From_MAP(0,tc_Extend_Count);
		gc_Q1_SBlock = (Mapping_Table[Q1_INX+tc_Extend_Count]>>8);
		gc_E_SBlock = (Mapping_Table[Q2_INX+tc_Extend_Count]>>8);
		gc_E_SPage  =(U8)Mapping_Table[Q2_INX+tc_Extend_Count];	
		if(tc_Operate_Mode==0x00){
			if(gc_HInAddr||tbt_PreLoadData){ 
				APort_FBlock[0]= APort_LBlock[0];					
				gw_SourcePage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
				NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 	gbt_Data_In_Buf=1;
			}
			gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
			gc_Write_EDRS =0x00; 
		}			
		if(tc_Operate_Mode==0x01){
			if(tbt_HPage_Euq_Q2&&(gc_E_SPage>0)){
			 	NAND_Close_E_SBlock(tc_Extend_Count);		 	
			}	
    		gc_T_SBlock =(U8)Mapping_Table[Q1_INX+tc_Extend_Count];
    		if(gc_T_SBlock==H_INFO.cSMALLBLOCK_NUM){
    			NAND_Close_Dummy_Block(tc_Extend_Count); 
    			gc_T_SBlock =(U8)Mapping_Table[Q1_INX+tc_Extend_Count];   		
    		}			
			if(CHK_DSBlock_Exist_Or_New(tc_Extend_Count,gc_SBlock,1)){
				QTable.WD=Mapping_Table[Q2_INX+tc_Extend_Count];
				NAND_Combin_E_D_To_New_E(tc_Extend_Count ,QTable.BY[0]);
				gc_T_SBlock =(U8)Mapping_Table[Q1_INX+tc_Extend_Count];			
			}				
    		CHK_DSBlock_Exist_Or_New(tc_Extend_Count,gc_SBlock,1);
    		if(SQTable.WD== 0x0000){  
				if(gc_HInAddr||tbt_PreLoadData){
					if(gc_SBlock<gc_Q1_SBlock){
						APort_FBlock[0] =APort_LBlock[0];
					}
					else{
						APort_FBlock[0]= APort_EBlock[0];
					}							
					gw_SourcePage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
					NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 		gbt_Data_In_Buf=1;
			 	}
			 	SQTable.BY[0] =gc_SPage;
			 	gw_TargetPage=((U16)gc_T_SBlock*H_INFO.cSMALLPAGE_NUM)+0;
			}	
			else{
				if(gc_SPage<SQTable.BY[0]){	
					if(gc_HInAddr||tbt_PreLoadData){
						if(gc_SBlock<gc_Q1_SBlock){
							APort_FBlock[0] =APort_LBlock[0];
						}
						else{
							APort_FBlock[0]= APort_EBlock[0];
						}					
						gw_SourcePage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
						NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 			gbt_Data_In_Buf=1;
			 		}
			 		SQTable.BY[0] = gc_SPage;		 		
			 		gw_TargetPage=((U16)gc_T_SBlock*H_INFO.cSMALLPAGE_NUM)+0;					
			 	}	
			 	else if((SQTable.BY[0]<=gc_SPage)&&(gc_SPage<SQTable.BY[1])){
			 		APort_FBlock[0]= APort_DBlock[0];
					APort_FBlock[1]= APort_DBlock[0];
					gw_SourcePage=((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+0;
       				gw_TargetPage=((U16)gc_T_SBlock*H_INFO.cSMALLPAGE_NUM)+0;					
					ZSpare[0] = 0x5A; 		ZSpare[1] = gc_SBlock;
					ZSpare[2] = HBlock.WD;	ZSpare[3] = SQTable.BY[0];	
					ZSpare[4] = 0x5A; 		ZSpare[5] = gc_SBlock;
					ZSpare[6] = HBlock.WD;	ZSpare[7] = SQTable.BY[0];				
					tc_XferPageCnt = gc_SPage-SQTable.BY[0];
					RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
					if(gc_HInAddr||tbt_PreLoadData){ 
						APort_FBlock[0]= APort_DBlock[0];					
						gw_SourcePage=((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+(gc_SPage-SQTable.BY[0]);
						NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 			gbt_Data_In_Buf=1;
			 		}
			 		gw_TargetPage=gw_TargetPage+tc_XferPageCnt;
			 							
			 	}
			 	else{
			 		
			 		APort_FBlock[0]= APort_DBlock[0];
					APort_FBlock[1]= APort_DBlock[0];
					tc_XferPageCnt = SQTable.BY[1]-SQTable.BY[0];
					gw_SourcePage=((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+0;
       				gw_TargetPage=((U16)gc_T_SBlock*H_INFO.cSMALLPAGE_NUM)+0;					
					ZSpare[0] = 0x5A; 		ZSpare[1] = gc_SBlock;
					ZSpare[2] = HBlock.WD;	ZSpare[3] = SQTable.BY[0];
					ZSpare[4] = 0x5A; 		ZSpare[5] = gc_SBlock;
					ZSpare[6] = HBlock.WD;	ZSpare[7] = SQTable.BY[0];					
					RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
				
					if(gc_SBlock<gc_Q1_SBlock){
						APort_FBlock[0] =APort_LBlock[0];
					}
					else{
						APort_FBlock[0]= APort_EBlock[0];
					}
					APort_FBlock[1]= APort_DBlock[0];
					tc_XferPageCnt = gc_SPage-SQTable.BY[1];
					gw_SourcePage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+SQTable.BY[1];
       				gw_TargetPage=((U16)gc_T_SBlock*H_INFO.cSMALLPAGE_NUM)+(SQTable.BY[1]-SQTable.BY[0]);					
					RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
					if(gc_HInAddr||tbt_PreLoadData){ 
						if(gc_SBlock<gc_Q1_SBlock){
							APort_FBlock[0] =APort_LBlock[0];
						}
						else{
							APort_FBlock[0]= APort_EBlock[0];
						}				
						gw_SourcePage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
						NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 			gbt_Data_In_Buf=1;
			 		}
			 		gw_TargetPage=gw_TargetPage+tc_XferPageCnt;	
			 			 		
			 	}				
			}
    		
    		gc_Write_EDRS =0x01;				
		}
		if(tc_Operate_Mode==0x02){		    	
		
			if(tbt_HPage_Euq_Q2){
				NAND_Fill_H2E_SPage(tc_Extend_Count, gc_SPage);
			}
			else{				
				if(gc_E_SPage>0){  
				 	NAND_Close_E_SBlock(tc_Extend_Count);	
				}	
				NAND_Fill_H2E_SBlock(tc_Extend_Count,gc_SBlock);
				NAND_Fill_H2E_SPage(tc_Extend_Count, gc_SPage);
			}
			if(gc_HInAddr||tbt_PreLoadData){ 
				APort_FBlock[0]= APort_LBlock[0];					
				gw_SourcePage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
				NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 	gbt_Data_In_Buf=1;
			}				
			gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;	
			gc_Write_EDRS =0x00; 		
				
		}		                                                                                                                                                                                                                                                 
	}
	else if(tbt_SmartRandom){
		if(tc_Operate_Mode==0x03){ 
			NAND_Combin_H_R_To_New_H(tc_Random_Count);		
				
			if(!tbt_Extend_Found ){
				QTable.WD =Mapping_Table[INVERTINX]; 
				tc_Extend_Count = (QTable.BY[0]+1)&(Extend_NUM-1);
				QTable.BY[0] =tc_Extend_Count;
				Mapping_Table[INVERTINX] = QTable.WD;
			
				tc_Offset = Mapping_Table[EINX+tc_Extend_Count];			
				APort_LBlock[0] = Mapping_Table[tc_Offset];
				APort_LCNT[0] = Wear_Table[tc_Offset];
				Load_E_D_Block_From_MAP(0,tc_Extend_Count);				
			
				NAND_Combin_H_E_D_To_New_H(tc_Extend_Count);
				 	
			}
			LoadBlock_From_MAP(0);
			Load_E_D_Block_From_MAP(0,tc_Extend_Count);	    
			Mapping_Table[EINX+tc_Extend_Count] =HBlock.WD;
			
			NAND_Update_PWR_INX(tc_Extend_Count);
			if(gc_SBlock>(H_INFO.cSMALLBLOCK_NUM>>COPY_OR_PASS)){
				QTable.WD= Mapping_Table[Q1_INX+tc_Extend_Count];
				QTable.BY[0] =gc_SBlock;
				Mapping_Table[Q1_INX+tc_Extend_Count]=QTable.WD;
				QTable.BY[0] =gc_SBlock;
				QTable.BY[1] =0x00;
				Mapping_Table[Q2_INX+tc_Extend_Count]= QTable.WD;
				NAND_Write_Q1_Mark(tc_Extend_Count);			
			}
			else{	
				QTable.WD= Mapping_Table[Q1_INX+tc_Extend_Count];
				QTable.BY[0] =0;
				Mapping_Table[Q1_INX+tc_Extend_Count]=QTable.WD;
				Mapping_Table[Q2_INX+tc_Extend_Count]= 0x0000;			
			}			
			NAND_Fill_H2E_SBlock(tc_Extend_Count,gc_SBlock);
			NAND_Fill_H2E_SPage(tc_Extend_Count,gc_SPage);
		
			if(gc_HInAddr||tbt_PreLoadData){
				APort_FBlock[0]= APort_LBlock[0];					
				gw_SourcePage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
				NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 	gbt_Data_In_Buf=1;
			}
			gw_TargetPage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
			gc_Write_EDRS =0x00;
			tbt_SmartLastBI=1;
			tbt_SmartRandom=0;	
		}
		if(tc_Operate_Mode==0x04){
			if(CHK_RandomPage_Exist(tc_Random_Count,HPage.WD)){
				gw_R_SPage=Mapping_Table[RPAGEINX];				
				if(gw_R_SPage== H_INFO.wHBLOCKPAGE){
					NAND_Close_Random_Block();		   
					gw_R_SPage = Mapping_Table[RPAGEINX];
				}
				if(gc_HInAddr||tbt_PreLoadData){ 
					APort_FBlock[0]= APort_RBlock[0];					
					tc_Offset = (tc_Random_Count*MAX_RANDOM_HPAGE); 
					gw_SourcePage=Mapping_Table[R_PAGE+tc_Offset+gc_NowIndex];
					NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 		gbt_Data_In_Buf=1;
			 	}						
			}
			else if(CHK_RandomPage_Space(tc_Random_Count)){
				gw_R_SPage=Mapping_Table[RPAGEINX];				
				if(gw_R_SPage==H_INFO.wHBLOCKPAGE){
					NAND_Close_Random_Block();
					gw_R_SPage = Mapping_Table[RPAGEINX];		   
				}
				if(gc_HInAddr||tbt_PreLoadData){					
					APort_FBlock[0]= APort_LBlock[0];					
					gw_SourcePage=HPage.WD;
					NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 		gbt_Data_In_Buf=1;
			 	}					
			}
			else{
					
				NAND_Combin_H_R_To_New_H(tc_Random_Count);
			
				gw_R_SPage=Mapping_Table[RPAGEINX];				 
				if(gw_R_SPage==H_INFO.wHBLOCKPAGE){
					NAND_Close_Random_Block();
					gw_R_SPage = Mapping_Table[RPAGEINX];		   
				}			
				Wear_Table[RINX+tc_Random_Count] =HBlock.WD;
				CHK_RandomPage_Space(tc_Random_Count);			
				if(gc_HInAddr||tbt_PreLoadData){ 
					APort_FBlock[0]= APort_LBlock[0];					
					gw_SourcePage=HPage.WD;
					NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 		gbt_Data_In_Buf=1;
			 	}
			}
			
			gw_TargetPage=gw_R_SPage;
			gc_Write_EDRS =0x02;
		}	
	}
	else{
		if(tc_Operate_Mode==0x05){
			if(!tbt_Extend_Found ){				
				QTable.WD =Mapping_Table[INVERTINX]; 
				tc_Extend_Count = (QTable.BY[0]+1)&(Extend_NUM-1);
				QTable.BY[0] =tc_Extend_Count;
				Mapping_Table[INVERTINX] = QTable.WD;
				
				tc_Offset = Mapping_Table[EINX+tc_Extend_Count];			
				APort_LBlock[0] = Mapping_Table[tc_Offset];
				APort_LCNT[0] = Wear_Table[tc_Offset];
				Load_E_D_Block_From_MAP(0,tc_Extend_Count);				
				
				NAND_Combin_H_E_D_To_New_H(tc_Extend_Count);
			     	
			}
			LoadBlock_From_MAP(0);
			Load_E_D_Block_From_MAP(0,tc_Extend_Count);	
		
			Mapping_Table[EINX+tc_Extend_Count] =HBlock.WD;
			NAND_Update_PWR_INX(tc_Extend_Count);
			if(gc_SBlock>(H_INFO.cSMALLBLOCK_NUM>>COPY_OR_PASS)){
				QTable.WD= Mapping_Table[Q1_INX+tc_Extend_Count];
				QTable.BY[0] =gc_SBlock;
				Mapping_Table[Q1_INX+tc_Extend_Count]=QTable.WD;
				QTable.BY[0] =gc_SBlock;
				QTable.BY[1] =0x00;
				Mapping_Table[Q2_INX+tc_Extend_Count]= QTable.WD;
				NAND_Write_Q1_Mark(tc_Extend_Count);			
			}
			else{	
				QTable.WD= Mapping_Table[Q1_INX+tc_Extend_Count];
				QTable.BY[0] =0;
				Mapping_Table[Q1_INX+tc_Extend_Count]=QTable.WD;
				Mapping_Table[Q2_INX+tc_Extend_Count]= 0x0000;			
			}
			NAND_Fill_H2E_SBlock(tc_Extend_Count,gc_SBlock);
			NAND_Fill_H2E_SPage(tc_Extend_Count,gc_SPage);			
			if(gc_HInAddr||tbt_PreLoadData){
				APort_FBlock[0]= APort_LBlock[0];					
				gw_SourcePage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
				NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 	gbt_Data_In_Buf=1;
			}			
			gw_TargetPage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
			gc_Write_EDRS =0x00;
			tbt_SmartLastBI=1;
			
		}
		if(tc_Operate_Mode==0x06){
			if(!tbt_Random_Found){
				QTable.WD =Mapping_Table[INVERTINX]; 
				tc_Random_Count = (QTable.BY[1]+1)&(MAX_RANDOM_HBLOCK-1);
				QTable.BY[1] =tc_Random_Count;
				Mapping_Table[INVERTINX] = QTable.WD;
				
				tc_Offset = Wear_Table[RINX+tc_Random_Count];			
				APort_LBlock[0] = Mapping_Table[tc_Offset];
				APort_LCNT[0] = Wear_Table[tc_Offset];
				
				NAND_Combin_H_R_To_New_H(tc_Random_Count);				
				
			}		
			LoadBlock_From_MAP(0);
		
			gw_R_SPage = Mapping_Table[RPAGEINX];
			if(gw_R_SPage==H_INFO.wHBLOCKPAGE){
				NAND_Close_Random_Block();
				gw_R_SPage = Mapping_Table[RPAGEINX];		   
			}			
			Wear_Table[RINX+tc_Random_Count] = HBlock.WD;
			CHK_RandomPage_Space(tc_Random_Count);
			if(gc_HInAddr||tbt_PreLoadData){ 
				APort_FBlock[0]= APort_LBlock[0];					
				gw_SourcePage=HPage.WD;
				NAND_Laod_Data_To_Buf(gw_SourcePage,0);
			 	gbt_Data_In_Buf=1;
			}			
			gw_TargetPage = gw_R_SPage;
			gc_Write_EDRS =0x02; 
		}		
	}
	#if EN_WEAR_LEVELING
	if(Wear_Table[SCNT]>=0xF0){
//		dbprintf("\n Zone:%x ,SBLK:%x ",gw_WhichGroup,Mapping_Table[SBK]);
		NAND_Update_WearBlock(gc_HDev);
	}	
	#endif
	
	LoadBlock_From_MAP(0);
	if(tbt_SmartLastBI){
		Load_E_D_Block_From_MAP(0,tc_Extend_Count);
		gc_E_SBlock = (Mapping_Table[Q2_INX+tc_Extend_Count]>>8);
		gc_E_SPage  =(U8)Mapping_Table[Q2_INX+tc_Extend_Count];	
		gc_Q1_SBlock = (Mapping_Table[Q1_INX+tc_Extend_Count]>>8);
	}
	
	NAND_Set_FBlock(gc_Write_EDRS);
	
	if((tc_Operate_Mode==0x04)||(tc_Operate_Mode==0x06)){
		
	
		NAND_Set_Spare_Data(gc_Write_EDRS,tc_Random_Count);
		if(gc_HInAddr){
			tc_XferPageCnt= NAND_INFO.cINADDRMAX-gc_HInAddr;
		}
		else{	
			tc_XferPageCnt = NAND_INFO.cINADDRMAX;
		}
		tc_Pre_XferCnt = (tc_XferPageCnt<=TotalXferPageNum.WD)? tc_XferPageCnt: TotalXferPageNum.WD;
		
		USB_Rcevie_Data_To_BUf(gc_HInAddr,tc_Pre_XferCnt);	
		if(!gbt_USB_Detected){
			goto USB_Plug_Out;
		}		
		TotalXferPageNum.WD-= tc_Pre_XferCnt;
		Device_Write_SRAM_Index.BY[0] = 0x80;			
		Device_Write_SRAM_Index.BY[1] = 0x00;
		NAND_SBlock_SPage_To_FDBP();
		Flash_Sequent_Write(NAND_INFO.cINADDRMAX,1);	
		
		tc_Offset = (tc_Random_Count *MAX_RANDOM_HPAGE)+gc_NowIndex; 
		Mapping_Table[R_HPAGE+tc_Offset] = HPage.WD;
		Mapping_Table[R_PAGE+tc_Offset]  =  gw_TargetPage;
		Wear_Table[RINX+tc_Random_Count] = HBlock.WD; 
		gw_R_SPage++;
		HPage.WD++;
		gc_HInAddr=0;
		Mapping_Table[RPAGEINX] =gw_R_SPage;
		BMT_KEY[gw_NowBMTPoint]=1;

	}
	else{
			
		if(tc_Operate_Mode==0x01){
			NAND_Set_Spare_Data(gc_Write_EDRS,tc_Extend_Count);
			NAND_OverWrite_Mode(tc_Extend_Count);		
			if(!gbt_USB_Detected){
				goto USB_Plug_Out;
			}
			
		}
		else{ 
			
			NAND_Set_Spare_Data(gc_Write_EDRS,tc_Extend_Count);
			NAND_SequentWrite_Mode(tc_Extend_Count);
			gbt_Force_SeqWrite_Mode =1;
			if(!gbt_USB_Detected){
				goto USB_Plug_Out;
			}		
		
		}
		BMT_KEY[gw_NowBMTPoint]=1;
	}
			
USB_Plug_Out:
	
	if(!gbt_USB_Detected){		
		DMA_REG[0xC0] =0x00;
		DEVICE_REG[0x00]|=0x10;									// 將Flash module reset
		DEVICE_REG[0x00]&=0xEF;
		DEVICE_REG[0x00]= 0x01;
		FLASH_REG[0x06] = 0xFF;
		FLASH_REG[0x04] = 0x08;
		CHK_Flash_Ready();
		FLASH_REG[0x03] = 0x01;		

         }
	

}





