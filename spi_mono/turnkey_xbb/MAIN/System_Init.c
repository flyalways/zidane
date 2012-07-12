#include "..\header\SPDA2K.h"
#include "..\header\variables.h"


void Sysetm_Init(void)
{
	XBYTE[0xB091]=0x01;	// 1.8v/3.3v regulator turn on 
	XBYTE[0xB092]=0x77; // 3.2V & 1.9V
	XBYTE[0xB093]=0x00;	// LVR=2.4V
	XBYTE[0xB0D1]=0xF0;
	
	XBYTE[0xB002]=0x07;	// 0000 0111 reset USB device
	XBYTE[0xB002]=0x0B;	// 0000 1011 reset FM(flash memory) & memory card, include DSP memory mode, DMA, FM regs
	XBYTE[0xB002]=0x13;	// 0001 0011 reset IR controller
	XBYTE[0xB002]=0x83;	// 1000 0011 reset DSP memory controller
	XBYTE[0xB002]=0x03;	// 0000 0011 reset finish (keep reset DSP and HOST)
	XBYTE[0xB002]=0x02;	// Release DSP Reset, Jimi 080729 for pop noise as 1st time play
	
	XBYTE[0xB010]=0x00;	// Disable DSP & DSP memory controller clock
	XBYTE[0xB011]=0xE6;	// Enable DMA, FM, RTC USB, Host clock
	
	XBYTE[0xB08A]|=0x08; 	//(JC)MCU:audio DAC on
	XBYTE[0xB08A]|=0x10; 	//(JC)MCU:Headphone driver on
	XBYTE[0xB08A]|=0x20; 	//(JC)MCU:Headphone driver DD comm-mode bias o/p
	XBYTE[0xB0ED]|=0x02;	//(Jimi 091027)Audio DA zero cross reset bypass on
	XBYTE[0xB0C1]=0xB0;		//(Jimi 091027)Pre-mux , post-mux and internal XCK select
	XBYTE[0xB0C4]=2;		//(Jimi 091027)LCH speaker volume(i.e LCH OP Gain)
	XBYTE[0xB0C5]=2;		//(Jimi 091027)RCH speaker volume(i.e RCH OP Gain)
	
	XBYTE[0xB05E]=0; 		// disable SAR FS clock
	XBYTE[0xB05F]=0x5F;		// SAR clock = 12MHz/16/6 = 125kHz = 8us
	XBYTE[0xB09F]|=0x06;	//(JC)Vref Fast setup mode 'n Internal reference voltage power control
	
	CKCON=0x10;//(JC)H0229
	TMOD=0x21;
	TCON=0x05;
	TH1=243;	// UART baud rate = 14400bps at CPU 12MHz
	SCON=0xC8;  // if UART, set value=0xD8
	PCON=0x80;	
	
	TR1=1;		// TR1:  timer 1 run
	TI=1;		// TI:   set TI to send first char of UART
	ET0=1;
	TR0=1;
	
	P1=0xFF;	// P1 output high to turn off transistor and to avoid input grounded.
	XBYTE[0xB102]=0x00;
	P3=0xFF;	// P3 output high to turn off transistor and to avoid input grounded.
	XBYTE[0xB103]=0x02;	// set P3.1 UART Tx pin output enabled.

	XBYTE[0xb400] =0x00;
	XBYTE[0xb401] |= 0x04;//fmgpio2 input//for linein det
	XBYTE[0xb405] &= ~0x04;//fmgpio2 input//for linein det
 
}


void Variables_Init(void)
{
	gb_DirPlay_Flag=1;
	gb_Play_firstPlay=1;
	gc_HostSectorUnit=1;
	gw_FM_frequency=875;
	gc_FM_Vol=0x0F;
	gc_CurrentHZK=HZK_LANGUAGE;
	TH0=gc_T0IntervalMSB;
	TL0=gc_T0IntervalLSB;
	gb_Host_Exist=1;
	gb_SD_Exist=1;
}


void SourceDevicePreSet(void)
{
	Get_LogData_PageIndex();
	USER_LogFile_ReadWrite(0);//use reserveblock for log
	USER_LogFile_ReadWrite(1);
	if(gc_PlayRecordDataBuf[508]!='L')
	{
		gc_FM_CHAmount=0;
		gb_DirPlay_Flag=0;
		gs_DSP_GLOBAL_RAM.sc_Volume=20;
		USER_LogFile_ReadWrite(1);
		gc_LastCard=5;      
	}
	
	if((gc_LastCard!=5)&&(gc_LastCard!=2)&&(gc_LastCard!=9))  //LastCard 2:SD 5:Host 9:FM
	{           
		gc_LastCard=5;
	}

	//-----HOST--> SD-->FM
	switch(gc_LastCard)
	{
		case 0x05:  //HOST--SD--FM
			if(HOST_Check())
			{
				if(SD_Check())
				{
					gc_CurrentCard=9;
				}
			}
		break;

		case 0x02:  //SD
			if(SD_Check())
			{
				if(HOST_Check())
				{
					gc_CurrentCard=9;
				}
			}
		break;

		case 0x09:  //FM
			gc_CurrentCard=9;
        break;
	}

	gs_System_State.c_Phase=TASK_PHASE_STOP;
	if((gc_CurrentCard==9)/*||(gc_SW_RESET==0x36)*/)
	{
		gc_Task_Current=C_Task_Idle;
		return;
	}

	LCM_Clear_L0_L7();
	LCM_ShowWait();
	Play_SDFlash_DosInit();
	gc_Task_Current=C_Task_MusicPlay;
}



