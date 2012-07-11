// For AS6001, HY1418
#ifdef CAR_48
	sbit	FM_DAT=P3^1;
	sbit	FM_CLK=P1^3;
#else
	sbit	FM_DAT=P1^5;
	sbit	FM_CLK=P1^4;
#endif
#ifdef CAR_64
	sbit 	FM_EN=P1^3; 
#elif defined(CAR_48)
	sbit 	FM_EN=P1^3; //
#elif defined(EVB_128)  
	sbit	FM_EN=P1^2;
#elif defined(EVB_64)
	sbit 	FM_EN=P1^2;	
#endif

// For ZI2020, AS6300
#ifdef 	CAR_48
	sbit	FM_SCL=P1^3;
	sbit	FM_SDA=P3^1;
#else
	sbit	FM_SDA=P1^5;
	sbit	FM_SCL=P1^4;
#endif 

#ifdef CAR_64
sbit	FM_RSTB=P1^3;
#elif defined(CAR_48)
sbit	FM_RSTB=P1^3;
#elif defined(EVB_128)  
sbit	FM_RSTB=P1^2;
#elif defined(EVB_64)
sbit	FM_RSTB=P1^2;
#endif

// For AS6300
#define REGPOS_RFPLL_PB		7
#define REGPOS_RFPLL_SB_H	1
#define REGPOS_RFPLL_SB_L	15
#define TX_MSK_RFPLL_PB		0x807f
#define TX_MSK_RFPLL_SB_L	0x7FFF
#define TX_MSK_RFPLL_SB_H	0xFFF8

#define	IIC_SCL_Low			FM_SCL=0
#define	IIC_SCL_High		FM_SCL=1
#define	IIC_SDA_Low			FM_SDA=0
#define	IIC_SDA_High		FM_SDA=1
#define	IIC_SDA_Test		FM_SDA

#ifdef CAR_48
#define IIC_SDA_Set_Input 	FM_SDA=1;	XBYTE[0xB103] &=0xFD
#define IIC_SDA_Set_Output 	FM_SDA=1;	XBYTE[0xB103] |=0x02
#else
#define IIC_SDA_Set_Input 	FM_SDA=1;	XBYTE[0xB102] &=0xDF
#define IIC_SDA_Set_Output 	FM_SDA=1;	XBYTE[0xB102] |=0x20
#endif


#define	IIC_Delay_Time	1
#define TX_MASK_ST 		0x0040
#define TX_MASK_POWC	0x000f
#define TX_MASK_PC		0x000f
#define TX_MSK_POWC		0xFFF0
#define TX_MSK_PC		0xFFF0
#define AS6200_Default_Freq 875

extern void	FM_drive(void);
extern void	FM_initial(void);
extern void	I2C_ZI2020_Send_data(U8 add,U8 dat);
extern void	I2C_ZI2020_SendByte(U8 dat);
extern void	I2C_ZI2020_StartBit(void);
extern void	I2C_ZI2020_StopBit(void);

extern void	IIC_Start(void);
extern void	IIC_Stop(void);
extern void	IIC_SendAck(bit k);
extern void	AS6200_init(void);
extern void	AS6200_Write_Data(U8 Reg,U16 Reg_val);
extern void	AS6200_Set_Freq(U16 frq);
extern void	AS6200_Read_Freq(void);
extern void I2C_Wait(U16 tt); //chiayen0805
extern void FM_FREQ_CHG(U8 tc_UP_DOWN,U8 offset);
extern U8	IIC_WriteByte(U8 addr);
extern U8	IIC_ReadByte(U8 ack);
extern U16	AS6200_Read_Data(U8 Reg); //AS6200 read data according to Reg

//===================QN8027 ===========    by home 
/* modify this according to I2C device address when you use standard I2C function like I2C_XXXX except AI2C_XXXX*/
#define I2C_DEV0_ADDRESS 0x58
#define I2C_TIMEOUT_TIME    10
#define I2C_TIMEOUT_COUNT    8
#define MS_DELAY_CONST    40
/************end*********************/


extern void I2C_QN8027_Send_data(U8 add,U8 dat);
extern U16  I2C_QN8027_read_data(U8 add);
U8 I2C_Write_1byte(U8 Slave,U8 Regis_Addr,U8 Data);
U8 I2C_Read_1byte(U8 Slave,U8 Regis_Addr);

#ifdef CAR_48
#define Read_SDA      (P3>>1 & 1)
#else
#define Read_SDA      (P1>>5 & 1)
#endif
#define FREQ2CHREG(freq)   ((freq-7600)/5)
#define CHREG2FREQ(ch)     (ch*5+7600)

#define BAND_FM		0

// stereo mode
#define QND_TX_AUDIO_MONO              0x10
#define QND_TX_AUDIO_STEREO            0x00

#define CHIPID_QN8027   0x40

#define SYSTEM1		0x00
#define SYSTEM2		0x00
#define CH			0x01
#define CH_STEP		0x00
#define RDSD0		0x08
#define PAG_CAL		0x1f
#define CID2		6

#define RDSEN		0x80
#define TXREQ       0x20
#define CH_CH       0x03
#define RDSTXRDY    0x04

extern void QND_Delay(U16 ms) ;
extern U8 QND_Init() ;
extern void  QND_TuneToCH(U16 ch) ;
extern U8 QNF_SetCh(U16 freq);



//BK1085=================

#define BK_DATA_HIGH()   IIC_SDA_High;
#define BK_DATA_LOW()    IIC_SDA_Low;

#ifdef CAR_48
#define BK_DATA_READ()      (P3>>1 & 1)
#else
#define BK_DATA_READ()      (P1>>5 & 1)
#endif
#define SDADIROUT()       IIC_SDA_Set_Output;
#define SDADIRIN()        IIC_SDA_Set_Input;

#define SCLDIROUT()       
#define BK_CLK_HIGH()     IIC_SCL_High;
#define BK_CLK_LOW()      IIC_SCL_Low;

#define	DEV_ADDR	0x1d		
extern void Chip_powerdown(void);

extern	void AudioGain(U8 volume);
extern	void SetFrequency(U16 freq_2);
extern	void Chip_initialization();

extern	void Wire2_Spi0_Write_32Bit(U8 addr,U32 value);
extern	U8 BEKEN_I2C_readbyte(void);
extern	void BEKEN_I2C_sendbyte(U8 I2CSendData);
extern	U8 BEKEN_I2C_ReceiveACK(void);
extern	void BEKEN_I2C_nack(void);
extern	void BEKEN_I2C_ack(void);
extern	void BEKEN_I2C_Stop(void);
extern	void BEKEN_I2C_Start(void);
extern	void BEKEN_I2C_init(void);
extern	void Delay1us();
extern	void Delay1us_50ms();
