#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\header\host_init.h"
#include "..\header\variables.h"

data	bit	gb_HostInitial;
data	bit	gb_dtg_in=0;
data	bit	gb_dtg_out=0;
data	bit	gb_HostConnect;

xdata	U8	EP_IN;
xdata	U8	EP_OUT;
xdata	U8	CFGValue;
xdata	U8	DeviceAddress;
xdata	U8	ConfigurationDescriptorLength;
xdata	U8	Max_Trans_Length;
xdata	U8	gc_HostUnitNum;
xdata	U16	gw_VID;

extern  xdata 	U8  Host_StallCmd_Timeout; 
extern	xdata	U32	gdw_CARD_TotalSizeMB;  
extern	xdata	U32	gdw_HOSTStartSectorRead;

extern	data	bit	gb_stall_flag;
extern	data	bit	gb_shortpack;
extern	data	bit gb_Repeat;

extern	void tran_in_(U8 addr,U8 amount,U16 offset);
extern	U8		HOST_Read_Logicsector(U32 tdw_LogicSectorAddr);

void Host_register_INIT(void)
{
	XBYTE[0xB011] |= 0x20;   //bit 4   turn on usb_device clk
    XBYTE[0xB002] |= 0x04;   //bit 2   turn on USB_SW_RESET
    XBYTE[0xB002]&=~0x04;  //bit 2   trun off USB_SW_RESET
	XBYTE[0xB0E9]&=~0x01;
    XBYTE[0xB017]  = 0x01;   //USB2.0 UTMI op mode set
    XBYTE[0xB002] |= 0x02;   //bit 2   turn on USB_host_SW_RESET
	XBYTE[0xB002]&=~0x02;  //bit 2   turn off USB_host_SW_RESET
 	XBYTE[0xB5e6]  = 0x00;	 //bulk sram gated clock
	XBYTE[0xB522]  = 0x01;	 //USB2.0 UTMI clock mode set				
	Syn_Reset();
	XBYTE[0xB019]  = 0x01;   //host mode
}

