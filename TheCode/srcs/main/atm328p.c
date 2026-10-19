#include <stdint.h>
#include "main.h"
#include "mux.h"
#include "led.h"
#include "../imu/icm42688p.h"

t_sensor_data g_sensor = {0};

uint8_t g_len_packet = sizeof(t_sensor_data);

// last directive received from the atm2561: touchscreen mode and color for the LEDs
glove_directive_t g_directive = {0};

/*
	Rebuild a glove_directive_t from the bytes received from the atm2561
	(stored in a buffer by the UART interrupt, taken out with uart0_read):
	  1. wait for the magic number, low byte first (0x5A then 0xA5)
	  2. collect the other bytes
	  3. check the crc, keep the directive only if it is correct
	Return true when a valid directive was copied into *directive.
*/
static bool read_directive(glove_directive_t *directive)
{
	static uint8_t	buf[sizeof(glove_directive_t)];
	static uint8_t	index = 0;
	const uint8_t	magic_low = SENSOR_PACKET_MAGIC_DIRECTIVE_TO_328 & 0xFF;
	const uint8_t	magic_high = SENSOR_PACKET_MAGIC_DIRECTIVE_TO_328 >> 8;
	uint8_t			byte;

	while (uart0_read(&byte))
	{
		if (index == 0)
		{
			if (byte == magic_low)
				buf[index++] = byte;
		}
		else if (index == 1)
		{
			if (byte == magic_high)
				buf[index++] = byte;
			else if (byte != magic_low) // 0x5A 0x5A: the second one may be the start
				index = 0;
		}
		else
		{
			buf[index++] = byte;
			if (index == sizeof(glove_directive_t))
			{
				uint16_t crc_received = (uint16_t)buf[sizeof(glove_directive_t) - 2]
					| ((uint16_t)buf[sizeof(glove_directive_t) - 1] << 8);

				index = 0;
				if (crc16_compute(buf, offsetof(glove_directive_t, crc16)) == crc_received)
				{
					ft_memcpy(directive, buf, sizeof(glove_directive_t));
					return (true);
				}
			}
		}
	}
	return (false);
}

// apply a directive received from the atm2561
static void process_directive(const glove_directive_t *directive, t_imu *imu_conf)
{
	// mode and color, kept for the LEDs
	g_directive = *directive;

	// IMU calibration asked on the IMU screen:
	// reset + configuration + gyro bias again (imu_reset alone would erase the configuration)
	if (directive->command == GLOVE_CMD_IMU_RESET)
		ICM42688_init(imu_conf);
}

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
		// orders from the atm2561 (mode, color, IMU reset)
		glove_directive_t directive;
		while (read_directive(&directive))
			process_directive(&directive, imu_conf);

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
