#ifndef FORCE_SENSOR_H
#define FORCE_SENSOR_H

#include <stdio.h>
#include "pico/stdlib.h"

#define SCK 16
#define DT 17
#define clock_time_us 1

void init_HX711(void);
int read_DT(void);

#endif