U8 Host_Initial(void)
{
	data	U8	i;
	data	U8	EP_flag=0;
	data	U8	states;
	data	U8	tc_StrProductName;
	data	U8	tc_StrSerialNum;
	data	U16	Stg_idx;

USB_HOST_Init:
	DeviceAddress = 1;
	gb_HostConnect = 1;
	gb_HostInitial=1;

	EA=0;
	XBYTE[0xB530]=0xFF;
	XBYTE[0xB01F]=0x01;
	Host_register_INIT();
	XBYTE[0xB01C]&=0xDF;	//Auto Ack in	
	delay(5000);

	// USB SUSPEND
	XBYTE[0xB610]=0x04; //force full-speed
	delay(60000);
	XBYTE[0xB610]=0x04;	
	delay(200);
	// USB RESET
	XBYTE[0xB610]=0x03; //force full-speed
	delay(20000);
	XBYTE[0xB610]=0x03;	
	delay(200);

	XBYTE[0xB630]=0x01;		//SOF ON
	XBYTE[0xB632]=0xB8;		//EOF point
	XBYTE[0xB633]=0x0B;
	delay(10000);

	gdw_pollingtime=320000;

	i=0;
	EA=1;
	while(1)
	{
		delay(2000);
		states = GetDescriptor(0,0x100,0,0x40);
		if(states == 0x00)
		{
        	Max_Trans_Length = XBYTE[HOST_BUF_SA+7];
			break;
		}
		else
		{
			i++;
			if(i==3)
			{
				gb_HostConnect=0;
				return HOST_GETDESCRIPTOR_FAIL;
			}

			if(gb_HostConnect == 0)
			{
				return HOST_PHYCOUNECT_FAIL;
			}
		}
	}
	EA=0;
	gb_HostInitial=0;
	XBYTE[0xB002] |= 0x02;   //bit 2   turn on USB_host_SW_RESET
	XBYTE[0xB002] &=~0x02;	 //bit 2   turn off USB_host_SW_RESET

	// USB RESET
	XBYTE[0xB610]=0x03; //force full-speed
	delay(1200);
	XBYTE[0xB610]=0x03;	
	delay(200);
	XBYTE[0xB630]=0x01;		//SOF ON
	XBYTE[0xB632]=0xB8;		//EOF point
	XBYTE[0xB633]=0x0B;
	delay(5000);
	XBYTE[0xB01C]|=0x10;	//clear RX error flags
	XBYTE[0xB01C]&=0xEF;
	EA=1;	

	// Set address--00 05 01 00 00 00 00 00
	states = SetAddress();
	if(states)
  	{
		return HOST_SETADDRESS_FAIL;
	}
	delay(50000);
	gb_dtg_in=0;
	gb_dtg_out=0;

	//---- 80 06 00 01 00 00 12 00----
    if(gb_HostConnect == 0)
	{
	    return HOST_PHYCOUNECT_FAIL;
	}
    states = GetDescriptor(DeviceAddress,0x0100,0x00,0x12);

	if(states)
 	{
		return HOST_GETDESCRIPTOR_FAIL;
	}

    tc_StrProductName=XBYTE[HOST_BUF_SA+15];
    tc_StrSerialNum=XBYTE[HOST_BUF_SA+16];

   	gw_VID=XBYTE[HOST_BUF_SA+8]+(XBYTE[HOST_BUF_SA+9]<<8);

	//---- 80 06 00 02 00 00 09 00 ----
	if(gb_HostConnect == 0)
	{
		return HOST_PHYCOUNECT_FAIL;
	}
	delay(1000);
	states = GetDescriptor(DeviceAddress,0x200,0,0x09);
	if(states)  
	{
		return HOST_GETDESCRIPTOR_FAIL;
	}	     
	Syn_Reset();
	ConfigurationDescriptorLength=XBYTE[HOST_BUF_SA+2];

	//---- 80 06 00 03 00 00 FF 00 ----
	//---- 80 06 3r 03 09 04 FF 00 ----
	if(tc_StrSerialNum!=0)
	{
        if(gb_HostConnect == 0)
		{
	    	return HOST_PHYCOUNECT_FAIL;
		}
		delay(1000);
		states = GetDescriptor(DeviceAddress,0x300,0,0xFF);

  		if(states) 
 		{
			return HOST_GETDESCRIPTOR_FAIL;
		}    			

	 	Stg_idx=(U16)(XBYTE[HOST_BUF_SA+3]<<8)+XBYTE[HOST_BUF_SA+2];

	    if(gb_HostConnect == 0)
		{
		    return HOST_PHYCOUNECT_FAIL;
		}
		delay(1000);
		states = GetDescriptor(DeviceAddress,tc_StrSerialNum+0x0300,Stg_idx,0xFF);

	    if(states) 
 		{
			return HOST_GETDESCRIPTOR_FAIL;
		}    			
	    Syn_Reset();
	}

	//---- 80 06 00 02 00 00 FF 00 ----
	if(gb_HostConnect == 0)
	{
		return HOST_PHYCOUNECT_FAIL;
	}
	delay(1000);
	states = GetDescriptor(DeviceAddress,0x200,0,0xFF);

	if(states)  
	{
		return HOST_GETDESCRIPTOR_FAIL;
	}    	

	//---- 80 06 00 06 00 00 0A 00 ----
//	delay(1000);
//	GetDescriptor(DeviceAddress,0x0600,0,0x0A);

	//---- 80 06 00 03 00 00 FF 00 ----
	//---- 80 06 PP 03 09 04 FF 00 ----
	//---- 80 06 00 03 00 00 FF 00 ----
	//---- 80 06 PP 03 09 04 FF 00 ----
	if(tc_StrProductName!=0)
	{
		U8	i;
		for(i=0;i<2;i++)
		{
	        if(gb_HostConnect == 0)
			{
		    	return HOST_PHYCOUNECT_FAIL;
			}
			delay(1000);
        	states = GetDescriptor(DeviceAddress,0x300,0,0xFF);

	  		if(states) 
	 		{
				return HOST_GETDESCRIPTOR_FAIL;
			}    			

		 	Stg_idx=(U16)(XBYTE[HOST_BUF_SA+3]<<8)+XBYTE[HOST_BUF_SA+2];

		    if(gb_HostConnect == 0)
			{
			    return HOST_PHYCOUNECT_FAIL;
			}
			delay(1000);
			states = GetDescriptor(DeviceAddress,tc_StrProductName+0x0300,Stg_idx,0xFF);
	
		    if(states)  
			{
				return HOST_GETDESCRIPTOR_FAIL;
			}    			
		}
	}

	//---- 80 06 00 01 00 00 12 00 ----
    if(gb_HostConnect == 0)
	{
	    return HOST_PHYCOUNECT_FAIL;
	}
	delay(1000);
    states = GetDescriptor(DeviceAddress,0x0100,0x00,0x12);
	if(states)
 	{
		return HOST_GETDESCRIPTOR_FAIL;
	}

	//---- 80 06 00 02 00 00 09 00 ----
	delay(1000);
	GetDescriptor(DeviceAddress,0x0200,0x00,0x09);
	//---- 80 06 00 02 00 00 xx 00 ----
	delay(1000);
	GetDescriptor(DeviceAddress,0x0200,0x00,ConfigurationDescriptorLength);

 	CFGValue=XBYTE[HOST_BUF_SA+0x05];

	//---- Set Endpoint Value ----
	for(i=0;i<ConfigurationDescriptorLength;)
	{
		if(XBYTE[HOST_BUF_SA+i]== 0x09)
		{
	 		i += 0x09;
		}
		else if(XBYTE[HOST_BUF_SA+i]== 0x07)
		{
			if((XBYTE[HOST_BUF_SA+i+1]==0x05)&&(XBYTE[HOST_BUF_SA+i+3]==0x02))
            {			
				if(XBYTE[HOST_BUF_SA+i+2]&0x80)
                { 
                	EP_IN   = XBYTE[HOST_BUF_SA+i+2]&0x0F;
                	EP_flag = EP_flag + 0x01;
                }
                else
                {              
  			    	EP_OUT  = XBYTE[HOST_BUF_SA+i+2]&0x0F;	
                	EP_flag = EP_flag + 0x10;
                }
			}
            if(EP_flag == 0x11)
			{
				break;
 			}
            i += 0x07;
		}
        else
		{
			i++;
		}
	}

	//---- 80 06 00 03 00 00 02 00 ----
	delay(1000);
	GetDescriptor(DeviceAddress,0x0300,0x00,0x02);
	//---- 80 06 00 03 00 00 04 00 ----
	delay(1000);
	GetDescriptor(DeviceAddress,0x0300,0x00,0x04);

	//---- 80 06 SS 03 09 04 02 00 ----
	if(tc_StrSerialNum!=0)
	{
		U8	tc_leng;
		if(gb_HostConnect == 0)
		{
			return HOST_PHYCOUNECT_FAIL;
		}
		delay(1000);
		states = GetDescriptor(DeviceAddress,0x300+tc_StrSerialNum,Stg_idx,0x02);
		if(states) 
		{
			return HOST_GETDESCRIPTOR_FAIL;
		}    			

		tc_leng=XBYTE[HOST_BUF_SA];

	    if(gb_HostConnect == 0)
		{
		    return HOST_PHYCOUNECT_FAIL;
		}
		delay(1000);
	    states = GetDescriptor(DeviceAddress,0x0300+tc_StrSerialNum,Stg_idx,tc_leng);

	    if(states)  
		{
			return HOST_GETDESCRIPTOR_FAIL;
		}    			
	}

	//---- 00 09 xx 00 00 00 00 00 ----
	if(gb_HostConnect == 0)
	{
		return HOST_PHYCOUNECT_FAIL;
	}
	delay(1000);
	states = SetConfiguration(DeviceAddress,CFGValue,0,0);

	//---- Get MaxLun ----
	if(gb_HostConnect == 0)
	{
    	return HOST_PHYCOUNECT_FAIL;
	}
	gb_stall_flag=0;
	delay(1000);
	GetMAXLUN();
	if(gb_stall_flag == 1)
	{
		MaxLUN=0;
		gb_stall_flag=0;
	}

	XBYTE[0xB01C]&=0xdf;	//Auto Ack in	
	delay(30000);

	SCSICMD_12_INQUIRY();
	delay(1000);
	states = SCSICMD_23_READFORMATCAPACITY();
	if(states!=0) 
	{
    	return HOST_PHYCOUNECT_FAIL;		
	}

	if(gb_HostConnect == 0)
	{
    	return HOST_PHYCOUNECT_FAIL;
	}

	Host_StallCmd_Timeout=0;
	while(1)
	{
		states = SCSICMD_25_READCAPACITY();
		if(gb_HostConnect == 0)
		{
	    	return HOST_PHYCOUNECT_FAIL;
		}

		if(Host_StallCmd_Timeout>60)
		{
			return 0x77;	// Initial fail 
		}

		if(states!=HOST_STALL)
		{
			break;
		}
	} 		

    gc_CurrentCard = CURRENT_MEDIA_HOST;
	gdw_pollingtime=3000;
	gdw_HOSTStartSectorRead=0xFFFFFFF0;
	states=HOST_Read_Logicsector(0);

	if(states==3)
	{	
		goto USB_HOST_Init;
	}
   	return 0;
}


