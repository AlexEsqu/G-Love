#include "../includes/config_atm2560.h"

void	set_pin_as_input(volatile uint8_t* reg, uint8_t pin)
{
	*reg &= ~(1 << pin);
}

void	set_pin_as_output(volatile uint8_t* reg, uint8_t pin)
{
	*reg |= (1 << pin);
}

void	set_pin_up(volatile uint8_t* reg, uint8_t pin)
{
	*reg |= (1 << pin);
}

void	set_pin_low(volatile uint8_t* reg, uint8_t pin)
{
	*reg &= ~(1 << pin);
}
