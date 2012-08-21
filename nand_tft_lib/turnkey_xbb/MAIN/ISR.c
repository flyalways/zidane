#include "SPDA2K.h"
#include "..\header\variables.h"
  
extern	data	bit	gb_HostConnect;  
//extern	data	U8	gc_PKSongCHSetMode;
extern	xdata	U8	gc_ShowMenuTimer;

extern xdata U8 gb_NumCHSet;

extern void System_HostDetect();

//=======================================================================
/*****************************************************************************
* NAME:         void _HostISR(void)
* DESCRIPTION:
*
*****************************************************************************/
void HostISR(void) interrupt 0 using 1 
{
	U8	tc_USB_flag;
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
			if(gc_ircount>20)
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
			tc_USB_flag=USB_REG[0xC0];
			if(tc_USB_flag&0x08)	// RESET
			{
			    gbt_USBRST_int_flag = 1;		    
			}   
			else if(tc_USB_flag&0x01)	// SETUP
			{
				gb_USB_ISR_Detected=1;
			    gbt_EP0SETUP_int_flag   = 1;		    
			}
			else if(tc_USB_flag&0x02)	// OUT
			{ 
			    gbt_EP0TX_int_flag      = 1;
			}
			else if(tc_USB_flag&0x04)	// IN
			{
			   gbt_EP0RX_int_flag      = 1;
			}
			else if(tc_USB_flag&0x10)	// SUSPEND
			{ 
			   gbt_SUSPEND_int_flag = 1;
			}
			else if(tc_USB_flag&0x20)
			{ 
			   gbt_USB_CONFIG_Chg_int_flag = 1;
			}
			else if(tc_USB_flag&0x40)	// WAKEUP
			{ 
			   
			}
			else if(tc_USB_flag&0x80)	// RESUME
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
//		return;
	}

	gc_XTimeBase = (gc_XTimeBase >= 3)? 0:(gc_XTimeBase+1);//(JC)Timebase x 4;Timebase=~7.5ms
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
			if(gbGetKey_Timeout==0)
			{
				gbGetKey_Timeout=1;	
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
					gc_TuneVolFreqStatus=0;
					gb_Frequency_Song=0;
					gb_SelectEQ=0;
//					gb_Err=0;
				}
			}

			if(gc_ShowMenuTimer!=0)
			{
				gc_ShowMenuTimer--;
			}

			if(gw_DisplayBitrateTime!=0)
			{
				gw_DisplayBitrateTime--;
			}

			if(gw_IR_Setting_Timer!=0)
			{
				gw_IR_Setting_Timer--;
				if(gw_IR_Setting_Timer==0)
				{
					gb_IR_Setting=0;
				}
			}
		
			if(gw_AutoJpeg_Timer!=0)  //20090331
			{
				gw_AutoJpeg_Timer--;
			}
		
			if(gw_ChangeFrequencyTimer!=0)
			{
				gw_ChangeFrequencyTimer--;
			}
		
			if(gw_LCMScrollTimer!=0)
			{
				gw_LCMScrollTimer--;
			}

			if(gc_Vrefinit_Timer!=0)
			{
				gc_Vrefinit_Timer--;
			}

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
					gb_PickSongSet=1;
					gc_ShowTimer=72;
				}		
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



