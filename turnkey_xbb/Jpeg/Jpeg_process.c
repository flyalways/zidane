#include <string.h>
#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "dos\dosfile.h"
#include "memalloc.h"
#include "..\LCM\model_define.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\LCM\LCM.h"
#include "..\LCM\UI_icon.h"
#include "..\LCM\TFT_display.h"
#include "..\LCM\TFT_config.h"
#include "..\header\variables.h"
#include "..\header\host_init.h"

extern data U8 gc_PhaseInx;
extern 	data U8 gc_KeyDet_Mask;  //20090107 chiayen modify
extern xdata System_Struct gs_System_State;
extern U8  xdata EXT_NameC[];
extern xdata U8  gc_Play_FileType;
extern	xdata	U8	gc_LCMDispIndexBak;
extern	xdata	U16	gc_DispFilePageBak;

extern	void	LCM_set_view(U8 view_type, U16 x_start, U16 y_start, U16 x_size, U16 y_size);
extern	void	IR_Service_Process_Menu_2();
extern	void	IR_Service_Process();
extern	void	ir_service_menu(void);

U8 jpeg_marker_parser(U32 *tpdw_File_Ptr, U8 tw_marker_Hbyte, U8 tw_marker_Lbyte);     //Jimi 090305

xdata	U8	gc_JPGSetting;
xdata	U8	gc_JpegVelocityBak;
extern	xdata	U8	gc_MenuItemCount;
extern	data	bit	gb_TFT_refresh;

/* =============== Image function =============== */
void Image_WakeUp(U8 tc_Type)
{	
	DSP_WakeUp(tc_Type);
}

U8 Image_Download(void)
{
	U8 tc_Cnt = 0;

 	if (DSP_Download(0x08, 0x10) != DSP_SUCCESS)
		return DSP_PLAY_COMMAND_ERROR;

	for (tc_Cnt=0; tc_Cnt<10; ++tc_Cnt) {
		if (DSP_PlayInit() == DSP_SUCCESS)
			break;
//		dbprintf("Retry to start DSP\n");
		if (tc_Cnt < 5) 
		{
			//GLOBAL_REG[0x02] |= 0x01;		// DSP reset, ycc mark
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
		else
		{
//			dbprintf("Hardware Reset DSP!!!!!\n");
			GLOBAL_REG[0x02] |= 0x01;		// DSP reset
			GLOBAL_REG[0x02] &= 0xFE;		// Enable DSP Run Normal Mode
		}
	}
	if (tc_Cnt == 10)
		return DSP_PLAY_COMMAND_ERROR;

	return DSP_SUCCESS;
}

U8 Image_PlayCmd(void)
{
	// sent "PLAY" command to DSP
	if (L2_DSP_SendCommandSet(DCMD_Play) != DCMD_Play)
	{
		return DSP_PLAY_COMMAND_ERROR;
	}
//	dbprintf("Play Cmd Ok~\n");

	gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;		// reset bitstream buffer index
	gs_DSP_GLOBAL_RAM.sc_Pixel_Index = 0;	// reset output pixel buffer index
	gs_DSP_GLOBAL_RAM.sc_End_Of_File = 0;	// reset file end flag

	// JPEG file open
	if (DOS_Open_File_r(C_OtherFHandle, C_Open_FoundFile, C_NullFileName))
		return DSP_PLAY_COMMAND_ERROR;

	return DSP_SUCCESS;
}


U8 Image_DataIn(void)
{
	if (!gs_DSP_GLOBAL_RAM.sc_End_Of_File)
	{
         // Jimi 090305 
		if(gc_Play_FileType != 1)
		{
			while(L2_DSP_Read_DMem16(DSP_EmptyBuffer) > ONE_SECTOR)
			{
				if (DOS_Read_File(C_OtherFHandle) != DOS_END_OF_FILE)
				{
                    L2_DSP_MCU_DM_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index++, 0x2E, (U16)gc_PlayRecordDataBuf, DMA_24BIT_MODE);

					if (L2_DSP_SendCommandSet(DCMD_DatIn) != DCMD_DatIn)
					{
//						dbprintf("DSP_DATAIN_COMMAND_ERROR\n");
						return DSP_DATAIN_COMMAND_ERROR;
					}
		
					if (gs_DSP_GLOBAL_RAM.sc_DM_Index >= 3)	// JPEG bitstream buffer 0x3400~0x3600, sz = 1536 bytes
						gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;
				}
				else
				{
//					dbprintf("file-end\n");
					L2_DSP_Write_DMem16(0x3F44, 0x00FF);	// JPEG file end flag
					gs_DSP_GLOBAL_RAM.sc_End_Of_File = 1;
					break;
				}
			}
		}
		
		else
		{
			U8 i;
			for(i=0; i<5; i++)
			{
				if(L2_DSP_Read_DMem16(DSP_EmptyBuffer) > ONE_SECTOR)
				{
					if (DOS_Read_File(C_OtherFHandle) != DOS_END_OF_FILE)
					{
                        L2_DSP_MCU_DM_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index++, 0x2E, (U16)gc_PlayRecordDataBuf, DMA_24BIT_MODE);
						if (L2_DSP_SendCommandSet(DCMD_DatIn) != DCMD_DatIn)
						{
//							dbprintf("DSP_DATAIN_COMMAND_ERROR\n");
							return DSP_DATAIN_COMMAND_ERROR;
						}
			
						if (gs_DSP_GLOBAL_RAM.sc_DM_Index >= 3)	// JPEG bitstream buffer 0x3400~0x3600, sz = 1536 bytes
							gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;
					}
					else
					{
//						dbprintf("file-end\n");
						L2_DSP_Write_DMem16(0x3F44, 0x00FF);	// JPEG file end flag
						gs_DSP_GLOBAL_RAM.sc_End_Of_File = 1;
						return  DSP_DECODE_END;	// 20100412 - BMP error
					}
				}
			}
		}
	}
	return	DSP_SUCCESS;
}


