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

//! backup flash data (from gw_SourceBlock to gw_TargetBlock)
/*!
 *	\param	StartPage		start sector address
 *	\param	EndPage			end sector address
 *	\param	Spare_Mark		0:not care spare data 1:care spare data
 */
void FLASH_Backup_Blcok(UWORD StartPage , UWORD EndPage , U8  Spare_Mark, U8 TargetDev) {	
	data	U16 tw_Page; 	
//dbprintf("A ");
	//DMA_REG[0xB1]    = 0x00;									// Disable DMA Auto address account
	//DEVICE_REG[0xA0] = 0x01;									// Reset ECC  
	//FLASH_REG[0xA3]  = 0x01;									// Enable ECC 
	//FLASH_REG[0xA0]  = 0x00;									// Disable Encode RS-Code
	//FLASH_REG[0xA1]  = 0x01;									// Start Decode RS-Code
	//FLASH_REG[0xA2] &= 0xF3;									// Clear ECC fail register
	///////////////////////////////////////////////////////////////////////////////////////
	NAND_Set_Spare_Data(Spare_Mark,0);//set mark type 
	FDBP.cFDev= TargetDev;
	
	for(tw_Page=StartPage; tw_Page<EndPage;tw_Page++){
		//-------------------------------------- //
		Device_Read_SRAM_Index.BY[0] = 0x80;
		Device_Read_SRAM_Index.BY[1] = 0x00; 
		FDBP.cFPlane = tw_Page%NAND_INFO.cPLANE_NUM;
		FDBP.wFBlock = gw_SourceBlock;// source block 
		FDBP.wFPage  = tw_Page/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr =0;
		Flash_Sequent_Read(NAND_INFO.cINADDRMAX);
		//--------------------------------------//
		Device_Write_SRAM_Index.BY[0] = 0x80;
		Device_Write_SRAM_Index.BY[1] = 0x00;
		FDBP.cFPlane = tw_Page%NAND_INFO.cPLANE_NUM;
		FDBP.wFBlock = gw_TargetBlock;// Target block
		FDBP.wFPage  = tw_Page/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr =0;
		Flash_Sequent_Write(NAND_INFO.cINADDRMAX,1);
		
		
	}
}


/*********************************************************************************
* NAME:               void NAND_Fill_H2E_SBlock(U8 tc_Start_SBlock,U8 tc_END_SBlock)
* DESCRIPTION:
*********************************************************************************/
void NAND_Fill_H2E_SBlock(U8 tc_Inx,U8 tc_END_SBlock){
	data	U8 tc_E_SBlock;
	data	U8 tc_XferPageCnt;
	NAND_Set_Spare_Data(0,tc_Inx);
	tc_E_SBlock =(Mapping_Table[Q2_INX+tc_Inx]>>8);
	for(; tc_E_SBlock<tc_END_SBlock;tc_E_SBlock++){
		APort_FBlock[0]= APort_LBlock[0];
		APort_FBlock[1]= APort_EBlock[0];				
		gw_SourcePage=((U16)tc_E_SBlock*H_INFO.cSMALLPAGE_NUM)+0;
    	gw_TargetPage=((U16)tc_E_SBlock*H_INFO.cSMALLPAGE_NUM)+0;					
		tc_XferPageCnt = H_INFO.cSMALLPAGE_NUM;
		RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
	}
	QTable.WD =Mapping_Table[Q2_INX+tc_Inx];
	QTable.BY[0] = tc_END_SBlock;
	//QTable.BY[1] = 0x0000;不要亂動～～會出現OVER WRITE
	Mapping_Table[Q2_INX+tc_Inx]= QTable.WD;
}
/*********************************************************************************
* NAME:               void NAND_Fill_H2E_SBlock(U8 tc_Start_SBlock,U8 tc_END_SBlock)
* DESCRIPTION:
*********************************************************************************/
void NAND_Fill_H2E_SPage(U8 tc_Inx,U8 tc_END_SPage){
	xdata	U8	tc_E_SBlock;
	xdata	U8	tc_E_SPage;
	xdata	U8	tc_XferPageCnt;
	NAND_Set_Spare_Data(0,tc_Inx);
	tc_E_SBlock =(Mapping_Table[Q2_INX+tc_Inx]>>8);
	tc_E_SPage = (U8)Mapping_Table[Q2_INX+tc_Inx];	
	APort_FBlock[0]= APort_LBlock[0];
	APort_FBlock[1]= APort_EBlock[0];				
	gw_SourcePage=((U16)tc_E_SBlock*H_INFO.cSMALLPAGE_NUM)+tc_E_SPage;
    gw_TargetPage=((U16)tc_E_SBlock*H_INFO.cSMALLPAGE_NUM)+tc_E_SPage;					
	tc_XferPageCnt = (tc_END_SPage-tc_E_SPage);
	RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );	
	QTable.WD =Mapping_Table[Q2_INX+tc_Inx];
	QTable.BY[1] = tc_END_SPage;
	Mapping_Table[Q2_INX+tc_Inx]= QTable.WD;
	//QTable.BY[0] = tc_E_SBlock;
	//QTable.BY[1] = tc_END_SPage;
	//Mapping_Table[Q2_INX+tc_Inx]= QTable.WD;
	
}




