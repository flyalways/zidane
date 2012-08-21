#include "..\Header\SPDA2K.h"
#include "..\header\variables.h"


void SetBacklight(void)
{
	data	bit	tb_Show=1;

	LCM_Clear_L0_L7();
	gc_MenuTimer=6;
	if(gc_LCMBacklightTime==0xFE)
	{
		gc_LCMBacklightTime=0;
	}

	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=6;
			if(gc_KeyEvent==4)	// Prev
			{
				if(gc_LCMBacklightTime!=0)
				{
					gc_LCMBacklightTime-=1;
					tb_Show=1;
				}
			}
			else if(gc_KeyEvent==3)	// Next
			{
				if(gc_LCMBacklightTime!=30)
				{
					gc_LCMBacklightTime+=1;
					tb_Show=1;
				}			
			}
			else if(gc_KeyEvent==6)	// M
			{
				if(gc_LCMBacklightTime==0)
				{
					gc_LCMBacklightTime=0xFE;
				}
				gc_LCMBacklightTimer=gc_LCMBacklightTime;
				USER_LogFile_ReadWrite(1);
				gc_Task_Current=C_Task_Setup;
				break;
			}
			else if(gc_KeyEvent==0x16)	// L-M(Abort)
			{
				gc_MenuTimer=0;
			}
		}

		if(tb_Show==1)
		{
			tb_Show=0;
			LCM_ShowBacklightTime();
		}

		if(gc_MenuTimer==0)
		{
			gc_Task_Current=C_Task_Idle;
			gc_LCM_line=0;
			gb_LCM_InverseDisp=0;
			break;
		}
	}
}
