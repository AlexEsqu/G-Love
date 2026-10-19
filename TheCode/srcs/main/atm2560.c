#include "main.h"
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

static volatile uint8_t g_rx_buff[RX_BUFFER_SIZE];
static volatile uint8_t g_rx_index = 0; //index used for writing in buffer
static volatile uint8_t g_rx_pos = 0; //pos for reading buffer

t_sensor_data_to_pc	g_packet_to_export = {0};
t_sensor_data		g_sensor_packet_from_wrist = {0};
tft_state_t			g_touchscreen_mode = TFT_STATE_HOMESCREEN;
uint8_t				g_animation_frame = 0;
tft_touch_info_t	g_last_touch = {0};

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

bool get_next_byte(uint8_t *byte)
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
		if (*byte == (uint8_t)(SENSOR_PACKET_MAGIC_FROM_328 & 0xFF))
		{
			buf[0] = *byte;
			flag = 1;
		}
	}
	else if (flag == 1)
	{
		if (*byte == (uint8_t)((SENSOR_PACKET_MAGIC_FROM_328 >> 8) & 0xFF))
		{
			buf[1] = *byte;
			index = 2;
			flag = 2; // magic number found
		}
		else if (*byte == (uint8_t)(SENSOR_PACKET_MAGIC_FROM_328 & 0xFF))
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

void setup()
{
	// i2c init
	i2c_init();

	// uart_init
	uart0_init(BAUDRATE);
	uart1_init(BAUDRATE);

	spi_init();
	cst816d_init();
	gc9a01_init();
	touch_timer_init();
	enableTouchInterrupt();

	// pin 7 en input
	DDRD &= ~(1 << DDD7);
	// pullup
	PORTD |= (1 << PORTD7);

	gc9a01_fill_screen(RGB16_BLUE);
	display_screen();
}

int main_ATM2560()
{
	setup();

	uint8_t			byte;
	t_sensor_data	packet_from_sensors; //packet of data from atm328P

	while (1)
	{
		if (g_last_touch.is_pending)
		{
			g_last_touch.is_pending = 0;
			onTouch();
		}
		refresh_screen();

		while (get_next_byte(&byte))
		{
			if (process_packet(&byte, &packet_from_sensors))
			{
				g_sensor_packet_from_wrist = packet_from_sensors;

				// copy received data into exportable data structure
				g_packet_to_export.magic = SENSOR_PACKET_MAGIC_FROM_2561;
				ft_memcpy(g_packet_to_export.sensorForce, packet_from_sensors.sensorForce, sizeof(packet_from_sensors.sensorForce));
				ft_memcpy(g_packet_to_export.sensorFlex, packet_from_sensors.sensorFlex, sizeof(packet_from_sensors.sensorFlex));
				ft_memcpy(g_packet_to_export.accel, packet_from_sensors.accel, sizeof(packet_from_sensors.accel));
				ft_memcpy(g_packet_to_export.gyro, packet_from_sensors.gyro, sizeof(packet_from_sensors.gyro));

				//complete packet from 328 with touch, crc
				g_packet_to_export.screen_x = g_last_touch.x;
				g_packet_to_export.screen_y = g_last_touch.y;
				g_packet_to_export.screen_action = g_last_touch.last_action;
				g_packet_to_export.crc16 = crc16_compute((const uint8_t *)&g_packet_to_export,
									offsetof(t_sensor_data_to_pc, crc16));

				uart0_send_data((const uint8_t *)&g_packet_to_export, sizeof(g_packet_to_export));
			}
		}
	}
	return 0;
}
