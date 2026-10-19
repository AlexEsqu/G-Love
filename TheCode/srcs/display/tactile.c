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

	// Set RESET as output, reset pulse: low 10 ms, then high,
	set_pin_as_output(&TOUCH_GPIO_DDR, TOUCH_RST_PIN);
	set_pin_low(&TOUCH_GPIO_PORT, TOUCH_RST_PIN);
	_delay_ms(10);
	set_pin_high(&TOUCH_GPIO_PORT, TOUCH_RST_PIN);
	_delay_ms(50);
}

inline uint8_t	is_right_quarter_of_screen(uint16_t row, uint16_t col)
{
	return (row < col && (row + col) > 240);
}

inline uint8_t	is_bottom_quarter_of_screen(uint16_t row, uint16_t col)
{
	return (row > col && (row + col) > 240);
}

inline uint8_t	is_left_quarter_of_screen(uint16_t row, uint16_t col)
{
	return (row > col && (row + col) < 240);
}

inline uint8_t	is_top_quarter_of_screen(uint16_t row, uint16_t col)
{
	return (row < col && (row + col) < 240);
}

inline uint8_t	is_in_top_left_of_screen(uint16_t row, uint16_t col)
{
	return (col < 120 && row < 120);
}

inline uint8_t	is_in_top_right_of_screen(uint16_t row, uint16_t col)
{
	return (col >= 120 && row < 120);
}

inline uint8_t	is_in_bottom_right_of_screen(uint16_t row, uint16_t col)
{
	return (col < 120 && row >= 120);
}

inline uint8_t	is_in_bottom_left_of_screen(uint16_t row, uint16_t col)
{
	return (col >= 120 && row >= 120);
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
	else if (g_last_touch.action == TFT_ACTION_SINGLE_TAP)
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
	g_touchscreen_mode = TFT_STATE_HOMESCREEN;
	refresh_screen();
}

void	update_touch_info(uint16_t x, uint16_t y, uint16_t action)
{
	uint8_t	last_action = g_last_touch.last_action;

	g_last_touch.x = x;
	g_last_touch.y = y;
	g_last_touch.rgb565 = get_color_from_colorscreen(y, x);
	g_last_touch.action = (uint8_t)action;
	g_last_touch.last_action = last_action;
	g_last_touch.is_pending = 0;

	if (action == TFT_ACTION_SINGLE_TAP
		|| action == TFT_ACTION_DOUBLE_TAP
		|| action == TFT_ACTION_TAP
		|| action == TFT_ACTION_LONG_TAP)
		g_screens[g_touchscreen_mode].on_touch_action();
	else if (action != last_action)
		navigate_touchscreen();

	g_packet_to_export.screen_x = x;
	g_packet_to_export.screen_y = y;
	g_packet_to_export.screen_action = action;
	send_g_love_directive();
}

void	print_touch_info(tft_touch_info_t* touch_info)
{
	putstr_screen(180, 60, "Touch", RGB16_WHITE, FONT_SIZE_SMALL);
	uint16_t pointer = 180;
	pointer = putstr_screen(pointer, 70, "x: ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putint_screen(pointer, 70, touch_info->x, RGB16_WHITE, FONT_SIZE_SMALL);
	putstr_screen(pointer, 70, "  ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(180, 80, "y: ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putint_screen(pointer, 80, touch_info->y, RGB16_WHITE, FONT_SIZE_SMALL);
	putstr_screen(pointer, 80, "  ", RGB16_WHITE, FONT_SIZE_SMALL);
}

void	onTouch()
{
	i2c_start();
	i2c_write(TOUCH_ADDR_CST816D << 1 | 0);
	if (g_i2C_state != 0x18) // SLA+W not acknowledged: CST816D not answering
	{
		i2c_stop();
		return;
	}
	i2c_write(0x01);

	i2c_start();
	i2c_write((TOUCH_ADDR_CST816D << 1) | 1);
	if (g_i2C_state != 0x40) // SLA+R not acknowledged
	{
		i2c_stop();
		return;
	}
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
		update_touch_info(x, y, action);
}