/*********************************************************************************
* NAME:               void NAND_Combin_E_D_To_New_E(U8 tc_END_SBlock)
* DESCRIPTION:
*********************************************************************************/
void NAND_Combin_E_D_To_New_E(U8 tc_Inx ,U8 tc_END_SBlock){
	data	U8	tc_Start_SBlock;
	data	bit	tbt_Not_D_SBlock;
	data	U8	tc_XferPageCnt ;
	data	U8	tc_Q1_SBlock;
//dbprintf("D ");	
	ZSpare[0] = 0xA5; 							ZSpare[1] = (U8)Mapping_Table[PWR_INX+tc_Inx];
	ZSpare[2] = Mapping_Table[EINX+tc_Inx]; 	ZSpare[3] = APort_SCNT[0];	
	ZSpare[4] = 'Q'; 							ZSpare[5] = 0x00;//0520 Jay add
	ZSpare[6] = 'Q'; 							ZSpare[7] = 0x00;// update Q1 Block 	
	tc_Q1_SBlock = (Mapping_Table[Q1_INX+tc_Inx]>>8);
	
	
	for(tc_Start_SBlock=0; tc_Start_SBlock<=tc_END_SBlock;tc_Start_SBlock++){
		tbt_Not_D_SBlock=CHK_DSBlock_Exist_Or_New(tc_Inx,tc_Start_SBlock,0); 
		
		if(tbt_Not_D_SBlock){//not find D_SBlock
			if(tc_Start_SBlock<tc_Q1_SBlock){		
				APort_FBlock[0]= APort_LBlock[0];
			}
			else{			
				APort_FBlock[0]= APort_EBlock[0];
			}
			APort_FBlock[1]= APort_SBlock[0];				
			gw_SourcePage=((U16)tc_Start_SBlock*H_INFO.cSMALLPAGE_NUM)+0;
    		gw_TargetPage=((U16)tc_Start_SBlock*H_INFO.cSMALLPAGE_NUM)+0;					
			if(tc_Start_SBlock==tc_END_SBlock){
				tc_XferPageCnt = (U8)Mapping_Table[Q2_INX+tc_Inx];
			}
			else{				
				tc_XferPageCnt = H_INFO.cSMALLPAGE_NUM;
			}
		
			RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
		}
		else{// 有資料在D_SBlock
			// 0~~SQTable.BY[0]
			if(SQTable.BY[0]){
				if(tc_Start_SBlock<tc_Q1_SBlock){		
					APort_FBlock[0]= APort_LBlock[0];
				}
				else{			
					APort_FBlock[0]= APort_EBlock[0];
				}			
				APort_FBlock[1]= APort_SBlock[0];				
				gw_SourcePage=((U16)tc_Start_SBlock*H_INFO.cSMALLPAGE_NUM)+0;
    			gw_TargetPage=((U16)tc_Start_SBlock*H_INFO.cSMALLPAGE_NUM)+0;					
				tc_XferPageCnt =SQTable.BY[0];
							
				RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
			}
			// SQTable.BY[0]~SQTable.BY[1] 	
			APort_FBlock[0]= APort_DBlock[0];
			APort_FBlock[1]= APort_SBlock[0];				
			gw_SourcePage=((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+0;
    		gw_TargetPage=((U16)tc_Start_SBlock*H_INFO.cSMALLPAGE_NUM)+SQTable.BY[0];					
			tc_XferPageCnt =(SQTable.BY[1]-SQTable.BY[0]);
					
			RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );	
			// SQTable.BY[1]~~~H_INFO.cSMALLBPAGE_NUM;
			if(tc_Start_SBlock<tc_Q1_SBlock){		
				APort_FBlock[0]= APort_LBlock[0];
			}
			else{			
				APort_FBlock[0]= APort_EBlock[0];
			}			
			APort_FBlock[1]= APort_SBlock[0];				
			gw_SourcePage=((U16)tc_Start_SBlock*H_INFO.cSMALLPAGE_NUM)+SQTable.BY[1];
    		gw_TargetPage=((U16)tc_Start_SBlock*H_INFO.cSMALLPAGE_NUM)+SQTable.BY[1];					
			tc_XferPageCnt =(H_INFO.cSMALLPAGE_NUM-SQTable.BY[1]);
			
			RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
				
		}
	}
	// Updata Mapping Table //
	Mapping_Table[EBK+tc_Inx] = APort_SBlock[0]; 			 	
	Wear_Table[ECNT+tc_Inx]  = APort_SCNT[0];
	NAND_Erase_Block_LEDR(1); // erase external 
	Mapping_Table[DBK+tc_Inx]=APort_EBlock[0];
	Wear_Table[DCNT+tc_Inx]= (APort_ECNT[0]+1);
	NAND_Erase_Block_LEDR(2); // earse dummy
	Mapping_Table[SBK]=APort_DBlock[0];
	Wear_Table[SCNT]= (APort_DCNT[0]+1);	
	//Mapping_Table[Q2_INX+tc_Inx]=((U16)tc_END_SBlock<<8);//0514不應該動
	Mapping_Table[Q1_INX+tc_Inx]= 0x0000;
	Load_E_D_Block_From_MAP(0,tc_Inx);	// updat E D block 		
	APort_SBlock[0] = Mapping_Table[SBK];//update SBlock
	APort_SCNT[0] = Wear_Table[SCNT];	
	NAND_Clear_D_SBlock_Flag(tc_Inx);
	

}
/*********************************************************************************
* NAME:               void NAND_Combin_E_D_To_New_E(U8 tc_END_SBlock)
* DESCRIPTION:
*********************************************************************************/
void NAND_Combin_H_E_D_To_New_H(U8 tc_Inx){
	data	U8	tc_Offset;
	xdata	U8	tc_i;
	data	bit	tbt_Find_D_SBlock=0;
	data	U8	tc_Q1_SBlock;
//dbprintf("E ");	
	tc_Q1_SBlock = Mapping_Table[Q1_INX+tc_Inx]>>8;
	tc_Offset= (tc_Inx* MAX_DUMMY_PAGE); 
	for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
		if(Mapping_Table[D_HPAGE+tc_Offset+tc_i]!=0xFFFF){
			tbt_Find_D_SBlock=1;
			break;
		}
	}
	QTable.WD = Mapping_Table[Q2_INX+tc_Inx];
	if(tbt_Find_D_SBlock||(tc_Q1_SBlock!=0)){// 有D_SBlock或是 Q1_Sblock!=0 應該是要寫swap block	
		NAND_Combin_E_D_To_New_E(tc_Inx ,QTable.BY[0]);		
	}
	QTable.WD = Mapping_Table[Q2_INX+tc_Inx];
	if(QTable.BY[1]){ 
		NAND_Close_E_SBlock(tc_Inx);
	}	
	NAND_Fill_H2E_SBlock(tc_Inx,H_INFO.cSMALLBLOCK_NUM);
	//---------  Updata Mapping Table -----------------//
	tc_Offset =Mapping_Table[EINX+tc_Inx];
	Mapping_Table[tc_Offset]=APort_EBlock[0];
	Wear_Table[tc_Offset] = APort_ECNT[0];
	NAND_Erase_Block_LEDR(0);
	Mapping_Table[EBK+tc_Inx] =APort_LBlock[0];// Swap Block -->New External Block 			 	
	Wear_Table[ECNT+tc_Inx] = (APort_LCNT[0]+1);
	Mapping_Table[Q2_INX+tc_Inx]=0x0000;
	Mapping_Table[Q1_INX+tc_Inx]=0x0000;
	Mapping_Table[EINX+tc_Inx]=0xFFFF;
}
/*********************************************************************************
* NAME:               void NAND_Combin_E_D_To_New_E(U8 tc_END_SBlock)
* DESCRIPTION:
*********************************************************************************/
void NAND_Combin_H_R_To_New_H(U8 tc_Inx){
	xdata	U16	tw_Page;
	data	U8  tc_Temp_HBlock;
	data	U8  tc_Offset;
	xdata	U8  tc_PwrInx;
//dbprintf("F ");	
	tc_Offset = tc_Inx*MAX_RANDOM_HPAGE;
	tc_Temp_HBlock = Wear_Table[RINX+tc_Inx];
	APort_LBlock[0] =	Mapping_Table[tc_Temp_HBlock];
	APort_LCNT[0] = 	Wear_Table[tc_Temp_HBlock];
	//------- update PWR_INX ---------------//
	FDBP.cFDev=gc_HDev;
	FDBP.cFPlane = 0;
	FDBP.wFBlock = APort_LBlock[0];
	FDBP.wFPage  = 0;
	FDBP.cFInAddr =0;  
	Device_Read_SRAM_Index.WD=0x8000;
	Flash_ReadSpare(4);  // get Hblock spare data 
	tc_PwrInx =ZSpare[1];
	//----------------------------------------//
	
	
	ZSpare[0] = 0xA5; 		    ZSpare[1] = tc_PwrInx+1;
	ZSpare[2] = tc_Temp_HBlock;	ZSpare[3] = APort_SCNT[0];	
	ZSpare[4] = 'Q'; 			ZSpare[5] = 0;// Q1_INX
	ZSpare[6] = 'Q';			ZSpare[7] = 0;// Q1_INX	
	APort_FBlock[1]= APort_SBlock[0];
	for(tw_Page =0; tw_Page<H_INFO.wHBLOCKPAGE; tw_Page++){
		if(CHK_RandomPage_Exist(tc_Inx,tw_Page)){
			APort_FBlock[0]= APort_RBlock[0];
			gw_SourcePage = Mapping_Table[R_PAGE+tc_Offset+gc_NowIndex];
		}
		else{
			APort_FBlock[0]=  APort_LBlock[0];
			gw_SourcePage = tw_Page;
		}
		gw_TargetPage= tw_Page;					
		RW_Data_Stage(gw_SourcePage,gw_TargetPage,1);			
	}
	// Updata Mapping Table //
	NAND_Erase_Block_LEDR(0);// Erase old L_block 	
	Mapping_Table[SBK]=APort_LBlock[0];
	Wear_Table[SCNT]= (APort_LCNT[0]+1);
	Mapping_Table[tc_Temp_HBlock] =APort_SBlock[0]; 			 	
	Wear_Table[tc_Temp_HBlock] =APort_SCNT[0];	
	APort_SBlock[0]= Mapping_Table[SBK];	
	APort_SCNT[0] = Wear_Table[SCNT];	
	APort_LBlock[0] = Mapping_Table[HBlock.WD];
	APort_LCNT[0] = Wear_Table[HBlock.WD];
	NAND_Clear_R_PageFlag(tc_Inx);		
	
	//gw_R_SPage=Mapping_Table[RPAGEINX];不要亂般～～到時候再說
	//if(gw_R_SPage>= (H_INFO.wHBLOCKPAGE-H_INFO.cSMALLPAGE_NUM)){//保留至少32K的空間可以寫入不用換,跟哪一個group 無關
	//	NAND_Close_Random_Block();		   // 只是將現有的random page 從新整理
	//}
	
}


/*********************************************************************************
* NAME:               bit CHK_RandomPage_Space(U8 tc_Inx);
* DESCRIPTION:
*********************************************************************************/
U8 CHK_RandomPage_Space(U8 tc_Inx){
	xdata	U8	tc_Offset;
	xdata	U8	tc_i;
	data	bit	tbt_FindPage=0;
	tc_Offset = tc_Inx*MAX_RANDOM_HPAGE;
	for(tc_i=0; tc_i<MAX_RANDOM_HPAGE;tc_i++){
		if(Mapping_Table[R_HPAGE+tc_Offset+tc_i] ==0xFFFF){
			gc_NowIndex =tc_i;
			tbt_FindPage=1;
			break;
		}	
	}	
	return tbt_FindPage;
}


/*********************************************************************************
* NAME:               void NAND_Erase_Block_LEDR(U8 tc_Inx);
* DESCRIPTION:
*********************************************************************************/
void NAND_Erase_Block_LEDR(U8 tc_Inx){
	FDBP.cFDev=gc_HDev;	
 	FDBP.cFPlane =0;
 	FDBP.wFPage =0;
 	FDBP.cFInAddr =0;
 	switch(tc_Inx){
 		case 0:
 			FDBP.wFBlock =APort_LBlock[0]; 			
 			Flash_MultiBank_Erase(0);//not chk busy 	
 			break;
 		case 1:
 			FDBP.wFBlock=APort_EBlock[0];
 			
 			Flash_MultiBank_Erase(0);//not chk busy 	
 			break;		
 		case 2:
 			FDBP.wFBlock=APort_DBlock[0];
 			
 			Flash_MultiBank_Erase(0);//not chk busy 
 			break;	
 		case 3:
 			FDBP.wFBlock=APort_RBlock[0];
 			
 			Flash_MultiBank_Erase(0);//not chk busy 	
 			break;
 		default:
 			// nothing to do
 			break;
 	}
 	 	
 	
}
/*********************************************************************************
* NAME:               void NAND_Clear_D_SBlock_Flag(U8 tc_group);
* DESCRIPTION:
*********************************************************************************/
void NAND_Clear_D_SBlock_Flag(U8 tc_group){
	xdata	U8 tc_Offset;
	xdata	U8	tc_i;
//dbprintf("I ");	
	tc_Offset= (tc_group* MAX_DUMMY_PAGE); 
	for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
		Mapping_Table[D_HPAGE+tc_Offset+tc_i]=0xFFFF;
		Mapping_Table[D_PAGE+tc_Offset+tc_i]=0xFFFF;		
	}	
	Mapping_Table[Q1_INX+tc_group] =0x0000;
}
/*********************************************************************************
* NAME:               void NAND_Clear_D_SBlock_Flag(U8 tc_group);
* DESCRIPTION:// 先將clear info 寫入
*********************************************************************************/
void NAND_Clear_R_PageFlag(U8 tc_group){
	data	U8 tc_Offset;
	data	U8	tc_i;
//dbprintf("J ");	
	gw_R_SPage=Mapping_Table[RPAGEINX];
	/*//不能先清
	Wear_Table[RINX+tc_group]=0xFF;//0520 Jay add
	tc_Offset= (tc_group* MAX_RANDOM_HPAGE); 
	for(tc_i=0; tc_i<MAX_RANDOM_HPAGE;tc_i++){
		Mapping_Table[R_HPAGE+tc_Offset+tc_i]=0xFFFF;
		Mapping_Table[R_PAGE+tc_Offset+tc_i]=0xFFFF;
	}*/	
	if(gw_R_SPage==H_INFO.wHBLOCKPAGE){
		Wear_Table[RINX+tc_group]=0xFF;//0513將Random block 處理
		tc_Offset= (tc_group* MAX_RANDOM_HPAGE); 
		for(tc_i=0; tc_i<MAX_RANDOM_HPAGE;tc_i++){
			Mapping_Table[R_HPAGE+tc_Offset+tc_i]=0xFFFF;
			Mapping_Table[R_PAGE+tc_Offset+tc_i]=0xFFFF;
		}	
		//dbprintf("\n clear R_1");
		NAND_Close_Random_Block();
	}
	else{
		ZSpare[0] =0x55;
		ZSpare[1] =0xCC;// clear flag
		ZSpare[2] =Wear_Table[RINX+tc_group]; // 要被clear 的Block inx
		ZSpare[3] =0xCC; // clear flag 
		Wear_Table[RINX+tc_group]=0xFF;
		for(tc_i=0; tc_i<MAX_RANDOM_HBLOCK;tc_i++){
			ZSpare[4+tc_i]= Wear_Table[RINX+tc_i];
		}		
		Device_Write_SRAM_Index.BY[0] = 0x80;
		Device_Write_SRAM_Index.BY[1] = 0x00; 		
		FDBP.cFPlane = gw_R_SPage%NAND_INFO.cPLANE_NUM;
		FDBP.wFBlock = APort_RBlock[0];
		FDBP.wFPage  = gw_R_SPage/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr =0;
		Flash_Sequent_Write(4,1);
		gw_R_SPage++;
		Mapping_Table[RPAGEINX]=gw_R_SPage;
		//dbprintf("\n clear R_2");
		tc_Offset= (tc_group* MAX_RANDOM_HPAGE); 
		for(tc_i=0; tc_i<MAX_RANDOM_HPAGE;tc_i++){
			Mapping_Table[R_HPAGE+tc_Offset+tc_i]=0xFFFF;
			Mapping_Table[R_PAGE+tc_Offset+tc_i]=0xFFFF;
		}
		
	 }

}


