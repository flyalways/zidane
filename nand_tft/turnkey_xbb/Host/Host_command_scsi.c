#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\header\host_init.h"

#define	HostDMAtoHOSTBUFFER	1		// 1K/2K sector一次讀取
#define	HOSTBUFFER			0x80

data	bit	gb_HostError;
data	bit	gb_Repeat=0;

xdata	U8	MaxLUN;
xdata	U8	CBWLUN;
xdata	U8	Host_StallCmd_Timeout;
xdata	U32	gdw_HOSTStartSectorRead;

extern	data	U16	Offset;
extern	data	bit	gb_shortpack;

extern	xdata	U16	gw_VID;

extern	data	U8	gc_CurrentCard;
extern	xdata	U8	gc_HostSectorUnit;
extern	xdata	U8	gc_HostUnitNum;

extern	void	tran_in_(U8 addr,U8 amount,U16 offset);
extern	U8		Host_phy_Command_check_(U8 offset,int toggle);


U8 HOST_Read_Logicsector(U32 tdw_LogicSectorAddr)
{
    U8 j;
   	U8 *buffer;

	if(gb_ReadWriteDataArea == 0)	
	{
		buffer = gc_PlayRecordDataBuf;
	} 
	else
	{
		buffer = gc_UserDataBuf;
	}

#if HostDMAtoHOSTBUFFER==1
	if((tdw_LogicSectorAddr>=gdw_HOSTStartSectorRead)&&(tdw_LogicSectorAddr<=(gdw_HOSTStartSectorRead+gc_HostSectorUnit-1)))
	{
		data	U8	x;
		x=tdw_LogicSectorAddr-gdw_HOSTStartSectorRead;
		XBYTE[0xB301]=0x00;        //DMA src: MCU Data SRAM;  dst: DSP DM
		XBYTE[0xB302]=0xFF;
		XBYTE[0xB303]=0x01;
		XBYTE[0xB112]=0x00;
		XBYTE[0xB113]=HOSTBUFFER+2*x;
		XBYTE[0xB114]=(U16)buffer;
		XBYTE[0xB115]=(U16)buffer>>8;
		XBYTE[0xB3B0]=0x01;
		while(!(XBYTE[0xB3C0]&0x01));
		XBYTE[0xB3C0]=0x00;
		return 0;
	}
	else
	{
		gdw_HOSTStartSectorRead=tdw_LogicSectorAddr;
	}
#endif
	gc_HostUnitNum=tdw_LogicSectorAddr%gc_HostSectorUnit;
	tdw_LogicSectorAddr=tdw_LogicSectorAddr/gc_HostSectorUnit;

	gb_HostError=0;
	j=HOST_Readsector(tdw_LogicSectorAddr,buffer);	//read one sector	
	if(j!=0)		
	{
		if(j==HOST_SCSICMD_MEDIACHANGED)
		{
			return HOST_SCSICMD_MEDIACHANGED;
		}
        else if(j==3)
		{
			gb_HostError=1;
			return 3;
		}
	}

	return 0;
}


