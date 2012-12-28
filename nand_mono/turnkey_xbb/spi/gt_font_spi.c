/**
 * I should not implement another spi bus in this system, but the sadness is,
 * EE uses another spi bus for font ic GT series instead of the spi bus for LCM...
 * That's really bad -_-!
 */
#include "SPDA2K.h"

//-----------------------------------------------------------------------------
// Signal layout:
//      CS#:    FMGPIO_0  (mcu output)
//      SCLK:   FMGPIO_15 (mcu output)
//      MISO:   FMGPIO_14 (mcu input)
//      MOSI:   FMGPIO_13 (mcu output)
//
// These pins are shared by nand flash. So pay attention to the switch between
// two functions.
//-----------------------------------------------------------------------------
#define GT_SPI_MASK_CSB     0x01
#define GT_SPI_MASK_SCLK    0x80
#define GT_SPI_MASK_SO      0x40
#define GT_SPI_MASK_SI      0x20

#define GT_SPI_CSB_ACTIVE   XBYTE[0xB401] &= ~0x01
#define GT_SPI_CSB_INACT    XBYTE[0xB401] |= 0x01
#define GT_SPI_SCLK_HIGH    XBYTE[0xB402] |= 0x80
#define GT_SPI_SCLK_LOW     XBYTE[0xB402] &= ~0x80
#define GT_SPI_SI_HIGH      XBYTE[0xB402] |= 0x20
#define GT_SPI_SI_LOW       XBYTE[0xB402] &= ~0x20
#define GT_SPI_SO           ((XBYTE[0xB40A] &= 0x40)? 1 : 0)

// Change the multiplexed interface to GPIO mode;
// Set output direction;
#define GT_SPI_START        XBYTE[0xB400] = 0x00;\
                            XBYTE[0xB405] |= GT_SPI_MASK_CSB;\
                            XBYTE[0xB406] |= (GT_SPI_MASK_SCLK|GT_SPI_MASK_SI)
// Change the interface to nand flash mode;
// dma auto-enable.
#define GT_SPI_STOP         XBYTE[0xB400] = 0x01;\
                            FLASH_REG[0x00]=0x01

static void _spi_delay(uint16 n)
{
    uint16 i;

    for (i=0; i<n; i++)
    {
        ;
    }
}

//-----------------------------------------------------------------------------
// Implement the 8 bits transfer during spi bus transaction.
//
// This is one part of spi communication. It is used to build a whole frame.
// So it does not care about the hw status. Hw should be initialized well.
//
// MSB first.
// Data is clocked out at rising edge. Data is clocked in at falling edge.
//-----------------------------------------------------------------------------
static void _spi_send_byte (uint8 byte)
{
    uint8 i;

    // Initial SCLK status. Just for safe.
    GT_SPI_SCLK_LOW;
    _spi_delay(1);

    for (i=0; i<8; i++)
    {
        if ((byte&0x80))
        {
            GT_SPI_SI_HIGH;
        }    
        else
        {
            GT_SPI_SI_LOW;
        }
        _spi_delay(1);
        GT_SPI_SCLK_HIGH;
        _spi_delay(1);
        GT_SPI_SCLK_LOW;

        byte<<=1;
    }

}

//----------------------------------------------------------------------------
// Because the falling edge of last clock of send will clock in the first
// bit, we take bit in first... Then we use other 7 clocks to take the rest.
//
// The clock we use to receive is different from sending data. We start from
// the low level.
//----------------------------------------------------------------------------
static uint8 _spi_receive_byte (void)
{
    uint8 i;
    uint8 byte=0;

    // Take the first bit.
    GT_SPI_SCLK_LOW;
    _spi_delay(1);
    byte |= GT_SPI_SO;

    // Take the rest 7 bits.
    for (i=0; i<7; i++)
    {
        byte<<=1;

        GT_SPI_SCLK_HIGH;
        _spi_delay(1);
        GT_SPI_SCLK_LOW;        
        _spi_delay(1);
        byte |= GT_SPI_SO; 
    }

    // This is the high level of the last clock peroid.
    GT_SPI_SCLK_HIGH;
    _spi_delay(1);

    return byte;
}

///-----------------------------------------------------------------------------
/// Normal read operation from GT20L16P1Y.
///
/// There are only two operation in total: read and fast read. I guess our mcu
/// is not that fast, so I only implement the normal read operation here.
///
/// During this operation, at the point where write switches to read, the last
/// clock will send the last bit of data at the rising edge and receive the first
/// bit at the falling edge. This makes it a little tricky to do the driver...
/// That means, the initial SCLK status should be completely correct avoiding
/// to output one more clock.
///
/// @date 2012/12/27 
///-----------------------------------------------------------------------------
void gt_font_spi_read (uint8 *font_data, uint32 addr, uint8 size_in)
{
    uint8 i;

    // Initial hw.
    GT_SPI_START; 
   
    GT_SPI_CSB_ACTIVE;   

    // Send the command.
    _spi_send_byte (0x03);

    // Send the address.
    _spi_send_byte ((addr>>16)&0xFF);
    _spi_send_byte ((addr>>8)&0xFF);
    _spi_send_byte (addr&0xFF);

    // Receive the font data.
    for (i=0; i<size_in; i++)
    {
        *(font_data+i) = _spi_receive_byte();
    }
    
    // Transaction ends.
    GT_SPI_CSB_INACT;
    
    // Change back to nand mode.
    GT_SPI_STOP;  
}