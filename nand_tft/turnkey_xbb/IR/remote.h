#define IR_NEC_PROTOCAL
//#define IR_SONY_SIRC_PROTOCAL
//#define IR_PHILIPS_RC5_PROTOCAL
//#define IR_SHARP_PROTOCAL

#define	IR_6_Key	0xFF02
#define	IR_7_Key	0x00FF
#define	IR_11_Key	0x00BF
#define	IR_11_1_Key	0xFFEF
#define	IR_18_Key	0x7F80
#define	IR_20_Key	0xFFFF//0xFD02
#define	IR_21_Key	0xBF00
#define	IR_21_1_Key	0xFF00//0xFD02//0xFFFF//0xFD02

#define IR_MOD_UNKNOWN	0
#define IR_MOD_PPM		1
#define IR_MOD_PWM		2
#define IR_MOD_BIPHASE	3
#define IR_MOD_BINARY	4

#define T_SAMPLE		(120-1)		// = 10us = 0.01ms

#ifdef IR_NEC_PROTOCAL
#define IR_MODULATION	IR_MOD_PPM
#define TIME_OUT_MIN	(1000)		// 10ms
#define START1_MAX		(900+200)	// 9ms
#define START1_MIN		(900-200)
#define START0_MAX		(450+90)	// 4.5ms
#define START0_MIN		(450-90)
#define LOGIC1_MAX		(169+40)	// 1.69ms
#define LOGIC1_MIN		(169-40)
#define LOGIC0_MAX		(57+20)		// 0.565ms
#define LOGIC0_MIN		(57-20)
#define BIT_TIME		(0)
#define SAMPLE_POINT	(0)
#endif

#ifdef IR_SONY_SIRC_PROTOCAL
#define IR_MODULATION	IR_MOD_PWM
#define TIME_OUT_MIN	(300)		// 3ms
#define START1_MAX		(240+60)	// 2.4ms
#define START1_MIN		(240-60)
#define START0_MAX		(60+15)		// 0.6ms
#define START0_MIN		(60-15)
#define LOGIC1_MAX		(120+30)	// 1.2ms
#define LOGIC1_MIN		(120-30)
#define LOGIC0_MAX		(60+15)		// 0.6ms
#define LOGIC0_MIN		(60-15)
#define BIT_TIME		(0)
#define SAMPLE_POINT	(0)
#endif

#ifdef IR_PHILIPS_RC5_PROTOCAL
#define IR_MODULATION	IR_MOD_BIPHASE
#define TIME_OUT_MIN	(300)		// 3ms
#define START1_MAX		(0)
#define START1_MIN		(0)
#define START0_MAX		(0)
#define START0_MIN		(0)
#define LOGIC1_MAX		(0)
#define LOGIC1_MIN		(0)
#define LOGIC0_MAX		(0)
#define LOGIC0_MIN		(0)
#define BIT_TIME		(89)		// 889us
#define SAMPLE_POINT	(89/2)
#endif

#ifdef IR_SHARP_PROTOCAL
#define IR_MODULATION	IR_MOD_PPM
#define TIME_OUT_MIN	(1000)		// 10ms
#define START1_MAX		(0)
#define START1_MIN		(0)
#define START0_MAX		(0)
#define START0_MIN		(0)
#define LOGIC1_MAX		(200+60)	// 2ms
#define LOGIC1_MIN		(200-60)
#define LOGIC0_MAX		(100+30)	// 1ms
#define LOGIC0_MIN		(100-30)
#define BIT_TIME		(0)
#define SAMPLE_POINT	(0)
#endif

sbit IR_IN = P3^0;

#define HI_BYTE(x)	((U8)(x>>8))
#define LO_BYTE(x)	((U8)x)

extern void ir_init(void);
extern void ir_service(void);
extern void ir_commandservice(void);
extern void SaveNumber(U8 number);
extern void ClearIRBuffer(void);
extern void Frequency_Disp(void);
extern void Channel_Set_Init(void);
extern void Change_EQ(void);
extern void VOL_UP(void);
extern void VOL_DOWN(void);
extern void PickSong_Process(void);
extern void NextorPre_Song(bit tb_state);
extern void ChannelSet_Process(void);//20090409 chiayen add
