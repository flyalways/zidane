#include "SPDA2K.H"
#include "..\header\variables.h"
#include "../ui/ui.h"
#include "../ui/ui_impl.h"
#include <stdio.h>

extern xdata	U8	gc_Err;
extern xdata U8 gc_Media_type;
extern data bit gb_PickSongSet;
extern data bit gb_PickSong;
extern data bit gb_ChannelSet; 
extern xdata U8 gc_NextorPreFlag;
extern xdata U8 gc_SelectEQ;
extern xdata U8 gc_SelectVol;
extern data bit gb_Frequency_Song;
extern data	U8	gc_LCM_Media;
extern data	U8	gc_DisplayEQIcon;
extern data	U8	gc_DisplayPlayMode;
extern data	U8	gc_DisplayEQ;
extern data	U8	gw_DisplayVol;
extern data	U8	gc_DisplaySongnameCount;
extern data	U8	gc_LCM_line;
extern xdata	U16	gw_DisplayBitrateTime;
extern xdata	U16	gw_DisplayBitRate;
extern xdata	U16	gw_DisplayTime;
extern xdata	U16	gw_DisplayFreq;
extern xdata	U16	gw_DispSongNum;
extern xdata	U16	gw_DispSongNum1;
extern data U16 gw_LogData_Timer;
extern data	U16	gw_LCMScrollTimer;
extern data U8 gc_ShowTimer;
extern data	U16 gw_SongNumber;
extern data U16 gw_FM_frequency;
extern void LCM_ShowPlayVol();
extern data System_Struct gs_System_State;
extern data	U16	gw_DisplayPinPuTime;

extern xdata U8 gc_irkey;

extern xdata UBYTE gc_RepPlayMode;
extern xdata UBYTE  gcPlay_FileType;
extern xdata WORD gw_Disp_CurrentSec;
extern xdata U16 gw_TotalSec ;
extern xdata U8 gc_FolderPlay_Flag;  //20090216 chiayen add

//void LCM_ShowChar8x16_1(U8 Num,U8 tc_column);
void LCM_ShowChar6x8_1(U8 Num,U8 tc_column);
void SongNum_char(U8 Num,U8 tc_column);
void SongTimeNum(U8 Num,U8 tc_column);
void LCM_ShowPlaySongNum(void);
extern data bit gb_SDNoFileflag;
bit	gb_FlashNoFileflag=0; 
void LCM_ShowFreqNum(U8 Num,U8 tc_column);		
void LCM_ShowPinpu_Icon();
xdata U8 gc_LrcExit_Pre=0;
extern xdata U8 gc_IRNum_Count;

code U8 CharIcon8x16_1[]=
{	// 8x16
    0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x0F,0x10,0x20,0x20,0x10,0x0F,0x00,//0  0
    0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00,//1  1
    0x00,0x70,0x08,0x08,0x08,0x88,0x70,0x00,0x00,0x30,0x28,0x24,0x22,0x21,0x30,0x00,//2  2
    0x00,0x30,0x08,0x88,0x88,0x48,0x30,0x00,0x00,0x18,0x20,0x20,0x20,0x11,0x0E,0x00,//3  3
    0x00,0x00,0xC0,0x20,0x10,0xF8,0x00,0x00,0x00,0x07,0x04,0x24,0x24,0x3F,0x24,0x00,//4  4
    0x00,0xF8,0x08,0x88,0x88,0x08,0x08,0x00,0x00,0x19,0x21,0x20,0x20,0x11,0x0E,0x00,//5  5
    0x00,0xE0,0x10,0x88,0x88,0x18,0x00,0x00,0x00,0x0F,0x11,0x20,0x20,0x11,0x0E,0x00,//6  6
    0x00,0x38,0x08,0x08,0xC8,0x38,0x08,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00,//7  7
    0x00,0x70,0x88,0x08,0x08,0x88,0x70,0x00,0x00,0x1C,0x22,0x21,0x21,0x22,0x1C,0x00,//8  8
    0x00,0xE0,0x10,0x08,0x08,0x10,0xE0,0x00,0x00,0x00,0x31,0x22,0x22,0x11,0x0F,0x00,//9  9
    0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,//10 :
};

