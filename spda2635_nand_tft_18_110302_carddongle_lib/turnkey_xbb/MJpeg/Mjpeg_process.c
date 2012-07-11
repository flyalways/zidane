#include <string.h>
#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "dos\dosfile.h"
#include "memalloc.h"
#include "..\header\host_init.h"

#include "..\LCM\model_define.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\LCM\LCM.h"
#include "..\LCM\UI_icon.h"
#include "..\LCM\TFT_display.h"		// TFT
#include "..\LCM\TFT_config.h"		// TFT
#include "..\header\variables.h"

#define C_LCM_ROW_MAX 5//6  //20090331--sunzhk modify 100702

xdata	U16	gw_IMG_DMA_Sz;
xdata	U16	gw_IMG_DMABuf_ptr[4];
extern	UBYTE 	xdata EXT_NameC[];
extern	void IR_Service_Process_Menu_1();
extern	void IR_Service_Process();

U8 MJPEG_DataIn(void);
U8 MJPEG_DataOut(void);

extern	U8		Image_Download(void);
extern	U8		Image_PlayCmd(void);
extern	U8		Music_ResumeCmd(void);
extern 	U8   	USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);//use reserveblock for log
extern	U8		X_Unicode2ISN(unsigned char *UnicodeArray, bit Convert);
extern	void	InitDispVariable(void);
extern	void	LCM_Display_Func1(void);
extern	void	LCM_set_view(U8 view_type, U16 x_start, U16 y_start, U16 x_size, U16 y_size);
extern	void	Image_MCU_LCM_DMA(U16 movesize, U16 tw_Src);
extern	void	DecAndReload1sTimeOutCnt(void);
extern	void	Music_VolumeUpCmd(void);
extern	void	Music_VolumeDownCmd(void);
extern	void	Music_SetVolumeCmd(void);
extern	void	LCM_set_view(U8 view_type, U16 x_start, U16 y_start, U16 x_size, U16 y_size);
extern	void	Image_WakeUp(U8 tc_Type);
extern	void	FM_FREQ_CHG(U8 tc_UP_DOWN,U8 offset);  //20090331
extern	xdata	System_Struct gs_System_State;
extern	struct	str_scroll_t	gs_str_scroll_state;

/* =============== MJPEG function =============== */
//===================================================================================
//
U8 MJPEG_Download(void)
{
	U8 tc_Cnt = 0;
	
	if (DSP_Download(0x08, 0x00) != DSP_SUCCESS)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
	
//	dbprintf("MJPEG_Download\n");
	
	for (tc_Cnt=0; tc_Cnt<10; ++tc_Cnt) 
	{
		if (DSP_PlayInit() == DSP_SUCCESS)
		{
			break;
		}
//		dbprintf("Retry to start DSP\n");

		if (tc_Cnt < 5) 
		{
			GLOBAL_REG[0x02] &= 0xFE;       // Enable DSP Run Normal Mode
		}
		else
		{
//			dbprintf("Hardware Reset DSP!!!!!\n");
			GLOBAL_REG[0x02] |= 0x01;       // DSP reset
			GLOBAL_REG[0x02] &= 0xFE;       // Enable DSP Run Normal Mode
		}
	}

	if (tc_Cnt == 10)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	return DSP_SUCCESS;
}

U8 MJPEG_PlayCmd(void)
{
	// sent "PLAY" command to DSP
	if (L2_DSP_SendCommandSet(DCMD_Play) != DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
//	dbprintf("Play Cmd Ok~\n");

	gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;      // reset bitstream buffer index
	gs_DSP_GLOBAL_RAM.sc_Pixel_Index = 0;   // reset output pixel buffer index
	gs_DSP_GLOBAL_RAM.sc_End_Of_File = 0;   // reset file end flag
	
	// JPEG file open
	if (DOS_Open_File_r(C_OtherFHandle, C_Open_FoundFile, C_NullFileName))
	{
		return DSP_PLAY_COMMAND_ERROR;
	}

	return DSP_SUCCESS;
}


void MJPEG_MCU_DM_DMA(U8 BufferIndex)
{
	U8  tc_DSPAddr_High;
	U16	tw_DSP_Word_Align;
	/*  _______________________________
	 * |___|___________|_______________|
	 *  0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 
	 *  msb 2-bit offset, 14-bit DSP DM address
     */
	code U16 MJPEG_DSP_Addr_Table[3] = 
	{
		0x2E00, 0xAEaa, 0x6F55
	};

	// ----- DMA Reset -----
	DMA_REG[0x04] = 0x09;
	DMA_REG[0x04] = 0x08;
	DMA_REG[0xC0] = 0x00;		// clear DMA complete
	DMA_REG[0x30] = 0x01;		// clear checksum
	
	// ----- DMA initial settings for DM transfer -----
	DSP_REG[0x16] = 0x00;		// Remain DSP mode.  When you do MCU -> DM, MCU will stop DSP automaticlly
	DMA_REG[0x01] = 0x50;		// DMA : SRAM --> DM

	// Define data size in DMA (assume 512 bytes)
	DMA_REG[0x02] = 0xFF;
	DMA_REG[0x03] = 0x01;

	// Destination
	DMA_REG[0x40] = 0x00;		// 24-bit DMA
	tw_DSP_Word_Align = MJPEG_DSP_Addr_Table[BufferIndex];
	tc_DSPAddr_High = *((U8 *)&tw_DSP_Word_Align);
	DSP_REG[0x1D] = tc_DSPAddr_High >> 6;
	DSP_REG[0x18] = tc_DSPAddr_High & 0x3F;
	DSP_REG[0x17] = *((U8 *)&tw_DSP_Word_Align + 1);
	CPU_REG[0x12] = 0;
	CPU_REG[0x13] = (U8)((U16)gc_PlayRecordDataBuf >> 8);
		
	// ------------------------------------------------
	DMA_REG[0xB0] = 0x01;		// DMA Run (0xB3B0)
	// wait until DMA cycle is completed (0xB3C0)
	while(!(DMA_REG[0xC0]&0x01));
	DSP_REG[0xC0] = 0x00;
}


void MJPEG_DM_MCU_DMA(U8 BufferIndex, U16 movesize)//Jimi 090813
{
	// ----- DMA Reset -----
	DMA_REG[0x04] = 0x09;
	DMA_REG[0x04] = 0x08;
	DMA_REG[0xC0] = 0x00;		// clear DMA complete
	DMA_REG[0x30] = 0x01;		// clear checksum		
	
	// ----- DMA initial settings for DM transfer -----		
	DSP_REG[0x16] = 0x00;		// Remain DSP mode.  When you do MCU -> DM, MCU will stop DSP automaticlly
	DMA_REG[0x01] = 0x05;		// DM -> SRAM

	//temp = movesize - 1;
	DMA_REG[0x02] = (U8)movesize - 1;
	DMA_REG[0x03] = (U8)((movesize - 1)>>8);

	// Source
	DMA_REG[0x40] = 0x01;		// 16-bit DMA
	DSP_REG[0x1D] = 0x01;

	DSP_REG[0x17] = (U8)gw_IMG_DMABuf_ptr[BufferIndex];
	DSP_REG[0x18] = (U8)(gw_IMG_DMABuf_ptr[BufferIndex]>>8);		// bitstream start address : 0x2000

	// Destination
	CPU_REG[0x14] = 0;
	CPU_REG[0x15] = (U8)((U16)gc_MotionJpegOutBuf >> 8);

	// ------------------------------------------------
	DMA_REG[0xB0] = 0x01;		// DMA1 start
	// wait until DMA cycle is completed (0xB3C0)		
	while(!(DMA_REG[0xC0]&0x01));		
	DMA_REG[0xC0] = 0x00;
}
//===================================================================================
//
//===================================================================================
void MJPEG_MCU_LCM_DMA(U16 movesize, U16 tw_Src)
{
	XBYTE[0xB304] = 0x09; //reset DMA machine
	XBYTE[0xB304] = 0x08; 
	
	XBYTE[0xB423] = 3;		// set NAND /CS(bit 0), /WP(bit 1), ALE(bit 2), CLE(bit 3)
	
	FLASH_REG[0x00] = 0x00;
	DMA_REG[0x01] = 0x20;		//Sram to Flash
	
	// source
	CPU_REG[0x12] = (U8)(tw_Src);
	CPU_REG[0x13] = (U8)(tw_Src >> 8);

	// size
	DMA_REG[0x02] = (U8)(movesize-1);
	DMA_REG[0x03] =	(U8)((movesize-1) >> 8);
	
	DMA_REG[0xC0] =	0x00;		//clear DMA completed flag
	LCM_A0_HI;
	LCM_CS_LO;
	DMA_REG[0xB0] =	0x01;		//trigger DMA
	while(DMA_REG[0xC0] == 0);
	DMA_REG[0xC0] = 0;
	LCM_CS_HI;
	LCM_A0_LO;

	FLASH_REG[0x00] = 0x01;
}


U8 MJPEG_DataIn(void)
{
	U8 tc_DosRead_Status;
	
	if (!gs_DSP_GLOBAL_RAM.sc_End_Of_File)
	{
		while(L2_DSP_Read_DMem16(DSP_EmptyBuffer) >= ONE_SECTOR)
		{
			tc_DosRead_Status=DOS_Read_File(C_OtherFHandle);
				
			if (tc_DosRead_Status != DOS_END_OF_FILE)
			{
				if(tc_DosRead_Status!=DOS_FALSE)
				{
					MJPEG_MCU_DM_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index++);
					if (L2_DSP_SendCommandSet(DCMD_DatIn) != DCMD_DatIn)
					{
						return DSP_DATAIN_COMMAND_ERROR;
					}
				
					if (gs_DSP_GLOBAL_RAM.sc_DM_Index >= 3)	// JPEG bitstream buffer 0x3400~0x3600, sz = 1536 bytes
					{
						gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;
					}
		
					gs_DSP_GLOBAL_RAM.sdw_sector_num--;
					if (gs_DSP_GLOBAL_RAM.sdw_sector_num == gs_DSP_GLOBAL_RAM.sw_sector_cnt)
					{
						return DSP_DECODE_END;				// DSP pre-stop to start ramp down
					}
				}
				else
				{
//					dbprintf("DOS read state fail!\n");
					L2_DSP_Write_DMem16(0x3F44, 0x00FF);	// JPEG file end flag
					gs_DSP_GLOBAL_RAM.sc_End_Of_File = 1;
					return DSP_DATAIN_COMMAND_ERROR;
				}
			}
			else
			{
				L2_DSP_Write_DMem16(0x3F44, 0x00FF);	// JPEG file end flag
				gs_DSP_GLOBAL_RAM.sc_End_Of_File = 1;
				break;
			}					
		}
	}
	return	DSP_SUCCESS;
}


