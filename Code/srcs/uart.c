#include "G_Love.h"

void uart_init(void)
{
	UCSR0A = (1 << U2X0);														// Double speed mode (U2X0 = 1) is used to reduce baud rate error and improve communication accuracy, especially at high speeds like 115200 baud. (178–179/182)
	
	UBRR0 = (F_CPU / (8UL * BAUDRATE)) - 1UL;

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
	if (nb == 0) 
	{
		uart_tx('0');
		return ;
	}

	unsigned int n;
	if (nb < 0)
	{
		uart_tx('-');
		n = (unsigned int)(- (long) nb);
	}
	else 
	{
		n = (unsigned int) nb;
	}

	char buf[12];
	int pos = 0;
	while (n > 0 && pos < (int)sizeof(buf)) {
		buf[pos++] = '0' + (n % 10);
		n /= 10;
	}

	for (int i = pos - 1; i >= 0; i--)
		uart_tx(buf[i]);
}

void uart_printhex(const int nb)
{
	const char hex[] = "0123456789ABCDEF";
	unsigned int n = (unsigned int) nb;
	char buf[9];
	int pos = 0;

	uart_printstr("0x");
	if (n == 0)
	{
		uart_tx('0');
		return;
	}

	while (n > 0 && pos < (int)sizeof(buf))
	{
		buf[pos++] = hex[n & 0xF]; // Get the last 4 bits (a hex digit)
		n >>= 4; // Shift right by 4 bits to process the next hex digit
	}

	for (int i = pos - 1; i >= 0; i--)
		uart_tx(buf[i]);
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