U8	code  Bit_CharIcon6x8_1[]=
{
	//just for solang:  显示比特率的数字。
    0x39,0x65,0x55,0x4D,0x39,0x01,//0
	0x01,0x49,0x7D,0x41,0x01,0x01,//1
	0x49,0x45,0x65,0x55,0x49,0x01,//2
	0x45,0x55,0x55,0x55,0x6D,0x01,//3
	0x21,0x31,0x29,0x7D,0x21,0x01,//4
	0x5D,0x55,0x55,0x55,0x25,0x01,//5
	0x39,0x55,0x55,0x55,0x21,0x01,//6
	0x05,0x05,0x75,0x0D,0x05,0x01,//7
	0x29,0x55,0x55,0x55,0x29,0x01,//8
	0x09,0x55,0x55,0x55,0x39,0x01,//9
};
U8  code SongNumTime_char[]=   //SongNum_char
{
	//just for solang:  显示当前歌曲数字。时间
	0x3E,0x51,0x49,0x45,0x3E,//0
	0x00,0x42,0x7F,0x40,0x00,//1
	0x42,0x61,0x51,0x49,0x46,//2
	0x21,0x41,0x45,0x4B,0x31,//3
	0x18,0x14,0x12,0x7F,0x10,//4
	0x27,0x45,0x45,0x45,0x39,//5
	0x3C,0x4A,0x49,0x49,0x30,//6
	0x01,0x71,0x09,0x05,0x03,//7
	0x36,0x49,0x49,0x49,0x36,//8
	0x06,0x49,0x49,0x29,0x1E,//9
	0x00,0x36,0x36,0x00,0x00,//:
};
U8	code FreqNum14x32[] =			//
{
	//just for solang: 显示频率数字。
	0x00,0x80,0x40,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x40,0x80,0x00,0x00,0xFF,
	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFB,0xF1,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF1,0xFB,0x00,0x00,0x7F,0xBF,0xC0,0xC0,0xC0,
	0xC0,0xC0,0xC0,0xC0,0xC0,0xBF,0x7F,0x00,												//0
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF1,0xFB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x3F,0x7F,0x00,												//1
	0x00,0x00,0x40,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x40,0x80,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xF0,0xE8,0x0C,
	0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0B,0x07,0x00,0x00,0x7F,0xBF,0xC0,0xC0,0xC0,
	0xC0,0xC0,0xC0,0xC0,0xC0,0x80,0x00,0x00,												//2
	0x00,0x00,0x40,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x40,0x80,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x04,0x06,
	0x06,0x06,0x06,0x06,0x06,0x06,0x06,0xF5,0xFB,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,
	0xC0,0xC0,0xC0,0xC0,0xC0,0xBF,0x7F,0x00,												//3
	0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0xFF,
	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x07,0x0B,0x0C,
	0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xEB,0xF7,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x3F,0x7F,0x00,												//4
	0x00,0x80,0x40,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x40,0x00,0x00,0x00,0xFF,
	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x0B,0x0C,
	0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xE8,0xF0,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,
	0xC0,0xC0,0xC0,0xC0,0xC0,0xBF,0x7F,0x00,												//5
	0x00,0x80,0x40,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x40,0x00,0x00,0x00,0xFF,
	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF7,0xEB,0x1C,
	0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0xE8,0xF0,0x00,0x00,0x7F,0xBF,0xC0,0xC0,0xC0,
	0xC0,0xC0,0xC0,0xC0,0xC0,0xBF,0x7F,0x00,												//6		
	0x00,0x40,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x40,0x80,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF1,0xFB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x3F,0x7F,0x00,												//7
	0x00,0x80,0x40,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x40,0x80,0x00,0x00,0xFF,
	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0xFB,0xF5,0x0C,
	0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xF5,0xFB,0x00,0x00,0x7F,0xBF,0xC0,0xC0,0xC0,
	0xC0,0xC0,0xC0,0xC0,0xC0,0xBF,0x7F,0x00,												//8
	0x00,0x80,0x40,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0x40,0x80,0x00,0x00,0xFF,
	0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x07,0x0B,0x0C,
	0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0xFB,0xF7,0x00,0x00,0x00,0x80,0xC0,0xC0,0xC0,
	0xC0,0xC0,0xC0,0xC0,0xC0,0xBF,0x7F,0x00,												//9     14x32
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,												//space 空白
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0xF0,
	0xF0,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,												//point			
};
U8	code BitRateIcon128x64[]=	// only for 128x64 LCD
{
	// 21x16
	0xFC,0xF6,0x06,0xFE,0x3E,0xBE,0xF6,0x06,0xBE,0xBE,0x7E,0xBE,0x3E,0xBE,0xBE,0x7E,0x7E,0xBE,0xBE,0xBE,0xFC,
	0x3F,0x7B,0x78,0x7E,0x79,0x7B,0x7F,0x7C,0x7B,0x7B,0x7C,0x6F,0x60,0x6B,0x7B,0x7C,0x7B,0x7A,0x7A,0x7D,0x3F
};

