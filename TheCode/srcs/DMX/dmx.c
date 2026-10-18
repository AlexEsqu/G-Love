#include "dmx.h"
#include "../../includes/main.h"

#define DMX_TX_BIT PE1
#define DMX_MUX_A PE2
#define DMX_MUX_B PE3

static void dmx_wait_frame_complete(void)
{
	while (!(UCSR0A & (1 << TXC0)))
	{
	}
}

static void dmx_tx_byte(uint8_t value)
{
	while (!(UCSR0A & (1 << UDRE0)))
	{}
	UDR0 = value;
}

static void dmx_break(void)
{
	UCSR0B &= ~((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0));
	DDRE |= (1 << DMX_TX_BIT);
	PORTE &= ~(1 << DMX_TX_BIT);
	_delay_us(100);
	PORTE |= (1 << DMX_TX_BIT);
	_delay_us(12);
	UCSR0A |= (1 << TXC0);
	UCSR0B = (1 << TXEN0);
}

void dmx_init(void)
{
	DDRE |= (1 << DMX_MUX_A) | (1 << DMX_MUX_B) | (1 << DMX_TX_BIT);
	PORTE &= ~(1 << DMX_MUX_A);
	PORTE |= (1 << DMX_MUX_B);

	UCSR0A = 0;
	UBRR0 = (uint16_t)((F_CPU / (16UL * DMX_BAUDRATE)) - 1UL);
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (1 << USBS0);
	UCSR0A |= (1 << TXC0);
}

void dmx_send_frame(uint8_t channel, uint8_t value)
{
	uint8_t status_register;

	if (channel == 0)
		return;

	status_register = SREG;
	SREG &= ~(1 << SREG_I);
	dmx_break();
	dmx_tx_byte(0x00);
	for (uint8_t slot = 1; slot < channel; slot++)
		dmx_tx_byte(0x00);
	dmx_tx_byte(value);
	dmx_wait_frame_complete();
	SREG = status_register;
}

void dmx_breathing_loop(void)
{
	static const uint8_t levels[] = {
		0, 2, 5, 9, 14, 20, 28, 37,
		48, 60, 74, 89, 106, 124, 143, 163,
		184, 163, 143, 124, 106, 89, 74, 60,
		48, 37, 28, 20, 14, 9, 5, 2
	};
	static uint8_t index = 0;

	dmx_send_frame(DMX_CHANNEL, levels[index]);
	index = (uint8_t)((index + 1) % (sizeof(levels) / sizeof(levels[0])));
	_delay_ms(35);
}
