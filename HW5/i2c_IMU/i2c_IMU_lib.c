#include "i2c_IMU_lib.h"

// need to use i2c1 because i2c0 is being used by the display
#define I2C_PORT i2c1
#define I2C_SDA 18
#define I2C_SCL 19

void initMPU6050(void){
    // initialize i2c for imu chip
    i2c_init(I2C_PORT, 400*1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // turn on chip
    setPin(CHIP_ADDR, PWR_MGMT_1, 0x00);

    // enable accelerator
    setPin(CHIP_ADDR, ACCEL_CONFIG, 0x00);

    // enable gyroscope
    setPin(CHIP_ADDR, GYRO_CONFIG, 3 << 3);
}

void setPin(unsigned char address, unsigned char reg, unsigned char value){
    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;
    i2c_write_blocking(i2c1, address, buf, 2, false); // array is length 2 because we send the register address and contents
}

unsigned char readPin(unsigned char address, unsigned char reg){
    unsigned char buf;
    i2c_write_blocking(i2c1, address, &reg, 1, true);  // true to keep host control of bus
    i2c_read_blocking(i2c1, address, &buf, 1, false);  // false - finished with bus

    return buf;
}

void readAll(unsigned char *buf){// where buf is a 14 long array
    unsigned char reg = ACCEL_XOUT_H; // i2c_write_blocking expects a pointer to the register, but ACCEL_XOUT_H is a constant
    i2c_write_blocking(i2c1, CHIP_ADDR, &reg, 1, true);  // start at ACCEL_XOUT_H, true to keep host control of bus
    i2c_read_blocking(i2c1, CHIP_ADDR, buf, 14, false);  // read 14 sequencial bytes, false - finished with bus
}

int16_t recombine(unsigned char byte_high, unsigned char byte_low){
    return (int16_t)((byte_high << 8) | byte_low);
}

void drawVerticalLine(int x, int y){ // like |, but only 4 pixels tall and 1 pixel wide for more flexibility
    for(int row = 0; row < 4; row++){
        ssd1306_drawPixel(x, y+row, 1);
    }
}

void deleteVerticalLine(int x, int y){ 
    for(int row = 0; row < 4; row++){
        ssd1306_drawPixel(x, y+row, 0);
    }
}

void drawLine_y(int num){ // num_max is 4 (can fit 8 4 pixel lines on a 32 height display, and num is signed)
    if(num < 0){
        for(int i = 0; i < abs(num); i++){
            drawVerticalLine(64,18+i*4);
        }
        for(int i = abs(num); i < 4; i++){
            deleteVerticalLine(64, 18+i*4);
        }
    }
    else if(num > 0){
        for(int i = 0; i < num; i++){
            drawVerticalLine(64,14-4*i);
        }
        for(int i = abs(num); i < 4; i++){
            deleteVerticalLine(64, 14-4*i);
        }
    }
    else{
        for(int i = 0; i < 8; i++){
            deleteVerticalLine(64, 0+4*i); // delete everything
        }
    }

}

void drawHorLine(int x, int y){ // like _, but only 1 pixel tall for more flexibility
    for(int col = 0; col < 5; col++){
        ssd1306_drawPixel(x+col, y, 1);
    }
}

void deleteHorLine(int x, int y){ 
    for(int col = 0; col < 5; col++){
        ssd1306_drawPixel(x+col, y, 0);
    }
}

void drawLine_x(int num){
    if(num > 0){ 
        for(int i = 0; i < abs(num); i++){
            drawHorLine(65+i*5,18);
        }
        for(int i = abs(num); i < 12; i++){
            deleteHorLine(65+i*5,18);
        }
    }
    else if(num < 0){
        for(int i = 0; i < abs(num); i++){
            drawHorLine(58-5*i,18);
        }
        for(int i = abs(num); i < 12; i++){
            deleteHorLine(58-5*i,18);
        }
    }
    else{
        for(int i = 0; i < 24; i++){
            deleteHorLine(3+5*i, 18); // delete everything
        }
    }

}

void drawLines(float accel_x, float accel_y){
    int num_x = (int)(accel_x*40.0);
    int num_y = (int)(accel_y*20.0);
    if(num_x > 12){
        num_x = 12;
    }
    else if(num_x < -12){
        num_x = -12;
    }
    if(num_y > 4){
        num_y = 4;
    }
    else if(num_y < -4){
        num_y = -4;
    }

    drawLine_y(num_y); 
    drawLine_x(num_x);
}