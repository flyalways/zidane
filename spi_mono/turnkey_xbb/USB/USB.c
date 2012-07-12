#include "..\header\SPDA2K.h"
#include "..\LCM\LCM_BMP.h"
#include "..\header\variables.h"
#include "..\header\usb_data.h"


extern void LCM_Disp_MyMP3Logo();
extern void LCM_Disp_PC();
extern void LCM_Disp_USBLine();

void USB_Task()
{
	set_clock_mode(CLOCK_MODE_USB);                      
	USB_Mode();

	XBYTE[0xB010]&=0xFE;	// Disable DSP clock, Jimi 080729 for pop noise as 1st time play
	XBYTE[0xB002]&=0xFE;	// Release DSP Reset, Jimi 080729 for pop noise as 1st time play
}

void USB_DisConnect(void)
{
	XBYTE[0xB01D]=0x01;
	XBYTE[0xB017]=0x00;
	XBYTE[0xB5A1]=0x00;
    XBYTE[0xB5C0]=0x00;  //clear suspend/resume/configuration change		      //turn off USB D+
	XBYTE[0xB5C2]=0x00;
	//Adding for avoid losting power
	XBYTE[0xB5D0]=0x00;
	XBYTE[0xB5D1]=0x00;
	XBYTE[0xB5D2]=0x00;
	XBYTE[0xB5D3]=0x00;
	XBYTE[0xB016]=0x05; //Close USB UTMI 30Mhz  USB匿堤綴參clock燊賸
	XBYTE[0xB016]=0x01; //Close USB UTMI 30Mhz  USB匿堤綴參clock燊賸
}


