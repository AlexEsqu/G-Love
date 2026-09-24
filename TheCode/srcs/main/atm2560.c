#include "atm2560.h"
#include "../../includes/main.h"
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

t_sensor_data_to_pc g_packet = {0};

static volatile uint8_t g_rx_buff[RX_BUFFER_SIZE];
static volatile uint8_t g_rx_index = 0; //index used for writing in buffer
static volatile uint8_t g_rx_pos = 0; //pos for reading buffer

ISR(USART1_RX_vect)
{
	uint8_t data_byte = UDR1;
	uint8_t next_index = (g_rx_index + 1) % RX_BUFFER_SIZE;

	g_rx_buff[g_rx_index] = data_byte;
	g_rx_index = next_index;
	
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
		if (*byte == SENSOR_PACKET_MAGIC)
		{
			index = 1;
			flag = 1; //magic number detected = start process packet
		}
	}
	else
	{
		buf[index] = *byte;
		index++;

		if (index == sizeof(t_sensor_data))
		{
			uint16_t crc16_test = crc16_compute((uint8_t*)&buf,
									sizeof(t_sensor_data) - 2);
			uint16_t crc16_received = buf[sizeof(t_sensor_data) - 2];

			if (crc16_test == crc16_received) //crc is validated
			{
				flag = 0;
				memcpy(&g_packet, packet, sizeof(*packet)); 
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

	sei();

	// pin 7 en input
	DDRD &= ~(1 << DDD7);
	// pullup
	PORTD |= (1 << PORTD7);
}

int main_ATM2560()
{
	setup();

	uint8_t buf[100];
	int i = 0;

	uint8_t byte;
	t_sensor_data packet; //packet from atm328P
	t_sensor_data_to_pc packet_to_pc; //packet to send to pc

	while (1)
	{
		// uart0_printstr("test\n");
		if (PIND & (1 << PIND7))
		{

			i2c_start();
			i2c_write(I2C_ADDR_CST816D << 1 | 0);
			i2c_write(0x01);
			// i2c_read_ack();

			i2c_start();
			i2c_write((I2C_ADDR_CST816D << 1) | 1);

			uint8_t action = i2c_read_ack();
			uint8_t points = i2c_read_ack();
			uint8_t x_high = i2c_read_ack();
			uint8_t x_low = i2c_read_ack();
			uint8_t y_high = i2c_read_ack();
			uint8_t y_low = i2c_read_nack();

			i2c_stop();

			int x = ((x_high & 0x0F) << 8) | x_low;
			int y = ((y_high & 0x0F) << 8) | y_low;

			if (points > 0)
			{
				uart0_printstr("action: ");
				uart0_print_10bit(action);
				if (action == 1)
					uart0_printstr("swipe up\n");
				else if (action == 2)
					uart0_printstr("swipe down\n");
				else if (action == 3)
					uart0_printstr("swipe right\n");
				else if (action == 4)
					uart0_printstr("swipe left\n");
				else if (action == 0)
					uart0_printstr("tap\n");
				else
					uart0_printstr("autre action\n");
			uart0_printstr("Toucher detecte ! X: ");
			uart0_print_10bit(x);
			uart0_printstr(" | Y: ");
			uart0_print_10bit(y);
			uart0_printstr("\n");
			}
		}

		while (get_next_byte(&byte))
		{
			if (process_packet(&byte, &packet))
			{
				//complete packet with touch, crc and send
			}
		}

/*
		if (uart1_available() > 0)
		{
		char c = uart1_rx();
		if (c == '\n') {
			buf[i] = '\0';
			uart0_printstr(buf);
			uart0_printstr("\n");
			i = 0;
		}
		else
		{
			buf[i] = c;
			i++;
		}
		}
*/
	}
	return 0;
}