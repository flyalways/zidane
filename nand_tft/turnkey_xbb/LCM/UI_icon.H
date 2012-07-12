#ifndef __UI_MONO_ICON_H_
#define	__UI_MONO_ICON_H_

struct string_view_t {
	U8 c_view_x;
	U8 c_view_y;
	U8 c_view_width;
	U8 c_row_spacing;
};

struct string_disp_t {
	U8 c_x;
	U8 c_y;
	U8 c_width;
	U8 c_reverse_flag;
	U8 xdata * pc_font_bitmap;
};

struct str_scroll_t
{
	U8 c_str_1st_char;
	U8 c_shift_in_1st_char;
};
#endif // !__UI_MONO_ICON_H_

