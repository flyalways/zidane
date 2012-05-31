#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\header\dos\fs_struct.h"
#include "..\IR\remote.h"
#include "..\IR\FM.h"
#include "..\header\variables.h"

//void	FM_FREQ_CHG(U8 tc_UP_DOWN,U8 offset);
void	FM_drive(void);
void	FM_initial(void);
void	I2C_ZI2020_Send_data(U8 add,U8 dat);
void	I2C_ZI2020_SendByte(U8 dat);
void	I2C_ZI2020_StartBit(void);
void	I2C_ZI2020_StopBit(void);
void	IIC_Start(void);
void	IIC_Stop(void);
void	IIC_SendAck(bit k);
void	AS6200_init(void);
void	AS6200_Write_Data(U8 Reg,U16 Reg_val);
void	AS6200_Set_Freq(U16 frq);
void	AS6200_Read_Freq(void);
void 	I2C_Wait(U16 tt); //chiayen0805
void 	FM_FREQ_CHG(U8 tc_UP_DOWN,U8 offset);
U8		IIC_WriteByte(U8 addr);
U8		IIC_ReadByte(U8 ack);
U16		AS6200_Read_Data(U8 Reg); //AS6200 read data according to Reg
unsigned int I2C_ZI2020_read_data(U8 add);
//===================8027Home======================
xdata	U8 qnd_i2c;
xdata	U8 qnd_i2c_timeout;
//====================================================


void FM_FREQ_CHG(U8 tc_UP_DOWN,U8 offset)	// 1=UP  0=DOWN
{
	gb_Frequency_Song=0;	// 0=Show Frequency    1=Show Song Number/EQ
	gc_SelectVol=0;			// 1=Show Vol
	gb_SelectEQ=0;			// 0=Show Song Number  1=Show EQ
	gc_ShowTimer=0;
	gb_ChannelSet=0;
	gc_TuneVolFreqStatus=2;
	gc_ShowTimer=72;

	if(tc_UP_DOWN==1)
	{
		gw_FM_frequency+=offset;
		if(gw_FM_frequency>1080)
		{
			gw_FM_frequency=gw_FM_frequency-206;
		}
	}
	else
	{
		gw_FM_frequency-=offset;
		if(gw_FM_frequency<875)
		{
			gw_FM_frequency=206+gw_FM_frequency;
		}
	}
	FM_drive();
	gw_IR_Setting_Timer=120;
	gb_IR_Setting=1;
}


void FM_drive(void)
{
	U8  tc_FMModule_Type;
	U16	tw_FM_frequency;

	gw_LogData_Timer=60; //chiayen0812
	tc_FMModule_Type=FM_MODULE;

//	if(tc_FMModule_Type==FM_ZI2020)	// ZI2020
	{
	    tw_FM_frequency=0xF000|gw_FM_frequency;

	    I2C_ZI2020_Send_data(0x00,(U8)tw_FM_frequency);
    	I2C_ZI2020_Send_data(0x01,tw_FM_frequency>>8);
	}

//	else if(tc_FMModule_Type==FM_AS6300)	// AS6300
	{
		AS6200_Set_Freq(gw_FM_frequency);
		AS6200_Read_Freq();
	}
//	else if(tc_FMModule_Type==FM_QN8027)	// QN8027
	{
        QND_TuneToCH(gw_FM_frequency);
	}
//	else if(tc_FMModule_Type==FM_BK1085)	// BK1085
	{
                SetFrequency(gw_FM_frequency);
	}

}


void FM_initial(void)
{
	U8  tc_FMModule_Type,REG1,REG2;

	tc_FMModule_Type=FM_MODULE;
//	if(tc_FMModule_Type==FM_AS6300)// AS6300 //chiayen0805
	{
		AS6200_init();
	}
//	else if(tc_FMModule_Type==FM_ZI2020)	// ZI2020
	{
		/*FM_SCL=1;
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
		I2C_Wait(10);*/

		//共12M晶振//KT0803L 
		REG1=I2C_ZI2020_read_data(0x17);
		I2C_ZI2020_Send_data(0x17,REG1|0x08);
		I2C_ZI2020_Send_data(0x1E,0x43);
		REG2=I2C_ZI2020_read_data(0x0D);
		I2C_ZI2020_Send_data(0x0D,REG2|0x02);
		I2C_Wait(50);
	}
//	else if(tc_FMModule_Type==FM_QN8027)	// QN8027
	{   
		QND_Init();   
	}
//	else if(tc_FMModule_Type==FM_BK1085)	// BK1085
	{
		Chip_initialization();
	}

}

