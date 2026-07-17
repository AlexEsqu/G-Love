#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <stdint.h>

#define TOUCHSCREEN_ADDR 0x15

#define SLIDE_UP 0x01
#define SLIDE_DOWN 0x02
#define SLIDE_LEFT 0x03
#define SLIDE_RIGHT 0x04
#define SINGLE_CLICK 0x05
#define DOUBLE_CLICK 0x0B
#define LONG_PRESS 0x0C

typedef struct touchscreen_data_s {
    uint16_t x;
    uint16_t y;
    uint8_t gesture;
} touchscreen_data_t;

#endif