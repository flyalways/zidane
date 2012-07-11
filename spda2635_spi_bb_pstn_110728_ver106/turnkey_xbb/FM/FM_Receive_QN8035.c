
#define QN8035_RD_CTRL   0x21 
#define QN8035_WR_CTRL   0x20 

#define QN8035_ID   0x20 
///////////////////////

#ifdef QND_SUNPLUS_C51
#define BOOL bit
#endif

#define PILOT_CCA                       1  // 0: disable stereo judgement for the country has many mono FM stations
#define USING_VALID_CH                  1
#define CCA_PILOT_SNR_FILTER            35//25 

#define CCS_RX                          0
#define CCS_TX                          1

//#define FREQ2CHREG(freq)                ((freq-6000)/5)
//#define CHREG2FREQ(ch)                  (ch*5+6000)

#define _QNCOMMON_H_
#define QND_REGISTER_NAME_ENABLE
#define QND_VARIABLE_NAMING_V2
//#define QND_CUSTOMER
#define FALSE                           0
#define QND_FAIL                        0

#define QND_REG_NUM_MAX                 85  
/**********************************QN8035's clock source selection**************
1.QN8035's default clock source is 32768HZ.
2.setting QN8035's clock source and clock source type(like sine-wave clock or digital clock).
3.user need to modify clock source according to actual hardware platform.
4.clock formula,the details please refer to the QN8035's datasheet 
  XTAL_DIV = Round(Clock/32768);
  PLL_DLT = Round((28500000*512*XTAL_DIV)/Clock)-442368
*******************************************************************************/
#define QND_SINE_WAVE_CLOCK             0x00    //inject sine-wave clock                                //32768 sin
#define QND_DIGITAL_CLOCK               0x80    //inject digital clock,default is inject digital clock	//32748 square
//crystal clock is 32768HZ
#define QND_XTAL_DIV0                   0x01
#define QND_XTAL_DIV1                   0x08
#define QND_XTAL_DIV2                   0x5C

#define QND_MODE_SLEEP                  0     
#define QND_MODE_WAKEUP                 1

// RX / TX value is using upper 8 bit
#define QND_MODE_RX                     0x8000
#define QND_MODE_TX                     0x4000
// AM / FM value is using lower 8 bit 
// need to check datasheet to get right bit 
#define QND_MODE_FM                     0x0000

#define BAND_FM                         0

// tune
#define QND_FSTEP_50KHZ                 0
#define QND_FSTEP_100KHZ                1
#define QND_FSTEP_200KHZ                2

#define QND_STEP_CONSTANT               0x40 //(QND_FSTEP_100KHZ << 6)
#define CHIPID_QN8035                   0x84

/*******************************************************************************
Performance configuration 
*******************************************************************************/
#define SMSTART_VAL                     19
#define HCCSTART_VAL                    25
#define SNCSTART_VAL                    51

/*******************************************************************************
limitation configuration 
*******************************************************************************/
#define QND_READ_RSSI_DELAY             100
#define SLEEP_TO_WAKEUP_DELAY_TIME      500       
#define CH_SETUP_DELAY_TIME             300       
/***************************************End************************************/

/*******************************************************************************
 definition register 
*******************************************************************************/
#define SYSTEM1         				0x00
#define CCA             				0x01
#define SNR            					0x02
#define RSSISIG         				0x03
#define STATUS1         				0x04
#define CID1            				0x05
#define CID2            				0x06
#define	CH								0x07
#define	CH_START						0x08
#define	CH_STOP							0x09
#define	CH_STEP							0x0A
#define	RDSD0							0x0B
#define	RDSD1							0x0C
#define	RDSD2							0x0D
#define	RDSD3							0x0E
#define	RDSD4							0x0F
#define	RDSD5							0x10
#define	RDSD6							0x11
#define	RDSD7							0x12
#define	STATUS2							0x13
#define	VOL_CTL							0x14
#define	XTAL_DIV0						0x15
#define	XTAL_DIV1						0x16
#define	XTAL_DIV2						0x17
#define INT_CTRL						0x18
#define SMP_HLD_THRD					0x19
#define	RXAGC_GAIN						0x1A
#define GAIN_SEL						0x1B
#define	SYSTEM_CTL1						0x1C
#define	SYSTEM_CTL2						0x1D
#define RDSCOSTAS						0x1E
#define REG_TEST						0x1F
#define STATUS4							0x20
#define	CCA1							0x27
#define PLT1                            0X2F
#define	PLT2                            0x30
#define	SMSTART							0x34
#define	SNCSTART						0x35
#define	HCCSTART						0x36
#define	CCA_CNT1					    0x37
#define	CCA_CNT2					    0x38
#define	CCA_SNR_TH_1					0x39
#define	CCA_SNR_TH_2					0x3A
#define NCCFIR3         				0x40
#define REG_REF                         0x49
#define REG_DAC							0x4C
/***************************************End************************************/

