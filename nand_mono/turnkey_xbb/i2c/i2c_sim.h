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


extern i2c_access_t i2c_access_sim;

#endif