#include <stdio.h>
#include "pico/stdlib.h"

#define SCK 16
#define DT 17
#define clock_time_us 1

void init_HX711(void);
int read_DT(void);

int main()
{
    stdio_init_all();
    init_HX711();

    int raw_values[1000];
    int iir_values[1000];
    uint64_t t[1000];

    while (true) {
        int filtered_val = 667000; // average value from sensor
        int num = 0;
        scanf("%d", &num);
        for(int i = 0; i < num; i++){
            int value = read_DT();
            filtered_val = 0.05*value + 0.95*filtered_val;
            raw_values[i] = value;
            iir_values[i] = filtered_val;
            t[i] = to_ms_since_boot(get_absolute_time()); 
            printf("%llu %d %d\n", t[i], raw_values[i], iir_values[i]);
        }
    }
}

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