U8 MJPEG_DataOut(void)
{
	U16 RemData = L2_DSP_Read_DMem16(DSP_SOutBufRemData);

	if (RemData >= gw_IMG_DMA_Sz)
	{
		do
		{
			MJPEG_DM_MCU_DMA(gs_DSP_GLOBAL_RAM.sc_Pixel_Index++, gw_IMG_DMA_Sz);

			if (L2_DSP_SendCommandSet(DCMD_DatOut) != DCMD_DatOut)
			{
				return DSP_DATAOUT_COMMAND_ERROR;
			}

			if (gs_DSP_GLOBAL_RAM.sc_Pixel_Index >= 4) //Jimi JPEG SW decoder output buffer from 0x3000~0x3A00
			{
				gs_DSP_GLOBAL_RAM.sc_Pixel_Index = 0;
			}

			/* ==== Display on LCM: DMA Sram to LCM ==== */
			MJPEG_MCU_LCM_DMA(gw_IMG_DMA_Sz, (U16)gc_MotionJpegOutBuf);
		
		}while(L2_DSP_Read_DMem16(DSP_SOutBufRemData) >= gw_IMG_DMA_Sz);
	
	}
	else if (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)
	{
		if (RemData > 0)
		{
			MJPEG_DM_MCU_DMA(gs_DSP_GLOBAL_RAM.sc_Pixel_Index, RemData);
			if (L2_DSP_SendCommandSet(DCMD_DatOut) != DCMD_DatOut)
			{
				return DSP_DATAOUT_COMMAND_ERROR;
			}

			/* ==== Display on LCM: DMA Sram to LCM ==== */
			MJPEG_MCU_LCM_DMA(RemData, (U16)gc_MotionJpegOutBuf);
		} 
		return DSP_DECODE_END;
	}
	else if (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x0020)
	{
		return DSP_USER_FILE_TYPE_ERROR;
	}

	return DSP_SUCCESS;
}


U8 MJPEG_PauseCmd(void) //Jimi 090406, in order to mute op gain.
{	
	U16 TimeOUT;
	if (L2_DSP_SendCommandSet(DCMD_Pause) != DCMD_Pause)
	{
		return DSP_PAUSE_COMMAND_ERROR;
	}

	if ((gs_System_State.c_Phase != TASK_PHASE_FASTFWD) && (gs_System_State.c_Phase != TASK_PHASE_FASTREV))
	{
		TimeOUT = 0xffff;
		do
		{
			TimeOUT--;
			if (TimeOUT == 0)
			{
				return DSP_RUNNING_STATUS1_ERROR;
			}

			if (MJPEG_DataIn() != DSP_SUCCESS)
			{
				gc_PhaseInx = 5;	// End file process
				break;
			}

			if (MJPEG_DataOut() != DSP_SUCCESS )
			{	// "file end" or "error format"
				gc_PhaseInx = 5;	// End file process
				break;
			}
		} while ((!L2_DSP_Read_DMem16(DSP_RampDownComplete))||(!(L2_DSP_Read_DMem16(DSP_RunningStatus)&0x0002))); //Jimi 090410
	}
	return DSP_SUCCESS;
}


