#include <intrins.h>
#include "..\header\SPDA2K.h"
#include "..\header\variables.h"


void SPI_Init(void)
{
	XBYTE[0xB400]=4;		// Turn FMGPIO to SPI mode.
	XBYTE[0xBB10]=0x0F;
	XBYTE[0xBB14]=0x02;		// mode 0, manual contro CE.
	SPI_Wait_Busy();
	SPI_Read_ID();
	SPI_Device_Parameter();
}


void SPI_Read_ID(void)
{
	SPI_ENABLE();			/* enable device */
	SPI_Send_Byte(0x90);		/* send read ID command (90h or ABh) */
	SPI_Send_Byte(0x00);		/* send address */
	SPI_Send_Byte(0x00);		/* send address */
	SPI_Send_Byte(0x00);		/* send address - either 00H or 01H */
	SPI_Get_Byte();		/* receive byte */
	SPI_DISABLE();			/* disable device */
}


void  SPI_Device_Parameter(void)
{
    SPI_ENABLE();
	SPI_Send_Byte(0x9F);
   	gc_SPI_ID=SPI_Get_Byte();
	SPI_DISABLE();

	// Enable write status register
    SPI_ENABLE();
	SPI_Send_Byte(0x50);
	SPI_DISABLE();
	// Write status register(0x00)-Disable Block write protection
    SPI_ENABLE();
	SPI_Send_Byte(0x01);
	SPI_Send_Byte(0x00);
	SPI_DISABLE();
}


