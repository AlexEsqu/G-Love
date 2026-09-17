#include "atm328p.h"
#include <stdint.h>
#include "../../includes/main.h"
#include "../muxSelect/mux.h"

void setup()
{
	i2c_init();
	adc_init();
	spi_init();
	uart0_init(BAUDRATE);

	mux_setup();

}

int main_ATM328P()
{
	setup();

	while (1) {
		
		mux_loop();

		_delay_ms(200);
	}
}