/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	dos.c
 *	\brief	
 *
 *  
 *	\author jay
 */
#include "SPDA2K.h"


bit	tgbt_Dos_Start_Stage;	
bit	tgbt_Dos_Continue_Stage;
bit tgbt_Dos_End_Stage;
unionWORD Source_Addr;

//! Read a sector at a given LBA
/*! \ 1-->ECC fail 0-->ECC_Pass
 *	\param	Read_LBA	Read sector logical block address
 *	\param	Buf_Index	Target Data Buf Address
 */
bit Dos_Read_LBA(LWORD Read_LBA ,UBYTE Buf_Index) {
	gc_ErrorCode=0x00;	
	LBA.LW = Read_LBA;	
	Source_Addr.BY[0] = Buf_Index;
	Source_Addr.BY[1] = 0x00;
	if(LBA.LW>= Capacity.LW){
//		dbprintf("\n Read out of range ");
		gc_ErrorCode = ID_NOT_FND;
	}	
	LBA2DBP();	
	SetHBlockToHDev();
	if (gc_ErrorCode) {
		return 1 ;
	}
	
	if(NAND_FlashReadSector_DOS()){
		return 1;
	}
	return 0;	
}

//! Write a sector at a given LBA
/*!
 *	\param	Write_LBA	Write sector logical block address
 *	\param	Buf_Index	ource Data Buf Address
 *	\param	Write_Stage	
 *						- 0	Dos_Start_Stage+ Dos_Continue_Stage 
 *	   					- 1	Only Dos_Continue_Stage 
 *		   				- 2	Dos_Continue_Stage + Dos_End _Stage	 
 *			   			- 3	Dos_Start_Stage + Dos_Continue_Stage + Dos_End _Stage
 *	   					- 4	Only Dos_End_Stage 
 */
void Dos_Write_LBA(LWORD Write_LBA,UBYTE Buf_Index,UBYTE Write_Stage) {
	data	U8 tc_Temp ;										// 081105 Jay add
	
	gc_ErrorCode=0x00;	
	LBA.LW =Write_LBA;
	Source_Addr.BY[0] = Buf_Index;
	Source_Addr.BY[1] = 0x00;
	tc_Temp= DMA_REG[0x90];
	DMA_REG[0x90] = 0x00;

	switch(Write_Stage){
		case 0:
			tgbt_Dos_Start_Stage 	= 1;
			tgbt_Dos_Continue_Stage	= 1;
			tgbt_Dos_End_Stage 		= 0;
			break;
		case 1:
			tgbt_Dos_Continue_Stage	= 1;
			tgbt_Dos_End_Stage		= 0;
			break;
		case 2:
			tgbt_Dos_Continue_Stage = 1;
			tgbt_Dos_End_Stage      = 1;
			break;
		case 3:
			tgbt_Dos_Start_Stage 	= 1;
			tgbt_Dos_Continue_Stage	= 1;
			tgbt_Dos_End_Stage 		= 1;
			break;
		default:
			if(tgbt_Dos_Start_Stage==1)
			{
				tgbt_Dos_Start_Stage 	= 0;
				tgbt_Dos_Continue_Stage	= 0;
				tgbt_Dos_End_Stage 		= 0;
				break;
			}
			tgbt_Dos_Continue_Stage = 0;
			if(!tgbt_Dos_End_Stage){						// 081117 Jay fix			
				tgbt_Dos_End_Stage      = 1;
			}
			else{
				tgbt_Dos_End_Stage   = 0;
			}
			break;
	}	
	if(LBA.LW>= Capacity.LW){
//		dbprintf("\n  Write out of range ");
		gc_ErrorCode=ID_NOT_FND;
	}	
	DMA_REG[0x90] =	tc_Temp;
}


