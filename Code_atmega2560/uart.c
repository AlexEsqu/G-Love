#include "main.h"

//UART0
void uart0_init(unsigned long baudrate) {
    // Double speed mode for reduced baud rate error at 115200 baud
    UCSR0A = (1 << U2X0);

    // Set Baud Rate Register
    UBRR0 = (F_CPU / (8UL * baudrate)) - 1UL;

    // Enable Transmitter and Receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    //8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart0_tx(char c) {
    // Wait for transmit empty
    while (!(UCSR0A & (1 << UDRE0)))
    {
    }
    UDR0 = c;
}

char uart0_rx(void) {
    // Wait for data
    while (!(UCSR0A & (1 << RXC0)))
    {
    }
    return UDR0;
}

void uart0_printstr(const char *str) {

	while (*str != '\0')
	{
		uart0_tx(*str);
		str++;
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

//UART1
void uart1_init(unsigned long baudrate) {
    // Double speed mode
    UCSR1A = (1 << U2X1);

    // Set Baud Rate Register
    UBRR1 = (F_CPU / (8UL * baudrate)) - 1UL;

    // Enable Transmitter and Receiver
    UCSR1B = (1 << TXEN1) | (1 << RXEN1);

    // 8 data bits, 1 stop bit, no parity
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

void uart1_tx(char c) {
    // Wait for transmit empty
    while (!(UCSR1A & (1 << UDRE1)))
    {
    }
    UDR1 = c;
}

char uart1_rx(void) {
    // Wait for data receive
    while (!(UCSR1A & (1 << RXC1)))
    {
    }
    return UDR1;
}

void uart1_printstr(const char *str) {

	while (*str != '\0')
	{
		uart0_tx(*str);
		str++;
	}

}

void uart1_print_10bit(uint32_t c) {
	if (c == 0)
		uart0_tx('0');
	if (c > 10) {
		uart0_print_10bit(c / 10);
	}
	uart0_tx('0' + (c % 10));
}

uint8_t uart1_available(void) {
	return (UCSR1A & (1 << RXC1));
}

uint8_t ft_hex(uint8_t val) {
	char hex[] = "0123456789ABCDEF";
	
	return hex[val];
}

void ft_uart_print_hex(uint8_t c) {
	uart0_tx(ft_hex(c / 16));
	uart0_tx(ft_hex(c % 16));
}
