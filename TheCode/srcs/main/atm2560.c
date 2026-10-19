#include "main.h"
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

// current touchscreen mode, changes logic of the LEDs and info sent / displayed
tft_state_t			g_touchscreen_mode = TFT_STATE_HOMESCREEN;

// timing for animations
uint8_t				g_animation_frame = 0;


void setup()
{
	// i2c init
	i2c_init();

	// uart_init
	uart0_init(BAUDRATE);
	uart1_init(BAUDRATE);

	spi_init();
	cst816d_init();
	gc9a01_init();
	touch_timer_init();
	enableTouchInterrupt();
	display_screen();
}

int main_ATM2560()
{
	setup();

	uint8_t			byte;

	while (1)
	{
		if (g_packet_to_export.last_touch.is_pending)
		{
			g_packet_to_export.last_touch.is_pending = 0;
			onTouch();
		}
		refresh_screen();

		while (get_next_byte(&byte))
		{
			update_sensor_data(&byte);
		}
	}
	return 0;
}
