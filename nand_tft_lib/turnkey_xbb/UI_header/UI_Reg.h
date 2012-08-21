#ifndef _reg759_
#define _reg759_

// Register define

//////////////////////////////////////////////////////////////////////
// GLOBAL REGISTER
//////////////////////////////////////////////////////////////////////
#define		REG_DSPWakeUp			0xB001
#define		REG_SWRESET			0xB002
#define		REG_SUSPEND			0xB003
#define		REG_TRESUME			0xB004
#define		REG_PLLCLK			0xB006
#define		REG_PLLMASKSEL			0xB007

#define		REG_PLLFREQADJUST		0xB008

#define		REG_CLK48MEN			0xB010
#define		REG_CLK				0xB011
#define		REG_PHASECLKEN			0xB013

#define		REG_SELECTPRB			0xB014
#define		REG_USBCKREADY			0xB015
#define		REG_USBSLUMBER			0xB016
#define		REG_USBOPMUX			0xB017

#define		REG_UTMICKSEL			0xB018
#define		REG_USBMODE			0xB019

#define		REG_MUX_PIN			0xB01A
#define		REG_MUX_PIN2			0xB01B
#define		REG_DSPLMSRAM			0xB01C
#define		REG_DSPTESTTEN			0xB01D

#define		REG_FREQCHANGE			0xB020

#define		REG_CPUFREQ			0xB024
#define		REG_PERPHECLK			0xB025
#define		REG_DSPFREQ			0xB026

#define		REG_SPDIFFREQ			0xB027

#define		REG_GPIOO_6_0			0xB030
#define		REG_GPIOOE_6_0			0xB038
#define		REG_GPIOI_6_0			0xB040
#define		REG_BONDGPIOO3_0		0xB042
#define		REG_BONDGPIOOE3_0		0xB043
#define		REG_BONDGPIOI3_0		0xB044
#define		REG_GPIOREVT6_0			0xB048
#define		REG_GPIOREVT10_7		0xB049
#define		REG_GPIOREVT18_11		0xB04A
#define		REG_GPIORINTEN6_0		0xB050
#define		REG_GPIORINTEN10_7		0xB051
#define		REG_GPIORINTEN18_11		0xB052
#define		REG_GPIOFINTEN6_0		0xB058
#define		REG_GPIOFINTEN10_7		0xB059
#define		REG_GPIOFINTEN18_11		0xB05A

#define		REG_SARFS			0xB05E
#define		REG_SARFREQ			0xB05F

#define		REG_SAREN			0xB060
#define		REG_SARRD			0xB061
#define		REG_SARD			0xB062

#define		REG_OTPDO			0xB063
#define		REG_OTPADDR			0xB064
#define		REG_OTPCEB			0xB065
#define		REG_RTCSIEN			0xB066
#define		REG_RTCFREQ			0xB067
#define		REG_RTCADDR			0xB068
#define		REG_RTCWRITEDATA		0xB069
#define		REG_RTCREADDATA			0xB06A
#define		REG_RTCREADY			0xB06B
#define		REG_RTCPROBE			0xB06C
#define		REG_RTCINTERRUPT		0xB06D

#define		REG_TIMER7_0			0xB070
#define		REG_TIMER15_8			0xB071
#define		REG_TIMER23_16			0xB072
#define		REG_TIMER_CNT			0xB073
#define		REG_TIMER			0xB074

#define		REG_TICKPERIOD7_0		0xB075
#define		REG_TICKPERIOD15_8		0xB076

#define		REG_GPIOFEVT6_0			0xB078
#define		REG_GPIOFEVT10_7		0xB079
#define		REG_GPIOFEVT18_11		0xB07A

#define		REG_CHARGERCOMPLETE		0xB080
#define		REG_CHARGERTIMEOUT		0xB081
#define		REG_CHARGEREN			0xB082
#define		REG_PADSET0			0xB088
#define		REG_PADSET1			0xB089
#define		REG_PADSET2			0xB08A
#define		REG_PADSET3			0xB08B

#define		REG_DCDCPOWER_READY		0xB090
#define		REG_DCDCPWM_EN			0xB091
#define		REG_DCDCEN			0xB092
#define		REG_DCDC_DCV			0xB093
#define		REG_PWMNSNE_SEL			0xB094
#define		REG_PWMPSPE_SEL			0xB095

#define		REG_DCDCPWRRDY			0xB090
#define 	REG_DCDCPWMEN			0xB091
#define 	REG_DCDCEN				0xB092
#define 	REG_DCDCVOLSEL			0xB093
#define		REG_DCDCPWMNSELL		0xb094 
#define		REG_DCDCPWMPSELL		0xB095
#define		REG_DCDCPWMNSELH		0xB096
#define		REG_DCDCPWMPSELH		0xB097

#define		REG_IOTRAP0			0xB0B1
#define		REG_IOTRAP1			0xB0B2
#define		REG_PROBE			0xB0B3

#define		REG_FMCONFIG			0xB0B4

#define		REG_TIME_INT			0xB0C0

#define		REG_CTIMERSET			0xB0C1
#define		REG_CTIMERSTARTVAL7_0		0xB0C2
#define		REG_CTIMERSTARTVAL15_8		0xB0C3
#define		REG_CTIMERSTARTVAL23_16		0xB0C4
#define		REG_CTIMERSTARTVAL31_24		0xB0C5
#define		REG_CTIMERSTARTVAL39_32		0xB0C6
#define		REG_CTIMERCOUNTER7_0		0xB0C7
#define		REG_CTIMERCOUNTER15_8		0xB0C8
#define		REG_CTIMERCOUNTER23_16		0xB0C9
#define		REG_CTIMERCOUNTER31_24		0xB0CA
#define		REG_CTIMERCOUNTER39_32		0xB0CB

