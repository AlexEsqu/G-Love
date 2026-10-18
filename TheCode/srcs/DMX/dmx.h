#ifndef DMX_H
#define DMX_H

#include <stdint.h>

#define DMX_BAUDRATE 250000UL
#define DMX_CHANNEL 1U

void dmx_init(void);
uint8_t dmx_send_frame(uint8_t channel, uint8_t value);
void dmx_breathing_loop(void);

#endif
