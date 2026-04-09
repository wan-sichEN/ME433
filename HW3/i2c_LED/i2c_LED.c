#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
#define PICO_LED 25

#define CHIP_ADDR 0b00100000

struct repeating_timer timer;

void setPin(unsigned char address, unsigned char register, unsigned char value);
unsigned char readPin(unsigned char address, unsigned char register);

bool ledHeartbeat(struct repeating_timer *t){
    static bool led_state = 1;
    gpio_put(PICO_LED, led_state);
    led_state = !led_state;
    return true;
}

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    gpio_init(PICO_LED);
    gpio_set_dir(PICO_LED, GPIO_OUT);

    add_repeating_timer_ms(-2000, ledHeartbeat, NULL, &timer);

    // initializing GP7 as output and GP0 as input
    setPin(CHIP_ADDR, 0x00, 0b01111111); //all other pins are initialized to inputs

    // test run to blink LED
    while (true) {
        unsigned char pin_status = readPin(CHIP_ADDR, 0x09);
        if(pin_status & 1){ // if the first bit of pin_status (GP0) is 1
            setPin(CHIP_ADDR, 0x0A, 0x80); // so GP7 is 1 (high)
        }
        else{
            setPin(CHIP_ADDR, 0x0A, 0x00); // so GP7 is 0 (low)
        }
    }
}

void setPin(unsigned char address, unsigned char reg, unsigned char value){
    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;
    i2c_write_blocking(i2c_default, address, buf, 2, false); // array is length 2 because we send the register address and contents
}

unsigned char readPin(unsigned char address, unsigned char reg){
    unsigned char buf;
    i2c_write_blocking(i2c_default, address, &reg, 1, true);  // true to keep host control of bus
    i2c_read_blocking(i2c_default, address, &buf, 1, false);  // false - finished with bus

    return buf;

}