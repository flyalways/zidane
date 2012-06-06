#include "SPDA2K.h"
#include "dos\fs_struct.h"
#include "Memalloc.h"
#include "..\DSP\DSPPHYSIC.H"
#include "..\DSP\dspuser.h"
#include "..\header\music_play.h"
#include "..\IR\remote.h"
#include "..\header\host_init.h"
#include "..\UI_Display\unicode_hzk.c"
#include "..\header\variables.h"
#include "..\LCM\TFT_display.h"		// TFT
#include "..\LCM\TFT_config.h"		// TFT
#include "..\LCM\LCM.h"
#include "..\LCM\LCM_func.h"
#include "..\LCM\UI_icon.h"

#define D_LrcNum		3

extern xdata U8 gc_NUMBER[4];

extern	data	bit	gb_HostError;  //20090107 chiayen add
extern	U8		gc_clock_mode;  //20090526 chiayen add
extern	U8		Get_LogData_PageIndex(void);
extern	U8		WMA_ASF_Parser();
extern	U8		DOS_DIRtable(void);  //20090216 chiayen add
extern	U8		USER_LogFile_ReadWrite(U8 tbt_ReadOrWrite);//use reserveblock for log
extern	U16		MP3_Total_Time_Parser();
extern	U16		MP3_Bitrate_Parser();
extern	void	MediaChange(void);  //20090107 chiayen add
extern	void	PickSong_Process(void);
extern	void	FM_FREQ_CHG(U8 tc_UP_DOWN,U8 offset);
extern	void	Lyric_NextGetLyrics(void);
extern	U8		DOS_SearchFreeCluster(U8 tc_SearchMode);

void InitDispVariable(void);
void Play_SDFlash_DosInit();
void Play_SourceDetect_Process();
void IR_Service_Process();
void Music_SetVolumeCmd(void); 
void LCM_Display_Func1(void);
void LCM_Display_Func2(void);
extern	void IR_Service_Process_Menu();
extern	void UI_fastFFFR_Play(U8 XFFFR);

U8 LRC_DisplayString(U8 *tpc_DataBuf, U8 tw_nByte,U8 tc_RowAddr);

xdata	System_Struct gs_System_State;
struct	str_scroll_t	gs_str_scroll_state;
//////////////sunzhk add 090612
void ir_service_menu(void);
void ir_commandservice_menu(void);
void IR_Service_Process_Menu();

xdata U8 gc_loopcount=0;
extern xdata	U32	gdw_HOSTStartSectorRead;
extern void play_fffr_end();


////////////sunzhk add over
////////////////////////////////////////////////////////////////////////////////////////////////

void TFT_disp_SongName(U8 tc_Width,U8 xdata *BMPdataBuf)  //yflin080820
{
	xdata	U8	m;
	data	U8	tc_i;
	data	U8	j;
	data	U8	tc_char;

	if (tc_Width == 0)
	{
		return;
	}
	//for 023Only
	LCD_Nand2DataMode();

	do
	{
		if(tc_Width>16)
		{
			tc_Width -= 16;
			tc_i = 32;
		}
		else
		{ 
			tc_i = tc_Width*2;
			if (tc_i == 0)
			{
				break;
			}
			tc_Width = 0;
		}

		j = tc_i;
		do { // loop 'tc_i' times
			tc_char = *BMPdataBuf++;
			m = 8;
			do { // loop 8 times
				tc_char = tc_char>>1;	//bitbit shift
				if(CY)		//低位为1 将输入一个点的颜色
				{	
					if(gb_LrcFileName_Exist==1)
					{
						if(gb_LRCDispLevel==0)
						{
							// LRC材舱︹
							XBYTE[0xB420] = FullLRC_Word1Color/256; 
							XBYTE[0xB420] = FullLRC_Word1Color; 	
						}
						else
						{
							// LRC材舱︹
							XBYTE[0xB420] = FullLRC_Word2Color/256; 
							XBYTE[0xB420] = FullLRC_Word2Color; 	
						}
					}
					else
					{

						if(gc_folder_disp==1)
						{
							XBYTE[0xB420] = WordColor_FolderName/256; 
							XBYTE[0xB420] = WordColor_FolderName;
						}else
						{	// 簈︹
						XBYTE[0xB420] = WordColor_FileName/256; 
						XBYTE[0xB420] = WordColor_FileName;
					}
				}
				}
				else
				{
					if(gb_LrcFileName_Exist==1)
					{
						if(gb_LRCDispLevel==0)
						{
							// LRC材舱璉春︹
							XBYTE[0xB420] = FullLRC_Word1Color_BG/256; 
							XBYTE[0xB420] = FullLRC_Word1Color_BG;
						}
						else
						{
							// LRC材舱璉春︹
							XBYTE[0xB420] = FullLRC_Word2Color_BG/256; 
							XBYTE[0xB420] = FullLRC_Word2Color_BG;						
						}
					}
					else
					{
						if(gc_folder_disp==1)
						{
							XBYTE[0xB420] = WordColorBG_FolderName/256; 
							XBYTE[0xB420] = WordColorBG_FolderName;
						}else
						{
						// 簈璉春︹
						XBYTE[0xB420] = WordColorBG_FileName/256; 
						XBYTE[0xB420] = WordColorBG_FileName;
					}
				}
				}
			} while (--m);
		} while (--tc_i);
	} while(tc_Width);

	LCD_Data2NandMode();
}

U8 TFT_GetStringHZK(struct string_view_t * p_disp_param,
					struct string_disp_t xdata * p_string_disp_param,
					U8 * tpc_DataBuf,
					U8 tc_nByte,
					U8 tc_ANSI,
					U8 line_num,
					U8 font_truncate_flag,
					U8 tc_scroll_flag,
					U8 xdata * p_bitmap)
{
	//mode: display mode  offset: reserved for file brwser  tc_nByte:string num tc_ANSI:string type(ANSI or unicode)
	struct string_view_t disp_param;
	data	U8	tc_Status = 0; 
	data	U8	i = 0;
	data	U8	tc_1st_char = 0;
	data	U8	tc_shift_start = 0;
	data	U8	tc_next_1st_char = 0;
	data	U8	tc_next_shift_start = 0;
	data	U8	tc_ColumnNum;
	data	U16	tw_Width = 0;

	disp_param = *p_disp_param;
	gw_BmpInx 	= 0;
	gb_TFT_scroll=0;

	if (tc_scroll_flag)
	{
		i = tc_1st_char
		  = tc_next_1st_char
		  = gs_str_scroll_state.c_str_1st_char;
		tc_shift_start = tc_next_shift_start	= gs_str_scroll_state.c_shift_in_1st_char;
	}

	while (i<tc_nByte)
	{		
		if(tc_ANSI)
		{
			tc_ColumnNum = UNICODE_HZK_GET_BMP(*(tpc_DataBuf+i),*(tpc_DataBuf+i+1),&(p_bitmap[gw_BmpInx]),1);
		}
		else //Unicode
		{ 
			tc_ColumnNum = UNICODE_HZK_GET_BMP(*(tpc_DataBuf+i+1),*(tpc_DataBuf+i),&(p_bitmap[gw_BmpInx]),0);
		}
			
		if (i == tc_1st_char)
		{
			tc_next_shift_start ++;
			if (tc_next_shift_start >= (tc_ColumnNum&0x7f))
			{
				tc_next_shift_start = 0;
				tc_next_1st_char++;
				if(tc_ColumnNum&0x80)tc_next_1st_char++;
			}
		}
			
		tw_Width += (tc_ColumnNum&0x7f);
		if(tw_Width > tc_shift_start+disp_param.c_view_width)
		{
			tc_Status=1;
			break;
		}

		if (tc_ColumnNum&0x80)
		{
			i+=2;
		}
		else
		{
			i+=1;
		}
	}

	if(tw_Width>FileName_Width)
	{
		gb_TFT_scroll=1;
	}

	tw_Width -= tc_shift_start;
	if (tw_Width > disp_param.c_view_width)
	{
		if (font_truncate_flag)
		{
			tw_Width = disp_param.c_view_width;
		}
		else
		{
			tw_Width -= (tc_ColumnNum&0x7f);
		}
	}
	else
	{
		tc_next_shift_start	= 0;
		tc_next_1st_char	= 0;
	}

	p_string_disp_param->c_x			= disp_param.c_view_x;
	p_string_disp_param->c_y			= disp_param.c_view_y + disp_param.c_row_spacing*line_num;
	p_string_disp_param->c_width		= tw_Width;
	p_string_disp_param->c_reverse_flag	= 0;
	p_string_disp_param->pc_font_bitmap	= p_bitmap +2*tc_shift_start; 

	if (tc_scroll_flag)
	{
		gs_str_scroll_state.c_str_1st_char		= tc_next_1st_char;
		gs_str_scroll_state.c_shift_in_1st_char	= tc_next_shift_start;
	}
	else
	{
		gs_str_scroll_state.c_str_1st_char		= 0;
		gs_str_scroll_state.c_shift_in_1st_char	= 0;
	}

	return tc_Status;
}


static void UI_StringTrigger(struct string_disp_t xdata *p_str)
{
	LCM_set_view(TFT_HZKDispDir, p_str->c_x, p_str->c_y, p_str->c_width, 16); 
	TFT_disp_SongName(p_str->c_width, p_str->pc_font_bitmap);//total width,reverse,string start index
}

void TFT_ShowSongName(struct string_view_t *p_disp_param,struct string_disp_t xdata *p_string_disp_param)
{
	data	U8	tc_ColumnNum;
	data	U16	i;
	data	U16 tw_Start;
	data	U16 tw_Width;
	struct string_view_t disp_param;

	disp_param = *p_disp_param;
	UI_StringTrigger(&(p_string_disp_param[0]));
	tw_Width	= p_string_disp_param[0].c_width;
	//clear resudue space
	if (tw_Width < disp_param.c_view_width)
	{
		tw_Start	 = p_string_disp_param[0].c_x+tw_Width;
		tc_ColumnNum = p_string_disp_param[0].c_y;
		tw_Width	 = disp_param.c_view_width-tw_Width;
 
		LCD_set_view(TFT_HZKDispDir, tw_Start, tc_ColumnNum,tw_Width,16); // horizontal view	
		LCD_Nand2DataMode();
		for(i=0;i<tw_Width*32;i++)
		{
			if(gc_folder_disp==1)
			{
				XBYTE[0xB420]=WordColorBG_FolderName>>8;
				XBYTE[0xB420]=WordColorBG_FolderName;

			}else
			{
			XBYTE[0xB420]=WordColorBG_FileName>>8;
			XBYTE[0xB420]=WordColorBG_FileName;
		}
		}
		LCD_Data2NandMode();
	}
}


