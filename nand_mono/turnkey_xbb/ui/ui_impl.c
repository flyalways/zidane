/******************************************************************************
* Implementation for common ui function.
*
* Interface provided from this file are expected to be called by ui.c
*
* Created: 2012/08/27
******************************************************************************/
#include <assert.h>
#include <string.h>
#include "SPDA2K.H"
#include "../IR/LCM_BMP.h"
#include "../IR/lcm_bmp_driver.h"
#include "ui_impl.h"
#include "../Header/variables.h"

#include "ui_bmp.c"

static xdata bool ui_is_lock = FALSE;        // Flag of ui locked or not.

//-----------------------------------------------------------------------------
// ui_init_impl
//
// Description: it's actually LCM initialization. The reason we put it here and
//              call it ui_init is, common area is really comstrained so I can't
//              put it at lcm_bmp_driver.c but ui module.
//
// Created: 2012/07/13
//-----------------------------------------------------------------------------
void ui_init_impl (void)
{
    // Enable IO output. The LCM pin definition is in LCM_BMP.h
    // P1^0, 1, 2 is set output
    XBYTE[0xB102] |= 0x07;         

    // Set FMGPIO as GPIO to use LCM before resetting LCM.
    // This is to avoid mis-sending the first bit when set pins' function
    // after resetting. After this function runs:
    //      CSB is high;
    //      SCL is high (idle level)
    lcm_set_fmgpio(LCM_SWITCH_FMGPIO_TO_GPIO);              

    // Reset pulse width. ST7587 requires that minimum is 10us
    LCM_RSTB_SPI_LO;
    USER_DelayDTms(100);    // This delay func is defined in host_init.lib. Not accurate. 
    LCM_RSTB_SPI_HI;

    // Reset duration. ST7587 requires that minimum is 120ms
    //SPI_CLK_CLR;            // This is to test the window of delay.
    USER_DelayDTms(1000);
    USER_DelayDTms(1000);
    //SPI_CLK_SET;

    lcm_write_command   (ST7587_SLEEP_OUT_CMD);
    lcm_write_command   (ST7587_DISPLAY_OFF_CMD);
    USER_DelayDTms(100);

    lcm_write_command   (ST7587_AUTO_READ_CMD);         // Disable auto read
    lcm_write_data      (ST7587_AUTO_READ_DISABLE);
    lcm_write_command   (ST7587_OTP_SELECT_CMD);        // Disable OTP
    lcm_write_data      (ST7587_OTP_SELECT_DISABLE);
    
    #if 0 // read OTP, then control out
    lcm_write_command(0xE0);
    lcm_write_data(0x00);
    USER_DelayDTms(1200);
    lcm_write_data(0xE3);
    USER_DelayDTms(1200);
    lcm_write_command(0xE1);
    #endif

    lcm_write_command   (ST7587_VOP_SET_CMD);           // Set Vop as 15V
    lcm_write_data      (ST7587_VOP_SET_15V_LO);
    lcm_write_data      (ST7587_VOP_SET_15V_HI);

    lcm_write_command   (ST7587_BIAS_SET_CMD);          // Set Bias as 1/14
    lcm_write_data      (ST7587_BIAS_SET_1_14);

    lcm_write_command   (ST7587_BOOSTER_SET_CMD);       // Set booster level as x8
    lcm_write_data      (ST7587_BOOSTER_SET_X8);

    lcm_write_command   (ST7587_ANALOG_CTRL_CMD);       // Enable analog circuit
    lcm_write_data      (ST7587_ANALOG_CTRL_DATA);

    lcm_write_command   (ST7587_LINE_INVERSION_CMD);    // N-line = 0
    lcm_write_data      (ST7587_LINE_INVERSION_FRAME);  

    lcm_write_command   (ST7587_DDR_ENABLE_CMD);        // Enable DDR
    lcm_write_command   (ST7587_DDR_IFC_CMD);           // Enable DDR interface
    lcm_write_data      (ST7587_DDR_IFC_DATA);

    //---------------------------------------------------------------
    // The LCD we use is from SEG383->SEG224, so set the scan control
    // based on this.
    //---------------------------------------------------------------
    lcm_write_command   (ST7587_SCAN_DIRECTION_CMD);    // Scan direction setting
    if (LCM_SCAN_DIRECTION_SETTING == NORMAL)
    {
        lcm_write_data (ST7587_SCAN_DIRECTION_DEFAULT);
    }
    else if (LCM_SCAN_DIRECTION_SETTING == REVERSE_X)
    {
        lcm_write_data (ST7587_SCAN_DIRECTION_REVERSE_COL);
    }
    else if (LCM_SCAN_DIRECTION_SETTING == REVERSE_Y)
    {
        lcm_write_data (ST7587_SCAN_DIRECTION_REVERSE_ROW);
    }
    else if (LCM_SCAN_DIRECTION_SETTING == REVERSE_ALL)
    {
        lcm_write_data (ST7587_SCAN_DIRECTION_REVERSE_ALL);
    }

    lcm_write_command   (ST7587_DUTY_SET_CMD);          // Set duty as 1/120
    lcm_write_data      (ST7587_DUTY_SET_DATA_120);

    //---------------------------------------------------------------
    // We use 160x120 screen and don't need to use partial mode.
    // Partial mode setting does not impact the size of DDRAM we use.
    // It just controls the size of area LCD displays.
    // partial area must be larger than 0x60. Otherwise, display will be
    // abnormal.
    //---------------------------------------------------------------
    //lcm_write_command   (ST7587_PARTIAL_ON_CMD);        // Partial mode on          
    lcm_write_command   (ST7587_PARTIAL_SET_CMD);       // set partial display.
    lcm_write_data      (ST7587_PARTIAL_SET_DATA);
    lcm_write_command   (ST7587_PARTIAL_AREA_CMD);      // set partial display area
    lcm_write_data      (ST7587_PARTIAL_AREA_START_HI);
    lcm_write_data      (ST7587_PARTIAL_AREA_START_LO);
    lcm_write_data      (ST7587_PARTIAL_AREA_END_HI);
    lcm_write_data      (ST7587_PARTIAL_AREA_END_LO);   // must be larger than 0x60

    //---------------------------------------------------------------
    // Clear the whole DDRAM before LCD displays
    //---------------------------------------------------------------
    ui_clear_ddram();

    LCM_BL_SPI_LO;  // light the blacklight

    lcm_write_command   (ST7587_INVERSION_DISABLE_CMD); // Display inversion off
    lcm_write_command   (ST7587_DISPLAY_ON_CMD);        // Display on
    
#if (LCM_TEST_ONLY == FEATURE_ON)
    lcm_test_exclusive();
#endif

}

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
//            dbprintf("%s, %d line, scan direction setting is incorrect\n",
//                        __FILE__,
//                        __LINE__);
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
//        dbprintf("%s, %d line: display bound setting incorrect!\n",
//                __FILE__,
//                __LINE__);
    }

    // REVISIT: should check if the X bound is 3's multiple.
    x_start = x_start/3;
    x_end   = (x_end-2)/3;

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
// ui_clear_area
//
// Description: clear specified area in the screen.
//
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_clear_area (ui_bound_t *p_ui_bound)
{
    uint16 i;
    uint8 x_start, x_end, y_start, y_end;
    uint16 size_ddram;

    x_start = p_ui_bound->x_start;
    x_end   = p_ui_bound->x_end;
    y_start = p_ui_bound->y_start;
    y_end   = p_ui_bound->y_end;

    // Check if the bound range is reasonable.

    size_ddram = (x_end-x_start)*(y_end-y_start)/8*4;

    ui_set_disp_bound (x_start, x_end, y_start, y_end);
    lcm_write_command (ST7587_WRITE_DISPLAY_DATA_CMD);
    // write 0 to this area
    for(i=0; i<size_ddram; i++)
    {
        lcm_write_data(0x00);
    } 
}


