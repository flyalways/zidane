#ifndef _I2C_H_
#define _I2C_H_

typedef struct
{
    (int8 *) write_reg_byte (uint8 addr, uint8 reg, int8 val);
    (int8 *) read_reg_byte  (uint8 addr,
                             uint8 reg,
                             int8 *p_val,
                             bool is_last);
} i2c_access_t;

struct i2c_driver_st
{
    i2c_access_t *i2c_access;
    (int8 *) i2c_write_reg_byte ( (i2c_driver_st *) i2c_driver,
                                  uint8 addr,
                                  uint8 reg,
                                  int8 val );
    (int8 *) i2c_read_reg_byte ( (i2c_driver_st *) i2c_driver,
                                 uint8 addr,
                                 uint8 reg,
                                 int8 *p_val,
                                 bool is_last );
};

typedef struct i2c_driver_st i2c_driver_t;


#endif