/******************************************************************************
* UI interface for outside modules.
*
* If you want to show something with UI, you should call some function in this
* file.
*
* Created: 2012/08/27
******************************************************************************/
#include "SPDA2K.H"
#include "ui.h"
#include "ui_impl.h"
#include "../IR/LCM_BMP.h"
#include "../IR/lcm_bmp_driver.h"

//-----------------------------------------------------------------------------
// ui_init
//
// Description: it's actually LCM initialization. The reason we put it here and
//              call it ui_init is, common area is really comstrained so I can't
//              put it at lcm_bmp_driver.c but ui module.
//
// Created: 2012/07/13
//-----------------------------------------------------------------------------
void ui_init (void)
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
    ui_test_impl();
}
#endif