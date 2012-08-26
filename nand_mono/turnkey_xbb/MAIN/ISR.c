#include "SPDA2K.h"
#include "..\header\variables.h"

bit gbLKeyTimer_Timeout=0;                  // Flag for long-press key detect
xdata U8 gcGetKey_Timeout=0;                // Flag of key scan peroid. One timer peroid.

data U8 gc_IR_Setting=0;
data bit gb_USB_ISR_Detected=0;
xdata U8 gc_USBRST_ISR_Detected=0;  //20090630 chiayen add  
data U8 gc_ircount=0;
data U16 gw_ChangeFrequencyTimer=0;
data U16 gw_IR_Setting_Timer=0;
data U16 gw_LogData_Timer=0;                // times of timer peroid of logging data.
xdata U8 gc_LogDataFlag=0;                  // Flag of we should log data.
extern data	bit	gb_Frequency_Song;
extern xdata	U8	gc_SelectEQ;
extern xdata	U8	gc_Err;
extern data	bit	gb_PickSong;
extern data	bit	gb_PickSongSet;
extern data	bit	gb_ChannelSet;
extern data	bit	gb_HostConnect;
extern data	U8	gc_ShowTimer;
extern data	U16	gw_IR_Timer;
extern xdata	U16	gw_DisplayFreq;
extern xdata	U16	gw_DisplayTime;
extern xdata U16 gw_USBDisplayTime;
extern xdata	U16	gw_DisplayBitrateTime;
extern data	U16	gw_LCMScrollTimer;
extern data  U8 gc_Dirchange_Timer;

extern data	U16	gw_DisplayPinPuTime;
extern xdata U8 gc_FmFreqChg_Flag;

extern void System_HostDetect();
extern xdata U8 gc_FreqCtrl_Flag;
extern xdata U8 gc_IRNum_Count;

extern data	U8	gc_LCM_Media;
//=======================================================================
/*****************************************************************************
* NAME:         void _HostISR(void)
* DESCRIPTION:
*
*****************************************************************************/
void HostISR(void) interrupt 0 using 1 
{
	EX0 = 0;
	
	if(IR_REG[0x19]!=0)  //for IR
	{
		if(IR_REG[0x19]==1)
		{
			gc_IRCmdStatus=1;
			gc_ircount=0;
		}
		if(IR_REG[0x19]==2)
		{
			gc_ircount++;
			if(gc_ircount>1)
			{
				gc_IRCmdStatus=2;
				gc_ircount=0;
			}
		}
		IR_REG[0x19] = 7;
	}
	else if(gbt_USB_Detected)  
	{
		USB_REG[0xD0] = 0x00;
		USB_REG[0xA0] = 0x00;
		if(USB_REG[0xC3]==0x10){    	
	    	gc_USB_ADDR = USB_REG[0xF2];
	    } 
	    if ((USB_REG[0xC2] & 0x02)) { // End Point 2 in (Bulk Out)
	     	
	     	USB_REG[0xC2]   &= 0xFD;   
	
			gbt_USB_CMD_IN     = 1;
		}
		else
		{
			if(USB_REG[0xC0]&0x08)
			{
				gc_USBRST_ISR_Detected=1;  //20090629 chiayen add
			    gbt_USBRST_int_flag = 1;		    
			}   
			else if(USB_REG[0xC0]&0x01)
			{
				gb_USB_ISR_Detected=1;  //20090629 chiayen move here
			    gbt_EP0SETUP_int_flag   = 1;		    
			}
			else if(USB_REG[0xC0]&0x02)
			{ 
			    gbt_EP0TX_int_flag      = 1;
			}
			else if(USB_REG[0xC0]&0x04)
			{
			   gbt_EP0RX_int_flag      = 1;
			}
			else if(USB_REG[0xC0]&0x10)
			{ 
			   gbt_SUSPEND_int_flag = 1;
			}
			else if(USB_REG[0xC0]&0x20)
			{ 
			   gbt_USB_CONFIG_Chg_int_flag = 1;
			}
			else if(USB_REG[0xC0]&0x40)
			{ 
			   
			}
			else if(USB_REG[0xC0]&0x80)
			{ 
			   gbt_RESUME_int_flag = 1;
			}
			else
			{
			  
			}
			
			Control_Transfer_Process();
	        USB_REG[0xC0] = 0x00;
	
	       	if(gc_CX_Status_flag&CX_STL	)
			{
	       		USB_REG[0xE8] =0x03;
	       		gc_CX_Status_flag =0;
	       	}
	       	else
			{
	       		USB_REG[0xE8] =0x00;
	       	}
	       	
	       	USB_REG[0xA0] = 0x03;
		}
		USB_REG[0xD0] |= 0xFF;  //20081211 chiayen modify
		USB_REG[0xc3]  = 0x00;  
		USB_REG[0xD3]  = 0x10;
	}
	EX0 = 1;		// enable INT 0
}

