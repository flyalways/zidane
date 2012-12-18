////////////////////////////////////////////////////////////////////////////////
/// @file 
/// @brief i2c driver simulated by gpio.
///
/// Currently this is built based on common i2c interface. Modification
/// is needed if difference exists when applied to another ic.
///
/// Pay attention to the initial state of the bus at the beginning of each
/// driver function. Try to keep each independent.
///
/// @author William Chang
/// @date   2012/10/28
////////////////////////////////////////////////////////////////////////////////

#include "SPDA2K.H"
#include "i2c.h"
#include "i2c_sim.h"

///============================================================================
/// Here are some useful notes about i2c specification:
///
/// i2c specification defines what is: START, STOP, ACK, NO-ACK, address, and the
/// data frame of one byte. But it does not define the application layer protocol.
/// Each i2c compatible device could define its own protocol by themselves. So does
/// tca8418a.
///
/// Here are some functions which are very common for some devices using i2c bus.
/// But for some devices like tca8418a, we need to implement its own protocol based
/// on its spec.
///
/// 1. Start signal: the falling edge of SDA when SCL is high.
/// 2. Stop signal: the rising edge of SDA when SCL is high.
/// 3. Ack signal: receiver keeps the SDA low when SCL is high.
/// 4. Only when SCL is low, SDA is allowed to change as data bit. Otherwise,
///    it will be treated as control signals like start, stop if SDA changes when
///    SCL is low.
/// 5. It's master to start a transfer, terminate a transfer and generate the
///    clock signal.
/// 6. When the bus is free, both lines are HIGH. That is the status before STOP
///    signal and after STOP signal. But between byte and byte in the tranfer,
///    it is recommended that SCL stays LOW.
/// 7. The master sends NON-ACK signal to tell slave to release SDA so that
///    master has the chance to send STOP or another START.
/// 8. Pay attention to the duration/timing requirements between signal edges.
///============================================================================


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
/// Initialize i2c bus to master output status. Because we have initialize the
/// gpios to output mode when system boots up, we don't need to use it unless
/// we set SDA line as INPUT and forget to change it back somewhere.
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
    I2C_SDA_SET_OUTPUT;

    I2C_SDA_HIGH;
    i2c_sim_delay(1);
    I2C_SCL_HIGH;
    i2c_sim_delay(1);

    I2C_SDA_LOW;        // Send start signal.
    i2c_sim_delay(1);

    I2C_SCL_LOW;        // Hold clock to be ready to transfer data.
    i2c_sim_delay(1);   // Just for safe.
}

///-----------------------------------------------------------------------------
/// Send stop condition in i2c bus simulated by gpio.
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
    I2C_SDA_SET_OUTPUT;

    I2C_SCL_LOW;        // Avoid re-send a start signal
    i2c_sim_delay(1);

    I2C_SDA_LOW;
    i2c_sim_delay(1);
    I2C_SCL_HIGH;
    i2c_sim_delay(1);

    I2C_SDA_HIGH;       // Send stop signal. 
    i2c_sim_delay(1);

    // After STOP signal is set out, the bus status is free:
    // both of SDA and SCL are HIGH.
}

///-----------------------------------------------------------------------------
/// Send ack in i2c bus simulated by gpio.
///
/// Keep SDA low during a ack-related clock pulse. This function should be called
/// after receiving a byte from slave. So check the status of SDA and SCL afer
/// i2c_sim_get_byte() runs. SDA line should be OUTPUT at this moment.
/// Normally, it is better to keep all functions independent if possible.
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
    i2c_sim_delay(1);   // Just for safe. Maybe redundant since SCL has been LOW for a while. 
    I2C_SDA_LOW;
    i2c_sim_delay(1);

    I2C_SCL_HIGH;
    i2c_sim_delay(1);
    I2C_SCL_LOW;
}

///-----------------------------------------------------------------------------
/// Do not send ack in i2c bus simulated by gpio.
///
/// Keep SDA high during a ack-related clock pulse.
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
    i2c_sim_delay(1);   // Just for safe. Maybe redundant since SCL has been LOW for a while. 
    I2C_SDA_HIGH;
    i2c_sim_delay(1);

    I2C_SCL_HIGH;
    i2c_sim_delay(1);
    I2C_SCL_LOW;
}

