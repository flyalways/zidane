/*
 *                          Sunplus mMedia Inc.
 *
 *   (c) Copyright 2008~, Sunplus mMedia Inc., Hsinchu, Taiwan R.O.C.
 *                          All Right Reserved
 */
/*! \file 	const.h
 *	\brief	Constants defined here.
 */

#ifndef __CONST_H__
#define __CONST_H__

/*----- Define Interrupt Parameters ---------*/
#define      EN_ALL_INT                 0xC0
#define      EN_INT0                SET_BIT0
#define      EN_TIMER0              SET_BIT1
#define      EN_INT1                SET_BIT2
#define      EN_TIMER1              SET_BIT3
#define      DISABLE_INT0           CLR_BIT0
#define      DISABLE_INT1           CLR_BIT1
#define      DISABLE_INT2           CLR_BIT2
/*-----  Bit Setting ----------*/
/*#define      SET_BIT0               0x01
#define      SET_BIT1                   0x02
#define      SET_BIT2                   0x04
#define      SET_BIT3                   0x08
#define      SET_BIT4                   0x10
#define      SET_BIT5                   0x20
#define      SET_BIT6                   0x40
#define      SET_BIT7                   0x80
#define      CHK_BIT0                   0x01
#define      CHK_BIT1                   0x02
#define      CHK_BIT2                   0x04
#define      CHK_BIT3                   0x08
#define      CHK_BIT4                   0x10
#define      CHK_BIT5                   0x20
#define      CHK_BIT6                   0x40
#define      CHK_BIT7                   0x80
#define      CLR_BIT0                   0xFE
#define      CLR_BIT1                   0xFD
#define      CLR_BIT2                   0xFB
#define      CLR_BIT3                   0xF7
#define      CLR_BIT4                   0xEF
#define      CLR_BIT5                   0xDF
#define      CLR_BIT6                   0xBF
#define      CLR_BIT7                   0x7F
#define      CLR_BIT8                 0xFEFF
#define      CLR_BIT9                 0xFDFF
#define      CLR_BIT10                0xFBFF
#define      CLR_BIT11                0xF7FF
#define      CLR_BIT12                0xEFFF
#define      CLR_BIT13                0xDFFF
#define      CLR_BIT14                0xBFFF
#define      CLR_BIT15                0x7FFF*/
/*-------   LOGIC Define ------------*/
#define      TRUE                          1
#define      FALSE                         0
#define      LED_OFF                       1
#define      LED_ON                        0
/* ----------------------------------------------------------------------
   Timer Timing Parameters
   -------------------------------------------------------------------- */


#define      T25MSH        ( T25MS & 0xFF00 ) >> 8
#define      T25MSL        ( T25MS & 0x00FF )

#define      NOCARDSELECT               0x00

#define      OBF_XDCardModule           0x02
#define      SMCardModule               0x04
#define      MSCardModule               0x08
#define      SDMMCCardModule            0x10
#define      CFATACardModule            0x20
#define      IDEHDModule                0x40
#define      ATAPIModule                0x80

#define      OBF_XDCardDetect           0x02
#define      SMCardDetect               0x04
#define      MSCardDetect               0x08
#define      SDMMCCardDetect            0x10
#define      CFATACardDetect            0x20
#define      IDEHDDetect                0x40
#define      ATAPIDetect                0x80

#define      OBF_XDCardModuleClose      0xFD
#define      SMCardModuleClose          0xFB
#define      MSCardModuleClose          0xF7
#define      SDMMCCardModuleClose       0xEF
#define      CFATACardModuleClose       0xDF
#define      IDEHDModuleClose           0xBF
#define      ATAPIModuleClose           0x7F

#define      No_OBF_XDCardDetect        0xFD
#define      No_SMCardDetect            0xFB
#define      No_MSCardDetect            0xF7
#define      No_SDMMCCardDetect         0xEF
#define      No_CFATACardDetect         0xDF
#define      No_IDEHDDetect             0xBF
#define      No_ATAPIDetect             0x7F

#define      OBF_XDCardWritable         0xFD
#define      SMCardWritable             0xFB
#define      MSCardWritable             0xF7
#define      SDMMCCardWritable          0xEF
#define      CFATACardWritable          0xDF
#define      IDEHDWritable              0xBF
#define      ATAPIWritable              0x7F

#define      OBF_XDCardWP               0x02
#define      SMCardWP                   0x04
#define      MSCardWP                   0x08
#define      SDMMCCardWP                0x10
#define      CFATACardWP                0x20
#define      IDEHDWP                    0x40
#define      ATAPIWP                    0x80


#define TEST_ALL        0x01
#define ASK_PARA	    0x02
#define READ_BLOCK      0x03
#define WRITE_BLOCK     0x04
#define ERASE_BLOCK     0x05
#define READ_FullPage   0x06
#define WRITE_FullPage  0x07
#define	WRITE_FONTPage	0xE7
#define Update_FW       0x08
#define Reset_uP        0x09
#define Verify_FW	    0x10
#define Erase_ROM       0x11
#define Write_ROM	    0x12
#define Read_RAM	    0x13
#define Write_RAM	    0x14
#define IROM_SEL	    0x15
#define PROM_WAVE 	    0x16
#define DNLD_IDT	    0x17
#define LOAD_MAP	    0x18
#define READ_SPARE 	    0x19
#define WRITE_SPARE	    0x20
#define DNLD_FMT	    0x21
#define CARD_STATUS	    0x22
#define LOCK_CARD	    0x23
#define UNLOCK_CARD	    0x24
#define SET_PWD		    0x25
#define FMT_REQ		    0x26
#define Eject_Card	    0x27
#define Card_IN		    0x28
#define INIT_SYS	    0x29

#define WRITE_PWD	    0x30
#define READ_PWD	    0x31
#define WRITE_ALLPAGE   0x32
#define UPDATE_CHECK	0x33
#define TEST_PASS	    0x34
#define Magic_Lock	    0x35
#define Magic_Unlock	0x36
#define PartI_Chanage   0x37
#define PartII_Chanage  0x38

#define Unknow_Flash_Init 0x40
#define BlankCheck_Block 0x41 
#define Block_Mark_Good_Tag 0x42 
#define Build_Mapping_Table 0x43  
#define Get_CIS_USB_Block 0x44
 
#define Erase_Bank_Block		0x50
#define Write_Factor			0x51
#define Read_Factor			    0x52

#define READ_MS_ATTRIB  0x60

#define SetCtrlReg	0x80
#define ReadCtlReg	0x81
#define WriteSingle     0x82
#define WriteMultiple	0x83
#define	ReadSingle	0x84
#define ReadMultiple	0x85
#define DOS_READ	0x86
#define DOS_WRITE   0x87
#define Write_Program_Ram 0x8A
#define Read_Program_Ram  0x88
#define Jump_To_Bank	  0x89
#define Change_SysCLK     0x90
#define RESET_FLASH       0x91

#define RESETI32	0x99

#define	ROM_CHECKI	0xAA
#define	ROM_CHECKII	0xBB
#define	ROM_CHECKIII	0xCC
#define SET_LOCK	0xDD
#define SET_UNLOCK	0xEE
#define USB_UN_PLUG	0xFE
#define Vendor_RESET	0xFF
// Error Message 
#define ID_NOT_FND      0x10
#define SET_ABRT	0x04

#define      NormalResponseData            1
#define      ModeSenseResponseData         0

/*-------- USB control define -------------*/
#define      BUF_CTL_FIFOcfg    0x00                  // BUF_CTL.2 = '0'

#endif //!__CONST_H__




