#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\IR\LCM_BMP.h"

extern xdata UBYTE gc_PhaseInx;
extern void Timer0_init(void);

extern bit gb_FindFlag;

extern data	U8	gc_LCM_Media;
extern data	U8	gc_DisplayEQIcon;
extern xdata	U16	gw_DisplayBitRate;
extern data	U8	gc_DisplayPlayMode;
extern xdata	U16	gw_DispSongNum1;

extern bit gb_cmd_timeout;
extern UBYTE gc_PseudoCard_Flag;


void USB_Task()
{
	while(1)
	{
		USB_On();//(JC)loop to wait 'n parse msdc scsi command, ret if usb unplugged (gbt_USB_Detected)

		if((!M_ChkFlag_USB) || (gb_cmd_timeout==1))  //080925chiayen modify
		{
			gbt_USB_Detected=0;
			GLOBAL_REG[0x10]&= 0xFE;	// Disable DSP clock, Jimi 080729 for pop noise as 1st time play
			GLOBAL_REG[0x02] &= 0xFE;	// Release DSP Reset, Jimi 080729 for pop noise as 1st time play

			Timer0_init();
			EA  = 1;

			gc_LCM_Media=0xff;
			gc_DisplayEQIcon=0xFF;
			gw_DisplayBitRate=0xFFFF;
			gc_DisplayPlayMode=0xFF;
			gw_DispSongNum1=0xFF;
			gc_PseudoCard_Flag=0;
			gc_Task_Next=C_Task_Play;
			gw_init_needed |= (SET_BIT0 | SET_BIT7);
			gw_FileIndex[0]=0;
			gc_PhaseInx = 0;								
			gb_FindFlag = 0;	
		}
		//Polling_TaskEvents();
//		if(gc_Task_Current!=gc_Task_Next)
		{
			gc_Task_Current=gc_Task_Next;
			break;	
		}
	}
}
