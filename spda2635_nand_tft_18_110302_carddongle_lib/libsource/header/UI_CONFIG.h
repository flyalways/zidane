#ifndef UI_CONFIG__
#define UI_CONFIG__

//#define CAR_64  //S+input set build target
//#define EVB_128
#define CAR_48

#define	USB_HOST 1

#define     LCM_ID3HZK_CHS       0 
#define     LCM_ID3HZK_CHT       1 
#define     LCM_ID3HZK_ENG       2 
#define     LCM_ID3HZK_JAP       3 
#define     LCM_ID3HZK_KOR       4 
#define     LCM_ID3HZK_GER       5 
#define     LCM_ID3HZK_FRA       6 
#define     LCM_ID3HZK_ITA       7 
#define     LCM_ID3HZK_SPA       8 
#define     LCM_ID3HZK_SWE       9 
#define     LCM_ID3HZK_JIE      10 
#define     LCM_ID3HZK_HOL      11 
#define     LCM_ID3HZK_POR      12 
#define     LCM_ID3HZK_DAN      13 
#define     LCM_ID3HZK_POL      14 
#define     LCM_ID3HZK_RUA      15 
#define     LCM_ID3HZK_TUR      16 


#define     HZK_LANGUAGE        LCM_ID3HZK_ENG  //set language

#define     FM_ZI2020            1
#define     FM_AS6001            2
#define     FM_HY1418            2
#define     FM_AS6300            3
#define		FM_QN8027			 4
#define		FM_BK1085			 5

#define     FM_MODULE           FM_QN8027       //set FM_module  

//================DisplayType DEFINE===============//
#define     TFT_18H
//#define     TFT_18V
//#define     TFT_15H


//================LCD Type DEFINE==================//
//#define		LD5414C								//丽电视角不对屏

//#define		SL9163								//三龙9163屏
//#define		SL8210							//索浪  8210
//#define       CS1783
//#define       	LZ7669
//#define		S6D0134
//#define		S6D0133
//#define		S6B33B9
//#define	S6B33BF
//#define		S6B33BC
//#define    S6B33B9_New
#define		HD66766
//#define	HD66773R
//#define		ST7735R                 //调货
#define Contrast_Set   //是否需要对比度菜单

#endif //UI_CONFIG__