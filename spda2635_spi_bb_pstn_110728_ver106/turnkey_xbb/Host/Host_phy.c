#include "..\header\SPDA2K.h"
#include "..\header\host_init.h"
#include "..\header\variables.h"


void tran_in_(U8 addr,U8 amount,U16 offset)
{
	XBYTE[0xB602]=addr;
	XBYTE[0xB603]=gc_Host_EPIN;
	XBYTE[0xB604]=0;
	XBYTE[0xB605]=((offset>>8)&0xFF)+((HOST_BUF_SA>>8)&0xFF);
	XBYTE[0xB606]=amount;
	XBYTE[0xB607]=0;
	if(gb_dtg_in==0)
	{
		XBYTE[0xB601]=0x91;
	}
	else
	{
        XBYTE[0xB601]=0x93;
	}
}

void tran_in(U8 addr,U8 amount,U16 offset)
{
	tran_in_(addr,amount,offset);
	gb_dtg_in=(!gb_dtg_in);
}


void tran_out(U8 addr,U8 amount)
{
	XBYTE[0xB602]=addr;
	XBYTE[0xB603]=gc_Host_EPOUT;
	XBYTE[0xB604]=0;
	XBYTE[0xB605]=0;
	XBYTE[0xB606]=amount;
	XBYTE[0xB607]=0;
	if(gb_dtg_out==0)	
	{
		XBYTE[0xB601]=0x11;
	}
	else
	{
		XBYTE[0xB601]=0x13;
	}
	gb_dtg_out=(!gb_dtg_out);
}

void tran_setup(U8 addr)
{
	XBYTE[0xB602]=addr;
	XBYTE[0xB603]=0x00;
	XBYTE[0xB604]=0x00;
	XBYTE[0xB605]=0x00;
	XBYTE[0xB606]=0x08;
	XBYTE[0xB607]=0x00;
	XBYTE[0xB601]=0xD1;	//go
}


void setup_in(U8 addr,U8 amount,U8 offset,U8 setup_dtg)	
{
	XBYTE[0xB602]=addr;
	XBYTE[0xB603]=0x00;
	XBYTE[0xB604]=offset&0xFF;
	XBYTE[0xB605]=0x00;
	XBYTE[0xB606]=amount&0xFF;
	XBYTE[0xB607]=0x00;

	if(setup_dtg==0)
	{
		XBYTE[0xB601]=0x91;
	}
	else
	{
 		XBYTE[0xB601]=0x93;
	}
}


void setup_out (U8 addr,U8 endpoint,U16 amount,U16 offset,U8 setup_dtg)
{
	XBYTE[0xB602]=addr;
	XBYTE[0xB603]=endpoint;
	XBYTE[0xB604]=offset&0xFF;
	XBYTE[0xB605]=((offset>>8)&0xFF)+((HOST_BUF_SA>>8)&0xFF);
	XBYTE[0xB606]=amount&0xFF;
	XBYTE[0xB607]=(amount>>8)&0xFF;

	if(!setup_dtg)
	{
 		XBYTE[0xB601]=0x11;
	}
	else
	{
 		XBYTE[0xB601]=0x13;
	}
}


U8 Host_phy_Command_check(void)
{
	xdata	U8	tc_b611_data;
	data	U32 i=0;

	gb_HostShortPack=0;
	gb_HostStallFlag=0;
	while(XBYTE[0xB601]&0x01)
	{//during transaction
		if(gb_HostConnect==0) 
		{
			return HOST_PHYCOUNECT_FAIL;
		}
		if (i>gdw_pollingtime) 
		{
			return HOST_TEST_ERROR;	//TONY
		}
		else if(XBYTE[0xB611]&0x01)
		{
			return	HOST_TimeOut_ERROR;
		}
		else
		{
			i++;
		}
	}

	tc_b611_data=XBYTE[0xB611];
	
	if(tc_b611_data&0x01)
	{
		return	HOST_TEST_ERROR1;
	}

	if(tc_b611_data==0)
	{//no any error
		return 0;
	}
	else
	{//different kinds of error
		if(tc_b611_data&0x10)
		{
			gb_HostStallFlag=1;
			return	HOST_STALL;
		}
		else if(tc_b611_data&0x20)
		{
			gb_HostShortPack=1;
			return 0;
		}
		else
		{
			return HOST_PHYCOUNECT_FAIL;
		}		
	}
}


U8 Host_phy_Command_check_(U8 offset,U16 toggle)
{
	xdata	U8	tc_b611_data;
	data	U16	time_out=0;
	data	U32	i=0;

	//***************************************************************************
	//important! if open SOF, it will cause TG_Match(B01C's bit6) be cleared to 0
	//so when Ack-in, it must close sof and re-open after receiving data
	//it can also adjust data if is received by 0xb608=0
	//***************************************************************************

	while(1)
	{
		while(XBYTE[0xB601] & 0x01)
		{
			if(gb_HostConnect == 0) 
			{
				return HOST_PHYCOUNECT_FAIL;
			}

			if (i>gdw_pollingtime) 
			{
				i = 0;
				return HOST_TEST_ERROR;	//TONY
			}
			else
			{
		 		 i++;
			}
		}
		if((XBYTE[0xB611] & 0x10) || (XBYTE[0xB608]==0x00))
		{
			break;
		}
		if(time_out==0x5000)
		{
			break;
		}

//*****************************************************************
		XBYTE[0xB604] = offset & 0xFF;
		XBYTE[0xB605] = ((offset>>8) & 0xFF) + ((HOST_BUF_SA>>8)&0xFF);	
//*************************************************F****************

		time_out++;
		if(toggle==0) 
		{
			XBYTE[0xB601]=0x91;
		}
		else
		{
			XBYTE[0xB601]=0x93;
		}
	}

	tc_b611_data = XBYTE[0xB611];
	if(tc_b611_data == 0)
	{
		return 0;
	}
	else
	{
		if(tc_b611_data&0x10)
		{
			gb_HostStallFlag=1;
			return	HOST_STALL;
		}
		else if(tc_b611_data & 0x20)
		{
			gb_HostShortPack=1;
			return 0;
		}
		else
		{
			return HOST_PHYCOUNECT_FAIL;
		}
	}
}
