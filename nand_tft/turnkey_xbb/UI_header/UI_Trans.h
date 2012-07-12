#ifndef	_UI_TRANS_H_
#define	_UI_TRANS_H_



#define	DbgPrint	dbprintf
//=======================按键有无的定义====================
//#include	"UI_Func.h"
#define	KEY_NOEQKEY
#define	KEY_NOLOCKKEY

#define	DEF_COMMENT			/

#ifdef	KEY_NOLOCKKEY
	#define	C_LKey_Lock		DEF_COMMENT/
	#define	C_Key_Lock		DEF_COMMENT/
#else

#endif



//===============const define===============================================================================
//#define	C_TRANSFERTABLE_LEN			6
#define	C_SUBTRANSFERTABLE_WIDTH		6
#define	C_SUBTRANSFERTABLE_WIDTH_MAX	8
#define	C_HEADERSIZE_MAX				6

#define	C_Task_PlayMenu_Tmp				14
#define	C_Task_PlayOther_Tmp			15
//#define	C_LKey_Lock

//==========UI States define==========================================================================
#define	C_UISTS_STARTNUMB			0	//1
#define	C_UISTS_END						0xff
#define	C_UISTS_NULL					0xff

//==========================UI Event define===========================================================
//----------key event-------------------------------------------------------------------------------
#define UIE_Key_Play                0x01	// UIEvent short_key_UIEvent
#define UIE_Key_Mode                0x02
#define UIE_Key_Next				0x03
#define UIE_Key_Prev               	0x04
#define UIE_Key_Volup             	0x05
#define UIE_Key_Voldn               0x06
#define UIE_Key_Rec                 0x07
#define UIE_Key_Vol	 				0x08

#define UIE_LKey_Play               0x11	// UIEvent long_key_UIEvent
#define UIE_LKey_Mode               0x12
#define UIE_LKey_Next               0x13
#define UIE_LKey_Prev               0x14
#define UIE_LKey_Volup              0x15
#define UIE_LKey_Voldn              0x16
#define UIE_LKey_Rec                0x17
#define UIE_LKey_Vol	 			0x18

#define UIE_RKey_Play               0x21	// UIEvent long_key_release_UIEvent
#define UIE_RKey_Mode               0x22
#define UIE_RKey_Next               0x23
#define UIE_RKey_Prev               0x24
#define UIE_RKey_Volup              0x25
#define UIE_RKey_Voldn              0x26
#define UIE_RKey_Rec                0x27
#define UIE_RKey_Vol				0x28

//---------menu event-------------------------------------------------------------------------------
#define	C_MENU2MENU			0x41
#define	C_MENU2MAIN			0x42	//从目录返回到主界面(非目录)	
#define	C_MENU2ABMODE		0x43

#define C_MENU2Rectime		0x44	//系统设置中进入录音时间设置
#define C_MENU2Cap			0x45	//磁盘容量
#define C_MENU2FW_VERSION	0x46	//固件版本
#define C_MENU_RECTIME_On		0x47	//显示录音时间项
#define C_MENU_RECTIME_Off		0x48	//不显示录音时间项

#define C_Init 				0x50 //sq added for Rec @080828
#define C_Init_Pic			0x51 //sq added for Rec @080828
#define C_Save				0x52
#define UIE_EOF				0x53 //Ching 080924
#define UIE_VOICESTOP		0x54 //sq added for refreshing LCD when stop after play once all@080928
#define UIE_PLAY			0x55 //lijian.peng 081029 add for disp MainInterface when play-start
#define UIE_PM_INI			0x56 //lijian.peng 081030 add for disp PlayMenu when jump to PlayMenu
#define UIE_FFFREND			0x57 //lijian.peng 081031 add for disp icon when return from fastFF/FR

#define UIE_PLAYSTOP		0x58
#define C_NEXTFILEDISP		0x59//08/09/28,joyce add


#define C_ABMODE2PLAY       0x5a//tangwei add 080927

//-----------timer event-----------------------------------------------------------------------------
#define	C_QUARSECDISP		0x49	// Quarter-Second trigger
#define	C_HALFSECDISP		0x4A	// Half-Second trigger
#define	C_SECDISP			0x4B	// one-Second trigger
#define	C_VOL2MAIN			0x4C	// timer trigger -- from Volume return to Main-Interface
#define	C_NOFILE2MAIN		0x4D	// timer trigger -- from No-file return to Main-Interface or Menu
#define C_BATTERY_DISP		0x4E   //constant timer trigger -----battery display huiling.gu add 081014
//---------vol event-------------------------------------------------------------------------------
#define	C_ReturnPlay		0x60	// vol return to playing mode
#define	C_ReturnStopPause	0x61	// vol return to StopPause mode


//=============setting MSG define=================
#define	Setting_Disp_Begin			0x00000010
#define	REC_Disp_HHSS          		0x00000020



#endif
