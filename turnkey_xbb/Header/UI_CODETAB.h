UBYTE code PlayTask_PhaseTab[]=
{
	C_PlayIdle,//none
	C_PlayIdle,//mode  //SW309
	C_PlayPause,//play //SW302
	C_PlayPrev,//prev  //SW304
	C_PlayNext,//next  //SW303
	C_PlayMenu,//mode  //SW305
	C_TuneVolFreq,//C_PlayVolUp,//vol+ //SW306
	C_PowerOff,//C_PlayVolDn,//vol- //SW307
	C_PlayIdle,//vol  //SW308
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,

	C_PlayIdle,//none
	C_PlayProc,//
	C_PowerOff,	
//	C_PlayVolDn,//
//	C_PlayVolUp,//
    C_PlayFR,
	C_PlayFF,//Lrec	
	C_MainMenu,//Longkey mode
	C_PlayProc,
	C_PlayProc,//Lrec
	C_PlayProc,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,
	C_PlayIdle,

	C_PlayIdle,//none
	C_PlayIdle,//Rmode
	C_PlayProc,
    C_PlayFfFrEnd,
	C_PlayFfFrEnd,
//	C_PlayProc,
//	C_PlayProc,
	C_PlayProc,
	C_PlayProc,
	C_PlayProc,//Rrec
	C_PlayProc,
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
	C_MjpegIdle,//mode
	C_MjpegPause,//play
	C_MjpegPrev,//prev
	C_MjpegNext,//next
	C_MjpegMenu,
	C_TuneVolFreq,
	C_PowerOff,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,
	C_MjpegIdle,

	C_MjpegIdle,//none
	C_MjpegIdle,
	C_PowerOff,
	C_MjpegFR,//Lprev
	C_MjpegFF,//Lnext
	C_MainMenu,
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

	C_MjpegIdle,//none
	C_MjpegIdle,//Rplay
	C_MjpegIdle,//Rmode
	C_MjpegFfFrEnd,//Rprev
	C_MjpegFfFrEnd,//Rnext
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

UBYTE code JpegTask_PhaseTab[]=
{
	C_JpegIdle,//none
	C_JpegIdle,//mode
	C_JpegPause,//play
	C_JpegPrev,//prev
	C_JpegNext,//next
	C_JpegMenu,
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
	C_PowerOff,
	C_JpegIdle,//Lnext
	C_JpegIdle,//Lprev
	C_MainMenu,
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
