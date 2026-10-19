#include "main.h"

#define RX_BUFFER_SIZE 128

static volatile uint8_t g_rx_buff[RX_BUFFER_SIZE];

//index used for writing in buffer
static volatile uint8_t g_rx_index = 0;

//pos for reading buffer
static volatile uint8_t g_rx_pos = 0;

// all possible exportable data, meant for BMX or Computer
t_sensor_data_to_pc	g_packet_to_export = {0};

// sensor data receivd from the 328P on the wrist
t_sensor_data		g_sensor_packet_from_wrist = {0};

// orders LEDs by giving mode and color
glove_directive_t	g_love_directive = {0};

__attribute__((signal, used))
void USART1_RX_vect(void)
{
	uint8_t data_byte = UDR1;
	uint8_t next_index = (g_rx_index + 1) % RX_BUFFER_SIZE;

	if (next_index != g_rx_pos)
	{
		g_rx_buff[g_rx_index] = data_byte;
		g_rx_index = next_index;
	}
}

bool	get_next_byte(uint8_t *byte)
{
	if (g_rx_index == g_rx_pos) // buf has nothing to read
	{
		return (false);
	}
	*byte = g_rx_buff[g_rx_pos];
	g_rx_pos = (g_rx_pos + 1) % RX_BUFFER_SIZE;
	return (true);
}

/*
	Check for magic number first,
	store data in buffer then check for crc match,
	return true if crc is correct and packet is copied into packed_to_pc
*/
bool process_packet(uint8_t *byte, t_sensor_data *packet)
{
	static uint8_t flag = 0;
	static uint8_t buf[sizeof(t_sensor_data)] = {0};
	static uint8_t index = 0;

	if (flag == 0)
	{
		if (*byte == (uint8_t)(SENSOR_PACKET_MAGIC_SENSOR_FROM_328 & 0xFF))
		{
			buf[0] = *byte;
			flag = 1;
		}
	}
	else if (flag == 1)
	{
		if (*byte == (uint8_t)((SENSOR_PACKET_MAGIC_SENSOR_FROM_328 >> 8) & 0xFF))
		{
			buf[1] = *byte;
			index = 2;
			flag = 2; // magic number found
		}
		else if (*byte == (uint8_t)(SENSOR_PACKET_MAGIC_SENSOR_FROM_328 & 0xFF))
		{
			buf[0] = *byte; // reset
		}
		else
		{
			flag = 0;
		}
	}
	else
	{
		buf[index++] = *byte;

		if (index == sizeof(t_sensor_data))
		{
			flag = 0;
			index = 0;

			uint16_t crc16_test = crc16_compute((const uint8_t*)buf,
									offsetof(t_sensor_data, crc16));
			uint16_t crc16_received = (uint16_t)buf[sizeof(t_sensor_data) - 2] |
									((uint16_t)buf[sizeof(t_sensor_data) - 1] << 8);

			if (crc16_test == crc16_received) //crc is validated
			{
				ft_memcpy(packet, buf, sizeof(t_sensor_data));
				return (true);
			}
		}
	}
	return (false);
}

void	update_sensor_data(uint8_t* byte)
{
	t_sensor_data	packet_from_sensors; //packet of data from atm328P

	if (process_packet(byte, &packet_from_sensors))
	{
		g_sensor_packet_from_wrist = packet_from_sensors;

		// copy received data into exportable data structure
		g_packet_to_export.magic = SENSOR_PACKET_MAGIC_ALL_DATA_FROM_2561;
		ft_memcpy(g_packet_to_export.sensorForce, packet_from_sensors.sensorForce, sizeof(packet_from_sensors.sensorForce));
		ft_memcpy(g_packet_to_export.sensorFlex, packet_from_sensors.sensorFlex, sizeof(packet_from_sensors.sensorFlex));
		ft_memcpy(g_packet_to_export.accel, packet_from_sensors.accel, sizeof(packet_from_sensors.accel));
		ft_memcpy(g_packet_to_export.gyro, packet_from_sensors.gyro, sizeof(packet_from_sensors.gyro));
		g_packet_to_export.crc16 = crc16_compute((const uint8_t *)&g_packet_to_export,
							offsetof(t_sensor_data_to_pc, crc16));

		uart0_send_data((const uint8_t *)&g_packet_to_export, sizeof(g_packet_to_export));
	}
}

void	package_g_love_directive(glove_directive_t* glove_directive)
{
	glove_directive->magic = SENSOR_PACKET_MAGIC_DIRECTIVE_TO_328;
	glove_directive->current_color = rgb565_to_rgb888(g_packet_to_export.last_touch.rgb565);
	glove_directive->current_mode = g_touchscreen_mode;
	glove_directive->crc16 = crc16_compute((const uint8_t *)glove_directive,
									offsetof(glove_directive_t, crc16));
}

void	send_g_love_directive()
{
	glove_directive_t glove_directive = {0};
	package_g_love_directive(&glove_directive);
	uart1_send_data((const uint8_t *)&glove_directive, sizeof(glove_directive_t));
}