void Image_MCU_LCM_DMA(U16 movesize, U16 tw_Src)
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


U8 Image_DataOut(void)
{
	U16 RemData = L2_DSP_Read_DMem16(DSP_SOutBufRemData);

	if (RemData >= ONE_SECTOR)
	{
		do
		{
			L2_DSP_DM_MCU_DMA(gs_DSP_GLOBAL_RAM.sc_Pixel_Index++, 9, (U16)gc_PlayRecordDataBuf, DMA_16BIT_MODE);
			if (L2_DSP_SendCommandSet(DCMD_DatOut) != DCMD_DatOut)
			{
//				dbprintf("DSP_DATAOUT_COMMAND_ERROR\n");
				return DSP_DATAOUT_COMMAND_ERROR;
			}

            if (gs_DSP_GLOBAL_RAM.sc_Pixel_Index >= 10) // JPEG output pixel buffer 0x3000~0x3A00, sz = 2560*2 bytes
				gs_DSP_GLOBAL_RAM.sc_Pixel_Index = 0;
		
			/* ==== Display on LCM: DMA Sram to LCM ==== */
			Image_MCU_LCM_DMA(ONE_SECTOR, (U16)gc_PlayRecordDataBuf);
		} while (L2_DSP_Read_DMem16(DSP_SOutBufRemData) >= ONE_SECTOR);
	}
	else if (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)
	{
		if (RemData > 0)
		{
			L2_DSP_DM_MCU_DMA(gs_DSP_GLOBAL_RAM.sc_Pixel_Index, 9, (U16)gc_PlayRecordDataBuf, DMA_16BIT_MODE);
			if (L2_DSP_SendCommandSet(DCMD_DatOut) != DCMD_DatOut)
			{
//				dbprintf("DSP_DATAOUT_COMMAND_ERROR\n");
				return DSP_DATAOUT_COMMAND_ERROR;
			}

			/* ==== Display on LCM: DMA Sram to LCM ==== */
			Image_MCU_LCM_DMA(RemData, (U16)gc_PlayRecordDataBuf);
		}
//		dbprintf("JPEG DSP decode end!!\n");
		return DSP_DECODE_END;
	}
	else if (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x0020)
	{
//		dbprintf("JPEG error format!!\n");
		return DSP_USER_FILE_TYPE_ERROR;
	}

	return DSP_SUCCESS;
}

U8 Image_StopCmd(U8 tc_FileHandle)
{
	U8 tc_Ret, tc_state;
	U16 TimeOUT;
	
	tc_Ret = DSP_StopCmd();
	TimeOUT = 0xffff;
	while (1)
	{
		TimeOUT--;
		if (TimeOUT == 0)
		{
//			dbprintf("JPEG stop command time out\n");
			tc_Ret = DSP_DECODE_STATUS_TIMEOUT_ERROR;
			break;
		}

		tc_state = Image_DataIn();
		if (tc_state == DSP_DATAIN_COMMAND_ERROR)
		{
			tc_Ret = tc_state;
			break;
		}

		tc_state = Image_DataOut();

		if (tc_state == DSP_DATAOUT_COMMAND_ERROR)
		{
			tc_Ret = tc_state;
			break;
		}
		else if (tc_state == DSP_USER_FILE_TYPE_ERROR)
		{
			tc_Ret = tc_state;
			L2_DSP_Write_DMem16(DSP_DecodeStatus, (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0xFFDF));
			break;
		}
		else if (tc_state == DSP_DECODE_END)
		{
			L2_DSP_Write_DMem16(DSP_DecodeStatus, (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0xFF3F)); 
			break;
		}
	}

	// Close current file
//	dbprintf("JPEG DOS close file\n");
	gw_AutoJpeg_Timer=(U16)(gc_JpegVelocity+1)*36;
	DOS_Close_File_r(tc_FileHandle);

	return tc_Ret;
}

/* =============== GIF function =============== */
U8 GIF_DataIn(void)
{
	if (L2_DSP_Read_DMem16(DSP_EmptyBuffer) >= ONE_SECTOR)
	{
		if (DOS_Read_File(C_OtherFHandle) != DOS_END_OF_FILE)
		{
            L2_DSP_MCU_DM_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index++, 0x2E, (U16)gc_PlayRecordDataBuf, DMA_24BIT_MODE);
			if (L2_DSP_SendCommandSet(DCMD_DatIn) != DCMD_DatIn)
			{
//				dbprintf("DSP_DATAIN_COMMAND_ERROR\n");
				return DSP_DATAIN_COMMAND_ERROR;
			}
	
			if (gs_DSP_GLOBAL_RAM.sc_DM_Index >= 3)	// JPEG bitstream buffer 0x3400~0x3600, sz = 1536 bytes
				gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;
		}
		else
		{
//			dbprintf("Re-start GIF file\n");
			gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
			gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = DOS_Seek_File(gs_System_State.c_FileHandle, 0);
		}

	}
	return	DSP_SUCCESS;
}


