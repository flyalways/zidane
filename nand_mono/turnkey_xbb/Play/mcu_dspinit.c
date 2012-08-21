#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"

U8 MP3_Download(void)
{
	U8 tc_Cnt = 0;

 	if ( DSP_Download(0x00, 0x10)!=DSP_SUCCESS )
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	for (tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if (DSP_PlayInit()==DSP_SUCCESS)
		{
			return 0;
		}

		if (tc_Cnt < 5) 
		{
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
		else
		{
			GLOBAL_REG[0x02] |= 0x01;		// DSP reset
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
	}

	return DSP_PLAY_COMMAND_ERROR;
}


U8 WMA_Download(void)
{
	U8 tc_Cnt = 0;
	U8 tc_result = 0;



	if ( DSP_Download(0x12, 0x25)!=DSP_SUCCESS )
		return DSP_PLAY_COMMAND_ERROR;

	L2_DSP_Write_DMem16(DSP_W_asf_parse_ok, 0x0);
	L2_DSP_Write_DMem16(DSP_W_asf_parse_mode, 0x01);		//Set ASF Parser Mode

	for (tc_Cnt=0;tc_Cnt<10;++tc_Cnt) 
	{
		if (DSP_PlayInit()==DSP_SUCCESS)
		{
			break;
		}

		if (tc_Cnt < 5) 
		{
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
		else
		{
			GLOBAL_REG[0x02] |= 0x01;		// DSP reset
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
	}
	if (tc_Cnt == 10)
		return DSP_PLAY_COMMAND_ERROR;

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
		return DSP_PLAY_COMMAND_ERROR;
	}

	gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;	// reset bitstream address of DM

	if (DOS_Open_File_r(0x00,0x02,0x00))
		return DSP_PLAY_COMMAND_ERROR;

	while(tc_result == 0)
	{
		if(WMA_DataIn() == DSP_DATAIN_COMMAND_ERROR) 	
			return DSP_DATAIN_COMMAND_ERROR;

		tc_result = L2_DSP_Read_DMem16(DSP_W_asf_parse_ok);
	}

	if(tc_result != 1)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	L2_DSP_Write_DMem16(L2_DSP_Write_DMem16,0);
/*-- DSP will jump to pc=0 automatically after asf parse has been done--*/

//----------------------
//2. Normal wma download
//----------------------

	switch(L2_DSP_Read_DMem16(DSP_w_EntropyMode))
	{
		case 1:
				tc_result = DSP_Download(0x01, 0x23);
				break;

		case 2:
				tc_result = DSP_Download(0x01, 0x24);
				break;

		case 3:
				tc_result = DSP_Download(0x01, 0x25);
				break;
		default:
				break;
	}
		
	if(tc_result != DSP_SUCCESS )	
		return DSP_PLAY_COMMAND_ERROR;

	for (tc_Cnt=0;tc_Cnt<10;++tc_Cnt) {
		if (DSP_PlayInit()==DSP_SUCCESS)
			break;

		if (tc_Cnt < 5) 
		{
			//GLOBAL_REG[0x02] |= 0x01;		// DSP reset, ycc mark
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
		else
		{
			GLOBAL_REG[0x02] |= 0x01;		// DSP reset
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
	}

	if (tc_Cnt == 10)
		return DSP_PLAY_COMMAND_ERROR;	

	L2_DSP_Write_DMem16(DSP_W_asf_parse_mode, 0x0);		//Clear ASF Parser Mode

	return DSP_SUCCESS;
}



