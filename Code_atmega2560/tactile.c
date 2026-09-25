#include "../includes/config_atm2560.h"

int	received_data_from_cst816d()
{
	return (TOUCH_I2C_PINS & (1 << TOUCH_SDA_PIN));
}

// Initialize the tactile driver of the touchscreen
// by setting the correct pins as inputs and/or pullup
void	cst816d_init()
{
	// SDA and SCK are initialized by i2c_init()

	// // Set INT as input
	// set_pin_as_input(&TOUCH_GPIO_DDR, TOUCH_INT_PIN);

	// // Set RESET as output
	// set_pin_as_output(&TOUCH_GPIO_DDR, TOUCH_RST_PIN);
}
