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
		g_sensor.crc16 = crc16_compute((uint8_t*)&g_sensor.magic,	
							sizeof(g_sensor) - 2);
		
		//send to atm2561 through uart0
		uart0_send_data((uint8_t*)&g_sensor.magic, sizeof(g_sensor));
		uart0_tx(0x00);

		_delay_ms(50);
	}
}