void GIF_DM_MCU_DMA(U16 BufferIndex,U8 tc_PowerOf2, U16 tw_Src)
{
	U8  tc_DSPAddr_High;
	U16 tw_MoveSize;

	// ----- DMA Reset -----
	DMA_REG[0x04] = 0x09;
	DMA_REG[0x04] = 0x08;
	DMA_REG[0xC0] = 0x00;		// clear DMA complete
	DMA_REG[0x30] = 0x01;		// clear checksum		
		
	// ----- DMA initial settings for DM transfer -----		
	DSP_REG[0x16] = 0x00;		// Remain DSP mode.  When you do MCU -> DM, MCU will stop DSP automaticlly
	DMA_REG[0x01] = 0x05;		// DM -> SRAM

	// Define data size in DMA		
	tw_MoveSize = (1 << tc_PowerOf2) - 1;
	DMA_REG[0x02] = ((U8 *)(&tw_MoveSize))[1];
	DMA_REG[0x03] = ((U8 *)(&tw_MoveSize))[0];

	// Source
	DMA_REG[0x40] = 0x01;		// 16-bit DMA
	DSP_REG[0x1D] = 0x01;
	tc_DSPAddr_High = (U8)(((BufferIndex << tc_PowerOf2) >> 1) >> 8);
    DSP_REG[0x18] = tc_DSPAddr_High + 0x2D;     // GIF output pixel start address : 0x3700
	DSP_REG[0x17] = (U8)((BufferIndex << tc_PowerOf2) >> 1);

	// Destination
	CPU_REG[0x14] = (U8)(tw_Src);
	CPU_REG[0x15] = (U8)(tw_Src >> 8);

	// ------------------------------------------------
	DMA_REG[0xB0] = 0x01;		// DMA1 start
	// wait until DMA cycle is completed (0xB3C0)		
	while (!(DMA_REG[0xC0]&0x01));
	DMA_REG[0xC0] = 0x00;
}


U8 GIF_DataOut(void)
{
	U16 RemData = L2_DSP_Read_DMem16(DSP_SOutBufRemData);
	if (RemData >= 256)
	{
		GIF_DM_MCU_DMA(gs_DSP_GLOBAL_RAM.sc_Pixel_Index++, 8, (U16)gc_PlayRecordDataBuf);
		if (L2_DSP_SendCommandSet(DCMD_DatOut) != DCMD_DatOut)
		{
//			dbprintf("DSP_DATAOUT_COMMAND_ERROR\n");
			return DSP_DATAOUT_COMMAND_ERROR;
		}

		if (gs_DSP_GLOBAL_RAM.sc_Pixel_Index >= 2) // GIF output pixel buffer 0x3700~0x37FF, sz = 256*2 bytes
			gs_DSP_GLOBAL_RAM.sc_Pixel_Index = 0;

		/* ==== Display on LCM: DMA Sram to LCM ==== */
		Image_MCU_LCM_DMA(256, (U16)gc_PlayRecordDataBuf);
	}
	else if (L2_DSP_Read_DMem16(0x3F8A) == 49)
	{
		if (RemData > 0)
		{
			GIF_DM_MCU_DMA(gs_DSP_GLOBAL_RAM.sc_Pixel_Index++, 8, (U16)gc_PlayRecordDataBuf);
			if (L2_DSP_SendCommandSet(DCMD_DatOut) != DCMD_DatOut)
			{
//				dbprintf("DSP_DATAOUT_COMMAND_ERROR\n");
				return DSP_DATAOUT_COMMAND_ERROR;
			}
			/* ==== Display on LCM: DMA Sram to LCM ==== */
			Image_MCU_LCM_DMA(RemData, (U16)gc_PlayRecordDataBuf);
		}
		gs_DSP_GLOBAL_RAM.sc_Pixel_Index = 0;
		L2_DSP_Write_DMem16(0x3F8A, 0);
	}
	else if (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x0020)
	{
//		dbprintf("GIF error format!!\n");
		//L2_DSP_Write_DMem16(DSP_DecodeStatus, (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0xFFDF));
		return DSP_USER_FILE_TYPE_ERROR;
	}

	return DSP_SUCCESS;
}


U8 GIF_StopCmd(U8 tc_FileHandle)
{
	U8 tc_Ret, tc_state;
	U16 TimeOUT;
	
	tc_Ret = DSP_StopCmd();

	if (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x0020)	// reset DSP_DecodeStatus
		L2_DSP_Write_DMem16(DSP_DecodeStatus, (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0xFFDF));

	TimeOUT = 0xffff;
	while (1)
	{
		TimeOUT--;
		if (TimeOUT == 0)
		{
//			dbprintf("GIF stop command time out\n");
			tc_Ret = DSP_DECODE_STATUS_TIMEOUT_ERROR;
			break;
		}

		tc_state = GIF_DataIn();
		if (tc_state == DSP_DATAIN_COMMAND_ERROR)
		{
			tc_Ret = tc_state;
			break;
		}

		tc_state = GIF_DataOut();
		if (tc_state == DSP_DATAOUT_COMMAND_ERROR)
		{
			tc_Ret = tc_state;
			break;
		}
		else if (tc_state == DSP_USER_FILE_TYPE_ERROR)
		{
			tc_Ret = tc_state;
			L2_DSP_Write_DMem16(DSP_DecodeStatus, (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0xFFDF));
			break;
		}

		if (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)	//wait file end
		{
//			dbprintf("GIF DSP decode end!!\n");
			L2_DSP_Write_DMem16(DSP_DecodeStatus, (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0xFF3F)); 
			break;
		}
	}		

	// Close current file
//	dbprintf("GIF DOS close file\n");
	gw_AutoJpeg_Timer=(U16)(gc_JpegVelocity+1)*36;
	DOS_Close_File_r(tc_FileHandle);

	return tc_Ret;
}

