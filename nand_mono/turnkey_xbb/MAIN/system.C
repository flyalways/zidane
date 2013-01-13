#include <string.h>
#include <intrins.h>
#include "SPDA2K.h"
#include "global.h"
#include "usb_data.h"
#include "PROTOTYP.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "Flash_code.h"
#include "..\Header\UI_CODETAB.h"
#include "..\IR\remote.h"
#include "..\IR\LCM_BMP.h"
#include "..\header\host_init.h"
#include "..\..\libsource\header\UI_config.h"
#include "..\header\variables.h"
#include "../ui/ui.h"
#include "../i2c/tca8418_keypad.h"
#include "../i2c/i2c_sim.h"
#include "../spi/gt_font.h"
#include "../i2c/kt0810.h"
#include "../i2c/fm_new.h"

xdata U8 gc_KeyDet_Mask=0; 

xdata U8 gc_LongKeyCount=0;
xdata U8 gc_USBDispStatus=0;
xdata U16 gw_USBDisplayTime=0;

xdata U8 gc_RepPlayMode=2;

void Timer0_init(void);
void TimeOutHandle(void);
void LCM_Disp_USBLine();
void LCM_Disp_XBMPArray_USB(U8 *Condition,U8 *Data);
void LCM_Disp_USBRWLogo(U8 Condition);	//USB 0:(Read); 1:(Write)

U8 Key_Detect(void);
U8 READ_SARADC(U8 tc_ADCChannel);

extern bit gbLKeyTimer_Timeout;
extern xdata U8 gcGetKey_Timeout;

extern xdata U8 gc_CurrentCard_backup;

extern void ProcKey_in_idle(void);
extern void ProcKey_in_play(void);
extern void ProcKey_in_rec(void);
extern void ProcKey_in_voice(void);
extern void ProcKey_in_jpeg(void);
extern void ProcKey_in_mjpeg(void);
extern void load_nand_bank(void);
extern void init_system(void);
extern void LCM_init_process(void); 
extern void	FM_drive(void); 
extern void	FM_initial(void);

extern bit gb_cmd_timeout;  //080925chiayen add
extern U8 gc_PseudoCard_Flag;

extern	data	bit	gb_SD_Exist;
extern	data	bit	gb_shortpack;
extern data unsigned int Offset;
extern data System_Struct gs_System_State;

extern Host_register_INIT();

extern U8 Get_LogData_PageIndex(void);
extern U8 USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);//use reserveblock for log
extern data U16 gw_FM_frequency;

void UI_host_detect(void);
extern void LCM_ShowWait();  //20081124 chiayen add 

extern U8 DOS_DIRtable(void);  //20090107 chiayen add
extern void Dir_Task();  //20090107 chiayen add
extern void USER_GetUISetIndex(void);  //20090107 chiayen add
extern xdata U8 gc_SetIndex;		//for rolling mechanism  //20090107 chiayen add
extern xdata U8 gc_SetNumber;  //20090107 chiayen add

U8	gc_clock_mode = CLOCK_MODE_XTL; // The current clock mode of the system.  //20090526 chiayen modify

extern void PlayMenuProcess();  //20090107 chiayen add
extern void PlayMenuProcess_IR();  //20090206 chiayen add
extern void PauseMenuProcess();  //20090107 chiayen add

extern xdata U8  gc_RespBuff[17];

extern data bit gb_Host_Exist; //20090803 chiayen add
extern xdata	U32	gdw_HOSTStartSectorRead;  //20090803 chiayen add

code U8 LanguageTable[]={15,16,0,18,17,0,0,0,0,0,1,0,0,0,1,4,0};

extern void PowerOffProcess();
extern void RPTMenuProcess();