static U8 LCD_Disp_FileName(U8 *tpc_DataBuf, U8 tc_ANSI, U8 tc_nByte, U8 tc_scroll_flag)
{	//mode: display mode  offset: reserved for file brwser  tc_nByte:string num tc_ANSI:string type(ANSI or unicode)
	U8 tc_Status;
	struct string_view_t disp_param;// = { Start_X_FileName,	Start_Y_FileName,	FileName_Width,	22};
	struct string_disp_t filename_attrib;
	
	if(gc_folder_disp==1)	 
	{//显示文件夹名称
		disp_param.c_view_x=Start_X_FolderName;
		disp_param.c_view_y=Start_Y_FolderName;
		disp_param.c_view_width=FolderName_Width;
		disp_param.c_row_spacing=20;  
		
		gs_str_scroll_state.c_str_1st_char=0;  //20090331
		gs_str_scroll_state.c_shift_in_1st_char=0;

	}else
	{//显示文件的名称
		disp_param.c_view_x=Start_X_FileName;
		disp_param.c_view_y=Start_Y_FileName;
		disp_param.c_view_width=FileName_Width;
		disp_param.c_row_spacing=22;
	}
	tc_Status=TFT_GetStringHZK(&disp_param, //*
								 &(filename_attrib),
								 &(tpc_DataBuf[0]), //* string to display
								 tc_nByte, //* string size in bytes
								 tc_ANSI, //* unicode or ansi flag
								 0, //* line number
								 1,/*truncate on char or not*/ //* 1
								 tc_scroll_flag, //rolling factor
								 &(gc_BmpString[0]));
	TFT_ShowSongName(&disp_param, &filename_attrib);
	return tc_Status;
}


void Music_EQ_Cmd(U8 tc_Type)
{
    DSP_EQ_Cmd(tc_Type);
    gs_DSP_GLOBAL_RAM.sc_EQ_Type = tc_Type;
    DSP_SpectrumOn();
}


void Music_WakeUp(U8 tc_Type)
{   
    DSP_WakeUp(tc_Type);
}


U8 MP3_DataIn(void)
{   
	data	U8 tc_status;
	xdata	U16	tw_SmpRateIdx;
    
    while (L2_DSP_Read_DMem16(DSP_EmptyBuffer) >= 512)           //Jimi 091126
    {
        tc_status=DOS_Read_File(C_MusicFHandle);
        if(tc_status == DOS_END_OF_FILE)
        {
			L2_DSP_Write_DMem16(DSP_MP3_file_end_flag, 1);  //Jimi 091126
            return DSP_DATAIN_COMMAND_ERROR;
        }
        else if(tc_status)
        {
            return tc_status;   
        }

        L2_DSP_MCU_DM_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index++, 0x20, (U16)gc_PlayRecordDataBuf, DMA_24BIT_MODE);
        L2_DSP_SendCommandSet(DCMD_DatIn);

        if (gs_DSP_GLOBAL_RAM.sc_DM_Index >= 24)
		{
            gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // DM Range: 0x2000 ~  0x2FFF
		}
    }

    tw_SmpRateIdx = L2_DSP_Read_DMem16(0x3F8E);	//0x3F8E is an index of mp3 sampling rate within DSP
                                                                                                                //0x0:44.1k;  0x1:48k;  0x2:32k;  0x3:22.1k;  0x4:24k;  0x5:16k;  0x6:11.025k;  0x7:12k;  0x8:8k
    if(tw_SmpRateIdx == 0 || tw_SmpRateIdx == 3 || tw_SmpRateIdx == 6)
	{
        GLOBAL_REG[0x46] |=0x04;
	}
    else
	{
        GLOBAL_REG[0x46] &=0xFB; 
	}

    return  DSP_SUCCESS;
}


U16 MP3_Bitrate(void)
{
    return(L2_DSP_Read_DMem16(DSP_MP3Bitrate));
}


U16 MP3_ReadTime(void)
{
	code	U16	SampleRate[4][3]={{11025,12000,8000},{0,0,0},{22050,24000,16000},{44100,48000,32000} };
	data	U8	tc_SampleRate, tc_MPEG_Type, tc_Layer;
    data	U16 tw_DurTime;
	data	U16 tw_SamplePerFrame;
    xdata	U32 tdw_Mpeg_Status = L2_DSP_Read_DMem24(DSP_MPEGSt);
    xdata	U32 tdw_FrameCnt = L2_DSP_Read_DMem24(DSP_DecodeFrameCounter);

	tc_SampleRate = ((U8)(tdw_Mpeg_Status>>5)&0x03);
	tc_MPEG_Type = ((U8)(tdw_Mpeg_Status>>(14+8))&0x03);
	tc_Layer = ((U8)(tdw_Mpeg_Status>>(12+8))&0x03);

    if(tdw_FrameCnt>0x20)
    {
		if(tc_Layer == 0x03) //layer-I, reference ISO/IEC 11172-3 header description
        {
			tw_SamplePerFrame = 384;
        }
		else if((tc_MPEG_Type != 0x03) && (tc_Layer == 0x01))
		{	
			tw_SamplePerFrame = 576;
		} 
		else
		{
			tw_SamplePerFrame = 1152;
    	}
		
		if(tc_MPEG_Type != 0x01)
		{
			tw_DurTime = gw_CurrentSec +  tdw_FrameCnt * tw_SamplePerFrame/SampleRate[tc_MPEG_Type][tc_SampleRate];
    	}
	    else
    	{
        	tw_DurTime = 0;
	    }
    }
	else//(JC)min ~32 frames -> 1sec
    {
        tw_DurTime = gw_CurrentSec;
    }

    return tw_DurTime;
}


U8 MP3_EOF_Proc(void)
{                       
	data	U16 tw_BufRem;
	data	U16 tw_RampDownBufSz;
	data	U16 tw_dec_bsbuf_rem;    //Jimi 091126

   	L2_DSP_Write_DMem16(0x3F44, 8000);	//Jimi 100517, enlarge DSP MP3 decoder ramp down step
	L2_DSP_Write_DMem16(0x3F45, 8000);

    tw_BufRem = L2_DSP_Read_DMem16(DSP_RemainBuffer);
    tw_RampDownBufSz =(U16)(((U32)gs_System_State.w_BitRate * 1000 )/8/20);  // 0.05 sec for ramp down
    tw_dec_bsbuf_rem = L2_DSP_Read_DMem16(DSP_MP3_dec_bsbuf_rem);   //Jimi 091126
    
    if(tw_RampDownBufSz < 512 )     //for 8k bitrate condition(M2L3 & M2.5L3)
	{
        tw_RampDownBufSz = 512;
	}
    
    if(tw_BufRem < tw_RampDownBufSz )
	{
		if(L2_DSP_Read_DMem16(DSP_RampDownComplete) == 0)	//(Jimi 091126)RampDownOK == 0
		{
			if( tw_dec_bsbuf_rem < tw_RampDownBufSz)
			{
        		return 1;
			}
		}
		else
		{	
			return 1;
		}
	}

    return 0;
}


//=================WMA=================
U8 WMA_err_bs_proc(void)
{
	data	U8	tc_iloop;
	data	U16	tw_tmp;
	data	U16 tw_DRMStatus;
	data	U16 tw_DecodeStatus;
	xdata	U32 tdw_testcluster;
	
	tw_DRMStatus = L2_DSP_Read_DMem16(DSP_WMAhasJanusDRM);
	tw_tmp = L2_DSP_Read_DMem16(DSP_DecodeStatus);
	tw_DecodeStatus = tw_tmp & 0x0020;

	if( (tw_DecodeStatus == 0x0020) || tw_DRMStatus  )	//wma format error or drm
	{
        DSP_ResetCmd();
		return DSP_USER_FILE_TYPE_ERROR;
	}
	
	tw_DecodeStatus = tw_tmp & 0x00C0;
    if(tw_DecodeStatus!=0x0000)
	{
		tdw_testcluster = (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint >> 9)+5;

		if( tdw_testcluster >= (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_TotalSize >> 9) )
		{
			return DSP_RUNNING_STATUS1_ERROR;
		}

		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster =gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint = tdw_testcluster << 9;
		gs_File_FCB[gs_System_State.c_FileHandle].dw_File_CurrentCluster = DOS_Seek_File(gs_System_State.c_FileHandle,gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint>>9);

		L2_DSP_Write_DMem16(DSP_EmptyBuffer,0x3C00);	//reset BS buffer information	//Jimi081112 mdf size to 0x3C00
        L2_DSP_Write_DMem16(DSP_RemainBuffer,0x0000);   

        gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;      
        L2_DSP_Write_DMem24(DSP_WMASectorOffset,(gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint>>9));      
        L2_DSP_Write_DMem16(DSP_DecodeStatus,0x0000);
        L2_DSP_Write_DMem16(DSP_WMARandomFlag,0x0001);   //set random flag
        tc_iloop = 0;

        do
        {
            if (DOS_Read_File(C_MusicFHandle) == DOS_END_OF_FILE)
            {
                return DSP_DATAIN_COMMAND_ERROR;
            }

            L2_DSP_MCU_DM_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index++, 0x20, (U16)gc_PlayRecordDataBuf, DMA_24BIT_MODE);
            L2_DSP_SendCommandSet(DCMD_DatIn); 

            tc_iloop ++;
        }while(tc_iloop!=5);

    }
    return DSP_SUCCESS; 
}