#define		REG_TIME_EN			0xB0D0
#define		REG_SRAM_TEST			0xB0E0
#define		REG_PROBESEL3_0			0xB0E1
//#define		REG_TSELECT				0x50E2
#define		REG_CKPHASE7_0			0xB0E3
#define		REG_CKPHASE15_8			0xB0E4
#define		REG_CKPHASE19_16		0xB0E5

#define		REG_SPDIFCLKPHASE		0xB0E6
#define		REG_HOSTCLKPHASE		0xB0E7

#define		REG_RVCONTROL7_0		0xB0E8
#define		REG_RVCONTROL15_8		0xB0E9

#define		REG_USBTIME7_0			0xB0EA
#define		REG_USBTIME15_8			0xB0EB

#define		REG_CTRLER_GCLKEN7_0		0xB0F0
#define		REG_CTRLER_GCLKEN12_8		0xB0F1
#define		REG_DSPCTRLER_GCLKDIS		0xB0F2
#define		REG_REVID7_0			0xB0FF

//////////////////////////////////////////////////////////////////////
// CPU CONTROL REGISTERS
//////////////////////////////////////////////////////////////////////
#define		REG_SRAM_SET			0xB100
#define		REG_SHADOWMAP7_0		0xB101
#define		REG_P1OESEL  			0xB102
#define		REG_P3OESEL 			0xB103
#define		REG_INTERRUPT			0xB104

#define		REG_POWERDOWN			0xB105
#define		REG_EXTROMIODIS			0xB106

//#define		REG_HSRAM1KDATA			0xB110
//#define		REG_HSRAM1KMODE			0xB111
#define		REG_SRAMDMASRCIDX7_0		0xB112
#define		REG_SRAMDMASRCIDX15_8		0xB113
#define		REG_SRAMDMADSTIDX7_0		0xB114
#define		REG_SRAMDMADSTIDX15_8		0xB115
//#define		REG_HSRAM1KIDX7_0		0xB112
//#define		REG_HSRAM1KIDX12_8		0xB113
//#define		REG_HSRAM1KWRIDX7_0		0xB1A0
//#define		REG_HSRAM1KWRIDX9_8		0xB1A1
//#define		REG_HSRAM1KRDIDX7-0		0xB1A2
//#define		REG_HSRAM1KRDIDX9-8		0xB1A3

#define		REG_MCUPROGRAMSAPCE		0xB120
#define		REG_MCURESET			0xB121
#define		REG_PROGRAMSPACESWITCH		0xB122
#define		REG_INTROMBANK			0xB123
#define		REG_EXTROMTOINTROM		0xB124
#define		REG_IRAM_GCLKEN			0xB125
#define		REG_SRAMBISTMODE		0xB128
#define		REG_SRAMBISTFINISH		0xB129
#define		REG_SRAMBISTFAIL		0xB12A
#define		REG_SRAMBISTERRMAP		0xB12B
#define		REG_ADDRSHUFFLEDIS		0xB170

#define		REG_SRAMDMARDIDX7_0		0xB1A0
#define		REG_SRAMDMARDIDX15_8		0xB1A1
#define		REG_SRAMDMAWRIDX7_0		0xB1A2
#define		REG_SRAMDMAWRIDX15_8		0xB1A3
#define		REG_CPUPRBSEL3_0		0xB1E0
//#define		REG_DUMPPCNT3_0			0xB1E1

//////////////////////////////////////////////////////////////////////
// DSP BUF REGISTERS
//////////////////////////////////////////////////////////////////////
//#define		REG_DSPBUFDATA			0xB210
//#define		REG_DSPPMDATA			0xB211
//#define		REG_DSPBUFIDX7_0		0xB212
//#define		REG_DSPBUFIDX11_8		0xB213
#define		REG_DSPDMIDX7_0			0xB217
#define		REG_DSPDMIDX12_8		0xB218

#define     REG_DSPACCESSMODE       0xB216
#define		REG_DSPBCLR				0xB216
#define		REG_DSPBADDRS			0xB217
#define		REG_DSPMODE				0xB218
#define		REG_DSPPMIDX7_0			0xB219
#define		REG_DSPPMIDX13_8		0xB21A

#define		REG_DSPPMIDX7_0			0xB219
#define		REG_DSPPMIDX12_8		0xB21A

#define		REG_DSPIMIDX7_0			0xB21B
#define		REG_DSPIMIDX13_8		0xB21C

#define		REG_DSPDMOFFSET			0xB21D
#define		REG_DSPPMOFFSET			0xB21E
#define		REG_DSPIMOFFSET			0xB21F

#define		REG_DSPBUFREQ			0xB220
#define		REG_DSPBUFACK			0xB221

#define		REG_DSPBUFBIPI7_0		0xB2A0
#define		REG_DSPBUFBIPI11_8		0xB2A1
#define		REG_DSPBUFBIPO7_0		0xB2A2
#define		REG_DSPBUFBIPO11_8		0xB2A3

#define		REG_DSPPMIP7_0			0xB2A4
#define		REG_DSPPMIP13_8			0xB2A5
#define		REG_DSPPMOP7_0			0xB2A6
#define		REG_DSPPMOP13_8			0xB2A7
#define		REG_DSPIMIP7_0			0xB2A8
#define		REG_DSPIMIP13_8			0xB2A9
#define		REG_DSPIMOP7_0			0xB2AA
#define		REG_DSPIMOP13_8			0xB2AB


//////////////////////////////////////////////////////////////////////
// DMA CONTROLLER REGISTERS
//////////////////////////////////////////////////////////////////////
#define		REG_DMADATA			0xB300
#define		REG_DMA_SEL			0xB301
#define		REG_DMASIZE7_0			0xB302
//#define		REG_DMASIZE9_8			0xB303
#define		REG_DMASIZE10_8			0xB303
#define		REG_DMA_STATE			0xB304

//#define		REG_CTRREGISTER1		0xB305
//#define		REG_CTRREGISTER2		0xB306

