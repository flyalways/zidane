#include "..\header\SPDA2K.h"
#include "..\header\variables.h"

void ReadSectors(void) 
{
	Check_LBA_ID();
	XBYTE[0xB5E9]=0x01;				//Clear EP1
	gb_OddTemp=0;				    //Device Buffer Pin-pon use~~
	SD_ReadSectors_USB();
	gc_USBStage=CSWStage;
}

 
void NoCard_ReadSectors(void) 
{
	memset(USB_BUF_Bank0,0x00,512);
	XBYTE[0xB510]=0x01;	
	
	do {
		XBYTE[0xB515]=0x00;
		XBYTE[0xB516]=0x02;
		XBYTE[0xB5A1]|=0x01;
		while(XBYTE[0xB5A1]&0x01);
		TotalXferPageNum.WD--;
		XBYTE[0xB5E9]=0x01;
	} while(TotalXferPageNum.WD);
	XBYTE[0xB510]=0x00;
}


void WriteSectors(void) 
{
	gb_OddTemp=0;
	Check_LBA_ID();
	SD_WriteSectors_USB();
	gc_USBStage=CSWStage;
}


void NoCard_WriteSectors(void) 
{
	while(TotalXferPageNum.WD) 
	{
		XBYTE[0xB511]=0x01;	//Turn on BulkOutEn, Turn off by data xfer done.
		XBYTE[0xB515]=0x00;
		XBYTE[0xB516]=0x02;
		XBYTE[0xB519]=0x00;
		XBYTE[0xB5A1]|=0x02;
		while(XBYTE[0xB5A1]&0x02);
		XBYTE[0xB5C2]&=0xFD;
		XBYTE[0xB511]=0x00;	//Turn-off BulkOutEn
		XBYTE[0xB5E9]=0x02;
		TotalXferPageNum.WD--;
	}
}


