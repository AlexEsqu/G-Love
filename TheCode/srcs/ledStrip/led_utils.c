#include "led.h"
#include "../../includes/main.h"
#include <avr/interrupt.h>

strip_buffer_t strip;

const uint16_t TOTAL_BYTES = sizeof(strip_buffer_t);
static uint8_t spi_buffer[sizeof(strip_buffer_t)];
static volatile uint16_t spi_index;
static volatile bool spi_busy;

// Unique initialisation of the fixed values of the LED buffer (start, end, and header of the LEDs)
void strip_init(uint8_t brightness)
{
    for (uint8_t i = 0; i < 4; i++) strip.start[i] = 0x00;
    
    uint8_t hdr = 0xE0 | (brightness & 0x1F);
    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        strip.leds[i].header = hdr;
        strip.leds[i].blue = 0;
        strip.leds[i].green = 0;
        strip.leds[i].red = 0;
    }

    for (uint8_t i = 0; i < END_FRAME_BYTES; i++) strip.end[i] = 0x00;
}

bool strip_flush(void)
{
    if (spi_busy)
    {
        return false;
    }

    for (uint16_t i = 0; i < TOTAL_BYTES; i++)
    {
        spi_buffer[i] = ((uint8_t *)&strip)[i];
    }

    spi_index = 1;
    spi_busy = true;
    SPCR |= (1 << SPIE);
    SPDR = spi_buffer[0];
    return true;
}

bool strip_is_busy(void)
{
    return spi_busy;
}

ISR(SPI_STC_vect)
{
    if (spi_index < TOTAL_BYTES)
    {
        SPDR = spi_buffer[spi_index++];
    }
    else
    {
        SPCR &= ~(1 << SPIE);
        spi_busy = false;
    }
}

/* Converts a color from HSV to RGB.
 * h is hue (color), as a number between 0 and 360.
 * s is the saturation (intensity), as a number between 0 and 255. (0 - 100%)
 * v is the value (luminosity), as a number between 0 and 255. (0 - 100%) */
rgb_color_t hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return (rgb_color_t){r, g, b};
}

// Fast 8-bit saturating addition (caps the value at uint8_t max instead of overflow)
uint8_t qadd8(uint8_t a, uint8_t b)
{
    uint16_t res = (uint16_t)a + b;
    return (res > 255) ? 255 : (uint8_t)res;
}