extern U8 xdata gc_xs;
//-----------------------------------------------------------------------------
// Entry for LCD display routine.
//
// Considering the poor performance of platform, the principle here is:
//      try only to show things which is renewed. 
// So we will use some flags to mark which things are renewed just now.
//-----------------------------------------------------------------------------
void LCM_Display(void)
{   
	U16	tw_DisplayTime;

    // REVISIT!!!
    // If we use this flag, we probably need to control this flag at many places
    // not only here where the system status changes.
    static bool music_basic_done = FALSE;
    static uint8 task_phase_previous;
    static bool task_phase_changed = FALSE;
    
    // The system is at play status. Show the music infrastructure menu.
    // But only once.
    if (gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
    {
        if (music_basic_done == FALSE)
        {
            ui_show_music_basic();
            ui_show_song_num (gw_FileIndex[0], gw_FileTotalNumber[0]);
            music_basic_done = TRUE;
        }

        // REVISIT!!!
        // If the task status does not change, this will be a waste. The better
        // way is to record the previous status and check if the status is
        // changed. The tracking of task phase should not be here but I do it.
        if (task_phase_previous != TASK_PHASE_PLAYACT)
        {
            task_phase_changed = TRUE;
        }
        if (task_phase_changed)
        {
            ui_show_task_phase (gs_System_State.c_Phase);
            task_phase_changed = FALSE;
        }
        task_phase_previous = TASK_PHASE_PLAYACT;
    }

	if(((gcPlay_FileType==1)&&(gs_System_State.c_Phase == TASK_PHASE_PLAYACT))&&(((gb_PickSong==1)||(gb_ChannelSet==1)) || gc_CurrentCard!=5))
	{
		if(gs_File_FCB[0].dw_File_TotalSize==0) 
	    { 
            gc_PhaseInx=5; 
            return; 
	    } 
        WMA_DataIn(); 

	}
	 if(gc_xs)
     {
       gc_xs=0;
	   return;
	 }

/*	
	if(gc_FolderPlay_Flag==1)  //20090216 chiayen add
	{
		LCM_clear_dir();
		LCM_ShowFrequency();
		if(gc_CurrentCard==5)
		{
			LCM_ShowUSBIcon();
		}
		else if(gc_CurrentCard==2)//chiayen0812
		{
			LCM_ShowSDIcon();
		}
		else //chiayen0812
		{
			LCM_ShowNandIcon();
		}
		gc_FolderPlay_Flag=0;
	}
*/
	// Show "ERROR"
	if(gc_Err==1)
	{
		if(gc_LCM_Media!=9)
		{
			gc_LCM_Media=9;
			LCM_ShowERROR();
		}
		goto Show_SongTime;
	}

	// Show MP3/WMA icon
    #if (SERIAL_MONO != FEATURE_ON)
	if(gc_Media_type!=gcPlay_FileType)
	{
		gc_Media_type=gcPlay_FileType;
		LCM_ShowMediaType();
	}
	#endif
    	
	// Show EQ icon
    #if (SERIAL_MONO != FEATURE_ON)
	if(gc_DisplayEQIcon!=gs_DSP_GLOBAL_RAM.sc_EQ_Type)
	{
		gc_DisplayEQIcon=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
		LCM_ShowEQIcon();
	}
    #endif

	// Show bit rate
    #if (SERIAL_MONO != FEATURE_ON) 
	if(gw_DisplayBitrateTime==0)
	{
		gw_DisplayBitrateTime=20;
		//if(gw_DisplayBitRate!=gs_System_State.w_BitRate)
		{		
			gw_DisplayBitRate=gs_System_State.w_BitRate;
			LCM_ShowBitRate();
			//LCM_ShowBitRateIcon();
		}
	}
    #endif

    // Show song numbers.
	if(gw_DispSongNum!=gw_FileIndex[0])
	{
		gw_DispSongNum = gw_FileIndex[0];
		LCM_ShowPlaySongNum();
	}

    #if (SERIAL_MONO == FEATURE_OFF)
    if((gb_ChannelSet==1)||(gb_PickSong==1) || (gc_Num_Flag==1))
	{
		if((gc_LCM_Media!=10)&&(gb_ChannelSet==1))
		{
			gc_LCM_Media=10;
			LCM_ClearScreen();
		}
		if((gc_LCM_Media!=11)&&(gb_PickSong==1))
		{
			gc_LCM_Media=11;
			LCM_ClearScreen();
		}
        if((gc_LCM_Media!=12)&&(gc_Num_Flag==1))
		{
			gc_LCM_Media=12;
			LCM_ClearScreen();
		}
		LCM_ShowIRNum();
		goto Show_SongTime;
		return;
	}
    #endif

	if(1)//(gc_CurrentCard==2)||(gc_CurrentCard==3)||(gc_CurrentCard==5))	// Media insert
	{
		// Show "PAUSE"
		if(gs_System_State.c_Phase == TASK_PHASE_PAUSE)
		{			
			if(gc_LCM_Media!=3)	// 3=Pause
			{
				gc_LCM_Media=3;
				LCM_ShowPAUSE();
				gw_DisplayTime=0xFFFF;
				gw_DisplayFreq=0xFFFF;
			}

            if (task_phase_previous != TASK_PHASE_PAUSE)
            {
                task_phase_changed = TRUE;
            }
            if (task_phase_changed)
            {
                ui_show_task_phase (gs_System_State.c_Phase);
                task_phase_changed = FALSE;
            }
            task_phase_previous = TASK_PHASE_PAUSE;

			return;
		}

		// Show play mode icon
        #if (SERIAL_MONO != FEATURE_ON)
		if((gc_ShowTimer!=0)&&(gc_DisplayPlayMode!=gc_RepPlayMode))
		{
			if(gc_LCM_Media!=7)	// 7=Play mode
			{
				gc_LCM_Media=7;
				LCM_ClearScreen();
			}
			gc_DisplayPlayMode=gc_RepPlayMode;
			LCM_ClearScreen();
			LCM_ShowPlayMode();
			// Show play mode small icon
			LCM_ShowPlayModeIcon();
			goto Show_SongTime;
		}
        #endif

		// Show EQ
        #if (SERIAL_MONO != FEATURE_ON)
		if((gc_ShowTimer!=0)&&(gc_SelectEQ==1))
		{
			gc_LCM_Media=8;		// 8=EQ

			if(gc_DisplayEQ!=gs_DSP_GLOBAL_RAM.sc_EQ_Type)
			{
				gc_DisplayEQ=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
				LCM_ClearScreen();
				LCM_ShowEQ();
			}
			goto Show_SongTime;
		}
        #endif

		// Change Frequency
        #if (SERIAL_MONO == FEATURE_OFF)
		if((gb_Frequency_Song==0)&&(gc_ShowTimer!=0))
		{
			if(gw_DisplayFreq!=gw_FM_frequency)
			{
				if(gc_LCM_Media!=6)
				{
					gc_LCM_Media=6;	// 6=Change Frequency
					LCM_ClearScreen();
				}
				gw_DisplayFreq=gw_FM_frequency;
				LCM_ShowFrequency_L();
				gw_DisplayTime=0xFFFF;
			}
			goto Show_SongTime;
		}
        #endif
        		
		// Change Volumn
        #if (SERIAL_MONO == FEATURE_OFF)    // Original parallel LCM
		if((gc_SelectVol==1)&&(gc_ShowTimer!=0))
		{			
			if(gw_DisplayVol!=gs_DSP_GLOBAL_RAM.sw_Volume)
			{
				if(gc_LCM_Media!=4)
				{
					gc_LCM_Media=4;	// 4=Change VOL
					LCM_ClearScreen();
				}
				gw_DisplayVol=gs_DSP_GLOBAL_RAM.sw_Volume;
				LCM_ShowVOL();
				gw_DisplayTime=0xFFFF;
				gw_DisplayFreq=0xFFFF;
			}
			goto Show_SongTime;
		}
		else
		{
			gw_DisplayVol=0xFF;
		}
        #elif (SERIAL_MONO == FEATURE_ON)
        if(gw_DisplayVol!=gs_DSP_GLOBAL_RAM.sw_Volume)
        {
            gw_DisplayVol = gs_DSP_GLOBAL_RAM.sw_Volume;
            LCM_ShowVOL();    
        }
        #endif

		// Show Song Number
		if((gb_Frequency_Song==1)&&(gc_ShowTimer!=0))
		{
			if(gw_DisplayVol!=gs_DSP_GLOBAL_RAM.sw_Volume)
			{
				if(gw_DispSongNum!=gw_FileIndex[0])
				{
				//	gw_DispSongNum=gw_FileIndex[0];
					if(gc_LCM_Media!=5)
					{
						gc_LCM_Media=5;	// 5=Song Number
//						LCM_ClearScreen();
					}
//					LCM_ShowSongNumber();
					gw_DisplayTime=0xFFFF;
					gw_DisplayFreq=0xFFFF;
				}
			}
			goto Show_SongTime;
		}
		else
		{
			gw_DisplayVol=0xFF;
		}

		if(gc_CurrentCard==5)
		{
			if(gc_LCM_Media!=1)
			{
				gc_LCM_Media=1;	// 1=USB
//				LCM_ClearScreen();
//				LCM_ShowFMIcon();
				LCM_ShowUSBIcon();
				gw_DisplayTime=0xFFFF;
				gw_DisplayFreq=0xFFFF;
				gw_DisplayPinPuTime=0;
			}
		}
		else if(gc_CurrentCard==2)//chiayen0812
		{
			if(gc_LCM_Media!=2)
			{
				gc_LCM_Media=2;	// 2=SD/MMC
//				LCM_ClearScreen();
//				LCM_ShowFMIcon();
				LCM_ShowSDIcon();
				gw_DisplayTime=0xFFFF;
				gw_DisplayFreq=0xFFFF;
				gw_DisplayPinPuTime=0;
			}
		}
		else //chiayen0812
		{
			if(gc_LCM_Media!=20)
			{
				gc_LCM_Media=20;	// Nand Flash
//				LCM_ClearScreen();
				LCM_ShowNandIcon();
				gw_DisplayTime=0xFFFF;
				gw_DisplayFreq=0xFFFF;
				gw_DisplayPinPuTime=0;
			}
		}

		if(gc_DisplayPlayMode!=gc_RepPlayMode)
		{
			gc_DisplayPlayMode=gc_RepPlayMode;
			LCM_ShowPlayModeIcon();
		}
		
		// Display Song name
		if(gw_FileTotalNumber[0]==0)
		{			
			if((gc_CurrentCard==0)  && (gb_FlashNoFileflag==0))
			{
				LCM_Show_NOMP3FILE();
				gb_FlashNoFileflag=1;
			}
			gw_DisplayTime=tw_DisplayTime;

            return;
		}
		else
		{
			if(gw_DispSongNum1!=gw_FileIndex[0])
			{
				U8	i;

				if((gc_CurrentCard==2) && (SD_Detect!=0))  ////if SD not exist return DOS_CLUSTER_LINK_ERR chiayen0813
				{
					return;		
				}
			
				if((gc_CurrentCard==5) && (gb_HostConnect == 0x00))  ////if Host not exist return DOS_CLUSTER_LINK_ERR chiayen 20081017
				{
					return;		
				}

				for(i=0;i<48;i++)
				{
					gc_FileLongName[i]=0;
				}
				gw_DispSongNum1=gw_FileIndex[0];
				DOS_GetLongFileName(0,gc_FileLongName);
				gw_LogData_Timer=2;  //20090107 chiayen modify 
				LCM_ShowPlayTime(0);
				gw_DisplayTime=gw_Disp_CurrentSec;
				LCM_Disp_FileName(&gc_FileLongName[5],gc_FileLongName[2],gc_FileLongName[4],1);
				gw_LCMScrollTimer=40;
			}

            #if (SERIAL_MONO == FEATURE_OFF)
			if(gw_LCMScrollTimer==0)
			{
				if(!gb_LrcFileName_Exist)
				{
					RollFileName();
				}
				gw_LCMScrollTimer=40;
			}
            #endif					
		}
	}
	else
	{
		if(gc_LCM_Media!=0)
		{
			gc_LCM_Media=0;	// 0=No media
			//LCM_ShowLineIn();
			gw_DisplayTime=0xFFFF;
			gw_DisplayFreq=0xFFFF;
			gw_LCMScrollTimer=0;
			gc_DisplaySongnameCount=0;
			if(gc_DisplayPlayMode!=gc_RepPlayMode)
			{
				gc_DisplayPlayMode=gc_RepPlayMode;
				LCM_ShowPlayModeIcon();
			}
		}
	}

	// Display Frequency
    #if (SERIAL_MONO == FEATURE_ON)
	if(gw_DisplayFreq!=gw_FM_frequency && gb_LrcFileName_Exist==0)
	{
		gb_FlashNoFileflag=0;
		gw_DisplayFreq=gw_FM_frequency;
		LCM_ShowFrequency();									//for solang by home. homeing

	}
    #endif

/*	if(gw_DisplayVol!=gs_DSP_GLOBAL_RAM.sw_Volume)			//show vol icon for solang by home
	{
			gw_DisplayVol=gs_DSP_GLOBAL_RAM.sw_Volume;
			LCM_ShowPlayVol();
	}*/

	//===========sunzhk add pinpu
    #if (SERIAL_MONO != FEATURE_ON)
	if((gs_System_State.c_Phase==TASK_PHASE_PLAYACT)&&gw_DisplayPinPuTime==0 && gb_LrcFileName_Exist==0)
	{  //频谱刷新条件 播放中+timer到+无LRC
		gw_DisplayPinPuTime=18;
		LCM_ShowPinpu_Icon();
			
	}
    #endif

	//====end
Show_SongTime:
	if(((gcPlay_FileType==1)&&(gs_System_State.c_Phase == TASK_PHASE_PLAYACT))&&(((gb_PickSong==1)||(gb_ChannelSet==1)) || gc_CurrentCard!=5))
	{
		WMA_DataIn();
	}

	tw_DisplayTime=gw_Disp_CurrentSec;
	if(tw_DisplayTime!=gw_DisplayTime)
	{	
		LCM_ShowPlayTime(tw_DisplayTime);
		gw_DisplayTime=tw_DisplayTime;
	}

}

void LCM_erase_one_page(U8 page)
{
	U8 i;

	LCM_set_address(page, 0);
	for (i=132; i; i--)
	{
		LCM_write_data(0);
	}
}

void LCM_ClearPage(U8 Page,U8 Page1 )
{
		U8 i;
	for(i=Page;i<=Page1;i++)
    {
        LCM_erase_one_page(i);
	}
}
void LCM_ClearScreen(void)
{
    U8 i;
	for(i=2;i<8;i++)//sunzhk add for simo
//	for(i=2;i<6;i++)
    {
        LCM_erase_one_page(i);
	}
}
void LCM_ShowFreqNum(U8 Num,U8 tc_column)			//homeing
{
		U8	i,j,k;
		k = 4;
	for(i=0;i<4;i++)
	{
		LCM_set_address(i+k,tc_column);
		for(j=0;j<14;j++)
		{
			LCM_write_data(FreqNum14x32[Num*56+14*i+j]);
		}
	}
	

}


void LCM_ShowBitRate(void)
{
	//U8  j;
	gc_LCM_line=0;

	LCM_ShowChar6x8_1(gs_System_State.w_BitRate/100,26);
	LCM_ShowChar6x8_1(gs_System_State.w_BitRate%100/10,32);
	LCM_ShowChar6x8_1(gs_System_State.w_BitRate%10,38);
	gc_LCM_line=0;
}

void LCM_ShowPlaySongNum(void)					//just for solang by home.
{
    #if (SERIAL_MONO == FEATURE_OFF)    // Original parallel LCM

    U8	tc_column;

	gc_LCM_line=0;
	tc_column =62;
	SongTimeNum(gw_FileIndex[0]/100,tc_column);
	tc_column+=6;
	SongTimeNum((gw_FileIndex[0]%100)/10,tc_column);
	tc_column+=6;
	SongTimeNum(gw_FileIndex[0]%10,tc_column);
	

	gc_LCM_line=1;
	tc_column=62;
	SongTimeNum(gw_FileTotalNumber[0]/100,tc_column);
	tc_column+=6;
	SongTimeNum((gw_FileTotalNumber[0]%100)/10,tc_column);
	tc_column+=6;
	SongTimeNum(gw_FileTotalNumber[0]%10,tc_column);
	gc_LCM_line=0;

    #elif (SERIAL_MONO == FEATURE_ON)   // Serial LCM

    uint8 file_num; // File NR in this dir not recursively.

    file_num = gs_DIRtable[gw_DIRtableIndex].c_file_num_lo;
    ui_show_song_num (gw_FileIndex[0], file_num);
    //ui_show_song_num (gw_FileIndex[0], gw_FileTotalNumber[0]);
    //ui_show_song_pos(gw_FileIndex[0]);
    //ui_show_song_num_total(gw_FileTotalNumber[0]);

    #endif
}

/*
void LCM_ShowBitRateIcon(void)
{
	U8	i,j,k=0;

	for(i=6;i<8;i++)
	{
		LCM_set_address(i,104);
		for(j=0;j<21;j++)
		{
			LCM_write_data(BitRateIcon128x64[k]);
			k++;
		}
	}	
}
*/

void LCM_ShowPlayTime(U16 time)
{
    #if (SERIAL_MONO == FEATURE_OFF)

	U8	tc_column;
	gc_LCM_line=0;
	tc_column=98;
	SongTimeNum(time/600,tc_column);
	tc_column+=6;	

	SongTimeNum((time%600)/60,tc_column);
	tc_column+=6;

	SongTimeNum(10,tc_column);
	tc_column+=6;

	SongTimeNum((time%60)/10,tc_column);
	tc_column+=6;

	SongTimeNum(time%10,tc_column);

	//===============total time===========
	
	
	gc_LCM_line=1;
	tc_column=98;
	SongTimeNum(gw_TotalSec/600,tc_column);
	tc_column+=6;	

	SongTimeNum((gw_TotalSec%600)/60,tc_column);
	tc_column+=6;

	SongTimeNum(10,tc_column);
	tc_column+=6;

	SongTimeNum((gw_TotalSec%60)/10,tc_column);
	tc_column+=6;

	SongTimeNum(gw_TotalSec%10,tc_column);

	gc_LCM_line=0;

    #elif (SERIAL_MONO == FEATURE_ON)

    ui_show_song_time(time);

    #endif

}




void LCM_ShowChar6x8_1(U8 Num,U8 tc_column)
{
	U8	j;
		
		LCM_set_address(gc_LCM_line,tc_column);
		for(j=0;j<6;j++)
		{
			LCM_write_data(Bit_CharIcon6x8_1[Num*6+j]);
}


}
void SongTimeNum(U8 Num,U8 tc_column)
{
	U8 j;
		LCM_set_address(gc_LCM_line,tc_column);
		for(j =0; j<5;j++)
		{
			LCM_write_data(SongNumTime_char[Num*5+j]);
		}
}
/*
void LCM_ShowChar8x16_1(U8 Num,U8 tc_column)
{
	U8	i,j;

	for(i=0;i<2;i++)
	{
		LCM_set_address(i+gc_LCM_line,tc_column);
		for(j=0;j<8;j++)
		{
			LCM_write_data(CharIcon8x16_1[Num*16+j+i*8]);
		}
	}
}
*/

void LRC_LCMDisplayString(U8 *tpc_DataBuf, U16 tw_nByte,U8 tc_RowAddr) 
{	
	U8 tc_ColumnEnd;
	U8 temp_RowAddr;
	U8 Tmp_DataBuf[32];
	U8 tc_ColumnAddr,tc_ColumnNum;
	U8 tc_temp;
	U8 CharDispOffset = 0;//bsb
	U16 tw_Loop;

	temp_RowAddr = tc_RowAddr;
	tc_ColumnEnd=128-CharDispOffset;
	tc_ColumnAddr = 0;
	tc_temp = 0;
	tw_Loop = 0;
	if(gcPlay_FileType==0)  //20081211 chiayen add for LRC
	{
	MP3_DataIn();  //chiayen add
	}
	else if(gcPlay_FileType==1)
	{
		WMA_DataIn();	
	}
	while(tw_Loop < tw_nByte)
	{
		tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(tpc_DataBuf+tw_Loop),*(tpc_DataBuf+tw_Loop+1),Tmp_DataBuf,0);
		if(*(tpc_DataBuf+tw_Loop)&0x80)
		{
			tc_temp = 16;
		}
		else
		{
		    tc_temp = 8;
		}
	   
		Polling_TaskEvents();
		if(gc_xs)
		{
		  //gc_xs=0;
		  return;
	    }
		if((tc_ColumnAddr + tc_temp) >= tc_ColumnEnd)
		{
			LRC_LCMFillColumn(tc_RowAddr/8,tc_ColumnAddr,tc_ColumnEnd);
			if(tc_RowAddr + 16 >=temp_RowAddr+32) 
			{
				break;
			}
			tc_ColumnAddr = 0;
			tc_RowAddr += 16;
		}
		if((tc_ColumnNum&0x7f)>8)
		{
			LCM_disp_HZKCharBMP(tc_RowAddr/8,tc_ColumnAddr,Tmp_DataBuf,1,0);
		}
		else
		{
			LCM_disp_HZKCharBMP(tc_RowAddr/8,tc_ColumnAddr,Tmp_DataBuf,0,0);
			if(tc_temp == 16)  LRC_LCMFillColumn(tc_RowAddr/8,tc_ColumnAddr + 8,tc_ColumnAddr + 16);
		}
		tc_ColumnAddr+=tc_temp;
		if(tc_ColumnNum&0x80)
		{
			tw_Loop+=2;
		}
		else
		{
			tw_Loop+=1;
		}
	}
	if(tc_ColumnAddr < tc_ColumnEnd)
	{
		LRC_LCMFillColumn(tc_RowAddr/8,tc_ColumnAddr,tc_ColumnEnd);
	}
	tc_RowAddr += 16;
	while(tc_RowAddr < temp_RowAddr+32)
	{
		LRC_LCMFillColumn(tc_RowAddr/8,0,tc_ColumnEnd);
		tc_RowAddr += 16;
	}
}