#define		REG_DMA_MODE			0xB310
#define		REG_FATCODE7_0			0xB311
#define		REG_FATCODE15_8			0xB312
#define		REG_HITCNT7_0			0xB313
#define		REG_HITCNT15_8			0xB314
#define		REG_FASTDMA7_0  		0xB320

#define		REG_FASTDMA9_8  		0xB321
#define		REG_CHECKSUMCLR  		0xB330
#define		REG_CHECKSUM31_24  		0xB331
#define		REG_CHECKSUM23_16  		0xB332
#define		REG_CHECKSUM15_8  		0xB333
#define		REG_CHECKSUM7_0  		0xB334
#define		REG_DSPDMAMODE			0xB340
#define		REG_16BITDMA	  		0xB341
#define		REG_DMAINCMT7_0  		0xB350
#define		REG_DMAINCNT15_8  		0xB351
#define		REG_DMAOUTCNT7_0  		0xB352
#define		REG_DMAOUTCNT15_8  		0xB353
#define		REG_FASTDMA8BYTE11 		0xB360
#define		REG_FASTDMA8BYTE10  		0xB361
#define		REG_FASTDMA8BYTE9  		0xB362
#define		REG_FASTDMA8BYTE8  		0xB363

#define		REG_DMA_BUF			0xB3A0
#define		REG_FATADDR7_0			0xB3A1
//#define		REG_FATADDR9_8			0xB3A2
#define		REG_FATADDR10_8			0xB3A2

#define		REG_FATHIT			0xB3A3

#define		REG_DMASTART			0xB3B0
#define		REG_DMACMP			0xB3C0
#define		REG_DMACMPEN			0xB3D0
#define		REG_DMAPRBSEL3_0		0xB3E0

//////////////////////////////////////////////////////////////////////
// FLASH MEMORY CONTROL REGISTER
//////////////////////////////////////////////////////////////////////
#define		REG_MEDIATYPE			0xB400
#define		REG_FMCTRLO7_0			0xB401
#define		REG_FMCTRLO15_8			0xB402
#define		REG_FMCTRLO23_16		0xB403
#define		REG_FMCTRLO29_24		0xB404
#define		REG_FMCTRLOE7_0			0xB405
#define		REG_FMCTRLOE15_8		0xB406
#define		REG_FMCTRLOE23_16		0xB407
#define		REG_FMCTRLOE29_24		0xB408
#define		REG_FMCTRLI7_0			0xB409
#define		REG_FMCTRLI15_8			0xB40A
#define		REG_FMCTRLI23_16		0xB40B
#define		REG_FMCTRLI29_24		0xB40C
#define		REG_FMGPIORINTEN7_0		0xB410
#define		REG_FMGPIORINTEN15_8		0xB411
#define		REG_FMGPIORINTEN23_16		0xB412
#define		REG_FMGPIORINTEN29_24		0xB413
#define		REG_FMGPIOFINTEN7_0		0xB414
#define		REG_FMGPIOFINTEN15_8		0xB415
#define		REG_FMGPIOFINTEN23_16		0xB416
#define		REG_FMGPIOFINTEN29_24		0xB417
#define		REG_FMGPIORINT7_0		0xB418
#define		REG_FMGPIORINT15_8		0xB419
#define		REG_FMGPIORINT23_16		0xB41A
#define		REG_FMGPIORINT29_24		0xB41B
#define		REG_FMGPIOFINT7_0		0xB41C
#define		REG_FMGPIOFINT15_8		0xB41D
#define		REG_FMGPIOFINT23_16		0xB41E
#define		REG_FMGPIOFINT29_24		0xB41F

//////////////////////////////////////////////////////////////////////
// NAND-GATE FLASH MEMORY
//////////////////////////////////////////////////////////////////////
#define		REG_NANDDATA			0xB420
#define		REG_NAND_SET			0xB421
#define		REG_NANDMODE			0xB422
#define		REG_NAND_SIGNAL			0xB423
#define		REG_NANDRDY			0xB424

#define		REG_LPIOWDATA16_7_0		0xB426
#define		REG_LPIOWDATA16_15_8		0xB427

//////////////////////////////////////////////////////////////////////
// COMPACTFLASH INTERFACE
//////////////////////////////////////////////////////////////////////
#define		REG_CFDATALOW			0xB430
#define		REG_CFDATAHIGH			0xB431
#define		REG_CFADDR2_0			0xB432
#define		REG_CFADDR10_3			0xB433
#define		REG_CF_SET			0xB434
#define		REG_CF_SIGNAL			0xB435
#define		REG_CFCS1_0			0xB436
#define		REG_CFREGNN			0xB439
#define		REG_CFRSTNN			0xB43A
#define		REG_CFREADY			0xB43B
#define		REG_CFRDYBSYNN			0xB43C

//////////////////////////////////////////////////////////////////////
// SERIAL INTERFACE(INCLUDING THE SPI SERIAL INTERFACE AND THE NEXTFLASH SERIAL INTERFACE)
//////////////////////////////////////////////////////////////////////
#define		REG_FMSITXD			0xB440
#define		REG_FMSIRXD			0xB441
#define		REG_FMSIPRX			0xB442
#define		REG_FMSIPRX1			0xB443
#define		REG_FMSIPRX7			0xB444
#define		REG_FMSLFREQ			0xB446
#define		REG_FM_SPI_SET			0xB447
#define		REG_SPIMODE			0xB448
#define		REG_SER_NX			0xB44A
#define		REG_FM_SPI_STATE		0xB44B