/*******************************************************************************
 definition operation bit of register
*******************************************************************************/
#define CCA_CH_DIS      				0x01
#define CHSC            				0x02
#define RDSEN           				0x08
#define CH_CH		    				0x03
#define CH_CH_START     				0x0c
#define CH_CH_STOP      				0x30
#define STNBY_MODE           		    0x20
#define RX_MODE         				0x10
#define IMR             				0x40
#define RDS_RXUPD       				0x80
#define ST_MO_RX                        0x01
#define STNBY_RX_MASK                   0x30
#define RXCCA_MASK                      0x03
#define RX_CCA                          0x02
#define RXCCA_FAIL                      0x08
#define RX_MONO                         0x04
#define ICPREF                          0x0F
/***************************************End************************************/
//////////////////////////////==========================================
U8 QND_ReadReg(U8 adr)
{
	U8 QNData;
	
	I2C_Read_nbyte(QN8035_ID,adr,&QNData,1);
	return QNData;
}

void QND_WriteReg(U8 adr, U8 value)
{
	I2C_Write_nbyte(QN8035_ID,adr,&value,1);
}
U16 FREQ2CHREG(U16 freq)
{
    return 	(freq-6000)/5;
}

void QNF_SetMute(U8 On)
{
    QND_WriteReg(REG_DAC, On?0x1B:0x10);        
}  


/**********************************************************************
U8 QNF_SetCh(U16 freq)
**********************************************************************
Description: set channel frequency 

Parameters:
    freq:  channel frequency to be set
Return Value:
    1: success
**********************************************************************/
void QNF_SetCh(U16 freq) 
{
    // calculate ch parameter used for register setting
    U8 tStep;
    U8 tCh;
    U16 f;    

	//Manually set RX Channel index
	//QNF_SetRegBit(SYSTEM1, CCA_CH_DIS, CCA_CH_DIS);
	//entering RX mode and disable CCA,channel index decides by manually input
	QND_WriteReg(0x00, 0x11);
    f = FREQ2CHREG(freq); 
    // set to reg: CH
    tCh = (U8) f;
    QND_WriteReg(CH, tCh);
    // set to reg: CH_STEP
    tStep = QND_ReadReg(CH_STEP);
    tStep &= ~CH_CH;
    tStep |= ((U8) (f >> 8) & CH_CH);
    QND_WriteReg(CH_STEP, tStep);    
}

/**********************************************************************
void QNF_ConfigScan(U16 start,U16 stop, U8 step)
**********************************************************************
Description: config start, stop, step register for FM/AM CCA or CCS

Parameters:
    start
        Set the frequency (10kHz) where scan to be started,
        eg: 7600 for 76.00MHz.
    stop
        Set the frequency (10kHz) where scan to be stopped,
        eg: 10800 for 108.00MHz
    step        
        1: set leap step to (FM)100kHz / 10kHz(AM)
        2: set leap step to (FM)200kHz / 1kHz(AM)
        0:  set leap step to (FM)50kHz / 9kHz(AM)
Return Value:
         None
**********************************************************************/
#if USING_VALID_CH
void QNF_ConfigScan(U16 freq) 
{
    // calculate ch para
    U8 tStep = 0;
    U8 tS;
    U16 fStart;
    
    fStart = FREQ2CHREG(freq);
    // set to reg: CH_START
    tS = (U8) fStart;
    QND_WriteReg(CH_START, tS);
    tStep |= ((U8) (fStart >> 6) & CH_CH_START);
    // set to reg: CH_STOP
 //   tS = (U8) fStart;
    QND_WriteReg(CH_STOP, tS);
    tStep |= ((U8) (fStart >> 4) & CH_CH_STOP);
    // set to reg: CH_STEP
//    tStep |= QND_FSTEP_100KHZ << 6;
    tStep |= QND_STEP_CONSTANT;
    QND_WriteReg(CH_STEP, tStep);
}
#else
void QNF_ConfigScan(U16 start,U16 stop) 
{
    // calculate ch para
    U8 tStep = 0;
    U8 tS;
    U16 fStart;
    U16 fStop;
    
    fStart = FREQ2CHREG(start);
    fStop = FREQ2CHREG(stop);
    // set to reg: CH_START
    tS = (U8) fStart;
    QND_WriteReg(CH_START, tS);
    tStep |= ((U8) (fStart >> 6) & CH_CH_START);
    // set to reg: CH_STOP
    tS = (U8) fStop;
    QND_WriteReg(CH_STOP, tS);
    tStep |= ((U8) (fStop >> 4) & CH_CH_STOP);
    // set to reg: CH_STEP
    tStep |= QND_FSTEP_100KHZ << 6;
    QND_WriteReg(CH_STEP, tStep);
}
#endif 