unsigned int I2C_ZI2020_read_data(U8 add) //AS6200 read data according to Reg
{
	unsigned char  value_l,value_h;
	unsigned int temp;
//	addr = 	((Reg)<<1)|0x81;

    I2C_ZI2020_StartBit();		// I2C start
	I2C_ZI2020_SendByte(0x7c);	// I2C WR command
	I2C_ZI2020_SendByte(add);	// I2C address
//    I2C_ZI2020_StopBit();		// I2C stop

	I2C_ZI2020_StartBit();		// I2C start
	I2C_ZI2020_SendByte(0x7D);	// I2C WR command
    value_l = IIC_ReadByte(1);
	value_h = IIC_ReadByte(1);
    I2C_ZI2020_StopBit();		// I2C stop

	temp = (unsigned int)(value_h<<8)+value_l;
//	dbprintf("add==%bx\n",add);
//	dbprintf("temp==%x\n",temp);
	return temp;
	
}
void I2C_ZI2020_Send_data(U8 add,U8 dat)
{
	I2C_ZI2020_StartBit();		// I2C start
	I2C_ZI2020_SendByte(0x7C);	// I2C WR command
	I2C_ZI2020_SendByte(add);	// I2C address
	I2C_ZI2020_SendByte(dat);	// I2C data
	I2C_ZI2020_StopBit();		// I2C stop
}


void I2C_ZI2020_StartBit(void)
{
	FM_SCL=1;	// clk=1
	I2C_Wait(3);
	FM_SDA=1;	// dat=1
	I2C_Wait(3);
	FM_SDA=0;	// dat=0
	I2C_Wait(3);
	FM_SCL=0;	// clk=0
	I2C_Wait(3);
}


void I2C_ZI2020_StopBit(void)
{
	FM_SDA=0;	// dat=0
	I2C_Wait(3);
	FM_SCL=1;	// clk=1
	I2C_Wait(3);
	FM_SDA=1;	// dat=1
	I2C_Wait(3);
}


void I2C_ZI2020_SendByte(U8 dat)
{
	U8 i;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)
		{
			FM_SDA=1;	// dat=1
		}
		else 
		{
			FM_SDA=0;	// dat=0
		}
		dat<<=1;
		I2C_Wait(3);
		FM_SCL=1;		// clk=1
		I2C_Wait(3);
		FM_SCL=0;		// clk=0
		I2C_Wait(3);
	}
	XBYTE[0xB102]&=0xFD;// P1.1=Input
	FM_SDA=1;			// dat=1
	I2C_Wait(3);

	// Wait ACK
	FM_SCL=1;		// clk=0
	I2C_Wait(3);
	FM_SCL=0;		// clk=1
	I2C_Wait(3);

	XBYTE[0xB102]|=0x02;// P1.1=Output
}


U16	AS6200_Write_BUFF[20] =
{
	0x2E00,
	0x01D8,
	0x27D3,
	0x01C5,
	0x4E03,
	0x9CA0,			//R5
	0x8803,			//R6
	0x592C,
	0x0881,			//0x1881
	0x0246,
	0x0028,
	0x0018,
	0x1001,
	0x0600,
	0xE6DB,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
	0x0000,
};

unsigned int idata AS6200_Freq;
unsigned char idata AS6200_Powc;
unsigned char idata AS6200_Pc;


