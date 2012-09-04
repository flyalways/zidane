#include <assert.h>
#include "SPDA2K.H"
#include "../IR/lcm_bmp_driver.h"

#include "ui_bmp.c"

//-----------------------------------------------------------------------------
// _ui_set_bound
//
// Description: Set the area in the LCD which we'd like to light using dot as
//              the unit. This is extremely conbined with the way to generate
//              dot matrix array data which is sent to LCM later.
//
// Created: 2012/08/27
//-----------------------------------------------------------------------------







//-----------------------------------------------------------------------------
// ui_disp_hello_impl
//
// Created: 2012/08/27
//-----------------------------------------------------------------------------
void ui_disp_hello_impl(void)
{
    uint16 i;

    // The display bound highly depands on the character's modulo.
    lcm_set_disp_bound(54, 101, 54, 65);

    lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);

    for (i=0;i<sizeof(bmp_001); i++)
    {
        lcm_write_data_cooked(bmp_001[i]);
    }
}

//-----------------------------------------------------------------------------
// ui_clear_screen
//
// Created: 2012/08/27
//-----------------------------------------------------------------------------
void ui_clear_screen(void)
{
    lcm_clear_screen();
}

//-----------------------------------------------------------------------------
// ui