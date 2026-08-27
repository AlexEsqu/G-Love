#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define S0 2
#define S1 0
#define S2 1

#define INHIBIT 3

#define Y_OUT 6;
#define X_OUT 7;

typedef struct adc_data_s {
    int16_t flex[5];
    int16_t pressure[5];
    bool touch;
} adc_data_t;

void adc_init(void);
uint16_t adc_read(uint8_t channel);
void adc_read_data(adc_data_t *data);

#endif // ADC_H