void AS6200_init(void)
{
	U8	cnt;
	for(cnt = 0; cnt < 20; cnt++)
	{
		AS6200_Write_Data(cnt, AS6200_Write_BUFF[cnt]);
	}	

	//Set to Stereo
	AS6200_Write_BUFF[13]|=TX_MASK_ST;
	AS6200_Write_Data(13,AS6200_Write_BUFF[13]);

	AS6200_Powc=10;	 //Set POWC to 5.6dB
	AS6200_Write_BUFF[10] = (AS6200_Write_BUFF[10] & TX_MSK_POWC) | AS6200_Powc;
	AS6200_Write_Data(10,AS6200_Write_BUFF[10]);

	AS6200_Freq = AS6200_Default_Freq;	//Set to Default Freq
	AS6200_Set_Freq(AS6200_Freq);

	AS6200_Powc=(unsigned char)(AS6200_Read_Data(10)&TX_MASK_POWC);	  //Read Powc
	AS6200_Pc=(unsigned char)(AS6200_Read_Data(9)&TX_MASK_PC);	  	//Read Pc	
}


void AS6200_Write_Data(unsigned char Reg, unsigned int Reg_val)  //AS6200 send data according to Reg
{
	U8	ack,addr;

	addr = ((Reg)<<1)|0x80;
	IIC_Start();
	ack = IIC_WriteByte(addr);
	ack |= IIC_WriteByte(Reg_val >> 8);
	ack |= IIC_WriteByte(Reg_val & 0xff);
	IIC_Stop();
}


U16 AS6200_Read_Data(unsigned char Reg) //AS6200 read data according to Reg
{
	U8	addr, value_l,value_h;
	U16	temp;

	addr = 	((Reg)<<1)|0x81;
	IIC_Start();
	IIC_WriteByte(addr);
	value_h = IIC_ReadByte(1);
	value_l = IIC_ReadByte(1);
	IIC_Stop();
	temp = (unsigned int)(value_h<<8)+value_l;
	return temp;
	
}


void AS6200_Set_Freq(U16 frq)
{  
	U8	data	rfpll_pb;
	U8	data	rfpll_sb;
	U16	data	temp;

	temp = frq-4;
	rfpll_sb  = temp % 15;
	rfpll_pb = (temp - rfpll_sb) / 15 - 1;
	AS6200_Write_BUFF[5] = (rfpll_pb << REGPOS_RFPLL_PB) | (AS6200_Write_BUFF[5] & TX_MSK_RFPLL_PB);

	AS6200_Write_BUFF[5] = ((rfpll_sb ) << REGPOS_RFPLL_SB_L) | (AS6200_Write_BUFF[5] & TX_MSK_RFPLL_SB_L);
	AS6200_Write_BUFF[6] = ((rfpll_sb ) >> REGPOS_RFPLL_SB_H) | (AS6200_Write_BUFF[6] & TX_MSK_RFPLL_SB_H);

	AS6200_Write_Data(5, AS6200_Write_BUFF[5]);
	AS6200_Write_Data(6, AS6200_Write_BUFF[6]);
}


void AS6200_Read_Freq(void)
{
	U16	data	temp1,temp2;
	U8	data	rfpll_p,rfpll_s;

	temp1 = AS6200_Read_Data(5);
	temp2 = AS6200_Read_Data(6);
	rfpll_p = (unsigned char)(temp1 >> REGPOS_RFPLL_PB);	
	rfpll_s = (unsigned char)(AS6200_Read_Data(5) >> REGPOS_RFPLL_SB_L) | ((unsigned char) (AS6200_Read_Data(6)&0x0007)<<REGPOS_RFPLL_SB_H);
	AS6200_Freq = 15*(rfpll_p+1)+rfpll_s+4;
}


void IIC_Delay(unsigned int t)
{
	while(0 != t)
	{	
		t--;
	}
}


void IIC_Start()				//IIC start command
{
  	IIC_SDA_High;    /*发送起始条件的数据信号*/
  	IIC_Delay(IIC_Delay_Time); 
  	IIC_SCL_High;		  /*起始条件建立时间大于4.7us,延时*/
       
  	IIC_Delay(IIC_Delay_Time); 
  	IIC_SDA_Low;    /*发送起始信号*/	 /* 起始条件锁定时间大于4μs*/
      
  	IIC_Delay(IIC_Delay_Time);        
  	IIC_SCL_Low;    /*钳住IIC总线，准备发送或接收数据 */
  	IIC_Delay(IIC_Delay_Time);  
}


