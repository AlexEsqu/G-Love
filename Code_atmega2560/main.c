#include "../includes/config_atm2560.h"
#include "../includes/main.h"

tft_state_t g_touchscreen_mode = TFT_STATE_COLORSCREEN;
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
	gc9a01_display_homescreen();
	// gc9a01_send_image(95, 95, &hand1);
}

int main(void)
{
	setup();
	while (1)
	{
		// gc9a01_send_image(95, 95, &hand1);
		// _delay_ms(100);
		// gc9a01_send_image(95, 95, &hand2);
		// _delay_ms(100);
		;
	}
	return 0;
}
