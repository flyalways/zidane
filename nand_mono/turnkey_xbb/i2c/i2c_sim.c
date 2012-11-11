////////////////////////////////////////////////////////////////////////////////
/// @file 
/// @brief i2c driver simulated by gpio.
///
/// Currently this is built based on the TCA8418 i2c interface. Modification
/// is needed if difference exists when applied to another ic.
///
/// Pay attention to the initial state of the bus at the beginning of each
/// driver function and the final state of the bus in the end in each function.
///
/// @author William Chang
/// @date   2012/10/28
////////////////////////////////////////////////////////////////////////////////

#include "SPDA2K.H"
#include "i2c.h"
#include "i2c_sim.h"

///-----------------------------------------------------------------------------
/// @brief  delay control used in i2c bus simulated by gpio.
///
/// @param  t: counter of loop in while.
///
/// @return
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
void i2c_sim_delay(unsigned int t)
{
	while(0 != t)
    {		
		t--;
    }
}

///-----------------------------------------------------------------------------
/// Initialize i2c bus simulated by gpio.
///
/// Initialize i2c bus to master output status.
///
/// @param  
///
/// @return
///
/// @author William Chang
/// @date   2012/11/03
///-----------------------------------------------------------------------------
void i2c_sim_init (void)
{
    I2C_SDA_SET_OUTPUT;
}

///-----------------------------------------------------------------------------
/// Send start condition in i2c bus simulated by gpio.
///
/// @param  
///
/// @return
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
void i2c_sim_start()
{
    I2C_SDA_HIGH;
    I2C_SCL_HIGH;

    i2c_sim_delay(1);
    I2C_SDA_LOW;    // Send start signal.

    i2c_sim_delay(1);
    I2C_SCL_LOW;    // Hold clock to be ready to transfer data.
}

///-----------------------------------------------------------------------------
/// Send stop condition in i2c bus simulated by gpio.
///
///
/// @param  
///
/// @return
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
void i2c_sim_stop()
{
    I2C_SCL_LOW;    // Avoid re-send a start signal

    I2C_SDA_LOW;
    I2C_SCL_HIGH;

    i2c_sim_delay(1);
    I2C_SDA_HIGH;   // Send stop signal.
    
    i2c_sim_delay(1);
    I2C_SCL_LOW;    // Hold clock 
}

///-----------------------------------------------------------------------------
/// Send ack in i2c bus simulated by gpio.
///
/// Keep SDA low during a clock pulse.
///
/// @param  
///
/// @return
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
void i2c_sim_ack (void)
{
    I2C_SCL_LOW;
    I2C_SDA_LOW;

    I2C_SCL_HIGH;
    i2c_sim_delay(1);
    I2C_SCL_LOW;
}

///-----------------------------------------------------------------------------
/// Do not send ack in i2c bus simulated by gpio.
///
/// Keep SDA high during a clock pulse.
///
/// @param  
///
/// @return
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
void i2c_sim_ack_not (void)
{
    I2C_SCL_LOW;
    I2C_SDA_HIGH;

    I2C_SCL_HIGH;
    i2c_sim_delay(1);
    I2C_SCL_LOW;
}

///-----------------------------------------------------------------------------
/// Send one byte data on the i2c bus simulated by gpio.
///
/// MSB first. After transmittion is done, set the bus back to initial state.
///
/// @param  byte of data you send.
///
/// @return int8:
///             0: receive the ack from device successfully.
///             1: don't receive the ack.
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
int8 i2c_sim_send_byte (int8 dat)
{
    uint8 i;
    int8 ack=0;

    // Initial clock state.
    I2C_SCL_LOW;        

    // Send data bit by bit. MSB first.
    for (i=0; i<8; i++)
    {
        if ( (dat<<i)&0x80 )
        {
            I2C_SDA_HIGH;
        }
        else
        {
            I2C_SDA_LOW;
        }
        i2c_sim_delay(1);

        I2C_SCL_HIGH;
        i2c_sim_delay(1);
        I2C_SCL_LOW;
    }
    
    // Release SDA.
    I2C_SDA_SET_INPUT;

    // Receive ack.
    i2c_sim_delay(1);
    I2C_SCL_HIGH;
    
    if (I2C_SDA)
    {
        ack = I2C_ERROR_NO_ACK;    
    }

    // Back to initial state.
    I2C_SCL_LOW;

    return ack;
}

