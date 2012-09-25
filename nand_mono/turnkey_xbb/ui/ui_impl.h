#ifndef UI_IMPL_H
#define UI_IMPL_H

#define UI_DISP_BOUND_LIMIT_X   159
#define UI_DISP_BOUND_LIMIT_Y   120

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

void ui_clear_ddram();
void ui_disp_hello_impl();
void ui_clear_screen();
void ui_show_demo_menu();
void ui_test_impl();

#endif
