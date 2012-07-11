/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	flshscan.c
 *	\brief	
 *
 *	\author	jay
 */

#include "SPDA2K.h"

/*********************************************************************************
* NAME:              void Create_Zone_Lookup_Table(void)
* DESCRIPTION:   // SPDA26xx flash  plan
*********************************************************************************/

void Create_Zone_Lookup_Table(void) {	
	U16		tw_Zone_index,tw_Index_Temp, tw_HBlockCnt,tw_SwapCnt;  
	U8 		tc_ExtCnt,tc_DmyCnt,tc_i,tc_j;
	bit     tbt_HBlock_Exist,tbt_DmyErr,tbt_RanErr,tbt_FastErr,tbt_ExtErr; 
	bit     tbt_Tempflag;
	U8      tc_PwrInx,tc_SwapInx;
	tw_HBlockCnt =0;
	//---------- initial variable value---/
	tc_ExtCnt  =0;
	tc_DmyCnt  =0;
	tw_SwapCnt =0;
	tbt_DmyErr=0;
	tbt_RanErr=0;
	tbt_FastErr=0;
	tbt_ExtErr=0; 
	tc_SwapInx =0;
//	dbprintf("\n Create_Zone_Lookup_Table:%x",gw_WhichGroup);
	//------- clear table---------//  
	memset(Mapping_Table,0xFF,1024);
	ExtErrBlock[15] =0;   			   
    DmyErrBlock[15] =0;                
    RanErrBlock[15] =0;  			   
    FastErrBlock[15] =0;      			
	//------ load bmt table -----//	
	FDBP.cFDev =BMT.cFDev;
	FDBP.cFPlane = BMT.cFPlane;
	FDBP.wFBlock = BMT.wFBlock;
	FDBP.wFPage  = BMT.wFPage;
	FDBP.cFInAddr = BMT.cFInAddr;
	Device_Read_SRAM_Index.BY[0] = 0x90;	
	Device_Read_SRAM_Index.BY[1] = 0x00;

	if(Flash_Sequent_Read(2)){// bmt table fail -->read backup bmt block 
		BMT.wFBlock = BMTTable_Backup[BMT.cFDev];		
		FDBP.cFDev =BMT.cFDev;
		FDBP.cFPlane = BMT.cFPlane;
		FDBP.wFBlock = BMT.wFBlock;
		FDBP.wFPage  = BMT.wFPage;
		FDBP.cFInAddr = BMT.cFInAddr;
		Device_Read_SRAM_Index.BY[0] = 0x90;	
		Device_Read_SRAM_Index.BY[1] = 0x00;
		Flash_Sequent_Read(2); 
	}	
	for(tw_Index_Temp = 0; tw_Index_Temp < ( SYS_ZONE.wZONE_SIZE+ZONE_TOTAL_OTHER); tw_Index_Temp ++){ 
		tbt_HBlock_Exist=0;
		FDBP.wFBlock = Source_Table[tw_Index_Temp];
		FDBP.cFPlane = 0;
		FDBP.wFPage  = 0;
		FDBP.cFInAddr=0;
		Device_Read_SRAM_Index.BY[0] = 0x98;//0601 Jay change 0x80-->0x98
		Device_Read_SRAM_Index.BY[1] = 0x00;
		Flash_ReadSpare(4);			
		switch (ZSpare[0]){
			case 0xA5:  // HBlock & External Block-->0xA5 |PW_Index|HBlock|Erase_Cnt|
				tw_Zone_index =ZSpare[2];
				if(Mapping_Table[tw_Zone_index]!=0xFFFF){
					APort_FBlock[1] = Source_Table[tw_Index_Temp];
					APort_FCNT  [1] = ZSpare[3];
					FDBP.wFBlock = Source_Table[tw_Index_Temp];
					FDBP.cFPlane=0;
					FDBP.wFPage  = NAND_INFO.wPAGE_NUM-1;
					FDBP.cFInAddr=0;
					Device_Read_SRAM_Index.BY[0] = 0x98;//0601 Jay change 0x80-->0x98
					Device_Read_SRAM_Index.BY[1] = 0x00;
					Flash_ReadSpare(4);
					//if((ZSpare[0]==0xA5)&& (ZSpare[2]==tw_Zone_index)){//最後一頁也有資料,需跟另一個block 比對
					if((ZSpare[0]==0xA5)){//0601 Jay fix 
						tc_PwrInx = ZSpare[1];
						FDBP.wFBlock = Mapping_Table[tw_Zone_index];
						FDBP.cFPlane = 0;
						FDBP.wFPage  = NAND_INFO.wPAGE_NUM-1;
						FDBP.cFInAddr= 0;
						Device_Read_SRAM_Index.BY[0] = 0x98;//0601 Jay change 0x80-->0x98
						Device_Read_SRAM_Index.BY[1] = 0x00;
						Flash_ReadSpare(4);	
						//if((ZSpare[0]==0xA5)&& (ZSpare[2]==tw_Zone_index)){//糟糕兩個都是到最後一頁-->不正常斷電比對pwr index或是有Dummy block 
						if((ZSpare[0]==0xA5)){//0601 Jay fix  
							if(tc_PwrInx!=(ZSpare[1]+1)){//source table的是Hblock 
								APort_FBlock[1] = Mapping_Table[tw_Zone_index];
								Mapping_Table[tw_Zone_index]= Source_Table[tw_Index_Temp];
								Wear_Table[tw_Zone_index] = APort_FCNT[1];
								APort_FCNT[1] = ZSpare[3];							
							}
							else{//不用處理，原本就填完了
								tc_PwrInx = ZSpare[1];
							}
						}
						else{//最後一頁沒資料所以原本HBlock 是EXT Block ,source table的是Hblock 
							APort_FBlock[1] = Mapping_Table[tw_Zone_index];
							Mapping_Table[tw_Zone_index]= Source_Table[tw_Index_Temp];
							Wear_Table[tw_Zone_index] = APort_FCNT[1];
							APort_FCNT[1] = ZSpare[3];		
						}						
					}
					//-------- 處理EXT 排列問題-----------------//
					tbt_Tempflag =0;
					for(tc_i=0;tc_i<Extend_NUM;tc_i++){  // 先尋找是否有相同EXT
						if(tw_Zone_index==Mapping_Table[EINX+tc_i]){
							if(Mapping_Table[EBK+tc_i]==0xFFFF){//先檢查是否已經有Blokck 佔住
								tbt_Tempflag=1;
								Mapping_Table[EBK+tc_i]=APort_FBlock[1];
								Wear_Table[ECNT+tc_i] =APort_FCNT[1];
								QTable.BY[0] = tc_PwrInx;
								QTable.BY[1] = tc_PwrInx+1;	
								Mapping_Table[PWR_INX+tc_i]= QTable.WD;
								tc_ExtCnt++;
								break;
							}
						}
					}
					//都沒有找到相同的Index-->a:  ALL oxFFFF, OR  B. 非oxFFFF,但是沒有符合的
					if(!tbt_Tempflag){// a :all 0xFFFF
						for(tc_i=0;tc_i<Extend_NUM;tc_i++){
							if(Mapping_Table[EINX+tc_i]==0xFFFF){
								Mapping_Table[EINX+tc_i]=tw_Zone_index; 
								Mapping_Table[EBK+tc_i]=APort_FBlock[1];
								Wear_Table[DCNT+tc_i] =APort_FCNT[1];
								QTable.BY[0] = tc_PwrInx;
								QTable.BY[1] = tc_PwrInx+1;	
								Mapping_Table[PWR_INX+tc_i]= QTable.WD;								
								tc_ExtCnt++;
								tbt_Tempflag =1;
								break;
							}
						}			
					}
					if(!tbt_Tempflag){//先記錄下來
						tc_i=ExtErrBlock[15];
						ExtErrBlock[tc_i] =APort_FBlock[1];
						tc_i++;
						ExtErrBlock[15]= tc_i;
						tbt_ExtErr =1;
					}
				}	
				else{		 
					Mapping_Table[tw_Zone_index] =	Source_Table[tw_Index_Temp];
					Wear_Table[tw_Zone_index] = ZSpare[3];
					tw_HBlockCnt++;
				}
				break;
			case 0x5A: // Dummy Block  --> 0x5A | Zone |Erase_Cnt | E_page
				tw_Zone_index =ZSpare[2];
				tbt_Tempflag =0;
				for(tc_i=0;tc_i<Extend_NUM;tc_i++){  // 先尋找是否有相同EXT
					if(tw_Zone_index==Mapping_Table[EINX+tc_i]){
						if(Mapping_Table[DBK+tc_i]==0xFFFF){// 先檢查是否是空的～～
							tbt_Tempflag=1;
							Mapping_Table[DBK+tc_i]=Source_Table[tw_Index_Temp];
							Wear_Table[DCNT+tc_i] =ZSpare[3];
							tc_DmyCnt++;
							break;
						}
					}
				}
				//都沒有找到相同的Index-->a:  ALL oxFFFF, OR  B. 非oxFFFF,但是沒有符合的
				if(!tbt_Tempflag){
					for(tc_i=0;tc_i<Extend_NUM;tc_i++){
						if(Mapping_Table[EINX+tc_i]==0xFFFF){
							Mapping_Table[EINX+tc_i]=tw_Zone_index; 
							Mapping_Table[DBK+tc_i]=Source_Table[tw_Index_Temp];
							Wear_Table[DCNT+tc_i] =ZSpare[3];
							tc_DmyCnt++;
							tbt_Tempflag =1;
							break;
						}
					}			
				}
				if(!tbt_Tempflag){//先記錄下來
					tc_i=DmyErrBlock[15];
					DmyErrBlock[tc_i] =Source_Table[tw_Index_Temp];
					tc_i++;
					DmyErrBlock[15]= tc_i;				
					tbt_DmyErr =1;
				}
				break;
			case 0x55: // Random Block --> 0x55 | Zone |HBlock | Erase_Cnt  
				if(Mapping_Table[RBK]==0xFFFF){
					Mapping_Table[RBK]=Source_Table[tw_Index_Temp];
					Wear_Table[RCNT] = ZSpare[3];
				}
				else{			
					tc_i=RanErrBlock[15];
					RanErrBlock[tc_i] =Source_Table[tw_Index_Temp];
					tc_i++;
					RanErrBlock[15]= tc_i;			
					tbt_RanErr =1;
				}				
				break;
			case 0xAA: // FastBMT Block --> 0xAA | Zone |XXXX |Erase_Cnt 
				if(FAST_BMTTable[gw_WhichGroup]==0xFFFF){			
					FAST_BMTTable[gw_WhichGroup]=Source_Table[tw_Index_Temp];
					FAST_BMTPage_Table[gw_WhichGroup] =0x00;
					Wear_Table[FCNT] = (ZSpare[3]+1);
					FDBP.cFDev =BMT.cFDev;
					FDBP.wFBlock = 	FAST_BMTTable[gw_WhichGroup];
					FDBP.cFPlane =0;
					Flash_EraseOneBlock_NO_ChkRdy();
									
				}
				else{			
					tc_i=FastErrBlock[15];
					FastErrBlock[tc_i] =Source_Table[tw_Index_Temp];
					tc_i++;
					FastErrBlock[15]= tc_i;
					tbt_FastErr =1;
				}				
				break;	
				
			default: //  0xFF : SWAP Block  or other  status  
				SwapTemp_Table[tw_SwapCnt] = Source_Table[tw_Index_Temp];
				tw_SwapCnt++;				
				break;
		
		}
	}
	//dbprintf("\n tw_Index_Temp:%x",tw_Index_Temp);
	//-------- Error Handle---------------------------//
	if(tbt_ExtErr){
	// 出現兩個相同的EX 可能是D＆E合併後，來不及Erase，
	// 任何一個Ext 都可以，只要在跟Dummy再一次	 
//		dbprintf("\n External error");
		for(tc_j=0; tc_j<ExtErrBlock[15];tc_j++){
			FDBP.cFDev=BMT.cFDev;
			FDBP.cFPlane =0;
			FDBP.wFBlock = ExtErrBlock[tc_j]; 
			FDBP.wFPage  = 0;
			FDBP.cFInAddr =0;
			Device_Read_SRAM_Index.WD =0x9800;//0601 Jay change 0x8000-->0x9800
			Flash_ReadSpare	(4);		
			gw_TargetBlock = ExtErrBlock[tc_j];
			QTable.WD = Search_Last_Page(gw_TargetBlock);		
			tbt_Tempflag =0;
			for(tc_i=0;tc_i<Extend_NUM;tc_i++){  
				if(ZSpare[2]==Mapping_Table[EINX+tc_i]){
					FDBP.cFDev=BMT.cFDev;
					SQTable.WD =Search_Last_Page(Mapping_Table[EBK+tc_i]);
					tbt_Tempflag=1;
					break;
				}
			}
			if(tbt_Tempflag){
				if(SQTable.WD<QTable.WD){// 原本的Ex 比較小需交換
					ExtErrBlock[tc_j]= Mapping_Table[EBK+tc_i];
					Mapping_Table[EBK+tc_i] = gw_TargetBlock;
					Wear_Table[ECNT+tc_i ] =ZSpare[3];					
				}				
			}
			SwapTemp_Table[tw_SwapCnt] =ExtErrBlock[tc_j] ;
			tw_SwapCnt++;			
		}
					
	
	}
	else if(tbt_DmyErr){
	// 出現兩個Dummy 可能是Dummy 已經滿了要搬移或是來不及Erase ,
	// 保留多的哪一個再來重新處理即可		
//		dbprintf("\n dummy error");
		for(tc_j=0; tc_j<DmyErrBlock[15];tc_j++){
			FDBP.cFDev=BMT.cFDev;
			FDBP.cFPlane =0;
			FDBP.wFBlock = DmyErrBlock[tc_j]; 
			FDBP.wFPage  = 0;
			FDBP.cFInAddr =0;
			Device_Read_SRAM_Index.WD =0x9800;//0601 Jay fix 0x80-->0x98
			Flash_ReadSpare	(4);		
			gw_TargetBlock = DmyErrBlock[tc_j]; 
			QTable.WD = Search_Last_Page(gw_TargetBlock);		
			tbt_Tempflag =0;
			for(tc_i=0;tc_i<Extend_NUM;tc_i++){  
				if(ZSpare[2]==Mapping_Table[EINX+tc_i]){
					FDBP.cFDev=BMT.cFDev;
					SQTable.WD =Search_Last_Page(Mapping_Table[DBK+tc_i]);
					tbt_Tempflag=1;
					break;
				}
			}
			if(tbt_Tempflag){
				if(SQTable.WD<QTable.WD){// 原本的Ex 比較小需交換
					DmyErrBlock[tc_j]= Mapping_Table[DBK+tc_i];
					Mapping_Table[DBK+tc_i] = gw_TargetBlock;;
					Wear_Table[DCNT+tc_i ] =ZSpare[3];					
				}				
			}
			SwapTemp_Table[tw_SwapCnt] =DmyErrBlock[tc_j] ;
			tw_SwapCnt++;			
		}
	}		
	else if(tbt_RanErr){
	// 出現兩個Dummy 可能是Dummy 已經滿了要搬移或是來不及Erase ,
	// 保留多的哪一個再來重新處理即可
//		dbprintf("\n random error");
		for(tc_j=0; tc_j<RanErrBlock[15];tc_j++){
			FDBP.cFDev=BMT.cFDev;
			FDBP.cFPlane =0;
			FDBP.wFBlock =RanErrBlock[tc_j]; 
			FDBP.wFPage  = 0;
			FDBP.cFInAddr =0;
			Device_Read_SRAM_Index.WD =0x9800;//0601 Jay fix 0x80-->0x98
			Flash_ReadSpare	(4);		
			gw_TargetBlock = RanErrBlock[tc_j]; 
			QTable.WD = Search_Last_Page(gw_TargetBlock);		
			tbt_Tempflag =0;
			FDBP.cFDev=BMT.cFDev;
			SQTable.WD =Search_Last_Page(Mapping_Table[RBK]);
			if(SQTable.WD<QTable.WD){// 原本的Ex 比較小需交換
				RanErrBlock[tc_j]= Mapping_Table[RBK];
				Mapping_Table[RBK] = gw_TargetBlock;;
				Wear_Table[RCNT] =ZSpare[3];					
								
			}
			SwapTemp_Table[tw_SwapCnt] =RanErrBlock[tc_j];
			tw_SwapCnt++;			
		}						
	}
	else if(tbt_FastErr){
//		dbprintf("\n Fast error");			
	//管他的反正都要重建了管那麼多幹嘛真接Erase當Swap 
		for(tc_j=0; tc_j<FastErrBlock[15];tc_j++){
			SwapTemp_Table[tw_SwapCnt] =FastErrBlock[tc_j];
			tw_SwapCnt++;
		}	
	}	
	//---------- build External& Dummy block page table----//
	for(tc_i=0;tc_i<Extend_NUM;tc_i++){ //Search external Q1 && Q2 
		if(Mapping_Table[EBK+tc_i]!=0xFFFF){
			 Search_Q1Q2_Table(tc_i);				
		}	
	}	
	for(tc_i=0;tc_i<Dummy_NUM;tc_i++){ //Search external Q1 && Q2 
		if(Mapping_Table[DBK+tc_i]!=0xFFFF){
			 Search_Dummy_Page(tc_i);				
		}	
	}
	//---------- build Random page table -----------------//
	if(Mapping_Table[RBK]!=0xFFFF){
		Search_Random_Page();
	}	
	//--------- 整理 swap block 與補滿 所有空的位置--------//
	if(tw_HBlockCnt<SYS_ZONE.wZONE_SIZE){
		memset(XBYTE_9800,0xFF,1024);//0601 Jay fix 0x80-->0x98
		memset(XBYTE_9C00,0xFF,1024);
		for(tw_Index_Temp = 0; tw_Index_Temp < SYS_ZONE.wZONE_SIZE; tw_Index_Temp ++){
			if(Mapping_Table[tw_Index_Temp]==0xFFFF){
				FDBP.cFDev =BMT.cFDev;
				FDBP.cFPlane =0;
				FDBP.wFBlock = SwapTemp_Table[tc_SwapInx]; 
				Flash_EraseOneBlock_NO_ChkRdy();		
				Mapping_Table[tw_Index_Temp]=SwapTemp_Table[tc_SwapInx];
				Wear_Table[tw_Index_Temp] = 0x7F;// 給一個中間值吧					
				tc_SwapInx++;
				Device_Write_SRAM_Index.BY[0] = 0x98;//0601 Jay fix 0x80-->0x98 
				Device_Write_SRAM_Index.BY[1] = 0x00;
				ZSpare[0] = 0xA5;// Hblock mark
				ZSpare[1] = 0x00; //default HBlock power inx
				ZSpare[2] = tw_Index_Temp; //Hblock num 
				ZSpare[3] = 0x7F; //erase times	
				ZSpare[4] = 'Q';
				ZSpare[5] = 0x00; 
				ZSpare[6] = 'Q'; 
				ZSpare[7] = 0x00; 					
				FDBP.cFPlane = 0;
				FDBP.wFBlock = Mapping_Table[tw_Index_Temp]; 
				FDBP.wFPage  = 0;
				FDBP.cFInAddr = 0;
				Flash_Sequent_Write(4,1);//寫兩個mark 進去 
				Device_Write_SRAM_Index.BY[0] = 0x98;//0601 Jay fix 0x80-->0x98 
				Device_Write_SRAM_Index.BY[1] = 0x00;				
				FDBP.cFPlane = 0;
				FDBP.wFBlock = Mapping_Table[tw_Index_Temp] ;
				FDBP.wFPage  = (NAND_INFO.wPAGE_NUM-1);
				FDBP.cFInAddr = 0;
				Flash_Sequent_Write(4,1);//寫兩個mark 進去 	
			}
		}
	}	
	if(FAST_BMTTable[gw_WhichGroup]==0xFFFF){	
		FDBP.cFDev =BMT.cFDev;
		FDBP.cFPlane =0;
		FDBP.wFBlock = SwapTemp_Table[tc_SwapInx]; 
		Flash_EraseOneBlock_NO_ChkRdy();	
		FAST_BMTTable[gw_WhichGroup]=SwapTemp_Table[tc_SwapInx];
		FAST_BMTPage_Table[gw_WhichGroup] =0x00;		
		Wear_Table[FCNT] = 0x7F;// 給一個中間值吧	
		tc_SwapInx++;
	}
	if(tc_SwapInx>tw_SwapCnt){
//		dbprintf("\n 1.lost block:%x,%x",tc_SwapInx,tw_SwapCnt);
	}		
	
	
	for(tc_i=0;tc_i<Extend_NUM;tc_i++){
		if(Mapping_Table[EBK+tc_i]==0xFFFF){			
			FDBP.cFDev =BMT.cFDev;
			FDBP.cFPlane =0;
			FDBP.wFBlock = SwapTemp_Table[tc_SwapInx]; 
			Flash_EraseOneBlock_NO_ChkRdy();			
			Mapping_Table[EBK+tc_i]=SwapTemp_Table[tc_SwapInx];
			Wear_Table[ECNT+tc_i] =0x01;
			Mapping_Table[EINX+tc_i]=0xFFFF;
			Mapping_Table[Q2_INX+tc_i] =0x0000;
			Mapping_Table[Q1_INX+tc_i]&=0x00FF;//只清除 Q1 sblock  inx			
			tc_SwapInx++;
			//dbprintf("\nExT[%bX] :%X ",tc_i,Mapping_Table[EBK+tc_i]);
		}
	}	
	for(tc_i=0;tc_i<Dummy_NUM;tc_i++){
		if(Mapping_Table[DBK+tc_i]==0xFFFF){			
			FDBP.cFDev =BMT.cFDev;
			FDBP.cFPlane =0;
			FDBP.wFBlock = SwapTemp_Table[tc_SwapInx]; 
			Flash_EraseOneBlock_NO_ChkRdy();			
			Mapping_Table[DBK+tc_i]=SwapTemp_Table[tc_SwapInx];
			Wear_Table[DCNT+tc_i] =0x01;
			Mapping_Table[Q1_INX+tc_i]&=0xFF00;//只清除 dummy sblock  inx
			tc_SwapInx++;
			//dbprintf("\nDMY[%bX] :%X ",tc_i,Mapping_Table[DBK+tc_i]);
		}
	}	
	for(tc_i=0;tc_i<RANDOM_NUM;tc_i++){
		if(Mapping_Table[RBK+tc_i]==0xFFFF){			
			FDBP.cFDev =BMT.cFDev;
			FDBP.cFPlane =0;
			FDBP.wFBlock = SwapTemp_Table[tc_SwapInx]; 
			Flash_EraseOneBlock_NO_ChkRdy();			
			Mapping_Table[RBK+tc_i]=SwapTemp_Table[tc_SwapInx];
			Mapping_Table[RPAGEINX+tc_i]= 0x00;
			for(tc_j=0; tc_j<MAX_RANDOM_HBLOCK;tc_j++){
				Wear_Table[RINX+tc_j]=0xFF;
			}
			Wear_Table[ECNT+tc_i] =0x01;
			tc_SwapInx++;
			//dbprintf("\nRandom[%bX] :%X ",tc_i,Mapping_Table[RBK+tc_i]);
		}
	}	 
	for(tc_i=0;tc_i<SWAP_NUM;tc_i++){
		if(Mapping_Table[SBK+tc_i]==0xFFFF){			
			FDBP.cFDev =BMT.cFDev;
			FDBP.cFPlane =0;
			FDBP.wFBlock = SwapTemp_Table[tc_SwapInx]; 
			Flash_EraseOneBlock_NO_ChkRdy();			
			Mapping_Table[SBK+tc_i]=SwapTemp_Table[tc_SwapInx];
			Wear_Table[SCNT+tc_i] =0x01;
			tc_SwapInx++;
			//dbprintf("\nSwap_BLK[%bX] :%X ",tc_i,Mapping_Table[SBK+tc_i]);
		}
	}
	if(tc_SwapInx>tw_SwapCnt){// 理論上不可能出現，萬一靈異出現後再想囉～
//		dbprintf("\n 2.lost block:%x,%x",tc_SwapInx,tw_SwapCnt);
	}
}

