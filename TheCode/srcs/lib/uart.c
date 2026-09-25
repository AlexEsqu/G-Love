#include "../../includes/main.h"
#include <avr/interrupt.h>

// UART0
void uart0_init(unsigned long baudrate)
{
    // Double speed mode for reduced baud rate error at 115200 baud
    UCSR0A = (1 << U2X0);

    // Set Baud Rate Register
    UBRR0 = (F_CPU / (8UL * baudrate)) - 1UL;

    // Enable Transmitter and Receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);

    // 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart0_tx(char c)
{
    // Wait for transmit empty
    while (!(UCSR0A & (1 << UDRE0)))
    {
    }
    UDR0 = c;
}

char uart0_rx(void)
{
    // Wait for data
    while (!(UCSR0A & (1 << RXC0)))
    {
    }
    return UDR0;
}

void uart0_printstr(const char *str)
{
    if (!str)
        return;

    while (*str)
        uart0_tx(*str++);
}

void uart0_print_10bit(uint32_t c)
{
    if (c == 0)
        uart0_tx('0');
    if (c > 10)
    {
        uart0_print_10bit(c / 10);
    }
    uart0_tx('0' + (c % 10));
}

void uart0_send_data(const uint8_t *data, size_t len)
{
    if (!data || len == 0)
        return;

    while (len--)
    {
        uart0_tx(*data++);
    }
}

#ifdef __AVR_ATmega2560__

// UART1
void uart1_init(unsigned long baudrate)
{
    UCSR0A = 0;
    UBRR0H = 0;
    UBRR0L = 1;

    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart1_tx(char c)
{
    // Wait for transmit empty
    while (!(UCSR1A & (1 << UDRE1)))
    {
    }
    UDR1 = c;
}

char uart1_rx(void)
{
    // Wait for data receive
    while (!(UCSR1A & (1 << RXC1)))
    {
    }
    return UDR1;
}

void uart1_printstr(const char *str)
{

    while (*str != '\0')
    {
        uart0_tx(*str);
        str++;
    }
}

void uart1_print_10bit(uint32_t c)
{
    if (c == 0)
        uart0_tx('0');
    if (c > 10)
    {
        uart0_print_10bit(c / 10);
    }
    uart0_tx('0' + (c % 10));
}

uint8_t uart1_available(void)
{
    return (UCSR1A & (1 << RXC1));
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

/** USART Receive Complete interrupt service routine
 * Triggered when a new byte is received and stored in UDR0 (RXC0 flag set)*/
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
ISR(USART_RX_vect)
{
    // Read the received byte from UDR0 to clear the RXC0 flag
    char recu = UDR0; // Read the received byte from UDR0 to clear the RXC0 flag
    
    // Echo the received byte back to the sender
    uart0_tx(recu);

    switch (recu)
    {
    case 'a':
        trigger_pulse(THUMB, 255, 0, 0, 100, 10);
        break;
    case 'b':
        trigger_pulse(INDEX, 0, 255, 0, 100, 10);
        break;
    case 'c':
        trigger_pulse(MIDDLE, 0, 0, 255, 100, 10);
        break;
    case 'd':
        trigger_pulse(RING, 255, 255, 0, 100, 10);
        break;
    case 'e':
        trigger_pulse(PINKY, 255, 0, 255, 100, 10);
        break;
    default:
        break;
    }
}
#endif