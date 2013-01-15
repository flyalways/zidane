#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "../IR/lcm_bmp.h"
#include "../i2c/tca8418_keypad.h"

extern data U8 gc_Dirchange_Timer;  //20090216 chiayen add
extern data	U16	gw_IR_Timer;
extern data System_Struct gs_System_State;

extern xdata UBYTE gc_LongKeyDelay;
extern xdata UBYTE gc_KeyEvent;
extern xdata UBYTE gc_PhaseInx;

extern UBYTE code IdleTask_PhaseTab[];
extern UBYTE code PlayTask_PhaseTab[];
extern UBYTE code RecTask_PhaseTab[];
extern UBYTE code VoiceTask_PhaseTab[];
extern UBYTE code JpegTask_PhaseTab[];
extern UBYTE code MjpegTask_PhaseTab[];
extern UBYTE code FMRTask_PhaseTab[];
extern UBYTE code MenuTask_PhaseTab[];
extern	void MediaChange(void);
//extern xdata WORD gw_SearchAmount; //Ching marked 080805
extern void SwitchClass(U8 tClass);

/*
extern void Music_WakeUp(U8 tc_Data);
extern void Music_VolumeUpCmd(void);
extern void Music_EQ_Cmd(U8 tc_Type);
*/
#if 0  //20081003 mark
void ProcKey_in_idle(void)
{
	//UBYTE t;	

	//KEY_Filter
	if((gc_LongKeyDelay < (MAX_REPEATKEYDELAY - 3)) &&((gc_KeyEvent == C_LKey_Play)||(gc_KeyEvent == C_LKey_Mode)))
	{//clear chain key
		gc_KeyEvent = 0;
	}
	gc_PhaseInx=MenuTask_PhaseTab[gc_KeyEvent];
}
#endif

void ProcKey_in_play(void)
{
     U8 x_gc_temp;

    // When the screen is not lit, don't act.
	if(LCM_BACKLIGHT==0)
	{
		gc_KeyEvent=0;
		return;	
	}	
	if(gw_IR_Timer)  
	{
		gc_KeyEvent=0;
		return;	
	}

    // If we've got a key to switch the file category:

    #if 1 // Only for test
    if ( (gc_KeyEvent == C_Key_Rec) )
    {
        SwitchClass(1); // comic;
        return;
    }
    if ( (gc_KeyEvent == C_Key_Vol) )
    {
        SwitchClass(2); // music;
        return;
    }
    if ( (gc_KeyEvent == C_LKey_Rec) )
    {
        SwitchClass(3); // song;
        return;
    }
    if ( (gc_KeyEvent == C_LKey_Vol) )
    {
        SwitchClass(4);
        return;
    }
    #endif // Only for test

    if ( (gc_KeyEvent == KEY_VALUE_OPERA)||(gc_KeyEvent==KEY_VALUE_MUSIC)
         ||(gc_KeyEvent == KEY_VALUE_FICTION)||(gc_KeyEvent == KEY_VALUE_COMIC)
         ||(gc_KeyEvent == KEY_VALUE_SONG)||(gc_KeyEvent == KEY_VALUE_STORY)
         ||(gc_KeyEvent == KEY_VALUE_LECTURE)||(gc_KeyEvent == KEY_VALUE_MISC) )
    {
        SwitchClass(gc_KeyEvent);
        gc_Dirchange_Timer=0;
   	    gc_KeyEvent=0;
        return;
    }

    // When the system is at pause, act differently depanding on ket event:
	if( (gs_System_State.c_Phase == TASK_PHASE_PAUSE)
        && ( (gc_KeyEvent!=C_Key_Play)&&(gc_KeyEvent!=0x15)&&(gc_KeyEvent!=0x12) )
        && ( (gc_Task_Current!=C_Task_Setting) ) )  
	{
		gc_KeyEvent=0;
		return;
	}
	if( (gs_System_State.c_Phase == TASK_PHASE_PAUSE)
        && (gc_KeyEvent==C_LKey_Mode) )
	{
         MediaChange(); 
         return; 
    } 
	
    // When the system is at stop:
	if(gs_System_State.c_Phase==TASK_PHASE_STOP)  
	{
		if(gc_KeyEvent==0x23 || gc_KeyEvent==0x24 || gc_KeyEvent==0x13 || gc_KeyEvent==0x14)
		{
			return;	
		}
		if(gc_KeyEvent==0x25 || gc_KeyEvent==0x26 || gc_KeyEvent==0x22)
		{
			gc_KeyEvent=0x20;
		}
	}
    // 2012.09.23 William: below statement will cause system crashing down.
    // The reason should be stack overflow (the string is too long).
    //dbprintf ("[%-10s] line %-4d: gc_KeyEvent = %x\n", __FILE__, __LINE__, gc_KeyEvent);

	x_gc_temp= PlayTask_PhaseTab[gc_KeyEvent];
    if( x_gc_temp != C_PlayIdle)
    {
       gc_PhaseInx = x_gc_temp;
    }
    // 2012.09.23 William: below statement will cause system crashing down.
    // The reason should be stack overflow (the string is too long).
    //dbprintf ("[%-10s] line %-4d: gc_PhaseInx = %x\n", __FILE__, __LINE__, gc_PhaseInx);
	
	if((gc_PhaseInx==C_PlayNext)||(gc_PhaseInx==C_PlayPrev))
	{
		gw_FileSkipNumber = 0;
	}

	gc_Dirchange_Timer=0;  //20090216 chiayen add
   	gc_KeyEvent=0;
}

#if 0  //20081003 mark
void ProcKey_in_rec(void)
{
	gc_PhaseInx=RecTask_PhaseTab[gc_KeyEvent];
	gc_KeyEvent=0;
}


void ProcKey_in_voice(void)
{
	gc_PhaseInx=VoiceTask_PhaseTab[gc_KeyEvent];
	gc_KeyEvent=0;
}


void ProcKey_in_jpeg(void)
{
	gc_PhaseInx=JpegTask_PhaseTab[gc_KeyEvent];
	gc_KeyEvent=0;
}


void ProcKey_in_mjpeg(void)
{
	gc_PhaseInx=MjpegTask_PhaseTab[gc_KeyEvent];
	if((gc_PhaseInx==C_MjpegNext)||(gc_PhaseInx==C_MjpegPrev))
	{
		gw_FileSkipNumber = 0;
	}
	gc_KeyEvent=0;
}


void ProcKey_in_FMRecv(void)
{
	gc_PhaseInx=FMRTask_PhaseTab[gc_KeyEvent];
	gc_KeyEvent=0;
}
#endif
