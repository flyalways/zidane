/******************************************************************************
* LCM driver for ST7587 + GWMS9708D.
* 
* The file name does not say that. It is a legacy
******************************************************************************/

#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\IR\LCM_BMP.h"
#include "lcm_bmp_driver.h"
#include "../ui/ui.h"
//#include <stdlib.h>


/******************************************************************************
* All precautions about ST7587 to driver 160x120
*
* 1. When you set the display bound before sending data to DDRAM, row setting
* is normal but column setting is really tricky:
*   a). The number N you set in the command uses pixel as the unit. And N stands
*   for N, N+1 and N+2. That means the column bound will be 3's multiple. Limit
*   of column which ST7587 can driver is 384 with 0x7F in the column address
*   setting command.
*   b). So even we use a 160x120 LCD, we can only use 159x120 actually.
*   c). The way to refresh the DDRAM is also 3's multiple. 4 bits control one
*   pixel. If you send 5 bytes of data to DDRAM, how many dots do you actually
*   control? You might say 10 bits because it is 5 bytes data. But the correct
*   answer is 9 bits. The reason is, ST7587 refreshed DDRAM 3 dots by 3 dots.
*   Once there are enought bytes of data written into DDRAM which is 3's multiple,
*   those pixels are displayed. One word, the number of pixels which are displayed
*   based on the data in DDRAM, is 3's multiple.
*   d). With c) met, 4 bits in DDRAM stands for 1 pixel. It is to say, the data
*   in DDRAM is managed by a scale of 4 bits not byte. If you send a last byte,
*   the first 4 bits controls the last pixel at row x, and the last 4 bits of
*   the last byte will control the first pixel at next row x+1.
*   e). If you want to send the right perfect number of data into DDRAM to
*   control the 3's multiple column dots, the perfect number of data will be
*   3's multiple. That also means the column of area you set will be 6's multiple.
*   f). For mono 1 gray scale LCD, 1 bit controls 1 pixel. If we use this kind
*   of original dot matrix array to construct our data into DDRAM, the column
*   of area we set must be multiple of 6 and 8.
*   g). So at last, the column we can set will be, 24, 48, 72, 96, etc.
*   h). The real strange thing is, if you set the column scan direction to be:
*   from SEG383->SEG0, you might think that the first group of 3 pixel will use
*   data in DDRAM like this:
*       0-2 bits: SEG383
*       3-5 bits: SEG382
*       6-8 BITS: SEG381
*   But actually not. The fact is:
*       0-2 bits: SEG381
*       3-5 bits: SEG382
*       6-8 BITS: SEG383
*   So, even the scan direction is SEG383->SEG0, but in a small group of 3 pixel,
*   the order is from low SEG to high SEG. This feature makes it difficult to use
*   LCD in scan direction of SEG383->SEG0.
*   
* 2. After you set a display area with column and row setting commands, you can
* send data into DDRAM. And then if you only change the row bound later, the column
* bound will also be reset to the begining based on the previous setting.
******************************************************************************/   

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
area_offset_t AREA_OFFSET[]=
{
    {225, 0},
    {0,   0},
    {225, 40},
    {225, 0}, 
};