//////////////////////////////////////////////////////////////////////
// SD MEMORY INTERFACE
//////////////////////////////////////////////////////////////////////
#define		REG_SDRST			0xB450
#define		REG_SD_TIMER			0xB451
#define		REG_SD_TRIGGER			0xB452
#define		REG_SD_STATE0			0xB453
#define		REG_SD_STATE1			0xB454
#define		REG_DATALEN7_0			0xB455
#define		REG_DATALEN9_8			0xB456
#define		REG_WAITRSPTIME			0xB457
#define		REG_WAITCRCTIME			0xB458
#define		REG_DATABUFTX			0xB459
#define		REG_DATABUFRX			0xB45A
#define		REG_CMDBUF1			0xB45B
#define		REG_CMDBUF2			0xB45C
#define		REG_CMDBUF3			0xB45D
#define		REG_CMDBUF4			0xB45E
#define		REG_CMDBUF5			0xB45F
#define		REG_RESPBUF1			0xB460
#define		REG_RESPBUF2			0xB461
#define		REG_RESPBUF3			0xB462
#define		REG_RESPBUF4			0xB463
#define		REG_RESPBUF5			0xB464
#define		REG_RESPBUF6			0xB465
#define		REG_CRC7BUF			0xB466
#define		REG_CRC16BUF0L			0xB467
#define		REG_CRC16BUF0H			0xB468
#define		REG_CRC16BUF1L			0xB469
#define		REG_CRC16BUF1H			0xB46A
#define		REG_CRC16BUF2L			0xB46B
#define		REG_CRC16BUF2H			0xB46C
#define		REG_CRC16BUF3L			0xB46D
#define		REG_CRC16BUF3H			0xB46E
#define		REG_CRC16COR			0xB46F

//////////////////////////////////////////////////////////////////////
// MEMORY STICK
//////////////////////////////////////////////////////////////////////
#define		REG_MS_SET			0xB470
#define		REG_MSCOMMAND			0xB471
#define		REG_MSSPEED			0xB474
#define		REG_MSMODE			0xB474
#define		REG_MSTIMEOUTCNT		0xB475
#define		REG_MSSTATE0			0xB476
#define		REG_MSSTATE1			0xB477
#define		REG_RDDATA0			0xB478
#define		REG_RDDATA1			0xB479
#define		REG_RDDATA2			0xB47A
#define		REG_RDDATA3			0xB47B
#define		REG_MSCRC16BUF7_0		0xB47C
#define		REG_MSCRC16BUF15_8		0xB47D
#define		REG_MSCRCERROR			0xB47E
#define		REG_MSPIORDY			0xB47F
#define		REG_MSDATA0			0xB480
#define		REG_MSDATA1			0xB481
#define		REG_MSDATA2			0xB482
#define		REG_MSDATA3			0xB483
#define		REG_MSDATA4			0xB484
#define		REG_MSDATA5			0xB485
#define		REG_MSDATA6			0xB486
#define		REG_MSDATA7			0xB487
#define		REG_MSDATA8			0xB488
#define		REG_MSDATA9			0xB489
#define		REG_MSDATA10			0xB48A
#define		REG_MSDATA11			0xB48B
#define		REG_MSDATA12			0xB48C
#define		REG_MSDATA13			0xB48D
#define		REG_MSDATA14			0xB48E
#define		REG_MSDATA15			0xB48F

//////////////////////////////////////////////////////////////////////
// ECC
//////////////////////////////////////////////////////////////////////
#define		REG_ECC2048FLASH1		0xB494
#define		REG_ECC2048FLASH0		0xB495
#define		REG_ECC2048FLASH2		0xB496
#define		REG_ECC2048FLASH4		0xB497
#define		REG_ECC2048FLASH3		0xB498
#define		REG_ECC2048FLASH5		0xB499
#define		REG_ECC2048FLASH7		0xB49A
#define		REG_ECC2048FLASH6		0xB49B
#define		REG_ECC2048FLASH8		0xB49C
#define		REG_ECC2048FLASHA		0xB49D
#define		REG_ECC2048FLASH9		0xB49E
#define		REG_ECC2048FLASHB		0xB49F

#define		REG_ECCRST			0xB4A0
#define		REG_PSFMDATA			0xB4A1
#define		REG_ECCMASK			0xB4A2
#define		REG_ECCMODE			0xB4A3
#define		REG_ECC1			0xB4A4
#define		REG_ECC0			0xB4A5
#define		REG_ECC2			0xB4A6
#define		REG_ECC4			0xB4A7
#define		REG_ECC3			0xB4A8
#define		REG_ECC5			0xB4A9
#define		REG_ECC7			0xB4AA
#define		REG_ECC6			0xB4AB
#define		REG_ECC8			0xB4AC
#define		REG_ECCA			0xB4AD
#define		REG_ECC9			0xB4AE
#define		REG_ECCB			0xB4AF

#define		REG_ECC1_2			0xB494
#define		REG_ECC0_2			0xB495
#define		REG_ECC2_2			0xB496
#define		REG_ECC4_2			0xB497
#define		REG_ECC3_2			0xB498
#define		REG_ECC5_2			0xB499
#define		REG_ECC7_2			0xB49A
#define		REG_ECC6_2			0xB49B
#define		REG_ECC8_2			0xB49C
#define		REG_ECCA_2			0xB49D
#define		REG_ECC9_2			0xB49E
#define		REG_ECCB_2			0xB49F
//////////////////////////////////////////////////////////////////////
// SPI CRC
//////////////////////////////////////////////////////////////////////
#define		REG_SPICRCRST			0xB4B0
#define		REG_SPICRC7BUF			0xB4B1
#define		REG_SPICRC16BUF7_0		0xB4B2
#define		REG_SPICRC16BUF15_8		0xB4B3

//////////////////////////////////////////////////////////////////////
// INTERRUPT
//////////////////////////////////////////////////////////////////////
#define		REG_CFIRQ			0xB4C0
#define		REG_MSINTREVT3_0		0xB4CE
#define		REG_MSINTFEVT3_0		0xB4CF

#define		REG_CFIRQEN			0xB4D0
#define		REG_MSRINTEN3_0			0xB4DE
#define		REG_MSFINTEN3_0			0xB4DF

//////////////////////////////////////////////////////////////////////
// TEST
//////////////////////////////////////////////////////////////////////
#define		REG_TSEL			0XB4E0
#define		REG_FMPRBSEL			0XB4E1