U8 WMA_DataIn(void)  //20090803 chiayen modify
{
	xdata	U8  tc_errbs;

/* Error Bitstream Process */
    tc_errbs = WMA_err_bs_proc();

    if(tc_errbs == DSP_USER_FILE_TYPE_ERROR || tc_errbs == DSP_RUNNING_STATUS1_ERROR)
    {
        return DSP_DATAIN_COMMAND_ERROR;
    }   

    while (L2_DSP_Read_DMem16(DSP_EmptyBuffer) > 512) //wei.tang modify 081024  //for test play WMA file 081024
    {
        if (DOS_Read_File(C_MusicFHandle) == DOS_END_OF_FILE)
        {
            return DSP_DATAIN_COMMAND_ERROR;
        }

        L2_DSP_MCU_DM_DMA(gs_DSP_GLOBAL_RAM.sc_DM_Index++, 0x20, (U16)gc_PlayRecordDataBuf, DMA_24BIT_MODE);
        L2_DSP_SendCommandSet(DCMD_DatIn);

        if (gs_DSP_GLOBAL_RAM.sc_DM_Index >= 30)  //20090420 chiayen modify 36->30
		{
            gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // DM Range: 0x2000 ~  0x37FF
    	}
    }
    return  DSP_SUCCESS;
}


U16 WMA_ReadTime(void)
{
    U32 tw_cur_time_h;
	U32	tw_cur_time;

    tw_cur_time_h = L2_DSP_Read_DMem24(DSP_WMACurrenTimeHigh);
    tw_cur_time = L2_DSP_Read_DMem24(DSP_WMACurrenTimeLow) + (tw_cur_time_h<<24);

    return(tw_cur_time / 1000);    
}


U16 WMA_Bitrate(void)
{
    return(L2_DSP_Read_DMem16(DSP_WMABitrate));
}


U8 WMA_EOF_Proc(void)
{    
    data	U8  tc_wma_eof_cnt = 0;
	data	U16 tw_DSP_RemainBufPrev = 0;
    xdata	U16 tw_bs_buf_remain;
    xdata	U16 tw_DSP_dec_status;
    
    tw_bs_buf_remain = L2_DSP_Read_DMem16(DSP_RemainBuffer);

    if(tw_bs_buf_remain == tw_DSP_RemainBufPrev)
	{
        tc_wma_eof_cnt ++;
	}   
    else
    {
        tw_DSP_RemainBufPrev = tw_bs_buf_remain;
        tc_wma_eof_cnt = 0;
    }    
    tw_DSP_dec_status = L2_DSP_Read_DMem16(DSP_DecodeStatus) & 0xC0;

    if( tw_DSP_dec_status || (tc_wma_eof_cnt >=10) )
    {
        L2_DSP_Write_DMem16(DSP_DecodeStatus, tw_DSP_dec_status|0xC0 );//磷ぇplay_stop hang
        
        return 1;
    }
    
    return 0;   
}


U8 Music_PlayCmd(void)
{
    // sent "PLAY" command to DSP
    if(L2_DSP_SendCommandSet(DCMD_Play) != DCMD_Play)
    {
        return DSP_PLAY_COMMAND_ERROR;
    }

    if(gc_Play_FileType!=1)                  //(Jimi 091028)wma do not need to reset bitstream address cuz the asf parser is contiguous to wma decoder 
    {
        gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // reset bitstream address of DM//(Jimi 091104 mdf)
        // MP3 file open        
    	if(DOS_Open_File_r(C_MusicFHandle, C_Open_FoundFile, C_NullFileName))
	    {
    	    return DSP_PLAY_COMMAND_ERROR;
	    }
    }

    return DSP_SUCCESS;
}


void Music_VolumeUpCmd(void)
{
    if (gs_DSP_GLOBAL_RAM.sw_Volume<50)
    {
        gs_DSP_GLOBAL_RAM.sw_Volume+=2;
    }
    L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sw_Volume);     // Vloume Range:0~63
}


void Music_SetVolumeCmd(void)
{
    L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sw_Volume);
}


void Music_VolumeDownCmd(void)
{
    if (gs_DSP_GLOBAL_RAM.sw_Volume>1)
    {
        gs_DSP_GLOBAL_RAM.sw_Volume-=2;
    }
    L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sw_Volume);     // Vloume Range:0~63
}


U8 Music_PauseCmd(void)  //20090803 chiayen modify
{   
    data	U8  tc_Ret;
	data	U16	TimeOUT;

    Music_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);
	tc_Ret=DSP_PauseCmd();
	TimeOUT=0xFFFF;

	if(gc_Play_FileType==0)
	{
		do
		{
			TimeOUT--;
			if (TimeOUT == 0)
			{
				return DSP_RUNNING_STATUS1_ERROR;
			}
		} while (!(L2_DSP_Read_DMem16(DSP_RunningStatus)&0x0002));
	}
	else
	{
		do
		{
			TimeOUT--;
			if (TimeOUT == 0)
			{
				return DSP_RUNNING_STATUS1_ERROR;
			}
		} while (!L2_DSP_Read_DMem16(DSP_RampDownComplete)); //H1226 DSP_RampDownComplete = 0x3FB8, indicates RampDown OK or NOT
	}

	return tc_Ret;
}


U8 Music_ResumeCmd(void)
{
    Music_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);

    if(L2_DSP_SendCommandSet(DCMD_Play) != DCMD_Play)
    {
        return DSP_PLAY_COMMAND_ERROR;
    }
    return DSP_SUCCESS;
}


U8 Music_StopCmd(U8 tc_FileHandle)
{
	data	U8  tc_Ret;
	data	U8	tc_value;
	data	U16 tw_temp;
	data	U16 dbgTmp;

    Music_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);
    tc_Ret = DSP_StopCmd();

    // Reset decode status
	tw_temp=0xffff;
	do
	{
		tw_temp --;
		if(tw_temp==0)
		{
			tc_Ret = DSP_DECODE_STATUS_TIMEOUT_ERROR;
			break;
		}
	}while(!((L2_DSP_Read_DMem16(DSP_DecodeStatus)&0x00C0)==0x00C0));  //wait file end

	tw_temp=L2_DSP_Read_DMem16(DSP_DecodeStatus);
	L2_DSP_Write_DMem16(DSP_DecodeStatus,tw_temp&0xFF3F);

    // Close current file
	DOS_Close_File_r(tc_FileHandle);

	// wait for ramp down ok and Turn off DSP clock, ycc 081113
	tw_temp=0xFFFF;
	while(1)
	{
		dbgTmp=L2_DSP_Read_DMem16(DSP_RestartFlag);  //read DSP 0x3F0B.0 ready status
		tw_temp--;     
		if((tw_temp==0)||(dbgTmp==0))
		{           
			break; 
		} 
	}
	tc_value=XBYTE[0xB010] & 0xFE;
	XBYTE[0xB010] = tc_value;   

	return tc_Ret;
}

//===============  UI flow  =================
void Idle_Disp_Icon()
{   
    gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;

    if(gc_Play_FileType==0)
    {
        gs_System_State.w_BitRate=MP3_Bitrate_Parser();
        gw_TotalSec = MP3_Total_Time_Parser();//(JC)H0630 LCM test
    }
    else if(gc_Play_FileType==1)
    {
        WMA_ASF_Parser();
    }

    DOS_GetLongFileName(0, gc_FileLongName);
}


void play_idle()
{
    
}


void UI_LRCTrigger(U8 tc_line,U8 width,U16 index,struct string_view_t code * p_str)
{
	U8 tc_y;

	tc_y= p_str->c_view_y +  p_str->c_row_spacing*tc_line;
	LCM_set_view(TFT_HZKDispDir, p_str->c_view_x, tc_y, p_str->c_view_width, 16); 
	TFT_disp_SongName(width,&(gc_BmpString[index]));
}


void UI_FulllcdDis_Lrc()//if the filename beyond the lcd ,so rolling ,the same with lrc
{
	code U8 Lrc_NoContent[11]={0x20,0x20,0x20,0x20,0x4E,0x6F,0x20,0x4C,0x52,0x43};

	TFT_LRCBackGround();
	if(gb_LrcFileName_Exist==1)
	{
		LRC_DisplayString(&gc_LrcDisplayBuf[0],gc_LrcCurrentLen,0);	//ROW 0	
		LRC_DisplayString(&gc_LrcDisplayBuf[60],gc_LrcCurrentLenNext,1); //ROW 1		
	}
	else
	{
		LRC_DisplayString(&Lrc_NoContent,10,0); //ROW 0	
	}
}


U8 LRC_DisplayString(U8 *tpc_DataBuf, U8 tw_nByte,U8 tc_RowAddr) 
{
	data	U8	i=0;
	data	U8	tc_line=0;
	data	U8	line_limit;
	data	U8	tc_ColumnNum;
	data	U16 tw_Width;
	data	U16 tw_WidthPre=0;
	data	U16 tw_BmpInxPre;
	data	U16 tw_BmpInx=0;	
	code struct string_view_t tp_lrc[2]={ 8, 4,144,20,		//for Low1 
									      8,68,144,20};		//foe Low2

	gb_LRCDispLevel=(bit)tc_RowAddr;

	gw_BmpInx 		=0;
	tw_Width 		=0;  
	line_limit= tp_lrc[tc_RowAddr].c_view_width;

	while (i<tw_nByte)
	{
		tc_ColumnNum = UNICODE_HZK_GET_BMP(*(tpc_DataBuf+i),*(tpc_DataBuf+i+1),&(gc_BmpString[gw_BmpInx]),1);//for ANSI only

		tw_Width += (tc_ColumnNum&0x7f);


		if(tw_Width > line_limit )
		{
			UI_LRCTrigger(tc_line,tw_WidthPre,tw_BmpInx,&tp_lrc[tc_RowAddr]);
			tc_line++;
			if(tc_line>=D_LrcNum)
			{
				return(tw_nByte-i);				
			}
			tw_BmpInx=tw_BmpInxPre; //record new BMP Index

			tw_Width=tc_ColumnNum&0x7f;
		}
		tw_WidthPre=tw_Width;
		tw_BmpInxPre=gw_BmpInx;

		if (tc_ColumnNum&0x80)
		{
			i+=2;
		}
		else
		{
			i+=1;
		}
	}

	while(tc_line<D_LrcNum)
	{
		UI_LRCTrigger(tc_line,tw_WidthPre,tw_BmpInx,&tp_lrc[tc_RowAddr]);
		tw_WidthPre=0;
		tc_line++;
	}
}