void main(void)
{	
	U8  tc_clock_mode_backup;  //20090817 chiayen mark	
	
	XBYTE[0xB091]=0x01;	// 1.8v/3.3v regulator turn on 
	XBYTE[0xB092]=0x77; // 3.2V & 1.9V
	XBYTE[0xB093]=0x00;	// LVR=2.4V
	
	// DSP initial
	XBYTE[0xB010]&=0xFE;	// Disable DSP clock, Jimi 080729 for pop noise as 1st time play
	XBYTE[0xB002]&=0xFE;	// Release DSP Reset, Jimi 080729 for pop noise as 1st time play
	XBYTE[0xB08a]|=0x08; 	//(JC)MCU:audio DAC on
	XBYTE[0xB08a]|=0x10; 	//(JC)MCU:Headphone driver on
	XBYTE[0xB08a]|=0x20; 	//(JC)MCU:Headphone driver DD comm-mode bias o/p
	XBYTE[0xB0ED]|=0x02;	//(Jimi 091027)Audio DA zero cross reset bypass on
	XBYTE[0xB0C1]=0xB0;		//(Jimi 091027)Pre-mux , post-mux and internal XCK select
	XBYTE[0xB0C4]=2;		//(Jimi 091027)LCH speaker volume(i.e LCH OP Gain)
	XBYTE[0xB0C5]=2;		//(Jimi 091027)RCH speaker volume(i.e RCH OP Gain)
	
	//SAR ADC init
	XBYTE[0xB05E] = 0; // disable SAR FS clock
	XBYTE[0xB05F] = 0x5F; // SAR clock = 12MHz/16/6 = 125kHz = 8us
	
	//--------------------------------------------	// 
	gbt_Force_USB_FullSpeed =FALSE;
	gbt_BuildMapping_Process = 0;
	gbt_USB_Detected = 0;
	gc_CardDetect = 0x00 ;
	gc_CardExist  = 0x00 ;  			// bit 0  : Nand Flash  bit 1: SD_Card
	gc_CardType   = 0x01 ;				// default Flash Interface 
	gbt_USB_Detected_backUp = 0;
	gc_CardChangeStage =0x00;           // bit 0 : Nand  bit 1 : SD_Card    
	gw_FileSkipNumber=0;
	gb_TriggerFileSkip=0;
	gb_FindFile=0;
	gbt_newFlash			= 0;
	gbt_Support_Dummy_Capacity =0;              // default disable dummy capacity function 
	gc_RespBuff[1]=0;
	
	init_system();
	USB_PlugDetect();                               // Detect USB plug                                   
	//----------------------------------------------// Initial Flash module 	
	Init_Flash_Reg();                                                           
	Init_Flash_Variable();
	//-------------- Need search Bank,Dsp ,hzk block info ,first---------//
	FDBP.cFDev = 0;
	XBYTE[0xB40F] = FDBP.cFDev;
	FlashReadID();
//	dbprintf("\n ID:%bx,%bx%bx",ID_Table[0],ID_Table[1],ID_Table[2]);
	Flash_State_Initial();
	Search_BankCode_Block();
	
	InitFlash();
	
	XBYTE[0xB09F]|=0x06;//(JC)Vref Fast setup mode 'n Internal reference voltage power control
	gc_Vrefinit_Timer=60;
	XBYTE[0xB421]=0x13;
	LCM_init_process();
    tca8418_init();
    kt0810_init();
    fm_init();

    //-------------------------------------------------------------------------
    // Put the test routine entry here.
    // They should be all infinite loop.
    //-------------------------------------------------------------------------
#if (UI_TEST_ONLY == FEATURE_ON)
    ui_test();
#endif
    ui_disp_hello();
#if (I2C_SIM_TEST_PIN == FEATURE_ON)
    i2c_sim_test_pin();
#endif
#if (TCA8418_TEST == FEATURE_ON)
    tca8418_test();
#endif
#if (GT_FONT_TEST == FEATURE_ON)
    gt_font_test ();
#endif
#if (KT0810_TEST == FEATURE_ON)
    kt0810_test();
#endif
#if (FM_TEST == FEATURE_ON)
    fm_test();
#endif
	
	if(gbt_USB_Detected)
	{						 		
		USB_Task();
	}

	gb_Host_Exist=1;
	gb_SD_Exist=1;	
	if(!Host_DetectDevice())
	{
		if(!Host_Initial())
		{
			gc_CurrentCard = CURRENT_MEDIA_HOST;
			gb_FindFlag = 0;
		}
		else
		{
			gc_CurrentCard=0;
		}		
	}
	else
	{			 
		if(SD_Card_Detect())
		{
			if(SD_Identification_Flow())
			{
				gc_CardExist |=0x02;
				gc_CurrentCard=2;     	    
			}
			else
			{
				gc_CurrentCard=0;
				gc_CardExist &=0xFD;
				gb_FindFlag = 0;	    
			}     
		}	                         
		else
	    {
			gb_SD_Exist=0;
			gc_CurrentCard = 0;
	    }
	}

	XBYTE[0xB010]&=0xFE;	// Disable DSP clock, Jimi 080729 for pop noise as 1st time play
	XBYTE[0xB002]&=0xFE;	// Release DSP Reset, Jimi 080729 for pop noise as 1st time play
	XBYTE[0xB08a]|=0x08;//(JC)MCU:audio DAC on
	XBYTE[0xB08a]|=0x10;//(JC)MCU:Headphone driver on
	XBYTE[0xB08a]|=0x20;//(JC)MCU:Headphone driver DD comm-mode bias o/p

	//SAR ADC init
	XBYTE[0xB05E] = 0; // disable SAR FS clock
	XBYTE[0xB05F] = 0x5f; // SAR clock = 12MHz/16/6 = 125kHz = 8us

    Get_LogData_PageIndex();  //chiayen0808
    USER_LogFile_ReadWrite(0);//use reserveblock for log
    if((gw_FM_frequency<875) || (gw_FM_frequency>1080))
    {
        gw_FM_frequency=875;
    }
	FM_initial(); 
	FM_drive();

	gc_SetNumber = gw_PagesPerBlock >> 2; //1set=4pages for dir management, Ching 080816  //20090107 chiayen add
	gc_SetIndex = 0; //Ching 080816  //20090107 chiayen add	
	USER_GetUISetIndex();	//for DIR table	index  //20090107 chiayen add

	gb_FindFlag = 0;
	tc_clock_mode_backup=gc_clock_mode;  //20090817 chiayen mark
	set_clock_mode(CLOCK_MODE_DOS); //20090803 chiayen mark
	if (DOS_Initialize()) //20090803 chiayen modify
	{
		gdw_HOSTStartSectorRead=0xFFFFFFF0; 
		if(gc_CurrentCard==CURRENT_MEDIA_HOST)
		{
			if(SD_Card_Detect())
			{
				gb_SD_Exist=1;	
				if(SD_Identification_Flow())
				{
					gc_CardExist |=0x02;
					gc_CurrentCard=2;     	    
				}
				else
				{
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;	    
				}     
			}	                         
			else
		    {
				gb_SD_Exist=0;
				gc_CurrentCard = 0;
		    }					
		}
		else
		{	
			gc_CurrentCard=0;
			gc_CardExist &=0xFD;
			gb_FindFlag = 0;
			InitFlash();					
		}
		DOS_Initialize();
	}
	gc_CurrentCard_backup=gc_CurrentCard; //20090803 chiayen move here
	set_clock_mode(CLOCK_MODE_DOS);  //20090817 chiayen mark
	DOS_DIRtable();  //20090817 chiayen mark

	LCM_clear();
    //LCM_ShowWait();  //20081124 chiayen add 
	gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
	DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);//(JC)count music file no. in root
	set_clock_mode(tc_clock_mode_backup);  //20090817 chiayen mark
	ir_init();
	EA  = 1;
	gc_Task_Current=C_Task_Play;
	gc_Task_Next=C_Task_Play;
	gc_PhaseInx=0;
	gw_init_needed=0xffff;
	Polling_TaskEvents();
	if(gc_Task_Current!=gc_Task_Next)
	{
		gc_Task_Current=gc_Task_Next;	
	}

	while(1)
	{
        dbprintf("Current task: %bx\n", gc_Task_Current);	
		switch(gc_Task_Current)
		{
			case C_Task_Play:
				Play_Task();
			break;					

			case C_Task_USB:
				USB_Task();
			break;

			case C_Task_Dir:  //20090107 chiayen add
				Dir_Task();
			break;

			case C_Task_PlayMenu:  //20090107 chiayen add
				PlayMenuProcess();
			break;

			/*case C_Task_PlayMenu_IR:  //20090107 chiayen add
				PlayMenuProcess_IR();
			break; */

			case C_Task_Setting:  //20090107 chiayen add
				PauseMenuProcess();
			break;					
			case C_Task_PowerOff:
                PowerOffProcess();
                break;

		/*	case C_Task_RPTMenu:
				RPTMenuProcess();
				break; */
		}
	}
}

