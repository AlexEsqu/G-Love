#include <G_Love.h>

void start_mux(void)
{
    PORTC |= (0 << INHIBIT);
}

void stop_mux(void)
{
    PORTC |= (1 << INHIBIT);
}

void adc_init(void)
{
    // Set the reference voltage to AVCC and select ADC0 as input channel
    ADMUX = (1 << REFS0);

    // Enable the ADC and set the prescaler to 128 for 16MHz clock
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // init selector pins
	DDRC |= (1 << S0);
	DDRC |= (1 << S1);
	DDRC |= (1 << S2);

    DDRC |= (1 << INHIBIT);
    
    start_mux();
}

uint16_t adc_read(uint8_t channel)
{
    // Select the input channel
    ADMUX = (1 << REFS0) | (channel & 0x0F);

    // Start the conversion
    ADCSRA |= (1 << ADSC);

    // Wait for the conversion to complete
    while (ADCSRA & (1 << ADSC));

    // Return the conversion result
    return ADC;
}

void mux_select(uint8_t channel)
{
    if (channel & 1)
        PORTC |= (1 << S0);
    else
        PORTC &= ~(1 << S0);

    if (channel & 2)
        PORTC |= (1 << S1);
    else
        PORTC &= ~(1 << S1);

    if (channel & 4)
        PORTC |= (1 << S2);
    else
        PORTC &= ~(1 << S2);
}

void adc_read_data(adc_data_t *data)
{
    // Read flex and pressure sensor values dual channel
    for (uint8_t i = 0; i < 5; i++)
    {
        mux_select(i);
        data->pressure[i] = adc_read(X_OUT);
        data->flex[i] = adc_read(Y_OUT);
    }

    // Read touch sensor value
    mux_select(6)
    data = adc_read(X_OUT);
    data->touch = (touch_value > TOUCH_THRESHOLD); // TO DO : Define TOUCH_THRESHOLD as needed
}