void play_proc()
{
    bit	tb_settime_status;

    gc_KeyDet_Mask=0;  //20090107 chiayen modify

//    if ( (gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_B) || (gs_File_FCB[gs_System_State.c_FileHandle].dw_File_DataPoint<=gs_DSP_GLOBAL_RAM.sdw_File_BDataPoint) )
    {   // normal play
		gw_Disp_CurrentSec=(*Music_ReadTime[gc_Play_FileType])();
			
		if(gw_Disp_CurrentSec!=gw_PrevTimeSec)
		{
			gw_PrevTimeSec=gw_Disp_CurrentSec;
			tb_settime_status=0;
		}
		else
		{
			tb_settime_status=1;
		}
     
        if(gb_LrcFileName_Exist==1)
        {
            if(!tb_settime_status)
            {
                Lyric_GetTimeStamp();
                if(gb_LrcGetTimeOrNot)
                {
                    gb_LrcGetTimeOrNot=0;
                    Lyric_GetLyrics();

					if(gb_LrcGetTimeNextOrNot)
					{   
						gb_LrcGetTimeNextOrNot=0;
						Lyric_NextGetLyrics();
					}

					if((gw_IR_Timer==0) && (gc_ShowTimer==0))
					{
						UI_FulllcdDis_Lrc();
					}
				}
            }
        }
       
        if ((*Music_Data_In[gc_Play_FileType])()!=0)// 0727
        {
            gc_PhaseInx = 2;    //End of File for play
            return;//(JC)H0620
        }
        
        if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_B)
        {
            if(gc_RepPlayMode==4)
            {
                if((*Music_ReadTime[gc_Play_FileType])()>10)
                {
                    gc_PhaseInx = 2;
                    return;
                }
            }
        }
    }

	if(gc_EQChangeFlag==1) //20090106 chiayen add
	{
		Music_EQ_Cmd(gs_DSP_GLOBAL_RAM.sc_EQ_Type);  
		gc_EQChangeFlag=0;
	} 
}


void play_playpause()
{ 
	U32 tdw_File_StartCluster;  //20090106 chiayen add
	U32 tdw_FDB_LogAdd;  //20090106 chiayen add
 	U8 i;
	gc_PlayMenu_IR=0;

	#if BankingCode_Recover
    if(gbt_Code_NeedRecover)
	{
		U8	tc_Reg_Status,tc_CE;
//		dbprintf("code recover \n");
		tc_Reg_Status =DEVICE_REG[0x00];
		DEVICE_REG[0x00]= 0x01; 
		tc_CE= FDBP.cFDev;
	  	for(gc_Global_I =0; gc_Global_I<gc_CodeBlock_Num;gc_Global_I++)
		{
			gw_SourceBlock = Bank_Block_A[gc_Global_I];
			gw_TargetBlock = Bank_Block_B[gc_Global_I];
			FDBP.cFDev=0;
			FDBP.cFPlane=0;
			FDBP.wFBlock = gw_TargetBlock;
			Flash_EraseOneBlock();			
			FLASH_Backup_Blcok(0,NAND_INFO.wPAGE_NUM,4,0);// 4-->bank  mark 
		}
		gbt_Code_NeedRecover=0;
		DEVICE_REG[0x00]=tc_Reg_Status ;
		FDBP.cFDev = tc_CE;
	}
	#endif

    if(gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode!=REPEAT_AB_NULL)
    {
        gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode=REPEAT_AB_NULL;
        gc_PhaseInx=1;
        return;
    }
    if(gs_System_State.c_Phase == TASK_PHASE_PLAYACT)
    {
        Music_PauseCmd();
        gs_System_State.c_Phase = TASK_PHASE_PAUSE;
    }
    else if(gs_System_State.c_Phase == TASK_PHASE_PAUSE)
    {
        Music_ResumeCmd();
        gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
    }
    else if ((gs_System_State.c_Phase == TASK_PHASE_FASTFWD)||(gs_System_State.c_Phase==TASK_PHASE_FASTREV))
    {
        gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // reset bitstream address of DM
        gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
    }
    else
    {
        gs_System_State.c_Phase=TASK_PHASE_PLAYACT;
        DOS_GetLongFileName(0,gc_FileLongName);
        gb_LrcFileName_Exist=Lyric_FileSearch();//(JC)Lyric file exist?
	for(i=0;i<100;i++) //sunzhk add
        { 
           gc_LrcDisplayBuf[i]=0x20; 		
        } 


		if(gb_LrcFileName_Exist==1)
		{
			TFT_LRCBackGround();
		}
        gc_Play_FileType=gs_File_FCB[gs_System_State.c_FileHandle].c_FileType;
        
        if(gc_Play_FileType==0)//(JC)MP3
        {
			if(gb_LrcFileName_Exist==1)
			{
				set_clock_mode(CLOCK_MODE_MP3L);
			}
			else
			{
            	set_clock_mode(CLOCK_MODE_MP3);
			}
        }
        else if((gc_Play_FileType==1))//(JC)WMA //Jimi: gc_Play_FileType==2 ==>WMV
        {
            set_clock_mode(CLOCK_MODE_WMA);
        }

		Music_WakeUp(DSP_CLK_CTRL | SRAM2T_CTRL);

		if((*Music_Download[gc_Play_FileType])())    // 材Ω磅︽璶更 IM, PM    //Jimi Yu 080509
        {
            gc_PhaseInx=2;                          // when wma 
            return; 
        }
        gs_DSP_GLOBAL_RAM.sc_DM_Index = 0;  // reset bitstream address of DM
		gw_Disp_CurrentSec=0;

		if(gc_VolumeMute==0)
		{
        	Music_SetVolumeCmd();
		}
		else
		{
			L2_DSP_Write_DMem16(DSP_VolumeControl,0);//20090812 chiayen add VOL=0 	
		}

		//Jimi 081208
		L2_DSP_Write_DMem16(DSP_PostProcessSelect, L2_DSP_Read_DMem16(DSP_PostProcessSelect)|0x0020);
		{
			U8 tc_timeout = 0xFF;
			while( (L2_DSP_Read_DMem16(DSP_PostProcessSelect)&0x0020))	//Handshake with DSP to make sure that DSP has ramp digital volume up.
			{
				if (!(tc_timeout--))
					break;
			}
		}

        Music_EQ_Cmd(gs_DSP_GLOBAL_RAM.sc_EQ_Type);

		if(gb_DirPlay_Flag==1)  //20090106 chiayen add
        {
			tdw_File_StartCluster=gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster;
			tdw_FDB_LogAdd=gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_LogAdd;
			DOS_Search_File(C_File_OneDir|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);
			gs_File_FCB[gs_System_State.c_FileHandle].dw_File_StartCluster=tdw_File_StartCluster;
			gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_LogAdd=tdw_FDB_LogAdd;
		}

        if(gc_Play_FileType==0)
        {
            gs_System_State.w_BitRate=MP3_Bitrate_Parser();
            gw_TotalSec = MP3_Total_Time_Parser();//(JC)H0630 LCM test
        }
        else if(gc_Play_FileType==1)
        {
            WMA_ASF_Parser();
        }


        if(Music_PlayCmd()==DSP_PLAY_COMMAND_ERROR)
        {
            //(JC)H0521; pls check if error handle needed
            gc_PhaseInx=5;  //20081219 chiayen modify for 0Byte mp3
            return;
        }
        gs_DSP_GLOBAL_RAM.s_DSP_RAM.sc_RepeatMode = REPEAT_AB_NULL; // –Ω穝冀Ρヘ常璶р RepeatAB 竚
    }

	gc_ShowTimer=0;
	gc_TuneVolFreqStatus=0;
    gc_PhaseInx=1;
}



void play_stop()
{
    if(gs_System_State.c_Phase != TASK_PHASE_STOP)
    {           
        U8 tc_Ret = Music_StopCmd(gs_System_State.c_FileHandle);
        gs_System_State.c_Phase = TASK_PHASE_STOP;
        gc_PhaseInx=9;
    }
}


void RandomGetFileIndex(void)
{
    xdata	U16 tw_Num;

    tw_Num=(gw_Random_Timer%gw_FileTotalNumber[gs_System_State.c_FileHandle])+1;
    
    if(gw_FileIndex[gs_System_State.c_FileHandle]==tw_Num)
    {
        gw_FileSkipNumber = 0;
    }
    else
    {
        if (gw_FileIndex[gs_System_State.c_FileHandle] > tw_Num)
        {
            gw_FileSkipNumber=gw_FileTotalNumber[gs_System_State.c_FileHandle] - (gw_FileIndex[gs_System_State.c_FileHandle] - tw_Num);
        }
        else
        {
            gw_FileSkipNumber=(tw_Num - gw_FileIndex[gs_System_State.c_FileHandle]);
        }
    }
}


