#include "main.h"
#include <avr/io.h>

volatile char to_send[sizeof(t_sensor_data)];
volatile uint8_t tx_index = 0;
volatile uint8_t tx_busy = 0; // Protège le tampon pendant une émission

// // Interruption de Réception (RX) - Vecteur 18 (USART_RX_vect)
// __attribute__((signal, used)) void __vector_18(void)
// {   
//     // On lit UDR0 TOUT DE SUITE pour vider le matériel et éviter un débordement
//     uint8_t received_char = UDR0; 

//     // On ne traite la donnée que si l'envoi précédent est totalement achevé
//     if (!tx_busy) 
//     {
//         to_send[0] = '[';
//         to_send[1] = received_char;
//         to_send[2] = ']';

//         tx_busy = 1;
//         tx_index = 0; // On commence par l'index 0 ('[')

//         // CRUCIAL : On active l'interruption "Registre de données vide" (UDRIE0)
//         // Cela va forcer le microcontrôleur à sauter immédiatement dans le __vector_20
//         UCSR0B |= (1 << UDRIE0);
//     }
// }

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
// Interruption de Registre de Données Vide - Vecteur 19 (USART_UDRE_vect sur atmega328p)
__attribute__((signal, used)) void __vector_19(void)
{   
    if (tx_index < g_len_packet)
    {
        // Écriture DIRECTE et instantanée dans le registre matériel (pas de fonction bloquante !)
        UDR0 = ((const uint8_t *)&g_sensor)[tx_index];
        tx_index++;
    }
    else
    {
        // Une fois les caractères chargés, on COUPE cette interruption.
        UCSR0B &= ~(1 << UDRIE0);
        
        // On libère le tampon pour la prochaine réception
        tx_busy = 0;
        tx_index = 0;
    }
}
#endif

void uart_init_test(void)
{
    UBRR0  = MYUBRR;
    UCSR0A |= (1 << U2X0);
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

    i2c_init();
    i2c_scan();

	while(1)
	{
		if (UCSR0A & (1 << RXC0)) {
			char c = UDR0;
			uart0_tx('[');
			uart0_tx(c);
			uart0_tx(']');
			if (c == '\r')
				uart0_printstr("\r\n");
			if (c == 's')
				i2c_scan();
		}
		_delay_ms(1);
	}
}

//UART0
void uart0_init() {
    // Set Baud Rate Register
    UBRR0 = MYUBRR;

    // Double speed mode for reduced baud rate error at 115200 baud
    UCSR0A = (1 << U2X0);

    // Enable Transmitter and Receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    // Receive interrupt only on the 328P
    // On the 2561 there is no USART0_RX_vect
    #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
    UCSR0B |= (1 << RXCIE0);
    #endif

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

// char uart0_rx(void)
// {
//     // Wait for data
//     // while (!(UCSR0A & (1 << RXC0)))
//     // {
//     // }
//     return UDR0;
// }

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

void uart0_send_data(const uint8_t *data, size_t len)
{
    if (!data || len == 0)
        return;

    while (len--)
    {
        uart0_tx(*data++);
    }
}

uint8_t uart0_available(void) {
	return (UCSR0A & (1 << RXC0));
}

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)

// UART1
void uart1_init()
{
    // Double speed mode
    UCSR1A = (1 << U2X1);

    // Set Baud Rate Register
    UBRR1 = MYUBRR;

    // Enable Transmitter and Receiver
    UCSR1B = (1 << RXCIE1) | (1 << TXEN1) | (1 << RXEN1);

    // 8 data bits, 1 stop bit, no parity
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
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

void uart1_send_data(const uint8_t *data, size_t len)
{
    if (!data || len == 0)
        return;

    while (len--)
    {
        uart1_tx(*data++);
    }
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

// /** USART Receive Complete interrupt service routine
//  * Triggered when a new byte is received and stored in UDR0 (RXC0 flag set)*/
// #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
// __attribute__ ((signal, __used__, externally_visible)) void USART_RX_vect(void)
// {
//     // Read the received byte from UDR0 to clear the RXC0 flag
//     char recu = UDR0; // Read the received byte from UDR0 to clear the RXC0 flag

//     // Echo the received byte back to the sender
//     uart0_tx(recu);

//     led_effect(recu - 48, 0); // Trigger LED effect on THUMB when a byte is received
// }
// #endif