U8 xdata gc_xs=0;
U8 Key_Detect(void)
{
	if(gc_KeyValue==0)
	{  
		gc_KeyEvent=0;
		return 1;
	}
	else
	{
        
	  	gc_KeyEvent = gc_KeyValue;
        // 2012.09.23 William: below statement will cause system crashing down.
        // The reason should be stack overflow (the string is too long).
        //dbprintf ("[%-10s] line %-4d: gc_KeyEvent = %x\n", __FILE__, __LINE__, gc_KeyEvent);
	   	gc_KeyValue = 0;
	
		if(gc_Task_Current!=C_Task_USB)  //20090107 chiayen modify
		{
			ProcKey_in_play();
            gc_xs=1;
		}
		return 0;
	}
}


void USB_On(void)
{
	/*------------ Speed Up USB CLK -----------*/  
	                    
	USB_Mode();
}
/*****************************************************************************
* NAME:         void USB_Mode_Disp(void)
* DESCRIPTION:
*
*****************************************************************************/
void USB_Mode_Disp(void)
{
	U8 tc_status;

	gw_USBDisplayTime=40;

	tc_status=DEVICE_REG[0x00];
	DEVICE_REG[0x00]=0x01;

	if((gc_USBStage == BulkInReadSectorsStage)||
	   (gc_USBStage == BulkInResponseStage)||
	   (gc_USBStage == BulkInReadBufferStage))
	{
		if(gc_USBDispStatus!=1)
		{
			LCM_Disp_USBRWLogo(0);
			gc_USBDispStatus=1;
		}
	}
	else if((gc_USBStage == BulkOutWriteSectorsStage)||(gc_USBStage == BulkOutStage)||(gc_USBStage == BulkOutWriteBufferStage))
	{
		if(gc_USBDispStatus!=2)
		{
			LCM_Disp_USBRWLogo(1);
			gc_USBDispStatus=2;
		}
	}
	else
	{	
		if(gc_USBDispStatus!=3)
		{
			LCM_Disp_USBLine();
			gc_USBDispStatus=3;
		}
	}

	DEVICE_REG[0x00]=tc_status;

}