/*****************************************************************************
* NAME:         void Timer0(void)
* DESCRIPTION:
*
*****************************************************************************/
void Timer0() interrupt 1 using 2
{
    TR0 = 0;
	TH0 = gc_T0IntervalMSB;
	TL0 = gc_T0IntervalLSB;
    TR0 = 1;

	gw_Random_Timer++;

	if(gbt_USB_Detected)
	{
		if(gw_USBDisplayTime!=0)
		{
			gw_USBDisplayTime--;
		}
	}

	gc_XTimeBase++;
	gc_XTimeBase&=0x03;

	if(gc_Vrefinit_Timer!=0)
	{
		gc_Vrefinit_Timer--;
	}
	switch(gc_XTimeBase)
	{
		case 0:

		break;
		case 1:			
			if(gbLKeyTimer_Timeout==0)
			{
				gbLKeyTimer_Timeout=1;	
			}			
		break;
		
		case 2:
			if(gcGetKey_Timeout==0)
			{
				gcGetKey_Timeout=1;	
			}		

			if(gw_LogData_Timer!=0)
			{
				gw_LogData_Timer--;
				if(gw_LogData_Timer==0)
				{
					gc_LogDataFlag=1;
				}
			}

			if(gc_ShowTimer!=0)
			{
				gc_ShowTimer--;
				if(gc_ShowTimer==0)
				{
					gb_Frequency_Song=0;
					gc_SelectEQ=0;
					gc_Err=0;
					gc_Dirchange_Timer=0;//change by homeing.
					gc_FmFreqChg_Flag=0;
					gw_IR_Timer=0;
					gc_IRNum_Count=0;
					gc_LCM_Media=0xff;
					gw_LCMScrollTimer=0;
					gw_DisplayPinPuTime=0;
				}
			}

			if(gw_DisplayBitrateTime!=0)
			{
				gw_DisplayBitrateTime--;
			}
			//====sunzhk add pinpu
			if(gw_DisplayPinPuTime!=0)
			{
				gw_DisplayPinPuTime--;
			}
			//===addover
			if(gw_IR_Setting_Timer!=0)
			{
				gw_IR_Setting_Timer--;
				if(gw_IR_Setting_Timer==0)
				{
					gc_IR_Setting=0;
				}
			}
		
			if(gw_ChangeFrequencyTimer!=0)
			{
				gw_ChangeFrequencyTimer--;
			}
		
			if(gw_LCMScrollTimer!=0)
			{
				gw_LCMScrollTimer--;
			}

		//	if(gc_Vrefinit_Timer!=0)
		//	{
		//		gc_Vrefinit_Timer--;
		//	}

			if(gc_PlayMenu_Timeout!=0)  //20090107 chiayen add
			{
				gc_PlayMenu_Timeout--;
			}

			if(gc_Dirchange_Timer!=0)  //20090107 chiayen add
			{
				gc_Dirchange_Timer--;
			}

			if(gw_IR_Timer!=0)
			{
				gw_IR_Timer--;
				if(gw_IR_Timer==0)
				{
                  
					if(gb_PickSong==1 || gc_Num_Flag==1)//gc_Num_Flag ，默认输入的数字是点歌的
					{
						gb_PickSongSet=1;
					}
					gb_ChannelSet=0;
					gc_IRNum_Count=0;
                   // gc_Num_Flag = 0;
                    
				}
			}

			if((gb_ChannelSet==0) && (gb_PickSong==0) && gc_Num_Flag==0)
			{
				gc_NUMBER[0]=0xFF;
				gc_NUMBER[1]=0xFF;
				gc_NUMBER[2]=0xFF;
				gc_NUMBER[3]=0xFF;
			}		
		break;

		case 3:

		break;

		default:
        break;
	}//switch
	//System_HostDetect();	
	{
		U8 i;
		if(gb_HostConnect == 1)
		{	
			for(i=0;i<10;i++)
			{
				if(XBYTE[0xB615]!=0x00)
				{
					return;
				}
			}
			gb_HostConnect = 0;
		}
		return;
	}
}