U8 GetDescriptor(U8 addr,U16 wValue,U16 wIndex,U16 wLength)
{
    data	bit	toggle = 1;
	data	U8	tc_state;
	data	U16	offset=0;

	XBYTE[HOST_BUF_SA]		=0x80;
	XBYTE[HOST_BUF_SA+1]	=0x06;
	XBYTE[HOST_BUF_SA+2]	=(U8)(wValue&0xff);
	XBYTE[HOST_BUF_SA+3]	=(U8)((wValue>>8)&0xff);
	XBYTE[HOST_BUF_SA+4]	=(U8)(wIndex&0xff);
	XBYTE[HOST_BUF_SA+5]	=(U8)((wIndex>>8)&0xff);
	XBYTE[HOST_BUF_SA+6]	=(U8)(wLength&0xff);
	XBYTE[HOST_BUF_SA+7]	=(U8)((wLength>>8)&0xff);

    //trans Descript CMD    
	tran_setup(addr);
   	tc_state = Host_phy_Command_check();
	
	if((tc_state!=0)&&(gb_HostInitial!=1))
	{
		if(tc_state!=HOST_TEST_ERROR1)
		{
			return HOST_TRANSETUP_ERROR;
		}
	}

	gb_shortpack=0;
    if(addr == 0x00) 
	{		
		setup_in(addr, wLength, offset, toggle);
		tc_state = Host_phy_Command_check();

		if(tc_state!=0)
		{
			return HOST_SETUPIN_ERROR;
		}
		else
		{
			if(gb_HostConnect == 0)
			{
				return HOST_PHYCOUNECT_FAIL;
			}
			setup_out(addr, 0, offset, 1);

			tc_state = Host_phy_Command_check();
			if((tc_state==HOST_TEST_ERROR)||(tc_state==HOST_TimeOut_ERROR))		   
			{
			}
	  		else if(tc_state)
			{
	  			return HOST_SETUPOUT_ERROR;
			}
            else 
			{
				return 0;
         	}
        }
	}
    else
    {
		while (wLength>0)
      	{
        	if(gb_HostConnect == 0)
 		 	{
		 		return HOST_PHYCOUNECT_FAIL;
		 	}

         	if(wLength <  Max_Trans_Length) 
		 	{
		 		setup_in(addr, wLength, offset, toggle);
				tc_state = Host_phy_Command_check();
		 	}
         	else 
         	{
		 		setup_in(addr, Max_Trans_Length, offset, toggle);
				tc_state = Host_phy_Command_check();
		 	}

			if(gb_shortpack==1)
			{
				break;
			}
         	if(tc_state != 0)
         	{
            	if(tc_state == 0x20)
				{
            		break;
				}
            	if((tc_state > 0x20)&&(offset>0))
	        	{
					break;
				}

            	return HOST_SETUPIN_ERROR;
         	}

         	if((wValue>0x0300)&(offset == 0))
         	{
				if(wLength > XBYTE[HOST_BUF_SA])
   				{
					wLength = XBYTE[HOST_BUF_SA];
				}
         	}
         	toggle ^= 1;
         	offset  = offset  + Max_Trans_Length;
  
         	if(wLength >= Max_Trans_Length)
	     	{
		 		wLength = wLength - Max_Trans_Length;
     	 	}
         	else
         	{
		 		wLength = 0;
 		 	}
	   	}
	}

    setup_out(addr, 0, offset, 1);
	tc_state = Host_phy_Command_check();

    if(tc_state)
	{
		return HOST_SETUPOUT_ERROR;
	}
    else 
	{
		return 0;
	}
}
//======================================================================================================================
void GetMAXLUN(void)
{
	data	U8 tc_state;

	XBYTE[HOST_BUF_SA+0]=0xA1;
	XBYTE[HOST_BUF_SA+1]=0xFE;
	XBYTE[HOST_BUF_SA+2]=0x00;
	XBYTE[HOST_BUF_SA+3]=0x00;
	XBYTE[HOST_BUF_SA+4]=0x00;
	XBYTE[HOST_BUF_SA+5]=0x00;
	XBYTE[HOST_BUF_SA+6]=0x01;
	XBYTE[HOST_BUF_SA+7]=0x00;
    
	tran_setup(1);
	tc_state = Host_phy_Command_check();
	if(tc_state)
    {
		Syn_Reset();
       	return;
    }
	setup_in(1, 1, 0, 1);
	tc_state = Host_phy_Command_check();
	if(tc_state)
    {
		Syn_Reset();
		return;
    }

   	setup_out(1, 0, 0, 1);
	tc_state = Host_phy_Command_check();
	if(tc_state == 0)
    {
 	   MaxLUN=XBYTE[HOST_BUF_SA];
    }
	else
    {
		Syn_Reset();
		return;
    }    
}


