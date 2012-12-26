///////////////////////////////////////////////////////////////////////////////
//
// Central control and definition for LCM, UI and FM
//
///////////////////////////////////////////////////////////////////////////////
#ifndef _UI_CONFIG_H_
#define _UI_CONFIG_H_
#include "SPDA2K.h"

#define SERIAL_MONO     FEATURE_ON      // Flag for lcd type
#define LCM_SIZE        2               // 0=128x64, 1=96x32, 2=160x120
#define Model	        1               // 0: 64 pins, 1: 128 pins in evaluation board.
#define LCM_TEST_ONLY   FEATURE_OFF
#define LCM_TEST_INIT   FEATURE_OFF
#define UI_TEST_ONLY    FEATURE_OFF     // Infinite function to test UI implementation.

// REVISIT!!!
// I should not put below flags here. But it is kind of convenient to put all
// the flag settings together. So...
#define I2C_SIM_TEST_PIN    FEATURE_OFF
#define TCA8418_TEST        FEATURE_OFF

#if Model==0
#define CAR_64  //S+input set build target
#else
#define EVB_128
#endif

#define     LCM_ID3HZK_CHS      15//0
#define     LCM_ID3HZK_CHT      16//1 
#define     LCM_ID3HZK_ENG       0//2 
#define     LCM_ID3HZK_JAP      18//3 
#define     LCM_ID3HZK_KOR      17//4 
#define     LCM_ID3HZK_GER       0//5 
#define     LCM_ID3HZK_FRA       0//6 
#define     LCM_ID3HZK_ITA       0//7 
#define     LCM_ID3HZK_SPA       0//8 
#define     LCM_ID3HZK_SWE       0//9 
#define     LCM_ID3HZK_JIE       1//10 
#define     LCM_ID3HZK_HOL       0//11 
#define     LCM_ID3HZK_POR       0//12 
#define     LCM_ID3HZK_DAN       0//13 
#define     LCM_ID3HZK_POL       1//14 
#define     LCM_ID3HZK_RUA       4//15 
#define     LCM_ID3HZK_TUR       0//sunzhk add 

#define     HZK_LANGUAGE        2//0~16 ¹²17¹ú  //set language	 gc_language_offset

#define     FM_ZI2020            1
#define     FM_AS6001            2
#define     FM_HY1418            2
#define     FM_AS6300            3
#define     FM_QN8027            4

#define     FM_MODULE           FM_AS6300       //set FM_module  

#endif