///////////////////////////////////////////
/*U8 QND_Read_ID(void)
{
	U8 temp_Read;
	
	QND_WriteReg(0x00, 0x81);
	IIC_Dealy(2);
	temp_Read = QND_ReadReg(CID2);

	return temp_Read;	
}*/

void QND_RXConfigAudio(U8 option ) //0~47 lever vol
{
    U8 regVal;
    regVal = QND_ReadReg(VOL_CTL);
	regVal = (regVal&0xC0)|(option/6)|(5-(option%6)<<3);
	QND_WriteReg(VOL_CTL,regVal); 
}
/**********************************************************************
void QND_Init()
**********************************************************************
Description: Initialize device to make it ready to have all functionality ready for use.

Parameters:
    None
Return Value:
    1: Device is ready to use.
    0: Device is not ready to serve function.
**********************************************************************/
void QND_Init(void)
{	
dbprintf("qn8035 init\n");
    QND_WriteReg(0x00, 0x81); 
    IIC_Dealy(2);
    // change crystal frequency setting here
	QND_WriteReg(0x01,QND_DIGITAL_CLOCK);
//	QND_WriteReg(XTAL_DIV0, QND_XTAL_DIV0);
//	QND_WriteReg(XTAL_DIV1, QND_XTAL_DIV1);
//	QND_WriteReg(XTAL_DIV2, QND_XTAL_DIV2);  
    IIC_Dealy(2);   
    QND_WriteReg(0x54, 0x47);//mod PLL setting
	QND_WriteReg(0x19, 0x40);//AGC setting	
    QND_WriteReg(0x2d, 0xD6);//notch filter threshold adjusting
    QND_WriteReg(0x43, 0x10);//notch filter threshold enable 
	QND_WriteReg(0x47,0x39); 	
    //enter receiver mode directly
    QND_WriteReg(0x00, 0x11);
	//Enable the channel condition filter3 adaptation,Let ccfilter3 adjust freely  
    QND_WriteReg(0x1D,0xA9);     
  //  QNF_SetMute(1);
}
 
void QNF_PowerDown(void)
{
	U8 temp_Read;
	
	temp_Read = QND_ReadReg(REG_DAC);    //make sure Power down control by FSM control
	temp_Read &= (~0x08);
	QND_WriteReg(REG_DAC,temp_Read);
	
	temp_Read = QND_ReadReg(SYSTEM1);
	temp_Read &= (~STNBY_RX_MASK);
	temp_Read |= STNBY_MODE;	
	QND_WriteReg(SYSTEM1,temp_Read);
    ///QNF_SetRegBit(SYSTEM1, STNBY_RX_MASK, STNBY_MODE);
}
/**********************************************************************
void QND_TuneToCH(U16 ch)
**********************************************************************
Description: Tune to the specific channel. call QND_SetSysMode() before 
call this function
Parameters:
ch
Set the frequency (10kHz) to be tuned,
eg: 101.30MHz will be set to 10130.
Return Value:
None
**********************************************************************/
void QND_TuneToCH() 
{   
    U8 reg;
	U16 ch;
   ch=gw_FM_frequency*10;
   dbprintf("ch=%x\n",ch); 
	//increase reference PLL charge pump current.
    QND_WriteReg(REG_REF,0x7A);
	
	/********** QNF_RXInit ****************/
    QND_WriteReg(0x1B,0x70);  //Let NFILT adjust freely
	//QNF_SetRegBit(0x2C,0x3F,0x12);  ///When SNR<ccth31, ccfilt3 will work
	//setting the threshold of Filter3 will be worked.
	QND_WriteReg(0x2C,0x52);	
	//QNF_SetRegBit(0x1D,0x40,0x00);  ///Let ccfilter3 adjust freely
	//QNF_SetRegBit(0x41,0x0F,0x0A);  ///Set a hcc index to trig ccfilter3's adjust
    QND_WriteReg(0x45,0x50);        ///Set aud_thrd will affect ccfilter3's tap number
	//QNF_SetRegBit(0x40,0x70,0x70);  ///snc/hcc/sm snr_rssi_sel; snc_start=0x40; hcc_start=0x30; sm_start=0x20
	QND_WriteReg(0x40,0x70);
	//QNF_SetRegBit(0x19,0x80,0x80);  ///Use SNR for ccfilter selection criterion
	//selecting SNR as filter3 filter condition
	QND_WriteReg(0x19, 0xC2);
	//QNF_SetRegBit(0x3E,0x80,0x80);  ///it is decided by programming this register
	//QNF_SetRegBit(0x41,0xE0,0xC0);  ///DC notching High pass filter bandwidth; remove low freqency dc signals        
    QND_WriteReg(0x41,0xCA);
	QND_WriteReg(0x34,SMSTART_VAL); ///set SMSTART
    QND_WriteReg(0x35,SNCSTART_VAL); ///set SNCSTART
    QND_WriteReg(0x36,HCCSTART_VAL); ///set HCCSTART
	/********** End of QNF_RXInit ****************/
	
    QNF_SetMute(1); 
  // dbprintf("ch=%x\n",ch); 

    QNF_ConfigScan(ch);      
 //  dbprintf("ch=%x\n",ch); 

    QNF_SetCh(ch);   
    //enable CCA mode
    QND_WriteReg(0x00, 0x13); 
#if 1
    //Auto tuning
    QND_WriteReg(0x4F, 0x80);
    reg = QND_ReadReg(0x4F);
    reg >>= 1;
    QND_WriteReg(0x4F, reg);
#endif 
    ///avoid the "POP" noise.
    //Delay_Time_ms(CH_SETUP_DELAY_TIME);
	IIC_Dealy(CH_SETUP_DELAY_TIME/7+1);
    ///decrease reference PLL charge pump current.
    QND_WriteReg(REG_REF,0x70);   
	LCM_ShowFrequency_L();
   	QNF_SetMute(0);  //unmute

}