/*****************************************************************************               
* NAME:         void USB_PlugDetect(void)                                                    
* DESCRIPTION:                                                                               
*                                                                                            
*****************************************************************************/               
//(JC)H0613
void USB_PlugDetect(void)
{
	U8 tc_i;
	U8 tc_usb_detect;

	XBYTE[0xB0D1] = 0xf0;//(JC)from debounce ckt
	tc_i =0x08;
	while(tc_i)
	{
		tc_usb_detect = XBYTE[0xB0D1]&0x02;
		tc_i--;
	}
	if(tc_usb_detect)
	{
		if(gb_cmd_timeout==0)
		{
	    	gbt_USB_Detected = 1;     // USB_Detected
		}
	}
	else
	{
	    gbt_USB_Detected = 0;
		gb_cmd_timeout=0;
	}
}


U8 READ_SARADC(U8 tc_ADCChannel)
{
	U8 tc_ADCValue;

    //=========SAR ADC init s===========	
	XBYTE[0xB060]=(tc_ADCChannel<<2)|0x01;//(JC)select SAR channel 'n Enable SAR ADC
	for (tc_ADCValue=0; tc_ADCValue<50; tc_ADCValue++);	
	//=========SAR ADC init f===========
	XBYTE[0xB05E] = 1;// enable SAR FS clock
	while(!XBYTE[0xB061]);
	tc_ADCValue = XBYTE[0xB062];
	XBYTE[0xB05E] = 0; // disable SAR FS clock
	return tc_ADCValue;
}


void Timer0_init()//(JC)Timer0 init;~7.5mS@24Mhz MCU 
{
//=========Timer0 init s============
	// set timer 0 and activete it.
	TR0 = 0;

	TH0 = gc_T0IntervalMSB;
	TL0 = gc_T0IntervalLSB;

	ET0 = 1;
	TR0 = 1;
//=========Timer0 init f============
}

/* According to EVB, ideal key values are 0, 59, 88, 118, 163, 203, 237 and 255(No key press).
 * To identify key presses, we use middle values to discriminate between keys.
 */