U8 HOST_Readsector(U32 SectorAddr,U8 *buffer)
{
	code	U8	CBW[31]={0x55,0x53,0x42,0x43,0x01,0x02,0x66,0x01,0x00,0x02,0x00,0x00,0x80,0x00,0x0A,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	data	U8	i,j;
	data	U8	CBWTag;
	data	U8	tc_state;
	data	U8	tc_RetryCount;
	xdata	U8	k;
	xdata	U16	Offset;

 	if(gb_HostConnect == 0)
	{ 
		return HOST_PHYCOUNECT_FAIL;
	}

	// Prepare CBW data
    for(i=0;i<31;i++) 		
	{
		XBYTE[HOST_BUF_SA+i]=CBW[i];
	}
    XBYTE[HOST_BUF_SA+7]=CBWTag++;
	XBYTE[HOST_BUF_SA+9]=gc_HostSectorUnit*2;
    XBYTE[HOST_BUF_SA+17]=(U8)(SectorAddr>>24);
    XBYTE[HOST_BUF_SA+18]=(U8)(SectorAddr>>16);
    XBYTE[HOST_BUF_SA+19]=(U8)(SectorAddr>>8);
    XBYTE[HOST_BUF_SA+20]=(U8)(SectorAddr>>0);

	// OUT CBW
    if(gb_HostConnect == 0) 
	{
		return HOST_PHYCOUNECT_FAIL;
	}

	tc_RetryCount=0;
	while(1)
	{
		tc_RetryCount++;
		if(tc_RetryCount>5)
		{
			return 1;
		}

		tran_out(DeviceAddress, 31);
		tc_state = Host_phy_Command_check();
		if(gb_HostConnect == 0) 
		{
			return HOST_PHYCOUNECT_FAIL;
		}
		if((tc_state==HOST_TEST_ERROR)||(tc_state==HOST_TimeOut_ERROR)||(tc_state==HOST_TEST_ERROR1))
		{
			gb_dtg_out = (!gb_dtg_out);		
		}
		else if(tc_state!=0) 
		{
			gb_HostConnect = 0; 
			return HOST_SCSICMD_MEDIUMNOTPRESENT;
		}
		else
		{
			break;
		}
	}

	Offset = 0;
	// maxliao 20070116 - For My Disk(U-disk) and Action MP3 player can't access 
	if(gw_VID==0x10D6)
	{
		XBYTE[0xB01C]|=0x20;	//Non Auto Ack in for action mp3 player
	}
	delay(1000);
	while(1)
	{
		tran_in_(DeviceAddress, 64, Offset);
		tc_state=Host_phy_Command_check_(Offset,gb_dtg_in);
		if(tc_state==0)
		{
			break;
		}
		if(gb_HostConnect == 0) 
		{
			return HOST_PHYCOUNECT_FAIL;
		}
		if(tc_state==HOST_STALL)
		{
			gb_dtg_in=0;
			ClearFeature(DeviceAddress,0,EP_IN,0);
			tran_in(DeviceAddress, 13, Offset);
			tc_state = Host_phy_Command_check();
			if(tc_state!=0) // 20080620
			{
				return tc_state;
			}	  		    
			SCSICMD_03_REQUESTSENSE();
			return 1;
		}

		k=0;
		if(tc_state==HOST_TEST_ERROR)
		{
			k++;
			if(k>12)
			{
				return 3;
			}
		}
	}

	// maxliao 20070116 - For My Disk(U-disk) and Action MP3 player can't access 
	XBYTE[0xB01C]&=0xdf; //Auto Ack in

	gb_dtg_in=!gb_dtg_in;
	Offset = 64;
		
	if(tc_state==0)
	{
		for(j=0;j<gc_HostSectorUnit;j++)
		{
			if(j==0)
			{
				i=1;
			}
			else
			{
				i=0;
				Offset=0;
			}

			for(;i<8;i++)
			{
				if(gb_HostConnect == 0) 
				{
					return HOST_PHYCOUNECT_FAIL;
				}

				XBYTE[0xB604] = Offset & 0xFF;
				XBYTE[0xB605] = (Offset>>8) + (HOST_BUF_SA>>8);
	
				tc_RetryCount=0;
				while(1)
				{	
					tc_RetryCount++;
					if(tc_RetryCount>5)
					{
						return 1;
					}
	
					if(gb_dtg_in == 0) 
					{
						XBYTE[0xB601]=0x91;
					}
					else
					{
						XBYTE[0xB601]=0x93;
					}

					tc_state = Host_phy_Command_check();
					if(tc_state==0)
					{
						break;
					}
					else
					{
						return 1;
					}
				}
				gb_dtg_in = (!gb_dtg_in);
				Offset = Offset + 64;	
			}

			XBYTE[0xB3C0]=0x00;
			XBYTE[0xB301]=0x00;        //DMA src: MCU Data SRAM;  dst: DSP DM
			XBYTE[0xB302]=0xFF;
			XBYTE[0xB303]=0x01;
			XBYTE[0xB112]=0x00;
			XBYTE[0xB113]=HOST_BUF_SA>>8;
			XBYTE[0xB114]=0x00;

			// 將指定Sector之512-byte資料DMA至指定buffer
			if(j==gc_HostUnitNum)
			{
				XBYTE[0xB115]=((U16)buffer)>>8;
				XBYTE[0xB3B0]=0x01;
				while(!(XBYTE[0xB3C0]&0x01));
				XBYTE[0xB3C0]=0x00;
			}

			#if HostDMAtoHOSTBUFFER==1
			// 備份USB資料至Host buffer, 若下一次是同一Sector則直接DMA至destination buffer
			XBYTE[0xB115]=HOSTBUFFER+2*j;
			XBYTE[0xB3B0]=0x01;
			while(!(XBYTE[0xB3C0]&0x01));
			XBYTE[0xB3C0]=0x00;
			#endif
		}

		// maxliao 20061205 - Enable SOF
		XBYTE[0xB630]=1;
	}
	else if(tc_state & 0x10)
	{//stall
		XBYTE[0xB630]=1;				//enable SOF
		gb_dtg_in = 0;		
		ClearFeature(DeviceAddress,0,EP_IN,0);

		tran_in_(DeviceAddress, 13, 0);		//stored at HOST_BUF_SA	
	    tc_state = Host_phy_Command_check();

		if ( XBYTE[HOST_BUF_SA+12]==0x01 )
		{
			SCSICMD_03_REQUESTSENSE();
			return 1;
		}
	}
	
	// IN CSW
	if(gb_HostConnect == 0) 
	{
		return HOST_PHYCOUNECT_FAIL;
	}
	
	for(i=0;i<20;i++)
	{
		tran_in(DeviceAddress, 13, 0);
		tc_state = Host_phy_Command_check();
		if((tc_state==0)&&(gb_shortpack==0))
		{
			break;
		}
		gb_dtg_in=!gb_dtg_in;
	}

	if(tc_state!=0)
	{
		return 0xff;	// fail
	}

   	return 0;
}


void delay(U16  i)
{
	while(i)   i--;
}


void Syn_Reset(void)
{
	XBYTE[0xB601]=0x04;	//syn reset
	delay(1000);
	XBYTE[0xB601]=0x04;
	delay(8000);
}


U8 ClearFeature(U8 addr,U16 wValue,U16 wIndex,U16 wLength)
{
	U8 tc_state;

	XBYTE[HOST_BUF_SA+0]=0x02;
	XBYTE[HOST_BUF_SA+1]=0x01;
	XBYTE[HOST_BUF_SA+2]=(U8)(wValue&0xff);
	XBYTE[HOST_BUF_SA+3]=(U8)((wValue>>8)&0xff);
	XBYTE[HOST_BUF_SA+4]=(U8)(wIndex&0xff) | 0x80;
	XBYTE[HOST_BUF_SA+5]=(U8)((wIndex>>8)&0xff);
	XBYTE[HOST_BUF_SA+6]=(U8)(wLength&0xff);
	XBYTE[HOST_BUF_SA+7]=(U8)((wLength>>8)&0xff);

	tran_setup(addr);
	tc_state = Host_phy_Command_check();

	if(tc_state)
	{
		return HOST_TRANSETUP_ERROR;
	}
	delay(1000);
	setup_in(addr, 0, 0, 1);
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


U8 SCSICMD_03_REQUESTSENSE(void)
{
	data	U8	i;
	data	U8	tc_state;
	code	U8	CBW_REQUEST_SENSE[31]={0x55,0x53,0x42,0x43,0x55,0xAA,0x99,0x04,0x12,0x00,0x00,0x00,0x80,0x00,0x0C,0x03,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	gb_Repeat=0;

	// Prepare CBW data
	for(i=0;i<31;i++) 		
	{
		XBYTE[HOST_BUF_SA+i]=CBW_REQUEST_SENSE[i];
	}	
	
	// OUT CBW
	tran_out(DeviceAddress, 31);//send command 03
	Host_phy_Command_check();

	// IN data
    if(gb_HostConnect == 0)
    {
		return HOST_PHYCOUNECT_FAIL;
	}

	for(i=0;i<20;i++)
	{
		tran_in(DeviceAddress, 18, 0);
		tc_state = Host_phy_Command_check();		
		if((tc_state==0)&&(gb_shortpack==0))
		{
			break;
		}
		gb_dtg_in=!gb_dtg_in;
	}

	if((XBYTE[HOST_BUF_SA+0x02]==0x06)&&(XBYTE[HOST_BUF_SA+0x0C]==0x3A))
	{
		gb_Repeat=1;
	}
	// IN CSW
	if(gb_HostConnect == 0)
    {
		return HOST_PHYCOUNECT_FAIL;
	}
	for(i=0;i<20;i++)
	{
		tran_in(DeviceAddress, 13, 0);
		tc_state=Host_phy_Command_check();
		if((tc_state==0)&&(gb_shortpack==0))
		{
			break;
		}
		gb_dtg_in=!gb_dtg_in;
	}
	return 0;
}



		 

