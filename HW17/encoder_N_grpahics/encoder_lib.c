#include "encoder_lib.h"


void i2c_write(unsigned char address, unsigned char reg, unsigned char value){
    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;
    i2c_write_blocking(I2C_PORT, address, buf, 2, false); // array is length 2 because we send the register address and contents
}

uint16_t i2c_read(unsigned char address, unsigned char reg){
    unsigned char buf[2]; // registers have 2 addresses
    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);  // true to keep host control of bus
    i2c_read_blocking(I2C_PORT, address, buf, 2, false);  // false - finished with bus

    return (((uint16_t)buf[0]) << 8 | buf[1]) & 0x0FFF;
}

uint16_t as5600_read_angle(void){
    return i2c_read(CHIP_ADDRESS, ANGLE_REG);
}