//=================(JC)Jpeg ctrl flow====================
void jpeg_idle()
{

}

void jpeg_init()
{	
	if(gw_init_needed & SET_BIT4)
	{
//		dbprintf("=jpeg init=\n");
		gc_DispFilePageBak=0xFFFF;
		gw_init_needed&=CLR_BIT4;
		
		gs_System_State.c_Phase = TASK_PHASE_STOP;
		gs_DSP_GLOBAL_RAM.sw_Volume = 42;//(JC)init value


		gs_System_State.c_FileHandle = 2;
		gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_StartCluster = gdw_DOS_RootDirClus;

		EXT_NameC[0] = 3;
		EXT_NameC[1] = 'J';
		EXT_NameC[2] = 'P';
		EXT_NameC[3] = 'G';
		EXT_NameC[4] = 'B';
		EXT_NameC[5] = 'M';
		EXT_NameC[6] = 'P';
		EXT_NameC[7] = 'G';
		EXT_NameC[8] = 'I';
		EXT_NameC[9] = 'F';
		DOS_Search_File(C_File_All|C_Cnt_FileNo,C_OtherFileType,C_CmpExtName|C_Next);

		if(DOS_Search_File(C_File_All|C_By_FDB, C_OtherFHandle, C_CmpExtName|C_Next))
		//if(gw_FileTotalNumber[2]==0)
		{
//			dbprintf("!No picture files!\n");
			gw_init_needed |= SET_BIT8;
			gc_Task_Next=C_Task_Menu;
			gc_PhaseInx = 0;
			return;
		}
		gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;
//		set_clock_mode(CLOCK_MODE_JPEG);	// removed by maxliao 
	}
	gc_PhaseInx = C_JpegPause; //Ching 090316
}

void jpeg_proc()
{
	gc_KeyDet_Mask = 0;

	if (gc_Play_FileType == 2)	// test
	{
		if (GIF_DataIn() == DSP_DATAIN_COMMAND_ERROR)
		{
//			dbprintf("GIF data in end\n");
			gc_PhaseInx = 4;
		}
		
		if (GIF_DataOut() != DSP_SUCCESS )
		{	// "error format" -> set stop command
//			dbprintf("GIF data out end\n");
			gc_PhaseInx = 4;
		}
	}
	else
	{
        if (Image_DataIn() != DSP_SUCCESS)	// 20100412 - BMP error
		{
//			dbprintf("JPEG data in end\n");
			gc_PhaseInx = 4;
		}
		
		if (Image_DataOut() != DSP_SUCCESS )
		{	// "file end" or "error format" -> set stop command
//			dbprintf("JPEG data out end\n");
			gc_PhaseInx = 4;
		}
	}
}

