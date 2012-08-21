#include "..\Header\SPDA2K.h"
#include "..\header\PROTOTYP.h"
#include "..\IR\LCM_BMP.h"
#include "lcm_bmp_driver.h"

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
        //printf("Warning: LCM resolution is too bigger!\n");
    }

    size_ddram = size_x*size_y/8*4; // 4 bits for each dot.

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
#if 1
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
    lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);            
    for(i=0; i<size_ddram; i++)
    {
        lcm_write_data(0xff);
    }
 
}

//-----------------------------------------------------------------------------
// void lcm_disp_area(uint16 row, uint16 col, uint8 level)
// 
// Description: Light the specified area in the screen in the level passed in.
//              The unit of row and col is dot. The default start address of
//              row and column is 0.
// Input:
//
// Output:
//
// Created: 2012/08/14
//-----------------------------------------------------------------------------
void lcm_disp_area(uint16 row, uint16 col, uint8 level)
{
    uint8 addr_row_lo;
    uint8 addr_row_hi;
    uint8 addr_col_lo;
    uint8 addr_col_hi;
    uint16 size_ddram;
    uint16 i;

    if ((col>384)||(row>160))   
    {
        //dbprintf("Warning: LCM resolution is too bigger!\n");
    }
    if (col%3)
    {
        //dbprintf("Warning: column para is not 3's mutiple!\n");
    }

    size_ddram = row*col/8*4;

    addr_row_hi = 0xff & ((row-1)>>8);
    addr_row_lo = 0xff & (row-1);
    addr_col_hi = 0xff & ((col/3 - 1)>>8);
    addr_col_lo = 0xff & (col/3 - 1);   

    lcm_write_command   (ST7587_COL_ADDR_SET_CMD);      // Set column address
    lcm_write_data      (ST7587_COL_ADDR_SET_START_HI);
    lcm_write_data      (ST7587_COL_ADDR_SET_START_LO);
    lcm_write_data      (addr_col_hi);
    lcm_write_data      (addr_col_lo);

    lcm_write_command   (ST7587_ROW_ADDR_SET_CMD);      // Set row address
    lcm_write_data      (ST7587_ROW_ADDR_SET_START_HI);
    lcm_write_data      (ST7587_ROW_ADDR_SET_START_LO);
    lcm_write_data      (addr_row_hi);
    lcm_write_data      (addr_row_lo);

    lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);            
    for(i=0; i<size_ddram; i++)
    {
        lcm_write_data(level);
    }
}

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
void lcm_test_exclusive(void)
{
    while(1)
    {  
        //lcm_write_command   (ST7587_ALL_PIXEL_ON);
        lcm_disp_area (120, 160, 0xff);        
        USER_DelayDTms(1000);

        //lcm_write_command   (ST7587_ALL_PIXEL_OFF);
        lcm_disp_area (120, 160, 0x00);
        USER_DelayDTms(1000);
    }
}

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
#if (LCM_TEST_INIT == FEATURE_ON)
    lcm_init_spi_simple();
#endif
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
    // based on this:
    // COM0->COM159, SEG383->SEG0.
    //---------------------------------------------------------------
    lcm_write_command   (ST7587_SCAN_DIRECTION_CMD);    // Scan direction setting
    lcm_write_data      (ST7587_SCAN_DIRECTION_REVERSE_COL);

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

    // Display the logo of LCM initializing.
    //lcm_dis_logo();

    //lcm_set_fmgpio (LCM_SWITCH_FMGPIO_TO_NAND);
}

//-----------------------------------------------------------------------------
// void lcm_init_spi_simple(void)

// Description: initialize LCM hardware interface and LCM function in the
//              simplest way. This is mainly to test the LCM
//              I don't do any other setting after resetting and just send data
//              to DDRAM to display.
//
// Input:
//
// Output:
//
// Created: 2012/08/07
//-----------------------------------------------------------------------------
void lcm_init_spi_simple (void)
{
    // Enable IO output. The LCM pin definition is in LCM_BMP.h
    // P1^0, 1, 2 is set output
    XBYTE[0xB102] |= 0x07;      

    LCM_BL_SPI_LO;  // light the blacklight

    // Set FMGPIO as GPIO to use LCM before resetting LCM.
    // This is to avoid mis-sending the first bit when set pins' function
    // after resetting. After this function runs:
    //      CSB is high;
    //      SCL is high (idle level)
    lcm_set_fmgpio(LCM_SWITCH_FMGPIO_TO_GPIO);              

    // Reset pulse width
    LCM_RSTB_SPI_LO;
    USER_DelayDTms(1200);     // This delay func is defined in host_init.lib. Not accurate. 
    LCM_RSTB_SPI_HI;

    // Reset duration
    //SPI_CLK_CLR;            // This is to test the window of delay.
    USER_DelayDTms(1200);   // the reset duration's minimum is 120ms.
    USER_DelayDTms(1200);
    USER_DelayDTms(1200);
    USER_DelayDTms(1200);
    USER_DelayDTms(1200);
    //SPI_CLK_SET;
      
    lcm_write_command   (ST7587_SLEEP_OUT_CMD);         // This is a must.

    lcm_write_command   (ST7587_PARTIAL_ON_CMD);        // Partial mode on  
    lcm_write_command   (ST7587_PARTIAL_SET_CMD);       // set partial display mode.
    lcm_write_data      (ST7587_PARTIAL_SET_DATA);
    lcm_write_command   (ST7587_PARTIAL_AREA_CMD);      // set partial display area
    lcm_write_data      (ST7587_PARTIAL_AREA_START_HI);
    lcm_write_data      (ST7587_PARTIAL_AREA_START_LO);
    lcm_write_data      (ST7587_PARTIAL_AREA_END_HI);
    lcm_write_data      (ST7587_PARTIAL_AREA_END_LO);
    lcm_write_command   (ST7587_ANALOG_CTRL_CMD);       // Enable analog circuit
    lcm_write_data      (ST7587_ANALOG_CTRL_DATA);
    lcm_write_command   (ST7587_DDR_ENABLE_CMD);        // Enable DDR
    lcm_write_command   (ST7587_DDR_IFC_CMD);           // Enable DDR interface
    lcm_write_data      (ST7587_DDR_IFC_DATA);

    lcm_write_command   (ST7587_DISPLAY_ON_CMD);        // Display on

    // clear the DDRAM
    //lcm_clear_screen();
    
#if (LCM_TEST_ONLY == FEATURE_ON)
    lcm_test_exclusive();
#endif
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