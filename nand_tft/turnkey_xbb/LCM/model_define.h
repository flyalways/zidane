//****************************************************
//* Product Model Definition
//****************************************************
#ifndef __MODEL_DEFINE_H__
#define __MODEL_DEFINE_H__

#include "..\LCM\port_define.h"

//===============================
//#define SUPPORT_2310//no REC,Voice,FM//huiling.gu add to support 2310 090217

//===== model 'n mechanism definition ===== 
#define EVB		//128 pin EVB     //S+input
//#define EVB_2320	// 100-pin EVB
//#define EVB_2300	// 64-pin EVB

//#define OLD_KEY_SAMPLING
#define	SUPPORT_BIG_BAT_DISP	//supprot big battery disp when USB-charge

#define SUPPORT_FileNumbWidth_4	//extend FileIndex/FileTotalNum's width from 3 to 4
//===== color panel module definition =====
//S+input
//#define		ST106B
//#define		KFM225B01
//#define		GPM834B0
//#define		SPFD54124A //160*128(old LCM)
//#define		SPFD54124C //(LCM:GPM897A0)160*128 (for EVB)
//#define		GPM767B0 //320*240 (for EVB)
//#define		GPM773A0  //320*240 (for EVB)odin add080826

//===== suspend mode support =====
#if defined(EVB) || defined(EVB_2320) || defined(EVB_2300)
#define OLD_KEY_SAMPLING
#define SUPPORT_SUSPEND_MODE   //S+input
#define C_USB_Release_Key   C_Key_Mode  // support longkey  release usb back to charge mode  
#endif


//===== Color panel control GPIO definition =====
#ifndef EVB_2320 
//	#define 	LCM_CS 		DbgP11
#endif

//(JC)I0210 for 100pin EVB
#ifdef CAR_64
	#define 	LCM_CS_HI	DbgP13=1//DEVICE_REG[0x23] |= 0x02    // FMGPIO9 = 1; for 100pin EVB	//DbgP13=1
	#define 	LCM_CS_LO	DbgP13=0//DEVICE_REG[0x23] &= ~0x02	// FMGPIO9 = 0; for 100pin EVB	//DbgP13=0
#elif defined(CAR_48)
	#define 	LCM_CS_HI	DbgP14=1//DbgP13=1//DEVICE_REG[0x23] |= 0x02    // FMGPIO9 = 1; for 100pin EVB	//DbgP13=1
	#define 	LCM_CS_LO	DbgP14=0//DbgP13=0//DEVICE_REG[0x23] &= ~0x02	// FMGPIO9 = 0; for 100pin EVB	//DbgP13=0
#else
	#define 	LCM_CS 		DbgP11

	#define 	LCM_CS_HI	LCM_CS = 1
	#define 	LCM_CS_LO	LCM_CS = 0
#endif

#define 	LCM_A0_HI	DEVICE_REG[0x23] |= 4	// A0 = 1 for data transmission
#define 	LCM_A0_LO	DEVICE_REG[0x23] &= ~4	// A0 = 0 for command transmission

#ifdef CAR_48
	#define LCM_RESET_HI	 XBYTE[0xB031] |= 0x04;		//home
	#define LCM_RESET_LO	 XBYTE[0xB031] &= ~0x04;	
#endif

//===== Color panel display dimension[Width, Height] definition =====
#ifdef SPFD54124A
#define DISPLAY_WIDTH	128	
#define DISPLAY_HEIGHT	160
#elif defined(GPM834B0)
#define DISPLAY_WIDTH	176
#define DISPLAY_HEIGHT	220
#elif defined(GPM767B0)
#define DISPLAY_WIDTH	240
#define DISPLAY_HEIGHT	320
#elif defined(GPM773A0)
#define DISPLAY_WIDTH	240
#define DISPLAY_HEIGHT	320
#elif defined(SPFD54124C)
#define DISPLAY_WIDTH	128	
#define DISPLAY_HEIGHT	160
#endif
//===== Supporting Decoders Definition =====
#define Support_LoseLessDecoder //(JC)main(MP3 'n WMA) + LL(APE 'n FLAC)
//#define Support_LoseLess_n_OGGv //(JC)main(MP3 'n WMA) + LL(APE 'n FLAC) + OGG vorbis

//#define SUPPORT_RTC
	
#endif // !__MODEL_DEFINE_H__

