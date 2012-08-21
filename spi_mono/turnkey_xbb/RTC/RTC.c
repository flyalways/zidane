#include "..\header\SPDA2K.h"


void RTC_Write(U8 address,U8 x)
{
	XBYTE[0xB068]=address;
	XBYTE[0xB069]=x;
	XBYTE[0xB067]=1; // trigger write
	while((XBYTE[0xB06B]&0x01)==0);
}


U8 RTC_Read(U8 address)
{
	XBYTE[0xB068]=address;
	XBYTE[0xB067]=2; // trigger read
	while((XBYTE[0xB06B]&0x01)==0);
	return XBYTE[0xB06A];
}


void RTC_DisableInterrupt(void)
{
	XBYTE[0xB0D0]&=~0x08;
	RTC_Write(0xD0,0); // disable rtc second and alarm interrupt
	RTC_Write(0xC0,0); // clear rtc second and alarm interrupt event
}


void RTC_EnableInterrupt(void)
{
	XBYTE[0xB0D0]|=0x08;
	RTC_Write(0xD0,1); // Enable RTC second interrupt
	RTC_Write(0xC0,0); // clear rtc second and alarm interrupt event
}