// REVISIT!!!
// Once this feature is used in ui code, every impl function must check this.
//-----------------------------------------------------------------------------
// ui_lock
//
// Description: Here intention is to disable any refresh on the screen. Expected
//              impl is to disable the DDRAM interface of LCM. But there is no
//              such function. And power save mode is not what I want neither...
//              
//              Problem is: once this feature is used, every function about ui
//              impl must check if ui is locked.
// 
// Created: 2012/10/14
//-----------------------------------------------------------------------------
void ui_lock (void)
{
    if (!ui_is_lock)
    {
        // REVISIT!!!
        // How to lock ui?
        // The temporary impl is to set the display area to an invalid area for
        // this LCM or only 3 dots.
        ui_set_disp_bound (0,0,0,0);

        ui_is_lock = TRUE;
    }
}

//-----------------------------------------------------------------------------
// ui_unlock
//
// Description: opposite operation of ui_lock. See ui_lock for details.
// 
// Created: 2012/10/14
//-----------------------------------------------------------------------------
void ui_unlock (void)
{
    if (ui_is_lock)
    {
        // REVISI1!!!
        // How to unlock ui?

        ui_is_lock = FALSE;
    }   
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
void ui_disp_ui_data (ui_data_t * p_ui_data, bool clear)
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
        if (!clear) // show it
        {
            for (i=0; i<size_bmp_data; i++)
            {
                lcm_write_data_cooked (*p_tmp);
                p_tmp++;
            }
        }
        else // clear it
        {
            for (i=0; i<size_bmp_data; i++)
            {
                lcm_write_data_cooked (0x00);
            }
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
            if (!clear) // show it
            {
                while(j<byte_of_line)
                {
                    lcm_write_data_cooked (*p_tmp);
                    p_tmp++;
                    j++;
                }
            }
            else // clear it
            {
                while(j<byte_of_line)
                {
                    lcm_write_data_cooked (0x00);
                    j++;
                }   
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
    ui_disp_ui_data (&ui_data, 0);
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

// REVISIT!!!
// Using char *str as the para is not a good choice. Better way is to use an
// index of the element in this table. If so, we need to add a index field in
// ui_bmp_profile_t.
//-----------------------------------------------------------------------------
// ui_show_bmp_by_string
//
// Description: with string passed in this function, we will get what we should
//              show from the table using the string as the index.
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_bmp_by_string (char *str, bool clear_if)
{
    uint8 i=0;

    if (str == "NULL")
    {
        dbprintf("Failure: NULL string\n");
    }

    // Search this string in the table.
    while (  strcmp(str, ui_bmp_profile[i].bmp_name)
           &&strcmp("NULL", ui_bmp_profile[i].bmp_name)
          )
    {
        i++;
    }

    // Don't find this string in table
    if ( !(strcmp("NULL", ui_bmp_profile[i].bmp_name)) )
    {
        dbprintf("Failure: no string\n");
        return;    
    }

    // Clear some area before show it if clr_first is set
    if (ui_bmp_profile[i].clr_first == TRUE)
    {
        ui_clear_area (&(ui_bmp_profile[i].clr_bound));
    }

    if (clear_if)
    {
        ui_disp_ui_data ((ui_data_t *)&ui_bmp_profile[i].ui_data, 1);
    }
    else
    {
        ui_disp_ui_data ((ui_data_t *)&ui_bmp_profile[i].ui_data, 0);
    }
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
// ui_show_err_impl
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_err_impl(void)
{
    ui_disp_ui_data (&ui_data_err,0);    
}

//-----------------------------------------------------------------------------
// ui_show_music_basic_impl
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_music_basic_impl (void)
{
    ui_disp_ui_data (&ui_data_music_line_0_0, 0);
    ui_disp_ui_data (&ui_data_music_line_0_1, 0);
    ui_disp_ui_data (&ui_data_music_line_2_0_0, 0);
    ui_disp_ui_data (&ui_data_music_line_2_1, 0);
}

//-----------------------------------------------------------------------------
// ui_disp_digit_6x12
//
// Description: draw a 6x12 digit on the screen
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_disp_digit_6x12 (uint8 origin_x, uint8 origin_y, uint8 digit)
{  
    ui_data_t ui_data_digit;

    // Check the display bound setting


    // Initialize ui_data_t fields for every digit to show.
    ui_data_digit.origin_x                  = origin_x;
    ui_data_digit.origin_y                  = origin_y;
    ui_data_digit.ui_bmp.bmp.length         = 6;
    ui_data_digit.ui_bmp.bmp.height         = 12;
    ui_data_digit.ui_bmp.bmp.byte_of_line   = 1;
    ui_data_digit.ui_bmp.bmp.p_data         = &bmp_digit_6x12[12*digit];
    ui_data_digit.ui_bmp.size_bmp_data      = 12;

    ui_disp_ui_data (&ui_data_digit, 0);   
}

//-----------------------------------------------------------------------------
// ui_show_song_pos
//
// Description: show song position.
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_song_pos_impl (uint16 song_pos)
{
    ui_disp_digit_6x12 (12, 0, song_pos/10);
    ui_disp_digit_6x12 (18, 0, song_pos%10);
}

//-----------------------------------------------------------------------------
// ui_show_song_num_total_impl
//
// Description: show how many songs are there under current dir.
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_song_num_total_impl (uint16 num)
{
    ui_disp_digit_6x12 (54, 0, num/10);
    ui_disp_digit_6x12 (60, 0, num%10);
}

//-----------------------------------------------------------------------------
// ui_show_song_num_impl
//
// Description:
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_song_num_impl (uint16 song_pos, uint16 num)
{
    ui_show_song_pos_impl (song_pos);
    ui_show_song_num_total_impl (num);
}

//-----------------------------------------------------------------------------
// ui_show_music_pause_impl
//
// Description: 
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_music_pause_impl (void)
{
    ui_disp_ui_data (&ui_data_music_line_2_0_1, 0);
}

//-----------------------------------------------------------------------------
// ui_show_nofile_impl
//
// Description: show message about no file and clear other redundant display area.
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_nofile_impl (void)
{
    // Clear the screen first
    ui_clear_screen();

    // Show the message of no file.
    ui_disp_ui_data (&ui_data_nofile, 0);
}

//-----------------------------------------------------------------------------
// ui_show_song_time_impl
//
// Description: 
// 
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void ui_show_song_time_impl (uint16 time)
{
    // Show the latest time stamp of the current song.
    ui_disp_digit_6x12 (93,     108, time/600);
    ui_disp_digit_6x12 (99,     108, (time%600)/60);
    ui_disp_digit_6x12 (111,    108, (time%60)/10);
    ui_disp_digit_6x12 (117,    108, time%10);

    // Show the total time of the current song.

    ui_disp_digit_6x12 (129,    108, gw_TotalSec/600);
    ui_disp_digit_6x12 (135,    108, (gw_TotalSec%600)/60);
    ui_disp_digit_6x12 (147,    108, (gw_TotalSec%60)/10);
    ui_disp_digit_6x12 (153,    108, gw_TotalSec%10);
}

//-----------------------------------------------------------------------------
// ui_show_task_phase_impl
//
// Description:
// 
// Created: 2012/10/14
//-----------------------------------------------------------------------------
void ui_show_task_phase_impl (uint8 task_phase)
{
    switch (task_phase)
    {
        case TASK_PHASE_PLAYACT:
            ui_show_bmp_by_string ("play", FALSE);
            //ui_disp_ui_data (&ui_data_music_line_2_0_0, 0);
            break;
        
        case TASK_PHASE_PAUSE:
            ui_show_bmp_by_string ("pause", FALSE);
            break;
        
        default:
            dbprintf("Fail: no such task phase\n");
            break; 
    }
    ui_lock();
}

//-----------------------------------------------------------------------------
// ui_show_file_name_impl
//
// Description: show file name
//
// Created: 2012/10/14
//-----------------------------------------------------------------------------
uint8 ui_show_file_name_impl
(
    uint8 *DataBuf,
    uint8 tc_ISNOrUnicode,
    uint8 nByte,
    uint8 DispOnOff
)
{
    ui_show_bmp_by_string("filename_no", 0);
    return 0;

    #if 0 // REVISIT!!!
	U8 i,Column;
	U8 Tmp_DataBuf[32];
	U8 Sts;
	U8 tc_ColumnNum,tc_FirstWorldColumnNum;

	DispOnOff=1;
	tc_FirstWorldColumnNum = 0x00;
	Sts = 0;
	i=0;
	Column=0;

	while(i<nByte)
	{
		if(DispOnOff)
		{
			if(!tc_ISNOrUnicode)
			{
				tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(DataBuf+i+1),*(DataBuf+i),Tmp_DataBuf,1);
			}else{
				tc_ColumnNum = LCM_UNICODE_HZK_GET_BMP(*(DataBuf+i),*(DataBuf+i+1),Tmp_DataBuf,0);
			}
			if(i==0)
			{
				tc_FirstWorldColumnNum = tc_ColumnNum;
			}
		}			
		if((Column+(tc_ColumnNum&0x7f))>128)
		{
			Sts=1;
			goto DispOver;
		}
		if(DispOnOff)
		{	
			if((tc_ColumnNum&0x7f) > 8)
			{
				LCM_disp_HZKCharBMP(2,Column,Tmp_DataBuf,LCM_IsWord,0);		//just for solang by home
			}
			else
			{
				LCM_disp_HZKCharBMP(2,Column,Tmp_DataBuf,LCM_IsChar,0);
			}
		}
		Column+=(tc_ColumnNum&0x7f);

		if(tc_ColumnNum&0x80)
		{
			i+=2;
		}else
		{
			i+=1;
		}
	}
	Sts=0;
DispOver:
	while(Column<128 && DispOnOff)
	{
		LCM_set_address(2,Column);//just for solang by home.
		LCM_write_data(0x00);
		LCM_set_address(2+1,Column);
		LCM_write_data(0x00);
		Column++;
	}
	if(Sts)
	{
		if(tc_FirstWorldColumnNum&0x80)
		{
			gw_DispFileName_ByteOffset++;
		}
		gw_DispFileName_ByteOffset+=1;
	}else{
		gw_DispFileName_ByteOffset=0;
	}
	return Sts;//overstep display area

    #endif
}

//-----------------------------------------------------------------------------
// ui_show_vol_impl
//
// Description:
//
// Created: 2012/11/27
//-----------------------------------------------------------------------------
void ui_show_vol_impl (uint8 vol)
{
    ui_disp_digit_6x12 (141,    0, vol/10);
    ui_disp_digit_6x12 (147,    0, vol%10);    
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
// vtoh_16x16
//
// Description: transform the 16x16 dot matrix data read from unicode hzk file from
//              vertical, negative modulo to horizontal positive modulo.
//
// Created: 2012/10/04
//-----------------------------------------------------------------------------
void vtoh_16x16 (unsigned char *v, unsigned char *h)
{
    // Be careful when using loop variables as decreasing step. It must be
    // defined as signed not unsigned because when it decreases to 0, j--
    // will be a positive number if it is defined as unsigned. Then the loop
    // will never stop.
    char i,j;
    for (i=0; i<16; i=i+2)
    {
        for (j=7; j>=0; j--)
        {
            h[i] |= ( ((v[0+2*(7-j)] >> (unsigned char)(i/2)) & (unsigned char)0x01) << j );
        }
    }
    for (i=1; i<16; i=i+2)
    {
        for (j=7; j>=0; j--)
        {
            h[i] |= ( ((v[16+2*(7-j)] >> (unsigned char)(i/2)) & (unsigned char)0x01) << j );
        }
    }
    for (i=16; i<32; i=i+2)
    {
        for (j=7; j>=0; j--)
        {
            h[i] |= ( ((v[1+2*(7-j)] >> (unsigned char)((i-16)/2)) & (unsigned char)0x01) << j );
        }
    }
    for (i=17; i<32; i=i+2)
    {
        for (j=7; j>=0; j--)
        {
            h[i] |= ( ((v[17+2*(7-j)] >> (unsigned char)((i-16)/2)) & (unsigned char)0x01) << j );
        }
    }
}

//-----------------------------------------------------------------------------
// UI test routine
//
// Created: 2012/09/05
//-----------------------------------------------------------------------------
xdata uint8 bmp_tmp[32];
xdata uint8 bmp_tmp_h[32];
extern uint8 LCM_UNICODE_HZK_GET_BMP(U8 tc_HighByte, U8 tc_LowByte,U8 * tc_BmpBuf,U8 tbt_UnicodeOrISN);

#if (UI_TEST_ONLY == FEATURE_ON)
void ui_test_impl(void)
{
    uint8 i;
    ui_data_t hz_data;

    LCM_UNICODE_HZK_GET_BMP (0x4e,
                             0x00,
                             bmp_tmp,
                             1);

    memset (bmp_tmp_h, 0, 32);
    vtoh_16x16 (bmp_tmp, bmp_tmp_h);

    //while(1)
    {
//        ui_show_demo_menu();
//        ui_clear_screen();
//        ui_disp_hello_impl();
//        ui_clear_screen();

        /*
        hz_data.origin_x = 0;
        hz_data.origin_y = 20;
        hz_data.ui_bmp.bmp.length       = 16;
        hz_data.ui_bmp.bmp.height       = 16;
        hz_data.ui_bmp.bmp.byte_of_line = 2;
        hz_data.ui_bmp.bmp.p_data       = bmp_tmp;
        hz_data.ui_bmp.size_bmp_data    = 32;
        ui_disp_ui_data (&hz_data, 0);

        hz_data.origin_x = 20;
        hz_data.origin_y = 20;
        hz_data.ui_bmp.bmp.p_data       = bmp_tmp_h;
        ui_disp_ui_data (&hz_data, 0);
        */
        //ui_show_music_basic();


    }
    
}
#endif