//////////////////////////////////////////////////////////////////////
// USB CONTROL REGISTERS
//////////////////////////////////////////////////////////////////////
#define		REG_EP0BUFDATA			0xB500
#define		REG_RMWAKEEN			0xB501
#define		REG_USBHIGHSPEED		0xB502
#define		REG_SELFPOWER			0xB503
//#define		REG_BULKINDATA			0xB506
//#define		REG_BULKOUTDATA			0xB507
#define		REG_INTINDATA			0xB508
#define		REG_BINLASTEN			0xB50F

#define		REG_BULKINAUTO			0xB510
#define		REG_BULKOUTAUTO			0xB511
#define		REG_NYETEN			0xB512

#define		REG_USBBANK0WRIDX7_0		0xB515
#define		REG_USBBANK0WRIDX11_8		0xB516
#define		REG_USBBANK1WRIDX7_0		0xB517
#define		REG_USBBANK1WRIDX11_8		0xB518
#define		REG_BULKOUTACKCTR		0xB519

//#define		REG_				0xB51E
//#define		REG_				0xB51F
//#define		REG_				0xB520
//#define		REG_				0xB521

#define		REG_UTMICLKSEL			0xB522
//#define		REG_				0xB523

#define		REG_NAK_EN			0xB524
#define		REG_UDCTXMODE			0xB525
//#define		REG_RESERVED			0xB526
#define		REG_SFTPLUGOUT			0xB527
//#define		REG_SUSCNTRST			0xB528
//#define		REG_RESERVED			0xB529
//#define		REG_RESERVED			0xB52A

#define		REG_USB_SRAM_SEL		0xB530
#define		REG_EP0STORAGE6_0		0xB532
#define		REG_EP1STORAGE7_0		0xB533
#define		REG_EP1STORAGE9_8		0xB534
#define		REG_EP2STORAGE7_0		0xB535
#define		REG_EP2STORAGE9_8		0xB536
#define		REG_EP3STORAGE6_0		0xB537
#define		REG_SPEED_SEL			0xB53A
#define		REG_UDC20_PHYMODE		0xB53B
#define		REG_LINE_STATE			0xB53C
#define		REG_SIMMODE			0xB53D
#define		REG_BULKINACKCNT7_0		0xB540
#define		REG_BULKINACKCNT15_8		0xB541
#define		REG_BULKINACKCNT23_16		0xB542
#define		REG_BULKOUTACKCNT7_0		0xB543
#define		REG_BULKOUTACKCNT15_8		0xB544
#define		REG_BULKOUTACKCNT23_16		0xB545
#define		REG_INTINACKCNT7_0		0xB546
#define		REG_INTINACKCNT15_8		0xB547
#define		REG_INTINACKCNT23_16		0xB548
#define		REG_ACK_CNT_RST			0xB549
#define		REG_FORCETO11			0xB550

//#define		REG_UGPIOO7_0			0xB550
//#define		REG_UGPIOO15_8			0xB551
//#define		REG_UGPIOO23_16			0xB552
//#define		REG_UGPIOO30_24			0xB553
//#define		REG_UGPIOOE7_0			0xB554
//#define		REG_UGPIOOE15_8			0xB555
//#define		REG_UGPIOOE23_16		0xB556
//#define		REG_UGPIOOE30_24		0xB557
//#define		REG_UGPIOI7_0			0xB558
//#define		REG_UGPIOI15_8			0xB559
//#define		REG_UGPIOI23_16			0xB55A
//#define		REG_UGPIOI30_24			0xB55B
//#define		REG_UGPIORINTEN7_0		0xB560
//#define		REG_UGPIORINTEN15_8		0xB561
//#define		REG_UGPIORINTEN23_16		0xB562
//#define		REG_UGPIORINTEN30_24		0xB563
//#define		REG_UGPIOFINTEN7_0		0xB564
//#define		REG_UGPIOFINTEN15_8		0xB565
//#define		REG_UGPIOFINTEN23_16		0xB566
//#define		REG_UGPIOFINTEN30_24		0xB567
//#define		REG_UGPIORINT7_0		0xB568
//#define		REG_UGPIORINT15_8		0xB569
//#define		REG_UGPIORINT23_16		0xB56A
//#define		REG_UGPIORINT30_24		0xB56B
//#define		REG_UGPIOFINT7_0		0xB56C
//#define		REG_UGPIOFINT15_8		0xB56D
//#define		REG_UGPIOFINT23_16		0xB56E
//#define		REG_UGPIOFINT30_24		0xB56F

#define		REG_BANK1BULKINBUFINPTR7_0		0xB593
#define		REG_BANK1BULKINBUFINPTR11_8		0xB594
#define		REG_BANK1BULKOUTBUFINPTR7_0		0xB595
#define		REG_BANK1BULKOUTBUFINPTR11_8		0xB596
#define		REG_BANK1BULKINBUFOUTPTR7_0		0xB598
#define		REG_BANK1BULKINBUFOUTPTR11_8		0xB599
#define		REG_BANK1BULKOUTBUFOUTPTR7_0		0xB59A
#define		REG_BANK1BULKOUTBUFOUTPTR11_8	0xB59B

//#define		REG_EP0_EN			0xB5A0
//#define		REG_BULK_EN0			0xB5A1
#define		REG_BULK_EN1			0xB5A2

#define		REG_USB_SIGNAL			0xB5B0
//#define		REG_USBCFG			0xB5B1
//#define		REG_			0xB5B2

#define		REG_BANK0BULKINBUFINPTR7_0		0xB5B3
#define		REG_BANK0BULKINBUFINPTR8		0xB5B4
#define		REG_BANK0BULKOUTBUFINPTR7_0		0xB5B5
#define		REG_BANK0BULKOUTBUFINPTR8		0xB5B6
#define		REG_INTINBUFINPTR6_0		0xB5B7
#define		REG_BANK0BULKINBUFOUTPTR7_0		0xB5B8
#define		REG_BANK0BULKINBUFOUTPTR8		0xB5B9
#define		REG_BANK0BULKOUTBUFOUTPTR7_0		0xB5BA
#define		REG_BANK0BULKOUTBUFOUTPTR11_8	0xB5BB

