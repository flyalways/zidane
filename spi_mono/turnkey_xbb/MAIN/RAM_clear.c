#include "..\header\SPDA2K.h"


void InitSystemRAM(void)
{
	// Clear System RAM(0x7000~0x7FFF)
#pragma asm
		mov		dptr,#7000h
?loop:	movx	@dptr,a
		inc		dptr
		mov		r0,dph
		cjne	r0,#80h,?loop
#pragma endasm
}

void InitCPURAM(void)
{
	// Clear 8051 RAM(20h~8Fh)
#pragma asm
		clr		a
		mov		r0,#20h
?loop1:	mov		@r0,a
		inc		r0
		cjne	r0,#90h,?loop1
#pragma endasm
}