///-----------------------------------------------------------------------------
/// Send one byte data on the i2c bus simulated by gpio.
///
/// MSB first.
///
/// In order to control the SDA direction correctly, note that this function will
/// follow below functions:
///     1. START. SDA is OUTPUT already of course.
///     2. Itself. SDA mode depends how the function handles in the end.
///
/// No erro handling here.
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

    // Initial bus state to send 8 bits.
    // See the comment in the beginning. Maybe below set is redundant. Anyway, it
    // is safe.
    I2C_SCL_LOW;
    I2C_SDA_SET_OUTPUT;

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
    //
    // Here I2C_SDA_HIGH is not a must. I do this is just to get the potential
    // communication error. Because the ACK signal is a low from receiver. If
    // the last bit the transmmiter sends is a low, then it will certainly get
    // a low from the same pin when it wants to check if there is a ACK even if
    // the receiver does not send ACK. 
    //I2C_SDA_HIGH;
    I2C_SDA_SET_INPUT;

    // Receive ack.
    i2c_sim_delay(2);
    I2C_SCL_HIGH;
    i2c_sim_delay(2);
    
    if (I2C_SDA)
    {
        ack = I2C_ERROR_NO_ACK;    
    }

    // Don't set a specific SDA status in the end. It's enough to do the correct
    // initialization at the beginning. But it is good to keep SCL LOW at the end.
    I2C_SCL_LOW;
    i2c_sim_delay(1);

    return ack;
}

///-----------------------------------------------------------------------------
/// Receive one byte data on the i2c bus simulated by gpio.
///
/// MSB first. Send ack back or not based on the caller. After this function runs,
/// SDA is OUTPUT mode.
///
/// In order to control the SDA direction correctly, note that this function will
/// follow below functions possibly:
///     1. i2c_sim_send_byte(). SDA is OUTPUT there.
///     2. Itself. SDA mode depends how the function handles in the end.
///
/// We shouldn't handle error at this level. Leave it in upper API. No error
/// indicated from this function.
///
/// @param  int8 *p_dat: hold the data received.
///         bool ack:
///                 TRUE:   send ack.
///                 FALSE:  do not send ack.
///
/// @return none
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
void i2c_sim_get_byte (int8 *p_dat, bool ack_or_not)
{
    uint8 i;
    int8 bits=0;

    // Initial bus state.
    I2C_SCL_LOW;
    I2C_SDA_SET_INPUT;
    i2c_sim_delay(1);

    // Receive data bit by bit from the bus.
    for (i=0; i<8; i++)
    {
        bits<<=1;

        I2C_SCL_HIGH;
        i2c_sim_delay(1);
        bits += (int8) I2C_SDA;
        I2C_SCL_LOW;
        i2c_sim_delay(1);
    }

    // Give slave transmitter some time to release SDA. And the slave should
    // do that... Then set SDA line to OUTPUT mode in order to ack or non-ack.
    i2c_sim_delay(2);
    I2C_SDA_SET_OUTPUT;

    // Ack or not. Note that who is using the SDA.
    if (ack_or_not)
    {
        i2c_sim_ack();
    }
    else
    {
        i2c_sim_ack_not();
    }

    // Don't set a specific SDA status in the end. It's enough to do the correct
    // initialization at the beginning.
    I2C_SCL_LOW;
    i2c_sim_delay(1);

    *p_dat = bits;
}




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Below is higher application level. Functions will call the units above.
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




///-----------------------------------------------------------------------------
/// Write one byte to a register of a device on the i2c bus simulated by gpio.
///
/// Although we do not get ack (you can say it is an error), we don't quit but
/// continue to send remaining stuff. The ideal handle is to wait and check again
/// several times. But it is not implemented here for simplicity.
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
int8 i2c_sim_write_reg_byte (uint8 addr, uint8 reg, int8 val)
{
    int8 err=0;

    // Send start condition.
    i2c_sim_start();

    // Send the device address on the bus.
    if ( i2c_sim_send_byte(addr) )
    {
        err |= I2C_ERROR_NO_ACK;
        dbprintf ("err= %bx sending dev addr in write_reg\n", err);
    }

    // Send the register address to the device.
    if ( i2c_sim_send_byte(reg) )
    {
        err |= I2C_ERROR_NO_ACK;
        dbprintf ("err= %bx sending reg in write_reg\n", err);
    }

    // Send the value to write the register.
    if ( i2c_sim_send_byte(val) )
    {
        err |= I2C_ERROR_NO_ACK;
        dbprintf ("err= %bx sending val in write_reg\n", err);
    }

    // Send stop condition.
    i2c_sim_stop();

    return err;
}