#define		REG_INTINBUFOUTPTR6_0		0xB5BC

#define		REG_EP0BUFINPTR6_0		0xB5BD
#define		REG_EP0BUFOUTPTR6_0		0xB5BE
//#define		REG_BULK_EVENT			0xB5C0

#define		REG_USBSOFINT			0xB5C1
//#define		REG_BULK_ACK			0xB5C2
//#define		REG_USB_CONFIG			0xB5C3
//#define		REG_USB_INT_EVENT		0xB5D0
#define		REG_USBSOFINTEN			0xB5D1
//#define		REG_BULK_ACK_EN			0xB5D2
#define		REG_SET_CONFIG_EN		0xB5D3

#define		REG_USBPROBDATA			0xB5E0
#define		REG_HWTEST			0xB5E1
#define		REG_TSELECT			0xB5E1
//#define		REG_USB_MODULE_SEL		0xB5E1
//#define		REG_FINISH				0xB5E2
//#define		REG_BISTFAIL			0xB5E3
//#define		REG_ERRMAP				0xB5E4
//#define		REG_USBPROBDATA			0xB5E5
//#define		REG_TSELECT			0xB5E6
#define 	REG_USBSRAM_CLK_SEL		0xB5E6
#define		REG_USB_STALL			0xB5E8
#define		REG_BULK_BUF_CLR		0xB5E9
#define		REG_EP0BUFCLR			0xB5EA
#define		REG_BULK_NAK_CLR		0xB5EB
#define		REG_EP0_NAK			0xB5EC


//#define		REG_EP0BUFOUTPTR		0xB5ED
#define		REG_BULK_NAK			0xB5EE
//#define		REG_TSELECT				0xB5EF
//#define		REG_BMREQUESTTYPE		0xB5F0
//#define		REG_BREQUEST			0xB5F1
#define		REG_WVALUE7_0			0xB5F2
#define		REG_WVALUE15_8			0xB5F3
//#define		REG_WINDEX7_0			0xB5F4
#define		REG_WINDEX15_8			0xB5F5
#define		REG_WLENGTH7_0			0xB5F6
#define		REG_WLENGTH15_8			0xB5F7
//#define		REG_CPU_ACC_USBSRAM		0xBC00	//0x5C00 ~ 0x5FFF

//////////////////////////////////////////////////////////////////////
// SDRAM 
//////////////////////////////////////////////////////////////////////
#define		REG_SDMDMAIDX7_0		0xB700
#define		REG_SDMDMAIDX15_8		0xB701
#define		REG_SDMDMAIDX23_16		0xB702
#define		REG_SDMDMAIDX25_24		0xB703
#define		REG_SDMDMATRNSZ			0xB704
#define		REG_SDMSEL			0xB705
#define		REG_SELFREF			0xB706
#define		REG_REFCLOCKSEL			0xB707
#define		REG_SDMTRCD			0xB708
#define		REG_SDMCLOCKSEL			0xB709

//////////////////////////////////////////////////////////////////////
// FLASH
//////////////////////////////////////////////////////////////////////
#define		REG_NANDDMASEL			0xB800
#define		REG_NANDSIZE			0xB801
#define		REG_NANDCOUNTER			0xB802
#define		REG_CMDTRIGGER			0xB803
#define		REG_NANDCMD3_0			0xB804
#define		REG_NANDSTATE3_0		0xB805
#define		REG_SPEEDCMDBUF0			0xB806
#define		REG_SPEEDCMDBUF1			0xB807
#define		REG_ADDRBUF4			0xB808
#define		REG_ADDRBUF3			0xB809
#define		REG_ADDRBUF2			0xB80A
#define		REG_ADDRBUF1			0xB80B
#define		REG_ADDRBUF0			0xB80C
#define		REG_AUTOADDRSEL			0xB80D
#define		REG_NANDAUTOCMP			0xB80F

