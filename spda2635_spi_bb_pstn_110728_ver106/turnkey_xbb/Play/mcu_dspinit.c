#include "..\header\SPDA2K.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\header\variables.h"


U8 MP3_Download(void)
{
	data	U8 tc_Cnt;

	DSP_WakeUp();
	if(DSP_Download(0x00,0x10)!=DSP_SUCCESS)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	for(tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if(DSP_PlayInit()==DSP_SUCCESS)
		{
			return 0;
		}

		if(tc_Cnt>=5) 
		{
			XBYTE[0xB002]|=0x01;		// DSP reset
		}
		XBYTE[0xB002]&=0xFE;		// Enable DSP Run Normal Mode
	}

	return DSP_PLAY_COMMAND_ERROR;
}


U8 WMA_Download(void)
{
	data	U8 tc_Cnt;
	data	U8 tc_result;

	tc_result=0;
	DSP_WakeUp();
	if(DSP_Download(0x12,0x25)!=DSP_SUCCESS)
	{
		return 0x90;
	}

	L2_DSP_Write_DMem16(DSP_W_asf_parse_ok,0);
	L2_DSP_Write_DMem16(DSP_W_asf_parse_mode,1);		//Set ASF Parser Mode

	for (tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if (DSP_PlayInit()==DSP_SUCCESS)
		{
			break;
		}

		if(tc_Cnt>=5) 
		{
			XBYTE[0xB002]|=0x01;		// DSP reset
		}
		XBYTE[0xB002]&=0xFE;		// Enable DSP Run Normal Mode
	}
	if (tc_Cnt == 10)
	{
		return 0x91;
	}

/*------------------------------------------------------------------*/
/* WMA download IMPM step:                                          */
/* 1. ASF parser mode: to check what entropy mode this bitstream is */
/* 2. check DRM: option                                             */
/* 3. normal wma download: to download the correct IMPM             */
/*------------------------------------------------------------------*/

//-------------------
//1. ASF parser mode
//-------------------

	if(L2_DSP_SendCommandSet(DCMD_Play) != DCMD_Play)
	{
		return 0x92;
	}

	gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;	// reset bitstream address of DM 

	if (DOS_Open_File_r(0x00,0x02,0x00))
	{
		return 0x93;
	}

	while(tc_result == 0)
	{
		if(WMA_DataIn() == DSP_DATAIN_COMMAND_ERROR) 	
		{
			return 0x94;
		}

		tc_result = L2_DSP_Read_DMem16(DSP_W_asf_parse_ok);
	}

	if(tc_result != 1)
	{
		return 0x95;
	}

/*-- DSP will jump to pc=0 automatically after asf parse has been done--*/

//----------------------
//2. Normal wma download
//----------------------
	switch(L2_DSP_Read_DMem16(DSP_w_EntropyMode))
	{
		case 1:
			tc_result=DSP_Download(0x01,0x23);
		break;

		case 2:
			tc_result=DSP_Download(0x01,0x24);
		break;

		case 3:
			tc_result=DSP_Download(0x01,0x25);
		break;
	}
		
	if(tc_result != DSP_SUCCESS )	
	{
		return 0x96;
	}

	for(tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if(DSP_PlayInit()==DSP_SUCCESS)
		{
			break;
		}

		if(tc_Cnt>=5) 
		{
			XBYTE[0xB002]|=0x01;		// DSP reset
		}
		XBYTE[0xB002]&=0xFE;		// Enable DSP Run Normal Mode
	}

	if (tc_Cnt == 10)
	{
		return 0x97;	
	}
	L2_DSP_Write_DMem16(DSP_W_asf_parse_mode,0);		//Clear ASF Parser Mode
	return DSP_SUCCESS;
}

U8 WAV_Play_Download(void)
{
	data	U8 tc_Cnt;

	DSP_WakeUp();
 	if(DSP_Download(0x02,0x12)!=0)
	{   
		return DSP_PLAY_COMMAND_ERROR;
	}

	for(tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if(DSP_PlayInit()==DSP_SUCCESS)
		{
			break;
		}

		if(tc_Cnt>=5) 
		{
			XBYTE[0xB002]|=0x01;		// DSP reset
		}
		XBYTE[0xB002]&=0xFE;		// Enable DSP Run Normal Mode
	}
	if(tc_Cnt==10)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	return 0;
}

U8 IMA_ADPCM_Rec_Download(void)
{
	data	U8	tc_Cnt;

	DSP_WakeUp();
 	if(DSP_Download(0x09,0x19)!=0)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	for(tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if(DSP_PlayInit()==0)
		{
			break;
		}
		if(tc_Cnt>=5) 
		{
			XBYTE[0xB002]|=0x01;		// DSP reset
		}
		XBYTE[0xB002]&=0xFE;		// Enable DSP Run Normal Mode
	}
//	USER_Delay(15); // delay 10 ms
	if(tc_Cnt==10)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
	return 0;
}

U8 IMA_ADPCM_Rec_PlayCmd(U8 tc_SampleRate,U8 tc_ChannelNum)
{
	data	U16 tw_CtrlMode;

	//setup Channel Number
	L2_DSP_Write_DMem16(0x3CAB,tc_ChannelNum);
	tw_CtrlMode=L2_DSP_Read_DMem16(DSP_ADPCMModeControl);
	// setup Sampling Rate
	if(tc_SampleRate==16)
	{
		tw_CtrlMode=tw_CtrlMode|0x0010;
	}
	else if(tc_SampleRate==32)
	{
		tw_CtrlMode=tw_CtrlMode|0x0020;
	}
	else	// always set 8K sampling rate
	{
		tw_CtrlMode=tw_CtrlMode&0xFFEF;
	}
	L2_DSP_Write_DMem16(DSP_ADPCMModeControl,tw_CtrlMode);

	if(L2_DSP_SendCommandSet(DCMD_Play)!=DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
	gs_DSP_GLOBAL_RAM.sc_DM_Index=0;
	return 0;
}


