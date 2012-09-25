/******************************************************************************
* Implementation for common ui function.
*
* Interface provided from this file are expected to be called by ui.c
*
* Created: 2012/08/27
******************************************************************************/
#include <assert.h>
#include "SPDA2K.H"
#include "../IR/LCM_BMP.h"
#include "../IR/lcm_bmp_driver.h"
#include "ui_impl.h"

#include "ui_bmp.c"

//-----------------------------------------------------------------------------
// void ui_clear_ddram (void)
//
// Description: Clear the whole DDRAM 384x160 in ST7587
//
// Input:
//
// Output:
//
// Created: 2012/08/14
//-----------------------------------------------------------------------------
void ui_clear_ddram (void)
{
    uint16 i;
    uint16 size_x;
    uint16 size_y;
    uint16 size_ddram;

    size_x = LCM_RESOLUTION_LIMIT_X;
    size_y = LCM_RESOLUTION_LIMIT_Y;
    size_ddram = size_x*size_y/8*4;

    lcm_write_command   (ST7587_COL_ADDR_SET_CMD);      // Set column address
    lcm_write_data      (0x00);
    lcm_write_data      (0x00);
    lcm_write_data      (0x00);
    lcm_write_data      (0x7F);                         // 384 columns
    lcm_write_command   (ST7587_ROW_ADDR_SET_CMD);      // Set row address
    lcm_write_data      (0x00);
    lcm_write_data      (0x00);
    lcm_write_data      (0x00);
    lcm_write_data      (0x9F);                         // 160 rows

    lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);            
    // write 0 to all DDRAM
    for(i=0; i<size_ddram; i++)
    {
        lcm_write_data(0x00);
    }    
}


//-----------------------------------------------------------------------------
// When you see the GWMS9708D from the front, the display area is:
//      X: SEG383 ~ SEG224
//      Y: COM0 ~ COM159
// That area is fixed phasically. We only can change the scan direction from DDRAM.
// The best way to use GWMS9708D is see the display area as:
//      X: SEG224 ~ SEG383
//      Y: COM159 ~ COM0
// With that, the scan direction is Y reversed.
//
// Below initialization is based on the order in scan_dir_t. I plan to use
// designated initializer to initialize this array, but the compiler does not
// support this C99 feature.
//-----------------------------------------------------------------------------
area_offset_t AREA_OFFSET[]=
{
    {225, 0},
    {0,   0},
    {225, 40},
    {225, 0}, 
};

//-----------------------------------------------------------------------------
// ui_get_offset
//
// Get the pixel offset info based on the scan direction setting.
//
// Created: 2012/09/01
//-----------------------------------------------------------------------------
void ui_get_offset (scan_dir_t direction, area_offset_t * p_offset)
{
    p_offset->offset_x = AREA_OFFSET[direction].offset_x;
    p_offset->offset_y = AREA_OFFSET[direction].offset_y;
    #if 0
    switch (direction)
    {
        case NORMAL:
            p_offset->offset_x = 225;
            p_offset->offset_y = 0;
            break;
        case REVERSE_X:
            p_offset->offset_x = 0;
            p_offset->offset_y = 0;
            break;
        case REVERSE_Y:
            p_offset->offset_x = 225;
            p_offset->offset_y = 40;
            break;
        case REVERSE_ALL:
            p_offset->offset_x = 0;
            p_offset->offset_y = 40;
            break;
        default:
            dbprintf("%s, %d line, scan direction setting is incorrect\n",
                        __FILE__,
                        __LINE__);
            break;
    }
    #endif            
}

//-----------------------------------------------------------------------------
// ui_set_disp_bound
//
// Before sending data into DDRAM, this function will configure the display area.
// This area concept here means the area in the LCD not the area in the DDRAM.
// Given the area in LCD and scan direction setting, this function will configure
// the area in the DDRAM.
//
// This function can only care about ST7587. In that case, the bound setting is:
//      X: 0-383
//      Y: 0-159
//
// If considering GWMS9708D, the bound setting will be:
//      X: 0-158.
//      Y: 0-119
//
// Created: 2012/09/01
//-----------------------------------------------------------------------------
void ui_set_disp_bound(uint16 x_start,
                        uint16 x_end,
                        uint16 y_start,
                        uint16 y_end
                        )
{
    area_offset_t offset;

    ui_get_offset (LCM_SCAN_DIRECTION_SETTING, &offset);

    x_start += offset.offset_x;
    x_end += offset.offset_x;
    y_start += offset.offset_y;
    y_end += offset.offset_y;

    if ( (x_start>x_end)
        ||(x_end>158)
        ||(y_start>y_end)
        ||(y_end>119)
        ||(x_start%3)
        ||((x_end-2)%3) )
    {
        dbprintf("%s, %d line: display bound setting incorrect!\n",
                __FILE__,
                __LINE__);
    }

    // REVISIT: should check if the X bound is 3's multiple.
    x_start = x_start/3;
    x_end   = (x_end)/3;

    lcm_write_command   (ST7587_COL_ADDR_SET_CMD);      // Set column address
    lcm_write_data      (x_start>>8);
    lcm_write_data      (x_start&0xFF);
    lcm_write_data      (x_end>>8);
    lcm_write_data      (x_end&0xFF);
    lcm_write_command   (ST7587_ROW_ADDR_SET_CMD);      // Set row address
    lcm_write_data      (y_start>>8);
    lcm_write_data      (y_start&0xFF);
    lcm_write_data      (y_end>>8);
    lcm_write_data      (y_end&0xFF);
}