#define		REG_2KPAGE			0xB810
#define		REG_SPEEDCMDBUF2			0xB811
#define		REG_SPEEDCMDBUF3			0xB812
#define		REG_AUTONANDRDATA		0xB813
#define		REG_IDLECSENA			0xB81F
#define		REG_CHECKECC			0xB820
#define		REG_LBTRIGGER			0xB821
#define		REG_PPTRIGGER			0xB822
#define		REG_PADD1TRIGGER		0xB823
#define		REG_SNBTRIGGER			0xB824
#define		REG_ALLTRIGGER			0xB825
#define		REG_LOGPAGEADDR3		0xB830
#define		REG_LOGPAGEADDR2		0xB831
#define		REG_LOGPAGEADDR1		0xB832
#define		REG_LOGPAGEADDR0		0xB833
#define		REG_CURRENTBANK			0xB834
#define		REG_LOGBLOCKADDR1		0xB835
#define		REG_LOGBLOCKADDR0		0xB836
#define		REG_PHYBLOCKADDR1		0xB837
#define		REG_PHYBLOCKADDR0		0xB838
//#define		REG_BLOCKSIZE			0xB839
#define		REG_BANKSIZE1			0xB83A
#define		REG_BANKSIZE0			0xB83B
#define		REG_LOGICALBLOCKPERBANK1	0xB83C
#define		REG_LOGICALBLOCKPERBANK0	0xB83D
#define		REG_BANK0TOTALBLK1		0xB83E
#define		REG_BANK0TOTALBLK0		0xB83F
#define		REG_REMSECTOR7_0		0xB840
#define		REG_OLDPHYADDRNUM31_24		0xB841
#define		REG_OLDPHYADDRNUM23_16		0xB842
#define		REG_OLDPHYADDRNUM15_8		0xB843
#define		REG_OLDPHYADDRNUM7_0		0xB844
#define		REG_NEWPHYADDRNUM31_24		0xB845
#define		REG_NEWPHYADDRNUM23_16		0xB846
#define		REG_NEWPHYADDRNUM15_8		0xB847
#define		REG_NEWPHYADDRNUM7_0		0xB848
#define		REG_NEWPHYBLOCKAR15_8		0xB849
#define		REG_NEWPHYBLOCKAR7_0		0xB84A
#define		REG_STARTLOGADDR31_24		0xB84B
#define		REG_STARTLOGADDR23_16		0xB84C
#define		REG_STARTLOGADDR15_8		0xB84D
#define		REG_STARTLOGADDR7_0		0xB84E
#define		REG_AUTOWRECCSEL		0xB84F
#define		REG_LBAFIELD_H			0xB850
#define		REG_LBAFIELD_L			0xB851
#define		REG_SNBINI06_0			0xB852
#define		REG_SNBINI16_0			0xB853
#define		REG_NANDEXTD00			0xB860
#define		REG_NANDEXTD01			0xB861
#define		REG_NANDEXTD02			0xB862
#define		REG_NANDEXTD03			0xB863
#define		REG_NANDEXTD04			0xB864
#define		REG_NANDEXTD05			0xB865
#define		REG_NANDEXTD06			0xB866
#define		REG_NANDEXTD07			0xB867
#define		REG_NANDEXTD08			0xB868
#define		REG_NANDEXTD09			0xB869
#define		REG_NANDEXTD0A			0xB86A
#define		REG_NANDEXTD0B			0xB86B
#define		REG_NANDEXTD0C			0xB86C
#define		REG_NANDEXTD0D			0xB86D
#define		REG_NANDEXTD0E			0xB86E
#define		REG_NANDEXTD0F			0xB86F

#define		REG_NANDEXTD10			0xB870
#define		REG_NANDEXTD11			0xB871
#define		REG_NANDEXTD12			0xB872
#define		REG_NANDEXTD13			0xB873
#define		REG_NANDEXTD14			0xB874
#define		REG_NANDEXTD15			0xB875
#define		REG_NANDEXTD16			0xB876
#define		REG_NANDEXTD17			0xB877
#define		REG_NANDEXTD18			0xB878
#define		REG_NANDEXTD19			0xB879
#define		REG_NANDEXTD1A			0xB87A
#define		REG_NANDEXTD1B			0xB87B
#define		REG_NANDEXTD1C			0xB87C
#define		REG_NANDEXTD1D			0xB87D
#define		REG_NANDEXTD1E			0xB87E
#define		REG_NANDEXTD1F			0xB87F

#define		REG_NANDEXTD20			0xB880
#define		REG_NANDEXTD21			0xB881
#define		REG_NANDEXTD22			0xB882
#define		REG_NANDEXTD23			0xB883
#define		REG_NANDEXTD24			0xB884
#define		REG_NANDEXTD25			0xB885
#define		REG_NANDEXTD26			0xB886
#define		REG_NANDEXTD27			0xB887
#define		REG_NANDEXTD28			0xB888
#define		REG_NANDEXTD29			0xB889
#define		REG_NANDEXTD2A			0xB88A
#define		REG_NANDEXTD2B			0xB88B
#define		REG_NANDEXTD2C			0xB88C
#define		REG_NANDEXTD2D			0xB88D
#define		REG_NANDEXTD2E			0xB88E
#define		REG_NANDEXTD2F			0xB88F

#define		REG_NANDEXTD30			0xB890
#define		REG_NANDEXTD31			0xB891
#define		REG_NANDEXTD32			0xB892
#define		REG_NANDEXTD33			0xB893
#define		REG_NANDEXTD34			0xB894
#define		REG_NANDEXTD35			0xB895
#define		REG_NANDEXTD36			0xB896
#define		REG_NANDEXTD37			0xB897
#define		REG_NANDEXTD38			0xB898
#define		REG_NANDEXTD39			0xB899
#define		REG_NANDEXTD3A			0xB89A
#define		REG_NANDEXTD3B			0xB89B
#define		REG_NANDEXTD3C			0xB89C
#define		REG_NANDEXTD3D			0xB89D
#define		REG_NANDEXTD3E			0xB89E
#define		REG_NANDEXTD3F			0xB89F

//////////////////////////////////////////////////////////////////////
// MLC ECC
//////////////////////////////////////////////////////////////////////
#define		REG_STARTENC			0xB8A0
#define		REG_STARTDEC			0xB8A1
#define		REG_ECCBUSY			0xB8A2
#define		REG_RSECCEN			0xB8A3
#define		REG_STARTPARITY			0xB8A4

#define		REG_PARITY0			0xB8B0
#define		REG_PARITY1			0xB8B1
#define		REG_PARITY2			0xB8B2
#define		REG_PARITY3			0xB8B3
#define		REG_PARITY4			0xB8B4
#define		REG_PARITY5			0xB8B5
#define		REG_PARITY6			0xB8B6
#define		REG_PARITY7			0xB8B7
#define		REG_PARITY8			0xB8B8
#define		REG_PARITY9			0xB8B9

#define		REG_SYNDROM07_0			0xB8C0
#define		REG_SYNDROM09_8			0xB8C1
#define		REG_SYNDROM17_0			0xB8C2
#define		REG_SYNDROM19_8			0xB8C3
#define		REG_SYNDROM27_0			0xB8C4
#define		REG_SYNDROM29_8			0xB8C5
#define		REG_SYNDROM37_0			0xB8C6
#define		REG_SYNDROM39_8			0xB8C7
#define		REG_SYNDROM47_0			0xB8C8
#define		REG_SYNDROM49_8			0xB8C9
#define		REG_SYNDROM57_0			0xB8CA
#define		REG_SYNDROM59_8			0xB8CB
#define		REG_SYNDROM67_0			0xB8CC
#define		REG_SYNDROM69_8			0xB8CD
#define		REG_SYNDROM77_0			0xB8CE
#define		REG_SYNDROM79_8			0xB8CF

