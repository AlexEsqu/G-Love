#include "../includes/config_atm2560.h"
#include "../includes/main.h"

tft_state_t g_touchscreen_mode = TFT_STATE_HOMESCREEN;
uint8_t g_animation_frame = 0;

void setup()
{
	uart0_init(BAUDRATE);
	uart1_init(BAUDRATE);
	i2c_init();
	spi_init();
	cst816d_init();
	gc9a01_init();

	touch_timer_init();
	enableTouchInterrupt();

	gc9a01_fill_screen(RGB16_BLUE);
	display_screen();
	// gc9a01_send_image(95, 95, &hand1);
	putstr_screen(0, 20, "hello", RGB16_GRAY);
}

int main(void)
{
	setup();
	while (1)
	{
		refresh_screen();
		putstr_screen_big(150, 40, "hello", RGB16_BLACK);
		putstr_screen_big(90, 70, "world", RGB16_PINK);
	}
	return 0;
}
