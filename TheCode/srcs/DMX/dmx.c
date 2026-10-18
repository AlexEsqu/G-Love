#include "dmx.h"
#include "../../includes/main.h"
#include <avr/interrupt.h>

#define DMX_TX_BIT PE1
#define DMX_MUX_A PE2
#define DMX_MUX_B PE3

enum dmx_state
{
	DMX_IDLE,
	DMX_BREAK,
	DMX_MAB,
	DMX_START,
	DMX_SLOTS,
	DMX_DATA
};

static volatile uint8_t g_dmx_state = DMX_IDLE;
static volatile uint8_t g_dmx_channel;
static volatile uint8_t g_dmx_value;
static volatile uint8_t g_dmx_slot;
static volatile uint8_t g_dmx_cooldown_ms;

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

	/* Timer1 runs at 1 MHz: it times the DMX break/MAB and the 35 ms pause. */
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS11);
	OCR1A = 999;
	TIMSK1 |= (1 << OCIE1A);
	sei();
}

uint8_t dmx_send_frame(uint8_t channel, uint8_t value)
{
	uint8_t status_register = SREG;

	if (channel == 0)
		return 0;

	SREG &= ~(1 << SREG_I);
	if (g_dmx_state != DMX_IDLE || g_dmx_cooldown_ms != 0)
	{
		SREG = status_register;
		return 0;
	}

	g_dmx_channel = channel;
	g_dmx_value = value;
	g_dmx_slot = 1;
	g_dmx_state = DMX_BREAK;
	DDRE |= (1 << DMX_TX_BIT);
	PORTE &= ~(1 << DMX_TX_BIT);
	TCNT1 = 0;
	OCR1A = 99;
	SREG = status_register;
	return 1;
}

// Timer/Counter1 Capture Event
void __vector_17(void) __attribute__((signal, used, externally_visible));
void __vector_17(void)
{
	if (g_dmx_state == DMX_BREAK)
	{
		PORTE |= (1 << DMX_TX_BIT);
		g_dmx_state = DMX_MAB;
		TCNT1 = 0;
		OCR1A = 11;
	}
	else if (g_dmx_state == DMX_MAB)
	{
		UCSR0A |= (1 << TXC0);
		UCSR0B |= (1 << TXCIE0) | (1 << TXEN0);
		UDR0 = 0;
		g_dmx_state = DMX_START;
		TCNT1 = 0;
		OCR1A = 999;
	}
	else if (g_dmx_cooldown_ms != 0)
	{
		--g_dmx_cooldown_ms;
	}
}

// USART0 Data Register Empty
void __vector_27(void) __attribute__((signal, used, externally_visible));
void __vector_27(void)
{
	if (g_dmx_state == DMX_START || g_dmx_state == DMX_SLOTS)
	{
		if (g_dmx_slot < g_dmx_channel)
		{
			UDR0 = 0;
			++g_dmx_slot;
			g_dmx_state = DMX_SLOTS;
		}
		else
		{
			UDR0 = g_dmx_value;
			g_dmx_state = DMX_DATA;
		}
	}
	else if (g_dmx_state == DMX_DATA)
	{
		UCSR0B &= ~(1 << TXCIE0);
		g_dmx_state = DMX_IDLE;
		g_dmx_cooldown_ms = 35;
	}
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

	if (dmx_send_frame(DMX_CHANNEL, levels[index]))
		index = (uint8_t)((index + 1) % (sizeof(levels) / sizeof(levels[0])));
}
