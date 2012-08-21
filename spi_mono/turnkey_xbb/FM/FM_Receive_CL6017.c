

#define CL6017_RD_CTRL  0x21// 0x0021 
#define CL6017_WR_CTRL  0x20// 0x0020 

U8 code CL6017ControlData_table[24] =
{
#if 1 //gp9 初始化
0xe4,0x81,0x3e,0xf8,
0x1f,0x40,0x38,0x5a,
0xe8,0xac,0xb0,0xf1,  
0x8b,0xaa,0xc6,0x04,
0x6d,0x25,0xff,0xfd,
0x12,0x0f,0x45,0x1d,
#else//原来程序的初始化
0xe4,0x83,0x3e,0x48,   
0xf8,0x1f,0xb0,0x5a,
0xe8,0xac,0xb0,0xf0,
0x8b,0xaa,0xc6,0x04,
0x6d,0x25,0xff,0xfD,
0x12,0x0f,0x45,0x1d,
#endif
};
U8 xdata CL6017ControlData[24];
void CL6017_FMVolumeSet(U16 vol);

/////////////////////////////
//read id not be used
/*
U8 CL6017_Read_ID(void)
{
	U8 temp_Read[6]={0};
	U8 i;

	for(i=0;i<6;i++)
	{
		temp_Read[i]=0xff;
	}

	RecStr(CL6017_RD_CTRL,temp_Read,6);

	for(i=0;i<6;i++)
	{
		dbprintf("%bx ",temp_Read[i]);
	}
	dbprintf("\n6017 read id end\n");
	return temp_Read[4];
}
*/
void CL6017_MUTE(void)
{
	CL6017ControlData[0] |= 0x20;  // mute on
	SendStr( CL6017_WR_CTRL,CL6017ControlData,  1); 	
}

void CL6017_UnMUTE(void)
{
	CL6017ControlData[0] &= 0xdf;  // mute off
	SendStr( CL6017_WR_CTRL,CL6017ControlData,  1); 
}

void CL6017_FMVolumeSet(U16 vol)
{	
	dbprintf("fmset vol=%x\n",vol);

	vol = vol & 0x0f;
	vol = vol << 2;
	CL6017ControlData[2] &= ~0x3c;
	CL6017ControlData[2] |= vol;
//	I2C_WriteCL6017Reg(CL6017ControlData,3);
	SendStr( CL6017_WR_CTRL,CL6017ControlData,  3);
}
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
void CL6017_FMInit(void)
{
	
	U8 i;
	for(i=0;i<24;i++)
	{
		CL6017ControlData[i]=CL6017ControlData_table[i];
	}	
	
	dbprintf("cl6017 init\n");

	CL6017ControlData[2] &= 0xFC; 
	CL6017ControlData[2] |= 0x01;
	CL6017ControlData[3] = 0xe4;


	CL6017ControlData[0] &= 0x7f; //power up
	SendStr( CL6017_WR_CTRL,CL6017ControlData, 24 ); 

	IIC_Dealy(107);  // wait 800ms for crystal oscillate stable

	CL6017ControlData[0] |= 0x02; // TUNE=1, begin tune operation
	SendStr( CL6017_WR_CTRL,CL6017ControlData, 24 ); 
	IIC_Dealy(14);  // delay 100ms*/
}

void CL6017_FMTune()
{

	U16 ch = 0;

//	dbprintf("set freq=%x\n",TunerFrequency);

	ch = (gw_FM_frequency*10 - 7000)/5; 

	CL6017ControlData[2] &= 0xfc;
	CL6017ControlData[2] |= (ch & 0x0300)>>8;
	CL6017ControlData[3] =	ch & 0xff;
	
	// tune function
	CL6017ControlData[0] &= 0xdf;// mute off

	CL6017ControlData[0] &= 0xfd;// tune=0
	SendStr( CL6017_WR_CTRL,CL6017ControlData, 4 ); 
	
	CL6017ControlData[0] |= 0x02;//tune=1, begin tune operaton
	SendStr( CL6017_WR_CTRL,CL6017ControlData, 2); 	
	
	IIC_Dealy(7); // wait for 50ms

	LCM_ShowFrequency_L();
}    

