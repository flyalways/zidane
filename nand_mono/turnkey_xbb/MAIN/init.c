#include "SPDA2K.h"
#include "..\header\variables.h"

extern void init_const_timer(void);

void init_system(void)
{
	/**************************************************************************
	 * Set CKCON
	 **************************************************************************/
	// CKCON
	//		T0_PRE  = 0		not prescaling timer 0
	//		MD_SRW  = 0
	//		MDR2    = 0		//timer2 * 3
	//		MDR1    = 0		//timer1 * 3
	//		MDR0    = 0		//timer0 * 3
	//		MDW2    = 0
	//		MDW1    = 1
	//		MDW0    = 0
	CKCON    = 0x10;//(JC)H0229

	// Initial Global Register
	/**************************************************************************
	 * Reset hardware macros.
	 **************************************************************************/
	XBYTE[0xB002] = 0x07;	// 0000 0111 reset USB device
	XBYTE[0xB002] = 0x0B;	// 0000 1011 reset FM(flash memory) & memory card, include DSP memory mode, DMA, FM regs
	XBYTE[0xB002] = 0x13;	// 0001 0011 reset IR controller
	XBYTE[0xB002] = 0x83;	// 1000 0011 reset DSP memory controller
	XBYTE[0xB002] = 0x03;	// 0000 0011 reset finish (keep reset DSP and HOST)

	XBYTE[0xB010] = 0x00;	// Disable DSP & DSP memory controller clock
	XBYTE[0xB011] = 0x46;	// Enable DMA, FM, RTC clock.  Disable USB, Host clock.

	/**************************************************************************
	 * Initialize timer and UART
	 **************************************************************************/
	// timer
	// TMOD					Page 83
	//		GATE_T1 = 0
	//		C/Tj_T1 = 0		interval timer
	//		M1_T1   = 1		mode 2: 8-bit auto-reload mode
	//		M0_T1   = 0
	//		GATE_T0 = 0
	//		C/Tj_T0 = 0		interval timer
	//		M1_T0   = 0		mode 1: 16-bit timer mode
	//		M0_T0   = 1
	TMOD = 0x21;
	// TCON					Page 84
	//		TF1     = 0		Timer 1 overflow flag.
	//		TR1     = 0		Timer 1 run-control bit.
	//		TF0     = 0		Timer 0 overflow flag.
	//		TR0     = 0		Timer 0 run-control bit.
	//		IE1     = 0		External interrupt 1 edge flag.
	//		IT1     = 1     External interrupt 1 type flag. 1 = edge, 0 = level.
	//		IE0     = 0		External interrupt 0 edge flag.
	//		IT0     = 1     External interrupt 0 type flag. 1 = edge, 0 = level.
	TCON = 0x05;
	// TIMER1 MODE2
	// BAUD = 
	//      SMOD
	//     2			F            CKCON[4]
	//   -----   X  ------------- X 3 
	//     32		 12X(256-TH1)
	TH1 = 243;	// UART baud rate = 14400bps at CPU 12MHz
	
	// UART
	// SCON					Page 101
	//		SM0     = 1		9-bit UART, variable baud rate set by timer 1.
	//		SM1     = 1
	//		SM2     = 0
	//		REN     = 0		Receiver enabler.
	//		TB8     = 1		Transmit bit 8. 9th bit transmitted in modes 2 and 3.
	//		RB8     = 0		Receive bit 8. 9th bit received.
	//		TI      = 0		Transmit interrupt flag.
	//		RI      = 0		Receive interrupt flag.
	SCON = 0xC8;
    	
	//PCON[7]
	//      SMOD    = 0 	double the UART Baud rate
	PCON = 0x80;	
    	
	TR1	= 1;		// TR1:  timer 1 run
	TI	= 1;		// TI:   set TI to send first char of UART

	/**************************************************************************
	 * Set clock to MP3 mode.  TBD : Should be changed to a lower clock rate.
	 **************************************************************************/
//	set_clock_mode(CLOCK_MODE_XTL);
	/**************************************************************************
	 * Set GPIO directions.  GPIO usage may vary between different turnkeys.
	 **************************************************************************/
	P1 = 0xff;	// P1 output high to turn off transistor and to avoid input grounded.

    // P1 is used for LCM and i2c. Find details at thoses modules.
    // P1.0~P1.2 for LCM;
    // P1.3 is key rest to key scan ic. But it is used as play key in 128 evb...
    // P1.4~P1.5 for i2c;
	CPU_REG[0x02] = 0x3f;
   	P3 = 0xff;	// P3 output high to turn off transistor and to avoid input grounded.
	CPU_REG[0x03] = 0x02;	// set P3.1 UART Tx pin output enabled.

	/**************************************************************************
	 * Set constant timer and RTC(if necessary).
	 * TBD : RTC status needs a nonvolatile record.
	 **************************************************************************/
	init_const_timer();	// initialize constant timer.
	TH0 = gc_T0IntervalMSB;
	TL0 = gc_T0IntervalLSB;
	ET0 = 1;
	TR0 = 1;
	gbt_newFlash	= 0;						//09/02/06,joyce add for new flash  
    gbt_Support_Dummy_Capacity =0;              // default disable dummy capacity function 
	gbt_enableCDROM =0;							//09/04/24,joyce
	gc_CDROM_Size=0 ;
}
