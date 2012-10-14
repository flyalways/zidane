#ifndef UI_IMPL_H
#define UI_IMPL_H

#define UI_DISP_BOUND_LIMIT_X   159
#define UI_DISP_BOUND_LIMIT_Y   120

typedef struct 
{
    uint8 x_start;
    uint8 x_end;
    uint8 y_start;
    uint8 y_end;
} ui_bound_t;

typedef struct
{
    uint8 length;
    uint8 height;
    uint8 byte_of_line;
    uint8 *p_data;
} bmp_t;

typedef struct
{
    bmp_t   bmp;
    uint16  size_bmp_data;
} ui_bmp_t;

typedef struct
{
    uint8       origin_x;
    uint8       origin_y;
    ui_bmp_t    ui_bmp;
} ui_data_t;

typedef struct
{
    char            bmp_name[20];
    ui_data_t       ui_data;
    bool            clr_first;
    ui_bound_t      clr_bound;
} ui_bmp_profile_t;

void ui_init_impl ();
void ui_clear_ddram();
void ui_disp_hello_impl();
void ui_clear_screen();
void ui_show_demo_menu();
void ui_test_impl();
void ui_show_err_impl();
void ui_show_song_pos_impl (uint16);
void ui_show_song_num_total_impl (uint16);
void ui_show_song_num_impl (uint16 song_pos, uint16 num);
void ui_show_music_pause_impl (void);
void ui_show_nofile_impl (void);
void ui_show_song_time_impl (uint16);
void ui_show_music_basic_impl (void);
void ui_show_task_phase_impl (uint8 task_phase);
uint8 ui_show_file_name_impl
(
    uint8 *DataBuf,
    uint8 tc_ISNOrUnicode,
    uint8 nByte,
    uint8 DispOnOff
);

#endif
