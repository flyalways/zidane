#include <intrins.h>
#include "..\header\SPDA2K.h"
#include "..\header\variables.h"


U8 code gac_clock_table[][6] =
{	//(Jimi 091109)add 0xB023 as secondary config
  // B006, B024, B026, TH1  TH0            TL0            
  {  0,	   0,    0,    243, C_T0_12MhzMSB, C_T0_12MhzLSB}, // 0=XTL  XTL=12M, MCU_PERI=12M, DSP=12M CLOCK_MODE_XTL
  {	 8|64, 1,    0,    243, C_T0_12MhzMSB, C_T0_12MhzLSB}, // 1=MP3  PLL=24M, MCU_PERI=12M, DSP=24M CLOCK_MODE_MP3
  {  8|64, 0,    0,    230, C_T0_24MhzMSB, C_T0_24MhzLSB}, // 2=MP3L PLL=24M, MCU_PERI=24M, DSP=24M CLOCK_MODE_MP3L
  { 16|64, 3,    0,    243, C_T0_12MhzMSB, C_T0_12MhzLSB}, // 3=WMA  PLL=48M, MCU_PERI=12M, DSP=48M CLOCK_MODE_WMA
  {	24|64, 0,    5,    178, C_T0_72MhzMSB, C_T0_72MhzLSB}, // 4=DOS  PLL=72M, MCU_PERI=72M, DSP=72M CLOCK_MODE_DOS
  {	16|64, 0,    0,    204, C_T0_48MhzMSB, C_T0_48MhzLSB}, // 5=SPI  PLL=48M, MCU_PERI=48M, DSP=48M CLOCK_MODE_SPI
  {	16|64, 1,    0,    230, C_T0_24MhzMSB, C_T0_24MhzLSB}, // 6=REC  PLL=48M, MCU_PERI=24M, DSP=48M CLOCK_MODE_REC
};

void set_clock_mode(U8 mode)
{
	if(gc_clock_mode==mode)
	{
		return;
	}

	if(gac_clock_table[gc_clock_mode][0]!=gac_clock_table[mode][0])
	{
		if(gc_clock_mode!=CLOCK_MODE_XTL)
		{
 			change_clock_ratio(CLOCK_MODE_XTL);
			XBYTE[0xB008]=0x00;	// PLLS_SWRSTN = 1, PLLS_MODE = 0(PLL clock output)
		}
    
		if(mode!=CLOCK_MODE_XTL)
		{
			XBYTE[0xB006]=gac_clock_table[mode][0];
			XBYTE[0xB006]=gac_clock_table[mode][0]|(1<<5);
		    USER_Delay(2);
			XBYTE[0xB008]=0;	// PLLS_SWRSTN = 1
			XBYTE[0xB008]=1;	// PLLS_MODE = 1(PLL clock output)
		    change_clock_ratio(mode);
    	}
	}
    else
    {
        change_clock_ratio(CLOCK_MODE_XTL);
        change_clock_ratio(mode);
    }
    TH1=gac_clock_table[mode][3];
    gc_T0IntervalMSB=gac_clock_table[mode][4];
    gc_T0IntervalLSB=gac_clock_table[mode][5];
    gc_clock_mode=mode;
}

void change_clock_ratio(U8 r)
{
    // disable auto wait
	XBYTE[0xB021]=0;
	XBYTE[0xB022]=1;
    // set new divide ratio
	XBYTE[0xB009]=0;	// cpu_mask_sel, per_mask_sel, dsp_mask_sel & n_mask
	XBYTE[0xB024]=gac_clock_table[r][1];	// CPUfreq
	XBYTE[0xB025]=gac_clock_table[r][1];	// PERfreq
	XBYTE[0xB026]=gac_clock_table[r][2];	// DSPfreq
    // Trigger the switch
	XBYTE[0xB020]=0;	// cpu_protect = dsp_protect = per_protect = 0
	XBYTE[0xB020]=1;	// freqchange = 1
}

void Key_Detect(void)
{
	EA=0;
	gc_KeyEvent=gc_KeyValue;
	gc_KeyValue=0;
	if(gc_KeyEvent!=0)
	{
		gc_LCMBacklightTimer=gc_LCMBacklightTime;
	}
	EA=1;
}


