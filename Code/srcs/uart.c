#include "G_Love.h"

void uart_init(void)
{
	UCSR0A = (1 << U2X0);														// Double speed mode (U2X0 = 1) is used to reduce baud rate error and improve communication accuracy, especially at high speeds like 115200 baud. (178–179/182)
	
	UBRR0 = (F_CPU / (8UL * BAUD)) - 1UL;

	UCSR0B = UART_MODE_TX_RX;									// Setting the Transmit Enable (TXEN) and Receive Enable (RXEN) bits in the UCSRnB Register.  (p185/188)
	UCSR0C = UART_CHAR_SIZE_8BIT | UART_MODE_ASYNC | UART_PARITY_DISABLED;		// The UMSELn bit in USART Control and Status Register C (UCSRnC) selects between asynchronous and synchronous operation (180/184/202) || (1 << UCSZ00) | (1 << UCSZ01) Data size 8 bits (p.203)
}

void uart_tx(char c)														// p186
{
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1 << UDRE0)))			// UDREn = Data Register Empty Flag in UCSRnA (USART Control and Status Register A) (p.187)
			;
	/* Put data into buffer, sends the data */
	UDR0 = c;									// UDRn = transmit buffer with data to send (p.179)
}

char uart_rx(void)
{
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)))				// RXCn = Receive Complete Flag in UCSRnA (USART Control and Status Register A) (p.187)
		;
	/* Get and return received data from buffer */
	return UDR0;								// UDRn = transmit buffer with data received (p.179)
}

void uart_printstr(const char* str)
{
	while (*str)
		uart_tx(*(str++));
}

void uart_printint(int nb)
{
	char str[11];
	uint8_t i = 0;
	uint8_t neg = 0;

	if (nb < 0) {
		neg = 1;
		nb *= -1;
	}
	while (nb > 9)
	{
		str[i++] = (nb % 10) + 48;
		nb /= 10;
	}
	str[i] = nb + 48;
	if (neg)
		uart_tx('-');
	for (; i > 0; i--)
		uart_tx(str[i]);
	uart_tx(str[i]);
	
}
/** USART Receive Complete interrupt service routine
 * Triggered when a new byte is received and stored in UDR0 (RXC0 flag set)*/
__attribute__((signal)) void USART_RX_vect(void)
{

}

/* USART Transmission Complete interrupt service routine
   Triggered when the transmit shift register and UDR0 are fully empty (TXC0 flag set) */
__attribute__((signal)) void USART_TX_vect(void)
{

}