//-----------------------------------------------------------------------------
// lcm_get_offset
//
// Get the pixel offset info based on the scan direction setting.
//
// Created: 2012/09/01
//-----------------------------------------------------------------------------
void lcm_get_offset (scan_dir_t direction, area_offset_t * p_offset)
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
// lcm_set_disp_bound
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
void lcm_set_disp_bound(uint16 x_start,
                        uint16 x_end,
                        uint16 y_start,
                        uint16 y_end
                        )
{
    area_offset_t offset;

    lcm_get_offset (LCM_SCAN_DIRECTION_SETTING, &offset);

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
// void lcm_set_fmgpio(lcm_switch_fmgpio_action_t action)
//
// Description: because we multiplex FMGPIO between nand and GPIO. When we talk
//              to LCM, we need to switch to GPIO mode, then initialize the
//              signals. After the data transfer is done, we need to to switch
//              it back to nand.
// Input:
//
// Output:
//
// Created: 2012/08/05
//-----------------------------------------------------------------------------
void lcm_set_fmgpio(lcm_switch_fmgpio_action_t action)
{
    switch (action)
    {
        case LCM_SWITCH_FMGPIO_TO_GPIO:
            XBYTE[0xB400] = 0;      // GPIO mode.
            XBYTE[0xB405] |= 0x10;  // Enable FMGPIO4 output as A0.
            XBYTE[0xB406] |= 0x08;  // Enable FMGPIO11 output as CSB.
            XBYTE[0xB407] |= 0x02;  // Enable FMGPIO17 output as SI.
            
            LCM_CSB_SPI_HI;         // do not select chip before transferring data
            SPI_CLK_SET;            // clock's idle level is high.
            break;
        case LCM_SWITCH_FMGPIO_TO_NAND:
            XBYTE[0xB400] = 1;      // Nand mode.
            FLASH_REG[0x00]=0x01;   // nandauto enable
            break;
        default:
            dbprintf("LCM: FMGPIO switch fails!\n");
            break;
    }                 
}

//-----------------------------------------------------------------------------
// Description: write a command to LCM through serial interface simulated
//              by GPIO port.
// Input:
//
// Output:
//
// Created: 2012/07/13
//-----------------------------------------------------------------------------
void lcm_write_command (uint8 command)
{
    uint8 i;

    // Set FMGPIO function as GPIO. Ususally, FMGPIO is used as nand.
    // When we talk to lcd, we set it as GPIO.
    // I don't know how much this switch process will impact performance.
    // Let's see.
    XBYTE[0xB400] = 0;      // GPIO mode.
    XBYTE[0xB405] |= 0x10;  // Enable FMGPIO4 output as A0.
    XBYTE[0xB406] |= 0x08;  // Enable FMGPIO11 output as CSB.
    XBYTE[0xB407] |= 0x02;  // Enable FMGPIO17 output as SI.

    LCM_CSB_SPI_LO;         // LCM chip select.
    LCM_A0_SPI_CMD;         // LCM command mode.

    // Send 8 bit data through IO mode.
    // MSB first.
    for (i=0; i<8; i++)
    {
        SPI_CLK_CLR;
        if ((command&0x80)) // MSB first
        {
            SPI_SI_SET;
        }
        else
        {
            SPI_SI_CLR;
        }
        SPI_CLK_SET;

        command = command<<1;
    }

    LCM_CSB_SPI_HI;
    XBYTE[0xB400] = 1;      // Nand mode.
    FLASH_REG[0x00]=0x01;   // nandauto enable
}

//-----------------------------------------------------------------------------
// Description: write one byte data to LCM through serial interface simulated
//              by GPIO port.
// Input:
//
// Output:
//
// Created: 2012/07/13
//-----------------------------------------------------------------------------
void lcm_write_data (uint8 content)
{
    uint8 i;

    // Set FMGPIO function as GPIO. Ususally, FMGPIO is used as nand.
    // When we talk to lcd, we set it as GPIO.
    // I don't know how much this switch process will impact performance. Let's see.
    // 
    // 2010/07/24, William:
    // Below is preparation to use serial interface for LCM by switching nand
    // interface to GPIO interface.
    // There is a concern about this preparation. We only need to do
    // this one time. But every call of this function to send one byte of data
    // will do below again. Usually, this function will be called hundreds of
    // times when we reflash the data in DDRAM.
    // A better way could be seperate this preparation to be another function,
    // which should be called every time before we start to write data to LCM.
    // But there are many places to be modified...
    XBYTE[0xB400] = 0;      // GPIO mode.
    XBYTE[0xB405] |= 0x10;  // Enable FMGPIO4 output as A0.
    XBYTE[0xB406] |= 0x08;  // Enable FMGPIO11 output as CSB.
    XBYTE[0xB407] |= 0x02;  // Enable FMGPIO17 output as SI.

    LCM_CSB_SPI_LO;         // LCM chip select.
    LCM_A0_SPI_DATA;        // LCM data mode.

    // Send 8 bit data through IO mode.
    // MSB first.
    for (i=0; i<8; i++)
    {
        SPI_CLK_CLR;
        if ((content&0x80)) // MSB first
        {
            SPI_SI_SET;
        }
        else
        {
            SPI_SI_CLR;
        }
        SPI_CLK_SET;

        content = content<<1;
    }

    LCM_CSB_SPI_HI;
    XBYTE[0xB400] = 1;      // Nand mode.
    FLASH_REG[0x00]=0x01;   // nandauto enable
}

//-----------------------------------------------------------------------------
// void lcm_write_data_cooked (uint8 content)
//
// Description: For most of dot's data here, one dot is controlled by one bit.
//              But we use multi gray level LCD, which means we could use several
//              bits to light one dot.
//              For 16-level ST7587, 4 bits control one dot. So we have to construct
//              4 bits mode data based on the 1 bit mode passed in.
//              
// Input:
//
// Output:
//
// Created: 2012/08/26
//-----------------------------------------------------------------------------
void lcm_write_data_cooked (uint8 content)
{
    uint8 i;
    uint8 j;
    uint8 data_tmp=0;

    // Set FMGPIO function as GPIO. Ususally, FMGPIO is used as nand.
    // When we talk to lcd, we set it as GPIO.
    // I don't know how much this switch process will impact performance. Let's see.
    // 
    // 2010/07/24, William:
    // Below is preparation to use serial interface for LCM by switching nand
    // interface to GPIO interface.
    // There is a concern about this preparation. We only need to do
    // this one time. But every call of this function to send one byte of data
    // will do below again. Usually, this function will be called hundreds of
    // times when we reflash the data in DDRAM.
    // A better way could be seperate this preparation to be another function,
    // which should be called every time before we start to write data to LCM.
    // But there are many places to be modified...
    XBYTE[0xB400] = 0;      // GPIO mode.
    XBYTE[0xB405] |= 0x10;  // Enable FMGPIO4 output as A0.
    XBYTE[0xB406] |= 0x08;  // Enable FMGPIO11 output as CSB.
    XBYTE[0xB407] |= 0x02;  // Enable FMGPIO17 output as SI.

    // The implementation is determined by the way to generate 1-bit dot matrix
    // array. Here we use: horizontal and left is MSB. 
    for (j=0; j<4; j++)
    {
        // Construct the high 4 bits in 1 byte.
        if (content&0x80)
        {
            data_tmp |= 0xF0;
        }
        else
        {
            data_tmp &= 0x0F;
        }

        // Construct the low 4 bits in 1 byte.
        if (content&0x40)
        {
            data_tmp |= 0x0F;
        }
        else
        {
            data_tmp &= 0xF0;
        }
              
        LCM_CSB_SPI_LO;         // LCM chip select.
        LCM_A0_SPI_DATA;        // LCM data mode.

        // Send 8 bit data through IO mode.
        // MSB first.
        for (i=0; i<8; i++)
        {
            SPI_CLK_CLR;
            if ((data_tmp&0x80)) // MSB first
            {
                SPI_SI_SET;
            }
            else
            {
                SPI_SI_CLR;
            }
            SPI_CLK_SET;
    
            data_tmp = data_tmp<<1;
        }
        LCM_CSB_SPI_HI;

        content = content<<2;   // Construct next two bits to be 1 byte.
        data_tmp = 0;
    }
 
    XBYTE[0xB400] = 1;      // Nand mode.
    FLASH_REG[0x00]=0x01;   // nandauto enable
}

//-----------------------------------------------------------------------------
// void lcm_clear_ddram (void)
//
// Description: Clear the whole DDRAM 384x160 in ST7587
//
// Input:
//
// Output:
//
// Created: 2012/08/14
//-----------------------------------------------------------------------------
void lcm_clear_ddram (void)
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
// Description: clear the DDRAM by writing 0 to all DDRAM space in valid range.
// Input:
//
// Output:
//
// Created: 2012/07/24
//-----------------------------------------------------------------------------
void lcm_clear_screen(void)
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

    #if 0
    // Set the display area to be the whole screen 160x120.
    // Because each column address has 3 dots and 4 bits for each dot,
    // So we only can display 159x120, which means the limit of the
    // column address is 0x34
    lcm_write_command   (ST7587_COL_ADDR_SET_CMD);      // Set column address
    lcm_write_data      (ST7587_COL_ADDR_SET_START_HI);
    lcm_write_data      (ST7587_COL_ADDR_SET_START_LO);
    lcm_write_data      (ST7587_COL_ADDR_SET_END_HI);
    lcm_write_data      (ST7587_COL_ADDR_SET_END_LO);
    lcm_write_command   (ST7587_ROW_ADDR_SET_CMD);      // Set row address
    lcm_write_data      (ST7587_ROW_ADDR_SET_START_HI);
    lcm_write_data      (ST7587_ROW_ADDR_SET_START_LO);
    lcm_write_data      (ST7587_ROW_ADDR_SET_END_HI);
    lcm_write_data      (ST7587_ROW_ADDR_SET_END_LO);
    #endif

    lcm_set_disp_bound (0, LCM_RESOLUTION_X-1, 0, LCM_RESOLUTION_Y-1);

    lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);            
    // write 0 to all DDRAM
    for(i=0; i<size_ddram; i++)
    {
        lcm_write_data(0x00);
    }    
}

