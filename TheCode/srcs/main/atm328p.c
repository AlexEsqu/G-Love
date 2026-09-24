#include <stdint.h>
#include "atm328p.h"
#include "../../includes/main.h"
#include "../muxSelect/mux.h"
#include "../ledStrip/led.h"

t_sensor_data g_sensor = {0};

void setup()
{

	SREG |= (1 << 7); // Active manuellement le bit d'interruption globale (Bit I)
	i2c_init();
	adc_init();
	uart0_init(BAUDRATE);
    spi_init();

	mux_setup();
}

int main_ATM328P()
{
	setup();

	while (1) {
		led_spi();
	}
}