//-----------------------------------------------------------------------------
// ui_fill_ui_data
//
// Populate the ui_data struct. Prepare the parameter to use ui_disp_ui_data( ).
//
// Parameter p_bmp is very special, which requires that the passed in para must
// be the address of bmp data generated by modulo. All of the logic is built
// based on the structure of bmp data:
//      1st byte: lenght.
//      2nd byte: height.
//      3rd byte: bytes in a line.
//      4st byte until end: data.
//
// Created: 2012/09/04
//-----------------------------------------------------------------------------
void ui_fill_ui_data ( 
                        ui_data_t   *p_ui_data,
                        uint8       origin_x,
                        uint8       origin_y,
                        uint8       *p_bmp,
                        uint16      size_bmp_data
                     )
{
    p_ui_data->origin_x     = origin_x;
    p_ui_data->origin_y     = origin_y;

    p_ui_data->ui_bmp.bmp.length        = *p_bmp;
    p_ui_data->ui_bmp.bmp.height        = *(p_bmp+1);
    p_ui_data->ui_bmp.bmp.byte_of_line  = *(p_bmp+2);
    p_ui_data->ui_bmp.bmp.p_data        = p_bmp+3;
    p_ui_data->ui_bmp.size_bmp_data     = size_bmp_data;
}
                      

//-----------------------------------------------------------------------------
// ui_disp_ui_data
//
// Created: 2012/09/04
//-----------------------------------------------------------------------------
void ui_disp_ui_data (ui_data_t * p_ui_data)
{
    uint8 i=0;
    uint8 j=0;
    uint8 bound_offset_x;
    uint8 bound_offset_y;
    uint8 bound_length;
    uint8 bound_height;

    uint8 bound_length_mod;

    uint8 *p_tmp;
    uint8 byte_of_line;
    uint16 size_bmp_data;
    uint8 line;
    uint8 line_mod;

    bound_offset_x      = p_ui_data->origin_x;
    bound_offset_y      = p_ui_data->origin_y;
    bound_length        = p_ui_data->ui_bmp.bmp.length;
    bound_height        = p_ui_data->ui_bmp.bmp.height;
    byte_of_line        = p_ui_data->ui_bmp.bmp.byte_of_line;    
    p_tmp               = p_ui_data->ui_bmp.bmp.p_data;
    size_bmp_data       = p_ui_data->ui_bmp.size_bmp_data;
    
    // Do necessary modification for length, height and byte_of_line.

    
    // If the length and byte_of_line match perfectly, show it right now!
    if ( !(bound_length%3)
        &&(bound_length==8*byte_of_line)
       )
    {
        // I do not check if the origin x is 3's multiple.
        ui_set_disp_bound  (bound_offset_x,
                            bound_offset_x+bound_length-1,
                            bound_offset_y,
                            bound_offset_y+bound_height-1);
    
        lcm_write_command (ST7587_WRITE_DISPLAY_DATA_CMD);
        for (i=0; i<size_bmp_data; i++)
        {
            lcm_write_data_cooked (*p_tmp);
            p_tmp++;
        }
        
        return;        
    }

    // Send the data line by line. This is slow.
    // If need, modify the bound length to 3's multiple.
    // Set the y bound range to 2 (y+1) in order to avoid the overlay in the
    // begining of a line by the redundant data at the end of a line.
    else
    {
        // Check if bound length needs modified.
        if (bound_length%3)
        {
            bound_length_mod = (bound_length/3)*3 + 3;
        }
        else
        {
            bound_length_mod = bound_length;
        }

        // Calculate how many lines there are in the bmp data.
        line_mod = size_bmp_data%byte_of_line;
        if (line_mod)
        {
            line = size_bmp_data/byte_of_line + 1;
        }
        else
        {
            line = size_bmp_data/byte_of_line;
        }

        for (i=0; i<line; i++)
        {
            
            ui_set_disp_bound  (bound_offset_x,
                                bound_offset_x+bound_length_mod-1,
                                bound_offset_y,
                                bound_offset_y+1);
            lcm_write_command (ST7587_WRITE_DISPLAY_DATA_CMD);
            while(j<byte_of_line)
            {
                lcm_write_data_cooked (*p_tmp);
                p_tmp++;
                j++;
            }

            // Prepare to send data of next line.
            bound_offset_y++;
            j=0;

            // If next line is the last line, check if it will be a full line.
            // If not, we will cut off the number of data we will send. This is
            // to avoid access the bmp data overflow.
            if ( (i==line-2) && (line_mod) )
            {
                j = byte_of_line-line_mod;
            }
        }
        
        return;                                
    }
                            
}

