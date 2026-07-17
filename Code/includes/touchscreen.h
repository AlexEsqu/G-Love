#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <stdint.h>

#define TOUCHSCREEN_ADDR 0x38

typedef struct touchscreen_data_s {
    uint16_t x;
    uint16_t y;
} touchscreen_data_t;

#endif