#include "main.h"
#include "dmx.h"
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

void setup()
{
	// reset 328p
	PORTC &= ~(1 << PORTC0);
	DDRC  |=  (1 << DDC0);
	_delay_ms(20);
	DDRC  &= ~(1 << DDC0);
	_delay_ms(50);

	// i2c init
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

	SREG |= (1 << 7);
}

int main_ATM2560()
{
	setup();

	// packets from the 328P are handled by the UART1 interrupt (comms.c)
	while (1)
	{
		if (g_packet_to_export.last_touch.is_pending)
		{
			g_packet_to_export.last_touch.is_pending = 0;
			onTouch();
		}
		if (g_current_uart_output == OUTPUT_TO_DMX)
			dmx_breathing_loop();
		refresh_screen();
	}
	return 0;
}
