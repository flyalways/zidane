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
#include "..\header\host_init.h"
#include "..\..\libsource\header\UI_config.h"
#include "..\LCM\TFT_display.h"
#include "..\LCM\LCM.h"
#include "..\LCM\LCM_func.h"
#include "..\LCM\UI_icon.h"
#include "..\header\variables.h"

extern	data	bit	gb_HostConnect;

extern U8 Host_Initial(void);

void UI_host_detect(void);
void Timer0_init(void);
void TimeOutHandle(void);
void LCM_Disp_XBMPArray_USB(U8 *Condition,U8 *Data);

U8 Key_Detect(void);
U8 READ_SARADC(U8 tc_ADCChannel);

extern void MainMenuProcess();
extern void SetupMenuProcess();
extern void PowerOffProcess();
extern void Dir_Task();  //20090107 chiayen add
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
extern void USER_GetUISetIndex(void);  //20090107 chiayen add

extern U8 DOS_DIRtable(void);  //20090107 chiayen add
extern U8 Get_LogData_PageIndex(void);
extern U8 USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);//use reserveblock for log

extern xdata U8 gc_LongKeyTimer;
extern xdata U8 gc_LongKeyDelay;

extern	U8	gc_PseudoCard_Flag;
extern	bit		gb_cmd_timeout;  //080925chiayen add
extern	xdata	System_Struct gs_System_State;

extern Host_register_INIT();
U8	gc_clock_mode = CLOCK_MODE_XTL; // The current clock mode of the system.

extern xdata	U32	gdw_HOSTStartSectorRead;

extern data bit gb_Host_Exist; //20090803 chiayen add
extern xdata	U32	gdw_HOSTStartSectorRead;  //20090803 chiayen add

void main(void)
{
//	XBYTE[0xB091]=0x01;	// 1.8v/3.3v regulator turn on 
	XBYTE[0xB092]=0x77; // 3.2V & 1.9V
//	XBYTE[0xB093]=0x00;	// LVR=2.4V

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
	gbt_USB_Detected = 0;
	gc_CardDetect = 0x00 ;
	gc_CardExist  = 0x00 ;  			// bit 0  : Nand Flash  bit 1: SD_Card
	gc_CardType   = 0x01 ;				// default Flash Interface 
	gc_CardChangeStage =0x00;           // bit 0 : Nand  bit 1 : SD_Card    
	gw_FileSkipNumber=0;
	gb_TriggerFileSkip=0;
	gb_FindFile=0;
	gbt_Support_Dummy_Capacity =0;              // default disable dummy capacity function 
	gc_bCBWLUN=0; 
	gbt_enableCDROM =0;							//09/04/24,joyce
	gc_CDROM_Size=0 ;
	init_system();	
	USB_PlugDetect();                               // Detect USB plug                                   
	//----------------------------------------------// Initial Flash module 	
	Init_Flash_Reg();                                                           
	Init_Flash_Variable();
	//-------------- Need search Bank,Dsp ,hzk block info ,first---------//
	FDBP.cFDev = 0;
	XBYTE[0xB40F] = FDBP.cFDev;
	FlashReadID();
	Flash_State_Initial();
	Search_BankCode_Block();

	InitFlash();

	Timer0_init();
//home   just for test
	Get_LogData_PageIndex();  //chiayen0808 
	USER_LogFile_ReadWrite(0);//use reserveblock for log 

	XBYTE[0xB09F]|=0x06;//(JC)Vref Fast setup mode 'n Internal reference voltage power control
	gc_Vrefinit_Timer=60;
	XBYTE[0xB421]=0x13;
	TFT_init();
	#ifdef CAR_48
		XBYTE[0xB102]|=0x3d;//p10=0;
	#else
		XBYTE[0xB102] |= 0x01;
	#endif

	TFT_PowerOnlogo();
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
	XBYTE[0xB400] =0x01;
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
					DEVICE_REG[0x00]= 0x01;
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;	    
				}     
			}	                         
			else
		    {
				DEVICE_REG[0x00]= 0x01;
				gb_SD_Exist=0;
				gc_CurrentCard = 0;
		    }					
		}
		else
		{	
			DEVICE_REG[0x00]= 0x01;  //20090730 chiayen add
			gc_CurrentCard=0;
			gc_CardExist &=0xFD;
			gb_FindFlag = 0;
			InitFlash();					
		}
		DOS_Initialize();
	}
	gc_CurrentCard_backup=gc_CurrentCard; //20090803 chiayen move here
	gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
	DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);//(JC)count music file no. in root
	ir_init();
	EA  = 1;
	gc_Task_Current=C_Task_Play;
	gc_Task_Next=C_Task_Play;
	gc_PhaseInx=0;
	gw_init_needed=0xffff;
	Detect_USB();
	Polling_TaskEvents();
	if(gc_Task_Current!=gc_Task_Next)
	{
		gc_Task_Current=gc_Task_Next;	
	}
	while(1)
	{		
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

			case C_Task_Jpeg:  //20090107 chiayen add
				Jpeg_Task();
			break;

			case C_Task_Mjpeg:  //20090107 chiayen add
				Mjpeg_Task();
			break;

			case C_Task_PlayMenu:  //20090107 chiayen add
//				PlayMenuProcess();
			break;

			case C_Task_PlayMenu_IR:  //20090107 chiayen add
//				PlayMenuProcess_IR();
			break;

			case C_Task_Setting:  //20090107 chiayen add
				SetupMenuProcess();
			break;

			case C_Task_Menu:
				MainMenuProcess();
			break;					

			case C_Task_PowerOff:
				PowerOffProcess();
			break;
		}
	}
}
//1

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
	   	gc_KeyValue = 0;
        if(gc_KeyEvent==0x12) 
        { //sunzhk add power off 100809 
            if(gs_System_State.c_Phase!=TASK_PHASE_STOP) 
            { 
                    if(gc_Task_Current == C_Task_Play) 
                    { 
                            play_stop();
                    } 
                    else if(gc_Task_Current == C_Task_Jpeg) 
                    { 
                            jpeg_stop();
                    } 
                    else if(gc_Task_Current == C_Task_Mjpeg) 
                    { 
                            mjpeg_stop();
                    }                   
            }           
            gc_Task_Next = C_Task_PowerOff; 
            gc_PhaseInx = C_PowerOff; 
            gc_KeyEvent = 0; 
            return 0; 
        } 
		if(gc_Task_Current==C_Task_Play)  //20090107 chiayen modify
		{
			ProcKey_in_play();
		}
		else if(gc_Task_Current==C_Task_Jpeg)  //20090107 chiayen modify
		{
			ProcKey_in_jpeg();
		}
		else if(gc_Task_Current==C_Task_Mjpeg)  //20090107 chiayen modify
		{
			ProcKey_in_mjpeg();
		}
		else
		{
			ProcKey_in_play();
		}
		return 0;
	}
}


