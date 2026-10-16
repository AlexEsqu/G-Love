#include <stdint.h>
#include "atm328p.h"
#include "../imu/icm42688p.h"
#include "../../includes/main.h"
#include "../muxSelect/mux.h"
#include "../ledStrip/led.h"

t_sensor_data g_sensor = {0};



void setup()
{
	i2c_init();
	// adc_init();
	// spi_init();
	uart0_init(BAUDRATE);
	// int imu_status =
	// mux_setup();
}

int main_ATM328P()
{
	setup();

  	uart0_printstr("\r\nstart of 328P loop:\r\n");
	t_imu *imu_conf = imu_conf_create();
	int imu_status = ICM42688_init(imu_conf);
	t_imu_data *imu_data = imu_data_create();/*setall to 0.0*/;
	while (1) {
		// mux_loop(); // multiplexer read
		i2c_scan();
		uart0_printstr("\r\nIMU Status: ");
		// uart0_print_int(imu_status);
		// uart0_printstr("\r\n");
		imu_get_raw_data(imu_conf, imu_data);
		uart0_printstr("\r\nIMU Data:\r\n");
		uart0_printstr("Accel: X=");
		uart0_print_float(imu_data->accel[0]);
		uart0_printstr(", Y=");
		uart0_print_float(imu_data->accel[1]);
		uart0_printstr(", Z=");
		uart0_print_float(imu_data->accel[2]);

		// imu_get_data(imu_conf, imu_data);
		// uart0_printstr("\r\nIMU Data:\r\n");
		// uart0_printstr("Accel: X=");
		// uart0_printstr_float(imu_data->accel[0]);
		// uart0_printstr(", Y=");
		// uart0_printstr_float(imu_data->accel[1]);
		// uart0_printstr(", Z=");
		// uart0_printstr_float(imu_data->accel[2]);
		//note: we may need a timestamp or equivalent to estimate time passed between two measures?
		//print content of imu_data to uart
		// imu_loop(); // todo

		// led_spi(); //led strip
/*
		g_sensor.magic = SENSOR_PACKET_MAGIC; // magic number

		//calculate crc value and stored in struct
		g_sensor.crc16 = crc16_compute((const uint8_t*)&g_sensor,
							offsetof(t_sensor_data, crc16));

		//send to atm2560 through uart0
		uart0_send_data((const uint8_t*)&g_sensor, sizeof(g_sensor));*/

		_delay_ms(20);
	}
}