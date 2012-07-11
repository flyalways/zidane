#include "..\header\SPDA2K.h"
#include "..\header\host_init.h"
#include "..\header\variables.h"

#define	HostDMAtoHOSTBUFFER	1		// 1K/2K sector一次讀取
#define	HOSTBUFFER			0x90


U8 HOST_Read_Logicsector(U32 tdw_LogicSectorAddr)
{
	data	U8	x;
	xdata	U8	j;
	xdata	U8	*buffer;

	if(gb_ReadWriteDataArea==0)	
	{
		buffer=gc_PlayRecordDataBuf;
	} 
	else
	{
		buffer=gc_UserDataBuf;
	}

#if HostDMAtoHOSTBUFFER==1
	if((tdw_LogicSectorAddr>=gdw_HOSTStartSectorRead)&&(tdw_LogicSectorAddr<=(gdw_HOSTStartSectorRead+gc_HostSectorUnit-1)))
	{
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
	j = HOST_Readsector(tdw_LogicSectorAddr,buffer);	//read one sector	
	if(j != 0)		
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
	data	U8	i;
	data	U8	j;
	data	U8	tc_state;
	data	U8	tc_RetryCount;
	xdata	U16	tw_Offset;
	xdata	U16	k;

	// Prepare CBW data
	for(i=0;i<31;i++) 		
	{
		XBYTE[HOST_BUF_SA+i]=CBW[i];
	}
	XBYTE[HOST_BUF_SA+7]=gc_CBWTag++;
	XBYTE[HOST_BUF_SA+9]=gc_HostSectorUnit*2;
	XBYTE[HOST_BUF_SA+17]=(U8)(SectorAddr>>24);
	XBYTE[HOST_BUF_SA+18]=(U8)(SectorAddr>>16);
	XBYTE[HOST_BUF_SA+19]=(U8)(SectorAddr>>8);
	XBYTE[HOST_BUF_SA+20]=(U8)(SectorAddr>>0);

	// OUT CBW
	tc_RetryCount=0;
	while(1)
	{
		tc_RetryCount++;
		if(tc_RetryCount>5)
		{
			return 1;
		}

		tran_out(gc_HostDeviceAddress,31);
		tc_state=Host_phy_Command_check();
		if(gb_HostConnect == 0) 
		{
			return HOST_PHYCOUNECT_FAIL;
		}
		if((tc_state==HOST_TEST_ERROR)||(tc_state==HOST_TimeOut_ERROR)||(tc_state==HOST_TEST_ERROR1))
		{
			gb_dtg_out=(!gb_dtg_out);		
		}
		else if(tc_state!=0) 
		{
			gb_HostConnect=0; 
			return HOST_SCSICMD_MEDIUMNOTPRESENT;
		}
		else
		{
			break;
		}
	}

	tw_Offset=0;
	// maxliao 20070116 - For My Disk(U-disk) and Action MP3 player can't access 
	if(gw_HostVID==0x10D6)
	{
		XBYTE[0xB01C]|=0x20;	//Non Auto Ack in for action mp3 player
	}
	delay(1000);
	while(1)
	{
		tran_in_(gc_HostDeviceAddress,64,tw_Offset);
		tc_state=Host_phy_Command_check_(tw_Offset,gb_dtg_in);
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
			ClearFeature(gc_HostDeviceAddress,0,gc_Host_EPIN,0);
			tran_in(gc_HostDeviceAddress,13,tw_Offset);
			tc_state=Host_phy_Command_check();
	
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
	tw_Offset=64;
		
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
				tw_Offset=0;
			}

			for(;i<8;i++)
			{
				if(gb_HostConnect == 0) 
				{
					return HOST_PHYCOUNECT_FAIL;
				}

				XBYTE[0xB604]=tw_Offset&0xFF;
				XBYTE[0xB605]=(tw_Offset>>8)+(HOST_BUF_SA>>8);

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
					tc_state=Host_phy_Command_check();
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
				tw_Offset+=64;	
			}

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
			// 備份USB資料至0xA000~0xA7FF, 若下一次是同一Sector則直接DMA至destination buffer
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
		ClearFeature(gc_HostDeviceAddress,0,gc_Host_EPIN,0);

		tran_in_(gc_HostDeviceAddress,13,0);		//stored at HOST_BUF_SA	
	    tc_state=Host_phy_Command_check();

		if(XBYTE[HOST_BUF_SA+12]==0x01)
		{
			tc_state=SCSICMD_03_REQUESTSENSE();
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
		tran_in(gc_HostDeviceAddress,13,0);
		tc_state=Host_phy_Command_check();
		if((tc_state==0)&&(gb_HostShortPack==0))
		{
			break;
		}
		gb_dtg_in=(!gb_dtg_in);
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

void Host_Write_LBA_Dos(U32 Write_LBA,U8 Write_Stage)
{
	data	bit	tb_Host_CBW_Stage=0;
	data	bit	tb_Host_Data_Stage=0;
	data	bit	tb_Host_CSW_Stage=0;
	xdata	U8	i;
	xdata	U8	j;
	xdata	U8	tc_state;
	xdata	U16	tw_addr;
	code	U8	CBW[31]={0x55,0x53,0x42,0x43,0x01,0x02,0x66,0x01,0x00,0x02,0x00,0x00,0x00,0x00,0x0A,0x2A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	if(gb_ReadWriteDataArea==0)	
	{
		tw_addr=gc_PlayRecordDataBuf;
	} 
	else
	{
		tw_addr=gc_UserDataBuf;
	}

	switch(Write_Stage)
	{
		case 0: // CBW+Data
			tb_Host_CBW_Stage=1;
			tb_Host_Data_Stage=1;
			break;
		case 1: // only data
			tb_Host_Data_Stage=1;
			break;
		case 2: //data+CSW
			tb_Host_Data_Stage=1;
			tb_Host_CSW_Stage=1;	
			break;
		case 3:// CBW+512-byte data+CSW
			tb_Host_CBW_Stage=1;
			tb_Host_Data_Stage=1;
			tb_Host_CSW_Stage=1;
			break;
		default:  // only CSW
			tb_Host_CSW_Stage=1;
			break;
	}

	if(tb_Host_CBW_Stage==1)
	{
		// Prepare CBW data
		for(i=0;i<31;i++) 		
		{
			XBYTE[HOST_BUF_SA+i]=CBW[i];
		}
		XBYTE[HOST_BUF_SA+7]=gc_CBWTag++;
		XBYTE[HOST_BUF_SA+9]=gc_HostSectorUnit*2;
		if(Write_Stage==0)
		{
			XBYTE[HOST_BUF_SA+9]*=gc_DOS_SectorPerCluster;
			XBYTE[HOST_BUF_SA+23]=gc_DOS_SectorPerCluster/gc_HostSectorUnit;
		}
		XBYTE[HOST_BUF_SA+17]=(U8)(Write_LBA>>24);
		XBYTE[HOST_BUF_SA+18]=(U8)(Write_LBA>>16);
		XBYTE[HOST_BUF_SA+19]=(U8)(Write_LBA>>8);
		XBYTE[HOST_BUF_SA+20]=(U8)(Write_LBA>>0);
		tran_out(gc_HostDeviceAddress,31);
		tc_state=Host_phy_Command_check();
	}

	if(tb_Host_Data_Stage==1)
	{
		for(i=0;i<8;i++)
		{
			XBYTE[0xB301]=0x00;
			XBYTE[0xB112]=(U8)tw_addr;
			XBYTE[0xB113]=(U8)(tw_addr>>8);
			XBYTE[0xB114]=0x00;
			XBYTE[0xB115]=HOST_BUF_SA>>8;
			XBYTE[0xB3B0]=0x01;
			while(!(XBYTE[0xB3C0]&0x01));
			XBYTE[0xB3C0]=0x00;
			tran_out(gc_HostDeviceAddress,64);
			j=0;
			while(1)
			{
				tc_state=Host_phy_Command_check();
				if(tc_state==0) 
				{
					break;
				}
				j++;
				if(j>50)
				{
					gc_CurrentCard=9;
					return;
				}
			}
			tw_addr+=64;
		}
	}

	if(tb_Host_CSW_Stage==1)
	{
		tran_in(gc_HostDeviceAddress,13,0);
		while(1)
		{
			tc_state=Host_phy_Command_check();
			if(tc_state==0||gb_HostConnect==0)
			{
				break;
			}
		}
	}
}

		 
