#include "..\header\SPDA2K.h"
#include "..\header\host_init.h"
#include "..\header\UI_config.h"
#include "..\header\variables.h"
#include "..\LCM\LCM_BMP.h"

extern void LineIn_Task(void);//

data bit gbt_USB_Detected;           
void USB_PlugDetect(void){//(JC)H0613
	U8 tc_i;
	U8 tc_usb_detect;

	GLOBAL_REG[0xD1] = 0xf0;							//(JC)from debounce ckt
	tc_i =0x08;
	while(tc_i)
	{
		tc_usb_detect = GLOBAL_REG[0xD1]&0x02;
		tc_i--;
	}
	if(tc_usb_detect){
	    gbt_USB_Detected = 1;     						// USB_Detected
	}
	else{
	    gbt_USB_Detected = 0;
	}
}


void main(void)
{
	InitCPURAM();
SYSTEM_ON:
	InitSystemRAM();
	Variables_Init();
	Sysetm_Init();
    SPI_Init();
	RTC_Init();
//	if(gc_SW_RESET!=0x36)
	{
    	LCM_Init();
		USB_Task();
	}
//	else
	{
		LCM_BACKLIGHT=0;
	}
	IE=0x83;
	IR_Init();

	if(gb_AlarmTrigger==1)
	{
		gc_Task_Current=C_Task_AlarmON;
		gc_LCMBacklightTimer=0;
		goto For_AlarmClock;
	}
	SourceDevicePreSet();
	gc_LCMBacklightTimer=gc_LCMBacklightTime;
	while(gb_RTC_wakeup==0);//蜆逄曆峈脹渾奀潔載陔.
    gc_LastCard=gc_CurrentCard;
	if(gb_OrderRecordTrigger==1)
	{
		gc_Task_Current=C_Task_FM;
	}

	if(gb_SuspendKeyRelease==1)	// Wakeup By [Press], Wait Key Release
	{
		while(READ_SARADC(0)<240)
		{
//			dbprintf("%bx ",gc_key_Pressed);
		}
	}
For_AlarmClock:
	gc_key_Pressed=0;
	gc_KeyValue=0;
	gc_IRCmdStatus=0;

	while(1)
	{
		dbprintf("gc_Task_Current=%bx\n",gc_Task_Current);
		gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB_NULL;
		switch(gc_Task_Current)
		{

			case C_Task_Idle:		// 顯示時間 HH:MM:SS
				Idle_Task();
			break;

			case C_Task_Suspend:	// 省電模式
				Suspend();
				EA=0;
				goto SYSTEM_ON;
			break;  

			case C_Task_FuncOption:	// 音樂/錄音播放/FM/日曆/設定
				MainMenu();
			break;

			case C_Task_MusicPlay:	// 音樂
				Play_Task();
			break;

			case C_Task_PlayWAV:	// FM錄音播放
				gc_RepPlayMode_Bak=gc_RepPlayMode;
				gc_RepPlayMode=0;	// Always Play-All
				WAV_Play();
				gc_RepPlayMode=gc_RepPlayMode_Bak;
			break;

			case C_Task_FM:  		// FM
				FM_Task();
			break;                  
	
			case C_Task_Calendar:	// Calendar
				DisplayCalendar();
			break;

			case C_Task_Setup:		// Setup menu
				SetupMenu();
			break;

			case C_Task_SetClock:	// Set Clock
				SetClock();
			break;

			case C_Task_SetBacklight:	// Set Backlight timer
				SetBacklight();
			break;

			case C_Task_SetLanguage:	// Select Language
				SetLanguage();
			break;

			case C_Task_SetAlarm:		// Set Alarm Clock
				SetAlarmClock();
			break;

			case C_Task_SetOFFTimer:	// Set OFF timer
				SetOFFTimer();
			break;

			case C_Task_AlarmON:		// Issue Alarm Sound
				AlarmMusic();
			break;
		
			case C_Task_LineIn:		// Issue Alarm Sound
				LineIn_Task();
			break;
        }
    }
}


