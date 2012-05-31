/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	lcm.h
 *	\brief	LCM related definitions.
 *
 *	LCM control pins and preprocessor definitions
 */

#ifndef __LCM_H__
#define __LCM_H__

#include "..\LCM\model_define.h"

//=================LCM Strobe Setting====================================================
#define D_LCM_Strobe   0x00 //0x00//0x11		//S+input

#ifdef EVB_128	// For EVB only
	#define	LCD_BL_STS_ON	1
	#define	LCD_BL_STS_OFF	0
#else
	#define	LCD_BL_STS_ON	0
	#define	LCD_BL_STS_OFF	1
#endif

sbit LCD_BACKLIGHT=	P1^0;

#define LCD_BACKLIGHT_ON	LCD_BACKLIGHT = LCD_BL_STS_ON
#define LCD_BACKLIGHT_OFF	LCD_BACKLIGHT = LCD_BL_STS_OFF

#endif // !__LCM_H__
