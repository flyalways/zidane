#include "..\header\SPDA2K.h"
#include "..\header\variables.h"
#include "..\LCM\LCM_BMP.h"

void Timer0() interrupt 1 using 2
{
	TR0=0;
	TH0=gc_T0IntervalMSB;
	TL0=gc_T0IntervalLSB;	
	TR0=1;	

	gw_Random_Timer++;
	if(gc_CL6017Cont)
	{
		gc_CL6017Cont--;
	}

	if(gc_IR_press_timer!=0)
	{
		gc_IR_press_timer--;
		if(gc_IR_press_timer==0)
		{
			gc_IRCmdStatus=1;
		}
	}

	gc_XTimeBase=(gc_XTimeBase+1)&0x03;
	gb_SD_pin=SD_Detect;

	switch(gc_XTimeBase)
	{
		case 0:
			if(gb_SARStable==0)
			{
				// Wait SARIN stable
				Get_KeyValue(READ_SARADC(0));
				if(gc_key_Pressed==C_Key_None)
				{
					gb_SARStable=1;
				}
			}
			else
			{
				ScanKey();
			}
		break;
		
		case 1:			
			if(gc_MenuEndTimer)
			{
				gc_MenuEndTimer--;
			}

			if(gc_PinPuTimer)
			{
				gc_PinPuTimer--;
				if(gc_PinPuTimer==0)
				{
					gc_PinPuCount=(gc_PinPuCount+1)%15;
				}
			}
			
		break;
		
		case 2:
			if(gc_LogData_Timer!=0)
			{
				gc_LogData_Timer--;
				if(gc_LogData_Timer==0)
				{
					gb_LogDataFlag=1;
				}
			}

			if(gc_ShowTimer!=0)
			{
				gc_ShowTimer--;
			}
	
			if(gc_LCMScrollTimer!=0)
			{
				gc_LCMScrollTimer--;
			}

			if(gc_PlayMenu_Timer!=0)
			{
				gc_PlayMenu_Timer--;
			}

			if(gc_ShowFMRVolTimer!=0)
			{
				gc_ShowFMRVolTimer--;
			}

			if(gc_IR_Timer!=0)
			{
				gc_IR_Timer--;
			}
		break;

		case 3:
			gw_MinuteTimer++;
			if(gw_MinuteTimer>33)
			{
				gw_MinuteTimer=0;
				READ_SARADC(2);
			}

			if(gc_ShowRepeatTimer!=0)
			{
				gc_ShowRepeatTimer--;
				if(gc_ShowRepeatTimer==0)
				{
					gb_FlashRepeatIcon=!gb_FlashRepeatIcon;
				}
			}
		break;

		default:
        break;
	}//switch

	if(gb_HostConnect==1)
	{	
		if(XBYTE[0xB615]==0x00)
		{
			gc_Host_remove_detect_counter++;
			if(gc_Host_remove_detect_counter>10)
			{
				gb_HostConnect=0;	
			}
		}
		else
		{
			gc_Host_remove_detect_counter=0;
		}
	}
	else
	{
		gc_Host_remove_detect_counter=0;
	}
}


void RTC_SnapValue(void)
{
	xdata	U8	tc_RTC[5];
	data	U8	i;

	RTC_Write(0xB1,0x01);		// Snap time value
	i=10;
	while(--i)
	{
		if(RTC_Read(0x00)==0x10)	// Wait snap done
		{
			break;
		}
	}

	tc_RTC[0]=RTC_Read(0x11);
	tc_RTC[1]=RTC_Read(0x12);
	tc_RTC[2]=RTC_Read(0x13);
	tc_RTC[3]=RTC_Read(0x14);
	tc_RTC[4]=RTC_Read(0x15);
	gdw_RTC_Second=((U32)tc_RTC[4]<<24)+((U32)tc_RTC[3]<<16)+((U16)tc_RTC[2]<<8)+tc_RTC[1];
	gdw_RTC_Second=(gdw_RTC_Second<<1)+(tc_RTC[0]>>7);
	XBYTE[0xB068]=0xC0;
	XBYTE[0xB069]=0;
	XBYTE[0xB067]=1; // trigger write
	while((XBYTE[0xB06B]&0x01)==0);
}

