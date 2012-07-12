#include <intrins.h>
#include "..\Header\SPDA2K.h"
#include "..\header\variables.h"


void Suspend(void)
{
	// P1/P3 Setting by circuit design
	XBYTE[0xB102]=0x21;
	XBYTE[0xB103]=0x00;
	P1=0xDE;
	P3=0xFF;
	gb_LCM_ForceShow=1;
	LCM_Clear_L0_L7();
//	dbprintf("Suspend\n");
	LCM_ShowChar16x32(10,78);
	LCM_ShowChar16x32(10,33);
	LCM_ShowFMOrderRecordIcon();
	LCM_ShowAlarmIcon();
	gc_SW_RESET=0x36;
	gb_SuspendKeyRelease=0;
suspend_again:
	LCM_ShowClock();
	set_clock_mode(CLOCK_MODE_XTL);
	XBYTE[0xB006]&=~0x20;	// turn off system PLL
	XBYTE[0xB09F]=0x02;
	XBYTE[0xB08A]&=~0x30;	// Turn off headphone driver
	XBYTE[0xB08A]&=~0x08;	// Turn off DAC
//**************************************************************

	XBYTE[0xB0AB]=0x00;	// enable cpuadrl pull

	// Disable GPIO[0..13] pull up/down
	XBYTE[0xB0A1]=0x00;
	XBYTE[0xB0A2]=0x00;
	XBYTE[0xB0A3]=0x00;
	XBYTE[0xB0A4]=0x00;
	XBYTE[0xB0A5]=0x00;//0x0a;
	XBYTE[0xB0A6]=0x00;//0xf0;	// set fmgpio pull
	XBYTE[0xB0A7]=0x00;//0x0f;	// set fmgpio pull

	// GPIO Setting by circuit design
	XBYTE[0xB030]=0xFF;	// GPIO[0~7] output 1
	XBYTE[0xB031]=0xFF;	// GPIO[8~13] output 1
	XBYTE[0xB038]=0x00;	// Enable GPIO[0~7] output
	XBYTE[0xB039]=0x00;	// Enable GPIO[8~13] output

	XBYTE[0xB400]=0;
	XBYTE[0xB405]=0x00;
	XBYTE[0xB406]=0x00;
	
	XBYTE[0xB059]=0x01;	// P30 falling interrupt
	XBYTE[0xB079]=0x00;	// clear falling event P3
	XBYTE[0xB0D1]=0x00; // clear rising & falling usb_det 
	XBYTE[0xB0C0]=0x00;	// clear all interrupt
	XBYTE[0xB0D0]=0x49; // Enable IR/RTC interrupt

	XBYTE[0xB005]=0xFF;	// XTAL stable time
	XBYTE[0xB010]=0x00;	// DSP/DSP-memory clock disable
	XBYTE[0xB011]=0x44;
	XBYTE[0xB016]=0x00;
	XBYTE[0xB019]=0x00;
	XBYTE[0xB060]=0x02;
	XBYTE[0xB08A]=0xC1;	// Turn off PLLA and DAC
	XBYTE[0xB0F7]=0x20;	// Turn-off All RAM module but Boot SRAM
	XBYTE[0xB092]=0x50; // 2.9V & 1.8V
	gb_IR_wakeup=0;
//dbprintf("Z z z ...\n");
	XBYTE[0xB013]=0x83;	// turn off XTAL
	_nop_();
	XBYTE[0xB013]=0x04;	// hold XTAL
//dbprintf("Wakeup...");
	XBYTE[0xB0D1]=0xF0;
	XBYTE[0xB0F7]=0xF8;
	XBYTE[0xB005]=0x00;
	XBYTE[0xB010]=0x04;	// DSP/DSP-memory clock disable
	XBYTE[0xB011]=0xE6;
	XBYTE[0xB016]=0x04;
	XBYTE[0xB019]=0x01;
	XBYTE[0xB060]=0x01;

	XBYTE[0xB09F]=0x06;
	XBYTE[0xB0C0]=0x00;		// clear all interrupt
	XBYTE[0xB0A5]|=0x08;	// set fmgpio pull

	if(gb_RTC_wakeup==1) // check if RTC alarm interrupt occurred
	{
		gb_RTC_wakeup=0;
		if(gb_OrderRecordTrigger==1)
		{
			gc_Task_Current=C_Task_FM;
			gb_Suspend_OrderRecord=1;
			goto WakeUp_By_OrderRecord;
		}
		
		if(gb_AlarmTrigger==1)
		{
			if(CheckAlarmSetting()==0)	// Match Alarm Clock Setting
			{
				gc_Task_Last=C_Task_Suspend;
				gc_Task_Current=C_Task_AlarmON;
				goto Wakeup_By_AlarmClock;
			}
		}

		if(gb_SuspendKeyRelease==0)
		{
			if(READ_SARADC(0)>0x78)	// [Play]Released(When Release, Key value about 0x7F)
			{
				gb_SuspendKeyRelease=1;
			}
		}
		else
		{
			if(READ_SARADC(0)<0x78)	// [Play]Pressed
			{
				if(READ_SARADC(0)<5)	// Double Check, Avoid SAR Unstable
				{
					goto Wakeup_By_PlayKey;
				}
			}
		}
		goto suspend_again;
	}
	XBYTE[0xB0D0]=0x41;
	XBYTE[0xB059]=0x00;		// P30 falling interrupt
	XBYTE[0xB079]=0x00;		// clear falling event P3

	USER_Delay(300);		// Wait IR receive all data
	if(gb_IR_wakeup==1)
	{ 		
		gb_IR_wakeup=0;
		if(XBYTE[0xBA1A]==0x00 && XBYTE[0xBA1B]==0xFF && XBYTE[0xBA1C]==0x46)
		{ // Wake by IR Power-on key	
			
		}
		else
		{
			goto suspend_again;
		}
	} 
	else
	{
//		dbprintf("-----some err ----\n");
		XBYTE[0xBA19]=0x07;
		goto suspend_again;
	}

Wakeup_By_PlayKey:
WakeUp_By_OrderRecord:
Wakeup_By_AlarmClock:
	XBYTE[0xB08A]=0xF8;	// Turn On PLLA and DAC
	IE=0;
	XBYTE[0xB092]=0x77; // 3.2V & 1.9V
	XBYTE[0xB0D0]=0x00;
	gc_clock_mode=0;
	load_nand_bank();
}