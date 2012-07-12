#define Model	0	// 0=2635  1=2600(EVB)

#if Model==0
#define CAR_64 
#else
#define EVB_128
#endif

#define LCM_SIZE    0   // 0=128x64  1=96x32

#define C_AlarmDataSize		80	// Unit:1-Sector

#define	C_VolumeDefault			20	// 0~31

#define SPI_1M	//if not define ,then is's a 2M&&17Lang code

#ifdef SPI_1M
	#define 	LANG_ENG            0
	#define 	LANG_CHS            1
	#define     HZK_LANGUAGE        LANG_CHS  //set language
#else 
	#define LANG_CHS 0
	#define LANG_CHT 1
	#define LANG_ENG 2
	#define LANG_JAP 3
	#define LANG_KOR 4
	#define LANG_GER 5
	#define LANG_FRA 6
	#define LANG_ITA 7
	#define LANG_SPA 8
	#define LANG_SWE 9
	#define LANG_CZH 10
	#define LANG_HOL 11
	#define LANG_POR 12
	#define LANG_DAN 13
	#define LANG_POL 14
	#define LANG_RUA 15
	#define	LANG_TUR 16
	#define     HZK_LANGUAGE        LANG_CHS  //set language
#endif

#define     FM_ZI2020            1
#define     FM_AS6001            2
#define     FM_HY1418            2
#define     FM_AS6300            3

#define     FM_MODULE           FM_AS6300       //set FM_module  