//-----------------------------------------------------------------------------
// Description: set the DDRAM by writing 0xff to all DDRAM space in valid range.
// Input:
//
// Output:
//
// Created: 2012/07/30
//-----------------------------------------------------------------------------
#if 0 // Take this out from build to save some code space since we don't use it.
void lcm_light_screen(void)
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

    // Set the display area to be the whole screen 160x120
    lcm_set_disp_bound( 0,
                        LCM_RESOLUTION_X-1,
                        0,
                        LCM_RESOLUTION_Y-1
                        );

    lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);            
    for(i=0; i<size_ddram; i++)
    {
        lcm_write_data(0xff);
    }
 
}
#endif

//-----------------------------------------------------------------------------
// void lcm_test_exclusive(void)
//
// Description: repeat to display wholee screen and then clear whole screen.
//              When this function is running, no other function can run.
//              This is mainly to test LCM if it can work.
// Input:
//
// Output:
//
// Created: 2012/07/30
//-----------------------------------------------------------------------------
#if (LCM_TEST_ONLY == FEATURE_ON)
void lcm_test_exclusive(void)
{
    uint8 i;

    while(1)
    {      

        lcm_set_disp_bound(0,11, 0,0);
        lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);
        for (i=0; i<3; i++)
        {
            lcm_write_data(0x48);
            lcm_write_data(0xcf);
        }
    
        lcm_set_disp_bound(0,11, 119,119);
        lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);
        for (i=0; i<3; i++)
        {
            lcm_write_data(0x48);
            lcm_write_data(0xcf);
        }
    
        //ui_disp_hello();
        
        //lcm_clear_screen();
         
        //USER_DelayDTms(1000);
    }
}
#endif

