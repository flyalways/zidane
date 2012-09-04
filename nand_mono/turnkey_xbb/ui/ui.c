/******************************************************************************
* UI interface for outside modules.
*
* If you want to show something with UI, you should call some function in this
* file.
*
* Created: 2012/08/27
******************************************************************************/
#include "SPDA2K.H"
#include "ui_impl.h"
#include "../IR/lcm_bmp_driver.h"

//-----------------------------------------------------------------------------
// ui_disp_hello
//
// Description: say hello to user at the startup process.
//
// Created: 2012/08/27
//-----------------------------------------------------------------------------
void ui_disp_hello (void)
{
    ui_disp_hello_impl();
}

//-----------------------------------------------------------------------------
// ui_test
//
// Description: Wrapper function to do some test about ui. It will be an infinite
//              loop.
//
// Created: 2012/08/27
//-----------------------------------------------------------------------------
#if (UI_TEST_ONLY == FEATURE_ON)
void ui_test(void)
{
    while(1)
    {
        ui_clear_screen();
        ui_disp_hello();
    }
}
#endif