bit SD_Card_Detect(void)
{
	if(SD_Detect==0)
	{
		gb_CardDetect=1;
		return 1;
	}
    else
	{
		gb_CardDetect=0; 
//		XBYTE[0xB002]=0x08;
//		XBYTE[0xB002]=0x00;
        return 0;
    }   
}


bit CheckUSBPlugIN(void)
{
	XBYTE[0xB016]=0x04;		// power on default is 0x04
	XBYTE[0xB0E9]&=~0x01;	// Enable USB PLL CLK 
	XBYTE[0xB01D]=0x00;
	XBYTE[0xB002]|=0x06;
	XBYTE[0xB002]&=~0x06;
	XBYTE[0xB5E6]=0x00;		// bulksram gated clock
	XBYTE[0xB522]=0x01;		// USB2.0 UTMI clock mode set   
	XBYTE[0xB017]=0x01;		// USB2.0 UTMI op mode set
	XBYTE[0xB018]=0x01;		// USB clock select
	XBYTE[0xB019]=0x01;		// Host mode
	
	if(XBYTE[0xB615]==0x01)
	{
		return 1;
	}
	return 0;
}


bit Host_DetectDevice(void)
{
    bit sts;

    if(gc_CurrentCard==5)
    {
        if(gb_HostConnect==0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        sts=CheckUSBPlugIN();
    }

    if(sts) 
    {
        return 0;
    }
    else
    {
		gb_HostConnect=0;
        return 1;
    }
}


bit HOST_Check(void)
{
	if(!Host_DetectDevice())
	{
		gb_Host_Exist=1;
		if(!Host_Initial())
		{
			gc_CurrentCard=5;
			gb_FindFlag=0;
			return 0;
		}
	}
	gb_Host_Exist=0;
	return 1; //Error
}


bit SD_Check(void)       
{
 	if(SD_Card_Detect())
	{
		gb_SD_Exist=1;
		if(SD_Identification_Flow())
		{
			gc_CurrentCard=2;
			gb_FindFlag=0;
			return 0; //Success
		}
	}
	gb_SD_Exist=0;
	return 1; //Error
}


U8 READ_SARADC(U8 tc_ADCChannel)
{
	xdata	U8	tc_ADCValue;
	xdata	U8	tc_BattLevelTemp;
	
	if(tc_ADCChannel==2)
	{
		XBYTE[0xB094]=0x01;
	}
	//=========SAR ADC init s===========   
	XBYTE[0xB060]=(tc_ADCChannel<<2)|0x01;
	//=========SAR ADC init f===========
	XBYTE[0xB05E]=1;// enable SAR FS clock
	for(tc_ADCValue=0;tc_ADCValue<150;tc_ADCValue++); 
	while(XBYTE[0xB061]&0x01==0);
	tc_ADCValue=XBYTE[0xB062];
	XBYTE[0xB05E]=0; // disable SAR FS clock
	if(tc_ADCChannel==2)
	{
		XBYTE[0xB094]=0x00;
		tc_BattLevelTemp=gc_BatteryLevel;
		if(tc_ADCValue>0x75)	// 3.7V
		{
			gc_BatteryLevel=9;
		}
		else if(tc_ADCValue>0x73)	// 3.6V
		{
			gc_BatteryLevel=7;
		}
		else if(tc_ADCValue>0x70)	// 3.5V
		{
			gc_BatteryLevel=5;
		}
		else if(tc_ADCValue>0x6C)	// 3.4V
		{
			gc_BatteryLevel=3;
		}
		else if(tc_ADCValue>0x69)	// 3.3V
		{
			gc_BatteryLevel=1;
		}
		else
		{
			gc_BatteryLevel=0;
		}
		if(tc_BattLevelTemp!=gc_BatteryLevel)
		{
			gb_ShowBatt=1;
		}
	}
	return tc_ADCValue;
}


void ScanKey(void)
{
	if(gc_KeyValue==0)
	{
		#ifdef EVB_128
		if(P1&0x08==0x00)
		{
			gc_key_Pressed=C_Key_Play;
		}
		else
		#endif
		{
			Get_KeyValue(READ_SARADC(0));
		}

		if(gc_key_PrevPressed==0)
		{
			if(gc_key_Pressed!=0)	// 穝龄
			{
				gc_key_PrevPressed=gc_key_Pressed;
				gc_LongKeyCount=1;
				return;
			}
			else	// 礚龄
			{
				gc_LongKeyCount=0;
				return;
			}
		}
		else
		{
			// 30*30mS=900mS  ->  Long key confirm
			// 3*30mS=90mS    ->  1-Long Key
			if(gc_key_Pressed==gc_key_PrevPressed)
			{
				if(gc_LongKeyCount<34)
				{
					gc_LongKeyCount++;
					if(gc_LongKeyCount==34)
					{
						gc_KeyValue=0x10|gc_key_PrevPressed;	// Long Key pressed
					}
				}
			}
			else if(gc_key_Pressed==0)
			{
				if(gc_LongKeyCount>19)
				{
					gc_KeyValue=0x20|gc_key_PrevPressed;	// Long Key released
				}
				else
				{
					gc_KeyValue=gc_key_PrevPressed;			// Short Key pressed
				}
				gc_key_PrevPressed=0;
			}
		}
	}
}


void Get_KeyValue(U8 ADCValue)
{
	if(ADCValue>246)
	{ 		
		gc_key_Pressed=0;
	}	
	else if(ADCValue>220)
	{	
		gc_key_Pressed=8;	
	}
	else if(ADCValue>183)
	{	
		gc_key_Pressed=7;	
	}
	else if(ADCValue>140)
	{	
		gc_key_Pressed=6;	
	}
	else if(ADCValue>103)
	{
		gc_key_Pressed=5;	
	}
	else if(ADCValue>74)
	{
		gc_key_Pressed=3;		
	}
	else if(ADCValue>30)
	{
		gc_key_Pressed=4;				
	}
	else
	{
		gc_key_Pressed=2;
	}
}

bit CheckAlarmSetting(void)
{
	if(gb_AlarmRepeatON==0)
	{
		gb_AlarmON=0;
		return 0;
	}
	else
	{
		if(gc_AlarmDaysSelect==0)
		{
			gb_AlarmRepeatON=0;
			gb_AlarmON=0;
			return 0;
		}
		else
		{
			RTC_Calendar();
			if(gc_Week==0&&gc_AlarmDaysSelect&0x01)
			{
				return 0;
			}
			else if(gc_Week==1&&gc_AlarmDaysSelect&0x02)
			{
				return 0;
			}
			else if(gc_Week==2&&gc_AlarmDaysSelect&0x04)
			{
				return 0;
			}
			else if(gc_Week==3&&gc_AlarmDaysSelect&0x08)
			{
				return 0;
			}
			else if(gc_Week==4&&gc_AlarmDaysSelect&0x10)
			{
				return 0;
			}
			else if(gc_Week==5&&gc_AlarmDaysSelect&0x20)
			{
				return 0;
			}
			else if(gc_Week==6&&gc_AlarmDaysSelect&0x40)
			{
				return 0;
			}
		}
	}
	gb_AlarmTrigger=0;
	return 1;	// Not Match, Do not Alarm
}
//只做了30（2010～2039）年的日历，所以瑞年的算法是四年一瑞，请注意。
void RTC_Calendar(void)
{
	data	U8	i;
	data	U16	tw_Days;
	code	U16	K_Year[4]={365,365,366,365};
	code	U8	K_Month[12]={31,28,31,30,31,30,31,31,30,31,30,31};

	gc_Sec=gdw_RTC_Second%60;
	gc_Min=(gdw_RTC_Second/60)%60;
	gc_Hour=(gdw_RTC_Second/3600)%24;
	tw_Days=gdw_RTC_Second/86400+1;
	gc_Week=(tw_Days+3)%7;
	gc_Year=0;
	i=0;
	while(1)
	{
		if(tw_Days<=K_Year[i&0x03])
		{
			break;
		}
		tw_Days-=K_Year[i&0x03];
		i++;
		gc_Year++;
	}
	gc_Month=1;
	i=0;
	while(1)
	{
		if((i==1)&&((gc_Year+2)&3==0))
		{
			if(tw_Days>29)
			{
				tw_Days-=29;
				i++;
				gc_Month++;
			}
		}
		else if(tw_Days>K_Month[i])
		{
			tw_Days-=K_Month[i];
			i++;
			gc_Month++;
		}
		else
		{
			break;
		}
	}
	gc_Day=(U8)tw_Days;
}
