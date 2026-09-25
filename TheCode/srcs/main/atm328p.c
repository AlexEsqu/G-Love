#include <stdint.h>
#include "atm328p.h"
#include "../../includes/main.h"
#include "../muxSelect/mux.h"
#include "../ledStrip/led.h"

t_sensor_data g_sensor = {0};

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
		mux_loop(); // multiplexer read
		
		//imu_loop(); // todo
		
		led_spi(); //led strip

		g_sensor.magic = SENSOR_PACKET_MAGIC; // magic number

		//calculate crc value and stored in struct
		g_sensor.crc16 = crc16_compute((const uint8_t*)&g_sensor,	
							offsetof(t_sensor_data, crc16));
		
		//send to atm2560 through uart0
		uart0_send_data((const uint8_t*)&g_sensor, sizeof(g_sensor));

		_delay_ms(50);
	}
}