void IIC_Stop()				//IIC stop command
{
	IIC_SDA_Low;   	/*发送结束条件的数据信号*/
    			/*发送结束条件的时钟信号*/
	IIC_Delay(IIC_Delay_Time); 
  	IIC_SCL_High;   /*结束条件建立时间大于4μs*/
	IIC_Delay(IIC_Delay_Time); 
  	IIC_SDA_High;   /*发送IIC总线结束信号*/
	IIC_Delay(IIC_Delay_Time); 
}


unsigned char IIC_WriteByte(unsigned char c)		//IIC send a byte data, return ack signal(0:无应答，1:有应答)
{
	U8	ack, i;

	XBYTE[0xB102]|=0x20;
	for(i = 0; i < 8; i++)
	{
		if((c << i) & 0x80)
			IIC_SDA_High;
		else
			IIC_SDA_Low;
		IIC_Delay(IIC_Delay_Time);
		IIC_SCL_High;
		IIC_Delay(IIC_Delay_Time);
		IIC_SCL_Low;
	}
	IIC_Delay(IIC_Delay_Time);
	IIC_SDA_Set_Input;			/*8位发送完后释放数据线，准备接收应答位*/
	IIC_Delay(IIC_Delay_Time);
	IIC_SCL_High;				//????
	IIC_Delay(IIC_Delay_Time);
	if(IIC_SDA_Test)			 /*判断是否接收到应答信号*/
		ack = 0;			////
	else
		ack = 1;
	IIC_SCL_Low;
	IIC_Delay(IIC_Delay_Time);
	IIC_SDA_Set_Output;
	return ack;
}


unsigned char IIC_ReadByte(unsigned char ack)		//IIC read a byte data, if ack = 1 ,send ack signal, if ack = 0, no send ack signal  
{
	unsigned char i, bits;
	bits = 0;
	IIC_SDA_Set_Input;
	for(i = 0; i < 8; i++)
		{
		IIC_SCL_Low;
		IIC_Delay(IIC_Delay_Time);
		IIC_SCL_High;
		IIC_Delay(IIC_Delay_Time);
		bits <<= 1;
		if(IIC_SDA_Test)
			bits += 1;
		IIC_Delay(IIC_Delay_Time);
		}
	IIC_SCL_Low;
	IIC_Delay(IIC_Delay_Time);
	IIC_SDA_Set_Output;
	if(ack == 1)
		IIC_SendAck(0);
	else
		IIC_SendAck(1);
	return bits;
		
}


void IIC_SendAck(bit k)
{
	if(k == 0)
		IIC_SDA_Low;
	else
		IIC_SDA_High;
	IIC_Delay(IIC_Delay_Time);
	IIC_SCL_High;
	IIC_Delay(IIC_Delay_Time);
	IIC_SCL_Low;
	IIC_Delay(IIC_Delay_Time);
}


void I2C_Wait(U16 tt)
{
	while(tt>0)
	{
		tt--;
	}
}
//==================================================================================================================
//		fmtx fuction end
//==================================================================================================================

//==========QN8027================
/////////////////////////////////////////////////////////////////////////


///////////////////////////////////8027zhanggw//////////////////////////////////////


void Start(void)               
{ 
    IIC_Delay(1);
    IIC_SCL_High;
    IIC_Delay(1);
    IIC_SDA_High;
    IIC_Delay(1);
    IIC_SDA_Low;
    IIC_Delay(2);
    IIC_SCL_Low;
}

void Stop(void)              
{
    IIC_Delay(1);
    IIC_SDA_Low;
    IIC_Delay(1);
    IIC_SCL_High;
    IIC_Delay(2);
    IIC_SDA_High;
}

void Send_ACK(U8 i) //i=0 for ACK and i=1 for Nack
{
    if(i == 0)
		IIC_SDA_Low;
	else
		IIC_SDA_High;
    IIC_Delay(1);
    IIC_SCL_High;
    IIC_Delay(1);
    IIC_SCL_Low;
    IIC_Delay(1);
    IIC_SDA_High;
}
 
