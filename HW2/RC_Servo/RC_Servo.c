#include <stdio.h> // set pico_enable_stdio_usb to 1 in CMakeLists.txt 
#include "pico/stdlib.h" // CMakeLists.txt must have pico_stdlib in target_link_libraries
#include "hardware/pwm.h" // CMakeLists.txt must have hardware_pwm in target_link_libraries
#include "hardware/adc.h" // CMakeLists.txt must have hardware_adc in target_link_libraries

#define PWMPIN 16

void setServo(int angle);

bool timer_interrupt_function(__unused struct repeating_timer *t) {
    // read the adc
    uint16_t result1 = adc_read();
    // print the voltage
    printf("%f\r\n",(float)result1/4095*3.3);
    return true;
}

int main()
{
    stdio_init_all();

    // turn on a timer interrupt
    struct repeating_timer timer;
    // -100 means call the function every 100ms
    // +100 would mean call the function 100ms after the function has ended
    add_repeating_timer_ms(-100, timer_interrupt_function, NULL, &timer);

    // turn on the pwm: 50 Hz with a resolution of 60000
    gpio_set_function(PWMPIN, GPIO_FUNC_PWM); // Set the Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(PWMPIN); // Get PWM slice number
    // the clock frequency is 150MHz divided by a float from 1 to 255
    float div = 50; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // sets the clock speed
    uint16_t wrap = 60000; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap); 
    pwm_set_enabled(slice_num, true); // turn on the PWM

    pwm_set_gpio_level(PWMPIN, 1500/2); // set the duty cycle to 50%

    // turn on the adc
    adc_init();
    adc_gpio_init(26); // pin GP26 is pin ADC0
    adc_select_input(0); // sample from ADC0

    while (true) {
        // sweep through the servo angles
        for(int i = 10; i < 170; i++){
            setServo(i);
            sleep_ms(10);
        }
        for(int i = 170; i > 10; i--){
            setServo(i);
            sleep_ms(10);
        }
    }
}

void setServo(int angle){
    float duty_cycle = 0.025 + 0.1*(angle/180.0);
    pwm_set_gpio_level(PWMPIN, (int)(duty_cycle*60000));
}