///-----------------------------------------------------------------------------
/// Receive one byte data on the i2c bus simulated by gpio.
///
/// MSB first. Send ack back or not based on the caller. After this function runs,
/// SDA is INPUT mode.
///
/// @param  int8 *p_dat: hold the data received.
///         bool ack:
///                 TRUE:   send ack.
///                 FALSE:  do not send ack.
///
/// @return unsigned int: data received.
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
int8 i2c_sim_get_byte (int8 *p_dat, bool ack_or_not)
{
    int8 err=0;
    uint8 i;
    int8 bits=0;

    // Initial bus state.
    I2C_SCL_LOW;
    I2C_SDA_SET_INPUT;

    // Receive data bit by bit from the bus.
    for (i=0; i<8; i++)
    {
        bits<<=1;

        I2C_SCL_HIGH;
        bits += (int8) I2C_SDA;
        I2C_SCL_LOW;
        i2c_sim_delay(1);
    }

    // Ack or not. Note that who is using the SDA.
    I2C_SDA_SET_OUTPUT;
    if (ack_or_not)
    {
        i2c_sim_ack();
    }
    else
    {
        i2c_sim_ack_not();
    }

    // Release the SDA.
    I2C_SDA_SET_INPUT;

    *p_dat = bits;

    return err; // Error doesn't exist in this function.
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Below is higher application level. Functions will call the units above.
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




///-----------------------------------------------------------------------------
/// Write one byte to a register of a device on the i2c bus simulated by gpio.
///
/// Although we do not get ack (you can say it is an error), we don't quit but
/// continue to send remaining stuff.
///
/// @param  uint8 addr: device address on the bus.
///         uint8 reg:  register address in the device.
///         int8 val:  value written to the register.
///
/// @return int8: error and status.
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
static int8 i2c_sim_write_reg_byte (uint8 addr, uint8 reg, int8 val)
{
    int8 err=0;

    // Send start condition.
    i2c_sim_start();

    // Send the device address on the bus.
    if ( i2c_sim_send_byte(addr) )
    {
        err |= I2C_ERROR_NO_ACK;
    }

    // Send the register address to the device.
    if ( i2c_sim_send_byte(reg) )
    {
        err |= I2C_ERROR_NO_ACK;
    }

    // Send the value to write the register.
    if ( i2c_sim_send_byte(val) )
    {
        err |= I2C_ERROR_NO_ACK;
    }

    // Send stop condition.
    i2c_sim_stop();

    return err;
}

///-----------------------------------------------------------------------------
/// Read one byte from a register of a device on the i2c bus simulated by gpio.
///
/// Although we do not get ack (you can say it is an error), we don't quit but
/// continue.
///
/// @param  uint8 addr: device address on the bus.
///         uint8 reg:  register address in the device.
///         uint8 *val: place to take the value returned from the register.
///         bool  is_last: if this byte is the last byte we want to get.
///
/// @return int8: error and status.
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
static int8 i2c_sim_read_reg_byte (uint8 addr, uint8 reg, uint8 *p_val, bool is_last) reentrant
{
    int8 err=0;

    // Send start condition.
    i2c_sim_start();

    // Send the device address on the bus.
    if ( i2c_sim_send_byte(addr) )
    {
        err |= I2C_ERROR_NO_ACK;
    }

    // Send the register address to the device.
    if ( i2c_sim_send_byte(reg) )
    {
        err |= I2C_ERROR_NO_ACK;
    }

    // Receive the byte from the register.
    if ( i2c_sim_get_byte(p_val, !is_last) )
    {
        err |= I2C_ERROR_NO_ACK;
    }

    // Send the stop signal if needed.
    if (is_last)
    {
        // After GET function runs, SDA is INPUT mode.
        I2C_SDA_SET_OUTPUT;
        i2c_sim_stop();    
    }

    return err;
}

// Interface provided for clients from i2c_sim module.
xdata i2c_access_t i2c_access_sim = { 
                                        i2c_sim_write_reg_byte,
                                        i2c_sim_read_reg_byte,
                                    };
                                      