void play_endfile()
{
    U32 tdw_FDB_LogAddr;
    
    play_SetTime2DisplayBuf((*Music_ReadTime[gc_Play_FileType])());//Jimi 080804

    if( (*Music_EOF_Proc[gc_Play_FileType])() ) //Jimi 080522
    {
        play_stop();
        //============== test cyclic ==============
        if(gc_RepPlayMode==C_NoRepeat)//(JC)no repeat
        {
            //gc_PhaseInx=9;
            if(gw_FileIndex[gs_System_State.c_FileHandle]==gw_FileTotalNumber[gs_System_State.c_FileHandle])
            {
                //gc_PhaseInx=9;
				gw_Disp_CurrentSecBak=0xFFFF;
				gw_Disp_CurrentSec=0;
  			    gc_PhaseInx=C_PlayNext;
            }
			else
			{
				if(gb_DirPlay_Flag==1) //20090107 chiayen add
				{
					DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
				}
				else
				{
	            	DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType,C_CmpExtName|C_Next);
				}
	            gc_PhaseInx=3;				
			}  
        }
        else if(gc_RepPlayMode==C_RepeatOne)//(JC)repeat one
        {
            gc_PhaseInx=3;      
        }
        else if(gc_RepPlayMode==C_RepeatAll||gc_RepPlayMode==C_IntroPlay)//(JC)repeat all || introduction
        {
			if(gb_DirPlay_Flag==1) //20090107 chiayen add
			{
				DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
			}
			else
			{
            	DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType,C_CmpExtName|C_Next);
			}
            gc_PhaseInx=3;
        }
        else if(gc_RepPlayMode==C_RandomPlay)//(JC)random
        {
            RandomGetFileIndex();
			if(gb_DirPlay_Flag==1) //20081226 chiayen add
			{
				gb_TriggerFileSkip=1;
				DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
				gb_TriggerFileSkip=0;
			}
			else
			{
				DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
			}
            gc_PhaseInx=3;
        }
        else if(gc_RepPlayMode==C_InDirRepeat)//(JC)repeat in a dir
        {
            DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
            gc_PhaseInx=3;
        }
        else if(gc_RepPlayMode==C_InDirPlay)
        {
            tdw_FDB_LogAddr=gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_LogAdd;
            DOS_Search_File(C_File_OneDir|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);
            gs_File_FCB[gs_System_State.c_FileHandle].dw_FDB_LogAdd=tdw_FDB_LogAddr;
            if(gw_FileIndex[gs_System_State.c_FileHandle]==gw_FileTotalNumber[gs_System_State.c_FileHandle])
            {
                gc_PhaseInx=1;  
            }
            else
            {
                DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
                gc_PhaseInx=3;
            }   
        }
        //============== test cyclic ============== 
    }
}


void play_next()
{
    data	U8 tc_playmode_status;

	gw_CurrentSec = 0;
    tc_playmode_status=gc_RepPlayMode;
    if(gb_PickSong==1 && gc_RepPlayMode==C_RandomPlay)
    {
        gc_RepPlayMode=2;
    }

    if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        if(gw_FileIndex[gs_System_State.c_FileHandle]==0)
		{
            return;//if no this line,error will happen
		}
        gc_PhaseInx=9;        
    }
    else if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
    {
        gc_PhaseInx=3;
        gc_KeyDet_Mask=1;//(JC)H0508 avoid key-press to change phase index  //20090107 chiayen modify  
    }
    else if(gs_System_State.c_Phase==TASK_PHASE_PAUSE)
    {
        gc_PhaseInx=9;
    }

    if(gs_System_State.c_Phase!=TASK_PHASE_STOP)//(JC)H0505
    {
        U8 tc_Ret = Music_StopCmd(gs_System_State.c_FileHandle);
        gs_System_State.c_Phase=TASK_PHASE_STOP;
    }

    if((gc_RepPlayMode==C_InDirRepeat)||(gc_RepPlayMode==C_InDirPlay))
    {
        DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);  
    }
    else if(gc_RepPlayMode==C_RandomPlay)//Ching 080805
    {
        RandomGetFileIndex();
		if(gb_DirPlay_Flag==1) //20081226 chiayen add
		{
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
			gb_TriggerFileSkip=0;
		}
		else
		{
	        DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
	    }
    }
    else
    {		
		if(gb_DirPlay_Flag==1) //20090107 chiayen add
		{
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
			gb_TriggerFileSkip=0;
		}
	    else
	    {
	        DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
	    }
    }

    gs_System_State.c_Phase=TASK_PHASE_STOP;
	gc_TuneVolFreqStatus=0;
    gc_RepPlayMode=tc_playmode_status;
    Idle_Disp_Icon();
}


void play_prev()
{
	gw_CurrentSec = 0;
    if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        if(gw_FileIndex[gs_System_State.c_FileHandle]==0)
		{
            return;//if no this line,error will happen
		}
        gc_PhaseInx=9;        
    }
    else if(gs_System_State.c_Phase==TASK_PHASE_PLAYACT)
    {
        gc_PhaseInx=3;
        gc_KeyDet_Mask=1;//(JC)H0508 avoid key-press to change phase index //20090107 chiayen modify     
    }
    else if(gs_System_State.c_Phase==TASK_PHASE_PAUSE)
    {
        gc_PhaseInx=9;        
    }
    
    if(gs_System_State.c_Phase!=TASK_PHASE_STOP)//(JC)H0505
    {
        U8 tc_Ret = Music_StopCmd(gs_System_State.c_FileHandle);
        gs_System_State.c_Phase=TASK_PHASE_STOP;
    }

    if((gc_RepPlayMode==C_InDirRepeat)||(gc_RepPlayMode==C_InDirPlay))
    {
        DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Prev);
    }
    else if(gc_RepPlayMode==C_RandomPlay)//Ching 080805
    {
        RandomGetFileIndex();
		if(gb_DirPlay_Flag==1) //20090107 chiayen add
		{
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
			gb_TriggerFileSkip=0;
		}
		else
		{
	        DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Next);
	    }
    }
    else
    {
		if(gb_DirPlay_Flag==1) //20090107 chiayen add 
		{
			gb_TriggerFileSkip=1;
			DOS_Search_File(C_File_OneDir|C_By_FDB, C_MusicFileType, C_CmpExtName|C_Prev);
			gb_TriggerFileSkip=0;
		}
	    else
	    {
	        DOS_Search_File(C_File_All|C_By_FDB, C_MusicFileType,C_CmpExtName|C_Prev);
	    }
    }

    gs_System_State.c_Phase=TASK_PHASE_STOP;

	gc_TuneVolFreqStatus=0;
    Idle_Disp_Icon();
}

void play_volup()
{
    gw_LogData_Timer=60; 
    gc_ShowTimer=72;       
    gb_Frequency_Song=1;    // 0=Show Frequency    1=Show Song Number/EQ
    gc_SelectVol=1;         // 1=Show Vol
    gb_SelectEQ=0;          // 0=Show Song Number  1=Show EQ
    gc_VolumeMute=0;

    if (gs_DSP_GLOBAL_RAM.sw_Volume<50)
    {
      gs_DSP_GLOBAL_RAM.sw_Volume+=2;
      L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sw_Volume);     // Vloume Range:0~63
    }
	gc_TuneVolFreqStatus=1;
    gc_PhaseInx=1;
}


void play_voldn()
{
    gw_LogData_Timer=60;
    gc_ShowTimer=72;
    gb_Frequency_Song=1;    // 0=Show Frequency    1=Show Song Number/EQ
    gc_SelectVol=1;         // 1=Show Vol
    gb_SelectEQ=0;          // 0=Show Song Number  1=Show EQ
    gc_VolumeMute=0;
    if (gs_DSP_GLOBAL_RAM.sw_Volume>1)
    {
        gs_DSP_GLOBAL_RAM.sw_Volume-=2;
		L2_DSP_Write_DMem16(DSP_VolumeControl,gs_DSP_GLOBAL_RAM.sw_Volume);     // Vloume Range:0~63
    }
	gc_TuneVolFreqStatus=1;
    gc_PhaseInx=1;  
}


void play_back2uplevel()
{
    if((gs_System_State.c_Phase==TASK_PHASE_PLAYACT)||(gs_System_State.c_Phase==TASK_PHASE_PAUSE))
    {
        play_stop();    
    }
    gc_Task_Next=C_Task_Menu;
    gw_init_needed |= SET_BIT8;
    gc_PhaseInx=0;
}


UBYTE play_SetTime2DisplayBuf(U16 tw_TimeSec)
{
    if(gw_PrevTimeSec != tw_TimeSec)
    {
        gw_PrevTimeSec=tw_TimeSec;
        return 0;
    }
    return 1;
}


void FM_FREQ_CHGUP(void) //chiayen0807
{
    FM_FREQ_CHG(1,1);
    if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        gc_PhaseInx=0x09;   
    }
    else
    {
        gc_PhaseInx=1;
    }
}


void FM_FREQ_CHGDOWN(void) //chiayen0807
{
    FM_FREQ_CHG(0,1);
    if(gs_System_State.c_Phase==TASK_PHASE_STOP)
    {
        gc_PhaseInx=0x09;   
    }
    else
    {
        gc_PhaseInx=1;
    }
}


void IR_Service_Process()
{
    if(gc_IRCmdStatus!=0)
    {
		if(gc_IRCmdStatus==1)  //20090216 chiayen add
		{
			gc_Dirchange_Timer=0;
		}
        ir_service();
    }

    if((gw_IR_Timer==0) && (gw_irkey_count!=0))
    {
        if(gw_FileIndex[0]<gw_irkey_count)
        {
            gw_FileSkipNumber=(gw_irkey_count-gw_FileIndex[0]);
            play_next();
        }
        else if(gw_FileIndex[0]>gw_irkey_count)
        {
            gw_FileSkipNumber=(gw_FileTotalNumber[0]-gw_FileIndex[0])+gw_irkey_count;
            play_next();
        }

        gw_irkey_count=0;
    }	
}


void Play_SDFlash_DosInit()
{
	U8  tc_clock_mode_backup;  //20090526 chiayen add
	tc_clock_mode_backup=gc_clock_mode;

	if (DOS_Initialize())
	{
		// Initial Fail
		XBYTE[0xB400]= 0x01;
		gc_PhaseInx=9;
		gc_Dosinitfail=1;
		gdw_HOSTStartSectorRead=0xFFFFFFF0; //20090803 chiayen add
	}
	else
	{		
		gb_FindFlag = 0;
		gc_PhaseInx=0;
		gw_init_needed=0xFFFF;
		if((gc_DirReBuildFlag != D_Valid) && (gc_CurrentCard==0))
		{
			set_clock_mode(CLOCK_MODE_MJPEG);  //20090803 chiayen move here for HOST 
			DOS_DIRtable();  //20090216 chiayen add
			set_clock_mode(tc_clock_mode_backup);  //20090526 chiayen add
		}       
		gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;           
		DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);//(JC)count music file no. in root
		gb_TFT_refresh=1;
		gc_DispWallpaper=0xFF;
		gc_ShowTimer=0;
		gc_TuneVolFreqStatus=0;
	}
}


