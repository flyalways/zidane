#include "SPDA2K.h"

void	disable_rtc_interrupt(void);
void	write_rtc(U8 address, U8 x);
U8		read_rtc(U8 address);

xdata	U32	gdw_RTC_CurrentSecond;

U8 read_rtc(U8 address)
{
	XBYTE[0xB068] = address;
	XBYTE[0xB067] = 2; // trigger read
	while ((XBYTE[0xB06B] & 1) == 0);
	return XBYTE[0xB06A];
}

void write_rtc(U8 address, U8 x)
{
	XBYTE[0xB068] = address;
	XBYTE[0xB069] = x;
	XBYTE[0xB067] = 1; // trigger write

	while ((XBYTE[0xB06B] & 1) == 0);
}

void disable_rtc_interrupt(void)
{
	XBYTE[0xB0D0] &= ~0x08;
	write_rtc(0xd0, 0); // disable rtc second and alarm interrupt
	write_rtc(0xc0, 0); // clear rtc second and alarm interrupt event
}

void enable_rtc_interrupt(void)
{
	XBYTE[0xB0D0] |= 0x08;
	write_rtc(0xc0, 0); // disable rtc second and alarm interrupt
	write_rtc(0xd0, 1); // clear rtc second and alarm interrupt event
}
