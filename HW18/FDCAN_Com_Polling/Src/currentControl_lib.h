#ifndef CURRENTCONTROL_LIB_H
#define CURRENTCONTROL_LIB_H

#include "main.h"
#include "string.h"

#define INA219_ADDR 0b1000000

#define INA219_REG_CONFIG 0x00
#define INA219_REG_CURRENT 0x04
#define INA219_REG_CALIBRATION 0x05
#define DESIRED_CURRENT_RAW 150 // since the actual current (mA) is raw/3, this translates to 50mA (small value for safety)
#define MAX_SAMPLES 400
#define U_MAX 2400 // max raw PWM value
#define EINT_MAX 150

extern volatile uint8_t state;
extern volatile signed short current_array[MAX_SAMPLES];
extern volatile signed short desired_array[MAX_SAMPLES];
extern volatile float desired_current;

void print_STM32(char *message);
uint32_t read_ADC(void);
void init_PWM_STM32(void);
void init_ina219();
float read_ina219();
void set_motor_pwm(int pwm);

#endif