void Check_ACK(void)
{
    IIC_SDA_Set_Input;
    IIC_Delay(1);
    IIC_SCL_High;
    qnd_i2c_timeout = (U8) Read_SDA;
    IIC_Delay(1);
    IIC_SCL_Low;
    IIC_SDA_Set_Output;
}

void Write_Bit(U8 i)
{
    IIC_Delay(1);
    if(i == 0)
		IIC_SDA_Low;
	else
		IIC_SDA_High;
    IIC_Delay(1);
    IIC_SCL_High;
    IIC_Delay(2);
    IIC_SCL_Low;
}

void Write_Byte(U8 Data)
{
    U8 i;
    for( i = 0;i < 8;i++)
    {
        Write_Bit(Data>>7);
        Data<<=1;
    }
}

U8 Read_Byte(void)
{
    U8 Data1=0x00;
    U8 j;
    IIC_SDA_Set_Input;
    for( j = 0;j < 8;j++)
    {
        IIC_Delay(1);
        IIC_SCL_High;
        Data1 = (Data1<<1) | ((U8) Read_SDA & 0x01);
        IIC_Delay(1);
            IIC_SCL_Low;
        }
    IIC_SDA_Set_Output;
    return Data1;
}
 
/************************************************************************************************************
** Name: I2C_Write_1byte                         
** Funcation:write a data to a desired            
**           register through i2c bus 
** Description: Slave---------device address
**              Regis_Addr----register address
*************************************************************************************************************/
U8 I2C_Write_1byte(U8 Slave,U8 Regis_Addr,U8 Data)
{
    U8 temp;
    temp=Slave;

    Start();
    Write_Byte(temp);
    Check_ACK();
    if (!qnd_i2c_timeout) 
    {
        Write_Byte(Regis_Addr);
        Check_ACK();
        if (!qnd_i2c_timeout) 
        {
            Write_Byte(Data);
            Check_ACK();
        }
    }
    Stop();
    return !qnd_i2c_timeout;
}

/***********************************************************************************************************
** Name: I2C_Read_1byte                          
** Function: Read a data from a desired register 
**           through i2c bus 
** Description: Slave---------device address
**              Regis_Addr----register address
************************************************************************************************************/
U8 I2C_Read_1byte(U8 Slave,U8 Regis_Addr)
{ 
    U8 Data=0x00;
    U8 temp;
    temp =Slave | 0x01;

    Start();
    Write_Byte(Slave);
    Check_ACK();
    if (!qnd_i2c_timeout) 
    {
        Write_Byte(Regis_Addr);
        Check_ACK();
        if(!qnd_i2c_timeout) 
        {
            //stop();
            Start();
            Write_Byte(temp);
            Check_ACK();
            if (!qnd_i2c_timeout) 
            {
                Data = Read_Byte();
                Send_ACK(1);
            }
        }
    }
    Stop();
    return Data;
}


void QN_ChipInitialization()
{
	I2C_Write_1byte(I2C_DEV0_ADDRESS,0x00,0x81);// reset all registers to the default value
    QND_Delay(20);
    I2C_Write_1byte(I2C_DEV0_ADDRESS, 0x03,0x10);
	I2C_Write_1byte(I2C_DEV0_ADDRESS, 0x04,0x41);//fm-vol
	I2C_Write_1byte(I2C_DEV0_ADDRESS, 0x00,0x41);
	I2C_Write_1byte(I2C_DEV0_ADDRESS, 0x00,0x01);
    QND_Delay(20);
	I2C_Write_1byte(I2C_DEV0_ADDRESS, 0x18,0xe4);
	I2C_Write_1byte(I2C_DEV0_ADDRESS, 0x1b,0xf0);
	I2C_Write_1byte(I2C_DEV0_ADDRESS, 0x11,0x50);//
	I2C_Write_1byte(I2C_DEV0_ADDRESS, 0x01,0x7e);
	I2C_Write_1byte(I2C_DEV0_ADDRESS, 0x02,0xb9);
	I2C_Write_1byte(I2C_DEV0_ADDRESS, 0x00,0x22);
}