U8 SetConfiguration(U8 addr, U16 wValue, U16 wIndex, U16 wLength)
{
	data	U16	offset;
	xdata	U8	tc_state;
	
	XBYTE[HOST_BUF_SA]		=0x00;
	XBYTE[HOST_BUF_SA+1]	=0x09;
	XBYTE[HOST_BUF_SA+2]	=(U8)(wValue&0xff);
	XBYTE[HOST_BUF_SA+3]	=(U8)((wValue>>8)&0xff);
	XBYTE[HOST_BUF_SA+4]	=(U8)(wIndex&0xff);
	XBYTE[HOST_BUF_SA+5]	=(U8)((wIndex>>8)&0xff);
	XBYTE[HOST_BUF_SA+6]	=(U8)(wLength&0xff);
	XBYTE[HOST_BUF_SA+7]	=(U8)((wLength>>8)&0xff);

	offset = 0;
	tran_setup(addr);
	tc_state = Host_phy_Command_check();

    if(tc_state)
	{
		return HOST_TRANSETUP_ERROR;
	}

	delay(1000);
	setup_in(addr, 0, offset, 1);
	tc_state = Host_phy_Command_check();

    if(tc_state)
	{
		return HOST_SETUPIN_ERROR;
	}
	else
	{
		return 0;
	}
}


