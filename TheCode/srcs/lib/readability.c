#include "main.h"

void	set_pin_as_input(volatile uint8_t* reg, uint8_t pin)
{
	*reg &= ~(1 << pin);
}

void	set_pin_as_output(volatile uint8_t* reg, uint8_t pin)
{
	*reg |= (1 << pin);
}

void	set_pin_high(volatile uint8_t* reg, uint8_t pin)
{
	*reg |= (1 << pin);
}

void	set_pin_low(volatile uint8_t* reg, uint8_t pin)
{
	*reg &= ~(1 << pin);
}

uint32_t rgb565_to_rgb888(uint16_t rgb565)
{
	uint8_t r5 = (rgb565 >> 11) & 0x1F;
	uint8_t g6 = (rgb565 >> 5) & 0x3F;
	uint8_t b5 = rgb565 & 0x1F;

	uint8_t r8 = (r5 << 3) | (r5 >> 2);
	uint8_t g8 = (g6 << 2) | (g6 >> 4);
	uint8_t b8 = (b5 << 3) | (b5 >> 2);

	return ((uint32_t)r8 << 16) | ((uint32_t)g8 << 8) | b8;
}
