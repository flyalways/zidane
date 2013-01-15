/**
 * @file
 * Driver for the FM ic kt0810sg
 *
 */
#include "SPDA2K.H"
#include "i2c.h"
#include "kt0810.h"

///----------------------------------------------------------------------------
/// Register macro definition.
///----------------------------------------------------------------------------
#define KT0810_REG_DEVICE_ID        0x00
#define KT0810_REG_CHIP_ID          0x01
#define KT0810_REG_SEEK_CFG         0x02
#define KT0810_REG_TUNE             0x03
#define KT0810_REG_VOL_CFG          0x04
#define KT0810_REG_DSP_CFG          0x05
#define KT0810_REG_RF_CFG           0x09
#define KT0810_REG_LO_SYN_CFG       0x0A
#define KT0810_REG_SYS_CFG          0x0F
#define KT0810_REG_STATUS_A         0x12
#define KT0810_REG_STATUS_B         0x13
#define KT0810_REG_STATUS_C         0x14
#define KT0810_REG_STATUS_D         0x15
#define KT0810_REG_ANTENNA          0x1D
#define KT0810_REG_SNR              0x1F
#define KT0810_REG_SEEKTH           0x20
#define KT0810_REG_SOFTMUTE         0x21
#define KT0810_REG_CLOCK            0x23

///----------------------------------------------------------------------------
/// Bits of registers mask macro definition.
///----------------------------------------------------------------------------
#define REG_MASK_STATUS_A_XTAL_OK       0x8000
#define REG_MASK_STATUS_A_PLL_LOCK      0x0800
#define REG_MASK_STATUS_A_STC           0x4000
#define REG_MASK_STATUS_A_SF            0x2000
#define REG_MASK_STATUS_B_CHAN          0x03FF

#define REG_MASK_SEEK_CFG_SEEKDIR       0x4000
#define REG_MASK_SEEK_CFG_SEEK          0x8000
#define REG_MASK_SEEK_CFG_SPACE         0x000C

#define REG_MASK_VOL_CFG_MUTE_B         0x4000
#define REG_MASK_VOL_CFG_VOL            0x000F

#define REG_MASK_TUNE_ENABLE            0x8000
#define REG_MASK_TUNE_CHAN              0x03FF

///----------------------------------------------------------------------------
/// Setting of fields in registers macro definition for my need.
/// It is supposed to be used combining with bits mask.
/// The value here has been put its position in a register like the mask.
/// For the field only with one bit, we can directly use mask as the value. 
///----------------------------------------------------------------------------
#define REG_FIELD_SEEK_CFG_SPACE_100K   0x0004
#define REG_FIELD_VOL_CFG_VOL_MAX       0x000F
#define REG_FIELD_VOL_CFG_VOL_MIN       0x0000

///----------------------------------------------------------------------------
/// Configuration macro definition for my need.
///----------------------------------------------------------------------------
#define KT0810_DEVICE_ADDR          0x37 // The first 7 bits.
#define KT0810_SEEK_DOWN            0
#define KT0810_SEEK_UP              1
#define KT0810_TUNE_CHAN_MIN        0x1CC
#define KT0810_TUNE_CHAN_MAX        0x370
#define KT0810_VOL_MAX              0x0F
#define KT0810_VOL_MIN              0




///----------------------------------------------------------------------------
/// KT0810 low level driver to write a register.
///
/// It is actually a wrapper of i2c driver.
///
/// @date 2013/01/06
///----------------------------------------------------------------------------         
int8 kt0810_write_reg (uint8 reg, uint16 val)
{
    return i2c_write_reg_word (KT0810_DEVICE_ADDR<<1, reg, val);
} 

///----------------------------------------------------------------------------
/// KT0810 low level driver to read a register.
///
/// It is actually a wrapper of i2c driver.
///
/// @date 2013/01/06
///----------------------------------------------------------------------------         
int8 kt0810_read_reg (uint8 reg, uint16 *p_val)
{
    return i2c_read_reg_word_kt0810 (KT0810_DEVICE_ADDR<<1, reg, p_val);
}

