#include "main.h"
#include <avr/io.h>

/*
	INTERRUPT DRIVEN UART (non blocking)

	The main loop is often busy for a long time (screen over SPI, touch
	over I2C), so the UART must work in the background:

	TX: uartN_send_data() / uartN_tx() only copy the bytes into a ring
	    buffer and enable the "data register empty" interrupt (UDRIEn).
	    The interrupt sends the next byte each time the UART is ready,
	    and disables itself when the buffer is empty.

	RX: the "receive complete" interrupt (RXCIEn) stores each byte in a
	    ring buffer as soon as it arrives, uart0_read() takes them out.
	    (on the 2561, UART1 RX is handled in comms.c: packets from the
	    328P are decoded directly in the interrupt)

	Ring buffer: head = next free slot (written by the producer)
	             tail = next byte to read (written by the consumer)
	             empty when head == tail, full when head + 1 == tail
	Sizes are powers of 2, so "& (SIZE - 1)" wraps the index around.
*/

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
	// the 328P has only one UART, named without number in avr/io.h
	# define UART0_RX_VECTOR	USART_RX_vect
	# define UART0_UDRE_VECTOR	USART_UDRE_vect
	# define UART0_TX_SIZE		64		// one packet to the 2561
#else
	# define UART0_RX_VECTOR	USART0_RX_vect
	# define UART0_UDRE_VECTOR	USART0_UDRE_vect
	# define UART0_TX_SIZE		128		// two packets to the PC
#endif
#define UART0_RX_SIZE			32
#define UART1_TX_SIZE			32		// directives to the 328P

typedef struct s_ring
{
	volatile uint8_t	*buf;
	uint8_t				mask;	// size - 1
	volatile uint8_t	head;
	volatile uint8_t	tail;
}	t_ring;

static volatile uint8_t	g_uart0_tx_buf[UART0_TX_SIZE];
static volatile uint8_t	g_uart0_rx_buf[UART0_RX_SIZE];
static t_ring			g_uart0_tx = {g_uart0_tx_buf, UART0_TX_SIZE - 1, 0, 0};
static t_ring			g_uart0_rx = {g_uart0_rx_buf, UART0_RX_SIZE - 1, 0, 0};

// free slots in the buffer (one slot always stays empty: full != empty)
static uint8_t ring_free(const t_ring *ring)
{
	return (ring->mask - ((uint8_t)(ring->head - ring->tail) & ring->mask));
}

static uint8_t ring_used(const t_ring *ring)
{
	return ((uint8_t)(ring->head - ring->tail) & ring->mask);
}

// must be called with interrupts disabled or from the only producer
static void ring_push(t_ring *ring, uint8_t byte)
{
	ring->buf[ring->head] = byte;
	ring->head = (ring->head + 1) & ring->mask;
}

static bool ring_pop(t_ring *ring, uint8_t *byte)
{
	if (ring->head == ring->tail)
		return (false);
	*byte = ring->buf[ring->tail];
	ring->tail = (ring->tail + 1) & ring->mask;
	return (true);
}

/*
	Queue len bytes, all or nothing: if there is not enough room, nothing
	is queued and false is returned (a half packet would be lost anyway).
	Interrupts are disabled while copying: the main loop and an interrupt
	(comms.c) can both send on the same UART.
*/
static bool ring_send(t_ring *ring, volatile uint8_t *ucsrb, uint8_t udrie,
						const uint8_t *data, size_t len)
{
	uint8_t	sreg_backup;
	bool	ok = false;

	if (!data || len == 0)
		return (false);

	sreg_backup = SREG;
	SREG &= ~(1 << SREG_I);
	if (len <= ring_free(ring))
	{
		for (size_t i = 0; i < len; i++)
			ring_push(ring, data[i]);
		// start sending in the background
		*ucsrb |= (1 << udrie);
		ok = true;
	}
	SREG = sreg_backup;
	return (ok);
}

/*
	One character (text, debug): waits only while the buffer is full.
	If interrupts are disabled (called from an interrupt), the buffer can
	not empty itself: the character is dropped instead of freezing the MCU.
*/
static void ring_send_char(t_ring *ring, volatile uint8_t *ucsrb, uint8_t udrie, char c)
{
	while (!ring_send(ring, ucsrb, udrie, (const uint8_t *)&c, 1))
	{
		if (!(SREG & (1 << SREG_I)))
			return;
	}
}

