#include "spi_lib.h"

void init_spi(void){
    spi_init(spi_default, 1000 * 1000); // the baud, or bits per second
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    gpio_init(CS);
    gpio_set_dir(CS, GPIO_OUT);
    gpio_put(CS, 1);
}

void writeDAC(int channel, float voltage){// where channel determines Vout A or B
    uint8_t data[2];
    data[0] = 0b01110000; // 4 bits of voltage + 1 or 0 for channel communication
    data[1] = 0; // remaining 6 bits of voltage

    uint16_t digital_v = voltage/3.3 * 1023; // converts voltage into a 10 bit binary value
    data[0] = data[0] | ((channel & 1) << 7) | ((digital_v >> 6) & 0b1111); // put channel bit and first 4 voltage bits in

    data[1] = data[1] | ((digital_v << 2) & 0xFF); // 0b1111_111111 -> 0b1111_11111100 & 0xFF -> get remaining bits in 0b11111100 format
    
    cs_select(CS);
    spi_write_blocking(spi_default, data, 2); // where data is a uint8_t array with length len
    cs_deselect(CS);

}