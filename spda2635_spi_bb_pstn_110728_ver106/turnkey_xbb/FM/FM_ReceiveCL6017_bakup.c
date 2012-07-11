#include "..\Header\SPDA2K.h"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"
#include "FM_receiveH.h"
U16 DECfreq(U16 fre);
U16 INCfreq(U16 fre);

void CL6017_FMTune(U16 TunerFrequency)
{
   data	U16 ch;

   // frequency transfer to channel number,  channel=(frequencyMhz-70)/0.05, e.g. 87.5Mhz->350, 108Mhz->760
   ch = (TunerFrequency*10 - 7000)/5;
   CL6017ControlData[2] &= 0xfc;
   CL6017ControlData[2] |= (ch & 0x0300)>>8;
   CL6017ControlData[3] =  ch & 0xff;
   if(gb_FM_Mute)
   {
     CL6017ControlData[0] |= 0x20;  // mute on
   }else{
     CL6017ControlData[0] &= 0xdf;// mute off
   }
   CL6017ControlData[0] &= 0xfd;// tune=0
   SendStr(0x20,CL6017ControlData,4);//I2C_WriteCL6017Reg(CL6017ControlData,4);
   IIC_Dealy(2);//30*7.5=210ms
   CL6017ControlData[0] |= 0x02;//tune=1, begin tune operaton
   SendStr(0x20,CL6017ControlData,2);//I2C_WriteCL6017Reg(CL6017ControlData,2);
   IIC_Dealy(10);//15*7.5=75ms

   gc_LogData_Timer=32;
   LCM_ShowFrequency_L(TunerFrequency);
}    

U16 INCfreq(U16 fre)
{
   if((fre+stepfreq) > highfreq)
     fre=lowfreq;
   else
     fre+=stepfreq;

   CL6017_FMTune(fre);
   gw_FM_frequency=fre;
   return fre;
}


U16 DECfreq(U16 fre)
{
   if((fre-stepfreq)<lowfreq)
     fre=highfreq;
   else
     fre-=stepfreq;

   CL6017_FMTune(fre);
   gw_FM_frequency=fre;
   return fre;
} 


void FM_AutoSearch(void)
{
	data	U8	stc=0;
	data	U8	stccnt;
	xdata	U8	rssi;
	xdata	U8	envelop;
	xdata	U8	fd_num;
	xdata	U8	tc_TmpData;
	xdata	U8	Temp[6]={0x00};
	xdata	U16	tw_Freq;

	tw_Freq=gw_FM_frequency;
    CL6017ControlData[0]|=0x20;
    CL6017ControlData[2]&=0xc3;        // clear VOLUME[3:0] bits
    CL6017ControlData[2]|=(0<<2);      // set VOLUME[3:0] to the desired volume value
    SendStr(0x20,CL6017ControlData,4);
	XBYTE[0xB0C4]=31;  //(Jimi 091030)Headphone gain Lch
	XBYTE[0xB0C5]=31;  //(Jimi 091030)Headphone gain Rch

    IIC_Dealy(10);//10*7.5=75ms
	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent==6)	// M
		{
			break;
		}

		if(gb_ADJ==0)	// 0=INC  1=DEC
		{
			if(gw_FM_frequency>=highfreq)
			{
				gw_FM_frequency=lowfreq;
			}
			else
			{
				gw_FM_frequency+=stepfreq;
			}
		}
		else
		{
			if(gw_FM_frequency<=lowfreq)
			{
				gw_FM_frequency=highfreq;
			}
			else
			{
				gw_FM_frequency-=stepfreq;
			}
		}
		
		if(gw_FM_frequency==tw_Freq)
		{
			break;
		}

		gb_FM_Mute=1;
		CL6017_FMTune(gw_FM_frequency);
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
				break;
			}
		}
	}

	CL6017ControlData[0]&=0xdf;
	CL6017ControlData[2]&=0xc3;				// clear VOLUME[3:0] bits
	CL6017ControlData[2]|=(gc_FM_Vol<<2);	// set VOLUME[3:0] to the desired volume value
	SendStr(0x20,CL6017ControlData,4);
	IIC_Dealy(10);
	gb_FM_Mute=0;
	CL6017_FMTune(gw_FM_frequency);
	
	tc_TmpData=31-(gs_DSP_GLOBAL_RAM.sc_Volume/2);
	XBYTE[0xB0C4]=tc_TmpData;  //(Jimi 091030)Headphone gain Lch
	XBYTE[0xB0C5]=tc_TmpData;  //(Jimi 091030)Headphone gain Rch
}

