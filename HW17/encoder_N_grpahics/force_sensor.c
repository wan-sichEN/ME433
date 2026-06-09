#include "force_sensor.h"

void init_HX711(void){
    gpio_init(SCK);
    gpio_set_dir(SCK, GPIO_OUT);
    gpio_put(SCK, 0);

    gpio_init(DT);
    gpio_set_dir(DT, GPIO_IN);
}

int read_DT(void){
    // wait for DT to go low
    while(gpio_get(DT)){
        tight_loop_contents();
    }

    unsigned int raw = 0;
    for(int i = 0; i < 24; i++){
        gpio_put(SCK, 1); // start blinking
        sleep_us(clock_time_us);
        raw = (raw << 1) | (gpio_get(DT) ? 1 : 0);
        gpio_put(SCK, 0);
        sleep_us(clock_time_us);
    }

    // set gain to 128
    gpio_put(SCK, 1); // start blinking
    sleep_us(clock_time_us);
    gpio_put(SCK, 0);
    sleep_us(clock_time_us);

    // sign-extend 24-bit two's complement to 32-bit signed int
    if (raw & 0x800000) {
        raw |= 0xFF000000;
    }

    return (int) raw;
}