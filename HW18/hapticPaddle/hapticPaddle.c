#include <stdio.h>
#include "pico/stdlib.h"
#include "can_lib.h"
#include "encoder_lib.h"
#include "force_sensor.h"
 
#define CAN_ID 0x111
#define MAX_ADDED_FORCE_RAW 25000
#define MAX_DEPTH 0.25 // corresponds to graphics
#define BASE_FORCE 662000
#define KP_FORCE 0.5
#define KD_FORCE 0.5
#define DT 0.001f

float haptic_force(float position);

bool force_control_callback(repeating_timer_t *rt) {
     // reading motor position
    uint16_t angle = as5600_read_angle();
    float python_position = ((float)angle-2800.0)/1000.0;
    if(python_position > 1.0){
        python_position = 1.0;
    }
    if(python_position < -1.0){
        python_position = -1.0;
    }

    // reading force
    int filtered_force = BASE_FORCE; // average value from sensor
    int force = read_DT();
    filtered_force = 0.10*force + 0.90*filtered_force;

    // PD control with haptics
    float desired_force = haptic_force(python_position);

    static volatile float e_prime = 0.0f;

    // PI Control
    float error = desired_force - force;
    static float prev_error = 0.0f;
    float derivative = (error - prev_error) / DT;
    prev_error = error;

    float u = KP_FORCE*error + KD_FORCE*e_prime;

    // conversion to desired current
    float desired_current = u/1000; // scale down because raw force values are too high

    // debugging
    printf("%.3f\t%.3f\t%.3f\t%.3f", python_position, desired_force, filtered_force, desired_current);
    
    can_send_float(CAN_ID, u);
    return true;  // keep repeating
}

int main(void) {
    stdio_init_all();
    sleep_ms(100);
    
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

    // Timer Initialization for PD Control
    repeating_timer_t timer;
    add_repeating_timer_ms(-1, force_control_callback, NULL, &timer);
 
    while (true) {
        tight_loop_contents();
        // CAN Communication with STM32
        // bool acked = can_send_float(CAN_ID, desired_current);
 
        // if (!acked) {
        //     printf("CAN no ACK\n");
        // }
 
        // next_time += PERIOD_US;
        // uint32_t now = time_us_32();
        // if ((int32_t)(next_time - now) > 0) {
        //     sleep_us(next_time - now);
        // }
    }
}

float haptic_force(float position) {
    static float prev_position = 0.0f;
    if (position >= 0.0f){
        prev_position = position;
        return 0.0f;  // in air, no force
    } 

    float depth = -position;  // 0 to 1 as you go deeper

    if(depth >= MAX_DEPTH){
        depth = MAX_DEPTH;
    }

    // sqrt allows for sharp-ish onset that levels off deeper
    float added_force = (depth/MAX_DEPTH) * sqrtf(depth/MAX_DEPTH) * MAX_ADDED_FORCE_RAW;  // (depth/max depth)^1.5 -> results in max added force at max depth

    float delta_pos = position - prev_position;
    // oppose the direction of motion
    if (delta_pos > 0.0f) {
        return BASE_FORCE + added_force;   // moving forward, resist forward
    } else if (delta_pos < 0.0f) {
        return BASE_FORCE - added_force;   // moving backward, resist backward
    } else {
        return BASE_FORCE;               // stationary, just hold neutral
    }

    return added_force;
}