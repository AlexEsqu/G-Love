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

static volatile uint8_t g_dmx_state = DMX_IDLE; /* Current state of the DMX interface */
static volatile uint8_t g_dmx_values[DMX_MAX_CHANNELS + 1]; /* The values of the DMX channels */
static volatile uint16_t g_dmx_last_slot; /* The last channel that has a value set. */
static volatile uint16_t g_dmx_slot; /* The current channel being transmitted. */
static volatile uint8_t g_dmx_cooldown_ms; /* The cooldown time in milliseconds. */

void dmx_init(void)
{
	/* Initialize the DMX interface by selecting the correct multiplexer inputs. */
	DDRE |= (1 << DMX_MUX_A) | (1 << DMX_MUX_B) | (1 << DMX_TX_BIT); 
	PORTE &= ~(1 << DMX_MUX_A); 
	PORTE |= (1 << DMX_MUX_B);

	/* Initialize the DMX USART for 250 kbps operation. (DMX standard) */
	UCSR0A = 0;
	UBRR0 = (uint16_t)((F_CPU / (16UL * DMX_BAUDRATE)) - 1UL);
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00) | (1 << USBS0);
	UCSR0A |= (1 << TXC0);

	/* Enable the USART0 Data Register Empty interrupt. */
	UCSR0B |= (1 << UDRIE0);
	/* Timer1 runs at 1 MHz: it times the DMX break/MAB and the 35 ms pause. */
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS11);
	OCR1A = 999;
	TIMSK1 |= (1 << OCIE1A);
	SREG |= (1 << SREG_I);
}

void dmx_disable(void)
{
	uint8_t status_register;
	status_register = SREG;

	SREG &= ~(1 << SREG_I);

	for (uint16_t channel = 0; channel <= DMX_MAX_CHANNELS; channel++)
		g_dmx_values[channel] = 0;

	if (g_dmx_last_slot != 0)
	{
		/* Keep interrupts enabled while waiting for the USART and timer ISRs. */
		SREG |= (1 << SREG_I);
		while (!dmx_is_idle())
			;

		/* Wait for the inter-frame cooldown, then send the blackout frame. */
		while (!dmx_send_frame())
			;
		while (!dmx_is_idle())
			;
	}

	SREG &= ~(1 << SREG_I);

	g_dmx_state = DMX_IDLE;
	g_dmx_last_slot = 0;
	g_dmx_slot = 0;
	g_dmx_cooldown_ms = 0;

	UCSR0B &= ~((1 << UDRIE0) | (1 << TXCIE0) | (1 << TXEN0));
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;
	TIMSK1 &= ~(1 << OCIE1A);

	PORTE &= ~(1 << DMX_TX_BIT);
	SREG = status_register;
}

uint8_t dmx_is_idle(void)
{
	uint8_t status_register = SREG;
	uint8_t is_idle;

	SREG &= ~(1 << SREG_I);
	is_idle = (g_dmx_state == DMX_IDLE);
	SREG = status_register;
	return is_idle;
}

/* Set the value of a DMX channel. */
uint8_t dmx_set_channel(uint16_t channel, uint8_t value)
{
	if (channel == 0 || channel > DMX_MAX_CHANNELS)
		return 0;

	uint8_t status_register;
	status_register = SREG;
	SREG &= ~(1 << SREG_I);
	g_dmx_values[channel] = value;
	if (channel > g_dmx_last_slot)
		g_dmx_last_slot = channel;
	SREG = status_register;
	return 1;
}

/* Send a DMX frame containing the current values of all channels. */
uint8_t dmx_send_frame(void)
{
	/* Save the current status register. */
	uint8_t status_register;
	status_register = SREG;

	/* Disable interrupts for the duration of the operation. */
	SREG &= ~(1 << SREG_I);

	if (g_dmx_last_slot == 0 || g_dmx_state != DMX_IDLE || g_dmx_cooldown_ms != 0)
	{
		SREG = status_register;
		return 0;
	}

	/* Start one frame containing all channels updated so far. */
	g_dmx_slot = 1;
	g_dmx_state = DMX_BREAK;
	DDRE |= (1 << DMX_TX_BIT);
	PORTE &= ~(1 << DMX_TX_BIT);
	TCNT1 = 0;
	OCR1A = 99;
	SREG = status_register;
	return 1;
}

/* Perform a breathing loop with the DMX channels. */
void dmx_breathing_loop(void)
{
	static const uint8_t levels[] = {
		0, 2, 5, 9, 14, 20, 28, 37,
		48, 60, 74, 89, 106, 124, 143, 163,
		184, 163, 143, 124, 106, 89, 74, 60,
		48, 37, 28, 20, 14, 9, 5, 2
	};
	static uint8_t index = 0;

	dmx_set_channel(DMX_CHANNEL, levels[index]);
	if (dmx_send_frame())
		index = (uint8_t)((index + 1) % (sizeof(levels) / sizeof(levels[0])));
}

// Timer/Counter1 Capture Event for DMX break and MAB timing, as well as the 35 ms pause between frames.
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
		UDR0 = g_dmx_values[g_dmx_slot];
		if (g_dmx_slot < g_dmx_last_slot)
		{
			++g_dmx_slot;
			g_dmx_state = DMX_SLOTS;
		}
		else
		{
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
