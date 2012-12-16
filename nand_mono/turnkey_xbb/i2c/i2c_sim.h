#ifndef _I2C_SIM_H_
#define _I2C_SIM_H_

sbit I2C_SDA = P1^5;
sbit I2C_SCL = P1^4;

#define I2C_SDA_HIGH        I2C_SDA=1
#define I2C_SDA_LOW         I2C_SDA=0
#define I2C_SCL_HIGH        I2C_SCL=1
#define I2C_SCL_LOW         I2C_SCL=0
#define I2C_SDA_SET_INPUT   I2C_SDA_HIGH; XBYTE[0xB103] &=0xFD
#define I2C_SDA_SET_OUTPUT  I2C_SDA_HIGH; XBYTE[0xB103] |=0x02

#if (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)

extern i2c_access_t i2c_access_sim;

#else

extern int8 i2c_sim_write_reg_byte (uint8 addr, uint8 reg, int8 val);
extern int8 i2c_sim_read_reg_byte (uint8 addr, uint8 reg, uint8 *p_val, bool is_last);
#if (I2C_SIM_TEST_PIN == FEATURE_ON)
extern void i2c_sim_test_pin();
#endif

#endif // (FEATURE_I2C_DRIVER_VTBL == FEATURE_ON)

#endif