/*********************************************************************************
* NAME:               void NAND_Update_PWR_INX(U8 tc_Inx);
* DESCRIPTION:
*********************************************************************************/
void NAND_Update_PWR_INX(U8 tc_Inx){
	xdata	U8	tc_Hblock;
	tc_Hblock =Mapping_Table[EINX+tc_Inx];
	FDBP.cFDev=gc_HDev;
	FDBP.cFPlane = 0;
	FDBP.wFBlock = Mapping_Table[tc_Hblock];
	FDBP.wFPage  = 0;
	FDBP.cFInAddr =0;  
	Device_Read_SRAM_Index.WD=0x8000;
	Flash_ReadSpare(4);  // get Hblock spare data 
	QTable.BY[0] = ZSpare[1];
	QTable.BY[1] = ZSpare[1]+1;
	Mapping_Table[PWR_INX+tc_Inx]=QTable.WD; 

}
/*********************************************************************************
* NAME:               void NAND_Write_Q1_Mark(U8 tc_Inx);
* DESCRIPTION:
*********************************************************************************/
void NAND_Write_Q1_Mark(U8 tc_Inx){
//dbprintf("L ");	
	ZSpare[0] = 0xA5; 							ZSpare[1] = (U8)Mapping_Table[PWR_INX+tc_Inx];
	ZSpare[2] = Mapping_Table[EINX+tc_Inx]; 	ZSpare[3] = Wear_Table[ECNT+tc_Inx];	
	ZSpare[4] = 'Q'; 							ZSpare[5] = Mapping_Table[Q1_INX+tc_Inx]>>8;
	ZSpare[6] = 'Q'; 							ZSpare[7] = Mapping_Table[Q1_INX+tc_Inx]>>8;
	//memset(XBYTE_8000,0xAA,1024);
	FDBP.cFDev=gc_HDev;
	FDBP.cFPlane = 0;
	FDBP.wFBlock = Mapping_Table[EBK+tc_Inx];
	FDBP.wFPage  = 0;
	FDBP.cFInAddr =0;  
	Device_Write_SRAM_Index.BY[0] = 0x80;
	Device_Write_SRAM_Index.BY[1] = 0x00; 		
	Flash_Sequent_Write(4,1);
	 

}


/*********************************************************************************
* NAME:               void NAND_Set_FBlock(U8 tc_Inx);
* DESCRIPTION:gc_write_EDRS 
*********************************************************************************/
void NAND_Set_FBlock(U8 tc_Inx){
	switch(tc_Inx){
		case 0:	// program  External Block
			APort_FBlock[0]=APort_EBlock[0];
			break;
		case 1: // program  dummy block 
			APort_FBlock[0]=APort_DBlock[0];
			break;
		case 2:	// program  Random block  
			APort_FBlock[0]=APort_RBlock[0];
			
			break;
		default: 
			APort_FBlock[0] = APort_SBlock[0];
			break;
	}

}
/*********************************************************************************
* NAME:               void NAND_Set_Mark_Register(U8 tc_Inx);
* DESCRIPTION:gc_write_EDRS 
*********************************************************************************/
void NAND_Set_Mark_Register(U8 tbt_2nd_data){
	xdata	U8	tc_i;
	for(tc_i=0; tc_i<4;tc_i++){
		if(tbt_2nd_data){
			FLASH_REG[0x60+tc_i] =ZSpare[4+tc_i];
		}
		else{
			FLASH_REG[0x60+tc_i] =ZSpare[tc_i];
 		}		
	}
}
/*********************************************************************************
* NAME:              void USB_To_BUf(U8 tc_BufPtr , bit tbt_1024);
* DESCRIPTION:gc_write_EDRS 
*********************************************************************************/
void USB_To_BUf(U8 tc_BufPtr , U8 tbt_1024){
	USB_CHK_Bank0_1_BulkOut_Done(0);
	Reset_DMA();
	DMA_REG[0x01]  = SRAM2SRAM;
	if(tbt_1024){				
		DMA_REG[0x03]  = 0x03;							
		DMA_REG[0x02]  = 0xFF;
	}
	else{
		DMA_REG[0x03]  = 0x01;							
		DMA_REG[0x02]  = 0xFF;
	}
	CPU_REG[0x13]  = USB_BUF_Bank0_HIGH;			
	CPU_REG[0x12]  = 0x00;
	CPU_REG[0x15]  = 0x80+(tc_BufPtr*2);
	CPU_REG[0x14]  = 0x00;
	Trigger_Data_Transfer_Done();		
	

}




/*********************************************************************************
* NAME:               void USB_Rcevie_Data_To_BUf(U8 tc_BufPtr , U8 tc_Xfercnt);
* DESCRIPTION:gc_write_EDRS 
*********************************************************************************/
void USB_Rcevie_Data_To_BUf(U8 tc_BufPtr , U8 tc_Xfercnt){
	bit tbt_USB_Sel=0;
	bit tbt_Xfer_512_OR_1024=0;
	
	if(gbt_USB_Data_In_Buf){
		gbt_USB_Data_In_Buf=0;
		if(tc_Xfercnt==0x01){
			USB_To_BUf(tc_BufPtr,0);//512
			tc_Xfercnt--;
			tc_BufPtr+=1;
		}
		else{
			USB_To_BUf(tc_BufPtr,1);//1024
			tc_Xfercnt-=2;
			tc_BufPtr+=2;
		}			
	}
	if(tc_Xfercnt){
		tbt_Xfer_512_OR_1024 =0;
		tbt_USB_Sel =0;		
		USB_Bank0_1_BulkOut(0,tbt_Xfer_512_OR_1024);
		tc_Xfercnt--;
		while(tc_Xfercnt){			
			if (tbt_USB_Sel) { //Bank 1-> RAM, USB -> Bank 0
				USB_CHK_Bank0_1_BulkOut_Done(1);
				if(!gbt_USB_Detected){
					return;
				}				
				DMA_REG[0x01]  = SRAM2SRAM;						//SRAM->SRAM
				DMA_REG[0x03]  = 0x01;							//512byte
				DMA_REG[0x02]  = 0xFF;
				CPU_REG[0x13]  = USB_BUF_Bank1_HIGH;			// 080104 joyce add
				CPU_REG[0x12]  = 0x00;
				CPU_REG[0x15]  = 0x80+(tc_BufPtr*2);
				CPU_REG[0x14]  = 0x00;
				Trigger_Data_Transfer();
				
				USB_Bank0_1_BulkOut(0,tbt_Xfer_512_OR_1024);
				
			}
			else{ //Bank 0 -> RAM, USB -> Bank 1
				USB_CHK_Bank0_1_BulkOut_Done(0);						
				if(!gbt_USB_Detected){
					return;
				}				
				DMA_REG[0x01]  = SRAM2SRAM;						//SRAM->SRAM
				DMA_REG[0x03]  = 0x01;							//512byte
				DMA_REG[0x02]  = 0xFF;
				CPU_REG[0x13]  = USB_BUF_Bank0_HIGH;			// 080104 joyce mark
				CPU_REG[0x12]  = 0x00;
				CPU_REG[0x15]  = 0x80+(tc_BufPtr*2);
				CPU_REG[0x14]  = 0x00;				
				Trigger_Data_Transfer();							//Trigger Data Xfer
				
				USB_Bank0_1_BulkOut(1,tbt_Xfer_512_OR_1024);
				
			}
			tbt_USB_Sel=!tbt_USB_Sel;
			tc_Xfercnt--;
			tc_BufPtr++;
			CHK_Data_Transfer_Done();			
		}
		if (tbt_USB_Sel) { //Bank 1-> RAM, USB -> Bank 0
			USB_CHK_Bank0_1_BulkOut_Done(1);
			if(!gbt_USB_Detected){
					return;
			}		
			DMA_REG[0x01]  = SRAM2SRAM;						//SRAM->SRAM
			DMA_REG[0x03]  = 0x01;							//512byte
			DMA_REG[0x02]  = 0xFF;
			CPU_REG[0x13]  = USB_BUF_Bank1_HIGH;			// 080104 joyce add
			CPU_REG[0x12]  = 0x00;
			CPU_REG[0x15]  = 0x80+(tc_BufPtr*2);
			CPU_REG[0x14]  = 0x00;			
			Trigger_Data_Transfer_Done();
		}
		else{ //Bank 0 -> RAM, USB -> Bank 1
				
			USB_CHK_Bank0_1_BulkOut_Done(0);						
			if(!gbt_USB_Detected){
					return;
			}		
			DMA_REG[0x01]  = SRAM2SRAM;						//SRAM->SRAM
			DMA_REG[0x03]  = 0x01;							//512byte
			DMA_REG[0x02]  = 0xFF;
			CPU_REG[0x13]  = USB_BUF_Bank0_HIGH;			// 080104 joyce mark
			CPU_REG[0x12]  = 0x00;
			CPU_REG[0x15]  = 0x80+(tc_BufPtr*2);
			CPU_REG[0x14]  = 0x00;				
			Trigger_Data_Transfer_Done();							//Trigger Data Xfer
				
		}
		
	}
	

}
/*********************************************************************************
* NAME:               void USB_Bank0_BulkOut(bit Xfer_1024){ (void)
* DESCRIPTION:
*********************************************************************************/
void USB_Bank0_1_BulkOut(U8 Use_Bank1 ,U8 Xfer_1024){
	USB_REG[0x11]       = 0x01;	//BulkoutEn auto turn-on(dma over)
	if(Xfer_1024){	
		USB_REG[0x19]  = 0x01; // 1024
	}
	else{
		USB_REG[0x19]  = 0x00;
	}
	if(Use_Bank1){
		USB_REG[0xE9]  = 0x20;							//Clear Bulk-Out(EP2) Buffer
		if(Xfer_1024){	
			//USB_REG[0x19]  = 0x01; // 1024
			USB_REG[0x16]  = 0x04;							//512Byte
			USB_REG[0x15]  = 0x00;
		}
		else{
			//USB_REG[0x19]  = 0x00; // 1024
			USB_REG[0x16]  = 0x02;							//512Byte
			USB_REG[0x15]  = 0x00;
		}
		USB_REG[0xA2]  = 0x02;		
	}
	else{
														//Allow Bulk-Out Tx.	
		USB_REG[0xE9]  = 0x02;							//Clear Bulk-Out(EP2) Buffer
		if(Xfer_1024){	
			//USB_REG[0x19]  = 0x01; // 1024
			USB_REG[0x16]  = 0x04;							
			USB_REG[0x15]  = 0x00;
		}
		else{
			//USB_REG[0x19]  = 0x00; 
			USB_REG[0x16]  = 0x02;							//512Byte
			USB_REG[0x15]  = 0x00;
		}
		USB_REG[0xA1]  = 0x02;							//Allow Bulk-Out Tx.	
	
	}
}
/*********************************************************************************
* NAME:               void USB_CHK_Bank0_1_BulkOut_Done(bit Chk_Bank1)
* DESCRIPTION:
*********************************************************************************/
void USB_CHK_Bank0_1_BulkOut_Done(U8 Chk_Bank1){
	if(Chk_Bank1){
		while(USB_REG[0xA2]&0x02){//Bank 1
			if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add			
				gbt_USB_Detected = 0;
				return ;
			}
		}
		USB_REG[0xC2] &= 0xDF;
		USB_REG[0x11]  = 0x00;	
	}
	else{
		while(USB_REG[0xA1]&0x02){  // Bank 0
			if(!(GLOBAL_REG[0xD1]&0x02)){// 081023 Jay add			
				gbt_USB_Detected = 0;
				return ;
			}
		}	
		USB_REG[0xC2] &= 0xFD;	
		USB_REG[0x11]  = 0x00;
	}
}

