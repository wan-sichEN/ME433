#include <stdio.h>
#include "pico/stdlib.h"
#include "can_lib.h"
#include "encoder_lib.h"
#include "force_sensor.h"
 
#define CAN_ID 0x111
#define MAX_DESIRED_CURRENT 2100 // mA 
#define MAX_ADDED_FORCE_RAW 150000
#define MAX_DEPTH 0.25 // corresponds to graphics
#define BASE_FORCE 674000
#define KP_FORCE 0.5
#define KD_FORCE 0.05
#define DELTA_T 0.001f

float haptic_force(float position, float default_force);

int main(void) {
        stdio_init_all();
        sleep_ms(2000);
        
        // CAN initialization
        can_init();
        
        const uint32_t PERIOD_US = 1000;  // 1000Hz
        uint32_t next_time = time_us_32();
    
        float desired_current = 200.0f;  // replace with your value

        // I2C Initialisation. Using it at 400Khz.
        i2c_init(I2C_PORT, 400*1000);
        
        gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
        gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
        gpio_pull_up(I2C_SDA);
        gpio_pull_up(I2C_SCL);

        // Force Sensing Initialization
        init_HX711();

        // Default Force Value Adjusting
        static float dynamic_base = BASE_FORCE;
        static float calib_sum = 0.0f;
        static int calib_count = 0;

        while (true) {
            // reading motor position
            uint16_t angle = as5600_read_angle();
            float python_position = ((float)angle-2800.0)/1000.0;
            if(python_position > 1.0){
                python_position = 1.0;
            }
            if(python_position < -1.0){
                python_position = -1.0;
            }

            float u = 0.0f;
            
            static float filtered_pos = 0.0f, prev_filtered_pos = 0.0f;
            filtered_pos = 0.1*python_position + 0.9*filtered_pos;

            // reading force
            static int filtered_force = BASE_FORCE; // average value from sensor
            int force = read_DT();
            filtered_force = 0.1*force + 0.9*filtered_force;

            // PD control with haptics
            float desired_force = haptic_force(filtered_pos, dynamic_base);

            static volatile float e_prime = 0.0f;

            float error = desired_force - filtered_force;
            static float prev_error = 0.0f;
            e_prime = (error - prev_error) / DELTA_T;
            prev_error = error;

            u = KP_FORCE*error + KD_FORCE*e_prime;
            
            // conversion to desired current
            float desired_current = u/100; // scale down because raw force values are too high
            if(desired_current > MAX_DESIRED_CURRENT){
                desired_current = MAX_DESIRED_CURRENT;
            }
            else if(desired_current < -MAX_DESIRED_CURRENT){
                desired_current = -MAX_DESIRED_CURRENT;
            }
        
            can_send_float(CAN_ID, desired_current);

            // recalibrate force when nearly stationary (not in water, not moving much)
            if (filtered_pos > 0.01f && fabsf(filtered_pos - prev_filtered_pos) < 0.001f) {
                calib_sum += force;
                calib_count++;
                if (calib_count >= 200) {
                    dynamic_base = calib_sum / calib_count;
                    calib_sum = 0.0f;
                    calib_count = 0;
                    printf("BASE recalibrated: %.0f\n", dynamic_base);
                }
            }

            prev_filtered_pos = filtered_pos;

            // debugging
            //printf("%.3f\t%.3f\t%d\t%.3f\t%.3f\n", filtered_pos, desired_force, force, dynamic_base, desired_current);

            // Graphics
            printf("%.3f\t%d\n", filtered_pos, filtered_force);
    }
}

float haptic_force(float position, float default_force) {
    static float prev_position = 0.0f;
    if (position >= 0.0f){
        prev_position = position;
        return default_force;  // in air, no force
    } 

    float depth = -position;  // 0 to 1 as you go deeper

    if(depth >= MAX_DEPTH){
        depth = MAX_DEPTH;
    }

    // sqrt allows for sharp-ish onset that levels off deeper
    float added_force = (depth/MAX_DEPTH) * sqrtf(depth/MAX_DEPTH) * MAX_ADDED_FORCE_RAW;  // (depth/max depth)^1.5 -> results in max added force at max depth

    float delta_pos = position - prev_position;
    prev_position = position;
    // oppose the direction of motion
    if (delta_pos > 0.002f) { // to account for encoder sensitivities
        return default_force - added_force;   // moving forward, resist forward (tension -> force value decreases)
    } else if (delta_pos < -0.002f) {
        return default_force + added_force;   // moving backward, resist backward (compression -> force value increases)
    } else {
        return default_force;               // stationary, just hold neutral
    }
}