void QND_Delay(U16 ms) 
{
	U16 i,k;
    for(i=0; i<3000;i++) {    
		    for(k=0; k<ms; k++) {
		}
    }
}
/**********************************************************************
int QND_Init()
**********************************************************************
Description: Initialize device to make it ready to have all functionality ready for use.

Parameters:
None
Return Value:
1: Device is ready to use.
0: Device is not ready to serve function.
**********************************************************************/
BYTE QND_Init() 
{    
/*  BYTE timeout;
    BYTE openTmp,chkflag;
	timeout = 100;
	chkflag = 1;
	while(timeout--) 
	{
		QND_Delay(100);
	    openTmp = I2C_Read_1byte(I2C_DEV0_ADDRESS, CID2);
		openTmp &=0xf0;  //for A1 and B1
	    if(openTmp == CHIPID_QN8027)
		{
            timeout = 0;
            chkflag = 0;
	    }
	}
    if(chkflag) 
	{
    	return 0;
    }
   
*/
    QN_ChipInitialization();
    I2C_Write_1byte(I2C_DEV0_ADDRESS,  00,  0x01); //resume original status of chip /* 2008 06 13 */
//    qnd_Band = BAND_FM;
    return 1;
}


/**********************************************************************
void QND_TuneToCH(UINT16 ch)
**********************************************************************
Description:    Tune to the specified channel. Before use this function
make sure the QND_SetSysMode() has been called. If system is working 
on FM&TX mode,it will turn FM to ch, and start Transmit; If system is
working on FM&RX mode,it will turn FM to ch, and start FM play.
Parameters:
ch
Set the frequency (10kHz) to be tuned,
eg: 101.30MHz will be set to 10130.
Return Value:
    None
**********************************************************************/
void QND_TuneToCH(U16 ch) 
{
    QNF_SetCh(ch);
//  QND_Delay(3);
}


/**********************************************************************
UINT8 QNF_SetCh(UINT16 freq)
**********************************************************************
Description: set specified frequency 

Parameters:
        freq:  specified frequency that is set
Return Value:
         1: set success
**********************************************************************/
U8 QNF_SetCh(U16 freq) 
{
    // calculate ch para
    U8 tStep;
    U8 tCh;
    U16 f; 
        freq=freq*10;
        f = FREQ2CHREG(freq); 
        // set to reg: CH
        tCh = (U8) f;
        I2C_Write_1byte(I2C_DEV0_ADDRESS,  CH, tCh);		
        // set to reg: CH_STEP
        tStep = I2C_Read_1byte(I2C_DEV0_ADDRESS,  CH_STEP);
        tStep &= ~CH_CH;
        tStep |= ((U8) (f >> 8) & CH_CH);
		tStep |= 0x20;	//设置为发射模式，寄存器00H的bit5设置为1
        I2C_Write_1byte(I2C_DEV0_ADDRESS,  CH_STEP, tStep);

    return 1;
}
//========================================BK1085==============================
//========================================BK1085==============================
#if 0  //32768晶振
code U32 HW_Reg[]=
{
0x02e286bd,//MSB// reg1 high byte(0x02); reg1 low byte(0xe2);reg0 high byte(0x86); reg0 low byte(0xbd);//LSB//  frequency*2^21 /3.8=reg1,reg0
0x000782e3,//MSB// reg3 high byte(0x00); reg3 low byte(0x07);reg2 high byte(0x82); reg2 low byte(0xe3);//LSB//
0x10c0800c,//MSB// reg4/reg5 byte1(0x10); reg4/reg5 byte0(0xc0); reg4/reg5 byte3(0x80); reg4/reg5 byte2(0x0e);//LSB//
0x00402398,//MSB// reg6/reg7 byte1(0x23); reg6/reg7 byte0(0x98); reg6/reg7 byte3(0x00); reg6/reg7 byte2(0x40);//LSB//
0X00402388,
};
#endif 
//12M晶振
code U32 HW_Reg[]=
{
0x02e286bd,//MSB// reg1 high byte(0x02); reg1 low byte(0xe2);reg0 high byte(0x86); reg0 low byte(0xbd);//LSB//  frequency*2^21 /3.8=reg1,reg0
0x000782e3,//MSB// reg3 high byte(0x00); reg3 low byte(0x07);reg2 high byte(0x82); reg2 low byte(0xe3);//LSB//
0x00c0f00c,//0x00c0800c,//0x10c0800c独立晶体//MSB// reg4/reg5 byte1(0x10); reg4/reg5 byte0(0xc0); reg4/reg5 byte3(0x80); reg4/reg5 byte2(0x0e);//LSB//
0x5b8e2198,//MSB// reg6/reg7 byte1(0x23); reg6/reg7 byte0(0x98); reg6/reg7 byte3(0x00); reg6/reg7 byte2(0x40);//LSB//
0x5b8e2188,
0x80000008,
};