#define		REG_BERLEKAMP07_0		0xB8D0
#define		REG_BERLEKAMP09_8		0xB8D1
#define		REG_BERLEKAMP17_0		0xB8D2
#define		REG_BERLEKAMP19_8		0xB8D3
#define		REG_BERLEKAMP27_0		0xB8D4
#define		REG_BERLEKAMP29_8		0xB8D5
#define		REG_BERLEKAMP37_0		0xB8D6
#define		REG_BERLEKAMP39_8		0xB8D7

#define		REG_ERRPOS07_0			0xB8E0
#define		REG_ERRPOS09_8			0xB8E1
#define		REG_ERRPOS17_0			0xB8E2
#define		REG_ERRPOS19_8			0xB8E3
#define		REG_ERRPOS27_0			0xB8E4
#define		REG_ERRPOS29_8			0xB8E5
#define		REG_ERRPOS37_0			0xB8E6
#define		REG_ERRPOS39_8			0xB8E7
#define		REG_CHIENOUT07_0		0xB8E8
#define		REG_CHIENOUT09_8		0xB8E9
#define		REG_CHIENOUT17_0		0xB8EA
#define		REG_CHIENOUT19_8		0xB8EB
#define		REG_CHIENOUT27_0		0xB8EC
#define		REG_CHIENOUT29_8		0xB8ED
#define		REG_CHIENOUT37_0		0xB8EE
#define		REG_CHIENOUT39_8		0xB8EF

#define		REG_FORNEYOUT07_0		0xB8F0
#define		REG_FORNEYOUT09_8		0xB8F1
#define		REG_FORNEYOUT17_0		0xB8F2
#define		REG_FORNEYOUT19_8		0xB8F3
#define		REG_FORNEYOUT27_0		0xB8F4
#define		REG_FORNEYOUT29_8		0xB8F5
#define		REG_FORNEYOUT37_0		0xB8F6
#define		REG_FORNEYOUT39_8		0xB8F7

//////////////////////////////////////////////////////////////////////
// LCD
//////////////////////////////////////////////////////////////////////
#define		REG_LCDSET0			0xB900
#define		REG_LCDSET1			0xB901
#define		REG_PIXCLKDIVIDER7_0		0xB902
#define		REG_PIXCLKDIVIDER9_8		0xB903
#define		REG_PIXVAL7_0			0xB904
#define		REG_PIXVAL10_8			0xB905
#define		REG_LINEVAL7_0			0xB906
#define		REG_LINEVAL8			0xB907
#define		REG_ADR_BASE7_0			0xB908
#define		REG_ADR_BASE15_8		0xB909
#define		REG_OFFSIZE7_0			0xB90A
#define		REG_OFFSIZE9_8			0xB90B
#define		REG_LP_CP_SET			0xB90C
#define		REG_LBLANK_WIDTH		0xB90D
#define		REG_FM_C_FREQ			0xB90E
#define		REG_FM_BC_TYPE			0xB90F

#define		REG_FIFO_OVER			0xB910
#define		REG_LINEAR_SIZE_B7_0		0xB911
#define		REG_LINEAR_SIZE_B10_8		0xB912
#define		REG_DMA_ADR7_0			0xB913
#define		REG_DMA_ADR15_8			0xB914
#define		REG_DMA_SIZE7_0			0xB915
#define		REG_DMA_SIZE15_8		0xB916
#define		REG_DRAM_ADR7_0			0xB917
#define		REG_DRAM_ADR15_8		0xB918
#define		REG_DRAM_ADR23_16		0xB919
#define		REG_DRAM_ADR31_24		0xB91A
#define		REG_LCD_INTR			0xB91B

//////////////////////////////////////////////////////////////////////
// DSP3
//////////////////////////////////////////////////////////////////////
#define		REG_SC10_7_0			0xBE0A
#define		REG_SC10_15_8			0xBE0B
#define		REG_SC32_7_0			0xBE0C
#define		REG_SC32_15_8			0xBE0D
#define		REG_SC54_7_0			0xBE0E
#define		REG_SC54_15_8			0xBE0F
#define		REG_SC76_7_0			0xBE10
#define		REG_SC76_15_8			0xBE11
#define		REG_SC98_7_0			0xBE12
#define		REG_SC98_15_8			0xBE13
#define		REG_SCBA_7_0			0xBE14
#define		REG_SCBA_15_8			0xBE15
#define		REG_TADDI			0xBE20
#define		REG_TADDO			0xBE22
#define		REG_D_ACK			0xBE24
#define		REG_DSP_ADDR_LOW		0xBE40
#define		REG_DSP_BULKINACKCNT7_0		0xBE41
#define		REG_INDIR_DATA7_0		0xBE42
#define		REG_INDIR_DATA15_8		0xBE43
#define		REG_INDIR_DATA23_16		0xBE44
#define		REG_DM_WRITE_TRIG		0xBE45
//#define		REG_DM_READ_TRIG		0xBE46
//#define		REG_KIM_WRITE_TRIG		0xBE47
//#define		REG_KIM_READ_TRIG		0xBE48

//////////////////////////////////////////////////////////////////////
// SRAM ADDRESS
//////////////////////////////////////////////////////////////////////
#define		REG_DSP_BUFFER			0x0000
#define		REG_DSP_PM			0x1000
#define		REG_LCD_BUFFER			0x3800
#define		REG_USB_BANK0			0x5000
#define		REG_USB_BANK1			0x5800
#define		REG_MCU_SRAM1			0x6000
#define		REG_MCU_SRAM2			0x6800
#define		REG_MCU_BOOT			0x7000
#define		REG_GLOBAL			0xB000
#define		REG_RAM_SPACE			0xC000



#endif
