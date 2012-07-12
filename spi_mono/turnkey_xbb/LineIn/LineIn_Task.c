#include "..\Header\SPDA2K.h"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"


void Linein_VolAdj_2()
{
	xdata	U8	tc_TmpData;
	
	if(gb_ADJ==0)
	{
		if(gs_DSP_GLOBAL_RAM.sc_Volume==0)
		{
			XBYTE[0xB0C6]=0;   //(Jimi 091030)Line in gain L maximum
			XBYTE[0xB0C7]=0;   //(Jimi 091030)Line in gain R maximum
		}

		if(gs_DSP_GLOBAL_RAM.sc_Volume<42)
		{
			gs_DSP_GLOBAL_RAM.sc_Volume+=2;
		}

		tc_TmpData=31-(gs_DSP_GLOBAL_RAM.sc_Volume/2);
		XBYTE[0xB0C4]=tc_TmpData;  //(Jimi 091030)Headphone gain Lch
		XBYTE[0xB0C5]=tc_TmpData;  //(Jimi 091030)Headphone gain Rch
		LCM_ShowVOL();
	}
	else
	{
		if(gs_DSP_GLOBAL_RAM.sc_Volume>1)
		{
			gs_DSP_GLOBAL_RAM.sc_Volume-=2;
		}

		tc_TmpData=31-(gs_DSP_GLOBAL_RAM.sc_Volume/2);
		XBYTE[0xB0C4]=tc_TmpData;  //(Jimi 091030)Headphone gain Lch
		XBYTE[0xB0C5]=tc_TmpData;  //(Jimi 091030)Headphone gain Rch
		if(gs_DSP_GLOBAL_RAM.sc_Volume==0)
		{
			XBYTE[0xB0C6]=31;
			XBYTE[0xB0C7]=31;
		}
		LCM_ShowVOL();
	}
}

void Linein_Out_2(void)
{
	DSP_Write_MMR16(0x3FC1,(DSP_Read_MMR16(0x3FC1)|0x0010));
    XBYTE[0xB0C2]=0;       //disable ADC LRch(bit[1:0]), Mic bias(bit[2]) and Mic boost(bit[5:4])
    XBYTE[0xB0C1]&=0xFC;   //set audio driver post-mux as DAC output
    XBYTE[0xB0C1]|=0x30;   //power down ADC input pre-mux
    XBYTE[0xB0ED]|=0x02;   //Audio DA zero cross reset bypass on
    XBYTE[0xB0C4]=0;       //LCH speaker volume(i.e LCH OP Gain)
    XBYTE[0xB0C5]=0;       //RCH speaker volume(i.e RCH OP Gain)
}

void Linein_Set_2(void)
{
    data	U8 tc_TmpData;
    
	XBYTE[0xB010]|=0x01;
	
	XBYTE[0xB0C4]=0x1F;          //(Jimi 091030)mute headphone Lch
	XBYTE[0xB0C5]=0x1F;          //(Jimi 091030)mute headphone Rch
	XBYTE[0xB0C2]|=0x03;         //(Jimi 091030)ADC enable
	DSP_Write_MMR16(0x3FC0,(DSP_Read_MMR16(0x3FC0)|0x0200));    //audio ADC clock enable
	
	XBYTE[0xB0C1]&=0xCC;       //(Jimi 091030)
	XBYTE[0xB0C1]|=0x11;       //(Jimi 091030)set pre-mux and post-mux as line-in
	XBYTE[0xB0ED]|=0x01;       //(Jimi 091030)disable ADC zero cross detect
	
	if(gb_linein_mute==0)
	{
		tc_TmpData=31-(gs_DSP_GLOBAL_RAM.sc_Volume/2);
		XBYTE[0xB0C4]=tc_TmpData;  //(Jimi 091030)Headphone gain Lch
		XBYTE[0xB0C5]=tc_TmpData;  //(Jimi 091030)Headphone gain Rch
		XBYTE[0xB0C6]=0;   //(Jimi 091030)Line in gain Lch maximum
		XBYTE[0xB0C7]=0;   //(Jimi 091030)Line in gain Rch maximum
	}
	else
	{
		XBYTE[0xB0C6]=31;    //(Jimi 091030)Line in gain Lch mute
		XBYTE[0xB0C7]=31;    //(Jimi 091030)Line in gain Rch mute
	}
}

void AccessKeyEvent_LineIn()
{
//	data	U8	tc_temp;
//	xdata	U8	i;

	switch(gc_KeyEvent)
	{
		case 0x02:	// Play
			if(gb_FM_Mute==1)
			{
				gb_FM_Mute=0;
				gb_linein_mute=1;
				Linein_Set();
			}else
			{
				gb_FM_Mute=1;
				gb_linein_mute=0;
				Linein_Set();
			}
		break;

		case 0x03:	// Next
			gb_ADJ=0;
			if(gb_SetVol)
			{
				Linein_VolAdj_2();	// Volume Adjust
			}			
		break;

		case 0x04:	// Prev
			gb_ADJ=1;
			if(gb_SetVol)
			{
				Linein_VolAdj_2();	// Volume Adjust
			}		
		break;

		case 0x05:	// V
			if(gb_SetVol==0)
			{
				gb_SetVol=1;
				gc_MenuTimer=6;
				LCM_Clear_L2_L5();
				LCM_ShowVOL();
				ClearIRNumberVariable();
			}			
		break;

		case 0x06:	// M			
		break;

		case 0x12:	// L-Play		
		break;

		case 0x13:	// L-Next
			if(gb_SetVol==1)
			{
				gb_ADJ=0;	// Volume Adjust
				Linein_VolAdj_2();
				gc_LongKeyCount=30;
			}		
		break;

		case 0x14:	// L-Prev
			if(gb_SetVol==1)
			{
				gb_ADJ=1;	// Volume Adjust
				Linein_VolAdj_2();
				gc_LongKeyCount=30;
			}
		break;

		case 0x15:	// L-V		
		break;

		case 0x16:	// L-M		
		break;
	}
}

void LineIn_Task(void)
{  
	set_clock_mode(CLOCK_MODE_MP3);
//	Music_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);
	MP3_Download();

	Linein_Set_2();
	LCM_Show_LineIn();
	while(1)
	{
 		Key_Detect();
		if(gc_KeyEvent!=0)
		{
			gc_MenuTimer=6;
			AccessKeyEvent_LineIn();
			gc_KeyEvent=0;
		}
		
		if((gc_MenuTimer==0)&&(gb_SetVol))
		{
			gb_SetVol=0;
			// Show LCM icon
			LCM_Show_LineIn();
		}

		if(XBYTE[0xb409]&0x04)//LineIn out 
		{
		dbprintf("linein out\n");
			gc_Task_Current=C_Task_MusicPlay;	
		}
		if(gc_Task_Current != C_Task_LineIn)
		{   			
			Linein_Out_2();
            break;	
		}
	}
}
