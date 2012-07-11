#include "..\UI_header\UI_Trans.h"


//===============const define===============================================================================
#define	C_TRANSFERTABLE_PLAY_LEN		6
#define	C_SUBTRANSFERTABLE_PLAY_WIDTH	6

//#define	C_DISPMSG_INI_PLAY				0x023fff
//#define	C_DISPMSG_INI_PLAY				0x02bfff // victor : to let volume display
#define	C_DISPMSG_INI_PLAY				0x027fff //(JC)display volume level in idle 
#define	C_DISPALL_L_PLAY				0xff
#define	C_DISPALL_M_PLAY				0x7f
#define	C_DISPALL_H_PLAY				0x02


#define	C_Task_PlayMenu_Tmp				14
#define	C_Task_PlayOther_Tmp			15
//#define	C_LKey_Lock

//=========in play bank===============================================================================

//enum UISTS_PLAY
/*
#define	C_UISTS_PLAY_PLAYING		0	//1
#define	C_UISTS_PLAY_PLAYPAUSE		1	//2
#define	C_UISTS_PLAY_PLAYVOL		2	//3
#define	C_UISTS_PLAY_PLAYABMODE		3	//4
#define	C_UISTS_PLAY_PLAYFF			4	//5	//¿ì½ø
#define	C_UISTS_PLAY_PLAYFR			5	//6	//¿ìÍË
#define	C_UISTS_PLAY_PLAYKEYLOCK	6	//7
*/
//=========need change bank===========================================================================

#define	C_UISTS_PLAY_BANKMENU			(0x80|C_Task_Menu)
#define	C_UISTS_PLAY_BANKIDLE			(0x80|C_Task_Idle)
#define	C_UISTS_PLAY_BANKREC			(0x80|C_Task_Rec)
#define	C_UISTS_PLAY_BANKPLAYOTHER		(0x80|C_Task_PlayOther_Tmp)


