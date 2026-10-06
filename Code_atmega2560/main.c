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

	gc9a01_fill_screen(RGB565_YELLOW);

	touch_timer_init();
	enableTouchInterrupt();
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