void Play_SourceDetect_Process()
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

    if(gc_CurrentCard_backup!=gc_CurrentCard)  //auto  //20090803 chiayen modify
    {
		U8 tc_CurrentCard;//20090803 chiayen add for media change
		tc_CurrentCard=gc_CurrentCard; //20090803 chiayen add for media change	
		gc_CurrentCard=gc_CurrentCard_backup;  //20090803 chiayen add for media change
		play_stop();
		gc_CurrentCard=tc_CurrentCard; //20090803 chiayen add for media change
		set_clock_mode(CLOCK_MODE_MP3); //20090803 chiayen add for media change

		if(gc_CurrentCard==2)
        {           
            if(SD_Identification_Flow())
            {
                gc_CardExist |=0x02;
            }
            else if((gb_Host_Exist==1) && (gc_HostNoFileflag==0)) //20090730 chiayen modify
            {
				XBYTE[0xB400]= 0x01;  //20090730 chiayen add
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
				XBYTE[0xB400]= 0x01;  //20090730 chiayen add
				gc_CurrentCard=0;
				gc_CardExist &=0xFD;
				gb_FindFlag = 0;
				InitFlash();
			}
			Play_SDFlash_DosInit();
        }
	  	else if(gc_CurrentCard==CURRENT_MEDIA_HOST)
		{
			XBYTE[0xB400]= 0x01;  //20090803 chiayen add			
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
					gc_CurrentCard=0;
					gc_CardExist &=0xFD;
					gb_FindFlag = 0;
					InitFlash();
				}
			}
			else 
        	{
				gc_CurrentCard=0;
				gb_FindFlag = 0;
				InitFlash();
			}
			Play_SDFlash_DosInit();
		}
		else if(gc_CurrentCard==0)
        {
			gc_CardExist &=0xFD;
			InitFlash();
			Play_SDFlash_DosInit();
		}
    }
}


void MediaChange_Play(void)  //20090803 chiayen add
{
	if(gc_Dosinitfail==0)  //20090818 chiayen add
	{
		if((gb_SD_Exist==1) && (gb_SDNoFileflag==0))
		{
			gc_CurrentCard=2;	
		}
		else
		{
			gc_CurrentCard=0;
		}
	}
}


void LCM_Display_Func1(void)
{
	// For WMA file size=0
	if(((gc_Play_FileType==1)&&(gs_System_State.c_Phase == TASK_PHASE_PLAYACT))&&(((gb_PickSong==1)||(gb_ChannelSet==1)) || gc_CurrentCard!=5))
	{
		if(gs_File_FCB[0].dw_File_TotalSize==0)
		{
			gc_PhaseInx=5;
			return;
		}
		WMA_DataIn();
	}

	// maxliao-20090602
	if(gdw_FreeClusterNum[1] < (gdw_DOS_FatMaxCluster - 1)) 
	{
		if(gc_loopcount==20)
		{         
//			DOS_SearchFreeCluster(1);
			gc_loopcount=0;
		}
		gc_loopcount++; 
	} 

	if(gb_LrcFileName_Exist==1)
	{
		return;
	}

	// 冀礶
	if(gc_DispWallpaper!=1)
	{
		gc_DispWallpaper=1;
		gb_TFT_refresh=1;
		TFT_Main();
	}

	// 礶繵瞯
	if((gw_DisplayFreq!=gw_FM_frequency)||(gb_TFT_refresh==1))
	{
		gb_FlashNoFileflag=0;
		gw_DisplayFreq=gw_FM_frequency;
		TFT_ShowFMFrequency();            
	}	

	// 郎Α:MP3/WMA/APE
	if((gc_Media_type!=gc_Play_FileType)||(gb_TFT_refresh==1))
	{
		gc_Media_type=gc_Play_FileType;
		TFT_ShowMusicType();
	}

	// 纗杆竚:NAND/U-disk/SDC
	if((gc_CurrentCard_Bak!=gc_CurrentCard)||(gb_TFT_refresh==1))
	{
		gc_CurrentCard_Bak=gc_CurrentCard;
		TFT_ShowMediaIcon();
	}

	// Show Play Mode:Normal/Repeat 1/Repeat All
	if((gc_DisplayPlayMode!=gc_RepPlayMode)||(gb_TFT_refresh==1))
	{
		gc_DisplayPlayMode=gc_RepPlayMode;
		TFT_ShowPlayMode();
	}

	// 冀Ρ计			     	
	if((gw_DispSongNum!=gw_FileIndex[0])||(gb_TFT_refresh==1))
	{
		gw_DispSongNum=gw_FileIndex[0];	
		TFT_ShowSongIndex();
	}

	// 羆Ρ计
	if((gw_DispTotalSong!=gw_FileTotalNumber[0])||(gb_TFT_refresh==1))
	{
		gw_DispTotalSong=gw_FileTotalNumber[0];	
		TFT_ShowTotalSong();
	}

	// Show EQ icon
	if((gc_DisplayEQIcon!=gs_DSP_GLOBAL_RAM.sc_EQ_Type)||(gb_TFT_refresh==1))
	{
		gc_EQBak=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
		gc_DisplayEQIcon=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
		TFT_ShowEQIcon();
	}

	// 冀篈:Play/Pause
	if((gc_DispPlayStatus!=gs_System_State.c_Phase)||(gb_TFT_refresh==1))
	{
		gc_DispPlayStatus=gs_System_State.c_Phase;
		TFT_ShowPlayPauseIcon();
	}

	// 讽玡冀丁
	if((gw_Disp_CurrentSecBak!=gw_Disp_CurrentSec)||(gb_TFT_refresh==1))
	{	
		gw_Disp_CurrentSecBak=gw_Disp_CurrentSec;
		TFT_ShowPlayTime();
 	}
#ifndef TFT_18_V      //sunzhk add 090617
    // 簈Ρ丁
	if((gw_DispTotalTime!=gw_TotalSec)||(gb_TFT_refresh==1))
	{
		gw_DispTotalTime=gw_TotalSec;
		TFT_ShowSongLength();
	}
#endif
	// Show BitRate
	if(((gw_DisplayBitRate!=gs_System_State.w_BitRate)&&(gw_DisplayBitrateTime==0))||(gb_TFT_refresh==1))
	{
		gw_DisplayBitRate=gs_System_State.w_BitRate;
		gw_DisplayBitrateTime=30;
		TFT_ShowBitRate();
	}

	// Display Song name
	if(gw_FileTotalNumber[0]==0)
	{			
		if((gc_CurrentCard==0)  && (gb_FlashNoFileflag==0))
		{
			gb_FlashNoFileflag=1;
		}
	}
	else
	{
		if((gw_DispSongNum1!=gw_FileIndex[0])||(gb_TFT_refresh==1))
		{
			U8	i;

			if((gc_CurrentCard==2) && (SD_Detect!=0))  //if SD not exist return DOS_CLUSTER_LINK_ERR chiayen0813
			{
				return;		
			}

			if((gc_CurrentCard==5) && (gb_HostConnect==0))  //if Host not exist return DOS_CLUSTER_LINK_ERR chiayen 20081017
			{
				return;		
			}

			for(i=0;i<48;i++)
			{
				gc_FileLongName[i]=0;
			}
			gw_DispSongNum1=gw_FileIndex[0];
			DOS_GetLongFileName(0,gc_FileLongName);

			gw_LogData_Timer=2;  //20090107 chiayen modify 
			gs_str_scroll_state.c_str_1st_char=0;  //20090331
		    gs_str_scroll_state.c_shift_in_1st_char=0;
   			LCD_Disp_FileName(&gc_FileLongName[5], gc_FileLongName[2], gc_FileLongName[4], 1);
			gw_LCMScrollTimer=40;
		}

		if((gw_LCMScrollTimer==0)&&(gb_TFT_scroll==1))
		{
			if(gb_LrcFileName_Exist==0)
			{
				LCD_Disp_FileName(&gc_FileLongName[5], gc_FileLongName[2], gc_FileLongName[4], 1);
			}
			gw_LCMScrollTimer=20;
		}					
	}

	if(((gc_Play_FileType==1)&&(gs_System_State.c_Phase == TASK_PHASE_PLAYACT))&&(((gb_PickSong==1)||(gb_ChannelSet==1)) || gc_CurrentCard!=5))
	{
		WMA_DataIn();
	}

	gb_TFT_refresh=0;
}


void RepeatMode_Process()
{
	bit	tc_Confirm=0;

	if(gc_PhaseInx)
	{
        switch(gc_PhaseInx)
        {
            case C_PlayMenu:
				gc_ShowTimer =150;
				gc_refresh_type=REFRESH_NOT;//该项确懧后不需要刷新菜单	、、sunzhkadd 00703
				tc_Confirm=1;
            break;

            case C_PlayNext:
				gc_ShowTimer =150;
				gc_refresh_type=REFRESH_NEXT;
				if(gc_LCMDispIndex==2)
				{
					gc_LCMDispIndex=0;
				}
				else
				{
					gc_LCMDispIndex++;
				}
            break;

            case C_PlayPrev:
				gc_ShowTimer =150;
				gc_refresh_type=REFRESH_PREV;
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
	
		if(tc_Confirm==1) //enter child dir or confirm selected item
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
			if(gb_LrcFileName_Exist==1)
			{
				UI_FulllcdDis_Lrc();
				gc_DispWallpaper=0;
			}
		}
	}
}	


