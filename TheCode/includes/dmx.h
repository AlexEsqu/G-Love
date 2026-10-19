#ifndef DMX_H
#define DMX_H

#include <stdint.h>

#define DMX_BAUDRATE 250000UL
#define DMX_CHANNEL 1U
#define DMX_MAX_CHANNELS 512U

void dmx_init(void);
void dmx_disable(void);
uint8_t dmx_is_idle(void);
uint8_t dmx_set_channel(uint16_t channel, uint8_t value);
uint8_t dmx_send_frame(void);
void dmx_breathing_loop(void);
void dmx_uart_data_empty(void);

#endif