///----------------------------------------------------------------------------
/// Initialize FM ic based on my need:
///     1. Traditional, Cycling seek.
///     2. Autotune.
///     3. Maybe softmute...
///     4. No any interrupt.
///     5. Channel spacing is 100K.
///     6. Channel range is 87M-108M.
///     7. After power on, the default tune channel is 87M.
///
/// Most of the default value after power on is what I want. I only need to do
/// a little configuration.
///
/// @date 2013/01/05
///----------------------------------------------------------------------------
void kt0810_init(void)
{
    uint16 reg;

    // The ic requires to wait for at least 400ms to use i2c bus after power on.
    // When we are here in a real application, it should have already been more
    // than 400ms since power on. But for safe in some special cases, still do
    // some delay here.
    USER_DelayDTms (200);

    // Check the system status.
    kt0810_read_reg (KT0810_REG_STATUS_A, &reg);
    if ( !(reg & REG_MASK_STATUS_A_XTAL_OK)
        && !(reg & REG_MASK_STATUS_A_PLL_LOCK) )
    {
        dbprintf ("kt0810 is not ready\n");
    }

    // Read the manufacture ID and chip ID.
    kt0810_read_reg (KT0810_REG_DEVICE_ID, &reg);
    if (reg == 0xB002)
    {
        dbprintf ("mfg ID is correct\n");
    }
    dbprintf ("kt0810 mfg ID is %x\n", reg);

    kt0810_read_reg (KT0810_REG_CHIP_ID, &reg);
    if (reg == 0x0440)
    {
        dbprintf ("chip ID is correct\n");
    }
    dbprintf ("kt0810 chip ID is %x\n", reg);

    // Just set the channel spacing from default 200K to 100K.
    kt0810_read_reg (KT0810_REG_SEEK_CFG, &reg);
    kt0810_write_reg (KT0810_REG_SEEK_CFG,
                      (reg&(~REG_MASK_SEEK_CFG_SPACE)) | REG_FIELD_SEEK_CFG_SPACE_100K);

    // Set the volume to the max. No hard mute.
    // It's lazy to set register like this way below...
    kt0810_read_reg (KT0810_REG_VOL_CFG, &reg);
    kt0810_write_reg (KT0810_REG_VOL_CFG,
                      reg|REG_MASK_VOL_CFG_MUTE_B|REG_MASK_VOL_CFG_VOL);


    // At this point, kt0810 is in a pause status waiting for my control.
    
}

///----------------------------------------------------------------------------
/// Check if the seek or tune operation is done
///
/// Several operation will do this. So I make it independent here.
///
/// @date 2013/01/06
///----------------------------------------------------------------------------         
void kt0810_is_done (void)
{   
    uint16 reg;
    uint32 station;

    while (1) // Very dangerous...
    {
        kt0810_read_reg (KT0810_REG_STATUS_A, &reg);

        if (reg & REG_MASK_STATUS_A_STC)
        {
            if (reg & REG_MASK_STATUS_A_SF)
            {
                dbprintf ("Seek fails\n");
            }
            
            // Let's see the current station.
            kt0810_read_reg (KT0810_REG_STATUS_B, &reg);
            station = (reg&REG_MASK_STATUS_B_CHAN)*50 + 64000;
            dbprintf ("Current station is %lx\n", station);   

            break; // Quit the polling.
        }
    }
}

///----------------------------------------------------------------------------
/// Read the current station.
///
/// @date 2013/01/15
///----------------------------------------------------------------------------
uint32 kt0810_get_station(void)
{
    uint16 reg;
    uint32 station;

    kt0810_read_reg (KT0810_REG_STATUS_B, &reg);
    station = (reg&REG_MASK_STATUS_B_CHAN)*50 + 64000;
    dbprintf ("Current station is %lx\n", station);   

    return station;
}

///----------------------------------------------------------------------------
/// Seek to a station
///
/// @param uint8 seek_dir: 0 is downward, 1 is upward.
/// @date 2013/01/06
///----------------------------------------------------------------------------         
void kt0810_seek(uint8 seek_dir)
{
    uint16 reg;
    uint16 channel_bits;
    uint32 station; // unit is K Hz.

    // Read the current channel.
    kt0810_read_reg (KT0810_REG_STATUS_B, &reg);
    channel_bits = reg&REG_MASK_STATUS_B_CHAN;
    station = channel_bits*50 + 64000;
    dbprintf ("station is %Lx before seek\n", station);

    // Set the seek direction.
    kt0810_read_reg (KT0810_REG_SEEK_CFG, &reg);
    if (seek_dir)
    {
        // Seek up.
        kt0810_write_reg (KT0810_REG_SEEK_CFG, reg|REG_MASK_SEEK_CFG_SEEKDIR);
    }
    else
    {
        // Seek down.
        kt0810_write_reg (KT0810_REG_SEEK_CFG, reg&(~REG_MASK_SEEK_CFG_SEEKDIR));
    }

    // Start to seek.
    kt0810_write_reg (KT0810_REG_SEEK_CFG, reg|REG_MASK_SEEK_CFG_SEEK);

    // Check how the seek did.
    kt0810_is_done();
}

