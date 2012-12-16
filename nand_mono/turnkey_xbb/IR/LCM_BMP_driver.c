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
#if (SERIAL_MONO == FEATURE_ON)

    
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
            #ifdef EVB_128
            XBYTE[0xB406] |= 0x08;  // Enable FMGPIO11 output as CSB.
            #else
            XBYTE[0xB406] |= 0x02;  // Enable FMGPIO_9 output as CSB.
            #endif
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
    #ifdef EVB_128
    XBYTE[0xB406] |= 0x08;  // Enable FMGPIO11 output as CSB.
    #else
    XBYTE[0xB406] |= 0x02;  // Enable FMGPIO_9 output as CSB.
    #endif
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
    #ifdef EVB_128
    XBYTE[0xB406] |= 0x08;  // Enable FMGPIO11 output as CSB.
    #else
    XBYTE[0xB406] |= 0x02;  // Enable FMGPIO_9 output as CSB.
    #endif
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
    #ifdef EVB_128
    XBYTE[0xB406] |= 0x08;  // Enable FMGPIO11 output as CSB.
    #else
    XBYTE[0xB406] |= 0x02;  // Enable FMGPIO_9 output as CSB.
    #endif
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
// lcm_set_address
//
// Description: temporary replacement of original function in the original driver.
//-----------------------------------------------------------------------------
// REVIST!!!
void lcm_set_address (uint8 page, uint8 col)
{
    // Make compiler happy.
    page++;
    col++;    
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
#if (LCM_TEST_ONLY == FEATURE_ON)
void ui_set_disp_bound(uint16 x_start,
                        uint16 x_end,
                        uint16 y_start,
                        uint16 y_end
                        );
void lcm_test_exclusive(void)
{
    uint8 i;

    ui_set_disp_bound(0,11, 0,0);
    lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);

    while(1)
    {      



        for (i=0; i<3; i++)
        {
            lcm_write_data(0xAA);

        }
    
//        ui_set_disp_bound(0,11, 119,119);
//        lcm_write_command   (ST7587_WRITE_DISPLAY_DATA_CMD);
//        for (i=0; i<3; i++)
//        {
//            lcm_write_data(0x48);
//            lcm_write_data(0xcf);
//        }
    
        //ui_disp_hello();
        
        //lcm_clear_screen();
         
        //USER_DelayDTms(1000);
    }
}
#endif

#endif // SERIAL_MONO == FEATURE_ON

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