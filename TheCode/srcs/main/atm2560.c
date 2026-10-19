#include "main.h"
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

void setup()
{
	// protocols
	i2c_init();
	uart0_init();
	uart1_init();
	spi_init();

	// driver specific sequences
	cst816d_init();
	gc9a01_init();

	// enable interrupts and timers
	milisecond_timer_init();
	enableInterrupts();
	enableTouchInterrupt();

	display_screen();
}

int main_ATM2560()
{
	setup();
	uint8_t		byte;

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
