#include "../includes/config_atm2560.h"
#include "../includes/main.h"

void setup()
{
	uart0_init(BAUDRATE);
	uart1_init(BAUDRATE);
	i2c_init();
	spi_init();
	cst816d_init();
	gc9a01_init();

	uart0_printstr("Initializing screen\n\r");
	gc9a01_send_init_sequence();
	uart0_printstr("Initializing done\n\r");

	// gc9a01_fill_screen(RGB16_BLUE);

	touch_timer_init();
	enableTouchInterrupt();

	// gc9a01_display_homescreen();
	gc9a01_fill_screen(RGB16_BLACK);
	gc9a01_send_stamp(95, 95);
	_delay_ms(50);
	gc9a01_send_stamp2(95, 95);
}

int g_touchscreen_mode = 0;

int main(void)
{
	setup();
	while (1)
	{
		;
	}
	return 0;
}
