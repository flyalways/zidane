#ifndef _I2C_H_
#define _I2C_H_

//------------------------------------------------------------------------------
// Definition of i2c bus error.
//
// The error code we get at application could be a combination of errors here.
//------------------------------------------------------------------------------
#define I2C_ERROR_NONE          0x00
#define I2C_ERROR_NO_ACK        0x01
#define I2C_ERROR_NO_DRIVER     0x02 // Driver initialized to NULL.

#if (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)

typedef struct
{
    int8 (*write_reg_byte) (uint8 addr, uint8 reg, int8 val);
    int8 (*read_reg_byte)  (uint8 addr,
                             uint8 reg,
                             int8 *p_val,
                             bool is_last) reentrant;
} i2c_access_t;

struct i2c_driver_st
{
    i2c_access_t *i2c_access;
    int8 (*i2c_write_reg_byte) (  struct i2c_driver_st *i2c_driver,
                                  uint8 addr,
                                  uint8 reg,
                                  int8 val ) reentrant;
    int8 (*i2c_read_reg_byte) (  struct i2c_driver_st *i2c_driver,
                                 uint8 addr,
                                 uint8 reg,
                                 int8 *p_val) reentrant;
};

typedef struct i2c_driver_st i2c_driver_t;




#if 0 // Another way to define the interface style.
typedef struct 
{
    const i2c_access_t *i2c_access;
    const struct i2c_driver_st *vtbl_p;
} i2c_driver_t;

typedef struct
{
    int8 (*i2c_write_reg_byte) (  i2c_driver_t *i2c_driver,
                                  uint8 addr,
                                  uint8 reg,
                                  int8 val );
    int8 (*i2c_read_reg_byte) ( (i2c_driver_t *) i2c_driver,
                                 uint8 addr,
                                 uint8 reg,
                                 int8 *p_val);        
} i2c_driver_st;
#endif

extern i2c_driver_t i2c_driver_tca8418;
extern i2c_driver_t i2c_driver_kt0810;

#else // (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)

extern int8 i2c_write_reg_byte (uint8 addr,uint8 reg,int8 val);
extern int8 i2c_read_reg_byte  (uint8 addr,uint8 reg,uint8 *p_val);

#endif // (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)

#endif