/**************************************************
Function: Delay1us
Description:
	Delay 1 us
Parameter:
	None
Return:
	None
**************************************************/
void Delay1us()//please revise this function according to your system
{
	U16 i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++);
}


void Delay1us_50ms()//please revise this function according to your system
{
	U16 i,j;
	for(i=0;i<4000;i++)
		for(j=0;j<200;j++);
}

/*************************************************
  Function:       BEKEN_I2C_init
  Description:    BEKEN I2C initialize
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_init(void)
{
    SDADIROUT();                  //SDA output
    SCLDIROUT();                  //SCL output
    BK_CLK_HIGH();
    BK_DATA_HIGH();
}

/*************************************************
  Function:       BEKEN_I2C_Start
  Description:    BEKEN I2C transaction start
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_Start(void)
{
    BEKEN_I2C_init();
    Delay1us();
    BK_DATA_LOW();
    Delay1us();
    BK_CLK_LOW();
    Delay1us();
    BK_DATA_HIGH();
}

/*************************************************
  Function:       BEKEN_I2C_Stop
  Description:    BEKEN I2C transaction end
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_Stop(void)
{
    SDADIROUT();
    BK_DATA_LOW();
    Delay1us();
    BK_CLK_HIGH();
    Delay1us();
    BK_DATA_HIGH();
    Delay1us();
}

/*************************************************
  Function:       BEKEN_I2C_ack
  Description:    send ACK,accomplish a operation
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_ack(void)
{
    SDADIROUT();

    BK_CLK_LOW();
    BK_DATA_LOW();
    Delay1us();

    BK_CLK_HIGH();
    Delay1us();
    BK_CLK_LOW();
}      

/*************************************************
  Function:       BEKEN_I2C_nack
  Description:    send NACK,accomplish a operation
  Input:          None
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_nack(void)
{
    SDADIROUT();

    BK_CLK_LOW();
    BK_DATA_HIGH();
    Delay1us();

    BK_CLK_HIGH();
    Delay1us();
    BK_CLK_LOW();
}

/*************************************************
  Function:       BEKEN_I2C_ReceiveACK
  Description:    wait ACK,accomplish a operation
  Input:          None
                          
  Output:         None
  Return:         ack flag
*************************************************/
U8 BEKEN_I2C_ReceiveACK(void)
{
    U32 ackflag;
    SDADIRIN();
    Delay1us();
    BK_CLK_HIGH();
    Delay1us();
    ackflag = (U8)BK_DATA_READ();
    BK_CLK_LOW();
    Delay1us();
    return ackflag;
}

/*************************************************
  Function:       BEKEN_I2C_sendbyte
  Description:    write a byte
  Input:          a byte written
                          
  Output:         None
  Return:         None
*************************************************/
void BEKEN_I2C_sendbyte(U8 I2CSendData)
{
   
    U8  i;
    SDADIROUT();
    Delay1us();
    for(i = 0;i < 8;i++)
    {
        if(I2CSendData & 0x80)
        {
            BK_DATA_HIGH();     //if high bit is 1,SDA= 1
        }
        else
            BK_DATA_LOW();                               //else SDA=0

       	Delay1us();
        BK_CLK_HIGH();
       	Delay1us();
        I2CSendData <<= 1;                          //shift left 1 bit
        BK_CLK_LOW();
    }                       
}

