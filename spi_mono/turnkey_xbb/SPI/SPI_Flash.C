#include <intrins.h>
#include "..\header\SPDA2K.h"
#include "..\header\variables.h"


void SPI_Send_Byte(U8 out)
{
    XBYTE[0xBB00]=out;
	while((XBYTE[0xBB1C]&0x10)==1);
	XBYTE[0xBB10]|=0x02;			// clear dummy read	
}


U8 SPI_Get_Byte(void)
{
    XBYTE[0xBB00]=0xFF;					// write dummy to trigger read.
	while((XBYTE[0xBB1C]&0x10)==1);
	return XBYTE[0xBB00];
}


U8  SPI_Read_Status(void)
{
	xdata	U8  byte;
	
	SPI_ENABLE();			/* enable device */
	SPI_Send_Byte(0x05);		/* send RDSR command */
	byte = SPI_Get_Byte();		/* receive byte */
	SPI_DISABLE();			/* disable device */
	return byte;    
}


void   SPI_Wait_Busy(void)
{
    while ((SPI_Read_Status() & 0x01))	/* waste time until not busy */
	{
		SPI_Read_Status();
	}
}


void  SPI_WREN(void)
{
	SPI_ENABLE();
	SPI_Send_Byte(0x06);   
	SPI_DISABLE();
} 


void  SPI_WRDI(void)
{
    SPI_ENABLE();
    SPI_Send_Byte(0x04);   
    SPI_DISABLE();
}


void  SPI_Read_Data(unionU16 tw_SPIAddr)
{ 
	data	U8	i;

	XBYTE[0xB400]|=0x20;	//reset spi interface 
	XBYTE[0xB400]=0x04;                     
	SPI_ENABLE(); 
	SPI_Send_Byte(0x03);  //CMD
	SPI_Send_Byte(tw_SPIAddr.BY[1]);  
	SPI_Send_Byte(tw_SPIAddr.BY[0]);  
	SPI_Send_Byte(0x00);  
	XBYTE[0xB304]=0x09;		//Reset DMA
	XBYTE[0xB304]=0x08;		//Normal DMA
	Device_Xfer_DataLength.WD=0x01FF;			//read 512 byte
	SET_DEVICE_READ();
	XBYTE[0xBB10]=0x0F;						// after DMA, SPI FIFO has to be cleared
	Trigger_Data_Transfer_Done();
    SPI_DISABLE();		    
	for(i=0;i<200;i++); //一定要delay太快 會lose data ~~~
}

void  SPI_Write_Data(unionU16 tw_SPIAddr)
{    
    if(gc_SPI_ID==0xBF)	// SST
	{
        SPI_Write_Data_ADh(tw_SPIAddr);   
    }
    else
	{
        SPI_Write_Data_02h(tw_SPIAddr); 
    }
}


void  SPI_Write_Data_02h(unionU16 tw_SPIAddr)
{
    data	U8 tw_Pagecnt;

    tw_Pagecnt=2;
    while(tw_Pagecnt)
	{
        SPI_WREN();// enable WR   
        SPI_ENABLE();         
        SPI_Send_Byte(0x02);  //CMD
        SPI_Send_Byte(tw_SPIAddr.BY[1]);  
        SPI_Send_Byte(tw_SPIAddr.BY[0]); 
        SPI_Send_Byte(0x00);    
        Device_Xfer_DataLength.WD = 0x00FF;	
        SET_DEVICE_WRITE();
        Trigger_Data_Transfer_Done();
        Device_Write_SRAM_Index.WD+=0x100;
        tw_SPIAddr.BY[0]++;
        tw_Pagecnt--;
        SPI_DISABLE();	
	    SPI_Wait_Busy();
	    SPI_WRDI();	    
	}
}


void  SPI_Write_Data_ADh(unionU16 tw_SPIAddr)
{
    data	U8 i;

    SPI_WREN();// enable WR
    SPI_ENABLE();
	SPI_Send_Byte(0x80);
	SPI_DISABLE();

    SPI_ENABLE();
    SPI_Send_Byte(0xAD);  //CMD
    SPI_Send_Byte(tw_SPIAddr.BY[1]);  
    SPI_Send_Byte(tw_SPIAddr.BY[0]);  
    SPI_Send_Byte(0x00);
    i=0;
	while(1)
	{
		SPI_Send_Byte(gc_PlayRecordDataBuf[i*2]);
		SPI_Send_Byte(gc_PlayRecordDataBuf[i*2+1]);            
		SPI_DISABLE();
		while(SPI_Read_Status()&0x01);
		SPI_ENABLE();           
		SPI_Send_Byte(0xAD);  
		i++;
		if(i==0)
		{
			break;
		}
	}
	SPI_DISABLE();	
	SPI_Wait_Busy();
	SPI_WRDI();
	SPI_Read_Status();
}


