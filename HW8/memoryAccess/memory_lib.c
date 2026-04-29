#include "memory_lib.h"

void spi_ram_init();

void init_memory(void){
    spi_init(spi1, 1000 * 1000); // the baud, or bits per second
    gpio_set_function(RX1, GPIO_FUNC_SPI);
    gpio_set_function(SCK1, GPIO_FUNC_SPI);
    gpio_set_function(TX1, GPIO_FUNC_SPI);
    
    gpio_init(CS1);
    gpio_set_dir(CS1, GPIO_OUT);
    gpio_put(CS1, 1);

    spi_ram_init();
}

void spi_ram_init(){
    uint8_t data[2];
    data[0] = 0x01; // to write to status register
    data[1] = 0b01000000; // sequential mode

    cs_select(CS1);
    spi_write_blocking(spi1, data, 2); // where data is a uint8_t array with length len
    cs_deselect(CS1);
}

void spi_ram_write(uint16_t addr, uint8_t *data){ // writing 2 bytes at a time
    uint8_t packet[5];
    packet[0] = WRITE;
    packet[1] = (addr >> 8) & 0xFF; // first half of addr
    packet[2] = addr & 0xFF; // second half of addr
    packet[3] = data[0];
    packet[4] = data[1];

    cs_select(CS1);
    spi_write_blocking(spi1, packet, 5);
    cs_deselect(CS1);
}

void spi_ram_read(uint16_t addr, uint8_t *data){ // reading 2 bytes at a time and storing them in data
    uint8_t packet[5];
    packet[0] = READ;
    packet[1] = (addr >> 8) & 0xFF; // first half of addr
    packet[2] = addr & 0xFF; // second half of addr
    packet[3] = 0;
    packet[4] = 0;

    uint8_t read_array[5];
    cs_select(CS1);
    spi_write_read_blocking(spi1, packet, read_array, 5);
    cs_deselect(CS1);
    data[0] = read_array[3];
    data[1] = read_array[4];
}

void ram_write_sine(void){
    uint8_t data[2];
    uint16_t data_short = 0;
    uint8_t spi_channel = 0;
    uint16_t addr = 0;

    uint16_t voltage = 0;
    for(int i = 0; i < 1000; i++){
        voltage = (sin(2*M_PI*i/1000.0)+1)*511.5 + 0.5; // 16 bit representation of the voltage float w/ 0.5 for rounding
        
        data_short = ((spi_channel&1) << 15) | (0b111 << 12); // prepping data for spi_write
        data_short = data_short | ((voltage<<2) & 0b111111111111); // b/c last 2 bits are zero and voltage is initially 10 bits
        data[0] = data_short >> 8;
        data[1] = data_short & 0xFF;
        spi_ram_write(addr, data);
        addr += 2;
    }
}

void update_dac_from_ram(int i){// reads 2 data points starting from address i and sends them to dac
    uint8_t data[2];
    spi_ram_read(i, data);

    cs_select(CS); // CS of dac pin
    spi_write_blocking(spi_default, data, 2);
    cs_deselect(CS);
}