U8 MJPEG_StopCmd(UBYTE tc_FileHandle)
{
	U8 tc_Ret, tc_state;
	U16 TimeOUT;
	
	tc_Ret = DSP_StopCmd();

	if (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)	// reset DSP_DecodeStatus
	{
		L2_DSP_Write_DMem16(DSP_DecodeStatus, (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0xFF3F));
	}
	
	TimeOUT = 0xffff;
	while (1)
	{
		TimeOUT--;
		if (TimeOUT == 0)
		{
			tc_Ret = DSP_DECODE_STATUS_TIMEOUT_ERROR;
			break;
		}

		tc_state = MJPEG_DataIn();
		if (tc_state == DSP_DATAIN_COMMAND_ERROR)
		{
			tc_Ret = tc_state;
			break;
		}

		tc_state = MJPEG_DataOut();

		if((SD_Detect==1) && (gc_CurrentCard==2))
		{
			tc_state=DSP_DECODE_END;	
		}
		if (tc_state == DSP_DATAOUT_COMMAND_ERROR)
		{
			tc_Ret = tc_state;
			break;
		}
		else if (tc_state == DSP_USER_FILE_TYPE_ERROR)
		{
			break;
		}
		else if (tc_state == DSP_DECODE_END)
		{
			break;
		}
	}

	// Close current file
	DOS_Close_File_r(tc_FileHandle);

	// wait for ramp down ok and Turn off DSP clock, ycc 081113
	{
		U16 dbgTmp;		   
    	U16 TimeOUT = 0xFFFF;
		do{
	   		dbgTmp = L2_DSP_Read_DMem16(DSP_RestartFlag);  //read DSP 0x3F0B.0 ready status
	  		TimeOUT --;		
	   		if(TimeOUT==0)
	       	{			
	       		break; 
	   		} 
		}while(dbgTmp!=0);
	}

	if (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00E0)	// reset DSP_DecodeStatus
	{
		L2_DSP_Write_DMem16(DSP_DecodeStatus, (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0xFF1F));
	}

	set_clock_mode(CLOCK_MODE_MP3);

	GLOBAL_REG[0x10] &= 0xFE;	//Turn off DSP clock for power saving
	return tc_Ret;
}


void mjpeg_idle()
{

}


void mjpeg_init()
{	
	if(gw_init_needed & SET_BIT5)
	{
		gw_init_needed&=CLR_BIT5;
		
		gs_System_State.c_Phase = TASK_PHASE_STOP;
		gs_System_State.c_FileHandle=2;
		//(JC)search music file overall by FDB, findout 1st one in ROOT
		gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster = gdw_DOS_RootDirClus;
		EXT_NameC[0] = 2;
		EXT_NameC[1] = 'S';
		EXT_NameC[2] = 'C';
		EXT_NameC[3] = 'V';
		EXT_NameC[4] = 'A';
		EXT_NameC[5] = 'M';
		EXT_NameC[6] = 'V';
		DOS_Search_File(C_File_All|C_Cnt_FileNo,C_OtherFileType,C_CmpExtName|C_Next);

		if(DOS_Search_File(C_File_All|C_By_FDB, C_OtherFHandle, C_CmpExtName|C_Next))
		{
			gw_init_needed |= SET_BIT8;
			gc_Task_Next=C_Task_Menu;
			gc_PhaseInx = 0;
			return;
		}
		gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;
		//#if 1 
		//set_clock_mode(3);
		//#else
		set_clock_mode(CLOCK_MODE_MJPEG);
		//#endif
		gc_PhaseInx=C_MjpegPause;
	}
}


void mjpeg_proc()
{
	gc_KeyDet_Mask = 0;
	if (MJPEG_DataIn() != DSP_SUCCESS)
	{
		gc_PhaseInx = 5;	// End file process
		return;
	}
	if (MJPEG_DataOut() != DSP_SUCCESS )
	{	// "file end" or "error format"
		gc_PhaseInx = 5;	// End file process
	}
}


void mjpeg_playpause()
{
	U8  i;
	U16 k,tw_temp,tw_AudioBR,tw_VideoBR;
	U8  tc_second, tc_minute;
	U8  *p_data;
	U16 tw_hour;

	switch(gs_System_State.c_Phase)
   	{
		case TASK_PHASE_PLAYACT:
			if (MJPEG_PauseCmd() != DSP_SUCCESS)
			{
			}

			gs_System_State.c_Phase = TASK_PHASE_PAUSE;
			for(i=0;i<2;i++)  //20090604 temp
			{
				TFT_PAUSE();
			}
		break;

		case TASK_PHASE_PAUSE:
		//Draw a black frame
			for (k=0; k<512; k++)
			{
				((U32 *)gc_MotionJpegOutBuf)[k] = 0;
			}
			LCM_set_view(1, 0, 0, TFT_H_Size, TFT_V_Size); // horizontal view

			for (i=0; i<20; i++)		//Jimi 090305
			{
				MJPEG_MCU_LCM_DMA(2048, (U16)gc_MotionJpegOutBuf);
			}

			if (gc_Play_FileType == 0)
			{	
				LCM_set_view(1, gw_x_start, gw_y_start, gw_width, gw_height); // horizontal view
			}
			else
			{
				LCM_set_view(3, gw_x_start, gw_y_start, gw_width, gw_height); // horizontal view	
			}
			//------------------------------------------------------------------------------
		//MJPEG resume cmd
		if (Music_ResumeCmd() != DSP_SUCCESS)
		{

		}
		gs_System_State.c_Phase = TASK_PHASE_PLAYACT;
		break;

		case TASK_PHASE_FASTFWD:
		case TASK_PHASE_FASTREV:
			gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
		break;

		default:
			for (k=0; k<512; k++)
			{
				((U32 *)gc_MotionJpegOutBuf)[k] = 0;
			}
			LCM_set_view(1, 0, 0, TFT_H_Size, TFT_V_Size); // horizontal view

			for (i=0; i<20; i++)		//Jimi 090305
			{
				MJPEG_MCU_LCM_DMA(2048, (U16)gc_MotionJpegOutBuf);
			}
	
			//DSP wake up
			Image_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);
			//DSP download
			if(MJPEG_Download())   // Load IM, PM
			{
//				dbprintf("DSP IMPM download err\n");
			}

			//Set file type, 0: SCV, 1: JPEG, 2: BMP, 3: GIF, 4: AMV
			if (gc_Play_FileType == 0)
			{
				L2_DSP_Write_DMem16(0x3F2F, 0x0000);	// SCV file
			}
			else
			{
				L2_DSP_Write_DMem16(0x3F2F, 0x0004);	// AMV file
			}

			//initialize structure 'n variables
			gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode = REPEAT_A;	// initial variables

			//set volume;restore the volume value
			if(gc_VolumeMute==0)
			{
        		Music_SetVolumeCmd();
			}
			else
			{
				L2_DSP_Write_DMem16(DSP_VolumeControl,0);//20090812 chiayen add VOL=0 	
			}

			//Parsing motion jpeg header
			if(DOS_Open_File_r(C_OtherFHandle, C_Open_FoundFile, C_NullFileName) || (gs_File_FCB[0].dw_File_TotalSize==0)) 
			{
			}
			if (gc_Play_FileType == 0)
			{	// Parsing SCV header
				DOS_Read_File(C_OtherFHandle);
				gw_width = *((U16 *)(gc_PlayRecordDataBuf + 0x06));
				gw_height = *((U16 *)(gc_PlayRecordDataBuf + 0x08));

				gs_DSP_GLOBAL_RAM.sc_frame_rate  = gc_PlayRecordDataBuf[10];			//Jimi 090410
				gw_TotalSec = 	*((U16 *)(gc_PlayRecordDataBuf+19));//Jimi 090410, total seconds.

				gw_x_start = (TFT_H_Size - gw_width) >> 1;
				gw_y_start = (TFT_V_Size - gw_height) >> 1;
				LCM_set_view(1, gw_x_start, gw_y_start, gw_width, gw_height); // horizontal view in normal order

				tw_AudioBR = *((U16 *)(gc_PlayRecordDataBuf + 0x0B));
				tw_VideoBR = *((U16 *)(gc_PlayRecordDataBuf + 0x0D));

				gs_DSP_GLOBAL_RAM.sw_data_start = ONE_SECTOR;
				gs_DSP_GLOBAL_RAM.sw_kbyte_per_sec = (tw_AudioBR + tw_VideoBR)/8;    //Jimi 090410
				gs_DSP_GLOBAL_RAM.sw_sector_cnt = (U16)(((U32)(tw_AudioBR + tw_VideoBR) * 1000) >> (3 + 1 + 9));	// 0.5 sec for ramp down
				gs_DSP_GLOBAL_RAM.sdw_sector_num = (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize - ONE_SECTOR) >> 9;	// minus header size		
			}
			else
			{	// Parsing AMV header
				DOS_Read_File(C_OtherFHandle);
				p_data = (U8 *)(gc_PlayRecordDataBuf + 0x40);
				gw_width = (U16)(*p_data++);
				gw_width |= ((U16)(*p_data) << 8);
				p_data += 3;
				gw_height = (U16)(*p_data++);
				gw_height |= ((U16)(*p_data) << 8);

				gs_DSP_GLOBAL_RAM.sc_frame_rate  = gc_PlayRecordDataBuf[72];    //Jimi 090410

				gw_x_start = (TFT_H_Size - gw_width) >> 1;
				gw_y_start = (TFT_V_Size - gw_height) >> 1;
				LCM_set_view(3, gw_x_start, gw_y_start, gw_width, gw_height); // horizontal view in BMP order
			
				p_data += 0x0F;
				tc_second = *p_data++;
				tc_minute = *p_data++;
				tw_hour = (U16)(*p_data++);
				tw_hour |= ((U16)(*p_data++) << 8);

				gw_TotalSec = (tw_hour * 3600) + (tc_minute * 60) + tc_second;
				gs_DSP_GLOBAL_RAM.sw_kbyte_per_sec = gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize/1000/gw_TotalSec;    //Jimi 090410

				gs_DSP_GLOBAL_RAM.sw_data_start = 0;
				gs_DSP_GLOBAL_RAM.sdw_sector_num = gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize >> 9;
				gs_DSP_GLOBAL_RAM.sw_sector_cnt = (gs_DSP_GLOBAL_RAM.sdw_sector_num / gw_TotalSec) >> 1;	// 0.5 sec for ramp down
			}

			gw_IMG_DMA_Sz = gw_width << 3;  //DSP_MJPEG_SOutBufDataSz = width *4(Lines) *2(Bytes/pixel)
			L2_DSP_Write_DMem16(DSP_MJPEG_SOutBufDataSz, gw_IMG_DMA_Sz);
			tw_temp = gw_width <<4;
			L2_DSP_Write_DMem16(DSP_MJPEG_SOutBufMaxSzPixel, tw_temp);

			gw_IMG_DMABuf_ptr[0] = 0x3000;
			tw_temp = gw_IMG_DMA_Sz/2;
			for( i =0; i< 3; i++)
			{
				gw_IMG_DMABuf_ptr[i+1] = gw_IMG_DMABuf_ptr[i] + tw_temp;
			}
		
			if (MJPEG_PlayCmd() == DSP_PLAY_COMMAND_ERROR)  //motion play cmd
			{

			}

			if(gc_Play_FileType == 0)
			{
				DOS_Read_File(C_OtherFHandle);
			}
		
			gs_System_State.c_Phase = TASK_PHASE_PLAYACT;
		break;
   	}
	#if 1
	set_clock_mode(CLOCK_MODE_MJPEG2);
	#else
	set_clock_mode(CLOCK_MODE_MJPEG);
	#endif
   	gc_PhaseInx = 2;
}