/*************************************************
  Function:       BEKEN_I2C_readbyte
  Description:    read a byte
  Input:          None
                          
  Output:         None
  Return:         a byte read
*************************************************/
U8 BEKEN_I2C_readbyte(void)
{
    U8 i;
    U8 ucRDData = 0;                     //return value
    
    SDADIRIN();
    Delay1us();;
    for(i = 0;i < 8;i++)
    {
        BK_CLK_HIGH();
        ucRDData <<= 1;
        Delay1us();
        if(BK_DATA_READ())
            ucRDData|=0x01;
        BK_CLK_LOW();
        Delay1us();
    }
    return(ucRDData);
}

/**************************************************
Function: Wire2_Spi0_Write_32Bit
Description:
	write a 32 bit register.

Parameter:
	addr:  register address*2
	value: register value
Return:
	None
**************************************************/
void Wire2_Spi0_Write_32Bit(U8 addr,U32 value)
{
  

    BEKEN_I2C_Start();

    BEKEN_I2C_sendbyte(DEV_ADDR);
    BEKEN_I2C_ReceiveACK();

    addr=addr<<1;
    BEKEN_I2C_sendbyte(addr);
    BEKEN_I2C_ReceiveACK();
	 
    BEKEN_I2C_sendbyte((value>>8)&0xff);
    BEKEN_I2C_ReceiveACK();

    BEKEN_I2C_sendbyte((value)&0xff);
    BEKEN_I2C_ReceiveACK();

    BEKEN_I2C_sendbyte((value>>24)&0xff);
    BEKEN_I2C_ReceiveACK();

    BEKEN_I2C_sendbyte((value>>16)&0xff);
    BEKEN_I2C_ReceiveACK();

    BEKEN_I2C_Stop();
}



/**************************************************
Function: Chip_initialization
Description:	
	BK1085 chip initialize
Parameter:
	none
Return:
	None
**************************************************/
void Chip_initialization()
{
	Wire2_Spi0_Write_32Bit(2*2,HW_Reg[2]);//reg2
	Wire2_Spi0_Write_32Bit(1*2,HW_Reg[1]);//reg1
	Wire2_Spi0_Write_32Bit(3*2,HW_Reg[3]);//reg3
	USER_DelayDTms(300);//300ms
	Wire2_Spi0_Write_32Bit(3*2,HW_Reg[4]);//reg3
	USER_DelayDTms(1);
	Wire2_Spi0_Write_32Bit(3*2,HW_Reg[3]);//reg3
	USER_DelayDTms(1);
	Wire2_Spi0_Write_32Bit(5*2,HW_Reg[5]);//reg10//解决蹼趾的声音
}

/**************************************************
Function: SetFrequency
Description:
	ser a frequency
Parameter:
	freq:real frequency*10;such as 1077,977,
Return:
	None
**************************************************/
void SetFrequency(U16 freq_2)
{
//(frequency*10) *2^21 /38
	
	U32 freq; 
	
	freq=(U32)freq_2; 
	  
	freq=freq<<21; 
	freq=freq/38; 
	Wire2_Spi0_Write_32Bit(0,freq); 
}
/**************************************************
Function: AudioGain
Description:
	change voice volume
Parameter:
	volume:0-3
Return:
	None
**************************************************/
void AudioGain(U8 volume)
{	
	U32 value;
	value=HW_Reg[2]&0xffffffcf; //clear bit5:bit4
//reg4<21:20>
	volume=volume<<4;
	value|=volume;
	
	Wire2_Spi0_Write_32Bit(2*2,value);

}

/**************************************************
Function: Chip_powerdown
Description:
	Power down chip,
	Please call AudioGain to set volume again when the next power up.
	
Parameter:
	none
Return:
	None
**************************************************/
void Chip_powerdown(void)
{	
	U32 value;
	value=0x10F3830E; 
	
	Wire2_Spi0_Write_32Bit(2*2,value);

}