void EQMenu_Process()
{
	bit tb_Confirm=0;

	if(gc_PhaseInx!=1)
	{
        switch(gc_PhaseInx)
        {
            case C_PlayMenu:
				gc_ShowTimer =150;
				gc_refresh_type=REFRESH_NOT;
				tb_Confirm = 1;
            break;

            case C_PlayNext:
				gc_ShowTimer =150;
				gc_refresh_type=REFRESH_NEXT;
				if(gc_LCMDispIndex==5)
				{
					gc_LCMDispIndex=0;
				}
				else
				{
					gc_LCMDispIndex++;
				}
            break;

            case C_PlayPrev:
				gc_ShowTimer =150;
				gc_refresh_type=REFRESH_PREV;
				if(gc_LCMDispIndex==0)
				{
					gc_LCMDispIndex=5;
				}
				else
				{
					gc_LCMDispIndex--;
				}
            break;
        }
	
		if(gs_DSP_GLOBAL_RAM.sc_EQ_Type!=gc_LCMDispIndex)
		{
			gs_DSP_GLOBAL_RAM.sc_EQ_Type=gc_LCMDispIndex;
			gc_EQChangeFlag=1;
		}

		if(tb_Confirm==1) //enter child dir or confirm selected item
		{
			gs_DSP_GLOBAL_RAM.sc_EQ_Type=gc_LCMDispIndex;
			gc_ShowTimer=0;
			gc_TuneVolFreqStatus=0;
			if(gb_LrcFileName_Exist==1)
			{
				UI_FulllcdDis_Lrc();
				gc_DispWallpaper=0;
			}
		}
	}
}	


void PlayMenu_Process()
{
	bit tb_Confirm=0;

	gc_refresh_type=REFRESH_ALL;
	if(gc_PhaseInx!=1)
	{
        switch(gc_PhaseInx)
        {
            case C_PlayMenu:
				gc_refresh_type=REFRESH_ALL;
				tb_Confirm=1;
				gc_ShowTimer =150;
            break;

            case C_PlayNext:
				gc_ShowTimer =150;
				gc_refresh_type=REFRESH_NEXT;
				if(gc_LCMDispIndex==4)
				{
					gc_LCMDispIndex=0;
				}
				else
				{
					gc_LCMDispIndex++;
				}
            break;

            case C_PlayPrev:
				gc_ShowTimer =150;
				gc_refresh_type=REFRESH_PREV;
				if(gc_LCMDispIndex==0)
				{
					gc_LCMDispIndex=4;
				}
				else
				{
					gc_LCMDispIndex--;
				}
            break;	
        }

		if(tb_Confirm==1) //confirm selected item
		{
			if(gc_LCMDispIndex==0)  //File Select
			{
     			if(gw_FileTotalNumber[0])
     			{
			   		play_stop();                     //Terry20090527
			   		gc_TaskMode_BkUp=C_Task_Play;    //
			   		gc_Task_Next=C_Task_Dir;         //
     			}
			}
			else if(gc_LCMDispIndex==1)  //Play All
			{
				play_stop();
				gb_DirPlay_Flag=0; //20081224 chiayen test
				gc_PlayModeChange_Flag=1;
				gw_init_needed=0xFFFF;             
				gb_FindFlag = 0;            
				gs_File_FCB[0].dw_FDB_StartCluster = gdw_DOS_RootDirClus;           
				DOS_Search_File(C_File_All|C_Cnt_FileNo, C_MusicFileType, C_CmpExtName|C_Next);//(JC)count music file no. in root
				gc_ShowTimer=0;
				gc_TuneVolFreqStatus=0;
				gc_PhaseInx=0;
			}
			else if(gc_LCMDispIndex==2)  //Equalizer
			{
				gc_DispPlayMenuAct=1;
				gc_TuneVolFreqStatus=4;
			}
			else if(gc_LCMDispIndex==3)  //Repeat
			{
				gc_DispPlayMenuAct=2;
				gc_TuneVolFreqStatus=5;
			}
			else if(gc_LCMDispIndex==4)  //Exit
			{
				gc_ShowTimer=0;
				gc_TuneVolFreqStatus=0;
				if(gb_LrcFileName_Exist==1)
				{
					UI_FulllcdDis_Lrc();
					gc_DispWallpaper=0;
				}
			}
		}
   	}	
}