void USB_Mode(void) 
{
	gb_USB_Plugin=1;
	set_clock_mode(CLOCK_MODE_USB);
    PowerOn_Init_USB();
	USBInitialize(); 
	XBYTE[0xB527]=0x00;	// 080304 jay change for new fpga
	XBYTE[0xB017]=0x01;	// UDC O/P	
    IE=0x83;	   
	gdw_USB_CountDown=0x1FFFF;
	while(gb_USBRST_ISR_Detected==0)    // Bulk_In_Out  將跳出while 迴圈  //20090630 chiayen modify
	{
    	if((--gdw_USB_CountDown==0))
		{
			goto USB_QUIT;
    	}
	}
	
	set_clock_mode(CLOCK_MODE_MP3);
	LCM_ShowUSBConnectIcon();
	set_clock_mode(CLOCK_MODE_USB);

	EA=0;   
	while(1)
	{
		gdw_USB_CountDown=0x3FFFF;
		if(gc_USBStage==CBWStage) 
		{			
			EA=1;							
			while(!gb_USB_CMD_IN)                        // Bulk_In_Out  將跳出while 迴圈
			{
				if(gb_USB_Connect_Flag)
				{
					if((gb_USER_Remove_Flag)||(--gdw_USB_CountDown==0))
					{
						goto USB_QUIT;
					} 
				}
				else
				{
			    	if(--gdw_USB_CountDown==0)  
					{
						goto USB_QUIT;
					}
				}
			}
			EA=0;

			gb_USB_CMD_IN=0;
			SD_Card_Detect();		
			gb_USB_Connect_Flag=1;
	    	gdw_USB_CountDown=0x3FFFF; 
			Save_CBW_Packet();
			Decode_CBW_Command();                             // 解CBW和記錄相關手續		    
		}

		#if 0
		if(gw_USBDisplayTime==0)
		{
			USB_Mode_Disp();
		}
		#endif

		if(gc_USBStage==BulkInReadSectorsStage) 
		{
			XBYTE[0xB5D2]&=0xDD;				//Disable Bulk-Out中斷
			ReadSectors();			    
			if(gb_USB_Plugin==0)
			{
USB_QUIT:
				set_clock_mode(CLOCK_MODE_MP3);
				USB_DisConnect();
				gb_USB_Plugin=0;
				return;
			}
				
			XBYTE[0xB5E9]=0x11;
			gdw_dCBW_DataXferLength.LW=0;
			gc_USBStage = CSWStage;                                   // 進入Comand Status Wrapper
		}

		if (gc_USBStage == BulkOutWriteSectorsStage) 
		{
			XBYTE[0xB5D2]&=0xDD;	//Disable Bulk-Out中斷
			XBYTE[0xB511]=0x01;		//HW auto ACK 
			WriteSectors();
			XBYTE[0xB511]=0x00;
			XBYTE[0xB5E9]=0x22;
			gc_bCSWStatus = 0x00;
			gc_RequestSenseCode = 0xFF;                        // normal
			gdw_dCBW_DataXferLength.LW = 0;
			gc_USBStage = CSWStage;
		}

		if(gc_USBStage==BulkInResponseStage)
		{
			XBYTE[0xB5D2]&=0xDD;				//Disable Bulk-Out中斷
			if(gb_XferNormalResponseData_ModeSense) 
			{                      // '1':Normal
		   		if(gc_COMD_IN[0]==0x23) 
				{
					gdw_dCBW_DataXferLength.WD[1] = 0x00;
					XBYTE[0xB515]=0x0C;
					XBYTE[0xB516]=0x00;
					XBYTE[0xB5A1]|=0x01;
					while(XBYTE[0xB5A1]&0x01)
					{
					}
					XBYTE[0xB5E9]=0x01;
		   		}
		   		else 
				{
					while(gdw_dCBW_DataXferLength.WD[1]) 
					{
						if(gdw_dCBW_DataXferLength.WD[1]>64) 
						{
							gdw_dCBW_DataXferLength.WD[1] = gdw_dCBW_DataXferLength.WD[1] - 64;
							XBYTE[0xB515]=0x40;
				       		XBYTE[0xB516]=0x00;
						}
						else 
						{
							XBYTE[0xB515]=gdw_dCBW_DataXferLength.BY[3];
				       		XBYTE[0xB516]=gdw_dCBW_DataXferLength.BY[2]&0x0F;
					       	gdw_dCBW_DataXferLength.WD[1] = 0x00;
				       	}
						XBYTE[0xB5A1]|=0x01;
						while(XBYTE[0xB5A1]&0x01)
						{
						}
						XBYTE[0xB5E9]=0x01;
					}
		   		}
			}
			else 
			{
				if(gc_COMD_IN[0]==0x1A) 
				{
					gdw_dCBW_DataXferLength.WD[1] = 0x00;
					XBYTE[0xB515]=0x04;
					XBYTE[0xB516]=0x00;
					XBYTE[0xB5A1]|=0x01;
					while(XBYTE[0xB5A1]&0x01)
					{
					}
					XBYTE[0xB5E9]=0x01;
				}
				else 
				{
					gdw_dCBW_DataXferLength.WD[1] = 0x00;
					XBYTE[0xB515]=0x08;
					XBYTE[0xB516]=0x00;
					XBYTE[0xB5A1]|=0x01;
					while(XBYTE[0xB5A1]&0x01)
					{
					}
					XBYTE[0xB5E9]=0x01;
				}
				gb_XferNormalResponseData_ModeSense=NormalResponseData;// 將預設設為一般型
			}
			gc_USBStage = CSWStage;
		}

		if(gc_USBStage==BulkStallStage) 
		{
			StallBulkRW();
			gc_USBStage = CSWStage;
		}

		if(gc_USBStage==BulkInReadBufferStage) 
		{
			XBYTE[0xB5D2]&=0xDD;				//Disable Bulk-Out中斷
			NoCard_ReadSectors();		    
			XBYTE[0xB5E9]=0x11;
			gdw_dCBW_DataXferLength.LW=0;			
			gc_USBStage=CSWStage;
		}

		if(gc_USBStage==BulkOutWriteBufferStage) 
		{
			XBYTE[0xB5D2]&=0xDD;				//Disable Bulk-Out中斷
			XBYTE[0xB511]=0x01;   //HW auto ACK 
			NoCard_WriteSectors();			
			XBYTE[0xB511]=0x00;
			XBYTE[0xB5E9]=0x22;
			gdw_dCBW_DataXferLength.LW=0;
			gc_USBStage=CSWStage;
		}

		if(gc_USBStage==CSWStage)
		{
			gc_USBStage=CBWStage;
			USB_BUF_Bank0[ 0]='U';
			USB_BUF_Bank0[ 1]='S';
			USB_BUF_Bank0[ 2]='B';
			USB_BUF_Bank0[ 3]='S';
			USB_BUF_Bank0[ 4]=gc_dCBW_Tag[0];
			USB_BUF_Bank0[ 5]=gc_dCBW_Tag[1];
			USB_BUF_Bank0[ 6]=gc_dCBW_Tag[2];
			USB_BUF_Bank0[ 7]=gc_dCBW_Tag[3];
			USB_BUF_Bank0[ 8]=gdw_dCBW_DataXferLength.BY[3];
			USB_BUF_Bank0[ 9]=gdw_dCBW_DataXferLength.BY[2];
			USB_BUF_Bank0[10]=gdw_dCBW_DataXferLength.BY[1];
			USB_BUF_Bank0[11]=gdw_dCBW_DataXferLength.BY[0];
			USB_BUF_Bank0[12]=gc_bCSWStatus;
			XBYTE[0xB515]=0x0D;
			XBYTE[0xB516]=0x00;
			XBYTE[0xB5A1]|=0x01;
			gdw_USB_CountDown=1000;
			while(XBYTE[0xB5A1]&0x01)
			{
				if(--gdw_USB_CountDown==0)
				{
					goto USB_QUIT;
				}
			}
		}
		XBYTE[0xB5E9]=0x01;			
		XBYTE[0xB5D2]|=0x02;	//Enable Bulk-Out中斷
		XBYTE[0xB5A1]|=0x02;		//下一筆31Byte Bulk-Out
	}
}



#if 0
void USB_Mode_Disp(void)
{
	U8 tc_status;

	gw_USBDisplayTime=40;

	tc_status=DEVICE_REG[0x00];
	DEVICE_REG[0x00]=0x01;

	if((gc_USBStage == BulkInReadSectorsStage)||(gc_USBStage == BulkInResponseStage)||(gc_USBStage == BulkInResponseStage)||(gc_USBStage ==BulkInReadBufferStage))
	{
		if(gc_USBDispStatus!=1)
		{
			LCM_Disp_USBRWLogo(0);
			gc_USBDispStatus=1;
		}
	}
	else if((gc_USBStage == BulkOutWriteSectorsStage)||(gc_USBStage == BulkOutStage)||(gc_USBStage == BulkOutWriteBufferStage))
	{
		if(gc_USBDispStatus!=2)
		{
			LCM_Disp_USBRWLogo(1);
			gc_USBDispStatus=2;
		}
	}
	else
	{	
		if(gc_USBDispStatus!=3)
		{
			LCM_Disp_USBLine();
			gc_USBDispStatus=3;
		}
	}

	DEVICE_REG[0x00]=tc_status;

}
#endif 

		 