/**************************************************************************************
*   函数名称：CL6017_FMSeekFromCurrentCh
*   功    能：
*   输入参数：BOOL seekUp: 0-seek up, 1-seek down
*   输入参数：U16 *TunedFrequency: the radio frequency stay in when seeking process complete, the frequency unit is 10khz	 

*   输出参数：BOOL sfflag: 0-Seek successful  1-Seek failure
*   说    明： seek from current channel automaticly,if want to seek from certain channel,  use CL6017_FMTune to certain channel ahead
**************************************************************************************/
xdata U8 gc_seekup;
//envelop===>信噪比,越小越好
//rssi===>信号强度,越大越好
void CL6017_FMSeekFromCurrentCh(U8 seekUp)
{
	U8 rssi,envelop,fd_num;
	U8 Temp[6]={0x00};
	U8 stc=0;
	U8 stccnt=0x20;
	U8 seek_auto;

	CL6017_MUTE();
	/*
	tc_TmpData = LineVolumeTable[0];    //Jimi 091030
	GLOBAL_REG[0xC4] = tc_TmpData;  //(Jimi 091030)Headphone gain Lch
	GLOBAL_REG[0xC5] = tc_TmpData;  //(Jimi 091030)Headphone gain Rch
	
	CL6017Dealy(10);//10*7.5=75ms
	*/
	gc_seekup=seekUp;
	seek_auto=1;
	do
	{
		if(gc_seekup)
		{
			if(gw_FM_frequency>=1080)
			{
				gw_FM_frequency=875;
				seek_auto=0;
			}
			else
			{
				gw_FM_frequency+=1;
			}
		}
		else
		{
			if(gw_FM_frequency<=875)
			{
				gw_FM_frequency=1080;
				seek_auto=0;
			}
			else
			{
				gw_FM_frequency-=1;
			}
		}
		//-------------------------------------------------------------------
//		CL6017_FMTune(freq,1); //write5767D(cha);
		{//mute 搜台	
			U16 ch = 0;
			ch = (gw_FM_frequency*10 - 7000)/5; 
			CL6017ControlData[2] &= 0xfc;
			CL6017ControlData[2] |= (ch & 0x0300)>>8;
			CL6017ControlData[3] =	ch & 0xff;
			// tune function
			CL6017ControlData[0] |= 0x20;  // mute on
			CL6017ControlData[0] &= 0xfd;// tune=0
			SendStr( CL6017_WR_CTRL,CL6017ControlData, 4 ); 
			
			CL6017ControlData[0] |= 0x02;//tune=1, begin tune operaton
			SendStr( CL6017_WR_CTRL,CL6017ControlData, 2); 	
			
			IIC_Dealy(7); // wait for 50ms
		
			LCM_ShowFrequency_L();
		}
		////搜台时按prev或者next时stop搜台状态
		Key_Detect();
		if(gc_KeyEvent==0x03 || gc_KeyEvent==0x04)
		{
			seek_auto=0;
		}
		//----------------------------------------------------
		stccnt=5;
		while(stccnt!=0)
		{
			stccnt--;
			RecStr(0x21,Temp,2);
			if(Temp[0]&0x04)    //check STC
			{
				break;
			}
			IIC_Dealy(1);//2*7.5=15ms
		}
		//---------------------------------------------------
		if(stccnt)
		{
			RecStr(0x21,Temp,6);      //RecStr(0x21,readdat,6);
			rssi=Temp[1];             //rssi=readdat[1]
			envelop=Temp[2]>>2;       //envelop=readdat[2]>>2;
			fd_num=Temp[5];
			if(fd_num>127)
			{
				fd_num=256-fd_num;
			}
			if((envelop<38)&&(rssi>200)&&(fd_num<13))     //38   //176 //14
			{
				seek_auto=0;
			}
		}
		// gw_FM_frequency=gw_CL6017freq;
	}while(seek_auto);
	
/*	CL6017ControlData[0] &= 0xdf;
	CL6017ControlData[2] &= 0xc3;          // clear VOLUME[3:0] bits
	CL6017ControlData[2] |= (fm_vol<<2);   // set VOLUME[3:0] to the desired volume value
	SendStr(0x20,CL6017ControlData,4);
	CL6017Dealy(10);*/

	CL6017_FMTune();         //write5767D(cha);
	
	/*
	tc_TmpData = LineVolumeTable[(gs_DSP_GLOBAL_RAM.sw_Volume/2)];  //Jimi 091030
	GLOBAL_REG[0xC4] = tc_TmpData;  //(Jimi 091030)Headphone gain Lch
	GLOBAL_REG[0xC5] = tc_TmpData;  //(Jimi 091030)Headphone gain Rch
	//gw_FM_frequency=gw_CL6017freq;*/

}
//search all freq
//xdata U16 gw_search_freq;
void CL6017AutoSearchAllFreq(U16 x_FreqTemp)
{
	U8 rssi,envelop,fd_num;
	U8 Temp[6]={0x00};
	U8 stc=0;
	U8 cnt=0;
	U8 stccnt=0x20;
	U8 seek_auto;
	
	gw_FM_frequency=x_FreqTemp;

	gc_FM_CHAmount=0;
	CL6017ControlData[0] |= 0x20;
	CL6017ControlData[2] &= 0xc3;            // clear VOLUME[3:0] bits
	CL6017ControlData[2] |= (0<<2);          // set VOLUME[3:0] to the desired volume value
	SendStr(CL6017_WR_CTRL,CL6017ControlData,4);
	//-----------------------------------------------------------------
/*	tc_TmpData = LineVolumeTable[0]; //Jimi 091030
	GLOBAL_REG[0xC4] = tc_TmpData;   //(Jimi 091030)Headphone gain Lch
	GLOBAL_REG[0xC5] = tc_TmpData;   //(Jimi 091030)Headphone gain Rch
	*/
	//-----------------------------------------------------------------
	IIC_Dealy(10);//10*7.5=75ms
	seek_auto =1;
	//------------------------------------------------------------
	do
	{
		if(seek_auto)
		{
			if(gw_FM_frequency>=1079)
			{
				gw_FM_frequency=1080;
				seek_auto=0;
			}
			else
			{
				gw_FM_frequency+=1;
			}
		}
//		CL6017_FMTune(gw_search_freq,1);    //write5767D(cha);
		{//mute 搜台	
			U16 ch = 0;
			ch = (gw_FM_frequency*10 - 7000)/5; 
			CL6017ControlData[2] &= 0xfc;
			CL6017ControlData[2] |= (ch & 0x0300)>>8;
			CL6017ControlData[3] =	ch & 0xff;
			// tune function
			CL6017ControlData[0] |= 0x20;  // mute on
			CL6017ControlData[0] &= 0xfd;// tune=0
			SendStr( CL6017_WR_CTRL,CL6017ControlData, 4 ); 
			
			CL6017ControlData[0] |= 0x02;//tune=1, begin tune operaton
			SendStr( CL6017_WR_CTRL,CL6017ControlData, 2); 	
			
			IIC_Dealy(7); // wait for 50ms
		
			LCM_ShowFrequency_L();
		}
		//------------------------------------------------------------

		stccnt=5;
		while(stccnt!=0)
		{
			stccnt--;
			RecStr(0x21,Temp,2);
			if(Temp[0]&0x04)    //check STC
			{
				break;
			}
			IIC_Dealy(1);    //1*7.5=7.5ms
		}
		
		if(stccnt)
		{
			RecStr(0x21,Temp,6);
			rssi=Temp[1];             //rssi=readdat[1];
			envelop=Temp[2]>>2;       //envelop=readdat[2]>>2;
			fd_num=Temp[5];
			if(fd_num>127)
			{
				fd_num=256-fd_num;
			}
			if((envelop<38)&&(rssi>200)&&(fd_num<13))     //38//176//14
			{
			/*	CL6017ControlData[0] &= 0xdf;
				CL6017ControlData[2] &= 0xc3;             // clear VOLUME[3:0] bits
				CL6017ControlData[2] |= (fm_vol<<2);     // set VOLUME[3:0] to the desired volume value
			
				SendStr(0x20,CL6017ControlData,4);	*/
				IIC_Dealy(10);
				CL6017_FMTune();
				gw_FM_CH[cnt]=gw_FM_frequency;
				gc_FM_CHAmount=cnt;
				CL6017ControlData[0] |= 0x20;
				CL6017ControlData[2] &= 0xc3;        // clear VOLUME[3:0] bits
				CL6017ControlData[2] |= (0<<2); // set VOLUME[3:0] to the desired volume value
				SendStr(CL6017_WR_CTRL,CL6017ControlData,4);
				IIC_Dealy(10);
				cnt++;
			//	LCD_Disp_FM_CHNUM(cnt);
			//	dbprintf("cnt=%bx::%x\n\n\n",cnt,gw_search_freq);
			}
		}
		if((gw_FM_frequency>1079)||(cnt>=50))
		{
			seek_auto=0;
		}
		
	}while(seek_auto);
	//--------------------------------------------------------------------
/*	CL6017ControlData[0] &= 0xdf;
	CL6017ControlData[2] &= 0xc3;        // clear VOLUME[3:0] bits
	CL6017ControlData[2] |= (fm_vol<<2); // set VOLUME[3:0] to the desired volume value
	
	SendStr(0x20,CL6017ControlData,4);*/
	IIC_Dealy(10);
//	fm_mute=0;
	dbprintf("CH_amount=%bx\n",gc_FM_CHAmount);
	if(gc_FM_CHAmount==0)
	{
		gw_FM_frequency= 875;
	}
	else
	{
		gw_FM_frequency = gw_FM_CH[0]; 
	}
	gc_FM_CH_COUNT=0;
	CL6017_FMTune();        //write5767D(cha);

	CL6017_FMVolumeSet(0x0f);

/*	tc_TmpData = LineVolumeTable[(gs_DSP_GLOBAL_RAM.sw_Volume/2)];   //Jimi 091030
	GLOBAL_REG[0xC4] = tc_TmpData;   //(Jimi 091030)Headphone gain Lch
	GLOBAL_REG[0xC5] = tc_TmpData;   //(Jimi 091030)Headphone gain Rch
	*/
//	LCD_Disp_FM_CHNUM(1);
	dbprintf("cnt=%bx::%x\n",gc_FM_CH_COUNT,gw_FM_frequency);
//	return cnt;
}

void CL6017_FMPowerdown(void)
{
	CL6017ControlData[0] |= 0x20;  // mute on
	SendStr( CL6017_WR_CTRL,CL6017ControlData,  2);

	CL6017ControlData[0] |= 0x80; // set DISABLE=1 to power down
	SendStr( CL6017_WR_CTRL,CL6017ControlData,  2);
}

//---------------------------------------------------------------------------------
void FM_OPEN_CL6017(void)
{
	CL6017ControlData[0] &= 0x7F;      // set DISABLE=0 to power on
	SendStr(CL6017_WR_CTRL,CL6017ControlData,2);//I2C_WriteCL6017Reg(CL6017ControlData,2);
	
	CL6017ControlData[0] &= 0xdf;  //mute off
	SendStr(CL6017_WR_CTRL,CL6017ControlData,2);//I2C_WriteCL6017Reg(CL6017ControlData,2);
}
