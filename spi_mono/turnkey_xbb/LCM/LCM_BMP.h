
#define LCM_IsChar 0
#define LCM_IsWord 1


#ifdef EVB_128
	sbit LCM_BACKLIGHT		= P1^0;
	sbit LCM_CS				= P1^1;
	sbit LCM_RESET			= P1^2;
	#define	LCM_RESET_HI	LCM_RESET = 1				// P1[2] = 1;
	#define	LCM_RESET_LO	LCM_RESET = 0				// P1[2] = 0;
	#define LCM_CS_HI		LCM_CS = 1					// P1[1] = 1
	#define LCM_CS_LO		LCM_CS = 0					// P1[1] = 0
	#define LCM_A0P_HI		XBYTE[0xB423] |= 0x04		// NAND_ALE = 1
	#define LCM_A0P_LO		XBYTE[0xB423] &= ~0x04		// NAND_ALE = 0
#else
	sbit LCM_BACKLIGHT		= P1^0;
	#define	LCM_RESET_HI	XBYTE[0xB401] |= 0x02		// FM[1] = 1
	#define	LCM_RESET_LO	XBYTE[0xB401] &= ~0x02		// FM[1] = 0
//	#define LCM_CS_HI		XBYTE[0xB423] |= 0x08		// NAND_CLE = 1
//	#define LCM_CS_LO		XBYTE[0xB423] &= ~0x08		// NAND_CLE = 0
	#define LCM_CS_HI		XBYTE[0xB423] |= 0x02		// NAND_CLE = 1
	#define LCM_CS_LO		XBYTE[0xB423] &= ~0x02		// NAND_CLE = 0
	#define LCM_A0P_HI		XBYTE[0xB423] |= 0x04		// NAND_ALE = 1
	#define LCM_A0P_LO		XBYTE[0xB423] &= ~0x04		// NAND_ALE = 0
#endif
//-------------------------------------------------------------
#define LCM_write_command(tc_Data)  do {\
	XBYTE[0xB400]=1; \
	FLASH_REG[0x00]=0x00; \
	LCM_A0P_LO; LCM_CS_LO; DEVICE_REG[0x20] = tc_Data; \
	LCM_CS_HI; LCM_A0P_HI; \
	FLASH_REG[0x00]=0x01; \
	} while(0)

#define LCM_write_data(tc_Data)  do {\
	U8	x=tc_Data;\
	if(gb_LCM_InverseDisp) x=~tc_Data;\
	XBYTE[0xB400]=1; \
	FLASH_REG[0x00]=0x00; \
	LCM_A0P_HI; LCM_CS_LO; DEVICE_REG[0x20]=x; LCM_CS_HI; \
	FLASH_REG[0x00]=0x01; \
	} while(0)