void HostISR(void) interrupt 0 using 2
{
	data	U8	tc_B5C0;
	data	U32	tdw_RTCTime;

	EX0=0;

	if((XBYTE[0xB0C0]&0x08))	// RTC interrupt
	{
		XBYTE[0xB0C0]&=~0x08;	// Clear RTC interrupt
		gb_RTC_wakeup=1;
		RTC_SnapValue();
		tdw_RTCTime=gdw_RTC_Second%86400;
		if(gb_OrderRecordConfirm==1)
		{
			if(tdw_RTCTime==gdw_OrderRecordTime)
			{
				gb_OrderRecordConfirm=0;
				gb_OrderRecordTrigger=1;
			}
		}

		if(gb_AlarmON==1)
		{
			if(tdw_RTCTime==gdw_AlarmTime)
			{
				gb_AlarmTrigger=1;
			}
		}

		if((gc_LCMBacklightTimer!=0)&&(gc_LCMBacklightTimer!=0xFF))
		{
			LCM_BACKLIGHT=0;
			gc_LCMBacklightTimer--;
			if(gc_LCMBacklightTimer==0)
			{
				LCM_BACKLIGHT=1;
			}
		}

		if(gc_MenuTimer!=0)
		{
			gc_MenuTimer--;
		}

		if(gc_Idle2Suspend!=0)
		{
			gc_Idle2Suspend--;
		}

		if(gw_AlarmClockTimer!=0)
		{
			gw_AlarmClockTimer--;
		}
	}

	if(XBYTE[0xBA19]!=0)	// IR interrupt
	{
		if(XBYTE[0xBA19]==1)
		{
			gb_IR_wakeup=1;
			gc_IRLongKeyCount=1;
			gc_IR_press_timer=17;
		}
		else if(XBYTE[0xBA19]==2)
		{
			if(gc_IRLongKeyCount!=0xEE)
			{
				gc_IRLongKeyCount++; //計算長按鍵
				gc_IR_press_timer=17;
				if(gc_IRLongKeyCount==IRKEY_REPEATTIMES) 
				{
					gc_IRCmdStatus=2; //長按鍵的狀態
					gc_IR_press_timer=0;
					gc_IRLongKeyCount=0xEE;
				}
			}
		}
		else
		{
			gc_IRCmdStatus=0;
		}
		XBYTE[0xBA19]=0x07;
	}

	if(gb_USB_Plugin==1)	// USB interrupt
	{
		XBYTE[0xB5D0]=0x00;
		XBYTE[0xB5A0]=0x00;
		if(XBYTE[0xB5C3]==0x10)
		{
	    	gc_USB_ADDR=XBYTE[0xB5F2];
	    } 
	    
		if ((XBYTE[0xB5C2]&0x02)) 
		{ // End Point 2 in (Bulk Out)
	     	XBYTE[0xB5C2]&=~0x02;   
			gb_USB_CMD_IN=1;
		}
		else
		{
			tc_B5C0=XBYTE[0xB5C0];
			if(tc_B5C0&0x08)
			{
				gb_USBRST_ISR_Detected=1;
				XBYTE[0xB002]|=0x04;	// SW Reset
			    XBYTE[0xB002]=0x03;
				XBYTE[0xB527]=0x00;
				XBYTE[0xB5E9]=0xFF;		// Clear ALL USB Buffer.
				XBYTE[0xB5EA]=0xFF;
				XBYTE[0xB5A0]&=0xFC;
				XBYTE[0xB5A1]=0x02;
				XBYTE[0xB5D2]=0x02;		//因為Reset完後要收Host端的Command.		
			}   
			else if(tc_B5C0&0x01)
			{
				Read_Setup_Packet();	//Get the 8-byte setup packet and decode these commands   		 		
			}
			else if(tc_B5C0&0x02)
			{ 
		   		while(XBYTE[0xB5A0]&0x01);
			}
			else if(tc_B5C0&0x04)
			{		     
			   	while(XBYTE[0xB5A0]&0x02); 
			}
			else if(tc_B5C0&0x10)
			{
				if(XBYTE[0xB5B1]&0xF0) 
				{
					XBYTE[0xB5D0]|=0x10;
				}
				if(gb_USB_Connect_Flag)
				{
				    gb_USER_Remove_Flag=1;
				}	
			}
			else if(tc_B5C0&0x20)
			{ 
				if(XBYTE[0xB5B1]&0xF0) 
				{
					XBYTE[0xB5D0]|=0x10;
				}
			}
			else if(tc_B5C0&0x80)
			{ 
				XBYTE[0xB5D0]&=0xEF;	//Disable Suspend INT
				XBYTE[0xB5D0]|=0x80;	//Enable Resume INT
				gb_USB_Connect_Flag=0;
				gb_USER_Remove_Flag=0;
			}
	        XBYTE[0xB5C0]=0x00;
	
	       	if(gc_CX_Status_flag&CX_STL)
			{
	       		XBYTE[0xB5E8]=0x03;
	       		gc_CX_Status_flag =0;
	       	}
	       	else
			{
	       		XBYTE[0xB5E8]=0x00;
	       	}
	       	XBYTE[0xB5A0]=0x03;
		}
		XBYTE[0xB5D0]=0xFF;
		XBYTE[0xB5C3]=0x00;  
		XBYTE[0xB5D3]=0x10;
	}
	EX0=1;		// enable INT 0
}


