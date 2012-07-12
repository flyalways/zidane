#include "SPDA2K.h"


extern	void	disable_rtc_interrupt(void);
extern	void	enable_rtc_interrupt(void);
extern	void	write_rtc(U8 address,U8 x);
extern	U8		read_rtc(U8 address);
	
#define RTC_mark	0x50


