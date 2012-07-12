#include "..\header\SPDA2K.h"

#define RELIABLE_CODE	0xA5

void RTC_Init(void)
{
	data	U8	i;
	data	U8	reliable_code;
	data	U16	count;

	XBYTE[0xB066]=0x21;		// RTC serial interface(bit 0) Enable
	RTC_Write(0x00,0x0D);	// Enable RTC, Load mode
	reliable_code=RTC_Read(0x02);
	if(reliable_code!=RELIABLE_CODE)	// reliable code is not reliable. RTC needs a reset.
	{
		XBYTE[0xB002]|=0x40;	// reset RTC
		XBYTE[0xB002]&=~0x40;
		RTC_Write(0x00,0x0F);	// enable RTC, load mode, reset RTC macro according to C1's suggestion.
		RTC_Write(0x00,0x0D);	// enable RTC, load mode
		RTC_DisableInterrupt();	// disable RTC interrupt here to power off for avoiding RTC auto start-up
								// when 1st time powered. RELIABLE_CODE writing should be after this line
								// for judging whether it is the 1st-time RTC setting in later power-on.

		RTC_Write(0x02,RELIABLE_CODE); // write reliable code
		count=1000;
		while(--count)
		{
			reliable_code=RTC_Read(0x02);
			if(reliable_code==RELIABLE_CODE)
			{
				break;
			}
			else
			{
				RTC_Write(0x02,RELIABLE_CODE);
			}
		}

		if(count==0)
		{
			return;
		}
		
		// Clear RTC alarm counter & RTC time value
		for(i=0;i<6;i++)
		{
			RTC_Write(0x10+i,0);
			RTC_Write(0x20+i,0);
		}
		RTC_Write(0xB0,0x01);	// load start value
		RTC_Write(0x00,0x05);	// enable RTC, read mode
	}

	RTC_EnableInterrupt();
	RTC_Write(0x00,0x05); // enable RTC, read mode
	if(RTC_Read(0xD0)&0x03) // if RTC interrupt enabled
	{
		XBYTE[0xB0D0]|=0x08;	// RTC interrupt enable
	}
}