//! Mapping ADC value to a key.
void Get_KeyValue(U8 ADCValue)
{
    // For the hw design with spda2635a, the ideal key values are:
    //      87:  undefined sw2
    //      162: vol+
    //      236: vol-
    //      255: no key pressed.
    #if defined CAR_64
    if (ADCValue>246)
    {
        gc_key_Pressed = C_Key_None;
    }
    else if (ADCValue>200)
    {
        gc_key_Pressed = C_Key_Voldn;
    }
    else if (ADCValue>125)
    {
        gc_key_Pressed = C_Key_Volup;
    }
    else
    {
        gc_key_Pressed = C_Key_None; // Maybe I can define a new one.
    }

    #else
	if(ADCValue>246)
	{ 		
		gc_key_Pressed=C_Key_None;
	}	
	else if(ADCValue>220)
	{	
 		gc_key_Pressed=C_Key_Vol;	
	}
	else if(ADCValue>183)
	{	
		gc_key_Pressed=C_Key_Rec;	
	}
	else if(ADCValue>140)
	{	
		gc_key_Pressed=C_Key_Voldn;	
	}
	else if(ADCValue>103)
	{
		gc_key_Pressed=C_Key_Volup;	
	}
	else if(ADCValue>74)
	{
		gc_key_Pressed=C_Key_Next;		
	}
	else if(ADCValue>30)
	{
		gc_key_Pressed=C_Key_Prev;				
	}
	else
	{
		gc_key_Pressed=C_Key_Mode;
	}

    #endif // #if defined CAR_64 
}   
extern bit gbLKeyTimer_Timeout;
//bit	gbLKeyTimer_Timeout=0;
void TimeOutHandle()
{
//	if(gbLKeyTimer_Timeout==1)//(JC)Long key detect period 
//	{
//		gbLKeyTimer_Timeout=0;
		if(gc_LongKeyTimer)
		{
			gc_LongKeyTimer++;
		}
//	}

	if(gcGetKey_Timeout==1)//(JC)Read ADC value(get key) period 
	{
		gcGetKey_Timeout=0;
        
        // Read the key event from tca8418 keypad. If we've got valid data, quit.
        // Otherwise, check sac line to see what's there.
        // The value from tca8418a means people have pressed that key because
        // we only care about the released key event from tca8418a.
        // This is very simple and should be enough since we don't use any long
        // key on the keypad matrix scanned by tca8418a.
        if ( gc_key_Pressed = tca8418_get_real_key() )
        {
            gc_KeyValue = gc_key_Pressed;
            return;     
        }

		if((!gc_KeyValue)&&(!gc_KeyDet_Mask))//(JC)Key event determined //20090107 chiayen modify
		{
            // 128 pin evaluation board:
            // P1.3 is used as the play key. But I set it as output direction.
            // Not sure if it can work as expectedly.
            //
            // 64 pin hw design with SPDA2635A:
            // P1.3 is used as the reset pin for the keyscan id.
		    #ifndef CAR_64
			if(!P1_3)
			{
				gc_key_Pressed=C_Key_Play;
			}
			else
		    #endif
			{
				Get_KeyValue(READ_SARADC(0));
			}

			if((gc_key_PrevPressed&0x0f)==gc_key_Pressed)
			{
				if(gc_key_Pressed != C_Key_None)
				{
					if(gc_LongKeyTimer>gc_LongKeyDelay)
					{
						gc_key_PrevPressed |= 0x10;//(JC)long key determined	

						if((gc_key_Pressed==C_Key_Mode) && (gc_LongKeyCount==0))
						{
							gc_KeyValue = gc_key_PrevPressed & 0x7f;
							gc_LongKeyCount=1;
						}
						if(gc_key_Pressed!=C_Key_Mode) 
						{
								gc_KeyValue = gc_key_PrevPressed & 0x7f;
						}

						gc_LongKeyTimer = REPEATKEY_DELAYVALUE;	//(JC)16, for long key over-pressed, counting from 16 to 29... and determining long key again
						gc_LongKeyDelay = (gc_LongKeyDelay < MIN_REPEATKEYDELAY)?MIN_REPEATKEYDELAY:/*(gc_LongKeyDelay-3)*/(REPEATKEY_DELAYVALUE+6);  //20090107 chiayen modify	
					}
					else
					{
						gc_key_PrevPressed |= 0x80;
					}	
				}
			}
			else if(gc_key_Pressed==C_Key_None)//(JC)key released
			{
				if((gc_key_PrevPressed & 0x90) == 0x80)//(JC)short key released
				{
					gc_KeyValue = gc_key_PrevPressed & 0x7f;
				}
				else if((gc_key_PrevPressed & 0x90) == 0x90)//(JC)long key released
				{
					//gc_KeyValue = (gc_key_PrevPressed | 0x20) & 0x6f;//(JC)key value will be 0x2x,long-pressed key released
					
                    // Don't handle the key event when long key is released.
                    // The old way will generate a 0x2x key value which conflicts
                    // with the category keys added later.
                    gc_KeyValue = C_Key_None;
                    gc_LongKeyCount=0; //chiayen0807
				}
				gc_key_PrevPressed = C_Key_None;
				gc_LongKeyTimer = 0;
				gc_LongKeyDelay = 0;	
			}
			else//(JC)new key pressed
			{
				gc_key_PrevPressed = gc_key_Pressed;
				gc_LongKeyTimer = 1;//(JC)long key timer launching
				if (!gc_LongKeyDelay) 
 			    {
					gc_LongKeyDelay = MAX_REPEATKEYDELAY;
				}
			}
		}
	}
}


void Polling_TaskEvents(void)
{
	if(gc_Vrefinit_Timer==0)
	{
		gc_Vrefinit_Timer=1;
		TimeOutHandle();
		Key_Detect();
	}
}


#define CK2_FLAG	(0<<6)
#define CK1_FLAG	(1<<6)