/*********************************************************************************
* NAME:               void Search_Dummy_Page(U8 tc_ExtInx)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/
void Search_Dummy_Page(U8 tc_ExtInx){
	bit		tbt_Data_0xFF_First;
	
	bit     tbt_Find_E_SBlock;
	U8   tc_i ,tc_Inx;
	U16  tw_Offset;
	FDBP.cFDev= BMT.cFDev;
	FDBP.cFPlane = 0;
	FDBP.wFBlock = Mapping_Table[DBK+tc_ExtInx];
	FDBP.wFPage  = 0;
	FDBP.cFInAddr= 0;
	QTable.WD =0;
	tw_Offset = tc_ExtInx*MAX_DUMMY_PAGE;
	tc_i=0;
	tc_Inx =0;
	// 1. 先到最後一個空D_SBlock ,並將 E ＆  D SBlock  關係建立
	for(gc_D_SBlock=0; gc_D_SBlock<H_INFO.cSMALLBLOCK_NUM; gc_D_SBlock++){
		gw_TargetPage = (U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM;
		FDBP.cFPlane  = gw_TargetPage&(NAND_INFO.cPLANE_NUM-1);
		FDBP.wFPage   = gw_TargetPage/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr = 0;		
		Device_Read_SRAM_Index.BY[0] = 0x98;// 0601 Jay change 0x80-->0x98
		Device_Read_SRAM_Index.BY[1] = 0x00;
		if(Flash_Sequent_Read(2)){//ECC fail 
			gc_E_SBlock = FLASH_REG[0x61];  
			if(CHK_ECCData_FF()){// 再次檢查是否為真的全部都是FF
				tbt_Data_0xFF_First =1;//ECC 全部都是0xFF
			}
			else{
				tbt_Data_0xFF_First=0;  // 非全部都是0xFF;
			}			
		}
		else{ //ECC pass
			gc_E_SBlock = FLASH_REG[0x61]; // 
			tbt_Data_0xFF_First=0;						
		}
		if(tbt_Data_0xFF_First){
			SQTable.WD = Mapping_Table[Q1_INX+tc_ExtInx];	
			SQTable.BY[1] = gc_D_SBlock;
			Mapping_Table[Q1_INX+tc_ExtInx]=SQTable.WD;
			break;
		}
		else{
			tbt_Find_E_SBlock =0;
			for(tc_i=0;tc_i<tc_Inx; tc_i++){
				if(gc_E_SBlock == (Mapping_Table[D_HPAGE+tw_Offset+tc_i]>>8)){
					Mapping_Table[D_HPAGE+tw_Offset+tc_i]= ((U16)gc_E_SBlock<<8)+gc_D_SBlock;
					tbt_Find_E_SBlock=1;
					break;					
				} 				
			}
			if(!tbt_Find_E_SBlock){
				if(tc_Inx<MAX_DUMMY_PAGE){
					Mapping_Table[D_HPAGE+tw_Offset+tc_Inx]= ((U16)gc_E_SBlock<<8)+gc_D_SBlock;
					tc_Inx++;
				}
				else{//應該不會發生這種問題還沒想到要如何處理～～
//					dbprintf("\n Dummy Hpage have full : %bx " ,gc_D_SBlock);
				}			
			}
		}
	}
	//if(!tbt_Data_0xFF_First){//0524 Jay mark 
		SQTable.WD = Mapping_Table[Q1_INX+tc_ExtInx];	
		SQTable.BY[1] = gc_D_SBlock;
		Mapping_Table[Q1_INX+tc_ExtInx]=SQTable.WD;
	//}
	
	//2. 將每個有效的 D_SBlock 中相對應 E_ Spage 範圍找出來
	FDBP.cFDev= BMT.cFDev;
	FDBP.cFPlane = 0;
	FDBP.wFBlock = Mapping_Table[DBK+tc_ExtInx];
	FDBP.wFPage  = 0;
	FDBP.cFInAddr= 0;
	for(tc_i=0; tc_i<tc_Inx;tc_i++){
		gc_D_SBlock = (U8)Mapping_Table[D_HPAGE+tw_Offset+tc_i];	
		tbt_Data_0xFF_First=0;	
		for(gc_SPage=0; gc_SPage<H_INFO.cSMALLPAGE_NUM;gc_SPage++){
			gw_TargetPage = ((U16)gc_D_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_SPage;
			FDBP.cFPlane  = gw_TargetPage&(NAND_INFO.cPLANE_NUM-1);
			FDBP.wFPage   = gw_TargetPage/NAND_INFO.cPLANE_NUM;
			FDBP.cFInAddr = 0;		
			Device_Read_SRAM_Index.BY[0] = 0x98;//0601 Jay change 0x80-->0x98
			Device_Read_SRAM_Index.BY[1] = 0x00;
			if(Flash_Sequent_Read(2)){//ECC fail 
				if(gc_SPage==0){
					QTable.BY[0]= FLASH_REG[0x63];
				}
				if(CHK_ECCData_FF()){// 再次檢查是否為真的全部都是FF
					tbt_Data_0xFF_First =1;//ECC 全部都是0xFF
				}
				else{
					tbt_Data_0xFF_First=0;  // 非全部都是0xFF;
				}			
			}
			else{ //ECC pass
				if(gc_SPage==0){
					QTable.BY[0]= FLASH_REG[0x63];
				}
				tbt_Data_0xFF_First=0;						
			}		
			if(tbt_Data_0xFF_First){
				QTable.BY[1] =QTable.BY[0]+gc_SPage;
				break;
			}
		}
		//if(!tbt_Data_0xFF_First){//0524 jay mark
			QTable.BY[1] =QTable.BY[0]+gc_SPage;
		//}	
		Mapping_Table[D_PAGE+tw_Offset+tc_i]=QTable.WD;
	}
	
}
/*********************************************************************************
* NAME:                void Search_Q2_Table(void)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/
void Search_Random_Page(void){ 
	U8	 tbt_Data_0xFF;
	U8   tc_i,tc_j;
	U8   INX[MAX_RANDOM_HBLOCK];
	U16  tw_Offset;
	U16  tw_HPage;	
	FDBP.cFDev= BMT.cFDev;
	FDBP.cFPlane = 0;
	FDBP.wFBlock = Mapping_Table[RBK];
	FDBP.wFPage  = 0;
	FDBP.cFInAddr= 0;
	QTable.WD =0;
	tbt_Data_0xFF =0;
	for(QTable.WD=0; QTable.WD< H_INFO.wHBLOCKPAGE;QTable.WD++){
		FDBP.cFPlane = QTable.WD%NAND_INFO.cPLANE_NUM;
		FDBP.wFPage  = QTable.WD/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr= 0;
		if(CHK_ECCData_FF()){
			tbt_Data_0xFF =1;
			break;
		}		
	}
	Mapping_Table[RPAGEINX] = QTable.WD;//Next random page index
// 1.需先找出是哪個HBlock 是有效的
	for(tc_i=0;tc_i<MAX_RANDOM_HBLOCK;tc_i++){
		INX[tc_i]=0x00;
	}
	if(QTable.WD==0){
		for(tc_i=0;tc_i<MAX_RANDOM_HBLOCK;tc_i++){
			Wear_Table[RINX+tc_i]=0xFF;
			
		}
		return ;
	}
	else{	
		QTable.WD--;
		FDBP.wFBlock = Mapping_Table[RBK];
		FDBP.cFPlane = QTable.WD%NAND_INFO.cPLANE_NUM;
		FDBP.wFPage  = QTable.WD/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr= 2;	
		Device_Read_SRAM_Index.BY[0] = 0x98;// 0601 Jay add
		Device_Read_SRAM_Index.BY[1] = 0x00;
		Flash_Sequent_Read(2);
		for(tc_i=0;tc_i<MAX_RANDOM_HBLOCK;tc_i++){
			Wear_Table[RINX+tc_i]=FLASH_REG[0x60+tc_i];
		}
	}
// 2. 重建出 Hblock ,HPage & RPage 關係
	FDBP.wFBlock = Mapping_Table[RBK];
	for(tw_HPage=0;tw_HPage<Mapping_Table[RPAGEINX];tw_HPage++){
		FDBP.cFPlane = tw_HPage%NAND_INFO.cPLANE_NUM;
		FDBP.wFPage  = tw_HPage/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr= 0;	
		Device_Read_SRAM_Index.BY[0] = 0x98;//0601 Jay change 0x80 -->0x98
		Device_Read_SRAM_Index.BY[1] = 0x00;
		Flash_Sequent_Read(2);
		if((FLASH_REG[0x61]==0xCC)&&(FLASH_REG[0x63]==0xCC)){// Clear flag
			for(tc_i=0;tc_i<MAX_RANDOM_HBLOCK;tc_i++){
				if(Wear_Table[RINX+tc_i]==FLASH_REG[0x62]){// 相對應HBlock 		
					tw_Offset=(tc_i*MAX_RANDOM_HPAGE);		
					for(tc_j=0;tc_j<MAX_RANDOM_HPAGE;tc_j++){
						Mapping_Table[R_HPAGE+tw_Offset+tc_j]=0xFFFF;		
						Mapping_Table[R_PAGE+tw_Offset+tc_j]=0xFFFF;		
					}
					INX[tc_i] =0x00;
					break;
				}
			}
		}		
		else{
			for(tc_i=0;tc_i<MAX_RANDOM_HBLOCK;tc_i++){
				if(Wear_Table[RINX+tc_i]==FLASH_REG[0x62]){// 相對應HBlock 
					tw_Offset=(tc_i*MAX_RANDOM_HPAGE);
					QTable.BY[0]=FLASH_REG[0x61];
					QTable.BY[1]=FLASH_REG[0x63];
					tbt_Data_0xFF =0;
					for(tc_j=0;tc_j<INX[tc_i];tc_j++){
						if(Mapping_Table[R_HPAGE+tw_Offset+tc_j]==QTable.WD){
							Mapping_Table[R_PAGE+tw_Offset+tc_j]=tw_HPage;
							tbt_Data_0xFF=1;
							break;
						}
					}
					if(!tbt_Data_0xFF){
						if(INX[tc_i]<MAX_RANDOM_HPAGE){				
							Mapping_Table[R_HPAGE+tw_Offset+INX[tc_i]]=QTable.WD;		
							Mapping_Table[R_PAGE+tw_Offset+INX[tc_i]]=tw_HPage;
							INX[tc_i]+=1;
						}
					}
					break;						
				}			
			}
		}
	}
}
/*********************************************************************************
* NAME:               void Search_Q1_Table(void)
* DESCRIPTION:// SPDA26xx flash  plan
*********************************************************************************/
void Search_Q1Q2_Table(U8 tc_ExtInx){
	U8		tbt_Data_0xFF_First;
	U8     tbt_Data_0xFF_Last;
	U8     tbt_Find_Q1_SBlock;
	FDBP.cFDev= BMT.cFDev;
	FDBP.cFPlane = 0;
	FDBP.wFBlock = Mapping_Table[EBK+tc_ExtInx];
	FDBP.wFPage  = 0;
	FDBP.cFInAddr= 0;
	QTable.WD =0;
	tbt_Data_0xFF_First =0;
	tbt_Data_0xFF_Last =0;
	tbt_Find_Q1_SBlock= 0;
	//0. find out first Q1 SBlock 
	gc_E_SBlock =0;	
	gw_TargetPage = (U16)gc_E_SBlock*H_INFO.cSMALLPAGE_NUM;
	FDBP.cFPlane  = gw_TargetPage&(NAND_INFO.cPLANE_NUM-1);
	FDBP.wFPage   = gw_TargetPage/NAND_INFO.cPLANE_NUM;
	FDBP.cFInAddr = 0;		
	Device_Read_SRAM_Index.BY[0] = 0x98;//0601 jay change 
	Device_Read_SRAM_Index.BY[1] = 0x00;
	Flash_ReadSpare(8);
	if(ZSpare[0] ==0xA5){
		if(ZSpare[4]=='Q'){
			SQTable.BY[0] =ZSpare[5];
			tbt_Find_Q1_SBlock= 1;
		}
	}
		
	if(!tbt_Find_Q1_SBlock){
//		dbprintf("\n Not find Q1");
		SQTable.BY[0] =0;
	}
	for(gc_E_SBlock=SQTable.BY[0]; gc_E_SBlock<H_INFO.cSMALLBLOCK_NUM; gc_E_SBlock++){
		// chk 1st page
		gw_TargetPage = (U16)gc_E_SBlock*H_INFO.cSMALLPAGE_NUM;
		FDBP.cFPlane  = gw_TargetPage&(NAND_INFO.cPLANE_NUM-1);
		FDBP.wFPage   = gw_TargetPage/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr = 0;		
		Device_Read_SRAM_Index.BY[0] = 0x98;  //0601 Jay fix 
		Device_Read_SRAM_Index.BY[1] = 0x00;
		if(Flash_Sequent_Read(2)){//ECC fail 
			if(CHK_ECCData_FF()){// 再次檢查是否為真的全部都是FF
				tbt_Data_0xFF_First =1;//ECC 全部都是0xFF
			}
			else{
				tbt_Data_0xFF_First=0;  // 非全部都是0xFF;
			}			
		}
		else{ //ECC pass
			tbt_Data_0xFF_First=0;
										
		}		
		// Chk last page
		gw_TargetPage = ((U16)gc_E_SBlock*H_INFO.cSMALLPAGE_NUM)+(H_INFO.cSMALLPAGE_NUM-1);
		FDBP.cFPlane  = gw_TargetPage&(NAND_INFO.cPLANE_NUM-1);
		FDBP.wFPage   = gw_TargetPage/NAND_INFO.cPLANE_NUM;
		FDBP.cFInAddr = 0;		
		Device_Read_SRAM_Index.BY[0] = 0x98;//0601 Jay fix
		Device_Read_SRAM_Index.BY[1] = 0x00;
		if(Flash_Sequent_Read(2)){//ECC fail 
			if(CHK_ECCData_FF()){// 再次檢查是否為真的全部都是FF
				tbt_Data_0xFF_Last =1;//ECC 全部都是0xFF
			}
			else{
				tbt_Data_0xFF_Last=0;  // 非全部都是0xFF;
			}			
		}
		else{ //ECC pass
			tbt_Data_0xFF_Last=0;						
		}		
		if(tbt_Data_0xFF_First||tbt_Data_0xFF_Last){
			QTable.BY[0] =gc_E_SBlock;
			break;
		}
	}
	//2. find out E_Spage
	if(tbt_Data_0xFF_First&&tbt_Data_0xFF_Last){
		QTable.BY[1] =0;
	}
	else if(tbt_Data_0xFF_Last){// Spage0不是空 但 Last Spage 是空的 這個SBlock 不是滿的 
		tbt_Data_0xFF_First =0;
		FDBP.wFBlock = Mapping_Table[EBK+tc_ExtInx];
		for(gc_E_SPage=0; gc_E_SPage<H_INFO.cSMALLPAGE_NUM;gc_E_SPage++){
			gw_TargetPage = ((U16)gc_E_SBlock*H_INFO.cSMALLPAGE_NUM)+gc_E_SPage;
			FDBP.cFPlane  = gw_TargetPage&(NAND_INFO.cPLANE_NUM-1);
			FDBP.wFPage   = gw_TargetPage/NAND_INFO.cPLANE_NUM;
			FDBP.cFInAddr = 0;		
			Device_Read_SRAM_Index.BY[0] = 0x98;//0601 Jay fix 0x80-->0x98
			Device_Read_SRAM_Index.BY[1] = 0x00;
			if(Flash_Sequent_Read(2)){//ECC fail 
				if(CHK_ECCData_FF()){// 再次檢查是否為真的全部都是FF
					tbt_Data_0xFF_First =1;//ECC 全部都是0xFF
				}
				else{
					tbt_Data_0xFF_First=0;  // 非全部都是0xFF;
				}			
			}
			else{ //ECC pass
				tbt_Data_0xFF_First=0;						
			}		
			if(tbt_Data_0xFF_First){
				QTable.BY[1] =gc_E_SPage;
				break;
			}
		}
			
	} 
	else{// 這個狀況應該是整個 block 都是滿的有dummy 情況
		QTable.BY[0] =H_INFO.cSMALLBLOCK_NUM;
		QTable.BY[1] = 0;		
	}
//	dbprintf("\n Search Q1_INX:%bx Q1 = %bx" , tc_ExtInx , SQTable.BY[0]);
//	dbprintf("\n search Q2 INX:%bx Q2=%X", tc_ExtInx,QTable.WD);
	Mapping_Table[Q2_INX+tc_ExtInx] =QTable.WD;  //Q2 page
	Mapping_Table[Q1_INX+tc_ExtInx] =SQTable.WD;//Q1 sblock
}
/*********************************************************************************
* NAME:                  U16 Search_Last_Page(U16 TBlock)                         
* DESCRIPTION: 找尋最後一個有資料 page  // SPDA26xx flash  plan                                          
*********************************************************************************/
U16 Search_Last_Page(U16 TBlock){//090820  Jay add                                                 
    U8 	tbt_Data_0xFF=0;                                                        
    FDBP.wFBlock = TBlock;
    gw_TargetPage=(H_INFO.wHBLOCKPAGE-1);
    while(gw_TargetPage){    
    //for(gw_TargetPage=0;gw_TargetPage<H_INFO.wHBLOCKPAGE;gw_TargetPage++){
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
                                                                                  
}/*********************************************************************************
* NAME:                bit CHK_ECCData_FF(void)
* DESCRIPTION:
*********************************************************************************/
U8 CHK_ECCData_FF(void){ 
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