//===================================================================
//
//===================================================================
void CL6017FreqSaveChannel(U8 NextorPrev)
{
    if(gc_FM_CHAmount==0)
    {
        return;
    }

    if(NextorPrev)
    {
        if(gc_FM_CH_COUNT<=gc_FM_CHAmount)
            gc_FM_CH_COUNT++;
        else
            gc_FM_CH_COUNT=0;
    }
    else
    {
        if(gc_FM_CH_COUNT>0)
            gc_FM_CH_COUNT--;
        else
            gc_FM_CH_COUNT=gc_FM_CHAmount;
    }
    //---------------------------------------------------------
    if(gc_FM_CH_COUNT==0)
    {
        gc_FM_CH_COUNT=1;   
    }
    if((gw_FM_CH[gc_FM_CH_COUNT-1]>=lowfreq)&&(gw_FM_CH[gc_FM_CH_COUNT-1]<=highfreq))
    {
        gw_FM_frequency = gw_FM_CH[gc_FM_CH_COUNT-1];
    }
    else
    {
        if(gc_FM_CH_COUNT==1)
        {
            gw_FM_frequency =875;
            gw_FM_CH[0]=gw_FM_frequency;    
        }
        else
        {
            gc_FM_CH_COUNT=1;
            gw_FM_frequency = gw_FM_CH[gc_FM_CH_COUNT-1];
        }
    }
    CL6017_FMTune(gw_FM_frequency);
    LCM_Show_FM_CHNUM(gc_FM_CH_COUNT);
    gb_FreqSaveChannel=1; 
}


void FMIRNumberKey(U16 x_tmpfreq)
{
   gw_FM_frequency = x_tmpfreq;
   CL6017_FMTune(gw_FM_frequency);
}
//===========================================================
//
//===========================================================
/////////////////////////////////////
void FM_Initial_CL6017(void)
{
	CL6017ControlData[0]=0xE4;  ////reg3  e4
	CL6017ControlData[1]=0x83;  //81-->83(auto HLSI)
	CL6017ControlData[2]=0x3e;   //reg4
	CL6017ControlData[3]=0xf8;
	CL6017ControlData[4]=0x1f;  // reg5   1f-1d
	CL6017ControlData[5]=0x48;   //0x40
	CL6017ControlData[6]=0xb0;   //0x38 reg6
	CL6017ControlData[7]=0x5a;
	CL6017ControlData[8]=0xe8;  //reg7
	CL6017ControlData[9]=0xac;
	CL6017ControlData[10]=0xb0;
	CL6017ControlData[11]=0xf0;  //0xf2
	CL6017ControlData[12]=0x8b;
	CL6017ControlData[13]=0xaa;
	CL6017ControlData[14]=0xc6;
	CL6017ControlData[15]=0x04;
	CL6017ControlData[16]=0x6d;
	CL6017ControlData[17]=0x25;
	CL6017ControlData[18]=0xff;
	CL6017ControlData[19]=0xfD;
	CL6017ControlData[20]=0x12;
	CL6017ControlData[21]=0x0f;
	CL6017ControlData[22]=0x45;
	CL6017ControlData[23]=0x1d;
	
	//==============================================================
	CL6017ControlData[2]&=0xF3;        // clear VOLUME[3:0] bits
	CL6017ControlData[2]|=(0x0F<<2); // set VOLUME[3:0] to the desired volume value
	//==============================================================
	// power up and initial operation
	// if initially set to 94.2mhz radio, channel = (94.2-70)/0.05=484 374
	CL6017ControlData[2]&=0xFC;
	CL6017ControlData[2]|=0x01;
	CL6017ControlData[3]=0xe4;
	
	CL6017ControlData[0] &= 0x7f; //power up
	SendStr(0x20,CL6017ControlData,24);
	
	IIC_Dealy(110);//110*7.5=825ms
	
	CL6017ControlData[0] |= 0x02; // TUNE=1, begin tune operation
	SendStr(0x20,CL6017ControlData,1);
	IIC_Dealy(30);//10*7.5=75ms
	
	CL6017ControlData[0] &= 0xdf;  // mute off
	SendStr(0x20,CL6017ControlData,1);
	IIC_Dealy(30);//10*7.5=75ms
	CL6017_FMTune(gw_FM_frequency);
}

