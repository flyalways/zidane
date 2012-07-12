#include "spda2k.h"
#include "..\LCM\model_define.h"
#include "..\LCM\LCM.h"
#include "..\LCM\LCM_func.h"
#include "..\header\variables.h"
//extern xdata U16 gw_icon_sector;

void USER_ReadReserveBlock(U16 tw_ReserveBLKSector)
{
	DSPHZK_Block_Read(1,tw_ReserveBLKSector,1,gc_PlayRecordDataBufHB);
}

void User_ReadReserveBlockData_To_TFT(U32 tdw_StartByteAddr,U32 tdw_ByteNum)
{
	xdata	U8	*mbDmaAddr;
	data	U8	tw_SectorNum; 
	data	U8	tw_Loop;
	data	U16	tw_LoopNum;
	data	U16 DmaDst;
	data	U16	tw_StartLogicSectorAddr;
	data	U16	tw_ByteStartOffset;
	data	U16	tw_ByteEndOffset;

	//-----------------------------------------------------------------------------
	tw_StartLogicSectorAddr = tdw_StartByteAddr>>9;  //将起始字节地址转换为起始sector地址
	tw_ByteStartOffset = tdw_StartByteAddr & 0x1ff;//start sector offset
	tw_ByteEndOffset = (tdw_StartByteAddr + tdw_ByteNum) & 0x1ff; //end sector offset
	tw_SectorNum=tdw_ByteNum>>9;                     //计算共有多少个sector

	if((tdw_ByteNum & 0x1ff) == 0)
	{
		if(tw_ByteStartOffset!=0)
		{
			tw_SectorNum++;
		}
	}
	else
	{
		tw_SectorNum++;
		if((tw_ByteStartOffset + (tdw_ByteNum & 0x1ff)) > 512 ) 	  
		{
			tw_SectorNum++;
		}
	}

	//------------------------------find kernel phyaddress-----------------------------------------
	EA = 0;

	for(tw_Loop=0;tw_Loop<tw_SectorNum;tw_Loop++)
	{	
		//--------------------------------Flash to SRAM------------------------------------
		USER_ReadReserveBlock(tw_StartLogicSectorAddr+tw_Loop);

		//for 023Only
		LCD_Nand2DataMode();

        XBYTE[0xB304]=0x09; //reset DMA machine and setting pad 0 enable 
        XBYTE[0xB304]=0x08; 
		//-------------------------------SRAM to Flash------------------------------------
		if(tw_Loop == 0)
		{
			if((tdw_ByteNum<512)&&(tw_SectorNum==1))   
			{
				tw_LoopNum = tdw_ByteNum;
			}
			else
			{                                         
				tw_LoopNum = 512-tw_ByteStartOffset;
			}
			mbDmaAddr = &gc_PlayRecordDataBuf[tw_ByteStartOffset];
		}
		else if(tw_Loop < tw_SectorNum-1)
		{
			tw_LoopNum = 512;				
			mbDmaAddr = &gc_PlayRecordDataBuf[0];
		}
		else
		{
			if(tw_ByteEndOffset == 0)   
			{
				tw_LoopNum = 512;	
			}
			else
			{
				tw_LoopNum = tw_ByteEndOffset;
			}
			mbDmaAddr = &gc_PlayRecordDataBuf[0];			
		}
	
		DmaDst = (U16)mbDmaAddr;
		tw_LoopNum = tw_LoopNum-1;    //DMA transfer size 

		//source
		XBYTE[0xB112] = (*((U8*)&DmaDst+1));
		XBYTE[0xB113] = (*((U8*)&DmaDst));					

		XBYTE[0xB302]=((U8 *)(&tw_LoopNum))[1];
		XBYTE[0xB303]=((U8 *)(&tw_LoopNum))[0];				

		XBYTE[0xB3C0] =	0x00;//clear DMA completed flag
		LCM_A0_HI;
		LCM_CS_LO;

		XBYTE[0xB3B0]=0x01;	//trigger DMA
		while(XBYTE[0xB3C0]==0);
		XBYTE[0xB3C0]=0;

		LCD_Data2NandMode();
	}	
	XBYTE[0xB113]=0xFF;		//unlock SRAM for DMA(src)  2006-04-19
	XBYTE[0xB112]=0xFF;	
    XBYTE[0xB304]=0x09; 	//reset DMA machine and setting pad 0 enable 
    XBYTE[0xB304]=0x08; 
	EA = 1;
}

void LCD_DisplayIcon(U8 tc_IconIDNumber,U8 tc_CurrentOffset)
{
	U8  tc_Sector;
	U16 tw_ByteOffset;
	U32 tdw_StartByteAddr;
	U32 tdw_ByteNum;

	tc_IconIDNumber += 1; //前面8个 Byte用来记录ID总数和Chuck sum,应跳过   
 	tc_Sector = tc_IconIDNumber>>6;
	tw_ByteOffset = ((U16)tc_IconIDNumber << 3) & 0x1ff;
	EA = 0;
	USER_ReadReserveBlock(gw_icon_sector + tc_Sector);
	EA = 1;

	((U8 *)&tdw_StartByteAddr)[0]=gc_PlayRecordDataBuf[tw_ByteOffset+0]; 
	((U8 *)&tdw_StartByteAddr)[1]=gc_PlayRecordDataBuf[tw_ByteOffset+1]; 
	((U8 *)&tdw_StartByteAddr)[2]=gc_PlayRecordDataBuf[tw_ByteOffset+2];
	((U8 *)&tdw_StartByteAddr)[3]=gc_PlayRecordDataBuf[tw_ByteOffset+3];
	((U8 *)&tdw_ByteNum)[0] = gc_PlayRecordDataBuf[tw_ByteOffset+4];
	((U8 *)&tdw_ByteNum)[1] = gc_PlayRecordDataBuf[tw_ByteOffset+5];
	((U8 *)&tdw_ByteNum)[2] = gc_PlayRecordDataBuf[tw_ByteOffset+6];
	((U8 *)&tdw_ByteNum)[3] = gc_PlayRecordDataBuf[tw_ByteOffset+7];
	tdw_StartByteAddr = tdw_StartByteAddr + tdw_ByteNum * tc_CurrentOffset;
	User_ReadReserveBlockData_To_TFT(((U32)gw_icon_sector*512)+tdw_StartByteAddr,tdw_ByteNum);	
}