void LRC_LCMFillColumn(U8 tc_Page,U8 tc_CurrentColumn,U8 tc_TotalColumn)
{
	U8 tc_Width,i,j;

	tc_Width = tc_TotalColumn - tc_CurrentColumn;
	for(j=0;j<2;j++)
	{
		LCM_set_address(tc_Page+j,tc_CurrentColumn);
		for(i=0;i<tc_Width;i++)
		{		
			LCM_write_data(0);	
		}
	}
}


U8 LCM_Disp_FileName(U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 DispOnOff)
{
    #if (SERIAL_MONO == FEATURE_OFF)

	U8 i,Column;
	U8 Tmp_DataBuf[32];
	U8 Sts;
	U8 tc_ColumnNum,tc_FirstWorldColumnNum;

	DispOnOff=1;
	tc_FirstWorldColumnNum = 0x00;
	Sts = 0;
	i=0;
	Column=0;

	while(i<nByte)
	{
		if(DispOnOff)
		{
			if(!tc_ISNOrUnicode)
			{
				tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(DataBuf+i+1),*(DataBuf+i),Tmp_DataBuf,1);
			}else{
				tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(DataBuf+i),*(DataBuf+i+1),Tmp_DataBuf,0);
			}
			if(i==0)
			{
				tc_FirstWorldColumnNum = tc_ColumnNum;
			}
		}			
		if((Column+(tc_ColumnNum&0x7f))>128)
		{
			Sts=1;
			goto DispOver;
		}
		if(DispOnOff)
		{	
			if((tc_ColumnNum&0x7f) > 8)
			{
				LCM_disp_HZKCharBMP(2,Column,Tmp_DataBuf,LCM_IsWord,0);		//just for solang by home
			}
			else
			{
				LCM_disp_HZKCharBMP(2,Column,Tmp_DataBuf,LCM_IsChar,0);
			}
		}
		Column+=(tc_ColumnNum&0x7f);

		if(tc_ColumnNum&0x80)
		{
			i+=2;
		}else
		{
			i+=1;
		}
	}
	Sts=0;
