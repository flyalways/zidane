#ifndef	_UI_FUNCINCLUDE_H_
#define	_UI_FUNCINCLUDE_H_

#define BYTE_0(x)  (*((unsigned char*)&x+3))
#define BYTE_1(x)  (*((unsigned char*)&x+2))
#define BYTE_2(x)  (*((unsigned char*)&x+1))
#define BYTE_3(x)  (*((unsigned char*)&x))

#define	U8_bit0		0x01
#define	U8_bit1		0x02
#define	U8_bit2		0x04
#define	U8_bit3		0x08
#define	U8_bit4		0x10
#define	U8_bit5		0x20
#define	U8_bit6		0x40
#define	U8_bit7		0x80


#define	C_QuarSec_TEvent		0x01
#define	C_HalfSec_TEvent		0x02
#define	C_Sec_TEvent			0x04


// Unicode mapping tables for character coding standatds
#define ISOIEC_8859_1_TAB_SECT	1
#define ISOIEC_8859_2_TAB_SECT	2
#define ISOIEC_8859_3_TAB_SECT	3
#define ISOIEC_8859_4_TAB_SECT	4
#define ISOIEC_8859_5_TAB_SECT	5
#define ISOIEC_8859_6_TAB_SECT	6
#define ISOIEC_8859_7_TAB_SECT	7
#define ISOIEC_8859_8_TAB_SECT	8
#define ISOIEC_8859_9_TAB_SECT	9	  //tuerqi
#define ISOIEC_8859_10_TAB_SECT	10
#define ISOIEC_8859_11_TAB_SECT	11
#define ISOIEC_8859_13_TAB_SECT	12
#define ISOIEC_8859_14_TAB_SECT	13
#define ISOIEC_8859_15_TAB_SECT	14
#define ISOIEC_8859_16_TAB_SECT	15

#define CP1250_TAB_SECT			16
#define CP1251_TAB_SECT			17
#define CP1252_TAB_SECT			18
#define CP1253_TAB_SECT			19
#define CP1254_TAB_SECT			20
#define CP1255_TAB_SECT			21
#define CP1256_TAB_SECT			22
#define CP1257_TAB_SECT			23
#define CP1258_TAB_SECT			24
#define CP874_TAB_SECT			25
#define CP932_TAB_SECT			26	// Shift JIS
#define CP936_TAB_SECT			154	// GBK
#define CP949_TAB_SECT			282 // for Korean
#define CP950_TAB_SECT			410	// Big5

#define SBCS	0
#define DBCS	1


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
#define LANG_TUR 16



#endif
