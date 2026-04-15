#ifndef I2CDISPLAY_LIB
#define I2CDISPLAY_LIB
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "font.h"
#include "ssd1306.h"

#define PICO_LED 25

extern struct repeating_timer timer;

void init_i2cDisplay(void);
bool ledHeartbeat(struct repeating_timer *t);
void drawLetter(int x, int y, char letter);
void drawWord(int x, int y, char *message);
void printVoltage(void);

#endif