data	U8	gc_ShowIRNumMain=0;
extern U8 LCD_Disp_FileName(U8 *tpc_DataBuf, U8 tc_ANSI, U8 tc_nByte, U8 tc_scroll_flag);
extern UBYTE DOS_GetDIRName(UBYTE tc_FileHandle, UBYTE tb_UicodeToISN, UBYTE * tpc_LongName);
extern void Folder_Display();
void LCM_Display_Func2(void)	// 匡兜:MP3/MP4/JPG/Setup
{
	// IR NUM
	if(gb_ShowIRNum==1)
	{
		gb_ShowIRNum=0;
		gc_TuneVolFreqStatus=0;
		if(gc_ShowIRNumMain==0)
		{
			gc_ShowIRNumMain=1;
			gb_TFT_refresh=1;
			gc_DispWallpaper=0;
			TFT_IRNumMain();
		}

		TFT_IRNum();
	}

	// EQ Frame(换北竟匡EQ)
	if(gb_Frequency_Song==1)
	{
		if(gc_EQBak!=gs_DSP_GLOBAL_RAM.sc_EQ_Type)
		{
			gc_TuneVolFreqStatus=0;
			gc_EQBak=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
			TFT_EQFrame();
			gb_TFT_refresh=1;
			gc_DispWallpaper=0xFF;
			return;
		}
	}

	if(gc_TuneVolFreqStatus==1)	// VOLUME ADJ.
	{
	    gc_ShowIRNumMain=0;
		if(gc_DispWallpaper!=2)
		{
			gb_TFT_VOL_FM=0;	// 0=VOL adj.  1=FM adj.
			//TFT_Main_VOL_FM_ADJ();
			Menu_Disp_Item_Play(gc_TuneVolFreqStatus,gc_LCMDispIndex,0);//显示音量调整的背景图
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
		if(gc_DispWallpaper!=3)
		{
			gb_TFT_VOL_FM=1;	// 0=VOL adj.  1=FM adj.
			//TFT_Main_VOL_FM_ADJ();
			Menu_Disp_Item_Play(gc_TuneVolFreqStatus,gc_LCMDispIndex,0);//显示音量调整的背景图
			gc_DispWallpaper=3;
			gw_DisplayFreq=0;
		}

		if(gw_DisplayFreq!=gw_FM_frequency)
		{
			gb_FlashNoFileflag=0;
			gw_DisplayFreq=gw_FM_frequency;
			TFT_ShowFMFreqAdj();
			gc_ShowIRNumMain=0;
		}
	}
	else if(gc_TuneVolFreqStatus==3)	// Playing operation(play menu)
	{
		//gc_ShowTimer=72;
		IR_Service_Process_Menu();
		if(gc_DispWallpaper!=4)
		{
			gc_DispWallpaper=4;
			gc_LCMDispIndex=0;
			gc_LCMDispIndexBak=0xFF;
			gc_PhaseInx=1;
		}
		PlayMenu_Process();
		if(gc_LCMDispIndexBak!=gc_LCMDispIndex)
		{
			gc_LCMDispIndexBak=gc_LCMDispIndex;
//		  	TFT_PlayingOperation_new(gc_LCMDispIndex,gc_TuneVolFreqStatus);
			Menu_Disp_Item_Play(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
			
		}		
	}
	else if(gc_TuneVolFreqStatus==4)
	{
		//gc_ShowTimer=72;
		IR_Service_Process_Menu();
		if(gc_DispWallpaper!=5)
		{
			gc_DispWallpaper=5;
			gc_LCMDispIndex=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
			gc_LCMDispIndexBak=0xFF;
			gc_PhaseInx=1;
		}

		EQMenu_Process();		
		if(gc_LCMDispIndexBak!=gc_LCMDispIndex)
		{
			gc_EQBak=gs_DSP_GLOBAL_RAM.sc_EQ_Type;
			gc_LCMDispIndexBak=gc_LCMDispIndex;
			//TFT_EQMenu();
			Menu_Disp_Item_Play(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
		}		
	}
	else if(gc_TuneVolFreqStatus==5)
	{
		//gc_ShowTimer=72;
		IR_Service_Process_Menu();
		if(gc_DispWallpaper!=6)
		{
			gc_DispWallpaper=6;
			gc_LCMDispIndex=0;
			gc_LCMDispIndexBak=0xFF;
			gc_PhaseInx=1;
		}

		RepeatMode_Process();		
		if(gc_LCMDispIndexBak!=gc_LCMDispIndex)
		{
			gc_LCMDispIndexBak=gc_LCMDispIndex;
			Menu_Disp_Item_Play(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
		//	TFT_RepeatMenu();
		}		
	}
//////////////sunzhk add disp folder name
	else if(gc_folder_disp==1)
	{
		gc_ShowTimer=72;
		IR_Service_Process_Menu();
		if(gc_DispWallpaper!=6)
		{
			gc_DispWallpaper=6;
			gc_LCMDispIndex=0;
			gc_LCMDispIndexBak=0xFF;
			gc_PhaseInx=1;
		}

		/*RepeatMode_Process();		
		if(gc_LCMDispIndexBak!=gc_LCMDispIndex)
		{
			gc_LCMDispIndexBak=gc_LCMDispIndex;
			Menu_Disp_Item_Play(gc_TuneVolFreqStatus,gc_LCMDispIndex,gc_refresh_type);
		//	TFT_RepeatMenu();
		}*/
			Folder_Display();
			DOS_GetDIRName(0, 0, gc_FileLongName);			 			
			LCD_Disp_FileName(&gc_FileLongName[5], gc_FileLongName[2], gc_FileLongName[4], 1);
			gc_folder_disp=0;		
	}
/////////
	if(gc_PhaseInx!=0)
	{
		if(gs_System_State.c_Phase==TASK_PHASE_STOP)
	    {
	        gc_PhaseInx=0x09;   
	    }
	    else
	    {
	        gc_PhaseInx=1;
	    }
	}
}


void Play_Task()
{
	gb_TFT_refresh=1;	// ┮ΤICON/计场陪ボΩ
	gc_DispWallpaper=0;
	LCD_BACKLIGHT_ON;
	while(1)
	{
        switch(gc_PhaseInx)
        {
            case C_PlayInit:
                play_init();
            break;

            case C_PlayProc:
                play_proc();
            break;
            
            case C_PlayEndfile:
				if(gb_LrcFileName_Exist==1)
				{
					gb_TFT_refresh=1;
					gc_DispWallpaper=0;
				}
                play_endfile();
            break;

            case C_PlayPause:
				if(!gb_FlashNoFileflag)
				{
	                play_playpause();
				}
				else
				{
					gc_PhaseInx=9;	
				}
            break;

            case C_PlayStop:
                play_stop();    
            break;

            case C_PlayNext:
				if(!gb_FlashNoFileflag)
				{
					if((gb_LrcFileName_Exist==1)&&(gc_ShowTimer==0))
					{
						gb_TFT_refresh=1;
						gc_DispWallpaper=0;
					}

					if(gc_TuneVolFreqStatus==0)
					{
	                	play_next();
					}
					else if(gc_TuneVolFreqStatus==1)
					{
						play_volup();	
					}
					else if(gc_TuneVolFreqStatus==2)
					{
						FM_FREQ_CHGUP();	
					}
				}
            break;

            case C_PlayPrev:
				if(!gb_FlashNoFileflag)
				{
					if((gb_LrcFileName_Exist==1)&&(gc_ShowTimer==0))
					{
						gb_TFT_refresh=1;
						gc_DispWallpaper=0;
					}

					if(gc_TuneVolFreqStatus==0)
					{
						play_prev();
					}
					else if(gc_TuneVolFreqStatus==1)
					{
						play_voldn();	
					}
					else if(gc_TuneVolFreqStatus==2)
					{
						FM_FREQ_CHGDOWN();	
					}					
				}
            break;

            case C_PlayIdle:
                play_idle();
            break;

            case C_PlayVolUp:
                play_volup();
            break;

            case C_PlayVolDn:
                play_voldn();
            break;

            case C_PlayUpLevel:
                play_back2uplevel();
            break;

            case C_PlayMenu:  //short key mode
 		    	gc_ShowTimer=72;
			
				if(gc_DispPlayMenuAct==0&&gc_TuneVolFreqStatus!=3)
				{
					gc_DispWallpaper=0; 
					gc_TuneVolFreqStatus=3;
				}
            break;
			case C_PlayChgDn:
					FM_FREQ_CHG(0,1);
			break;

			case C_PlayChgUp:
					FM_FREQ_CHG(1,1);
			break;
            case C_TuneVolFreq:  //tune vol key
				if(gc_TuneVolFreqStatus<3)  //20090331
				{
					gc_ShowTimer=72;
					gc_TuneVolFreqStatus++;
					if(gc_TuneVolFreqStatus>2)
					{
						gc_ShowTimer=0;
						gc_TuneVolFreqStatus=0;	
					}				
				}

				if(gs_System_State.c_Phase==TASK_PHASE_STOP)
			    {
			        gc_PhaseInx=0x09;   
			    }
			    else
			    {
			        gc_PhaseInx=1;
			    }
            break;

            case C_PowerOff:
				gc_TaskMode_BkUp=C_Task_Play;
				if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
				{
					play_stop();	
				}
				gc_Task_Next=C_Task_PowerOff;  					    	

            break;

			case C_MainMenu: //Long key mode
				play_stop();
				gc_Task_Next=C_Task_Menu;
			break;

            case C_PlayFF:
				UI_fastFFFR_Play(0);
			break;

			case C_PlayFR:
				UI_fastFFFR_Play(1);
            break;

            case C_PlayFfFrEnd:
				play_fffr_end();
			break;
        }

        if((gs_System_State.c_Phase == TASK_PHASE_PLAYACT)  && (gc_Play_FileType!=0x03))
        {
            gs_System_State.w_BitRate = (*Music_Bitrate[gc_Play_FileType])();
        }

		if((gc_Dirchange_Timer==0)&&(gc_ShowTimer==0)&&(gw_IR_Timer==0)&&(gc_folder_disp==0))
		{	
        	LCM_Display_Func1();	// 冀礶		
			gc_DispPlayMenuAct=0;
			gc_ShowIRNumMain=0;
			if((gb_LrcFileName_Exist==1) && ((gc_DispWallpaper==2)||(gc_DispWallpaper==3))) //for LRC Volkey
			{
				UI_FulllcdDis_Lrc();
				gc_DispWallpaper=0;
			}
		}
		else
		{
			LCM_Display_Func2();
		}

		gc_CurrentCard_backup=gc_CurrentCard;
		IR_Service_Process();

		if(gb_PickSongSet==1)
			{		
				gb_PickSongSet=0;
				if(gb_ChannelSet==0)
				{
					PickSong_Process();
				}
				else
				{
					ChannelSet_Process();
					gb_ChannelSet=0;
					gc_ShowIRNumMain=0;
				}
			}
		
        Polling_TaskEvents();
        if(gc_LogDataFlag==1)
        {
            USER_LogFile_ReadWrite(1);
            gc_LogDataFlag=0;
        }

	  	if(gb_HostError==1)  //20090107 chiayen add
		{
			gb_HostError=0;
			MediaChange_Play();
		}
		Play_SourceDetect_Process();

		if(gc_Task_Current!=gc_Task_Next)
        {
            gc_Task_Current=gc_Task_Next;
			gc_ShowTimer=0;  //20090331
			gc_DispWallpaper=1;
			gc_TuneVolFreqStatus=0;
			gc_Dirchange_Timer =0;
			gw_IR_Timer=0;
			gc_folder_disp=0;
			ClearIRBuffer();
            if((gc_Task_Current==C_Task_USB) || (gc_Task_Current==C_Task_Dir) || (gc_Task_Current==C_Task_PlayMenu) || (gc_Task_Current==C_Task_PlayMenu_IR)) //20090107 chiayen add
            {
                if((gs_System_State.c_Phase==TASK_PHASE_PLAYACT)||(gs_System_State.c_Phase==TASK_PHASE_PAUSE))
                {
                    play_stop();    
                }
            }
            break;  
        }
    }
}

void ir_service_menu(void)
{
	U16 tw_temp;
	
	if(gc_IRCmdStatus!=0)
	{
		tw_temp=(IR_REG[0x1b]<<8)+IR_REG[0x1a];
	
		
		if(tw_temp==IR_21_Key)
		{
			gc_irkey=5;	
		}
		else if(tw_temp==IR_21_1_Key)
		{
			gc_irkey=7;	
		}
		if(gc_irkey!=0)
		{
			if((IR_REG[0x1c]+IR_REG[0x1d])==0xFF)
			{
				if(gc_IRCmdStatus==1)
				{
					ir_commandservice_menu();
					gc_IRCmdStatus=0;
				}
				if(gc_IRCmdStatus==2)
				{					
					ir_commandservice_menu();
					gc_IRCmdStatus=0;
				}
			}					
		}
		else
		{
			gc_IRCmdStatus=0;
		}
		gc_IRCmdStatus=0;
	}
	gc_irkey=0;
}


void ir_commandservice_menu(void)
{

  	
	if(gc_irkey==5)
	{
	    switch(IR_REG[0x1c])
	    {
			case 0x1e:								//just for suo lang.
			case 0x01:	//Mode	//Menu   
				if(gc_Task_Current == C_Task_Jpeg)
				gc_PhaseInx = C_JpegMenu;
				else if(gc_Task_Current == C_Task_Mjpeg)
				gc_PhaseInx = C_MjpegMenu;
				else
				gc_PhaseInx = C_PlayMenu;

				break;
	        
			case 0x0d:	// PlayPrev --
				if(gc_Task_Current == C_Task_Jpeg)
				gc_PhaseInx = C_JpegPrev;
				else if(gc_Task_Current == C_Task_Mjpeg)
				gc_PhaseInx = C_MjpegPrev;
				else
				gc_PhaseInx = C_PlayPrev;

				break;
	        case 0x0e:	// PlayNext ++
				if(gc_Task_Current == C_Task_Jpeg)
				gc_PhaseInx = C_JpegNext;
				else if(gc_Task_Current == C_Task_Mjpeg)
				gc_PhaseInx = C_MjpegNext;
				else
				gc_PhaseInx = C_PlayNext;

				break;
			case 0x00:	//ON/OFF 
             
					if(gs_System_State.c_Phase!=TASK_PHASE_STOP)
					{
						if(gc_Task_Current == C_Task_Play)
						{
							play_stop();
						}
						else if(gc_Task_Current == C_Task_Jpeg)
						{
							jpeg_stop();
						}
						else if(gc_Task_Current == C_Task_Mjpeg)
						{
							mjpeg_stop();
						}
					}
                  	gc_Task_Next=C_Task_PowerOff;
					gc_PhaseInx=0xff; 
				

				break;
			default:		
			break;
	    }
	}
	else if(gc_irkey==7)	// 21-key
	{
	    switch(IR_REG[0x1c])
	    {
#if 0
	        case 0x0d:	// Menu
				if(gc_Task_Current == C_Task_Jpeg)
				gc_PhaseInx = C_JpegMenu;
				else if(gc_Task_Current == C_Task_Mjpeg)
				gc_PhaseInx = C_MjpegMenu;
				else
				gc_PhaseInx = C_PlayMenu;

				break;
#endif 
	        case 0x04:	// PlayPrev --
				if(gc_Task_Current == C_Task_Jpeg)
				gc_PhaseInx = C_JpegPrev;
				else if(gc_Task_Current == C_Task_Mjpeg)
				gc_PhaseInx = C_MjpegPrev;
				else
				gc_PhaseInx = C_PlayPrev;

				break;
	        case 0x06:	// PlayNext ++
				if(gc_Task_Current == C_Task_Jpeg)
				gc_PhaseInx = C_JpegNext;
				else if(gc_Task_Current == C_Task_Mjpeg)
				gc_PhaseInx = C_MjpegNext;
				else
				gc_PhaseInx = C_PlayNext;

				break;
			default:		
			break;
	    }		
	}
}


void IR_Service_Process_Menu()
{
    if(gc_IRCmdStatus==1)
    {
        ir_service_menu();
    }
}


