#include "memory_lib.h"


int main()
{
    init_spi();
    init_memory();
    stdio_init_all();

    ram_write_sine(); // initialize sine to ram

    while (true) {
        for(int i = 0; i < 2000; i=i+2){ // addr+=2 for writing sine, so the index of this loop must follow
            update_dac_from_ram(i);
            sleep_ms(1);
        }
    }
}