// Set this table according to operating mode for clock switching.
static U8 code gac_clock_table[][8] =
{
	//	0xB006,			0xB009,	0xB024,	0xB025,	0xB026, TH1  TH0            TL0            0xB421
	{	0x00,			0,		0,		0,		0,		243, C_T0_12MhzMSB, C_T0_12MhzLSB},	// No PLL(Xtl=12M),	MCU=12M, PERI=12M, DSP=12M	CLOCK_MODE_XTL
	{  8|64, 			0,         0,      0,      0,    230,      C_T0_24MhzMSB, C_T0_24MhzLSB}, // 2=MP3L PLL=24M, MCU=24M, DSP=24M CLOCK_MODE_MP3LRC
	{ 16|64, 			0,         1,      1,      0,    230,      C_T0_48MhzMSB, C_T0_48MhzLSB}, // 3=WMA  PLL=48M, MCU=24M, DSP=48M CLOCK_MODE_WMA
	{	0x0D|CK2_FLAG,	0x40,	1,		1,		0,		214, C_T0_39MhzMSB,	C_T0_39MhzLSB},	// PLL=78M,			MCU=39M, PERI=39M, DSP=52M	CLOCK_MODE_APE_FAST 		//Jimi Yu 091118 for APE
	{	0x18|CK1_FLAG,	0,		0,		0,		0,		178, C_T0_72MhzMSB, C_T0_72MhzLSB},	// PLL=72M,			MCU=72M, PERI=72M, DSP=72M	CLOCK_MODE_MJPEG
	{	0x18|CK1_FLAG,	0x00,	1,		1,		0,		217, C_T0_36MhzMSB,	C_T0_36MhzLSB},	// PLL=90M,			MCU=36M, PERI=36M, DSP=72M	CLOCK_MODE_APE_NORMAL 		//Jimi Yu 091118 for APE
	{	0x0F|CK2_FLAG,	0x00,	1,		1,		0,		207, C_T0_45MhzMSB,	C_T0_45MhzLSB},	// PLL=90M,			MCU=45M, PERI=45M, DSP=90M	CLOCK_MODE_APE_HIGH	 		//Jimi Yu 091118 for APE
	{	0x18|CK1_FLAG,	0x40,	2,		2,		0,		230, C_T0_24MhzMSB, C_T0_24MhzLSB},	// PLL=72M,			MCU=24M, PERI=24M, DSP=48M	CLOCK_MODE_SSR
	{	0x18|CK1_FLAG,	0,		0,		0,		2,		178, C_T0_72MhzMSB, C_T0_72MhzLSB},	// PLL=72M,			MCU=72M, PERI=72M, DSP=24M	CLOCK_MODE_USB
	{ 0x08|CK1_FLAG,    0,      1,      1,      0,      243, C_T0_12MhzMSB, C_T0_12MhzLSB} // PLL=24M, MCU=12M, PERI=12M, DSP=24M CLOCK_MODE_MP3 

};

//! Set CPU/Peripheral/DSP clock divider in PLL mode.
static void change_clock_ratio(U8 r)
{
	// disable auto wait
	XBYTE[0xB021] = 0;
	XBYTE[0xB022] = 1;
	// set new divide ratio
	XBYTE[0xB009] = gac_clock_table[r][1];	// cpu_mask_sel, per_mask_sel, dsp_mask_sel & n_mask
	XBYTE[0xB024] = gac_clock_table[r][2];	// CPUfreq
	XBYTE[0xB025] = gac_clock_table[r][3];	// PERfreq
	XBYTE[0xB026] = gac_clock_table[r][4];	// DSPfreq
	// Trigger the switch
	XBYTE[0xB020] = 0;	// cpu_protect = dsp_protect = per_protect = 0
	XBYTE[0xB020] = 1;	// freqchange = 1
	_nop_();
}


void xtl_to_pll(U8 tc_clock_mode)
{
	XBYTE[0xB006] = gac_clock_table[tc_clock_mode][0];
	XBYTE[0xB006] = gac_clock_table[tc_clock_mode][0] | (1<<5);
	USER_DelayDTms(2);
	XBYTE[0xB008] = 2;	// PLLS_SWRSTN = 1
	XBYTE[0xB008] = 3;	// PLLS_MODE = 1(PLL clock output)
	change_clock_ratio(tc_clock_mode);
}


void pll_to_xtl(void)
{
	change_clock_ratio(CLOCK_MODE_XTL);
	XBYTE[0xB008]=0x02;	// PLLS_SWRSTN = 1, PLLS_MODE = 0(PLL clock output)
}


void set_clock_mode(U8 mode)
{
	if (gc_clock_mode == mode)
	{
        return;
	}

	if (gac_clock_table[gc_clock_mode][0] != gac_clock_table[mode][0])
	{
		if (gc_clock_mode != CLOCK_MODE_XTL)
		{
			pll_to_xtl();
		}
		if (mode != CLOCK_MODE_XTL)
		{
            xtl_to_pll(mode);
    	}
	}
	else
	{
		change_clock_ratio(CLOCK_MODE_XTL);
		change_clock_ratio(mode);
	}
	TH1 = gac_clock_table[mode][5];
	gc_clock_mode = mode;
	gc_T0IntervalMSB = gac_clock_table[gc_clock_mode][6];
	gc_T0IntervalLSB = gac_clock_table[gc_clock_mode][7];
}