/***********************************************************************
void QND_RXSetTH(UINT8 th)
***********************************************************************
Description: Setting the threshold value of automatic scan channel 
th:
  Setting threshold for quality of channel to be searched,
  the range of th value:CCA_SENSITIVITY_LEVEL_0 ~ CCA_SENSITIVITY_LEVEL_9   
Return Value:
  None
***********************************************************************/
void QND_RXSetTH(U8 th) 
{ 
    ///increase reference PLL charge pump current.
    QND_WriteReg(REG_REF,0x7A);
	//NFILT program is enabled 
    QND_WriteReg(0x1B,0x78);
	//using Filter3
    QND_WriteReg(CCA1,0x75);    
    //setting CCA IF counter error range value(256).
    QND_WriteReg(CCA_CNT2,0x01);
#if PILOT_CCA   
    QND_WriteReg(PLT1,0x00); 
#endif    
	//selection the time of CCA FSM wait SNR calculator to settle:20ms
	//0x00:	    20ms(default)
	//0x40:	    40ms
	//0x80:	    60ms
	//0xC0:	    100m
	//    QNF_SetRegBit(CCA_SNR_TH_1 , 0xC0, 0x00);     
    //selection the time of CCA FSM wait RF front end and AGC to settle:20ms
    //0x00:     10ms
	//0x40:     20ms(default)
    //0x80:     40ms
	//0xC0:     60ms                            
	//    QNF_SetRegBit(CCA_SNR_TH_2, 0xC0, 0x40); 
	//    QNF_SetRegBit(CCA, 30);  //setting CCA RSSI threshold is 30
	QND_WriteReg(CCA_SNR_TH_2,0x85);       //adam 20110113
	QND_WriteReg(CCA,QND_ReadReg(CCA)&0xC0|30);
#if PILOT_CCA	
	QND_WriteReg(CCA_SNR_TH_1,8+th); //setting SNR threshold for CCA    8 adam 
#else
	QND_WriteReg(CCA_SNR_TH_1,10+th); //setting SNR threshold for CCA	
#endif	
}
		 


