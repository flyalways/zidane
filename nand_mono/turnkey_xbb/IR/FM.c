#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\header\dos\fs_struct.h"
#include "..\IR\remote.h"
#include "..\IR\LCM_BMP.h"
#include "..\IR\FM.h"

extern  xdata	U8	gc_Err;
extern	xdata	U8	gc_SelectVol;		// 1=Show Vol
extern	xdata	U8	gc_SelectEQ;		// 0=Show Song Number  1=Show EQ
extern	data	bit	gb_Frequency_Song;	// 0=Show Frequency    1=Show Song Number/EQ
extern	data	bit	gb_ChannelSet;
extern	data	bit	gb_LED_LCM;
extern	data	U8	gc_ShowTimer;
extern  data    U16 gw_FM_frequency;  // range=87.7MHz~107.9MHz
extern  data	U16	gw_IR_Setting_Timer;

void	FM_FREQ_CHG(U8 tc_UP_DOWN,U8 offset);
void	FM_drive(void);
void	FM_initial(void);
void 	FM_FREQ_CHG(U8 tc_UP_DOWN,U8 offset);

extern data U16 gw_LogData_Timer;//chiayen0812

xdata	U8 qnd_i2c;
xdata	U8 qnd_i2c_timeout;

void FM_FREQ_CHG(U8 tc_UP_DOWN,U8 offset)	// 1=UP  0=DOWN
{
#if 0
	U8	tc_FM_select;

	gc_Err=0;
	gb_Frequency_Song=0;	// 0=Show Frequency    1=Show Song Number/EQ
	gc_SelectVol=0;			// 1=Show Vol
	gc_SelectEQ=0;			// 0=Show Song Number  1=Show EQ
	gb_ChannelSet=0;

	gc_ShowTimer=72;

	tc_FM_select=0x10;
	
	if((tc_FM_select==0x00)||(tc_FM_select==0x10)||(tc_FM_select==0x90))	// 全頻(87.5MHz~108.0MHz)
	{
		if(tc_UP_DOWN==1)
		{
			gw_FM_frequency+=offset;
			{
				if(gw_FM_frequency>1080)
				{
					gw_FM_frequency=gw_FM_frequency-206;
				}
			}
		}
		else
		{
			gw_FM_frequency-=offset;
/*			if((P3&0x01)==0x01)	// 歐美規格(88.1MHz~107.9MHz)
			{
				if(gw_FM_frequency<881)
				{
					gw_FM_frequency=199+gw_FM_frequency;
				}
			}
			else*/
			{
				if(gw_FM_frequency<875)
				{
					gw_FM_frequency=206+gw_FM_frequency;
				}
			}
		}
	}
	FM_drive();
	gw_IR_Setting_Timer=120;
#endif
}


void FM_drive(void)
{
#if 0

	U8  tc_FMModule_Type;
	U16	tw_FM_frequency;

	gw_LogData_Timer=60; //chiayen0812
	tc_FMModule_Type=FM_MODULE;

	if(tc_FMModule_Type==FM_ZI2020)	// ZI2020
	{
	    tw_FM_frequency=0xF000|gw_FM_frequency;

	    I2C_ZI2020_Send_data(0x00,(U8)tw_FM_frequency);
    	I2C_ZI2020_Send_data(0x01,tw_FM_frequency>>8);
	}
	else if((tc_FMModule_Type==FM_AS6001) || (tc_FMModule_Type/*FM_MODULE*/==FM_HY1418))	// AS6001, HY1418
	{
		U8 i;
		FM_EN=1;	// CE=1
		tw_FM_frequency=gw_FM_frequency|0x4800;

		for(i=0;i<16;i++)
		{
			if(tw_FM_frequency&0x01)
			{
				FM_DAT=1;
			}
			else
			{
				FM_DAT=0;
			}
			I2C_Wait(500);	
			FM_CLK=1;	
			I2C_Wait(500);	
			FM_CLK=0;	
			tw_FM_frequency>>=1;
		}
		
		FM_EN=0;
		I2C_Wait(500);
	}
	else if(tc_FMModule_Type==FM_AS6300)	// AS6300
	{
		AS6200_Set_Freq(gw_FM_frequency);
		AS6200_Read_Freq();
	}
	else if(tc_FMModule_Type==FM_QN8027)	// QN8027
	{
                QND_TuneToCH(gw_FM_frequency);
	}
#endif
}


void FM_initial(void)
{
#if 0

	U8  tc_FMModule_Type;

	tc_FMModule_Type=FM_MODULE;
	if(tc_FMModule_Type==FM_AS6300)// AS6300 //chiayen0805
	{
		AS6200_init();
	}
	else if(tc_FMModule_Type==FM_ZI2020)	// ZI2020
	{
		FM_SCL=1;
		FM_DAT=1;
		// ZI2020 RESET
		FM_RSTB=0;
		I2C_Wait(300);
		FM_RSTB=1;
		I2C_Wait(300);
		// Initial ZI2020
		I2C_ZI2020_Send_data(0x00,0x81);
		I2C_Wait(10);
		I2C_ZI2020_Send_data(0x01,0x03);
		I2C_Wait(10);
		I2C_ZI2020_Send_data(0x02,0x00);
		I2C_Wait(10);
		I2C_ZI2020_Send_data(0x13,0x80);
		I2C_Wait(10);
	}
	else if(tc_FMModule_Type==FM_QN8027)	// QN8027
	{   
		QND_Init();   
		dbprintf("FM_QN8027_FM_init\n");
	}
	else/* if((FM_MODULE==FM_AS6001) || (FM_MODULE==FM_HY1418))	// AS6001, HY1418*/	// AS6001, HY1418, AS6002, HY1417LP
	{
		FM_EN=0;
		FM_CLK=0;
		I2C_Wait(500);
	}
#endif
}