//-----------------------------------------------------------------------------
// ui_disp_bmp_data
//
// Given the origin postion, bmp_data and size of data, show it. The idea is:
// tell me where you want to show and what you want to show, then I will show
// you.
//
// IMPORTANT!
// The logic here higher depands on the struct of bmp data generated from the
// modulo software.
//
// Created: 2012/09/04
//-----------------------------------------------------------------------------
void ui_disp_bmp_data (
                        uint8   origin_x,
                        uint8   origin_y,
                        uint8*  bmp_data,
                        uint16  size_bmp_data
                      )
{
    ui_data_t ui_data;

    ui_fill_ui_data (
                        &ui_data,
                        origin_x,
                        origin_y,
                        bmp_data,
                        size_bmp_data
                    );
    ui_disp_ui_data (&ui_data);
}

//-----------------------------------------------------------------------------
// Display the bmp in the middle of the screen.
//
// Created: 2012/09/05
//-----------------------------------------------------------------------------
void ui_disp_bmp_data_mid_xy (uint8* bmp_data, uint16 size_bmp_data)
{
    uint8 origin_x;
    uint8 origin_y;
    uint8 length;
    uint8 height;

    length = *bmp_data;
    height = *(bmp_data+1);

    origin_x = (UI_DISP_BOUND_LIMIT_X-length)/2;
    origin_y = (UI_DISP_BOUND_LIMIT_Y-height)/2;

    // Have to adjust origin x to be 3's multiple
    if (origin_x%3)
    {
        origin_x = (origin_x/3 + 1)*3;
    }

    ui_disp_bmp_data (origin_x,
                      origin_y,
                      bmp_data,
                      size_bmp_data);
}

//-----------------------------------------------------------------------------
// ui_disp_hello_impl
//
// Created: 2012/09/04
//-----------------------------------------------------------------------------
void ui_disp_hello_impl(void)
{
    ui_disp_bmp_data (54,
                      54,
                      bmp_hello,
                      sizeof(bmp_hello)-3);
}

//-----------------------------------------------------------------------------
// Description: clear the DDRAM by writing 0 to all DDRAM space in valid range.
// Input:
//
// Output:
//
// Created: 2012/07/24
//-----------------------------------------------------------------------------
void ui_clear_screen(void)
{
    uint16  i;
    uint16  size_x; // column counter
    uint16  size_y; // row counter
    uint16  size_ddram;
    
    size_x = LCM_RESOLUTION_X;
    size_y = LCM_RESOLUTION_Y;
    
    if ((size_x>384)||(size_y>160))   
    {
        //dbprintf("Warning: LCM resolution is too bigger!\n");
    }

    size_ddram = size_x*size_y/8*4; // 4 bits for each dot.

    ui_set_disp_bound (0, LCM_RESOLUTION_X-1, 0, LCM_RESOLUTION_Y-1);

    lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);            
    // write 0 to all DDRAM
    for(i=0; i<size_ddram; i++)
    {
        lcm_write_data(0x00);
    }    
}

//-----------------------------------------------------------------------------
// Show a music demo menu like UES.
//
// Created: 2012/09/05
//-----------------------------------------------------------------------------
void ui_show_demo_menu(void)
{
    ui_disp_bmp_data (0,
                      10,
                      bmp_demo_top,
                      sizeof(bmp_demo_top)-3);
    ui_disp_bmp_data_mid_xy (bmp_demo_mid,
                             sizeof(bmp_demo_mid)-3);
    ui_disp_bmp_data (0,
                      98,
                      bmp_demo_bot,
                      sizeof(bmp_demo_bot)-3);   
}

//-----------------------------------------------------------------------------
// UI test routine
//
// Created: 2012/09/05
//-----------------------------------------------------------------------------
#if (UI_TEST_ONLY == FEATURE_ON)
void ui_test_impl(void)
{
    while(1)
    {
        ui_show_demo_menu();
        ui_clear_screen();
        ui_disp_hello_impl();
        ui_clear_screen();
    }
    
}
#endif