void mjpeg_stop()
{
	U8 tc_Ret;

	if (gs_System_State.c_Phase != TASK_PHASE_STOP)
	{					
		tc_Ret = MJPEG_StopCmd(gs_System_State.c_FileHandle); //DSP stop cmd
		if (tc_Ret != DSP_SUCCESS)
		{
			//dbprintf("MJPEG stop error !!\n");
		}

		gs_System_State.c_Phase = TASK_PHASE_STOP;
	}
}


void mjpeg_endfile()
{	
	mjpeg_stop(); //DSP stop cmd// Mark test code	
    if(gc_RepPlayMode==C_NoRepeat) //no repeat  //20090331
    {
        if(gw_FileIndex[gs_System_State.c_FileHandle]==gw_FileTotalNumber[gs_System_State.c_FileHandle])
        {
            gc_PhaseInx=0;  
			gc_DispWallpaper=0;
			gw_Disp_CurrentSec=0;
			gs_System_State.c_Phase=2;
			gc_Play_FileType=2;
			gw_TotalSec=0;
			LCM_Display_Func1();
        }
		else
		{
			if(gb_DirPlay_Flag==1)
			{
				DOS_Search_File(C_File_OneDir|C_By_FDB, C_OtherFileType, C_CmpExtName|C_Next);
			}
			else
			{
            	DOS_Search_File(C_File_All|C_By_FDB, C_OtherFileType,C_CmpExtName|C_Next);
			}
            gc_PhaseInx=3;				
		}  
    }
	else
	{
		if(gc_RepPlayMode==C_RepeatOne) //repeat one
	    {
    	    gc_PhaseInx=3;      
    	}
		else
		{
			DOS_Search_File(C_File_All|C_By_FDB, C_OtherFileType, C_CmpExtName|C_Next);
			gc_PhaseInx = 3;
		}
	}
	gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;           //Jimi 090305	
}


void mjpeg_next()
{
	U8 tc_Ret;

	switch(gs_System_State.c_Phase)
	{
		case TASK_PHASE_STOP:
			if (gw_FileIndex[gs_System_State.c_FileHandle] == 0)
			{
				return;
			}
			gc_PhaseInx = 0;
		break;

		case TASK_PHASE_PLAYACT:
			gc_PhaseInx = 3;
			gc_KeyDet_Mask = 1;
		break;

		case TASK_PHASE_PAUSE:
			gc_PhaseInx = 0;
		break;
	}

	if (gs_System_State.c_Phase != TASK_PHASE_STOP)
	{
		tc_Ret = MJPEG_StopCmd(gs_System_State.c_FileHandle); //DSP stop cmd
		if (tc_Ret != DSP_SUCCESS)
		{
//			dbprintf("NEXT MJPEG stop error !!\n");
		}		
		gs_System_State.c_Phase = TASK_PHASE_STOP;
	}
	DOS_Search_File(C_File_All|C_By_FDB, C_OtherFileType, C_CmpExtName|C_Next);	
	gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;           //Jimi 090305
	gs_System_State.c_Phase = TASK_PHASE_STOP;
}


void mjpeg_prev()
{
	U8 tc_Ret;
	switch(gs_System_State.c_Phase)
	{
		case TASK_PHASE_STOP:
			if (gw_FileIndex[gs_System_State.c_FileHandle] == 0)
			{
				return;
			}
			gc_PhaseInx=0;
		break;
	
		case TASK_PHASE_PLAYACT:
			gc_PhaseInx = 3;
			gc_KeyDet_Mask = 1;	
		break;
	
		case TASK_PHASE_PAUSE:
			gc_PhaseInx = 0;
		break;
	}
	
	if (gs_System_State.c_Phase != TASK_PHASE_STOP)
	{		
		tc_Ret = MJPEG_StopCmd(gs_System_State.c_FileHandle); //DSP stop cmd
		if (tc_Ret != DSP_SUCCESS)
		{
//			dbprintf("MJPEG stop error !!\n");
		}
		gs_System_State.c_Phase = TASK_PHASE_STOP;
	}
	DOS_Search_File(C_File_All|C_By_FDB, C_OtherFileType, C_CmpExtName|C_Prev);
	gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;           //Jimi 090305
	gs_System_State.c_Phase = TASK_PHASE_STOP;
}


