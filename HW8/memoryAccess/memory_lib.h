#ifndef MEMORY_LIB_h
#define MEMORY_LIB_h

#include "spi_lib.h"

#define SCK1 10
#define TX1 11
#define RX1 12
#define CS1 13

#define WRITE 0b00000010
#define READ 0b00000011

void init_memory(void);
void ram_write_sine(void);
void update_dac_from_ram(int i);

#endif