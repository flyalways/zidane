/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	fepr.c
 *	\brief	
 *
 *	\author	jay
 */
#include "SPDA2K.h"

static  void FLASH_Recover_Blcok(UWORD StartPage , UWORD EndPage , U8 TargetDev);
/*********************************************************************************
* NAME:               U16 Flash_Search_ReplaceBlock (U8 cDev)
* DESCRIPTION:
*********************************************************************************/
U16 Flash_Search_ReplaceBlock_Index (void ){
	U16 tw_Replace;
	U8  tc_Find_Replace =0 ;
	U8  tc_i;
	if((Wear_Table[WINX]>=SYS_ZONE.wZONE_SIZE)||(Wear_Table[WINX]==0)){
		Wear_Table[WINX] = (SYS_ZONE.wZONE_SIZE-1);
	}
	//tw_Replace = Wear_Table[WINX];
	tw_Replace = (SYS_ZONE.wZONE_SIZE-1);
	do{
		if((Wear_Table[tw_Replace]>0x70)&&(Wear_Table[tw_Replace]<=0x7F)){
			tc_Find_Replace =1; 
			for(tc_i=0;tc_i<Extend_NUM;tc_i++){
				if(Mapping_Table[EINX+tc_i]==tw_Replace){
					tc_Find_Replace =0;
					break;
				}
			}			
			for(tc_i=0;tc_i<MAX_RANDOM_HBLOCK;tc_i++){
				if(Wear_Table[RINX+tc_i]==tw_Replace){
					tc_Find_Replace =0;						
					break;
				}
			}					
		}				
		if(tc_Find_Replace){			
			break;
		}	
		tw_Replace--;
	}while(tw_Replace);
	if(tc_Find_Replace){
		
		return tw_Replace;
	}
	else{
		tw_Replace = Wear_Table[WINX];
		tw_Replace= tw_Replace%SYS_ZONE.wZONE_SIZE;
		do{			
			tc_Find_Replace =1; 
			for(tc_i=0;tc_i<Extend_NUM;tc_i++){
				if(Mapping_Table[EINX+tc_i]==tw_Replace){
					tc_Find_Replace =0;
					break;
				}
			}			
			for(tc_i=0;tc_i<MAX_RANDOM_HBLOCK;tc_i++){
				if(Wear_Table[RINX+tc_i]==tw_Replace){
					tc_Find_Replace =0;						
					break;
				}
			}					
							
			if(tc_Find_Replace){				
				break;
			}
				
			else{
				 tw_Replace--;
				 if(tw_Replace==0){
				 	tw_Replace =(SYS_ZONE.wZONE_SIZE-1);
				}
			}
		}while(tw_Replace);	
		Wear_Table[WINX] = tw_Replace-1;
		return tw_Replace;
	}
}
/*********************************************************************************
* NAME:               void  Flash_Update_WearBlock (void)
* DESCRIPTION:
*********************************************************************************/
void  NAND_Update_WearBlock (U8 tc_Dev){
  	U16 tw_HBlock;
  	tw_HBlock = Flash_Search_ReplaceBlock_Index();
//	dbprintf("\n Wear_Index:%X",tw_HBlock);
	
	FDBP.cFDev= tc_Dev;
	FDBP.cFPlane = 0;
	FDBP.wFBlock = Mapping_Table[tw_HBlock];
	FDBP.wFPage  = 0;
	FDBP.cFInAddr =0;  
	Device_Read_SRAM_Index.WD=0x8000;
	Flash_ReadSpare(8);   
	ZSpare[3] =0;  
	gw_SourceBlock = Mapping_Table[tw_HBlock];
	gw_TargetBlock = Mapping_Table[SBK];
	FLASH_Recover_Blcok(0 , NAND_INFO.wPAGE_NUM ,tc_Dev);
	Mapping_Table[tw_HBlock] =gw_TargetBlock;
	Mapping_Table[SBK] =gw_SourceBlock;
	Wear_Table[SCNT]= Wear_Table[tw_HBlock]+1;	
	Wear_Table[tw_HBlock] =0;
	FDBP.cFDev = tc_Dev;
	FDBP.cFPlane = 0;
	FDBP.wFBlock = gw_SourceBlock;
	FDBP.wFPage  = 0;
	FDBP.cFInAddr = 0;
	Flash_EraseOneBlock_NO_ChkRdy();
} 
/*********************************************************************************
* NAME:                   void NAND_Recover_Block(U16 tw_Inx, U8 tc_Recover_Type );
* DESCRIPTION:
*********************************************************************************/
void NAND_Recover_Block(U16 tw_Inx, U8 tc_Recover_Type ){
	U8  tc_Replace_Mode =0;
	U16 tw_Cnt,tw_WhichGroup;
	gw_SourceBlock = Mapping_Table[tw_Inx];
	
	if((tc_Recover_Type&0x03)==0x02){// replace block 
		tc_Replace_Mode=1;
		FDBP.cFDev = gc_HDev;
		FDBP.cFPlane = 0;
		FDBP.wFBlock = ReplacementBlock_Table[gc_HDev];
		FDBP.wFPage  = ReplacementBlock_Page[gc_HDev];
		FDBP.cFInAddr = 0;
		Device_Read_SRAM_Index.BY[0] = 0x80;	
		Device_Read_SRAM_Index.BY[1] = 0x00;
		Flash_Sequent_Read(4);
		if((FLASH_REG[0x60]==0x3E)&&(FLASH_REG[0x61] =='R')){	
			tw_Cnt = XWORD_8000[511];
			if(tw_Cnt ==0){//表示已經將所有 replace block 用完
				tc_Replace_Mode =0;
//				dbprintf("\n Recover Over");
				return;
			}
			else{
				tw_Cnt--;
				gw_TargetBlock = XWORD_8000[tw_Cnt];
				XWORD_8000[tw_Cnt] =0xFFFF;
				XWORD_8000[511 ] =tw_Cnt;
				tw_Cnt=XWORD_8400[511];
				XWORD_8400[tw_Cnt]= Mapping_Table[tw_Inx];
				XWORD_8400[511]= tw_Cnt+1;
				FDBP.cFDev = gc_HDev;
				FDBP.cFPlane = 0;
				FDBP.wFBlock = ReplacementBlock_Table[gc_HDev];
				FDBP.wFPage  = ReplacementBlock_Page[gc_HDev]+1;
				FDBP.cFInAddr = 0;
				if(FDBP.wFPage== NAND_INFO.wPAGE_NUM){
					Flash_EraseOneBlock_NO_ChkRdy();
					ReplacementBlock_Page[gc_HDev] =0;
					FDBP.wFPage =ReplacementBlock_Page[gc_HDev];
					FDBP.cFInAddr = 0;
				}
				ZSpare[0] = 0x3E; 							ZSpare[1] = 'R';
				ZSpare[2] = 'M'; 							ZSpare[3] = 'T';	
				ZSpare[4] =  0x3E; 							ZSpare[5] = 'R';
				ZSpare[6] = 'M'; 							ZSpare[7] = 'T';
				Device_Write_SRAM_Index.BY[0] = 0x80;	
				Device_Write_SRAM_Index.BY[1] = 0x00;
				Flash_Sequent_Write(4,1); 
			}
		}
		else{
			tc_Replace_Mode =0;
//			dbprintf("\n No Replacement block");
			return;
		}	
	}
	if(!tc_Replace_Mode){
		gw_TargetBlock = Mapping_Table[SBK];
	}	
	//------ Read Source Block Mark ----------//
	FDBP.cFDev=gc_HDev;
	FDBP.cFPlane = 0;
	FDBP.wFBlock = Mapping_Table[tw_Inx];
	FDBP.wFPage  = 0;
	FDBP.cFInAddr =0;  
	Device_Read_SRAM_Index.WD=0x8000;
	Flash_ReadSpare(8);  // get Hblock spare data 
	ZSpare[3] =1; // update erase cnt 
	
	FLASH_Recover_Blcok(0 , NAND_INFO.wPAGE_NUM , gc_HDev);
	//----------------------------------//
	if(tc_Replace_Mode){
		Mapping_Table[tw_Inx]=gw_TargetBlock;
		Wear_Table[tw_Inx] =0x01;
		//--------- erase Temp Block ---//
		FDBP.cFDev= gc_HDev;
		FDBP.cFPlane = 0;
		FDBP.wFBlock = TempBlock_Table[FDBP.cFDev];
		FDBP.wFPage  = 0;
		FDBP.cFInAddr =0;  
		Flash_EraseOneBlock_NO_ChkRdy();
		//--------- Copy BMT to  Temp --//
		tw_WhichGroup =MAP_Table[gw_NowBMTPoint];
		NAND_ZONE_To_BMT_DBP(tw_WhichGroup);
		FDBP.cFDev= BMT.cFDev;
		FDBP.cFPlane = 0;		
		for(FDBP.wFPage=0; FDBP.wFPage< SYS_ZONE.cDEV_ZONE_NUM; FDBP.wFPage++){
			FDBP.wFBlock  = BMT.wFBlock;
			FDBP.cFInAddr =0;
			Device_Read_SRAM_Index.WD= 0x8000;
			Flash_Sequent_Read(4);
			if(FDBP.wFPage==BMT.wFPage){
				for(tw_Cnt=0;tw_Cnt<( SYS_ZONE.wZONE_SIZE+ZONE_TOTAL_OTHER);tw_Cnt++){
					if(XWORD_8000[tw_Cnt]==gw_SourceBlock){
						XWORD_8000[tw_Cnt] =gw_TargetBlock;
					}
				}
			}						
			Device_Write_SRAM_Index.WD =0x8000;
			FDBP.wFBlock = TempBlock_Table[FDBP.cFDev];
			FDBP.cFInAddr =0;
			Flash_Sequent_Write(4,0);	
		}
		gw_TargetBlock = BMTTable_Backup[BMT.cFDev];	
		gw_SourceBlock = TempBlock_Table[BMT.cFDev];
		FDBP.wFBlock = gw_TargetBlock;
		FDBP.wFPage  = 0;
		FDBP.cFInAddr =0;  
		Flash_EraseOneBlock_NO_ChkRdy();
		ZSpare[0] = 0x3E; 							ZSpare[1] = 'B';
		ZSpare[2] = 'M'; 							ZSpare[3] = 'T';	
		ZSpare[4] =  0x3E; 							ZSpare[5] = 'B';
		ZSpare[6] = 'M'; 							ZSpare[7] = 'T';
		FLASH_Recover_Blcok(0 , SYS_ZONE.cDEV_ZONE_NUM ,  BMT.cFDev);
		gw_TargetBlock = BMTTable[BMT.cFDev];	
		gw_SourceBlock = TempBlock_Table[BMT.cFDev];
		FDBP.wFBlock = gw_TargetBlock;
		FDBP.wFPage  = 0;
		FDBP.cFInAddr =0;  
		Flash_EraseOneBlock_NO_ChkRdy();
		ZSpare[0] = 0x3E; 							ZSpare[1] = 'B';
		ZSpare[2] = 'M'; 							ZSpare[3] = 'T';	
		ZSpare[4] =  0x3E; 							ZSpare[5] = 'B';
		ZSpare[6] = 'M'; 							ZSpare[7] = 'T';		
		FLASH_Recover_Blcok(0 , SYS_ZONE.cDEV_ZONE_NUM , BMT.cFDev);
		
	}
	else{
		Mapping_Table[tw_Inx]=gw_TargetBlock;
		tw_Cnt =Wear_Table[tw_Inx];	
		Wear_Table[tw_Inx]=Wear_Table[SCNT];
		FDBP.cFDev=gc_HDev;
		FDBP.cFPlane = 0;
		FDBP.wFBlock = gw_SourceBlock;
		FDBP.wFPage  = 0;
		FDBP.cFInAddr =0;
		Flash_EraseOneBlock_NO_ChkRdy();
		Mapping_Table [SBK] = gw_SourceBlock;
		Wear_Table[SCNT] =tw_Cnt+1;	
	}


}
/*********************************************************************************
* NAME:  void FLASH_Recover_Blcok(UWORD StartPage , UWORD EndPage , U8 TargetDev)
* DESCRIPTION:
*********************************************************************************/
static  void FLASH_Recover_Blcok(UWORD StartPage , UWORD EndPage , U8 TargetDev){	
	U16 tw_Page; 	
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
                                                                                    