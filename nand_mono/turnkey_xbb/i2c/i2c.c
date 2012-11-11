////////////////////////////////////////////////////////////////////////////////
/// @file 
/// @brief i2c driver common interface.
///
/// Any device which wants to use i2c driver should call functions here using
/// data types defined here.
/// You can use the functions here directly or use the i2c_driver variable.
/// Currently we only define two functions for i2c_driver but we probably will
/// add more fields.
///
/// @author William Chang
/// @date   2012/11/03
////////////////////////////////////////////////////////////////////////////////

#include "SPDA2K.H"
#include "i2c.h"
#include "i2c_sim.h"

#if (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)

int8 i2c_write_reg_byte (
                            i2c_driver_t *i2c_driver,
                            uint8 addr,
                            uint8 reg,
                            int8 val
                        ) reentrant
{
    if ((i2c_driver)&&(i2c_driver->i2c_access))
    {
        return i2c_driver->i2c_access->write_reg_byte (addr, reg, val);    
    }

    return I2C_ERROR_NO_DRIVER;
}

int8 i2c_read_reg_byte  (
                            i2c_driver_t *i2c_driver,
                            uint8 addr,
                            uint8 reg,
                            uint8 *p_val,
                            bool is_last
                        ) reentrant
{
    if ((i2c_driver)&&(i2c_driver->i2c_access))
    {
        return i2c_driver->i2c_access->read_reg_byte (addr,reg,p_val,is_last);
    }

    return I2C_ERROR_NO_DRIVER;
}

//-----------------------------------------------------------------------------
// i2c driver interface provided to clients.
// Assign an i2c dirver for each device. Just we use the same i2c driver so
// drivers are the same. I do this just for better modularization.
//-----------------------------------------------------------------------------
xdata i2c_driver_t i2c_driver_tca8418 = {
                                            &i2c_access_sim,  
                                            &i2c_write_reg_byte,
                                            &i2c_read_reg_byte,
                                        };

xdata i2c_driver_t i2c_driver_kt0810 = {
                                           &i2c_access_sim,  
                                           &i2c_write_reg_byte,
                                           &i2c_read_reg_byte,
                                       };
#else

int8 i2c_write_reg_byte (
                            uint8 addr,
                            uint8 reg,
                            int8 val
                        )
{
    return i2c_sim_write_reg_byte (addr, reg, val);
}

int8 i2c_read_reg_byte  (
                            uint8 addr,
                            uint8 reg,
                            uint8 *p_val,
                            bool is_last
                        )
{
    return i2c_sim_read_reg_byte (addr, reg, p_val, is_last);
}

#endif // (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)