/*********************************************************************************
* NAME:  bit NAND_FlashReadSector_DOS(void)
* DESCRIPTION:  1-->ECC_fail 0-->ECCP ASS , ¥ý¥á¨ì 0X9000
*********************************************************************************/
bit NAND_FlashReadSector_DOS(void) {
	data	bit	tbt_ECC_Fail=0;
	data	bit	tbt_Extend_Found,tbt_Random_Found,tbt_Find_D_SBlock;
	data	U8	tc_Extend_Count;
	data	U8	tc_Random_Count;
	xdata	U8	tc_Offset;
   	xdata	U8	tc_Buf_InAddr;
    xdata	U8	tc_i;
    xdata	U8	tc_RealInAddr;     
    xdata	U16	tw_TotalESPage;

    DEVICE_REG[0x00]|=0x10;  //for interface switch									// ±NFlash module reset
	DEVICE_REG[0x00]&=0xEF;
	DEVICE_REG[0x00]= 0x01;
	Flash_State_Initial();
	Check_LoadMap();	
	NAND_HPage_To_SBlock_SPage();
	APort_FBlock[0] = Mapping_Table[HBlock.WD];
	tbt_Extend_Found = 0;   
	tbt_Random_Found = 0;
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
						if(gc_SBlock<gc_Q1_SBlock){	//0524 Jay add		
							APort_FBlock[0] =APort_LBlock[0];
						}
						else{			
							APort_FBlock[0] =APort_EBlock[0];				
						}						
						gw_TargetPage = ((U16)gc_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
					}
				}				
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
	}
	else if (tbt_Random_Found){		
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
		APort_FBlock[0] =APort_LBlock[0];
		gw_TargetPage = HPage.WD;
		
	}
	tc_Buf_InAddr =gc_HInAddr&0x01;
	tc_RealInAddr = gc_HInAddr&0xFE;
	NAND_SBlock_SPage_To_FDBP();	
	FDBP.cFInAddr =tc_RealInAddr;
	Device_Read_SRAM_Index.BY[0] = 0x90;	
	Device_Read_SRAM_Index.BY[1] = 0x00;
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
	FLASH_REG[0xA1] = 0x01;
	FLASH_REG[0xA2] &= 0xF3;
	FLASH_REG[0x1A] = 0x11;										// last flag = 1
	Trigger_Data_Transfer_Done();    
	if(CHK_ECC_Done()){
		tbt_ECC_Fail=1;
	}
	if(tc_Buf_InAddr){// 
		Device_Read_SRAM_Index.WD= 0x9200;		
	}
	else{
		Device_Read_SRAM_Index.WD= 0x9000;
	}		
	Move_RAM(Device_Read_SRAM_Index.WD,Source_Addr.WD,511);
	return tbt_ECC_Fail;
}

/*****************************************************************************
* NAME:         void Move_RAM(void)
* DESCRIPTION:  using DMA2  module to move ram 
*   
*****************************************************************************/
void Move_RAM(WORD RAM_Source,WORD RAM_Destination,WORD RAM_Size){
	DMA_REG[0xC0]=0x00;											// Clean 0xB3C0
	DMA_REG[0x01]=SRAM2SRAM;									// DMA from SRAM to SRAM
	DMA_REG[0x02]=(RAM_Size&0xFF);								// Define data size in DMA transfer =512 byte (0xB302, 0xB303)
	DMA_REG[0x03]=(RAM_Size>>8);								//
	CPU_REG[0x12]=(RAM_Source&0xFF);							// DMA Source Addr[7:0]
	CPU_REG[0x13]=(RAM_Source>>8);								//                [15:8]
	CPU_REG[0x14]=(RAM_Destination&0xFF);						// DMA Destination Addr[7:0]
	CPU_REG[0x15]=(RAM_Destination>>8);							//                     [15:8]
	DMA_REG[0xB0]=0x01;											// Tigger DMA transfer
	while(!(DMA_REG[0xC0]&0x01));								// wait until DMA cycle is completed (0xB3C0)
	DMA_REG[0xC0]=0x00;											// Clean 0xB3C0
}