void jpeg_playpause()
{
	U16	i;	

	if (gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
   	{
//		dbprintf("Keep decoding...\n");
   	}
	else
	{
//		dbprintf("=play jpeg=\n");
		
		//jpeg init
		//clock setting
//		set_clock_mode(CLOCK_MODE_JPEG);

		//Draw a black frame
		for (i=0; i<512; i++)
		{
			((U32 *)gc_MotionJpegOutBuf)[i] = 0;
		}

		LCM_set_view(TFT_BMPDispDir, 0, 0, TFT_H_Size, TFT_V_Size); // vertical view, Jimi 090305

		for (i=0; i<20; i++)		//Jimi 090305 for 128x160 LCM
		{
			Image_MCU_LCM_DMA(2048, (U16)gc_MotionJpegOutBuf);
		}

		Image_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);

		if (Image_Download())	// Load IM, PM
		{
//			dbprintf("load IMPM err!\n");
		}

//		dbprintf("gc_Play_FileType = %bx\n",gc_Play_FileType);     //Jimi 090305
		//Set file type, 0: SCV, 1: JPEG, 2: BMP, 3: GIF, 4: AMV
		if (gc_Play_FileType == 0)
		{
			L2_DSP_Write_DMem16(0x3F2F, 0x0001);	// JPEG file
			/* Jimi 090305 , JPEG parser to get image height & length */
			{
				U32 tdw_File_Ptr;
				U16 tw_Buf_idx;
				U16 tw_img_height=0;
				U16 tw_img_width=0;
				U16 tw_rem_data;
				U8 tc_ret;
				U8 tc_SOF_Buf[7];
			 	//Open File
				if(DOS_Open_File_r(C_OtherFHandle, C_Open_FoundFile, C_NullFileName) || (gs_File_FCB[0].dw_File_TotalSize==0)) 
				{
					//gs_System_State.w_BitRate=0;
//					dbprintf("open file err\n");	
				}
       		  	//Read File
				gb_ReadWriteDataArea = 0;		//select gc_PlayRecordDataBuf[]
				DOS_Read_File(C_OtherFHandle);

				tdw_File_Ptr = 0;
				//find SOI marker
				tc_ret = jpeg_marker_parser(&tdw_File_Ptr, 0xFF, 0xD8);
				if(!tc_ret)
				{
//					dbprintf("SOI File Pointer = %lx\n", tdw_File_Ptr);

					//find SOF marker
					tc_ret = jpeg_marker_parser(&tdw_File_Ptr, 0xFF, 0xC0);			
					if(!tc_ret)
					{
//						dbprintf("SOF File Pointer = %lx\n", tdw_File_Ptr);
					
						tw_Buf_idx = tdw_File_Ptr & 0x1FF;
						if(tw_Buf_idx ==0)
						{
							DOS_Read_File(C_OtherFHandle);
						}

						tw_rem_data = 512 - tw_Buf_idx;
						if(tw_rem_data >= 7)
						{
							memcpy(tc_SOF_Buf, &gc_PlayRecordDataBuf[tw_Buf_idx], 7);
						}
						else
						{
							memcpy(tc_SOF_Buf, &gc_PlayRecordDataBuf[tw_Buf_idx], tw_rem_data);
							tw_Buf_idx = 0;
							DOS_Read_File(C_OtherFHandle);
							memcpy(&tc_SOF_Buf[tw_rem_data], &gc_PlayRecordDataBuf[0], (7-tw_rem_data));
						}	

						((U8 *)(&tw_img_height))[0] = tc_SOF_Buf[3];
						((U8 *)(&tw_img_height))[1] = tc_SOF_Buf[4];
//						dbprintf("tw_img_height = %x\n", tw_img_height);
						((U8 *)(&tw_img_width))[0] = tc_SOF_Buf[5];
						((U8 *)(&tw_img_width))[1] = tc_SOF_Buf[6];
//						dbprintf("tw_img_width = %x\n", tw_img_width);
					}

					{
						// horizontal view
						LCM_set_view(/*TFT_JPGDispDir*/1, 0, 0, TFT_H_Size, TFT_V_Size); // horizontal view
						L2_DSP_Write_DMem16(0x3F40, TFT_H_Size);		  // Set width
						L2_DSP_Write_DMem16(0x3F41, TFT_V_Size);			  // Set height
					}

					L2_DSP_Write_DMem16(0x3F48, 0x0000);				// Set background color
				}
			}
		}
		else if (gc_Play_FileType == 1)
		{
			L2_DSP_Write_DMem16(0x3F2F, 0x0002);	// BMP file
			LCM_set_view(TFT_BMPDispDir, 0, 0, TFT_H_Size, TFT_V_Size); // horizontal view in BMP order
		}
		else
		{
			L2_DSP_Write_DMem16(0x3F2F, 0x0003);	// GIF file
		}

        //DSP_Write_MMR16(0x3FD4, DSP_Read_MMR16(0x3FD4)|0x18);  //20090324 jimi add (set DSP Jpeg hardware clock) 
		if (Image_PlayCmd() == DSP_PLAY_COMMAND_ERROR)
		{
//			dbprintf("SYS play init fail\n");
		}
		
		if (gc_Play_FileType == 2)
		{	// Set GIF display range
			U16 tw_width, tw_height, tw_x_start, tw_y_start;

			if (GIF_DataIn() == DSP_DATAIN_COMMAND_ERROR)
			{
//				dbprintf("Data in command error\n");
				gs_System_State.c_Phase = TASK_PHASE_PLAYACT;
				gc_PhaseInx = 4;
				gc_KeyDet_Mask = 0;
				return;
			}

			while (L2_DSP_Read_DMem16(0x3F5E) == 0)
			{
				if (L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x0020)	// GIF error format
				{
//					dbprintf("GIF error format!!\n");
					gs_System_State.c_Phase = TASK_PHASE_PLAYACT;
					gc_PhaseInx = 4;
					gc_KeyDet_Mask = 0;
					return;
				}
			}
			tw_width = L2_DSP_Read_DMem16(0x3F5D);
			tw_height = L2_DSP_Read_DMem16(0x3F5E);
//			dbprintf("width = %x, height = %x\n", tw_width, tw_height);
			//Jimi 090305
			tw_x_start = (TFT_H_Size - tw_width) >> 1;
			tw_y_start = (TFT_V_Size - tw_height) >> 1;
			LCM_set_view(TFT_IconDispDir, tw_x_start, tw_y_start, tw_width, tw_height); // horizontal view in normal order
		}
		//Jimi 090305
		else if(gc_Play_FileType == 1)
		{
			/* ===== Setup DSP parameter ===== */
			L2_DSP_Write_DMem16(0x3F40, TFT_H_Size);		// Set width  // horizontal view in normal order
			L2_DSP_Write_DMem16(0x3F41, TFT_V_Size);		// Set height
			L2_DSP_Write_DMem16(0x3F48, 0x0000);				// Set background color
		}

		gs_System_State.c_Phase = TASK_PHASE_PLAYACT;
   	}
   	gc_PhaseInx = 2;
}

void jpeg_stop()
{
	U8 tc_Ret;

//	dbprintf("jpeg_stop!\n");
	if (gs_System_State.c_Phase != TASK_PHASE_STOP)
	{			
		//jpeg stop
		if (gc_Play_FileType == 2)
		{
			tc_Ret = GIF_StopCmd(gs_System_State.c_FileHandle);
		}
		else
		{
			tc_Ret = Image_StopCmd(gs_System_State.c_FileHandle);
		}

		if (tc_Ret != DSP_SUCCESS)
		{
//			dbprintf("JPEG stop error !!\n");
		}

		gs_System_State.c_Phase = TASK_PHASE_STOP;
	}
	gc_PhaseInx = 0;	//Jimi 090305

}


