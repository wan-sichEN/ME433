#include "i2c_IMU_lib.h"

int main()
{
    stdio_init_all();
    
    init_i2cDisplay();
    
    initMPU6050();
    
    // read from WHO_AM_I to confirm chip readiness
    unsigned char who_am_i = readPin(CHIP_ADDR, WHO_AM_I);
    if (who_am_i != 0x68){
        LED_error_state = true;
        while (true){
            tight_loop_contents();
            printf("%d", who_am_i);
        }
    }
    
    while (true) {
        unsigned char data[14];
        readAll(data);
        float accel_x = 0.000061 * recombine(data[0], data[1])-0.105; // convert to g's
        float accel_y = 0.000061 * recombine(data[2], data[3])+0.01;
        float accel_z = 0.000061 * recombine(data[4], data[5]);
        float temp = recombine(data[6], data[7])/340.0 + 36.53; // convert to celcius
        float gyro_x = 0.007630 * recombine(data[8], data[9]) + 0.405; // convert to degrees/sec + filtered
        float gyro_y = 0.007630 * recombine(data[10], data[11]) + 0.185;
        float gyro_z = 0.007630 * recombine(data[12], data[13]);

        // filtering:
        
        if(fabsf(accel_x)<=0.005){
            accel_x = 0;
        }
        if(fabsf(accel_y)<=0.005){
            accel_y = 0;
        }
        
        static float angle_x = 0; 
        static float angle_y = 0;
        angle_x += gyro_x*0.01; 
        angle_y += gyro_y*0.01;

        printf("%f, %f\n", accel_x, accel_y);
        
        drawLines(accel_x, accel_y);

        sleep_ms(10); //~ running at 10ms
        ssd1306_update();
    }
}