/***********************************************************************
UINT16 QND_RXValidCH(UINT16 freq, UINT8 step);
***********************************************************************
Description: to validate a ch (frequency)(if it's a valid channel)
Freq: specific channel frequency, unit: 10Khz
  eg: 108.00MHz will be set to 10800.
Step:  
  FM:
  QND_FMSTEP_100KHZ: set leap step to 100kHz
  QND_FMSTEP_200KHZ: set leap step to 200kHz
  QND_FMSTEP_50KHZ:  set leap step to 50kHz
Return Value:
  0: not a valid channel
  other: a valid channel at this frequency
***********************************************************************/
U8 QND_RXValidCH(U16 freq) 
{
	U8 regValue;
	U8 timeOut;
#if PILOT_CCA    
	U8 snr,readCnt,stereoCount=0;
#endif
#if USING_VALID_CH
	QNF_ConfigScan(freq);
#else
	QNF_ConfigScan(freq, freq);
#endif 
	//QNF_SetRegBit(SYSTEM1,0x03,0x02);//enter channel scan mode,channel frequency is decided by internal CCA
	//entering into RX mode and CCA mode,channels index decide by CCA.
	QND_WriteReg(0x00, 0x12);
	timeOut = 250;
	do
	{
		regValue = QND_ReadReg(SYSTEM1);
		IIC_Dealy(1);   //delay 1ms
		timeOut --;
	} while((regValue & CHSC) && timeOut);//when seeking a channel or time out,be quited the loop
	regValue = QND_ReadReg(0x04)& 0x08;//reading the rxcca_fail flag of RXCCA status 
	
	if(!regValue)
	{
#if PILOT_CCA
		IIC_Dealy(15);  
		snr = QND_ReadReg(SNR);       
		if(snr> 25) return 1;		
		for(readCnt=10;readCnt>0;readCnt--)
		{
			IIC_Dealy(2);
			stereoCount += ((QND_ReadReg(STATUS1) & ST_MO_RX) ? 0:1); 	      
			if(stereoCount >= 3) return 1;                 
		} 	
#else
		return 1;   
#endif
	}
	return 0;
}	

xdata U16 gw_search_freq;

void QN8035_FMSeekFromCurrentCh(U8 seekUp)
{
//	unsigned int tmpfreq;
	U8   finefreq;
	
	///Amplifier_Switch(0);
	QNF_SetMute(1);
	///if(MP3PlayerOptionMode & Mode_PGASET)
	///{
	///    PGA_RampDown();         // set PGA ramp down
	///}
	///else
	///{
	///	SetSYSCtrlIO(C_SYSCTRL_MP3);     // switch to DAC channel
	///}
	gw_search_freq = gw_FM_frequency*10;
	QND_RXSetTH(0);
    do
	{
		gw_search_freq += seekUp ? 10 : -10;
//		DisplayFM_12x32(tmpfreq/10);
//		RefeshLCD_1621();	
gw_FM_frequency=gw_search_freq/10;
		LCM_ShowFrequency_L();
		finefreq = QND_RXValidCH(gw_search_freq);
   	}while(finefreq == 0 && gw_search_freq > 8750 && gw_search_freq < 10800);

	///if(MP3PlayerOptionMode & Mode_PGASET)
	///{
    ///    PGA_RampUp();      // set PGA ramp up
	///}	
	///else
	///{
	///	SetSYSCtrlIO(C_SYSCTRL_FM);     // switch to FM channel
	///}
	///Amplifier_Switch(1);
	
//	DisplayFM_12x32(tmpfreq/10);
//	#ifdef LCD_4x32_USE
//	RefeshLCD_1621();
//	#endif	//fdef LCD_4x32_USE
   	gw_FM_frequency = gw_search_freq/10;
	QND_TuneToCH();

//	return finefreq;
}

//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------
//--sunzhk add search all freq
 void QN8035_FMSeekAllCh(U16 TunedFrequency)
{
	U8 finefreq;
	
	gc_FM_CHAmount=0;
	QNF_SetMute(1);	
	gw_search_freq = TunedFrequency*10;
	while(1)
	{
		QND_RXSetTH(0);

		gw_search_freq += 10;
		gw_FM_frequency=gw_search_freq/10;
		LCM_ShowFrequency_L();
		finefreq = QND_RXValidCH(gw_search_freq);
		if(finefreq == 1&& gw_search_freq > 8750 && gw_search_freq < 10800)
		{
			gw_FM_CH[gc_FM_CHAmount]=gw_search_freq/10;
			dbprintf("find freq,gw_FM_CH[%bx]=%x\n",gc_FM_CHAmount,gw_FM_CH[gc_FM_CHAmount]);
			gc_FM_CHAmount++;	
		}

		if(gw_search_freq==10800 || gc_FM_CHAmount>=30) //total 30 ch
		{
		//	dbprintf("search end\n");
			break;
		}
	}


	if(gc_FM_CHAmount==0)
	{
		gw_FM_frequency= 875;
	}
	else
	{
		gw_FM_frequency = gw_FM_CH[0]; 
	}

	QND_TuneToCH();

//	return finefreq;
}