/***************************************************************************
* NAME:      void RW_Stage_EtoS(UWORD StartPage,UWORD EndPage,bit L_E)
* DESCRIPTION:
*     This subroutine is to do the Host Read Sector.
*     Input  Vars: small block 
***************************************************************************/
void RW_Data_Stage(UWORD tw_SourcePage,UWORD tw_TargetPage,U8 tc_Xfe_PageCnt ){ 
	
	#if 1// Normal R W
	data	U8 tc_i;	
	//U8 tc_Temp;
	FDBP.cFDev =gc_HDev;
	for(tc_i=0; tc_i<tc_Xfe_PageCnt ;tc_i++){
		//-------------------------------------- //
		Device_Read_SRAM_Index.BY[0] = 0x80;
		Device_Read_SRAM_Index.BY[1] = 0x00; 		
		FDBP.cFPlane = tw_SourcePage%NAND_INFO.cPLANE_NUM;
		FDBP.wFBlock = APort_FBlock[0];// source block 
		FDBP.wFPage  = tw_SourcePage/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr =0;
		Flash_Sequent_Read(NAND_INFO.cINADDRMAX);
		//--------------------------------------//
		Device_Write_SRAM_Index.BY[0] = 0x80;
		Device_Write_SRAM_Index.BY[1] = 0x00; 		
		FDBP.cFPlane = tw_TargetPage%NAND_INFO.cPLANE_NUM;
		FDBP.wFBlock = APort_FBlock[1];// Target block
		FDBP.wFPage  = tw_TargetPage/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr =0;
		Flash_Sequent_Write(NAND_INFO.cINADDRMAX,1);
		tw_SourcePage++;
		tw_TargetPage++;
	}
	#endif 

}
/*********************************************************************************
* NAME:                  void NAND_OverWrite_Mode(U8 tc_Extend_Count)
* DESCRIPTION:以SBlock 方式來處理
*********************************************************************************/
void NAND_OverWrite_Mode(U8 tc_Extend_Count){
	data	U8	tc_XferPageCnt;
	data	U8	tc_Pre_XferCnt;
	data	U8	tc_BigSectorCnt;
	data	U8	tc_SmallSectorCnt;
	data	U8	tc_Offset;
	data	U8	tc_Page_Offset;
	data	bit	tbt_DoSendCommand;
	gbt_Send_80_81_CMD =0;
	tc_Page_Offset=0;
	//NAND_Set_Mark_Register(0);// set mark register	
	
	if(gbt_Data_In_Buf){// 處理先前零碎或是無法寫滿一個page
		if(gc_HInAddr){
			tc_XferPageCnt= NAND_INFO.cINADDRMAX-gc_HInAddr;
		}
		else{	
			tc_XferPageCnt = NAND_INFO.cINADDRMAX;
		}
		tc_Pre_XferCnt = (tc_XferPageCnt<=TotalXferPageNum.WD)? tc_XferPageCnt: TotalXferPageNum.WD;		
		USB_Rcevie_Data_To_BUf(gc_HInAddr,tc_Pre_XferCnt);	
		TotalXferPageNum.WD = TotalXferPageNum.WD- tc_Pre_XferCnt;
		Device_Write_SRAM_Index.BY[0] = 0x80;			
		Device_Write_SRAM_Index.BY[1] = 0x00;
		NAND_SBlock_SPage_To_FDBP(); // 
		Flash_Sequent_Write(NAND_INFO.cINADDRMAX,1);
		gbt_USB_Data_In_Buf =0;
		gbt_Data_In_Buf =0;
		gc_SPage++;
		gw_TargetPage++;	
		gc_HInAddr =0;
		HPage.WD++;
		tc_Page_Offset ++;
		if(!gbt_USB_Detected){
			goto USB_CLOSE_BLOCK;
		}
		
	}
	
	tc_BigSectorCnt = TotalXferPageNum.WD/NAND_INFO.cINADDRMAX;
	tc_SmallSectorCnt = TotalXferPageNum.WD%NAND_INFO.cINADDRMAX;
	if(gc_SPage==H_INFO.cSMALLPAGE_NUM){
		tc_Offset = tc_Extend_Count*MAX_DUMMY_PAGE;
		SQTable.BY[1] = gc_SPage;
		Mapping_Table[D_PAGE+tc_Offset+gc_NowIndex] = SQTable.WD;
		QTable.BY[0] =gc_SBlock;
		QTable.BY[1] =gc_T_SBlock;
		Mapping_Table[D_HPAGE+tc_Offset+gc_NowIndex] = QTable.WD;		
		QTable.WD= Mapping_Table[Q1_INX+tc_Extend_Count];
		QTable.BY[1] =gc_T_SBlock+1;
		Mapping_Table[Q1_INX+tc_Extend_Count]=QTable.WD;
		return;
	}
	if(tc_BigSectorCnt){//full page write
		if(!gbt_USB_Data_In_Buf){
			USB_Bank0_1_BulkOut(0,1);
			USB_CHK_Bank0_1_BulkOut_Done(0);		
		}
		if(!gbt_USB_Detected){
			goto USB_CLOSE_BLOCK;
		}		
		NAND_SBlock_SPage_To_FDBP(); // 
		if(FDBP.cFPlane||(tc_BigSectorCnt==1)||(gc_SPage == (H_INFO.cSMALLPAGE_NUM-1))){
			gc_Flash_CMD1 =0x80;
			gc_Flash_CMD2 =0x10;
			gbt_Send_80_81_CMD =0;
		}
		else{
			gc_Flash_CMD1 =0x80;
			if(NAND_INFO.cSUPPORT_CACHE_RW){			
				gc_Flash_CMD2 =0x15;// 2_plan:0x11
			}
			else{
				gc_Flash_CMD2=0x10;
			}
			gbt_Send_80_81_CMD =1;
		}		
		NAND_Set_Mark_Register(0);
		gc_Flash_CMD_Type = 0x0F;//0630 Jay fix for Hynix M die
		Device_Write_SRAM_Index.BY[0] = USB_BUF_Bank0_HIGH;;
		Device_Write_SRAM_Index.BY[1] = 0x00;			
		Device_Xfer_DataLength.WD = 0x03FF;			
		SET_DEVICE_WRITE();
		ENA_ECC_Encode();
		TRIGGER_FLASH_CMD_COL_ADDR_CMD();
		tbt_DoSendCommand =0;
		gbt_OddTemp=0;
		gc_HInAddr =0;
		while(tc_BigSectorCnt) {// 處理以page為單位的data		
			
			if (gc_SPage == H_INFO.cSMALLPAGE_NUM) {
				
				tc_Offset = tc_Extend_Count*MAX_DUMMY_PAGE;
				SQTable.BY[1] = gc_SPage;			
				Mapping_Table[D_PAGE+tc_Offset+gc_NowIndex] =SQTable.WD;
				QTable.BY[0] =gc_SBlock;
				QTable.BY[1] =gc_T_SBlock;
				Mapping_Table[D_HPAGE+tc_Offset+gc_NowIndex] = QTable.WD;
				QTable.WD= Mapping_Table[Q1_INX+tc_Extend_Count];
				QTable.BY[1] =gc_T_SBlock+1;
				Mapping_Table[Q1_INX+tc_Extend_Count]=QTable.WD;				
				
				return ;
			}			
			
			if (tbt_DoSendCommand) {
				NAND_SBlock_SPage_To_FDBP();
				if(gbt_Send_80_81_CMD){
					gc_Flash_CMD1 =0x80;
					if(NAND_INFO.cSUPPORT_CACHE_RW){			
						gc_Flash_CMD2 =0x15;// 2_plan:0x11
					}
					else{
						gc_Flash_CMD2=0x10;
					}
					gbt_Send_80_81_CMD=0;
				}
				else{
					gc_Flash_CMD1 =0x80;
					if(NAND_INFO.cSUPPORT_CACHE_RW){			
						gc_Flash_CMD2 =0x15;// 2_plan:0x11
					}
					else{
						gc_Flash_CMD2=0x10;
					}
					gbt_Send_80_81_CMD=1;
				}					
				if((tc_BigSectorCnt==1)||(gc_SPage == (H_INFO.cSMALLPAGE_NUM-1))){
					gc_Flash_CMD2 = 0x10;
				}
				gc_Flash_CMD_Type = 0x0F;//0630 Jay fix for Hynix M die				
				tbt_DoSendCommand =0;
				TRIGGER_FLASH_CMD_COL_ADDR_CMD();	
			}			
			for(gc_HInAddr=0; gc_HInAddr<NAND_INFO.cINADDRMAX;gc_HInAddr+=2){
				if(gc_HInAddr==(NAND_INFO.cINADDRMAX-2)){
					FLASH_REG[0x1A] =0x11;
					if((tc_BigSectorCnt==1)||(gc_SPage == (H_INFO.cSMALLPAGE_NUM-1))){
						tc_Pre_XferCnt =0;
					}
					else{
						tc_Pre_XferCnt =2;
					}				
				}
				else{
					FLASH_REG[0x1A] =0x10;
					tc_Pre_XferCnt =2;
				}		

				NAND_Set_Mark_Register((gc_HInAddr>>1)&0x01);
					
				if(gbt_OddTemp) { //Bank 1-> Flash, USB -> Bank 0
					USB_CHK_Bank0_1_BulkOut_Done(1); //chk Bank 0 done
					if(!gbt_USB_Detected){
						goto USB_CLOSE_BLOCK;
					}		
					CPU_REG[0x13]    = USB_BUF_Bank1_HIGH;			// 080104 joyce add
					CPU_REG[0x12]    = 0x00;
					DEVICE_REG[0xA0] = 0x01;						//Reset ECC
					FLASH_REG[0xA0]  = 0x01;						//Start Encode RS-Code
					Trigger_Data_Transfer();								
					if(tc_Pre_XferCnt){
						USB_Bank0_1_BulkOut(0,1);
					}
				}
				else { //Bank 0 -> Flash, USB -> Bank 1
					USB_CHK_Bank0_1_BulkOut_Done(0);// chk Bank 1 done		
					if(!gbt_USB_Detected){
						goto USB_CLOSE_BLOCK;
					}		
					
					CPU_REG[0x13]    = USB_BUF_Bank0_HIGH;			// 080104 joyce mark
					CPU_REG[0x12]    = 0x00;
					DEVICE_REG[0xA0] = 0x01;						//Reset ECC
					FLASH_REG[0xA0]  = 0x01;						//Start Encode RS-Code										
					Trigger_Data_Transfer();						
					if(tc_Pre_XferCnt){
						USB_Bank0_1_BulkOut(1,1);
						
					}
				}		
				gbt_OddTemp = !(gbt_OddTemp);
				TotalXferPageNum.WD-=2;
				CHK_Data_Transfer_Done();
				while (FLASH_REG[0xA2] & 0x40);							
				CHK_NandAutocmp_Done();
			}			
			if(gc_HInAddr==NAND_INFO.cINADDRMAX){
				FLASH_REG[0x06]  = gc_Flash_CMD2;//	0630 Jay add 								
				FLASH_REG[0x04] = 0x08;										
				TRIGGER_FLASH_CMD();
				gc_HInAddr =0;
				gc_SPage++;
				gw_TargetPage++;
				tc_BigSectorCnt--;
				HPage.WD++;
				tc_Page_Offset ++;
				tbt_DoSendCommand = 1;								
				gbt_USB_Data_In_Buf=0;//目前usb data buf 內已經沒有任何資料了～
			}
			
		}// end while loop	
		if (gc_SPage == H_INFO.cSMALLPAGE_NUM) {
				
			tc_Offset = tc_Extend_Count*MAX_DUMMY_PAGE;
			SQTable.BY[1] = gc_SPage;			
			Mapping_Table[D_PAGE+tc_Offset+gc_NowIndex] =SQTable.WD;
			QTable.BY[0] =gc_SBlock;
			QTable.BY[1] =gc_T_SBlock;
			Mapping_Table[D_HPAGE+tc_Offset+gc_NowIndex] = QTable.WD;
			QTable.WD= Mapping_Table[Q1_INX+tc_Extend_Count];
			QTable.BY[1] =gc_T_SBlock+1;
			Mapping_Table[Q1_INX+tc_Extend_Count]=QTable.WD;				
			
			return ;
		}	
	
	
	
	}

	if(tc_SmallSectorCnt){	//處理後面的零碎檔案
		
		tc_Offset = tc_Extend_Count*MAX_DUMMY_PAGE;//0622 Jay fix
		QTable.WD= Mapping_Table[D_PAGE+tc_Offset+gc_NowIndex];
		if((gc_SPage>=QTable.BY[0])&&(gc_SPage<QTable.BY[1])){
			APort_FBlock[0]= APort_DBlock[0];
			gw_SourcePage=((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+(gc_SPage-QTable.BY[0]);
			
		}
		else{		
			if(gc_SBlock<gc_Q1_SBlock){
				APort_FBlock[0] = APort_LBlock[0];
			}
			else{			
				APort_FBlock[0]= APort_EBlock[0];
			}					
			gw_SourcePage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
		}
		NAND_Laod_Data_To_Buf(gw_SourcePage,0);
		//---------- USB To RAM----------//
		gbt_OddTemp=0;	
		USB_Rcevie_Data_To_BUf(gc_HInAddr,tc_SmallSectorCnt);
		TotalXferPageNum.WD-=tc_SmallSectorCnt;
		Device_Write_SRAM_Index.BY[0] = 0x80;			
		Device_Write_SRAM_Index.BY[1] = 0x00;
		APort_FBlock[0]= APort_DBlock[0];		
		NAND_SBlock_SPage_To_FDBP();		
		Flash_Sequent_Write(NAND_INFO.cINADDRMAX,1);
		gbt_USB_Data_In_Buf =0;
		gc_SPage++;
		gw_TargetPage++;	
		gc_HInAddr =0;
		HPage.WD++;
		tc_Page_Offset ++;
		
	}
USB_CLOSE_BLOCK:	
	if (gc_SPage == H_INFO.cSMALLPAGE_NUM) {
		tc_Offset = tc_Extend_Count*MAX_DUMMY_PAGE;
		SQTable.BY[1] = gc_SPage;			
		Mapping_Table[D_PAGE+tc_Offset+gc_NowIndex] =SQTable.WD;
		QTable.BY[0] =gc_SBlock;
		QTable.BY[1] =gc_T_SBlock;
		Mapping_Table[D_HPAGE+tc_Offset+gc_NowIndex] = QTable.WD;
		QTable.WD= Mapping_Table[Q1_INX+tc_Extend_Count];
		QTable.BY[1] =gc_T_SBlock+1;
		Mapping_Table[Q1_INX+tc_Extend_Count]=QTable.WD;
		
		return ;
	}		
	else{// 重新整理Table	
		
		gc_Q1_SBlock = (Mapping_Table[Q1_INX+tc_Extend_Count]>>8);
		tc_Offset = tc_Extend_Count*MAX_DUMMY_PAGE;
		QTable.WD= Mapping_Table[D_PAGE+tc_Offset+gc_NowIndex];
		
		if(gc_SPage<QTable.BY[0]){			
			
			if(gc_SBlock<gc_Q1_SBlock){
				APort_FBlock[0] = APort_LBlock[0];
			}
			else{			
				APort_FBlock[0]= APort_EBlock[0];
			}			
			APort_FBlock[1]= APort_DBlock[0];
			tc_XferPageCnt = QTable.BY[0]-gc_SPage;
			
			gw_SourcePage=((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
       		//gw_TargetPage=((U16)gc_T_SBlock*H_INFO.cSMALLPAGE_NUM)+tc_Page_Offset;//0520 Jay mark					
			RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
			//tc_Page_Offset += tc_XferPageCnt;//0520 Jay mark
			gw_TargetPage+=tc_XferPageCnt;//0520 Jay add
			// move Q1~Q2 to new D SBlock 	
			APort_FBlock[0]= APort_DBlock[0];
			APort_FBlock[1]= APort_DBlock[0];
			tc_XferPageCnt = QTable.BY[1]-QTable.BY[0];
			
			gw_SourcePage=((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+0;
       		//gw_TargetPage=((U16)gc_T_SBlock*H_INFO.cSMALLPAGE_NUM)+tc_Page_Offset;//0520 Jay mark					
			RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
			//tc_Offset = tc_Extend_Count*MAX_RANDOM_HPAGE;
			Mapping_Table[D_PAGE+tc_Offset+gc_NowIndex] =SQTable.WD;
			
			
		}		
		else if((gc_SPage>=QTable.BY[0])&&(gc_SPage<QTable.BY[1])){
			
			APort_FBlock[0]= APort_DBlock[0];
			APort_FBlock[1]= APort_DBlock[0];
			tc_XferPageCnt = QTable.BY[1]-gc_SPage;
			
			gw_SourcePage=((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+(gc_SPage-QTable.BY[0]);
       		//gw_TargetPage=((U16)gc_T_SBlock*H_INFO.cSMALLPAGE_NUM)+tc_Page_Offset;//0520 Jay mark 					
			RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
			//tc_Offset = tc_Extend_Count*MAX_RANDOM_HPAGE;
			Mapping_Table[D_PAGE+tc_Offset+gc_NowIndex] =SQTable.WD;
			
			
		}
		else{  //gc_SPage>QTable.BY[1]
			//tc_Offset = tc_Extend_Count*MAX_RANDOM_HPAGE;
			SQTable.BY[1] = gc_SPage;
			Mapping_Table[D_PAGE+tc_Offset+gc_NowIndex] =SQTable.WD;
			
		}
	
		QTable.BY[0] =gc_SBlock;
		QTable.BY[1] =gc_T_SBlock;		
		Mapping_Table[D_HPAGE+tc_Offset+gc_NowIndex] = QTable.WD;
		QTable.WD= Mapping_Table[Q1_INX+tc_Extend_Count];
		QTable.BY[1] =gc_T_SBlock+1;		
		Mapping_Table[Q1_INX+tc_Extend_Count]=QTable.WD;
	}
	
}

/*********************************************************************************
* NAME:                  void NAND_OverWrite_Mode(U8 tc_Extend_Count)
* DESCRIPTION:
*********************************************************************************/
void NAND_SequentWrite_Mode(U8 tc_Extend_Count)
{
	data	bit	tbt_Find_D_SBlock;	
	data	U8	tc_XferPageCnt;
	data	U8	tc_Pre_XferCnt;
	data	U8	tc_BigSectorCnt;
	data	U8	tc_SmallSectorCnt;
	data	U8	tc_Offset;
	data	U8	tc_i;
	data	bit tbt_DoSendCommand;
	gbt_Send_80_81_CMD =0;
	//tc_Page_Offset=0;	
	tbt_Find_D_SBlock=0;
	USB_PlugDetect();											// 081014 Jay add for usb plug out
	if(!gbt_USB_Detected){
		return ;
	}
	NAND_Set_Mark_Register(0);// set mark register
	if(gbt_Data_In_Buf){// 處理先前零碎或是無法寫滿一個page
		if(gc_HInAddr){
			tc_XferPageCnt= NAND_INFO.cINADDRMAX-gc_HInAddr;
		}
		else{	
			tc_XferPageCnt = NAND_INFO.cINADDRMAX;
		}
		tc_Pre_XferCnt = (tc_XferPageCnt<=TotalXferPageNum.WD)? tc_XferPageCnt: TotalXferPageNum.WD;
		USB_Rcevie_Data_To_BUf(gc_HInAddr,tc_Pre_XferCnt);	
		TotalXferPageNum.WD = TotalXferPageNum.WD- tc_Pre_XferCnt;
		Device_Write_SRAM_Index.BY[0] = 0x80;			
		Device_Write_SRAM_Index.BY[1] = 0x00;
		NAND_SBlock_SPage_To_FDBP(); // 
		Flash_Sequent_Write(NAND_INFO.cINADDRMAX,1);
		gbt_USB_Data_In_Buf =0;
		gbt_Data_In_Buf =0;
		gc_SPage++;
		gw_TargetPage++;	
		gc_HInAddr =0;
		HPage.WD++;
		//tc_Page_Offset ++;
		if(gw_TargetPage==H_INFO.wHBLOCKPAGE){// 準備換HBlock 　
			gc_Q1_SBlock = (Mapping_Table[Q1_INX+tc_Extend_Count]>>8);
			tc_Offset= (tc_Extend_Count* MAX_DUMMY_PAGE); 
			tbt_Find_D_SBlock =0;
			for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
				if(Mapping_Table[D_HPAGE+tc_Offset+tc_i]!=0xFFFF){
					tbt_Find_D_SBlock=1;
					break;
				}
			}
			if(tbt_Find_D_SBlock||gc_Q1_SBlock){
				QTable.BY[0] =H_INFO.cSMALLBLOCK_NUM;
				QTable.BY[1] = 0x00;
				Mapping_Table[Q2_INX+tc_Extend_Count]=QTable.WD;	
				NAND_Combin_E_D_To_New_E(tc_Extend_Count,H_INFO.cSMALLBLOCK_NUM);
			}		
			Mapping_Table[HBlock.WD] =APort_EBlock[0];
			Wear_Table[HBlock.WD] =APort_ECNT[0];
			NAND_Erase_Block_LEDR(0);
			//Mapping_Table[DBK+tc_Extend_Count] =APort_LBlock[0];
			//Wear_Table[DCNT+tc_Extend_Count]= APort_LCNT[0]+1;
			/*if(!tbt_Find_D_SBlock){
				NAND_Erase_Block_LEDR(2);
				APort_DCNT[0]+=1;	
			}*/
			Mapping_Table[SBK] =APort_LBlock[0];
			Wear_Table[SCNT] =(APort_LCNT[0]+1);
			Mapping_Table[EBK+tc_Extend_Count] =APort_SBlock[0];
			Wear_Table[ECNT+tc_Extend_Count]=APort_SCNT[0];
			Mapping_Table[EINX+tc_Extend_Count]=0xFFFF;
			Mapping_Table[Q2_INX+tc_Extend_Count]=0x0000;			
			Mapping_Table[Q1_INX+tc_Extend_Count] =0x0000;
			//dbprintf("\n0 seq return");				
			return;
		}	

	}
	if(!gbt_USB_Detected){
		goto USB_CLOSE_E_BLOCK;
	}
	//P3_0=0;P3_0=1;
	tc_BigSectorCnt = TotalXferPageNum.WD/NAND_INFO.cINADDRMAX;
	tc_SmallSectorCnt= TotalXferPageNum.WD%NAND_INFO.cINADDRMAX;
	
	
	if(tc_BigSectorCnt){//full page write
		
		if(!gbt_USB_Data_In_Buf){
			USB_Bank0_1_BulkOut(0,1);
		}
		
		NAND_SBlock_SPage_To_FDBP(); // 
		if(FDBP.cFPlane||(tc_BigSectorCnt==1)||(gw_TargetPage == (H_INFO.wHBLOCKPAGE-1))){
			gc_Flash_CMD1 =0x80;
			gc_Flash_CMD2 =0x10;
			gbt_Send_80_81_CMD =0;
		}
		else{
			gc_Flash_CMD1 =0x80;
			if(NAND_INFO.cSUPPORT_CACHE_RW){			
				gc_Flash_CMD2 =0x15;// 2_plan:0x11
			}
			else{
				gc_Flash_CMD2=0x10;
			}
			gbt_Send_80_81_CMD =1;
		}		
		NAND_Set_Mark_Register(0);
		gc_Flash_CMD_Type = 0x0F;//0630 Jay fix for Hynix M die
		Device_Write_SRAM_Index.BY[0] = USB_BUF_Bank0_HIGH;;
		Device_Write_SRAM_Index.BY[1] = 0x00;			
		Device_Xfer_DataLength.WD = 0x03FF;			
		SET_DEVICE_WRITE();
		ENA_ECC_Encode();
		TRIGGER_FLASH_CMD_COL_ADDR_CMD();				
		tbt_DoSendCommand =0;
		gbt_OddTemp=0;
		gc_HInAddr =0;
		
		while(tc_BigSectorCnt) {// 處理以page為單位的data		
			if (gw_TargetPage == H_INFO.wHBLOCKPAGE) {
				
				gc_Q1_SBlock = (Mapping_Table[Q1_INX+tc_Extend_Count]>>8);
				tc_Offset= (tc_Extend_Count* MAX_DUMMY_PAGE); 
				tbt_Find_D_SBlock =0;
				for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
					if(Mapping_Table[D_HPAGE+tc_Offset+tc_i]!=0xFFFF){
						tbt_Find_D_SBlock=1;
						break;
					}
				}
				if(tbt_Find_D_SBlock||gc_Q1_SBlock){
					QTable.BY[0] =H_INFO.cSMALLBLOCK_NUM;
					QTable.BY[1] = 0x00;
					Mapping_Table[Q2_INX+tc_Extend_Count]=QTable.WD;	
					NAND_Combin_E_D_To_New_E(tc_Extend_Count,H_INFO.cSMALLBLOCK_NUM);
				}			
				Mapping_Table[HBlock.WD] =APort_EBlock[0];
				Wear_Table[HBlock.WD] =APort_ECNT[0];
				NAND_Erase_Block_LEDR(0);
				//Mapping_Table[DBK+tc_Extend_Count] =APort_LBlock[0];
				//Wear_Table[DCNT+tc_Extend_Count]= APort_LCNT[0]+1;
				/*if(!tbt_Find_D_SBlock){
					NAND_Erase_Block_LEDR(2);
					APort_DCNT[0]+=1;	
				}*/
				Mapping_Table[SBK] =APort_LBlock[0];
				Wear_Table[SCNT] =(APort_LCNT[0]+1);
				Mapping_Table[EBK+tc_Extend_Count] =APort_SBlock[0];
				Wear_Table[ECNT+tc_Extend_Count]=APort_SCNT[0];
				Mapping_Table[EINX+tc_Extend_Count]=0xFFFF;
				Mapping_Table[Q2_INX+tc_Extend_Count]=0x0000;			
				Mapping_Table[Q1_INX+tc_Extend_Count] =0x0000;
				//dbprintf("\n1 seq return");				
				return ;
			}			
			if (tbt_DoSendCommand) {
				NAND_SBlock_SPage_To_FDBP();
				if(gbt_Send_80_81_CMD){
					gc_Flash_CMD1 =0x80;
					if(NAND_INFO.cSUPPORT_CACHE_RW){			
						gc_Flash_CMD2 =0x15;// 2_plan:0x11
					}
					else{
						gc_Flash_CMD2=0x10;
					}//2_plan 0x11 cache :0x15
					gbt_Send_80_81_CMD=0;
				}
				else{
					gc_Flash_CMD1 =0x80;
					if(NAND_INFO.cSUPPORT_CACHE_RW){			
						gc_Flash_CMD2 =0x15;// 2_plan:0x11
					}
					else{
						gc_Flash_CMD2=0x10;
					}
					gbt_Send_80_81_CMD=1;
				}					
				if((tc_BigSectorCnt==1)||(gw_TargetPage == (H_INFO.wHBLOCKPAGE-1))){
					gc_Flash_CMD2 = 0x10;
				}
				gc_Flash_CMD_Type = 0x0F;		//0630 Jay fix for Hynix M die		
				tbt_DoSendCommand =0;
				TRIGGER_FLASH_CMD_COL_ADDR_CMD();	
			}			
			
			for(gc_HInAddr=0; gc_HInAddr<NAND_INFO.cINADDRMAX;gc_HInAddr+=2){
				
				if(gc_HInAddr==(NAND_INFO.cINADDRMAX-2)){
					FLASH_REG[0x1A] =0x11;
					
					if((tc_BigSectorCnt==1)||(gw_TargetPage == (H_INFO.wHBLOCKPAGE-1))){
						tc_Pre_XferCnt =0;
					}
					else{
						tc_Pre_XferCnt =2;
					}				
				}
				else{
					FLASH_REG[0x1A] =0x10;
					tc_Pre_XferCnt =2;
				}		
				
				NAND_Set_Mark_Register((gc_HInAddr>>1)&0x01);
							
				if(gbt_OddTemp) { //Bank 1-> Flash, USB -> Bank 0
					
					USB_CHK_Bank0_1_BulkOut_Done(1); //chk Bank 0 done
					if(!gbt_USB_Detected){
						goto USB_CLOSE_E_BLOCK;
					}					
					CPU_REG[0x13]    = USB_BUF_Bank1_HIGH;			// 080104 joyce add
					CPU_REG[0x12]    = 0x00;
					DEVICE_REG[0xA0] = 0x01;						//Reset ECC
					FLASH_REG[0xA0]  = 0x01;						//Start Encode RS-Code
					Trigger_Data_Transfer();
					
					if(tc_Pre_XferCnt){							
						USB_Bank0_1_BulkOut(0,1);
						
					}
				}
				else { //Bank 0 -> Flash, USB -> Bank 1
					
					USB_CHK_Bank0_1_BulkOut_Done(0);// chk Bank 1 done		
					if(!gbt_USB_Detected){
						goto USB_CLOSE_E_BLOCK;
					}
					
					CPU_REG[0x13]    = USB_BUF_Bank0_HIGH;			// 080104 joyce mark
					CPU_REG[0x12]    = 0x00;
					DEVICE_REG[0xA0] = 0x01;						//Reset ECC
					FLASH_REG[0xA0]  = 0x01;						//Start Encode RS-Code										
					Trigger_Data_Transfer();
					if(tc_Pre_XferCnt){						
						USB_Bank0_1_BulkOut(1,1);
						
					}
				}		
				gbt_OddTemp = !(gbt_OddTemp);
				TotalXferPageNum.WD-=2;
								
				CHK_Data_Transfer_Done();
				while (FLASH_REG[0xA2] & 0x40);							
				CHK_NandAutocmp_Done();
			}			
			if(gc_HInAddr==NAND_INFO.cINADDRMAX){
				FLASH_REG[0x06]  = gc_Flash_CMD2;//	0630 Jay add 								
				FLASH_REG[0x04] = 0x08;										
				TRIGGER_FLASH_CMD();
				gc_HInAddr =0;
				gc_SPage++;
				gw_TargetPage++;
				tc_BigSectorCnt--;
				HPage.WD++;				
				tbt_DoSendCommand = 1;								
				gbt_USB_Data_In_Buf=0;//目前usb data buf 內已經沒有任何資料了～
			}	
			
		}// end while loop
		if (gw_TargetPage == H_INFO.wHBLOCKPAGE) {
				gc_Q1_SBlock = (Mapping_Table[Q1_INX+tc_Extend_Count]>>8);
				tc_Offset= (tc_Extend_Count* MAX_DUMMY_PAGE); 
				tbt_Find_D_SBlock =0;
				for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
					if(Mapping_Table[D_HPAGE+tc_Offset+tc_i]!=0xFFFF){
						tbt_Find_D_SBlock=1;
						break;
					}
				}
				if(tbt_Find_D_SBlock||gc_Q1_SBlock){
					QTable.BY[0] =H_INFO.cSMALLBLOCK_NUM;
					QTable.BY[1] = 0x00;
					Mapping_Table[Q2_INX+tc_Extend_Count]=QTable.WD;	
					NAND_Combin_E_D_To_New_E(tc_Extend_Count,H_INFO.cSMALLBLOCK_NUM);
				}			
				
				Mapping_Table[HBlock.WD] =APort_EBlock[0];
				Wear_Table[HBlock.WD] =APort_ECNT[0];
				NAND_Erase_Block_LEDR(0);
				//Mapping_Table[DBK+tc_Extend_Count] =APort_LBlock[0];
				//Wear_Table[DCNT+tc_Extend_Count]= APort_LCNT[0]+1;
				/*if(!tbt_Find_D_SBlock){
					NAND_Erase_Block_LEDR(2);
					APort_DCNT[0]+=1;	
				}*/
				Mapping_Table[SBK] =APort_LBlock[0];
				Wear_Table[SCNT] =(APort_LCNT[0]+1);
				Mapping_Table[EBK+tc_Extend_Count] =APort_SBlock[0];
				Wear_Table[ECNT+tc_Extend_Count]=APort_SCNT[0];
				Mapping_Table[EINX+tc_Extend_Count]=0xFFFF;
				Mapping_Table[Q2_INX+tc_Extend_Count]=0x0000;			
				Mapping_Table[Q1_INX+tc_Extend_Count] =0x0000;
				//dbprintf("\ntc_bigend seq return");				
				return ;
		}
		
	}
	if(tc_SmallSectorCnt){	//處理後面的零碎檔案
	
		APort_FBlock[0]= APort_LBlock[0];					
		gw_SourcePage=HPage.WD;
		NAND_Laod_Data_To_Buf(gw_SourcePage,0);
		
		//---------- USB To RAM----------//
		gbt_OddTemp=0;		
		USB_Rcevie_Data_To_BUf(gc_HInAddr,tc_SmallSectorCnt);
		TotalXferPageNum.WD-=tc_SmallSectorCnt;
		Device_Write_SRAM_Index.BY[0] = 0x80;			
		Device_Write_SRAM_Index.BY[1] = 0x00;
		APort_FBlock[0]= APort_EBlock[0];
		
		NAND_SBlock_SPage_To_FDBP(); // 
		Flash_Sequent_Write(NAND_INFO.cINADDRMAX,1);//have marked 
		gbt_USB_Data_In_Buf =0;
		gbt_Data_In_Buf =0;
		gw_TargetPage++;	
		gc_HInAddr =0;
		HPage.WD++;
		
	}
USB_CLOSE_E_BLOCK:
	if (gw_TargetPage == H_INFO.wHBLOCKPAGE)  {
		
		gc_Q1_SBlock = (Mapping_Table[Q1_INX+tc_Extend_Count]>>8);
		tc_Offset= (tc_Extend_Count* MAX_DUMMY_PAGE); 
		tbt_Find_D_SBlock =0;
		for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
			if(Mapping_Table[D_HPAGE+tc_Offset+tc_i]!=0xFFFF){
				tbt_Find_D_SBlock=1;
				break;
			}
		}
		if(tbt_Find_D_SBlock||gc_Q1_SBlock){
			QTable.BY[0] =H_INFO.cSMALLBLOCK_NUM;
			QTable.BY[1] = 0x00;
			Mapping_Table[Q2_INX+tc_Extend_Count]=QTable.WD;	
			NAND_Combin_E_D_To_New_E(tc_Extend_Count,H_INFO.cSMALLBLOCK_NUM);
		}		
		Mapping_Table[HBlock.WD] =APort_EBlock[0];
		Wear_Table[HBlock.WD] =APort_ECNT[0];
		NAND_Erase_Block_LEDR(0);
		//Mapping_Table[DBK+tc_Extend_Count] =APort_LBlock[0];
		//Wear_Table[DCNT+tc_Extend_Count]= APort_LCNT[0]+1;
		/*if(!tbt_Find_D_SBlock){
			NAND_Erase_Block_LEDR(2);
			APort_DCNT[0]+=1;	
		}*/
		Mapping_Table[SBK] =APort_LBlock[0];
		Wear_Table[SCNT] =(APort_LCNT[0]+1);
		Mapping_Table[EBK+tc_Extend_Count] =APort_SBlock[0];
		Wear_Table[ECNT+tc_Extend_Count]=APort_SCNT[0];
		Mapping_Table[EINX+tc_Extend_Count]=0xFFFF;
		Mapping_Table[Q2_INX+tc_Extend_Count]=0x0000;			
		Mapping_Table[Q1_INX+tc_Extend_Count] =0x0000;
		
		return ;
	}		
	else{
		
		QTable.BY[0] = HPage.WD/H_INFO.cSMALLPAGE_NUM;
		QTable.BY[1] = HPage.WD%H_INFO.cSMALLPAGE_NUM; 
		Mapping_Table[Q2_INX+tc_Extend_Count ] =QTable.WD;
		
	}	
	
}


/***************************************************************************
* NAME:      void RW_Stage_EtoS_Copyback(UWORD StartPage,UWORD EndPage,bit L_E)
* DESCRIPTION:
*     This subroutine is to do the Host Read Sector.
*     Input  Vars: L_E =1;Last to Swap
***************************************************************************/
void NAND_Laod_Data_To_Buf(UWORD tw_StartPage,U8 tc_Bufaddr){ 
	xdata	U8	tc_XferCnt;
	FDBP.cFDev =gc_HDev;
	Device_Read_SRAM_Index.BY[0] = 0x80+(tc_Bufaddr<<1);
	Device_Read_SRAM_Index.BY[1] = 0x00; 		
	FDBP.cFPlane = tw_StartPage%NAND_INFO.cPLANE_NUM;
	FDBP.wFBlock = APort_FBlock[0];// source block 
	FDBP.wFPage  = tw_StartPage/NAND_INFO.cPLANE_NUM;
	FDBP.cFInAddr= tc_Bufaddr;
	tc_XferCnt = (NAND_INFO.cINADDRMAX-tc_Bufaddr);
	Flash_Sequent_Read(tc_XferCnt);

}
/*********************************************************************************
* NAME:                  void Flash_Close_SmallBlock(void)
* DESCRIPTION:
*********************************************************************************/
void NAND_Close_Random_Block(void){
	data	U16 tw_Page;
	data	U8	tc_Offset;
	data	U8	tc_Inx;
	data	U8	tc_i;
	xdata	U8	tc_XferPageCnt;
	tw_Page =0;
	ZSpare[0] =0x55;
//dbprintf("X ");	
	for(tc_Inx=0; tc_Inx<MAX_RANDOM_HBLOCK;tc_Inx++){
		ZSpare[4+tc_Inx]= Wear_Table[RINX+tc_Inx];
	}
	APort_FBlock[0] = APort_RBlock[0];  // source block 
	APort_FBlock[1] = APort_SBlock[0];	// target block 
	for(tc_Inx=0; tc_Inx<MAX_RANDOM_HBLOCK;tc_Inx++){
		//ZSpare[2] =0xFF;
		if(Wear_Table[RINX+tc_Inx]!=0xFF){//表示有效的random block 
			tc_Offset = tc_Inx*MAX_RANDOM_HPAGE;
			ZSpare[2] = Wear_Table[RINX+tc_Inx]; 		
			for(tc_i=0; tc_i<MAX_RANDOM_HPAGE;tc_i++){
				QTable.WD = Mapping_Table[R_HPAGE+tc_Offset+tc_i];
				if(QTable.WD!=0xFFFF){ // random page是有效的
					ZSpare[1] = QTable.BY[0];
					ZSpare[3] = QTable.BY[1];
					gw_SourcePage = Mapping_Table[R_PAGE+tc_Offset+tc_i];
					gw_TargetPage = tw_Page;
					tc_XferPageCnt = RANDOM_SBLK_SIZE;				
					RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt);
					Mapping_Table[R_PAGE+tc_Offset+tc_i] =tw_Page;
					tw_Page++;
				}	
			}	
		}
	}
	// Updata Mapping Table //
	Mapping_Table[RBK]=APort_SBlock[0];
	Wear_Table[RCNT]= APort_SCNT[0];
	Mapping_Table[SBK] =APort_RBlock[0]; 			 	
	Wear_Table[SCNT]  =(APort_RCNT[0]+1);
	NAND_Erase_Block_LEDR(3); 	
	Mapping_Table[RPAGEINX]=tw_Page; // new RPAGEINX	
	APort_RBlock[0] = Mapping_Table[RBK]; 
	APort_SBlock[0] = Mapping_Table[SBK];
	APort_RCNT[0] = Wear_Table[RCNT];
	APort_SCNT[0] = Wear_Table[SCNT];
	
}
/*********************************************************************************
* NAME:      void NAND_Close_Dummy(U8 tc_Inx)
* DESCRIPTION:先判斷是否Ext &Dummy 是否已經超過一半了
*********************************************************************************/
void NAND_Close_Dummy_Block(U8 tc_Inx){
 	data	U8	tc_i;
	data	U8	tc_Offset;
 	data	U8	tc_XferPageCnt;
 	data	U8	tc_D_SBlock=0;
//dbprintf("Y ");	
 	tc_Offset= (tc_Inx* MAX_DUMMY_PAGE); 
	for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
		if(Mapping_Table[D_HPAGE+tc_Offset+tc_i]!=0xFFFF){
			tc_i++;
			break;
		}
	}
 	if(tc_i>(MAX_DUMMY_PAGE>>1)){//被佔了超過一半的位置直接收起來吧
 		tc_XferPageCnt=Mapping_Table[Q2_INX+tc_Inx]>>8;//借用變數而已
 		NAND_Combin_E_D_To_New_E(tc_Inx,tc_XferPageCnt);
 	}		
 	else{//沒有超過一半表示是一直寫同一個位置-->處理Dummy block 即可
 		APort_FBlock[0]= APort_DBlock[0];
		APort_FBlock[1]= APort_SBlock[0];				
 		for(tc_i=0; tc_i<MAX_DUMMY_PAGE;tc_i++){
			if(Mapping_Table[D_HPAGE+tc_Offset+tc_i]!=0xFFFF){
				QTable.WD= Mapping_Table[D_HPAGE+tc_Offset+tc_i];
				SQTable.WD = Mapping_Table[D_PAGE+tc_Offset+tc_i];
				ZSpare[0] = 0x5A; 							ZSpare[1] = QTable.BY[0];
				ZSpare[2] = Mapping_Table[EINX+tc_Inx];		ZSpare[3] = SQTable.BY[0];
				ZSpare[4] = 0x5A; 							ZSpare[5] = QTable.BY[0];
				ZSpare[6] = Mapping_Table[EINX+tc_Inx];		ZSpare[7] = SQTable.BY[0];			
				gw_SourcePage=((U16)QTable.BY[1]*H_INFO.cSMALLPAGE_NUM)+0;
    			gw_TargetPage=((U16)tc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+0;					
				tc_XferPageCnt = SQTable.BY[1]-SQTable.BY[0];
				RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );			
				QTable.BY[1] =tc_D_SBlock;//update 相對應的 D_SBlock 
				Mapping_Table[D_HPAGE+tc_Offset+tc_i]=QTable.WD;
				tc_D_SBlock++;
			}
		}
		// update Mapping_Table
		Mapping_Table[DBK+tc_Inx] =APort_SBlock[0];
		Wear_Table[DCNT+tc_Inx] =APort_SCNT[0];
		NAND_Erase_Block_LEDR(2);
		Mapping_Table[SBK]= APort_DBlock[0];
		Wear_Table[SCNT]= APort_DCNT[1]+1;
		QTable.WD =Mapping_Table[Q1_INX+tc_Inx];
		QTable.BY[1] =tc_D_SBlock;
		Mapping_Table[Q1_INX+tc_Inx] =QTable.WD;
		APort_DBlock[0]= Mapping_Table[DBK+tc_Inx];
		APort_DCNT[0] = Wear_Table[DCNT+tc_Inx];
		APort_SBlock[0]=Mapping_Table[SBK];
		APort_SCNT[0] = Wear_Table[SCNT];

	}	

}




/*********************************************************************************
* NAME:      void NAND_Close_E_SBlock(void)
* DESCRIPTION:
*********************************************************************************/
void NAND_Close_E_SBlock(U8 tc_Inx){
 	xdata	U8	tc_XferPageCnt;
//dbprintf("Z ");	
 	/*ZSpare[0] = 0xA5; 				ZSpare[1] = (U8)Mapping_Table[PWR_INX+tc_Inx];//set E Block power inx
	ZSpare[2] = Mapping_Table[EINX+tc_Inx];  		ZSpare[3] = APort_ECNT[0];	//0601 Jay fix	
	ZSpare[4] = 0xFF; 				ZSpare[5] = 0xFF;
	ZSpare[6] = 0xFF; 				ZSpare[7] = 0xFF;*/
	NAND_Set_Spare_Data(0,tc_Inx);//0601 Jay fix	
	APort_FBlock[0]= APort_LBlock[0];
	APort_FBlock[1]= APort_EBlock[0];				
	QTable.WD= Mapping_Table[Q2_INX+tc_Inx];
	gw_SourcePage=((U16)QTable.BY[0]*H_INFO.cSMALLPAGE_NUM)+QTable.BY[1];
    gw_TargetPage=((U16)QTable.BY[0]*H_INFO.cSMALLPAGE_NUM)+QTable.BY[1];					
	tc_XferPageCnt = H_INFO.cSMALLPAGE_NUM-QTable.BY[1];
	RW_Data_Stage(gw_SourcePage,gw_TargetPage,tc_XferPageCnt );
	QTable.BY[0]+=1;
	QTable.BY[1] =0x00;
	Mapping_Table[Q2_INX+tc_Inx]= QTable.WD;// update table
}
/***************************************************************************
* NAME:         void NAND_Set_Spare_Data(U8 tc_Type,U8 tc_Inx)
* DESCRIPTION:
*          
***************************************************************************/
void NAND_Set_Spare_Data(U8 tc_Type,U8 tc_Inx){
	xdata	U8 tc_i;
	switch(tc_Type){
		case 0://write External
			ZSpare[0] = 0xA5; 							ZSpare[1] = (U8)Mapping_Table[PWR_INX+tc_Inx];
			ZSpare[2] = Mapping_Table[EINX+tc_Inx]; 	ZSpare[3] = Wear_Table[ECNT+tc_Inx];	
			ZSpare[4] = 'Q'; 							ZSpare[5] = (Mapping_Table[Q1_INX+tc_Inx]>>8);//Q1
			ZSpare[6] = 'Q'; 							ZSpare[7] = (Mapping_Table[Q1_INX+tc_Inx]>>8);//Q1
			break;
		case 1:// write dummy 
			ZSpare[0] = 0x5A; 							ZSpare[1] = gc_SBlock;
			ZSpare[2] = Mapping_Table[EINX+tc_Inx];		ZSpare[3] = SQTable.BY[0];
			ZSpare[4] = 0x5A; 							ZSpare[5] = gc_SBlock;
			ZSpare[6] = Mapping_Table[EINX+tc_Inx];		ZSpare[7] = SQTable.BY[0];
			break;
		case 2:// write random 
			ZSpare[0] =0x55;   							ZSpare[1] = HPage.BY[0];
			ZSpare[2] = HBlock.WD;                      ZSpare[3] = HPage.BY[1];
		
			for(tc_i=0; tc_i<MAX_RANDOM_HBLOCK;tc_i++){
				ZSpare[4+tc_i]= Wear_Table[RINX+tc_i];
			}		
			break;		
		case 3:
			ZSpare[0] = 'C'; 							ZSpare[1] = 'O';
			ZSpare[2] = 'D';							ZSpare[3] = 'E';
			ZSpare[4] = 'C'; 							ZSpare[5] = 'O';
			ZSpare[6] = 'D';							ZSpare[7] = 'E';
			break;	
		case 4:
			ZSpare[0] = 'B'; 							ZSpare[1] = 'A';
			ZSpare[2] = 'N';							ZSpare[3] = 'K';
			ZSpare[4] = 'B'; 							ZSpare[5] = 'A';
			ZSpare[6] = 'N';							ZSpare[7] = 'K';
			break;	
	}	
}