void jpeg_endfile()
{
	//stop DSP 'n close file
	jpeg_stop();
}


void jpeg_next()
{

//	dbprintf("jpeg next!\n");

	if (gs_System_State.c_Phase == TASK_PHASE_STOP)
	{
		if (gw_FileIndex[gs_System_State.c_FileHandle] == 0)
		{
			return;//if no this line,error will happen
		}

		gc_PhaseInx = 3;
	}
	else if (gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
	{
		gc_PhaseInx = 3;
		gc_KeyDet_Mask = 1;//(JC)H0508 avoid key-press to change phase index	
	}
	/*else if (gs_System_State.c_Phase == TASK_PHASE_PAUSE)
	{
		gc_PhaseInx = 0;
	}*/

	if (gs_System_State.c_Phase != TASK_PHASE_STOP)
	{
		U8 tc_Ret;
		if (gc_Play_FileType == 2)
		{
			tc_Ret = GIF_StopCmd(gs_System_State.c_FileHandle);
		}
		else
		{
			tc_Ret = Image_StopCmd(gs_System_State.c_FileHandle);
		}
		if (tc_Ret != DSP_SUCCESS)
		{
//			dbprintf("JPEG stop error !!\n");
		}
		
		gs_System_State.c_Phase = TASK_PHASE_STOP;
	}

	DOS_Search_File(C_File_All|C_By_FDB, C_OtherFileType, C_CmpExtName|C_Next);
	gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;           //Jimi 090305

	if((gc_AutoJpeg_Flag==1) && (gc_Play_FileType==2) && (gc_JpegVelocity==0))  //20090331
	{
		gw_AutoJpeg_Timer=(U16)(gc_JpegVelocity+1)*36;	
	}	
}

void jpeg_prev()
{
//	dbprintf("jpeg prev!\n");

	if (gs_System_State.c_Phase == TASK_PHASE_STOP)
	{
		if (gw_FileIndex[gs_System_State.c_FileHandle] == 0)
			return;//if no this line,error will happen
		gc_PhaseInx = 3;
	}
	else if (gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
	{
		gc_PhaseInx = 3;
		gc_KeyDet_Mask = 1;//(JC)H0508 avoid key-press to change phase index		
	}
	/*else if (gs_System_State.c_Phase == TASK_PHASE_PAUSE)
	{
		gc_PhaseInx = 0;
	}*/
	
	if (gs_System_State.c_Phase != TASK_PHASE_STOP)//(JC)H0505
	{
		U8 tc_Ret;
		if (gc_Play_FileType == 2)
		{
			tc_Ret = GIF_StopCmd(gs_System_State.c_FileHandle);
		}
		else
		{
			tc_Ret = Image_StopCmd(gs_System_State.c_FileHandle);
		}
		if (tc_Ret != DSP_SUCCESS)
		{
//			dbprintf("JPEG stop error !!\n");
		}

		gs_System_State.c_Phase = TASK_PHASE_STOP;
	}

	DOS_Search_File(C_File_All|C_By_FDB, C_OtherFileType, C_CmpExtName|C_Prev);
	gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;          //Jimi 090305
}


void jpeg_back2uplevel()
{
	if((gs_System_State.c_Phase == TASK_PHASE_PLAYACT) || (gs_System_State.c_Phase == TASK_PHASE_PAUSE))
	{
		jpeg_stop();	
	}
	
//	dbprintf("back 2 menu level\n");
	gc_Task_Next=C_Task_Menu;
	gw_init_needed |= SET_BIT8;
	gc_PhaseInx=0;
}


U8 jpeg_marker_parser(U32 *tpdw_File_Ptr, U8 tc_marker_Hbyte, U8 tc_marker_Lbyte)
{
	bit tb_soi_found = 0;
	U8 tc_sector_cnt = 0;
	U16 tw_Buf_idx;

	do{
		tw_Buf_idx = *tpdw_File_Ptr & 0x1FF;
		if ((!tw_Buf_idx) && (*tpdw_File_Ptr>0))
		{
			DOS_Read_File(C_OtherFHandle);
			tc_sector_cnt ++;
		}
		
		if(gc_PlayRecordDataBuf[tw_Buf_idx++] == tc_marker_Hbyte)
		{
			bit tb_next_sector = 0;
			//marker identifier found

			if(tw_Buf_idx == 512)
			{
				tw_Buf_idx = 0;
				DOS_Read_File(C_OtherFHandle);
				tb_next_sector = 1;
				tc_sector_cnt ++;
			}

			if(gc_PlayRecordDataBuf[tw_Buf_idx] == tc_marker_Lbyte)
				tb_soi_found = 1;

			if(!tb_next_sector)
			{		
				tw_Buf_idx++;
				(*tpdw_File_Ptr)++;
			}			
		}
		(*tpdw_File_Ptr)++;

		if(tc_sector_cnt >=8)
			return FAIL;

	}while(!tb_soi_found);		

	return SUCCESS;

}


void JpegMenu_Process()
{
	bit	tc_Confirm=0;
	gc_refresh_type = REFRESH_ALL;
	if(gc_PhaseInx!=2)
	{
        switch(gc_PhaseInx)
        {
            case C_JpegMenu:
				gc_refresh_type = REFRESH_ALL;	
				tc_Confirm=1;
            break;

            case C_JpegNext:
				gc_refresh_type = REFRESH_NEXT;
				if(gc_LCMDispIndex==2)
				{
					gc_LCMDispIndex=0;
				}
				else
				{
					gc_LCMDispIndex++;
				}
            break;

            case C_JpegPrev:
				gc_refresh_type = REFRESH_PREV;
				if(gc_LCMDispIndex==0)
				{
					gc_LCMDispIndex=2;
				}
				else
				{
					gc_LCMDispIndex--;
				}
            break;	
        }

	  	if(tc_Confirm==1) //confirm selected item
		{
			if(gc_LCMDispIndex==0)  //jpeg Dir
			{
				play_stop();
				gc_TaskMode_BkUp=C_Task_Jpeg;
				gc_Task_Next=C_Task_Dir;
				return;
			}
			else if(gc_LCMDispIndex==1)  //Select Jpeg Mode
			{
				gc_DispPlayMenuAct=1;
				gc_TuneVolFreqStatus=32;
				return;
			}
			else if(gc_LCMDispIndex==2)  //Select Display velocity
			{
				gc_DispPlayMenuAct=2;
				gc_TuneVolFreqStatus=33;
				return;
			}
		}
   	}	
}


void JpegDispMode_Process()
{
	bit	tc_Confirm=0;

	if(gc_PhaseInx!=2)
	{
        switch(gc_PhaseInx)
        {
            case C_JpegMenu:
				tc_Confirm=1;
            break;

            case C_JpegNext:
            case C_JpegPrev:
				gc_refresh_type=REFRESH_NEXT;
				gc_LCMDispIndex=(gc_LCMDispIndex+1)&0x01;
            break;	
        }

	  	if(tc_Confirm==1) //confirm selected item
		{
			if(gc_LCMDispIndex==0)  //User
			{
				gc_AutoJpeg_Flag=0;
				gc_TuneVolFreqStatus=0;
				gc_DispPlayMenuAct=0;
			}
			else if(gc_LCMDispIndex==1)  //Auto
			{
				gc_AutoJpeg_Flag=1;
				gc_TuneVolFreqStatus=0;
				gc_DispPlayMenuAct=0;
			}
		}
   	}		
}


void JpegDispVelocity_Process()
{
	if(gc_PhaseInx!=2)
	{
        switch(gc_PhaseInx)
        {
            case C_JpegMenu:
				gc_TuneVolFreqStatus=0;
				gc_DispPlayMenuAct=0;
            break;

            case C_JpegNext:
				if(gc_JpegVelocity<10)
				{
					gc_JpegVelocity++;
				}
            break;

            case C_JpegPrev:
				if(gc_JpegVelocity>0)
				{
					gc_JpegVelocity--;
				}
            break;	
        }
   	}	
}


void JpegMenu_Disp()
{
	if(gc_TuneVolFreqStatus==31)
	{
		gc_JPGSetting=0;
		IR_Service_Process_Menu_2();

		if(gc_DispWallpaper!=32)
		{
			//TFT_PhotoMenu();//--sunzhk modif 100703
			Menu_Disp_Item_Other(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
			gc_DispWallpaper=32;
			gc_LCMDispIndex=0;
			gc_LCMDispIndexBak=0xFF;
			gc_PhaseInx=0;
		}

		JpegMenu_Process();

		if(gc_LCMDispIndexBak!=gc_LCMDispIndex)
		{
			gc_LCMDispIndexBak=gc_LCMDispIndex;
			//TFT_PhotoMenu();//--sunzhk modif 100703
			Menu_Disp_Item_Other(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);;
		}
	}
	else if(gc_TuneVolFreqStatus==32)
	{
		IR_Service_Process_Menu_2();
		if(gc_DispWallpaper!=33)
		{
			gc_DispWallpaper=33;
			gc_LCMDispIndex=0;
			gc_LCMDispIndexBak=0xFF;
			gc_PhaseInx=0;
		}

		gc_JPGSetting=1;
		JpegDispMode_Process();

		if(gc_LCMDispIndexBak!=gc_LCMDispIndex)
		{
			gc_LCMDispIndexBak=gc_LCMDispIndex;
			//TFT_PhotoMenu();//--sunzhk modif 100703
			Menu_Disp_Item_Other(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
		}
	}
	else if(gc_TuneVolFreqStatus==33)
	{
		IR_Service_Process_Menu_2();
		if(gc_DispWallpaper!=34)
		{
			gc_JpegVelocityBak=0xFF;
			gc_DispWallpaper=34;
			gc_LCMDispIndex=0;
			gc_PhaseInx=0;
		}

		JpegDispVelocity_Process();

		if(gc_JpegVelocityBak!=gc_JpegVelocity)
		{
			TFT_Velocity();
			gc_JpegVelocityBak=gc_JpegVelocity;
		}
	}

	if(gc_TuneVolFreqStatus==0)  //½T©w¿ï¾Ü«á play jpeg
	{
		if(gc_AutoJpeg_Flag==1)
		{
			gw_AutoJpeg_Timer=(U16)gc_JpegVelocity*36;
		}
		gc_PhaseInx=C_JpegPause;	
	}
	else
	{
		gc_PhaseInx=C_JpegIdle;
	}
}


void Play_SDFlash_DosInit_Jpeg()
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
		gc_MenuItemCount=0;
		gs_System_State.c_FileHandle=0;
//		DOS_DIRtable();  //20090216 chiayen add       
		gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;           
		DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);//(JC)count music file no. in root
		gc_PhaseInx=0;
		gb_TFT_refresh=1;
		gc_DispWallpaper=0xFF;
	}
}


void Play_SourceDetect_Process_Jpeg()
{
	gb_SD_Exist_pre=gb_SD_Exist;
	if(!SD_Detect)  //SD exist
    {
        gb_SD_Exist=1;
    }
	else
	{
		gb_SD_Exist=0;
		gb_SDNoFileflag=0;
	}
#if 1//(USB_HOST==1)
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
#endif

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
		jpeg_stop();
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
			Play_SDFlash_DosInit_Jpeg();
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
			Play_SDFlash_DosInit_Jpeg();
		}
		else if(gc_CurrentCard==0)
        {
			DEVICE_REG[0x00]= 0x01;
			gc_CardExist &=0xFD;
			InitFlash();
			Play_SDFlash_DosInit_Jpeg();
		}
		#endif

		#if 0
		if(gc_CurrentCard==2)
        {           
            if(SD_Identification_Flow())
            {
                gc_CardExist |=0x02;
				Play_SDFlash_DosInit_Jpeg();      
            }
            else
            {
				XBYTE[0xB400]= 0x01;
				gc_CurrentCard=0;
				gc_CardExist &=0xFD;
				gb_FindFlag = 0;
				InitFlash();
				Play_SDFlash_DosInit_Jpeg();					    
            }
        }
        else if(gc_CurrentCard==0)
        {
			XBYTE[0xB400]= 0x01;
			gc_CardExist &=0xFD;
			InitFlash();
			Play_SDFlash_DosInit_Jpeg();
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
			Play_SDFlash_DosInit_Jpeg();
		}
		#endif
    }
}


