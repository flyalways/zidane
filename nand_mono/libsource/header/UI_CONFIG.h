#define Model	0

#if Model==0
#define CAR_64  //S+input set build target
#else
#define EVB_128
#endif

#define LCM_SIZE    0   // 0=128x64  1=96x32


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