U8 SetAddress(void)
{
	U8 tc_state;

	XBYTE[HOST_BUF_SA+0]=0x00;
	XBYTE[HOST_BUF_SA+1]=0x05;
	XBYTE[HOST_BUF_SA+2]=0x01;	// Address=1
	XBYTE[HOST_BUF_SA+3]=0x00;
	XBYTE[HOST_BUF_SA+4]=0x00;
	XBYTE[HOST_BUF_SA+5]=0x00;
	XBYTE[HOST_BUF_SA+6]=0x00;
	XBYTE[HOST_BUF_SA+7]=0x00;

	tran_setup(0);
	tc_state = Host_phy_Command_check();

	if(tc_state)
	{
		return HOST_TRANSETUP_ERROR;
	}

	delay(1000);

	setup_in(0, 0, 0, 1);
	tc_state = Host_phy_Command_check();

	if(tc_state)
	{
		return HOST_SETUPIN_ERROR;
	}
	else
	{
		return 0;
	}
}


U8 SCSICMD_25_READCAPACITY(void)
{
	code	U8	READ_CAPACITY[31]={0x55,0x53,0x42,0x43,0x55,0xAA,0xBB,0x05,0x08,0x00,0x00,0x00,0x80,0x00,0x0A,0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	data	U8	tc_state;
	xdata	U8	i;

	// Prepare CBW data
	for(i=0;i<31;i++)
	{
		XBYTE[HOST_BUF_SA+i]=READ_CAPACITY[i];
	}

	// OUT CBW
	tran_out(DeviceAddress, 31); 
	Host_phy_Command_check();
	delay(1000);	//for action mp3 player

	// IN data
	tran_in(DeviceAddress, 8 , 0);
	tc_state=Host_phy_Command_check();

	gc_HostSectorUnit=XBYTE[HOST_BUF_SA+6]/2;

	if(!tc_state)
	{
		((U8 *)(&gdw_CARD_TotalSizeMB))[0]=XBYTE[HOST_BUF_SA+0];
		((U8 *)(&gdw_CARD_TotalSizeMB))[1]=XBYTE[HOST_BUF_SA+1];
		((U8 *)(&gdw_CARD_TotalSizeMB))[2]=XBYTE[HOST_BUF_SA+2];
		((U8 *)(&gdw_CARD_TotalSizeMB))[3]=XBYTE[HOST_BUF_SA+3];
	}

	if(tc_state==HOST_STALL)
	{
		Host_StallCmd_Timeout++;
		gb_dtg_in=0;
		ClearFeature(DeviceAddress,0,EP_IN,0);
		tran_in(DeviceAddress, 13, 0);
		tc_state = Host_phy_Command_check();

		if(tc_state!=0) // 20080620
		{
			return tc_state;
		}	  		    
		SCSICMD_03_REQUESTSENSE();
		delay(10000);
		return HOST_STALL;
	}

	if(tc_state!=0)
	{
		gb_dtg_in=0;
		Syn_Reset();
		ClearFeature(DeviceAddress,0,EP_IN,0);
	}
	// IN CSW
	delay(1000);
	tran_in(DeviceAddress, 13, 0);
	Host_phy_Command_check();
		
	if(XBYTE[HOST_BUF_SA+12]!=0)
	{
		SCSICMD_03_REQUESTSENSE();
		return 1;
	}
	else
	{
		return 0;
	}
}


U8 SCSICMD_23_READFORMATCAPACITY(void)
{
	xdata	U8	i;
	data	U8	j;
	data	U8	tc_state;
	code	U8	READ_FORMAT_CAPACITY[31]={0x55,0x53,0x42,0x43,0x55,0xAA,0xBB,0x05,0xFC,0x00,0x00,0x00,0x80,0x00,0x0A,0x23,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	while(1)
	{
		// Prepare CBW data
	    for(i=0;i<31;i++) 		
		{
	    	XBYTE[HOST_BUF_SA+i]=READ_FORMAT_CAPACITY[i];
		}	    	
	
		// OUT CBW
		tran_out(DeviceAddress, 31); 
		Host_phy_Command_check();
		delay(1000);

		// IN data(The data is useless)
		if(gb_HostConnect == 0)
		{ 
			return HOST_PHYCOUNECT_FAIL;
		}	
		for(j=0;j<4;j++)
		{
			gb_shortpack=0;
			tran_in(DeviceAddress, 64, 0);
			tc_state = Host_phy_Command_check();		   
			if((gb_shortpack==1)||(tc_state==HOST_STALL)) // 20080620
			{
				break;
			}
		}

		if(tc_state==HOST_STALL)
		{			
			gb_dtg_in=0;
			ClearFeature(DeviceAddress,0,EP_IN,0);
			tran_in(DeviceAddress, 13, 0);
			tc_state = Host_phy_Command_check();
	
			if(tc_state!=0) // 20080620
			{
				return tc_state;
			}	  	
	    	delay(10000);
			SCSICMD_03_REQUESTSENSE();
			delay(10000);
		}
		else
		{
			// 20080623
			tran_in_(DeviceAddress, 13, 0);
			tc_state=Host_phy_Command_check();
			if(tc_state==HOST_STALL)
			{
				gb_dtg_in=0;
				ClearFeature(DeviceAddress,0,EP_IN,0);
				tran_in(DeviceAddress, 13, 0);
				Host_phy_Command_check();
			}
			return 0;
		}

		if(gb_Repeat==0)
		{
			return 0;
		}
	}
}


U8 SCSICMD_12_INQUIRY(void)
{
	xdata	U8	i;
	xdata	U8	tc_state;
	xdata	U16	Offset;
	code	U8	INQUIRY[31]={0x55,0x53,0x42,0x43,0xf8,0x98,0x56,0x85,0x24,0x00,0x00,0x00,0x80,0x00,0x06,0x12,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	// Prepare CBW data
    for(i=0;i<31;i++)
  	{
		XBYTE[HOST_BUF_SA+i]=INQUIRY[i];
	}

	// OUT CBW
	if(gb_HostConnect == 0)
	{
		return HOST_PHYCOUNECT_FAIL;
	}
	Offset = 0;
	tran_out(DeviceAddress, 31);
    Host_phy_Command_check();

	delay(1000);	//for action mp3 player
	
	// IN data   	
   	if(gb_HostConnect == 0) 
   	{
	  	return HOST_PHYCOUNECT_FAIL;
   	}

	for(i=0;i<250;i++)
	{
   		tran_in(DeviceAddress, 36, Offset);
		tc_state=Host_phy_Command_check();		
		if((tc_state==0)&&(gb_shortpack==0))
		{
			break;
		}
		gb_dtg_in = (!gb_dtg_in);
	}

	// IN CSW
   	if(gb_HostConnect == 0)
	{ 
		return HOST_PHYCOUNECT_FAIL;
	}

	for(i=0;i<20;i++)
	{
   		tran_in(DeviceAddress, 13, Offset);
		tc_state=Host_phy_Command_check();		
		if((tc_state==0)&&(gb_shortpack==0))
		{
			break;
		}
		gb_dtg_in = (!gb_dtg_in);
	}
	return 0;
}



