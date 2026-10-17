#include "atm2560.h"
#include "main.h"

int	received_data_from_cst816d()
{
	return !(TOUCH_GPIO_PINS & (1 << TOUCH_INT_PIN));
}

// Initialize the tactile driver of the touchscreen
// by setting the correct pins as inputs and/or pullup
void	cst816d_init()
{
	// SDA and SCK are initialized by i2c_init()

	// Set INT as input with its active-low pull-up.
	set_pin_as_input(&TOUCH_GPIO_DDR, TOUCH_INT_PIN);
	set_pin_high(&TOUCH_GPIO_PORT, TOUCH_INT_PIN);

	// // Set RESET as output
	// set_pin_as_output(&TOUCH_GPIO_DDR, TOUCH_RST_PIN);
}

void	onTouch()
{
	i2c_start();
	i2c_write(TOUCH_ADDR_CST816D << 1 | 0);
	i2c_write(0x01);
	// i2c_read_ack();

	i2c_start();
	i2c_write((TOUCH_ADDR_CST816D << 1) | 1);

	uint8_t action = i2c_read_ack();
	uint8_t points = i2c_read_ack();
	uint8_t x_high = i2c_read_ack();
	uint8_t x_low = i2c_read_ack();
	uint8_t y_high = i2c_read_ack();
	uint8_t y_low = i2c_read_nack();

	i2c_stop();

	// voiding probably bad reading cuz further than surface of TFT
	(void)x_high;
	(void)y_high;

	uint16_t x = x_low;
	uint16_t y = y_low;

	if (points > 0) {
		uart0_printstr("action: ");
		uart0_print_10bit(action);
		if (action == (uint8_t)TFT_ACTION_UP)
			uart0_printstr("swipe up\r\n");
		else if (action == (uint8_t)TFT_ACTION_DOWN)
			uart0_printstr("swipe down\r\n");
		else if (action == (uint8_t)TFT_ACTION_RIGHT)
			uart0_printstr("swipe right\r\n");
		else if (action == (uint8_t)TFT_ACTION_LEFT)
			uart0_printstr("swipe left\r\n");
		else if (action == (uint8_t)TFT_ACTION_TAP)
			uart0_printstr("tap\r\n");
		else
			uart0_printstr("autre action\r\n");
		uart0_printstr("Toucher detecte ! X: ");
		uart0_print_10bit(x);
		uart0_printstr(" | Y: ");
		uart0_print_10bit(y);
		uart0_printstr("\r\n");
	}

	if (action == TFT_ACTION_TAP)
	{
		uint16_t pointer = 180;
		pointer = putstr_screen(pointer, 100, "x: ", RGB16_WHITE, FONT_SIZE_SMALL);
		pointer = putint_screen(pointer, 100, x, RGB16_WHITE, FONT_SIZE_SMALL);
		putstr_screen(pointer, 100, "  ", RGB16_WHITE, FONT_SIZE_SMALL);
		pointer = putstr_screen(180, 110, "y: ", RGB16_WHITE, FONT_SIZE_SMALL);
		pointer = putint_screen(pointer, 110, y, RGB16_WHITE, FONT_SIZE_SMALL);
		putstr_screen(pointer, 110, "  ", RGB16_WHITE, FONT_SIZE_SMALL);
		if (g_touchscreen_mode == TFT_STATE_COLORSCREEN)
		{
			pointer = putstr_screen(170, 120, "rgb: 0x", RGB16_WHITE, FONT_SIZE_SMALL);
			pointer = puthex_screen(pointer, 120, get_color_from_colorscreen(x, y), RGB16_WHITE, FONT_SIZE_SMALL);
			putstr_screen(pointer, 120, "  ", RGB16_WHITE, FONT_SIZE_SMALL);
		}

	}
	else if (action != g_touchscreen_action)
	{
		g_touchscreen_action = action;
		if (g_touchscreen_action == (uint8_t)TFT_ACTION_LEFT)
			g_touchscreen_mode = TFT_STATE_HOMESCREEN;
		if (g_touchscreen_action == (uint8_t)TFT_ACTION_RIGHT)
			g_touchscreen_mode = TFT_STATE_COLORSCREEN;
		if (g_touchscreen_action == (uint8_t)TFT_ACTION_UP)
			gc9a01_fill_screen(RGB16_MAGENTA);
	}
}