void FM_Open(void)
{
   CL6017ControlData[0]&=~0x80;	// set DISABLE=0 to power on
   SendStr(0x20,CL6017ControlData,2);
   CL6017ControlData[0]&=~0x20;	// mute off
   SendStr(0x20,CL6017ControlData,2);
}

void FM_Mute(void)
{
    CL6017ControlData[0]|=0x20;	// mute on
    SendStr(0x20,CL6017ControlData,2);
    CL6017ControlData[0]|=0x80;	// set DISABLE=1 to power down
    SendStr(0x20,CL6017ControlData,2);
}

U8 FM_SearchAllChannel(void)
{
	data	U8	cnt=0;
	data	U8	stccnt;
	data	U16	tw_Freq;
	xdata	U8	rssi;
	xdata	U8	envelop;
	xdata	U8	fd_num;
	xdata	U8	Temp[6]={0x00};
	xdata	U8	stc=0;
	xdata	U8	tc_TmpData;

	tw_Freq=875;
	gc_FM_CHAmount=0;
	CL6017ControlData[0]|=0x20;
	CL6017ControlData[2]&=0xC3;            // clear VOLUME[3:0] bits
	CL6017ControlData[2]|=(0<<2);          // set VOLUME[3:0] to the desired volume value
	SendStr(0x20,CL6017ControlData,4);

	// Mute
	XBYTE[0xB0C4]=31;   //(Jimi 091030)Headphone gain Lch
	XBYTE[0xB0C5]=31;   //(Jimi 091030)Headphone gain Rch
	IIC_Dealy(10);//10*7.5=75ms
	while(1)
	{
		Key_Detect();
		if(gc_KeyEvent==6)	// M
		{
			break;
		}

		gb_FM_Mute=1;           
		CL6017_FMTune(tw_Freq); 

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
				CL6017ControlData[0] &= 0xdf;
				CL6017ControlData[2] &= 0xc3;             // clear VOLUME[3:0] bits
				CL6017ControlData[2] |= (gc_FM_Vol<<2);     // set VOLUME[3:0] to the desired volume value
				SendStr(0x20,CL6017ControlData,4);
				IIC_Dealy(10);
				gb_FM_Mute=0;
				CL6017_FMTune(tw_Freq);
				gw_FM_CH[cnt]=tw_Freq;
				gc_FM_CHAmount=cnt;
				CL6017ControlData[0] |= 0x20;
				CL6017ControlData[2] &= 0xc3;        // clear VOLUME[3:0] bits
				CL6017ControlData[2] |= (0<<2); // set VOLUME[3:0] to the desired volume value
				SendStr(0x20,CL6017ControlData,4);
				IIC_Dealy(10);
				cnt++;
				LCM_Show_FM_CHNUM(cnt);
			}
		}

		tw_Freq++;
		if((tw_Freq>1080)||(cnt>=50))
		{
			break;
		}
	}

	CL6017ControlData[0] &= 0xdf;
	CL6017ControlData[2] &= 0xc3;        // clear VOLUME[3:0] bits
	CL6017ControlData[2] |= (gc_FM_Vol<<2); // set VOLUME[3:0] to the desired volume value
	SendStr(0x20,CL6017ControlData,4);
	IIC_Dealy(10);
	gb_FM_Mute=0;

	if(gc_FM_CHAmount==0)
	{
		gw_FM_CH[0]=875;
	}
	gw_FM_frequency=gw_FM_CH[0]; 

	gc_FM_CH_COUNT=0;
	CL6017_FMTune(gw_FM_frequency);
	tc_TmpData=31-(gs_DSP_GLOBAL_RAM.sc_Volume/2);
	XBYTE[0xB0C4]=tc_TmpData;   //(Jimi 091030)Headphone gain Lch
	XBYTE[0xB0C5]=tc_TmpData;   //(Jimi 091030)Headphone gain Rch
	LCM_Show_FM_CHNUM(1);
	return cnt;
}