void mjpeg_fastfwd()
{
	U8	tc_Ret;
	U16	tw_curr_sec;
	U32	tdw_datapoint,tdw_sector;

	if (gs_System_State.c_Phase == TASK_PHASE_STOP)
	{
		return;
	}

	if (gs_System_State.c_Phase != TASK_PHASE_FASTFWD)
	{
		gs_System_State.c_Phase = TASK_PHASE_FASTFWD;
		L2_DSP_Write_DMem16(0x3F59, 0x0001);
	}
	
	if (L2_DSP_Read_DMem16(0x3F5A) == 0x0001)  //FF/FR mechanism; accumulate time, overflow?
	{
		tdw_sector = gs_DSP_GLOBAL_RAM.sdw_sector_num - gs_DSP_GLOBAL_RAM.sw_sector_cnt;
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
		if (tdw_sector > gs_DSP_GLOBAL_RAM.sw_sector_cnt)
		{
			tdw_datapoint = gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint + ((U32)gs_DSP_GLOBAL_RAM.sw_sector_cnt << 9);
			gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = DOS_Seek_File(gs_System_State.c_FileHandle, tdw_datapoint>>9);
			gs_DSP_GLOBAL_RAM.sdw_sector_num = tdw_sector;
	
			tw_curr_sec = tdw_datapoint/1000/gs_DSP_GLOBAL_RAM.sw_kbyte_per_sec;	
			gs_DSP_GLOBAL_RAM.sw_frm_cnt = tw_curr_sec * (U16)gs_DSP_GLOBAL_RAM.sc_frame_rate;	
			L2_DSP_Write_DMem16(DSP_DecodeFrameCounter, gs_DSP_GLOBAL_RAM.sw_frm_cnt);

			gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;		// reset bitstream buffer index
			L2_DSP_Write_DMem16(0x3F5A, 0x0000);
		}
		else
		{
			tdw_datapoint = gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize - ((U32)gs_DSP_GLOBAL_RAM.sw_sector_cnt << 9);
			gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = DOS_Seek_File(gs_System_State.c_FileHandle, tdw_datapoint>>9);
			gs_DSP_GLOBAL_RAM.sdw_sector_num = gs_DSP_GLOBAL_RAM.sw_sector_cnt;
	
			gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;		// reset bitstream buffer index
			L2_DSP_Write_DMem16(0x3F59, 0x0000);
			L2_DSP_Write_DMem16(0x3F5A, 0x0000);

			gs_DSP_GLOBAL_RAM.sw_frm_cnt = gw_TotalSec * (U16)gs_DSP_GLOBAL_RAM.sc_frame_rate;  //20090414 move here
			L2_DSP_Write_DMem16(DSP_DecodeFrameCounter, gs_DSP_GLOBAL_RAM.sw_frm_cnt);

			tc_Ret = MJPEG_StopCmd(gs_System_State.c_FileHandle);
			if (tc_Ret != DSP_SUCCESS)
			{
			}
			gs_System_State.c_Phase = TASK_PHASE_STOP; //Jimi 090410
			return;
		}
	}

	if (MJPEG_DataIn() == DSP_DATAIN_COMMAND_ERROR)
	{

	}

	if (MJPEG_DataOut() != DSP_SUCCESS )
	{	// "file end" or "error format"

	}
}


void mjpeg_fastrev()
{
	U8	tc_Ret;
	U16	tw_curr_sec;
	U32	tdw_datapoint;

	if ((gs_System_State.c_Phase != TASK_PHASE_FASTREV) && (gs_System_State.c_Phase != TASK_PHASE_PAUSE))
	{
		gs_System_State.c_Phase = TASK_PHASE_FASTREV;
		L2_DSP_Write_DMem16(0x3F59, 0x0001);
	}

	if (L2_DSP_Read_DMem16(0x3F5A) == 0x0001)  //FF/FR mechanism; decrease time, underflow??
	{
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
		tdw_datapoint = gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint - ((U32)gs_DSP_GLOBAL_RAM.sw_sector_cnt << 9);
                
		tw_curr_sec = tdw_datapoint/1000/gs_DSP_GLOBAL_RAM.sw_kbyte_per_sec;  //Jimi 090410	
		gs_DSP_GLOBAL_RAM.sw_frm_cnt = tw_curr_sec * (U16)gs_DSP_GLOBAL_RAM.sc_frame_rate;
		L2_DSP_Write_DMem16(DSP_DecodeFrameCounter, gs_DSP_GLOBAL_RAM.sw_frm_cnt);
		if (tdw_datapoint > gs_DSP_GLOBAL_RAM.sw_data_start)
		{
			gs_DSP_GLOBAL_RAM.sdw_sector_num += gs_DSP_GLOBAL_RAM.sw_sector_cnt;
		}
		else
		{
			tdw_datapoint = gs_DSP_GLOBAL_RAM.sw_data_start;
			gs_DSP_GLOBAL_RAM.sdw_sector_num = (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize - tdw_datapoint) >> 9;	// minus header size

			L2_DSP_Write_DMem16(0x3F59, 0x0000);
			tc_Ret = MJPEG_PauseCmd();
			if (tc_Ret != DSP_SUCCESS)
			{

			}
			gs_System_State.c_Phase = TASK_PHASE_PAUSE; //090410
			gs_DSP_GLOBAL_RAM.sw_frm_cnt = 0;
			L2_DSP_Write_DMem16(DSP_DecodeFrameCounter, gs_DSP_GLOBAL_RAM.sw_frm_cnt);	//Jimi 090408
		}
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = DOS_Seek_File(gs_System_State.c_FileHandle, tdw_datapoint>>9);
		gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;		// reset bitstream buffer index
		L2_DSP_Write_DMem16(0x3F5A, 0x0000);
	}

	if (MJPEG_DataIn() == DSP_DATAIN_COMMAND_ERROR)
	{

	}

	if (MJPEG_DataOut() != DSP_SUCCESS )
	{	// "file end" or "error format"

	}
}


void mjpeg_fffr_end()
{
	if ((gs_System_State.c_Phase == TASK_PHASE_FASTFWD) || (gs_System_State.c_Phase == TASK_PHASE_FASTREV))
	{
		L2_DSP_Write_DMem16(0x3F59, 0x0000);
		if (L2_DSP_Read_DMem16(0x3F5A) == 0x0001)
		{
			gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;		// reset bitstream buffer index
			L2_DSP_Write_DMem16(0x3F5A, 0x0000);
		}
		gc_PhaseInx = 3;
	}
	else if (gs_System_State.c_Phase == TASK_PHASE_STOP)	// fastfwd to file end
	{
		gc_PhaseInx = 3; //next //Jimi 090406
		gc_KeyDet_Mask = 1;

		DOS_Search_File(C_File_All|C_By_FDB, C_OtherFileType, C_CmpExtName|C_Next);	
		gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;
	}
	else if (gs_System_State.c_Phase == TASK_PHASE_PAUSE)	// fastrev to file start
	{
		gc_PhaseInx = 3;									// if fast backward to the file start then pause and resume
	}
}


