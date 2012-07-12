#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\header\variables.h"
#include "LCM_BMP.h"


void LCM_set_address(U8 page,U8 col)
{
	LCM_write_command(page|0xb0);
	LCM_write_command((col>>4)|0x10);
	LCM_write_command(col&0x0f);
}


void LCM_ReadSPIBMPdata(U16 tw_Addr,U16 tw_Length)
{
	data	U16	i;
	data	U16	PageNum;
	unionU16	tdw_SPIAddr;

	tw_Addr=tw_Addr-0x6000;
	PageNum=16+(tw_Addr/512)*2;	// 1-page=256-byte
	tw_Addr=tw_Addr&0x1FF;
	Device_Read_SRAM_Index.BY[0]=gc_PlayRecordDataBufHB;
	Device_Read_SRAM_Index.BY[1]=0x00;
	tdw_SPIAddr.BY[0]=PageNum;
	tdw_SPIAddr.BY[1]=2;

	i=0;
	while(1)
	{
		SPI_Read_Data(tdw_SPIAddr);
		while(1)
		{
			gc_DirLCMtempBuf[i]=gc_PlayRecordDataBuf[tw_Addr];
			tw_Length--;
			if(tw_Length==0)
			{
				return;
			}
			i++;
			tw_Addr++;
			if(tw_Addr==512)
			{
				break;
			}
		}
		tdw_SPIAddr.BY[0]+=2;
		tw_Addr=0;
	}
}