void  SPI_Erase_Block(U8 tc_BlockNum)
{
	XBYTE[0xB400]|=0x20;	//reset spi interface 
	XBYTE[0xB400]=0x04;                     
    SPI_WREN();// enable WR
    SPI_ENABLE();
    SPI_Send_Byte(0xD8);  //CMD
	SPI_Send_Byte(tc_BlockNum);  
	SPI_Send_Byte(0x00);  
	SPI_Send_Byte(0x00); 
   	SPI_DISABLE();
	SPI_Wait_Busy();
	SPI_Read_Status();  
}


void SPI_UserData_Block_Erase(void)
{
    SPI_Erase_Block(SPI_USERdata_AREA);
}


void SPI_UserData_Block_Read(U16 Sector_Addr,U8 Buf_Index)
{
	data	unionU16	tdw_SPIAddr;

	Device_Read_SRAM_Index.BY[0]=Buf_Index;
	Device_Read_SRAM_Index.BY[1]=0x00;
	tdw_SPIAddr.BY[0]=Sector_Addr<<1;
	tdw_SPIAddr.BY[1]=Sector_Addr/128+SPI_USERdata_AREA;
	SPI_Read_Data(tdw_SPIAddr);
}


void SPI_UserData_Block_Write(U16 Sector_Addr,U8 Buf_Index)
{
	data	unionU16	tdw_SPIAddr;

	XBYTE[0xB400]|=0x20;	//reset spi interface 
	XBYTE[0xB400]= 0x04;                     

	Device_Write_SRAM_Index.BY[0]=Buf_Index;
	Device_Write_SRAM_Index.BY[1]=0x00;
	tdw_SPIAddr.BY[0]=Sector_Addr<<1;
	tdw_SPIAddr.BY[1]=Sector_Addr/128+SPI_USERdata_AREA;
	SPI_Write_Data(tdw_SPIAddr);
}


void SPI_DSPHZK_Block_Read(U8 Area,U16 Offset_Addr,U8 Buf_Index)
{
	unionU16	tdw_SPIAddr;
	XBYTE[0xB400]|=0x20;	//reset spi interface 
	XBYTE[0xB400]= 0x04;                     

	Device_Read_SRAM_Index.BY[0]=Buf_Index;
	Device_Read_SRAM_Index.BY[1]=0x00;
	tdw_SPIAddr.BY[0]=Offset_Addr<<1;
	tdw_SPIAddr.BY[1]=Offset_Addr/128;

	if(Area==1)
	{
		tdw_SPIAddr.BY[1]+=SPI_HZK_AREA;	// HZK : Block8 ~
    }
    else
	{
		tdw_SPIAddr.BY[1]+=SPI_DSP_AREA;	// DSP : Block3 ~ Block5
    }
    SPI_Read_Data(tdw_SPIAddr);
}


void Trigger_Data_Transfer(void) 
{
	XBYTE[0xB3C0] = 0x00;										//Clear DMA Xfer done interrupt
	XBYTE[0xB3B0] = 0x01;										//Trigger Data Xfer
}


void Trigger_Data_Transfer_Done(void) 
{
	XBYTE[0xB3B0]=0x01;
	while(!(XBYTE[0xB3C0]&0x01))	//Wait Data xfer done
	{
	}						
	XBYTE[0xB3C0]=0x00;										//Clear DMA Xfer done interrupt
}


void USER_Delay(U16 DT)
{
	data	U16	i;
	data	U8	j;
	data	U8	k;

	if(gc_clock_mode==2)		DT*=2;
	else if(gc_clock_mode==5)	DT*=4;
	else if(gc_clock_mode==6)	DT*=4;
	else if(gc_clock_mode==4)	DT*=6;
	for(i=0;i<DT;i++)
	{
		for(j=155;j>0;j--)
		{
			for(k=3;k>0;k--);
		}
	}
}


void SET_DEVICE_READ(void) 
{
	XBYTE[0xB301]=0x02;								//FLASH to SRAM
	XBYTE[0xB302]=Device_Xfer_DataLength.BY[1];
	XBYTE[0xB303]=Device_Xfer_DataLength.BY[0];		//傳送長度
	XBYTE[0xB114]=Device_Read_SRAM_Index.BY[1];
	XBYTE[0xB115]=Device_Read_SRAM_Index.BY[0];		//SRAM Index
}


void SET_DEVICE_WRITE(void) 
{
	XBYTE[0xB301]=0x20;								//SRAM to FLASH
	XBYTE[0xB302]=Device_Xfer_DataLength.BY[1];
	XBYTE[0xB303]=Device_Xfer_DataLength.BY[0];		//傳送長度
	XBYTE[0xB112]=Device_Write_SRAM_Index.BY[1];
	XBYTE[0xB113]=Device_Write_SRAM_Index.BY[0];	//SRAM Index
} 


void SET_DEVICE_READ_TO_DRAM(void)
{
	XBYTE[0xB301]=0x62;	// FLASH to Program RAM
	XBYTE[0xB302]=0xFF;               
	XBYTE[0xB303]=0x01;
	//DRAM Index
	XBYTE[0xB118] = 0x00;               
	XBYTE[0xB119] = gc_ProgRAM_Index;               
}