///----------------------------------------------------------------------------
/// Tune by a step.
///
/// After power on, the default tune channel is 87M and the chan bits in tune
/// register is 0x1CC. When at the 108M, the bits will be 0x370.
///
/// @param uint8 seek_dir: 0 is downward, 1 is upward.
/// @date 2013/01/06
///----------------------------------------------------------------------------         
void kt0810_tune_step (uint8 tune_dir)
{
    uint16 reg;
    uint16 channel_bits;
    uint32 station;

    // Read the current channel.
    kt0810_read_reg (KT0810_REG_STATUS_B, &reg);
    channel_bits = reg&REG_MASK_STATUS_B_CHAN;
    station = channel_bits*50 + 64000;
    dbprintf ("station is %lx before tune\n", station);

    // Adjust a new channel. If the channel has been at the edge of band, cycle it.
    if (tune_dir) // Upward.
    {
        if(channel_bits>=(KT0810_TUNE_CHAN_MAX-1))
        {
            channel_bits = KT0810_TUNE_CHAN_MIN;
        }    
        else
        {
            channel_bits += 2; // Step is 100k.
        }
    }
    else // Downward.
    {
        if (channel_bits<=(KT0810_TUNE_CHAN_MIN+1))
        {
            channel_bits = KT0810_TUNE_CHAN_MAX;
        }
        else
        {
            channel_bits -= 2;
        }    
    }

    // Write the new channel to tune register and start to tune.
    kt0810_read_reg (KT0810_REG_TUNE, &reg);
    kt0810_write_reg (KT0810_REG_TUNE, (reg&(~REG_MASK_TUNE_CHAN))|channel_bits);
    kt0810_read_reg (KT0810_REG_TUNE, &reg);
    kt0810_write_reg (KT0810_REG_TUNE, reg|REG_MASK_TUNE_ENABLE);

    // Check if the tune complets.
    kt0810_is_done();

}

///----------------------------------------------------------------------------
/// Tune to a station we specify.
///
/// @param uint32 station: unit is KHz. The band we use is 87M-108M.
///
/// @date 2013/01/08
///----------------------------------------------------------------------------   
void kt0810_set_station (uint32 station)
{
    uint16 reg;
    uint16 channel_bits;

    // Check the validity of the station frequency.
    if ((station < 87000) || (108000<station))
    {
        dbprintf ("Excess the band range\n");
        return;
    }

    // Calculate the new channel bits.
    channel_bits = (station-64000)/50;

    // Write the new channel to tune register and start to tune.
    kt0810_read_reg (KT0810_REG_TUNE, &reg);
    kt0810_write_reg (KT0810_REG_TUNE, (reg&(~REG_MASK_TUNE_CHAN))|channel_bits);
    kt0810_read_reg (KT0810_REG_TUNE, &reg);
    kt0810_write_reg (KT0810_REG_TUNE, reg|REG_MASK_TUNE_ENABLE);

    // Wait until it is done.
    kt0810_is_done();
}

///----------------------------------------------------------------------------
/// Adjust the volume by step.
///
/// The step is 1.
///
/// @param uint8 dir: 0 means vol down; 1 means vol up.
///
/// @date 2013/01/09
///----------------------------------------------------------------------------   
void kt0810_vol_change (uint8 dir)
{
    uint16 reg;
    uint8 vol;
        
    // Read the current volume.
    kt0810_read_reg (KT0810_REG_VOL_CFG, &reg);
    vol = reg & REG_MASK_VOL_CFG_VOL;

    if (dir)
    {
        if (vol == KT0810_VOL_MAX)
        {
            return;
        }
        else
        {
            vol++;
        }
    }
    else
    {
        if (vol == KT0810_VOL_MIN)
        {
            return;
        }
        else
        {
            vol--;
        }
    }

    kt0810_write_reg (KT0810_REG_VOL_CFG, (reg&(~REG_MASK_VOL_CFG_VOL) | vol));

    // Read the volume value again for verification.
    kt0810_read_reg (KT0810_REG_VOL_CFG, &reg);
    dbprintf ("New vol is %bx\n", reg & REG_MASK_VOL_CFG_VOL);
}

///----------------------------------------------------------------------------
/// Read the volume field.
/// 
/// @return uint8: the volume value.
/// @date 2013/01/14
///----------------------------------------------------------------------------   
uint8 kt0810_get_vol (void)
{
    uint16 reg;
       
    // Read the volume.
    kt0810_read_reg (KT0810_REG_VOL_CFG, &reg);
    return reg & REG_MASK_VOL_CFG_VOL;
}




///----------------------------------------------------------------------------
/// kt0810 test routine.
///
/// @date 2013/01/06
///----------------------------------------------------------------------------   
#if (KT0810_TEST == FEATURE_ON)

void kt0810_test (void)
{
    uint8 i;
    uint16 reg;

    kt0810_init();

    //kt0810_seek(1);
    //kt0810_seek(1);

    // 101.7M: 0x18D44
    kt0810_read_reg (KT0810_REG_TUNE, &reg);
    kt0810_write_reg (KT0810_REG_TUNE, (reg&(~REG_MASK_TUNE_CHAN))|0x2f2);
    kt0810_read_reg (KT0810_REG_TUNE, &reg);
    kt0810_write_reg (KT0810_REG_TUNE, reg|REG_MASK_TUNE_ENABLE);

    // Try to read all the registers.
    for (i=2; i<KT0810_REG_CLOCK; i++)
    {
        kt0810_read_reg (i, &reg);
        dbprintf (" register %bx = %x\n", i, reg);    
    }

    while(1)
    {
        ;
    }
}

#endif      