//-----------------------------------------------------------------------------
// Description: initialize LCM hardware interface and LCM function.
//
// Input:
//
// Output:
//
// Created: 2012/07/13
//-----------------------------------------------------------------------------
void lcm_init_spi (void)
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
    lcm_clear_ddram();

    LCM_BL_SPI_LO;  // light the blacklight

    lcm_write_command   (ST7587_INVERSION_DISABLE_CMD); // Display inversion off
    lcm_write_command   (ST7587_DISPLAY_ON_CMD);        // Display on
    
#if (LCM_TEST_ONLY == FEATURE_ON)
    lcm_test_exclusive();
#endif

    //lcm_set_fmgpio (LCM_SWITCH_FMGPIO_TO_NAND);
}

//----------------------------------------------------------------------------
// Description: Set the display area boundary before sending the DDRAM data.
//              In this function, we set a display area: 1 row and the column
//              is from col to maxium column.
// 
// Input:
//
// Output:
//
// Created: 2012/07/25
//----------------------------------------------------------------------------
void lcm_set_addr(uint8 row, uint8 col)
{
    // This function is made to replace original LCM_set_address(). The page
    // in LCM_set_address() is thought as row number. Not sure if that makes
    // sense. Anyway, we will use lcm_set_dis_area() in the future to set the
    // display boundary before sending data to DDRAM.
    uint8 row_hi=row;
    uint8 row_lo=row;
    uint8 col_hi=0;
    uint8 col_lo=col;

    // I don't want to switch FMGPIO between nand and GPIO every data/command
    // transfer to LCM. So I put the switch process to two function seperately.
    // This intention requires to call this switch process at every place
    // where LCM_write_data() is called, which needs too many efforts. Since
    // at every place where LCM_write_data() is called, LCM_set_address() is
    // also called, I'd like to put this switch process here. But what about
    //switch from GPIO to nand after we finish talking to LCM?
    //lcm_set_fmgpio(0);
        
    // Set the display area to be the whole screen 160x120
    lcm_write_command   (ST7587_COL_ADDR_SET_CMD);      // Set column address
    lcm_write_data      (ST7587_COL_ADDR_SET_START_HI);
    lcm_write_data      (col_lo);
    lcm_write_data      (ST7587_COL_ADDR_SET_END_HI);
    lcm_write_data      (ST7587_COL_ADDR_SET_END_LO);
    lcm_write_command   (ST7587_ROW_ADDR_SET_CMD);      // Set row address
    lcm_write_data      (ST7587_ROW_ADDR_SET_START_HI);
    lcm_write_data      (row);
    lcm_write_data      (ST7587_ROW_ADDR_SET_END_HI);
    lcm_write_data      (row);

    lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD); // start to write DDRAM
}

#if (SERIAL_MONO == FEATURE_OFF)  // Re-#define to mine
//============================================================================
void LCM_set_address(U8 page, U8 col)
{
	LCM_write_command(page|0xb0);
	LCM_write_command((col>>4)|0x10);
	LCM_write_command(col&0x0f);
}
//============================================================================
#endif