void Jpeg_Task()
{
	gc_TuneVolFreqStatus=0;  //20090331
	gc_DispPlayMenuAct=0;
	gw_AutoJpeg_Timer=(U16)gc_JpegVelocity*36+18;
	 
	while (1)
	{
		switch(gc_PhaseInx)
		{
			case C_JpegIdle:
				set_clock_mode(CLOCK_MODE_MP3);		// added by maxliao
				jpeg_idle();
			break;

			case C_JpegInit:
			//	#ifdef CS1783
				set_clock_mode(CLOCK_MODE_MP3L);	// added by maxliao
			//	#else
			//	set_clock_mode(CLOCK_MODE_MJPEG);	// added by maxliao
			//	#endif
				jpeg_init();
			break;			

			case C_JpegProc:
				jpeg_proc();
			break;
			
			case C_JpegPause:
				if(gc_TuneVolFreqStatus<30)
				{
					jpeg_playpause();
				}
			break;

			case C_JpegStop:
				jpeg_stop();
			break;

			case C_JpegEndfile:
				jpeg_endfile();	
			break;

			case C_JpegNext:
				if(gc_TuneVolFreqStatus<30)
				{
					jpeg_next();
				}				
			break;

			case C_JpegPrev:
				if(gc_TuneVolFreqStatus<30)
				{
					jpeg_prev();
				}
			break;
			
			case C_JpegUpLevel:
				jpeg_back2uplevel();
			break;

			case C_JpegMenu:  //20090331
				if(gc_DispPlayMenuAct==0)
				{
					gc_TuneVolFreqStatus=31;
				}											
			break;
				
            case C_PowerOff:
				if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
				{
					jpeg_stop();	
				}
				gc_Task_Next=C_Task_PowerOff;  					    	
            break;
				
			case C_MainMenu:
				jpeg_stop();
				gc_Task_Next=C_Task_Menu;
			break;
		}	
		
		if(gc_TuneVolFreqStatus>30)
		{
			if((gc_PhaseInx==C_JpegProc)&&(gc_Play_FileType<2))			//sunzhk add 090612 
			{
				U8 tc_Ret;
				if (gs_System_State.c_Phase != TASK_PHASE_STOP)
				{			
					//jpeg stop
					if (gc_Play_FileType == 2)
					{
						tc_Ret = GIF_StopCmd(gs_System_State.c_FileHandle);
					}
					else
					{
						tc_Ret = Image_StopCmd(gs_System_State.c_FileHandle);
					}
			
					if (tc_Ret != DSP_SUCCESS)
					{
//						dbprintf("JPEG stop error !!\n");
						XBYTE[0xB001]|=0x02;
						XBYTE[0xB001]&=~0x02;
					}
					gs_System_State.c_Phase = TASK_PHASE_STOP;
				}
			}
			JpegMenu_Disp();
		}
		else
		{
			if((gc_PhaseInx==C_JpegIdle)&&(gc_Play_FileType<2))
			{
				if(gc_AutoJpeg_Flag==1 && gw_AutoJpeg_Timer==0)
		 	    {
					gw_AutoJpeg_Timer=(U16)gc_JpegVelocity*36;
					gc_PhaseInx=C_JpegNext;	
				}
			}

			if((gc_Play_FileType==2))
			{
				if(gc_AutoJpeg_Flag==1 && gw_AutoJpeg_Timer==0)
		 	    {
					gw_AutoJpeg_Timer=(U16)gc_JpegVelocity*36;
					gc_PhaseInx=C_JpegNext;	
				}				
			}
		}	
			
		Polling_TaskEvents();
        IR_Service_Process();
		Play_SourceDetect_Process_Jpeg();
		if (gc_Task_Current != gc_Task_Next)
		{
			gc_Task_Current = gc_Task_Next;
			break;	
		}
	}
}


void IR_Service_Process_Menu_2(void)
{
    if(gc_IRCmdStatus==1)
    {
        ir_service_menu();
    }
}