/*****************************************************************************
* NAME:         void USB_I2C(void)
* DESCRIPTION:
*
*****************************************************************************/
void  USB_I2C(void)
{
	XBYTE[0xB00E] = 0x00 ; 						// disable I2C interrupt
	XBYTE[0xB00A] = 0x15 ; 						
	XBYTE[0xB00D] = 0x0F ; 						
	XBYTE[0xB00B] = 0x01 ; 						
	 
	XBYTE[0xB00A] = 0x10 ; 						
	XBYTE[0xB00D] = 0x20 ; 						
	XBYTE[0xB00B] = 0x01 ; 
	USB_I2C_ID = 0x02;
	USB_I2C_Addr = 0x0D;	 						
    USB_I2C_Wdata = 0xA0; 
    I2C_M_Tx(USB_I2C_ID,USB_I2C_Addr,USB_I2C_Wdata);
    I2C_M_Rx(USB_I2C_ID,USB_I2C_Addr);  
}
/*****************************************************************************
* NAME:         void I2C_M_Tx(U8 ID, U8 Addr , U8 Wdata )
* DESCRIPTION:
*
*****************************************************************************/
void I2C_M_Tx(U8 ID,U8 Addr,U8 Wdata ) {
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xD0 ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = ID ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xF0 ; 						
	XBYTE[0xB00B] = 0x01 ; 
	I2C_Wait_Intpend();
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = Addr ; 						
	XBYTE[0xB00B] = 0x01 ;
	I2C_Clr_Int();
	I2C_Wait_Intpend();
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = Wdata ; 						
	XBYTE[0xB00B] = 0x01 ;
	I2C_Clr_Int();
	I2C_Wait_Intpend(); 
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xD0 ; 						
	XBYTE[0xB00B] = 0x01 ;
}
/*****************************************************************************
* NAME:         void I2C_Clr_Int(void )
* DESCRIPTION:
*
*****************************************************************************/	
void I2C_Clr_Int(void )
{
	XBYTE[0xB00A] = 0x10 ; 						
	XBYTE[0xB00D] = 0x20 ; 						
	XBYTE[0xB00B] = 0x01 ;
}
/*****************************************************************************
* NAME:         void I2C_Wait_Intpend(void )
* DESCRIPTION:
*
*****************************************************************************/	
void I2C_Wait_Intpend(void )
{
	while(!(XBYTE[0xB00F]&0x01));
}
/*****************************************************************************
* NAME:         void I2C_M_Rx(U8 ID, U8 Addr )
* DESCRIPTION:
*
*****************************************************************************/
void I2C_M_Rx(U8 ID,U8 Addr) 
{
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xD0 ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = ID ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xF0 ; 						
	XBYTE[0xB00B] = 0x01 ; 
	I2C_Wait_Intpend();
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = Addr ; 						
	XBYTE[0xB00B] = 0x01 ;
	I2C_Clr_Int();
	I2C_Wait_Intpend();
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xD0 ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0x90 ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x13 ; 						
	XBYTE[0xB00D] = ID ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xB0 ; 						
	XBYTE[0xB00B] = 0x01 ;
	I2C_Wait_Intpend();
	I2C_Clr_Int();
	I2C_Wait_Intpend();
	
	XBYTE[0xB00A] = 0x11 ; 						
	XBYTE[0xB00D] = 0xD0 ; 						
	XBYTE[0xB00B] = 0x01 ;
	
	XBYTE[0xB00A] = 0x13 ;
	XBYTE[0xB00B] = 0x10 ;
	USB_I2C_Rdata  =XBYTE[0xB00C];	
}


/***************************************************************************
* NAME:         bit  SD_Card_Detect(void)
* DESCRIPTION:
***************************************************************************/
bit SD_Card_Detect(void)
{
    // REVISIT!!!
    // For our hw design with SPDA2635A, we use micro SD card, where
    // the /CD signal and DAT3 share the same pin, XFMGPIO_16.
    // I don't know how to define a pin for SD_Detect, but just keep
    // it the same with P1^7 in EVB_128. Because P1^7 is not pulled out
    // in 64 pin SPDA2635A.
	#ifdef CAR_64
    P1|=0x80;
	XBYTE[0xB102]&=0x7F;
	#elif defined(EVB_128)  
    P1|=0x80;
	XBYTE[0xB102]&=0x7F;
	#endif	

	if(!SD_Detect)
	{
		if(gc_PseudoCard_Flag==0)
		{
			gc_CardDetect |= 0x02;
			return 1;
		}
		else
		{
			return 0;	
		}
	}
	else
	{
	    gc_CardDetect &= 0xFD; 
	    gc_CardExist &=0xFD;
	    gc_Start_Stop_Flag &= 0xFD;
	    gc_PseudoCard_Flag=0;
	    return 0;
	}  	
}


U8 code LCM_BmpUsbIdleLogo[]=
{  //idle logo
	/*--  width * height=33x32  --*/
	0x00,0x00,0x00,0x00,0x40,0xC0,0x40,0x00,0x12,0x1A,0x16,0x12,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x20,0xA0,0x60,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x14,0x1C,0x14,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x00,0x28,0x38,0x28,0x00,0x80,
	0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x02,0x03,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
};


