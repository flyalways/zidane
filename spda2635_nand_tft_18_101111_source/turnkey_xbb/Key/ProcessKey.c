#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\header\variables.h"

extern xdata System_Struct gs_System_State;

extern UBYTE code PlayTask_PhaseTab[];
extern UBYTE code JpegTask_PhaseTab[];
extern UBYTE code MjpegTask_PhaseTab[];
extern   void  MediaChange(void);

void ProcKey_in_play(void)
{
	if(gw_IR_Timer)  
	{
		gc_KeyEvent=0;
		return;	
	}

	if((gs_System_State.c_Phase == TASK_PHASE_PAUSE) && ((gc_KeyEvent!=0x02) && (gc_KeyEvent!=0x12))&& (gc_Task_Current!=C_Task_Setting)) //20090107 chiayen  modify  
	{
		gc_KeyEvent=0;
		return;
	}
	if((gs_System_State.c_Phase == TASK_PHASE_PAUSE) && (gc_KeyEvent==0x12))
	{
         MediaChange(); 
         return; 
    } 
	
	if(gs_System_State.c_Phase==TASK_PHASE_STOP)  
	{
		if(gc_KeyEvent==0x23 || gc_KeyEvent==0x24 || gc_KeyEvent==0x13 || gc_KeyEvent==0x14)
		{
			return;	
		}
		if(gc_KeyEvent==0x25 || gc_KeyEvent==0x26 || gc_KeyEvent==0x22)
		{
			gc_KeyEvent=0;
			return;
		}
	}
	gc_PhaseInx=PlayTask_PhaseTab[gc_KeyEvent];
	if((gc_PhaseInx==C_PlayNext)||(gc_PhaseInx==C_PlayPrev))
	{
		gw_FileSkipNumber = 0;
	}
	gc_Dirchange_Timer=0;  //20090216 chiayen add
   	gc_KeyEvent=0;
}


void ProcKey_in_jpeg(void)
{
	gc_PhaseInx=JpegTask_PhaseTab[gc_KeyEvent];
	gc_KeyEvent=0;
}


void ProcKey_in_mjpeg(void)
{
		
	if(gc_KeyEvent==0x22)
	{
		gc_KeyEvent=0;
		return;
	}
	gc_PhaseInx=MjpegTask_PhaseTab[gc_KeyEvent];
	if((gc_PhaseInx==C_MjpegNext)||(gc_PhaseInx==C_MjpegPrev))
	{
		gw_FileSkipNumber = 0;
	}
	gc_KeyEvent=0;
}


