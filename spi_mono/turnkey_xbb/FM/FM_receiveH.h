sbit DbgP14=P1^4;
sbit DbgP13=P1^3;

#if 0
	#define CLR_IIC_SCL 	XBYTE[0xB402]&=~0x04	// FM[10]=0
	#define SET_IIC_SCL 	XBYTE[0xB402]|=0x04		// FM[10]=1
	
	#define CLR_IIC_SDA 	XBYTE[0xB402]&=~0x10	// FM[12]=0
	#define SET_IIC_SDA 	XBYTE[0xB402]|=0x10		// FM[12]=1
	#define IIC_SDA_IN		XBYTE[0xB402]|=0x10;	XBYTE[0xB406]&=~0x10
	#define IIC_SDA_OUT		XBYTE[0xB406]|=0x10
	#define	IIC_SDA_Test	XBYTE[0xB40A]&=0x10
#else
	#define CLR_IIC_SCL DbgP13=0//	XBYTE[0xB402]&=~0x04	// FM[10]=0
	#define SET_IIC_SCL DbgP13=1//	XBYTE[0xB402]|=0x04		// FM[10]=1
	
	#define CLR_IIC_SDA DbgP14=0//	XBYTE[0xB402]&=~0x10	// FM[12]=0
	#define SET_IIC_SDA DbgP14=1//	XBYTE[0xB402]|=0x10		// FM[12]=1
	#define IIC_SDA_IN	DbgP14=1;	XBYTE[0xB102]&=~0x10//	XBYTE[0xB402]|=0x10;	XBYTE[0xB406]&=~0x10
	#define IIC_SDA_OUT	XBYTE[0xB102]|=0x10//	XBYTE[0xB406]|=0x10
	#define	IIC_SDA_Test DbgP14//	XBYTE[0xB40A]&=0x10
#endif

#define highfreq  1080
#define lowfreq   875
#define stepfreq  1
#define SomeNOP(); ;;;;;;;;


//--程序支持的FM模组
#define	Mode_FM_CL6017		//1//6017S
//#define	Mode_FM_RDA5807P	//2
//#define	Mode_FM_QN8035		//5

//#define	Mode_FM_BK1080  	//3
//#define	Mode_FM_KT0830E    	//4
//#define	Mode_FM_AR1000		//6
//#define	Mode_FM_CL6017G		//7///新的6017
//#define	Mode_FM_CL6016G		//8//已停产

//delay
extern void IIC_Dealy(x_DelayD);
extern U8 iicCheckAck(void);
extern void iicSTART(void);
extern void iicSTOP(void);
extern void iicAck(U8 ans);

//------IIC 调用的程序
extern void	iicWriteByte(U8 I2cData);
extern void	IIC_Dealy(x_DelayD);
extern void	SendStr(U8 sla,U8 *s,U8 no);
extern void	RecStr(U8 sla,U8 *s,U8 no);
extern U8	iicReadByte(void);


extern void I2C_Write_nbyte(U8 Slave,U8 Regis_Addr,U8 *s,U8 no);
extern void I2C_Read_nbyte(U8 Slave,U8 Regis_Addr,U8 *s,U8 no);
//-----------------------------------------------------------------


#ifdef Mode_FM_CL6017	//--CL6017(6017s)//tea5767
//	extern U8  CL6017_Read_ID(void);
	extern void CL6017_FMInit(void);
	extern void CL6017_MUTE(void);
	extern void CL6017_UnMUTE(void);
	extern void CL6017_FMVolumeSet(U16 vol);
	extern void CL6017_FMTune();//设台，tc_mute=1时设台后无声音（用作搜台时设台）
	extern void CL6017_FMSeekFromCurrentCh(U8 seekUp);//seekUp=0向上搜台，seekUp=1向下搜台
	extern void CL6017AutoSearchAllFreq(U16 x_FreqTemp);//全部搜台
	extern void CL6017_FMPowerdown(void);//关掉FM
	extern void FM_OPEN_CL6017(void);//打开fm
#elif defined Mode_FM_RDA5807P
	//extern U8 RDA5807P_Read_ID();
	extern void RDA5807P_FMInit();
	extern void RDA5807P_FMPowerdown(void);
	extern void RDA5807P_MUTE(void);
	extern void RDA5807P_UnMUTE(void);
	extern void RDA5807P_FMTune(); //87.5Mhz -> 8750
	extern void RDA5807P_FMSeekFromCurrentCh(U8 seekUp);
	extern void RDA5807P_FMVolumeSet(unsigned int vol) ; /*input: 0 - 15 */
#elif defined Mode_FM_QN8035
	//extern U8 QND_Read_ID(void);
	extern void QND_Init(void);
	extern void QND_TuneToCH();
	extern void QND_RXConfigAudio(U8 option);
	extern void QNF_PowerDown(void);
	extern void QNF_SetMute(U8 On);
	extern void QN8035_FMSeekFromCurrentCh(U8 seekUp);
	extern void QN8035_FMSeekAllCh(U16 TunedFrequency);
#endif