U8 code LCM_BmpUSBWlogo[]=	//USB Write	
{
	/*--  width * height=33x32  --*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xE0,0xF0,0xF8,0xC0,0xC0,
	0xC0,0xC0,0x00,0xC0,0xC0,0xC0,0x00,0xC0,0xC0,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x0F,0x01,
	0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x00,0x01,0x01,0x00,0x01,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
};


U8 code LCM_BmpUSBRlogo[]=	//USB Read	
{
	/*--  width * height=33x32  --*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0x00,0xC0,0xC0,0x00,0xC0,0xC0,
	0xC0,0x00,0xC0,0xC0,0xC0,0xC0,0xF8,0xF0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x01,0x00,0x01,
	0x01,0x01,0x00,0x01,0x01,0x01,0x01,0x0F,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
};


void LCM_Disp_USBLine()  //for diaplay USB
{
	#if(LCM_SIZE==0)||(LCM_SIZE==2)
	U8  code X_C_IdleLogo[]={4,33,33,50,0,2};//offset3:start_column,offset5:start_page
	#endif
	#if(LCM_SIZE==1)
	U8  code X_C_IdleLogo[]={4,33,33,38,0,0};//offset3:start_column,offset5:start_page
	#endif
 
	LCM_Disp_XBMPArray_USB(X_C_IdleLogo,LCM_BmpUsbIdleLogo);
}


void LCM_Disp_XBMPArray_USB(U8 *Condition,U8 *Data)//_PLAY_&_USB_
{
	U8 i,j;

	for(j=0;j<Condition[0];j++)
	{
		LCM_set_address(j+Condition[5],Condition[3]); 
		for(i=0;i<Condition[1];i++)
		{	
			LCM_write_data(Data[((unsigned int)j*Condition[2])+i+Condition[4]]);
		}
	}
}


void LCM_Disp_USBRWLogo(U8 Condition)	//USB 0:(Read); 1:(Write)
{
	#if(LCM_SIZE==0)||(LCM_SIZE==2)
	U8  code X_C_TranLogo[]={4,33,33,50,0,2};//offset3:start_column,offset5:start_page
	#endif
	#if(LCM_SIZE==1)
	U8  code X_C_TranLogo[]={4,33,33,38,0,0};//offset3:start_column,offset5:start_page
	#endif


	if(Condition==0) 
	{
		LCM_Disp_XBMPArray_USB(X_C_TranLogo,LCM_BmpUSBRlogo);
	} else if(Condition==1)
	{
		LCM_Disp_XBMPArray_USB(X_C_TranLogo,LCM_BmpUSBWlogo);		
	}
}


void Host_Remove(void)
{
	if(gb_HostConnect==1)	
	{
		gb_HostConnect = 0;
	}
}



U8 CheckUSBPlugIN(void)
{
	U8 i;

	XBYTE[0xB011] |= 0x20;   //bit 4   turn on usb_device clk

    XBYTE[0xB016]  = 0x04;                       // power on default is 0x04
    XBYTE[0xB0E9] &= 0xFE;                       // Enable usb pll clk 
    USER_DelayDTms(5);                              // 使用Delay 方式不用polling方式
    XBYTE[0xB01D]  =0x00;

    XBYTE[0xB002] |= 0x04;   //bit 3   turn on USB_SW_RESET
    XBYTE[0xB002] &= 0xFB;   //bit 3   trun off USB_SW_RESET
    XBYTE[0xB002]  |= 0x02;   //bit 2   turn on USB_host_SW_RESET
    XBYTE[0xB5e6]  = 0x00;   //bulksram gated clock
    XBYTE[0xB522]  = 0x01;   //USB2.0 UTMI clock mode set   
    XBYTE[0xB018]  = 0x01;      //USB clock select              //TONY
    XBYTE[0xB002] &= 0xFD;   //bit 2   turn off USB_host_SW_RESET

	Syn_Reset();
	XBYTE[0xB019]  = 0x01;   //host mode
	XBYTE[0xB018]  = 0x01;		//USB clock select				//TONY	
	if(XBYTE[0xB615]==0x01)
	{
		i=100;
		while(i)	i--;	
		if(XBYTE[0xB615]==0x01)
		{
			return 1;
		}
	}
	return 0;
}


U8 Host_DetectDevice(void)
{
	U8	sts;

	if(gc_CurrentCard == CURRENT_MEDIA_HOST)
	{
		if(gb_HostConnect==0)
		{
			Host_Remove();
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		sts = CheckUSBPlugIN();
	}

	if(sts) 
	{
		return 0;
	}
	else
	{
		Host_Remove();
		return 1;
	}
}