void mjpeg_volup()
{
	gw_LogData_Timer=60;
	Music_VolumeUpCmd();

	if(gs_System_State.c_Phase==TASK_PHASE_STOP)  //20090331
    {
        gc_PhaseInx=C_MjpegIdle;   
    }
    else
    {
        gc_PhaseInx=C_MjpegProc;
    }
}


void mjpeg_voldn()
{
	gw_LogData_Timer=60;
	Music_VolumeDownCmd();

	if(gs_System_State.c_Phase==TASK_PHASE_STOP)  //20090331
    {
        gc_PhaseInx=C_MjpegIdle;   
    }
    else
    {
        gc_PhaseInx=C_MjpegProc;
    }
}


void mjpeg_back2uplevel()
{
	if ((gs_System_State.c_Phase == TASK_PHASE_PLAYACT) || (gs_System_State.c_Phase == TASK_PHASE_PAUSE))
	{		
		mjpeg_stop(); //DSP stop cmd	
	}
	
	gc_Task_Next=C_Task_Menu;
	gw_init_needed |= SET_BIT8;
	gc_PhaseInx=0;
}


void MjpegMenu_Process()
{
	data	bit	tb_Confirm=0;
	gc_refresh_type = REFRESH_ALL;
	if(gc_PhaseInx!=2)
	{
        switch(gc_PhaseInx)
        {
            case C_MjpegMenu:
				gc_refresh_type = REFRESH_ALL;
				tb_Confirm = 1;
            break;

            case C_MjpegNext:
				gc_refresh_type = REFRESH_NEXT;
				if(gc_LCMDispIndex==2)
				{
					gc_LCMDispIndex=0;
        		}
				else
				{
					gc_LCMDispIndex++;
				}
				//TFT_MovieMenu(); //--sunzhenk add 100703
				Menu_Disp_Item_Other(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
            break;

            case C_MjpegPrev:
				gc_refresh_type = REFRESH_PREV;
				if(gc_LCMDispIndex==0)
				{
					gc_LCMDispIndex=2;
				}
				else
				{
					gc_LCMDispIndex--;
				}
				//FT_MovieMenu();
				Menu_Disp_Item_Other(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
            break;	
		}
		
		if(tb_Confirm==1) //confirm selected item
		{
			if(gc_LCMDispIndex==0)  //Mjpeg Dir
			{
				mjpeg_stop();
				gc_TaskMode_BkUp=C_Task_Mjpeg;
				gc_Task_Next=C_Task_Dir;
				gc_ShowTimer=0;
				gc_TuneVolFreqStatus=0;
				return;
			}
			else if(gc_LCMDispIndex==1)  //Select Play Mode
			{
				gc_DispPlayMenuAct=1;
				gc_TuneVolFreqStatus=22;
				return;
			}
			else if(gc_LCMDispIndex==2)  //Exit
			{
				gc_ShowTimer=0;
				gc_TuneVolFreqStatus=0;
				gc_PhaseInx=C_MjpegPause;
				return;
			}
		}
		gc_PhaseInx=C_MjpegIdle;
   	}	
}


void RepeatMode_Process_Mjpeg()
{
	data	bit	tb_Confirm=0;

	if(gc_PhaseInx)
	{
        switch(gc_PhaseInx)
        {
            case C_MjpegMenu:
				gc_refresh_type = REFRESH_ALL;
				tb_Confirm=1;
            break;

            case C_MjpegNext:
				gc_refresh_type = REFRESH_NEXT;
				if(gc_LCMDispIndex==2)
				{
					gc_LCMDispIndex=0;
				}
				else
				{
					gc_LCMDispIndex++;
				}
				//TFT_MoviePlayMode();
				Menu_Disp_Item_Other(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
            break;

            case C_MjpegPrev:
				gc_refresh_type = REFRESH_PREV;
				if(gc_LCMDispIndex==0)
				{
					gc_LCMDispIndex=2;
				}
				else
				{
					gc_LCMDispIndex--;
				}
				//TFT_MoviePlayMode();
				Menu_Disp_Item_Other(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
            break;
        }
	
		if(tb_Confirm==1) //enter child dir or confirm selected item
		{
			if(gc_LCMDispIndex==0)
			{
				gc_RepPlayMode=0;
			}
			else if(gc_LCMDispIndex==1)
			{
				gc_RepPlayMode=1;
			}
			else if(gc_LCMDispIndex==2)
			{
				gc_RepPlayMode=2;
			}
			gc_ShowTimer=0;
			gc_TuneVolFreqStatus=0;
			gc_DispPlayMenuAct=0;
			gc_PhaseInx=C_MjpegPause;
			return;
		}
		gc_PhaseInx=C_MjpegIdle;
	}
}	

void LCM_Display_Mjpeg2()
{
	if(gc_TuneVolFreqStatus==1)	// VOLUME ADJ.
	{
		if(gc_DispWallpaper!=2)
		{
			gb_TFT_VOL_FM=0;	// 0=VOL adj.  1=FM adj.
			//TFT_Main_VOL_FM_ADJ();
			Menu_Disp_Item_Play(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
			gc_DispWallpaper=2;
			gc_DisplayVol=0xFF;
		}
		if(gc_DisplayVol!=gs_DSP_GLOBAL_RAM.sw_Volume)
		{
			gc_DisplayVol=gs_DSP_GLOBAL_RAM.sw_Volume;
			TFT_ShowVOLAdj();
		}
	}
	else if(gc_TuneVolFreqStatus==2)	// FM ADJ.
	{
		gc_DispPlayMenuAct=0x12;    //hao.yang add for auto back 090603

		if(gc_DispWallpaper!=3)
		{
			gb_TFT_VOL_FM=1;	// 0=VOL adj.  1=FM adj.
			//TFT_Main_VOL_FM_ADJ();
			Menu_Disp_Item_Play(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
			gc_DispWallpaper=3;
			gw_DisplayFreq=0;
		}

		if(gw_DisplayFreq!=gw_FM_frequency)
		{
			gb_FlashNoFileflag=0;
			gw_DisplayFreq=gw_FM_frequency;
			TFT_ShowFMFreqAdj(); 
		}
	}
	else if(gc_TuneVolFreqStatus==21)	//Mjpeg menu
	{
		gc_ShowTimer=72;
		IR_Service_Process_Menu_1();

		if(gc_DispWallpaper!=22)
		{
			gc_DispWallpaper=22;
			gc_LCMDispIndex=0;
			gc_PhaseInx=0;
			//TFT_MovieMenu(); //--sunzhenk add 100703
			Menu_Disp_Item_Other(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
		}
		MjpegMenu_Process();
	}
	else if(gc_TuneVolFreqStatus==22)	//Mjpeg repeat mode menu
	{
		gc_ShowTimer=72;
		IR_Service_Process_Menu_1();

		if(gc_DispWallpaper!=23)
		{
			gc_DispWallpaper=23;
			gc_LCMDispIndex=0;
			gc_PhaseInx=0;
			//TFT_MoviePlayMode(); //--sunzhenk add 100703
			Menu_Disp_Item_Other(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
		}
		RepeatMode_Process_Mjpeg();
	}	
}

void FM_FREQ_CHGUP_MPEG(void) //chiayen0807
{
	gw_LogData_Timer=60;
    FM_FREQ_CHG(1,1);

	if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        gc_PhaseInx=C_MjpegIdle;   
    }
    else
    {
        gc_PhaseInx=C_MjpegProc;
    }
}

void FM_FREQ_CHGDOWN_MPEG(void) //chiayen0807
{
	gw_LogData_Timer=60;
    FM_FREQ_CHG(0,1);

	if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        gc_PhaseInx=C_MjpegIdle;   
    }
    else
    {
        gc_PhaseInx=C_MjpegProc;
    }
}


void Play_SDFlash_DosInit_Mjpeg()
{
	gb_FindFlag = 0;
	gc_PhaseInx=0;
	gw_init_needed=0xFFFF;

	if (DOS_Initialize())
	{
	}
	else
	{
		gc_ShowTimer=0;  //20090331
		gc_DispWallpaper=1;
		gc_TuneVolFreqStatus=0;
		gb_FindFlag = 0;
		gc_PhaseInx=0;
		gw_init_needed=0xFFFF;

		gc_TaskMode_BkUp=C_Task_Play;//(JC)I0304 for back to rite TASK from Dir-list
		gc_Task_Next=C_Task_Play;

		gs_System_State.c_FileHandle=0;
		gc_MenuItemCount=0;       
		gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;           
		DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);//count music file no. in root
		gc_PhaseInx=0;
		gb_TFT_refresh=1;
		gc_DispWallpaper=0xFF;
	}
}


void Play_SourceDetect_Process_Mjpeg()
{
	gb_SD_Exist_pre=gb_SD_Exist;
	if(!SD_Detect)  //SD exist
    {
		gb_SD_Exist=1;     //SD exist
    }
	else
	{
		gb_SD_Exist=0;
		gb_SDNoFileflag=0;
	}
	
	gb_Host_Exist_pre=gb_Host_Exist;
	if(!Host_DetectDevice())
	{
		gb_Host_Exist=1;	//host_exist		
	}
	else
	{
		gb_Host_Exist=0;
		gc_HostNoFileflag=0;
	}

	if((gc_CurrentCard==0) && ((gb_Host_Exist_pre!=gb_Host_Exist)||(gb_SD_Exist_pre!=gb_SD_Exist)))
	{
		if((gb_Host_Exist_pre!=gb_Host_Exist) && (gb_Host_Exist==1))
		{
			gc_CurrentCard=5;
		}
		else if((gb_SD_Exist_pre!=gb_SD_Exist) && (gb_SD_Exist==1))
		{
			gc_CurrentCard=2;	
		}
		else
		{
			gc_CurrentCard=0;	
		}
	}
	else
	{
		if(gc_CurrentCard==2)
		{
			if(gc_Dosinitfail==0)  //20090803 chiayen add
			{
				if((gb_Host_Exist_pre!=gb_Host_Exist) && (gb_Host_Exist==1))
				{
					gc_CurrentCard=5;
				}
				else if(((gb_SD_Exist==0) || (gb_SDNoFileflag==1)) && (gb_Host_Exist==1))
				{
					gc_CurrentCard=5;
					if(gc_HostNoFileflag==1)
					{
						gc_CurrentCard=0;	
					}
				}
				else if((gb_SD_Exist==0) || (gb_SDNoFileflag==1))
				{
					gc_CurrentCard=0;	
				}
			}
			else //20090803 chiayen add for SD Dos initial fail
			{
				if((gb_Host_Exist==1) && (gc_HostNoFileflag==0))
				{
					gc_CurrentCard=5;
				}
				else
				{
					gc_CurrentCard=0;
				}
				gc_Dosinitfail=0;				
			}
		}
		if(gc_CurrentCard==5)
		{
			if(gc_Dosinitfail==0)  //20090803 chiayen add
			{
				if((gb_SD_Exist_pre!=gb_SD_Exist) && (gb_SD_Exist==1))
				{
					gc_CurrentCard=2;
				}
				else if(((gb_Host_Exist==0) || (gc_HostNoFileflag==1)) && (gb_SD_Exist==1))
				{
					gc_CurrentCard=2;
					if(gb_SDNoFileflag==1)
					{
						gc_CurrentCard=0;	
					}
				}
				else if((gb_Host_Exist==0) || (gc_HostNoFileflag==1))
				{
					gc_CurrentCard=0;	
				}
			}
			else //host dos initial fail  //20090803 chiayen add
			{
				if((gb_SD_Exist==1) && (gb_SDNoFileflag==0))
				{
					gc_CurrentCard=2;
				}
				else
				{
					gc_CurrentCard=0;
				}
				gc_Dosinitfail=0;
			}
		}
	}

    if(gc_CurrentCard_backup!=gc_CurrentCard)
    {
		U8 tc_CurrentCard;
		tc_CurrentCard=gc_CurrentCard;
		gc_CurrentCard=gc_CurrentCard_backup;
		mjpeg_stop();
		gc_MenuItemCount=0;
		gc_CurrentCard=tc_CurrentCard;
		gc_CurrentCard_backup=gc_CurrentCard;

		set_clock_mode(CLOCK_MODE_MP3); //20090803 chiayen add for media change

		if(gc_CurrentCard==2)
        {           
            if(SD_Identification_Flow())
            {
                gc_CardExist |=0x02;
            }
            else if((gb_Host_Exist==1) && (gc_HostNoFileflag==0)) //20090730 chiayen modify
            {
				DEVICE_REG[0x00]= 0x01;  //20090730 chiayen add
				if(!Host_Initial())
				{
					gc_CurrentCard = CURRENT_MEDIA_HOST;
					gb_FindFlag = 0;
            	}
	            else
	            {
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;
					InitFlash();				    
	            }
        	}
			else 
        	{
				DEVICE_REG[0x00]= 0x01;  //20090730 chiayen add
				gc_CurrentCard=0;
				gc_CardExist &=0xFD;
				gb_FindFlag = 0;
				InitFlash();
			}
			Play_SDFlash_DosInit_Mjpeg();
        }
		#if 1//(USB_HOST==1)
		else if(gc_CurrentCard==CURRENT_MEDIA_HOST)
		{
			DEVICE_REG[0x00]= 0x01;  //20090803 chiayen add			
			if(!Host_Initial())
			{
				gc_CurrentCard = CURRENT_MEDIA_HOST;
				gb_FindFlag = 0;
			}
			else if((gb_SD_Exist==1) && (gb_SDNoFileflag==0)) //20090730 chiayen add
			{
	            if(SD_Identification_Flow())
	            { 
	                gc_CardExist |=0x02;
					gc_CurrentCard=2;      
				}
				else
				{
					DEVICE_REG[0x00]= 0x01;
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;
					InitFlash();
				}
			}
			else 
        	{
				DEVICE_REG[0x00]= 0x01;
				gc_CurrentCard=0;
				gb_FindFlag = 0;
				InitFlash();
			}
			Play_SDFlash_DosInit_Mjpeg();
		}
		else if(gc_CurrentCard==0)
        {
			DEVICE_REG[0x00]= 0x01;
			gc_CardExist &=0xFD;
			InitFlash();
			Play_SDFlash_DosInit_Mjpeg();
		}
		#endif

		#if 0
		if(gc_CurrentCard==2)
        {           
            if(SD_Identification_Flow())
            {
                gc_CardExist |=0x02;
				Play_SDFlash_DosInit_Mjpeg();      
            }
            else
            {
				XBYTE[0xB400]= 0x01;
				gc_CurrentCard=0;
				gc_CardExist &=0xFD;
				gb_FindFlag = 0;
				InitFlash();
				Play_SDFlash_DosInit_Mjpeg();					    
            }
        }
        else if(gc_CurrentCard==0)
        {
			XBYTE[0xB400]= 0x01;
			gc_CardExist &=0xFD;
			InitFlash();
			Play_SDFlash_DosInit_Mjpeg();
        }
		else if(gc_CurrentCard==CURRENT_MEDIA_HOST)
		{
			set_clock_mode(CLOCK_MODE_MP3);
			if(!Host_Initial())
			{
				gc_CurrentCard = CURRENT_MEDIA_HOST;
				gb_FindFlag = 0;
			}
			else
			{
				XBYTE[0xB400]= 0x01;
				gc_CurrentCard=0;
				gb_FindFlag = 0;
				InitFlash();
				gc_CurrentCard=0;
			}
			Play_SDFlash_DosInit_Mjpeg();
		}
		#endif
    }	
}


void Mjpeg_Task()
{
	LCD_BACKLIGHT_ON;
	XBYTE[0xB046]&=~0x04;

	while (1)
	{
		switch(gc_PhaseInx)
		{
			case C_MjpegIdle:
				mjpeg_idle();
			break;

			case C_MjpegInit:
				mjpeg_init();
			break;	
		
			case C_MjpegProc:
				mjpeg_proc();
			break;
			
			case C_MjpegPause:
				if(gc_ShowTimer==0)
				{
					gc_DispWallpaper=1;
					mjpeg_playpause();
				}
				else
				{
					gc_PhaseInx=C_MjpegIdle;	
				}
			break;

			case C_MjpegStop:
				mjpeg_stop();
			break;

			case C_MjpegEndfile:
				mjpeg_endfile();	
			break;

			case C_MjpegNext:
				if(gc_TuneVolFreqStatus==0)  //20090331
				{
					mjpeg_next();
				}
				else if(gc_TuneVolFreqStatus==1)  //20090331
				{
					gc_ShowTimer=72;
					if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
					{
						mjpeg_volup();
					}
					else
					{
						if (gs_DSP_GLOBAL_RAM.sw_Volume<50)
					    {
					        gs_DSP_GLOBAL_RAM.sw_Volume+=2;
					    }
						gc_PhaseInx=C_MjpegIdle;
					}
				}
				else if(gc_TuneVolFreqStatus==2)
				{
					FM_FREQ_CHGUP_MPEG();	
				}
			break;

			case C_MjpegPrev:
				if(gc_TuneVolFreqStatus==0)  //20090331
				{
					mjpeg_prev();
				}
				else if(gc_TuneVolFreqStatus==1)  //20090331
				{
					gc_ShowTimer=72;
					if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
					{
						mjpeg_voldn();
					}
					else
					{
					    if (gs_DSP_GLOBAL_RAM.sw_Volume>1)
					    {
					        gs_DSP_GLOBAL_RAM.sw_Volume-=2;
					    }
						gc_PhaseInx=C_MjpegIdle;
					}
				}
				else if(gc_TuneVolFreqStatus==2)
				{
					FM_FREQ_CHGDOWN_MPEG();	
				}
			break;

			case C_MjpegFF:
				mjpeg_fastfwd();
			break;

			case C_MjpegFR:
				mjpeg_fastrev();
			break;

			case C_MjpegFfFrEnd:
				mjpeg_fffr_end();
			break;

			case C_MjpegVolUp:
			//	mjpeg_volup();
				if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
					mjpeg_playpause();
				}
					gc_TuneVolFreqStatus=1;
					gc_DispPlayMenuAct=0x11;
					gc_ShowTimer=72;
					if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
					{
				mjpeg_volup();
					}
				
			break;

			case C_MjpegVolDn:
				//mjpeg_voldn();
				 if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
					mjpeg_playpause();
				}
					gc_TuneVolFreqStatus=1;
					gc_DispPlayMenuAct=0x11;
					gc_ShowTimer=72;
					if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
					{
				mjpeg_voldn();
					}
					
			break;
			case C_MjpegChgUp:
				if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
					mjpeg_playpause();
				}
					gc_TuneVolFreqStatus=2;
					gc_DispPlayMenuAct=0x11;
					gc_ShowTimer=72;
					if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
					{
						FM_FREQ_CHGUP_MPEG();
					}
			break;
			case C_MjpegChgDn:
					if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
					{
						mjpeg_playpause();
					}
					gc_TuneVolFreqStatus=2;
					gc_DispPlayMenuAct=0x11;
					gc_ShowTimer=72;
					if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
					{
						FM_FREQ_CHGDOWN_MPEG();
					}
			break;

			case C_MjpegUpLevel:
				mjpeg_back2uplevel();
			break;

			case C_TuneVolFreq:  //20090331
				if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
					mjpeg_playpause();
				}
				
				gc_DispPlayMenuAct=0x11;    

				if(gc_TuneVolFreqStatus<3)
				{
					gc_ShowTimer=72;
					gc_TuneVolFreqStatus++;
					if(gc_TuneVolFreqStatus>2)
					{
						gc_TuneVolFreqStatus=0;
						gc_ShowTimer=0;
						gc_PhaseInx=C_MjpegPause;
					}
				}

				if(gs_System_State.c_Phase==TASK_PHASE_STOP)
			    {
			        gc_PhaseInx=C_MjpegIdle;   
			    }
			    else
			    {
			        gc_PhaseInx=C_MjpegProc;
			    }
			break;

			case C_MjpegMenu:  //20090331
				if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
				{
					mjpeg_playpause();
				}

 		    	gc_ShowTimer=72;
				if((gc_DispPlayMenuAct==0) || (gc_DispPlayMenuAct==0x11))
				{
					gc_TuneVolFreqStatus=21;
				}					
			break;

            case C_PowerOff:
				if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
				{
					mjpeg_stop();
				}
				gc_Task_Next=C_Task_PowerOff;  					    	
            break;

			case C_MainMenu:  //Long Key Mode
				mjpeg_stop();
				gc_Task_Next=C_Task_Menu;
			break;
		}

		if((gc_TuneVolFreqStatus!=0) && (gc_ShowTimer!=0))
		{
			LCM_Display_Mjpeg2();		
		}
		else
		{
			if((gc_DispPlayMenuAct!=0)&&(gs_System_State.c_Phase==TASK_PHASE_PAUSE))
			{
				gc_DispPlayMenuAct=0;
				gc_PhaseInx=C_MjpegPause;				
			}
		}

		Polling_TaskEvents();

        if(gc_LogDataFlag==1)
        {
            USER_LogFile_ReadWrite(1);
            gc_LogDataFlag=0;
        }

		if(gc_PhaseInx==C_MjpegProc)
		{
			mjpeg_proc();	
		}

		if((gc_TuneVolFreqStatus!=21) && (gc_TuneVolFreqStatus!=22))
		{
			IR_Service_Process();
		}

		Play_SourceDetect_Process_Mjpeg();

		if (gc_Task_Current != gc_Task_Next)
		{
			gc_Task_Current = gc_Task_Next;
			break;	
		}
	}
}