//UART0
void uart0_init()
{
    // Set Baud Rate Register
    UBRR0 = MYUBRR;

    // Double speed mode for reduced baud rate error
    UCSR0A = (1 << U2X0);

    // Enable Transmitter, Receiver and the receive interrupt
    // (the send interrupt UDRIE0 is enabled only when there is data to send)
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

    // 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// UART0 ready for the next byte
__attribute__((signal, used))
void UART0_UDRE_VECTOR(void)
{
	uint8_t byte;

	if (ring_pop(&g_uart0_tx, &byte))
		UDR0 = byte;
	else
		UCSR0B &= ~(1 << UDRIE0); // nothing left: stop this interrupt
}

// UART0 received a byte
__attribute__((signal, used))
void UART0_RX_VECTOR(void)
{
	uint8_t byte = UDR0; // reading UDR0 clears the interrupt

	if (ring_free(&g_uart0_rx))
		ring_push(&g_uart0_rx, byte);
}

void uart0_tx(char c)
{
	ring_send_char(&g_uart0_tx, &UCSR0B, UDRIE0, c);
}

bool uart0_send_data(const uint8_t *data, size_t len)
{
	return (ring_send(&g_uart0_tx, &UCSR0B, UDRIE0, data, len));
}

// number of received bytes waiting to be read
uint8_t uart0_available(void)
{
	return (ring_used(&g_uart0_rx));
}

// take one received byte, false if there is none (never waits)
bool uart0_read(uint8_t *byte)
{
	return (ring_pop(&g_uart0_rx, byte));
}

void uart0_printstr(const char *str)
{
    if (!str)
        return;

    while (*str)
        uart0_tx(*str++);
}

void uart0_print_int(int32_t num) {
	if (num < 0) {
		uart0_tx('-');
		num = -num;
	}
	uart0_print_10bit(num);
}

void uart0_print_float(float num) {
	if (num < 0) {
		uart0_tx('-');
		num = -num;
	}
	int32_t intPart = (int32_t)num;
	float fracPart = num - intPart;

	uart0_print_10bit(intPart);
	uart0_tx('.');

	// Print fractional part with 3 decimal places
	for (int i = 0; i < 3; i++) {
		fracPart *= 10;
		int digit = (int)fracPart;
		uart0_tx('0' + digit);
		fracPart -= digit;
	}
}

void uart0_print_10bit(uint32_t c) {
	if (c == 0)
		uart0_tx('0');
	if (c > 10) {
		uart0_print_10bit(c / 10);
	}
	uart0_tx('0' + (c % 10));
}

void uart_init_test(void)
{
	uint8_t c;

	uart0_init();
	SREG |= (1 << SREG_I); // the UART works with interrupts

	i2c_init();
	i2c_scan();

	while(1)
	{
		if (uart0_read(&c))
		{
			uart0_tx('[');
			uart0_tx(c);
			uart0_tx(']');
			if (c == '\r')
				uart0_printstr("\r\n");
			if (c == 's')
				i2c_scan();
		}
	}
}

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)

static volatile uint8_t	g_uart1_tx_buf[UART1_TX_SIZE];
static t_ring			g_uart1_tx = {g_uart1_tx_buf, UART1_TX_SIZE - 1, 0, 0};

// UART1
void uart1_init()
{
    // Double speed mode
    UCSR1A = (1 << U2X1);

    // Set Baud Rate Register
    UBRR1 = MYUBRR;

    // Enable Transmitter, Receiver and the receive interrupt (USART1_RX_vect in comms.c)
    UCSR1B = (1 << RXCIE1) | (1 << TXEN1) | (1 << RXEN1);

    // 8 data bits, 1 stop bit, no parity
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

// UART1 ready for the next byte
__attribute__((signal, used))
void USART1_UDRE_vect(void)
{
	uint8_t byte;

	if (ring_pop(&g_uart1_tx, &byte))
		UDR1 = byte;
	else
		UCSR1B &= ~(1 << UDRIE1);
}

void uart1_tx(char c)
{
	ring_send_char(&g_uart1_tx, &UCSR1B, UDRIE1, c);
}

bool uart1_send_data(const uint8_t *data, size_t len)
{
	return (ring_send(&g_uart1_tx, &UCSR1B, UDRIE1, data, len));
}

void uart1_printstr(const char *str) {

	while (*str != '\0')
	{
		uart1_tx(*str);
		str++;
	}
}

void uart1_print_10bit(uint32_t c) {
	if (c == 0)
		uart1_tx('0');
	if (c > 10) {
		uart1_print_10bit(c / 10);
	}
	uart1_tx('0' + (c % 10));
}
#endif

uint8_t ft_hex(uint8_t val)
{
    char hex[] = "0123456789ABCDEF";

    return hex[val];
}

void ft_uart_print_hex(uint8_t c)
{
    uart0_tx(ft_hex(c / 16));
    uart0_tx(ft_hex(c % 16));
}