void USB_PlugDetect(void)
{
	xdata	U8 tc_i;
	xdata	U8 tc_usb_detect;

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
	xdata	U8 tc_ADCValue;

	if(tc_ADCChannel==2)
	{
		XBYTE[0xB094]=1;
	}
	XBYTE[0xB060]=(tc_ADCChannel<<2)|0x01;
	for(tc_ADCValue=0;tc_ADCValue<50;tc_ADCValue++);
	XBYTE[0xB05E]=1;	// enable SAR FS clock
	while((XBYTE[0xB061]&0x01)==0);
	tc_ADCValue=XBYTE[0xB062];
	XBYTE[0xB05E]=0;	// disable SAR FS clock
	XBYTE[0xB094]=0;
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
}


void TimeOutHandle()
{
	if(gbLKeyTimer_Timeout==1)//(JC)Long key detect period 
	{
		gbLKeyTimer_Timeout=0;
		if(gc_LongKeyTimer)
		{
			gc_LongKeyTimer++;
		}
	}

	if(gbGetKey_Timeout==1)//(JC)Read ADC value(get key) period 
	{
		gbGetKey_Timeout=0;	
		if((!gc_KeyValue)&&(!gc_KeyDet_Mask))//(JC)Key event determined //20090107 chiayen modify
		{
		/*	#ifndef CAR_64
			if(!P1_3)  //chiayenmark for car
			{
				gc_key_Pressed=C_Key_Play;
			}
			else
			#endif*/
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
					gc_KeyValue = (gc_key_PrevPressed | 0x20) & 0x6f;//(JC)key value will be 0x2x,long-pressed key released
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
		TimeOutHandle();
		Key_Detect();
	}
}



#define CK2_FLAG	(0<<6)
#define CK1_FLAG	(1<<6)

static U8 code gac_clock_table[][8] = 
{ 
 //0xB006,    0xB009, 0xB024, 0xB025, 0xB026, TH1  TH0            TL0             
 { 0x00,      0,      0,      0,      0,      243, C_T0_12MhzMSB, C_T0_12MhzLSB}, // No PLL,  MCU=12M, PERI=12M, DSP=12M        CLOCK_MODE_XTL 
 { 0x08|0x40, 0x42,   1,      1,      0,      243, C_T0_12MhzMSB, C_T0_12MhzLSB}, // PLL=24M, MCU=12M, PERI=12M, DSP=19M        CLOCK_MODE_MP3 
 { 0x08|0x40, 0,      0,      0,      0,      230, C_T0_24MhzMSB, C_T0_24MhzLSB}, // PLL=24M, MCU=24M, PERI=24M, DSP=24M        CLOCK_MODE_MP3LRC 
 { 0x10|0x40, 0,      3,      3,      0,      243, C_T0_12MhzMSB, C_T0_12MhzLSB}, // PLL=48M, MCU=12M, PERI=12M, DSP=48M  		CLOCK_MODE_WMA 
 { 0x10|0x40, 0,      1,      1,      0,      230, C_T0_24MhzMSB, C_T0_24MhzLSB}, // PLL=48M, MCU=24M, PERI=24M, DSP=48M        CLOCK_MODE_JPEG 
 { 0x18|0x40, 0,      0,      0,      0,      178, C_T0_72MhzMSB, C_T0_72MhzLSB}, // PLL=72M, MCU=72M, PERI=72M, DSP=72M        CLOCK_MODE_MJPEG 
 { 0x18|0x40, 0,      0,      0,      2,      178, C_T0_72MhzMSB, C_T0_72MhzLSB}, // PLL=72M, MCU=72M, PERI=72M, DSP=24M        CLOCK_MODE_USB 
 { 	20 |0x40, 0,	  0,      0,      0,      217, C_T0_48MhzMSB, C_T0_48MhzLSB},  		// 5-MJPG PLL=60M, MCU=60M, PERI=60M, DSP=48M  CLOCK_MODE_MJPEG 
 //{ 	20 |0x40, 			0,	  			  0,      			0,     			  0,      			217, C_T0_48MhzMSB, C_T0_48MhzLSB},  		// 5-MJPG PLL=60M, MCU=60M,
 }; 






//! Set CPU/Peripheral/DSP clock divider in PLL mode.
static void change_clock_ratio(U8 r)
{
	// disable auto wait
	XBYTE[0xB021] = 0;
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


static void xtl_to_pll(U8 tc_clock_mode)
{
	XBYTE[0xB006] = gac_clock_table[tc_clock_mode][0];
	XBYTE[0xB006] = gac_clock_table[tc_clock_mode][0] | (1<<5);
	USER_DelayDTms(2);
	XBYTE[0xB008] = 2;	// PLLS_SWRSTN = 1
	XBYTE[0xB008] = 3;	// PLLS_MODE = 1(PLL clock output)
	change_clock_ratio(tc_clock_mode);
}

//! Change clock source from PLL to crystal.
static void pll_to_xtl(void)
{
	change_clock_ratio(CLOCK_MODE_XTL);
	XBYTE[0xB008] = 2;	// PLLS_SWRSTN = 1, PLLS_MODE = 0(PLL clock output)
}

U8 set_clock_mode(U8 mode)
{
	U8 tc_old_mode;

	if (gc_clock_mode == mode)
	{
		return gc_clock_mode;
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
	tc_old_mode = gc_clock_mode;
	gc_clock_mode = mode;
	gc_T0IntervalMSB = gac_clock_table[gc_clock_mode][6];
	gc_T0IntervalLSB = gac_clock_table[gc_clock_mode][7];

	return tc_old_mode;
}


/***************************************************************************
* NAME:         bit  SD_Card_Detect(void)
* DESCRIPTION:
***************************************************************************/
bit SD_Card_Detect(void)
{
#ifdef CAR_64
    P1|=0x02;
	XBYTE[0xB102]&=0xFD;
#elif defined(CAR_48)
    P1|=0x02;
	XBYTE[0xB102]&=0xFD;
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
 	XBYTE[0xB5e6]  = 0x00;	 //bulksram gated clock
	XBYTE[0xB522]  = 0x01;	 //USB2.0 UTMI clock mode set	
	XBYTE[0xB018]  = 0x01;		//USB clock select				//TONY
	XBYTE[0xB002] &= 0xFD;	 //bit 2   turn off USB_host_SW_RESET

	Syn_Reset();
	XBYTE[0xB018]  = 0x01;		//USB clock select				//TONY	
	XBYTE[0xB019]  = 0x01;   //host mode
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
	xdata	U8 sts;

	if(gc_CurrentCard == CURRENT_MEDIA_HOST)
	{
		if(gb_HostConnect==0)
		{
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
		if(sts) 
		{
			return 0;
		}
		else
		{
			gb_HostConnect = 0;
			return 1;
		}
	}
}



