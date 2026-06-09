#include "encoder_lib.h"
#include "force_sensor.h"

int main()
{
    stdio_init_all();
    init_HX711();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    float t = 0.0f;

    while (true) {
        // reading angle
        uint16_t angle = as5600_read_angle();
        float python_position = ((float)angle-2800.0)/1000.0;
        if(python_position > 1.0){
            python_position = 1.0;
        }
        if(python_position < -1.0){
            python_position = -1.0;
        }

        // reading force
        int filtered_val = 662000; // average value from sensor
        int value = read_DT();
        filtered_val = 0.05*value + 0.95*filtered_val;

        printf("%.3f\t%d\n", python_position, value);
        sleep_ms(10);
        // float paddle_pos = sinf(t);  // oscillates -1.0 to 1.0
        // printf("%.3f\n", paddle_pos);
        // t += 0.05f;
        // sleep_ms(10);  // ~100Hz
    }
}