DispOver:
	while(Column<128 && DispOnOff)
	{
		LCM_set_address(2,Column);//just for solang by home.
		LCM_write_data(0x00);
		LCM_set_address(2+1,Column);
		LCM_write_data(0x00);
		Column++;
	}
	if(Sts)
	{
		if(tc_FirstWorldColumnNum&0x80)
		{
			gw_DispFileName_ByteOffset++;
		}
		gw_DispFileName_ByteOffset+=1;
	}else{
		gw_DispFileName_ByteOffset=0;
	}
	return Sts;//overstep display area

    #elif (SERIAL_MONO == FEATURE_ON)
    
    ui_show_file_name (DataBuf, tc_ISNOrUnicode, nByte, DispOnOff);
    return 0;

    #endif
}


void LCM_disp_HZKCharBMP(U8 Page, U8 Column,U8 *BMPdataBuf, U8 CharOrWord,U8 reverse)
{
	U8 i,j;

	if(CharOrWord)//char(8x16)--------word(12x16)
	{
		i=0;j=31;
	}else{
		i=0;j=15;
	}
	LCM_set_address(Page,Column);

	for(;i<j;i=i+2)
	{
		if(reverse)
			LCM_write_data(~*(BMPdataBuf+(i)));
		else
			LCM_write_data(*(BMPdataBuf+(i)));
	}

	if(CharOrWord)
	{
		i=1;j=32;
	}else{
		i=1;j=16;
	}

	LCM_set_address(Page+1,Column);
	
	for(;i<j;i=i+2)
	{
		if(reverse)
			LCM_write_data(~*(BMPdataBuf+(i)));
		else
			LCM_write_data(*(BMPdataBuf+(i)));
	}
}


