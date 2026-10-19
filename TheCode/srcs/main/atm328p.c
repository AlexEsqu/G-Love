#include <stdint.h>
#include "main.h"
#include "mux.h"
#include "led.h"
#include "icm42688p.h"

t_sensor_data g_sensor = {0};

uint8_t g_len_packet = sizeof(t_sensor_data);

void setup()
{
	SREG |= (1 << 7); // Active manuellement le bit d'interruption globale (Bit I)
	i2c_init();
	adc_init();
	uart0_init();
    spi_init();

	mux_setup();
}

int main_ATM328P()
{
	setup();

	t_imu *imu_conf = imu_conf_create();
	int imu_status = ICM42688_init(imu_conf);
	(void)imu_status;
	t_imu_data *imu_data = imu_data_create(); /*setall to 0.0*/;
	(void)imu_data;

	while (1) {
		mux_loop(); // multiplexer read

		//imu loop;
		imu_get_raw_data(imu_conf, imu_data);
		// // copy the IMU values (float) into the packet sent to the atm2561
		// // imu_get_raw_data gives the register value without its sign (0..65535):
		// // (uint16_t) then (int16_t) gives back the signed value (-32768..32767)
		if (imu_data)
		{
			for (uint8_t i = 0; i < 3; i++)
			{
				g_sensor.accel[i] = (float)(int16_t)(uint16_t)imu_data->accel[i];
				g_sensor.gyro[i] = (float)(int16_t)(uint16_t)imu_data->gyro[i];
			}
		}

		// //note: we may need a timestamp or equivalent to estimate time passed between two measures?
		// //print content of imu_data to uart

		led_spi(); //led strip

		g_sensor.magic = SENSOR_PACKET_MAGIC_SENSOR_FROM_328; // magic number

		// //calculate crc value and stored in struct
		g_sensor.crc16 = crc16_compute((const uint8_t*)&g_sensor,
							offsetof(t_sensor_data, crc16));

		//send to atm2560 through uart0
		uart0_send_data((const uint8_t*)&g_sensor, sizeof(g_sensor));
		// uint8_t ptr_tmp = *(uint8_t*)&g_sensor;
		// UDR0 = *(uint8_t*)&g_sensor;

		// UCSR0B |= (1 << UDRIE0);

		_delay_ms(20);
	}
}
