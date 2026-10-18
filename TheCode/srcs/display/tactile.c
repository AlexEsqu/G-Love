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

tft_action_t	match_touch_action(uint16_t action)
{
	switch (action)
	{
		case (uint8_t)TFT_ACTION_TAP:
			return (TFT_ACTION_TAP);
		case (uint8_t)TFT_ACTION_LEFT:
			return (TFT_ACTION_LEFT);
		case (uint8_t)TFT_ACTION_UP:
			return (TFT_ACTION_UP);
		case (uint8_t)TFT_ACTION_RIGHT:
			return (TFT_ACTION_RIGHT);
		case (uint8_t)TFT_ACTION_DOWN:
			return (TFT_ACTION_DOWN);
		default:
			return (TFT_ACTION_NONE);
	}
}

void	debug_touch_info()
{
	uart0_printstr("action: ");
	uart0_print_10bit(g_last_touch.action);
	if (g_last_touch.action == TFT_ACTION_UP)
		uart0_printstr("swipe up\r\n");
	else if (g_last_touch.action == TFT_ACTION_DOWN)
		uart0_printstr("swipe down\r\n");
	else if (g_last_touch.action == TFT_ACTION_RIGHT)
		uart0_printstr("swipe right\r\n");
	else if (g_last_touch.action == TFT_ACTION_LEFT)
		uart0_printstr("swipe left\r\n");
	else if (g_last_touch.action == TFT_ACTION_TAP)
		uart0_printstr("tap\r\n");
	else
		uart0_printstr("autre action\r\n");
	uart0_printstr("Toucher detecte ! X: ");
	uart0_print_10bit(g_last_touch.x);
	uart0_printstr(" | Y: ");
	uart0_print_10bit(g_last_touch.y);
	uart0_printstr("\r\n");
}

void	navigate_touchscreen()
{
	switch (g_last_touch.action)
	{
		// why can't I iterate on enums grrrm
		// horrible for now, required sanity check
		case TFT_ACTION_UP:
			if (g_touchscreen_mode == TFT_STATE_COMMSCREEN
				|| g_touchscreen_mode == TFT_STATE_HOMESCREEN)
				g_touchscreen_mode = TFT_STATE_COLORSCREEN;
			else if (g_touchscreen_mode == TFT_STATE_FLEXSCREEN)
				g_touchscreen_mode = TFT_STATE_COMMSCREEN;
			else if (g_touchscreen_mode == TFT_STATE_IMUSCREEN)
				g_touchscreen_mode = TFT_STATE_FLEXSCREEN;
			else
				g_touchscreen_mode = TFT_STATE_IMUSCREEN;
			break;
		case TFT_ACTION_DOWN:
			if (g_touchscreen_mode == TFT_STATE_COLORSCREEN
				|| g_touchscreen_mode == TFT_STATE_HOMESCREEN)
				g_touchscreen_mode = TFT_STATE_COMMSCREEN;
			else if (g_touchscreen_mode == TFT_STATE_COMMSCREEN)
				g_touchscreen_mode = TFT_STATE_FLEXSCREEN;
			else if (g_touchscreen_mode == TFT_STATE_IMUSCREEN)
				g_touchscreen_mode = TFT_STATE_COLORSCREEN;
			break;
		default:
			g_touchscreen_mode = TFT_STATE_HOMESCREEN;
			break;
	}

	refresh_screen();
}

void	update_touch_info(uint16_t x, uint16_t y, uint16_t action)
{
	uint8_t	last_action = g_last_touch.action;

	g_last_touch.x = x;
	g_last_touch.y = y;
	g_last_touch.rgb565 = get_color_from_colorscreen(y, x);
	g_last_touch.rgb888 = rgb565_to_rgb888(g_last_touch.rgb565);
	g_last_touch.action = match_touch_action(action);
	g_last_touch.last_action = last_action;
	g_last_touch.has_displayed = 0;

	if (action != last_action && action != TFT_ACTION_TAP)
		navigate_touchscreen();
}

void	print_touch_info(tft_touch_info_t* touch_info)
{
	putstr_screen(180, 90, "Touch", RGB16_WHITE, FONT_SIZE_SMALL);
	uint16_t pointer = 180;
	pointer = putstr_screen(pointer, 100, "x: ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putint_screen(pointer, 100, touch_info->x, RGB16_WHITE, FONT_SIZE_SMALL);
	putstr_screen(pointer, 100, "  ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(180, 110, "y: ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putint_screen(pointer, 110, touch_info->y, RGB16_WHITE, FONT_SIZE_SMALL);
	putstr_screen(pointer, 110, "  ", RGB16_WHITE, FONT_SIZE_SMALL);
}

void	onTouch()
{
	i2c_start();
	i2c_write(TOUCH_ADDR_CST816D << 1 | 0);
	i2c_write(0x01);

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

	

	if (points > 0)
	{
		update_touch_info(x, y, action);
		debug_touch_info();
	}
}
