#include "spi_lib.h"

int main()
{
    init_spi();
    stdio_init_all();

    float voltage[100];
    for(int i = 0; i < 100; i++){
        voltage[i] = 3.3*(sin(2*M_PI*2*i/100)+1)/2;
    }

    float voltage_tri[100];
    for(int i = 0; i < 50; i++){
        voltage_tri[i] = 3.3*i/49.0;
    }
    for(int i = 50; i < 100; i++){
        voltage_tri[i] = 3.3*(99.0-i)/49.0;
    }

    while (true) {
        static int i = 0;
        writeDAC(0, voltage[i]);
        writeDAC(1, voltage_tri[i]);
        i++;
        if(i == 100){
            i = 0;
        }
        sleep_ms(10);
    }
}