void RollFileName()
{
    if((!gw_LCMScrollTimer))
	{
		LCM_Disp_FileName(&gc_FileLongName[gw_DispFileName_ByteOffset+5],gc_FileLongName[2],gc_FileLongName[4]-gw_DispFileName_ByteOffset,1);
	}
}
////////////sunzhk add pinpu reflash---100427=====

code U8 Pinpu_16x11[]=	//频谱高度
{	
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x00,//高度0	每副高度 32*4bit=16Byte
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0x00,//高度1
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xe0,0xe0,0x00,//高度2
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf0,0xf0,0xf0,0x00,//高度3
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0x00,//高度4
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0x00,0xff,0xff,0xff,0x00,//高度5
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0xFC,0xFC,0x00,0xff,0xff,0xff,0x00,//高度6
	0x00,0x00,0x00,0x00,0xc0,0xc0,0xc0,0x00,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0x00,//高度7
	0x00,0x00,0x00,0x00,0xFE,0xFE,0xFE,0x00,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0x00,//高度8
	0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0x00,//高度9
	0xc0,0xc0,0xc0,0x00,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0x00,0xff,0xff,0xff,0x00,//高度10
};
code U8 gc_PinPu_table[10][11]=//10副频谱图片，每副频谱中有10个频谱列，表中的每个数据为每副中每列的高度
{
	//第0幅频谱每列高度： 
	0,1,2,3,4,5,6,7,8,9,10,
	//第1幅频谱每列高度：
	4,7,3,8,4,7,3,9,4,8,3,
	//第2幅频谱每列高度：
	9,3,7,4,9,3,2,8,4,9,3,
	//第3幅频谱每列高度：
	4,9,3,7,4,3,9,4,8,4,8,
	//第4幅频谱每列高度：
	9,3,8,4,9,3,7,4,8,3,7,
	//第5幅频谱每列高度：
	9,4,3,7,8,4,9,3,7,4,3,
	//第6幅频谱每列高度：
	8,3,7,4,8,3,2,7,4,9,3,
	//第7幅频谱每列高度：
	9,4,3,2,7,8,4,9,3,2,7,
	//第8幅频谱每列高度：
	4,9,8,3,7,2,9,4,7,3,2,
   	//第9幅频谱每列高度：
	9,3,4,2,7,4,8,3,2,6,4,
};
xdata U8 gc_Pinpu_Off=0;
void LCM_ShowPinpu_Icon()
{
	U8 i,j,k,m,tc_pinpu;
	
	if(gc_Pinpu_Off<9)	//total 0~ 9 = 10 副图片
	{
		gc_Pinpu_Off++;
	}else
	{
		gc_Pinpu_Off=0;
	}
  
	for(m=0;m<11;m++)	  //如果卡音则需要调整数据输入方法
	{	//11列频谱
		k=0; 
		tc_pinpu = 16*gc_PinPu_table[gc_Pinpu_Off][m];

		for(i=4;i<8;i++)   //显示1列频谱
		{
			LCM_set_address(i,m*5);	  				//m*5  5= width
			for(j=0;j<4;j++)	
			{
				LCM_write_data( Pinpu_16x11[tc_pinpu+k] );
				k++;
			}
		}	
	} 

}
