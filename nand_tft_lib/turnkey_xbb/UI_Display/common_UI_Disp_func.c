#include "..\LCM\LCM_func.h"
#include "..\LCM\UI_icon.h"

extern struct str_scroll_t	gs_str_scroll_state;
extern xdata 	System_Struct gs_System_State;
extern U8 UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tc_ANSI);



#ifdef USE_UI_StringTrigger
static void UI_StringTrigger(struct string_disp_t xdata * p_str)
{
	LCM_set_view(TFT_HZKDispDir, p_str->c_x, p_str->c_y, p_str->c_width, 16); 
	TFT_disp_HZKCharBMP(p_str->c_width, p_str->c_reverse_flag, p_str->pc_font_bitmap);//total width,reverse,string start index
}
#endif

#ifdef USE_LCD_Disp_FileName
static U8 LCD_Disp_FileName(U8 *tpc_DataBuf, U8 tc_ANSI, U8 tc_nByte, U8 tc_scroll_flag)
{	//mode: display mode  offset: reserved for file brwser  tc_nByte:string num tc_ANSI:string type(ANSI or unicode)
	U8 tc_Status = 0;
	struct string_view_t code disp_param = { Start_X_FileName,	Start_Y_FileName,	Px_FileNameWidth,	22};
	struct string_disp_t filename_attrib;
			tc_Status=UI_get_string_hzk_bitmap(&disp_param, //*
								 &(filename_attrib),
								 &(tpc_DataBuf[0]), //* string to display
								 tc_nByte, //* string size in bytes
								 tc_ANSI, //* unicode or ansi flag
								 0, //* line number
								 1,/*truncate on char or not*/ //* 1
								 tc_scroll_flag, //rolling factor
								 &(gc_BmpString[0]));



			UI_render_strings(&disp_param, &filename_attrib);
	return tc_Status;
}
#endif

#ifdef USE_UI_get_string_hzk_bitmap
static U8 UI_get_string_hzk_bitmap(struct string_view_t * p_disp_param,
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
	U8 tc_Status = 0; 
	U8 i = 0;
	U16 tw_Width = 0;
	U8 tc_1st_char = 0;
	U8 tc_shift_start = 0;
	U8 tc_next_1st_char = 0;
	U8 tc_next_shift_start = 0;
	U8 tc_ColumnNum;

	disp_param = *p_disp_param;
	gw_BmpInx 	= 0;

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
#endif

#ifdef USE_UI_render_strings
static void UI_render_strings(struct string_view_t * p_disp_param,
							  struct string_disp_t xdata * p_string_disp_param)
{
	struct string_view_t disp_param;

	disp_param = *p_disp_param;

	UI_StringTrigger(&(p_string_disp_param[0]));
}
#endif



