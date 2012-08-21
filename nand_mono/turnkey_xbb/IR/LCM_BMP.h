#ifndef _LCM_BMP_H_
#define _LCM_BMP_H_

#include "SPDA2K.H"
#include "lcm_bmp_driver.h" // This is ugly.

/*
sbit LCM_CS				= P1^1; //P1^1;
sbit LCM_RESET			= P1^2;
*/
sbit LCM_CS				= P1^4;
#define LCM_RESET_HI	 XBYTE[0xB031] |= 0x04;		//home
#define LCM_RESET_LO	 XBYTE[0xB031] &= ~0x04;
sbit LCM_BACKLIGHT		= P1^0;

#define LCM_IsChar 0
#define LCM_IsWord 1


#define LCM_A0P_HI		DEVICE_REG[0x23] |= 0x04	// FMGPIO4 = 1
#define LCM_A0P_LO		DEVICE_REG[0x23] &= ~0x04	// FMGPIO4 = 0

#ifndef EVB_128
	#define LCM_CS_HI		LCM_CS = 1 //DEVICE_REG[0x23] |= 0x02	// FMGPIO9 = 1
	#define LCM_CS_LO		LCM_CS = 0 //DEVICE_REG[0x23] &= ~0x02	// FMGPIO9 = 0
#else
	#define LCM_CS_HI		LCM_CS = 1
	#define LCM_CS_LO		LCM_CS = 0
#endif

//-----------------------------------------------------------------------------
// Definitions for SPI interface LCM.
// 
// RESETB:  P1^2
// CSB:     FMGPIO11
// A0:      FMGPIO4
// CLK:     P1^1
// SI:      FMGPIO17
// BACKLB:  P1^0
//-----------------------------------------------------------------------------
sbit LCM_BLB  =   P1^0;
sbit SPI_CLK  =   P1^1;
sbit LCM_RSTB =   P1^2;

#define LCM_BL_SPI_LO       LCM_BLB = 0
#define LCM_BL_SPI_HI       LCM_BLB = 1
#define LCM_RSTB_SPI_LO     LCM_RSTB = 0
#define LCM_RSTB_SPI_HI     LCM_RSTB = 1
#define LCM_CSB_SPI_LO      XBYTE[0xB402] &= ~0x08  // FMGPIO11
#define LCM_CSB_SPI_HI      XBYTE[0xB402] |= 0x08
#define LCM_A0_SPI_CMD      XBYTE[0xB401] &= ~0x10  //FMGPIO4
#define LCM_A0_SPI_DATA     XBYTE[0xB401] |= 0x10

#define SPI_CLK_CLR         SPI_CLK = 0
#define SPI_CLK_SET         SPI_CLK = 1
#define SPI_SI_CLR          XBYTE[0xB403] &= ~0x02
#define SPI_SI_SET          XBYTE[0xB403] |= 0x02

// Definition for different size LCM
typedef enum
{
	LCM_SIZE_128X64=0,
	LCM_SIZE_96X32=1,
	LCM_SIZE_160X120=2,
} lcm_size_t;

// Use new driver for serial mono lcd.
// In order to use the dev package as much as possible without too big change,
// below 3 functions are composed to replace the original ones.
#if (SERIAL_MONO == FEATURE_ON)

#define LCM_write_command(tc_Data)  lcm_write_command(tc_Data)
#define LCM_write_data(tc_Data)     lcm_write_data(tc_Data)
#define LCM_set_address(page, col)  lcm_set_addr(page, col)
#define LCM_clear()                 lcm_clear_screen()

#else

#define LCM_write_command(tc_Data)  do {\
	XBYTE[0xB400]=1; \
	FLASH_REG[0x00]=0x00; \
	LCM_A0P_LO; LCM_CS_LO; DEVICE_REG[0x20] = tc_Data; \
	LCM_CS_HI; LCM_A0P_HI; \
	FLASH_REG[0x00]=0x01; \
	} while(0)

#define LCM_write_data(tc_Data)  do {\
	XBYTE[0xB400]=1; \
	FLASH_REG[0x00]=0x00; \
	LCM_A0P_HI; LCM_CS_LO; DEVICE_REG[0x20] = tc_Data; LCM_CS_HI; \
	FLASH_REG[0x00]=0x01; \
	} while(0)

#endif // #if (SERIAL_MONO == FEATURE_ON)

#define	LCM_ReverseOffset	36

#if (SERIAL_MONO == FEATURE_OFF)  // Re-#define to mine
extern void LCM_set_address(U8 page, U8 col);
#endif

extern void LCM_Display(void);
extern void LCM_ShowCompanyLogo(void);
//extern void LCM_ShowLineIn(void);
extern void LCM_ShowFMIcon(void);
extern void LCM_ShowSDIcon(void);
extern void LCM_ShowNandIcon(void);
extern void LCM_ShowUSBIcon(void);
extern void LCM_ShowFrequency(void);
extern void LCM_ShowPAUSE(void);
extern void LCM_ShowVOL(void);
extern void LCM_ShowFrequency_L(void);	// 12x32
extern void LCM_ShowPlayMode(void);	// Big icon
extern void LCM_ShowPlayModeIcon(void);
extern void LCM_ShowEQ(void);
extern void LCM_Show_NOMP3FILE(void);
extern void LCM_ShowERROR(void);
extern void LCM_ShowIRNum(void);
extern void LCM_ShowIRNum_PicKSong(void);
extern void LCM_ShowMediaType(void);
extern void LCM_ShowEQIcon(void);
extern void LCM_ShowBitRate(void);
//extern void LCM_ShowBitRateIcon(void);
extern void LCM_ClearScreen(void);
extern void LCM_init_process(void);
extern void LCM_Init(void);
#if (SERIAL_MONO == FEATURE_OFF)  // Re-#define to mine
extern void LCM_clear(void); // re-#define this function to mine
#endif
extern void LCM_clear_dir(void);  //20090216 chiayen add 
extern void LCM_ShowSongNumber(void);
extern void LCM_ShowSongNumber_dir(void);
extern void RollFileName();
extern void LCM_Disp_PCConnect();
extern void LCM_Disp_PC();
extern void LCM_Disp_MyMP3Logo();
extern void LCM_Disp_USBLine();
extern void LCM_Disp_XBMPArray_USB(U8 *Condition,U8 *Data);
extern void LCM_Disp_USBRWLogo(U8 Condition);	//USB 0:(Read); 1:(Write)
extern void LCM_erase_one_page(U8 page);
extern void LCM_ShowPlayTime(U16 time);
extern void LCM_ShowChar12x32(U8 Num,U8 tc_column);

extern void LCM_ShowChar8x16(U8 Num,U8 tc_column);
extern void LCM_ShowChar8x16_L2(U8 Num,U8 tc_column);
extern void LRC_LCMDisplayString(U8 *tpc_DataBuf, U16 tw_nByte,U8 tc_RowAddr);
extern void LRC_LCMFillColumn(U8 tc_Page,U8 tc_CurrentColumn,U8 tc_TotalColumn);
extern void LCM_disp_HZKCharBMP(U8 Page, U8 Column,U8 *BMPdataBuf, U8 CharOrWord,U8 reverse);
extern U8 LCM_Disp_FileName(U8 *DataBuf,U8 tc_ISNOrUnicode, U8 nByte,U8 DispOnOff);
extern U8 LCM_UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tbt_UnicodeOrISN);


extern void LCM_ShowWait();

#endif