///-----------------------------------------------------------------------------
/// Read one byte from a register of a device on the i2c bus simulated by gpio.
///
/// Although we do not get ack (you can say it is an error), we don't quit but
/// continue. The ideal handle is to wait and check again
/// several times. But it is not implemented here for simplicity.
///
/// @param  uint8 addr: device address on the bus.
///         uint8 reg:  register address in the device.
///         uint8 *val: place to take the value returned from the register.
///
/// @return int8: error and status.
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
#if (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)

// When using function pointer to call below function, we must make the function
// to be a reentrant function since the some parameters passed in can not fit the
// registers.
int8 i2c_sim_read_reg_byte (uint8 addr, uint8 reg, uint8 *p_val) reentrant
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
    i2c_sim_get_byte(p_val, 0);

    // Send the stop signal if needed.
    i2c_sim_stop();    

    return err;
}

#else

int8 i2c_sim_read_reg_byte (uint8 addr, uint8 reg, uint8 *p_val)
{
    int8 err=0;

    // Send start condition.
    i2c_sim_start();

    // Send the device address on the bus.
    if ( i2c_sim_send_byte(addr) )
    {
        err |= I2C_ERROR_NO_ACK;
        dbprintf ("err= %bx sending dev addr in read_reg\n", err);
    }

    // Send the register address to the device.
    if ( i2c_sim_send_byte(reg) )
    {
        err |= I2C_ERROR_NO_ACK;
        dbprintf ("err= %bx sending reg in read_reg\n", err);
    }

    // Should we wait for some time???

    // Receive the byte from the register.
    i2c_sim_get_byte(p_val, 0); // NON-ACK

    // Send the stop signal if needed.
    i2c_sim_stop();    

    return err;
}

///-----------------------------------------------------------------------------
/// Read one byte from a register of tca8418a simulated by gpio.
///
/// This is a specific read function for tca8418a based on its own spec.
/// 
/// Pay attention to the addr parameter.
///
/// @param  uint8 addr: device address for write operation on the bus.
///         uint8 reg:  register address in the device.
///         uint8 *val: place to take the value returned from the register.
///
/// @return int8: error and status.
///
/// @author William Chang
/// @date   2012/10/28
///-----------------------------------------------------------------------------
int8 i2c_sim_read_reg_byte_tca8418a (uint8 addr, uint8 reg, uint8 *p_val)
{
    int8 err=0;

    // Send start condition.
    i2c_sim_start();

    // Send the device address of write operation.
    if ( i2c_sim_send_byte(addr) )
    {
        err |= I2C_ERROR_NO_ACK;
        dbprintf ("err= %bx sending dev write addr in read_reg 8418\n", err);
    }

    // Send the register address to the device.
    if ( i2c_sim_send_byte(reg) )
    {
        err |= I2C_ERROR_NO_ACK;
        dbprintf ("err= %bx sending reg in read_reg 8418\n", err);
    }

    // Send the start signal again.
    i2c_sim_start();

    // Send the device address of read operation. For tca8418a, it is
    // write address plus 1. That is also a common standard for i2c devices.
    if ( i2c_sim_send_byte (addr+1) )
    {
        err |= I2C_ERROR_NO_ACK;
        dbprintf ("err= %bx sending dev read addr in read_reg 8418\n", err);   
    }

    // Should we wait for some time???

    // Receive the byte from the register.
    i2c_sim_get_byte(p_val, 0); // NON-ACK

    // Send the stop signal if needed.
    i2c_sim_stop();    

    return err;
}

#endif // (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)

#if (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)
// Interface provided for clients from i2c_sim module.
xdata i2c_access_t i2c_access_sim = { 
                                        i2c_sim_write_reg_byte,
                                        i2c_sim_read_reg_byte,
                                    };
#endif

#if (I2C_SIM_TEST_PIN == FEATURE_ON)
void i2c_sim_test_pin ()
{
    uint8 value;
    uint8 i;

    dbprintf("\n");

    for (i=0x01; i<0x2e; i++)
    {
        i2c_sim_read_reg_byte_tca8418a (0x68, i, &value);
        dbprintf("%bx, value=%bx\n", i, value);
    }

    for (i=0x01; i<0x2e; i++)
    {
        i2c_sim_write_reg_byte (0x68, i, i);       
    }

    for (i=0x01; i<0x2e; i++)
    {
        i2c_sim_read_reg_byte_tca8418a (0x68, i, &value);
        dbprintf("%bx, value=%bx\n", i, value);       
    }

    while (1)
    {
        USER_DelayDTms(10);
    ;
    }
}
#endif                                      