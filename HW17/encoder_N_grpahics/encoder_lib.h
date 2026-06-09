#ifndef ENCODER_LIB_H
#define ENCODER_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define CHIP_ADDRESS 0x36
#define ANGLE_REG 0x0E // starting address

uint16_t as5600_read_angle(void);

#endif