UBYTE code IdleTask_PhaseTab[]=
{
	C_NULLPHASE,//none
	C_NULLPHASE,//play
	C_NULLPHASE,//mode
	C_ModeNext,//next
	C_ModePrev,//prev
	C_NULLPHASE,//vol+
	C_NULLPHASE,//vol-
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,

	C_NULLPHASE,//none
	C_NULLPHASE,//Lplay
	C_NULLPHASE,//Lmode
	C_ModeNext,//Lnext
	C_ModePrev,//Lprev
	C_NULLPHASE,//Lvol+
	C_NULLPHASE,//Lvol-
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,

	C_NULLPHASE,//none
	C_NULLPHASE,//Rplay
	C_NULLPHASE,//Rmode
	C_NULLPHASE,//Rnext
	C_NULLPHASE,//Rprev
	C_NULLPHASE,//Rvol+
	C_NULLPHASE,//Rvol-
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE
};

#message "Does index mismatch with key event?"
/// REVISIT!!!
/// The sequence of play task phase highly depends on the macro definition
/// of keys. This is so ugly. But I don't have time to improve this...:(
/// If you change the key macro defintion, you must change here accordingly.
UBYTE code PlayTask_PhaseTab[]=
{
	C_PlayIdle,         // 0x00	C_Key_None
	C_PlayPause,        // 0x01	C_Key_Play
	C_PlayPause,        // 0x02	C_Key_Mode
                        // The phase should be C_PlayIdle. Just use mode as play.
	C_PlayNext,         // 0x03	C_Key_Next 
	C_PlayPrev,         // 0x04	C_Key_Prev 
	C_PlayVolUp,        // 0x05	C_Key_Volup. It was C_FmFreqChg.
	C_PlayVolDn,        // 0x06	C_Key_Voldn. It was C_FmFreqChgDown.
	C_PlayIdle,         // 0x07	C_Key_Rec
	C_PlayIdle,         // 0x08	C_Key_Vol
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,         // 0x0F

	C_PlayIdle,         // 0x10
	C_PowerOff,         // 0x11	C_LKey_Play 
	C_PowerOff,         // 0x12	C_LKey_Mode 
	C_PlayFF,           // 0x13	C_LKey_Next 
	C_PlayFR,           // 0x14	C_LKey_Prev 
	C_PlayModeChg,      // 0x15	C_LKey_Volup
	C_FmFreqChgDown,    // 0x16	C_LKey_Voldn
	C_PlayProc,         // 0x17	C_LKey_Rec  
	C_PlayProc,         // 0x18	C_LKey_Vol
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,         // 0x1F

	C_PlayIdle,         // 0x20
	C_Play_to_fm,       // 0x21 KEY_VALUE_FM
	C_PlayProc,
	C_PlayProc,
	C_PlayProc,
	C_PlayProc,
	C_PlayProc,
	C_PlayProc,
	C_PlayProc,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE         // 0x2F
};


UBYTE code JpegTask_PhaseTab[]=
{
	C_JpegIdle,//none
	C_JpegPause,//play
	C_JpegIdle,//mode
	C_JpegNext,//next
	C_JpegPrev,//prev
	C_JpegIdle,//vol+
	C_JpegIdle,//vol-
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,

	C_JpegIdle,//none
	C_JpegStop,//Lplay
	C_JpegUpLevel,//Lmode
	C_JpegIdle,//Lnext
	C_JpegIdle,//Lprev
	C_JpegIdle,//Lvol+
	C_JpegIdle,//Lvol-
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,
	C_JpegIdle,

	C_JpegIdle,//none
	C_JpegIdle,//Rplay
	C_JpegIdle,//Rmode
	C_JpegIdle,//Rnext
	C_JpegIdle,//Rprev
	C_JpegIdle,//Rvol+
	C_JpegIdle,//Rvol-
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE
};


UBYTE code MjpegTask_PhaseTab[]=
{
	C_MjpegIdle,//none
	C_MjpegPause,//play
	C_MjpegIdle,//mode
	C_MjpegNext,//next
	C_MjpegPrev,//prev
	C_MjpegVolUp,//vol+
	C_MjpegVolDn,//vol-
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,

	C_MjpegIdle,//none
	C_MjpegStop,//Lplay
	C_MjpegUpLevel,//Lmode
	C_MjpegFF,//Lnext
	C_MjpegFR,//Lprev
	C_MjpegIdle,//Lvol+
	C_MjpegIdle,//Lvol-
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,

	C_MjpegIdle,//none
	C_MjpegIdle,//Rplay
	C_MjpegIdle,//Rmode
	C_MjpegFfFrEnd,//Rnext
	C_MjpegFfFrEnd,//Rprev
	C_MjpegVolUp,//Rvol+
	C_MjpegVolDn,//Rvol-
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE
};

UBYTE code FMRTask_PhaseTab[]=
{
	C_FMRIdle,//none
	C_FMRProc,//play
	C_FMRIdle,//mode
	C_FMRNextCH,//next
	C_FMRPrevCH,//prev
	C_FMRVolUp,//vol+
	C_FMRVolDn,//vol-
	C_FMRIdle,
	C_FMRIdle,
	C_FMRIdle,
	C_FMRIdle,
	C_FMRIdle,
	C_FMRIdle,
	C_FMRIdle,
	C_FMRIdle,
	C_FMRIdle,

	C_FMRIdle,//none
	C_FMRStop,//Lplay
	C_FMRUpLevel,//Lmode
	C_FMRNextCH,//Lnext
	C_FMRPrevCH,//Lprev
	C_NULLPHASE,//Lvol+
	C_NULLPHASE,//Lvol-
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,

	C_NULLPHASE,//none
	C_NULLPHASE,//Rplay
	C_NULLPHASE,//Rmode
	C_NULLPHASE,//Rnext
	C_NULLPHASE,//Rprev
	C_FMRVolUp,//Rvol+
	C_FMRVolDn,//Rvol-
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE
};

UBYTE code MenuTask_PhaseTab[]=
{
	C_NULLPHASE,//none
	C_MenuIdle,//play
	C_MenuProc,//mode
	C_MenuProc,//next
	C_MenuProc,//prev
	C_MenuIdle,//vol+
	C_MenuIdle,//vol-
	C_MenuProc,//rec
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,

	C_NULLPHASE,//none
	C_MenuIdle,//Lplay
	C_MenuIdle,//Lmode
	C_MenuIdle,//Lnext
	C_MenuIdle,//Lprev
	C_MenuIdle,//Lvol+
	C_MenuIdle,//Lvol-
	C_MenuIdle,//Lrec
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,

	C_NULLPHASE,//none
	C_MenuIdle,//Rplay
	C_MenuIdle,//Rmode
	C_MenuIdle,//Rnext
	C_MenuIdle,//Rprev
	C_MenuIdle,//Rvol+
	C_MenuIdle,//Rvol-
	C_MenuIdle,//Rrec
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE,
	C_NULLPHASE
};
