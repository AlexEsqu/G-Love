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
	static const tft_state_t SCREEN_SEQUENCE[] = {
		TFT_STATE_HOMESCREEN,
		TFT_STATE_COLORSCREEN,
		TFT_STATE_COMMSCREEN,
		TFT_STATE_FLEXSCREEN,
		TFT_STATE_IMUSCREEN
	};
	static const uint8_t NUM_SCREENS = sizeof(SCREEN_SEQUENCE) / sizeof(SCREEN_SEQUENCE[0]);

	int current_index = 0;
    for (int i = 0; i < NUM_SCREENS; i++) {
        if (SCREEN_SEQUENCE[i] == g_touchscreen_mode) {
            current_index = i;
            break;
        }
    }

    switch (g_last_touch.action)
    {
        case TFT_ACTION_UP:
            current_index = (current_index - 1 + NUM_SCREENS) % NUM_SCREENS;
            break;

        case TFT_ACTION_DOWN:
            current_index = (current_index + 1) % NUM_SCREENS;
            break;

        default:
            current_index = 0;
            break;
    }

    g_touchscreen_mode = SCREEN_SEQUENCE[current_index];
	refresh_screen();
}

void	update_touch_info(uint16_t x, uint16_t y, uint16_t action)
{
	uint8_t	last_action = g_last_touch.action;

	g_last_touch.x = x;
	g_last_touch.y = y;
	g_last_touch.rgb565 = get_color_from_colorscreen(y, x);
	g_last_touch.rgb888 = rgb565_to_rgb888(g_last_touch.rgb565);
	g_last_touch.action = (uint8_t)action;
	g_last_touch.last_action = last_action;
	g_last_touch.has_displayed = 0;

	if (action == TFT_ACTION_TAP)
		g_screens[g_touchscreen_mode].on_touch_action();
	else if (action != last_action)
		navigate_touchscreen();
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
		update_touch_info(x, y, action);
}
