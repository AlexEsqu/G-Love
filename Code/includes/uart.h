#ifndef UART_H
#define UART_H

#include <avr/io.h>

#define myUBBR (F_CPU / (8UL * BAUDRATE) - 1UL) // Formula p.182

/* UART mode and config flags grouped as enums for clarity */
enum uart_mode_flags {
    UART_TX_ENABLE = (1 << TXEN0),
    UART_RX_ENABLE = (1 << RXEN0),

    UART_RX_INTERRUPT = (1 << RXCIE0),
    UART_TX_INTERRUPT = (1 << UDRIE0),

    UART_MODE_TX = UART_TX_ENABLE,
    UART_MODE_RX = UART_RX_ENABLE,
    UART_MODE_TX_RX = (UART_TX_ENABLE | UART_RX_ENABLE),
    UART_MODE_RX_IT = (UART_RX_ENABLE | UART_RX_INTERRUPT),
    UART_MODE_TX_IT = (UART_TX_ENABLE | UART_TX_INTERRUPT)
};

enum uart_ucsrc_bits {
    UART_MODE_ASYNC = ((0 << UMSEL01) | (0 << UMSEL00)),
    UART_PARITY_DISABLED = ((0 << UPM01) | (0 << UPM00)),
    UART_PARITY_EVEN = ((1 << UPM01) | (0 << UPM00)),
    UART_PARITY_ODD = ((1 << UPM01) | (1 << UPM00)),
    UART_STOP_1BIT = (0 << USBS0),
    UART_STOP_2BIT = (1 << USBS0),
    UART_CHAR_SIZE_5BIT = ((0 << UCSZ01) | (0 << UCSZ00)),
    UART_CHAR_SIZE_6BIT = ((0 << UCSZ01) | (1 << UCSZ00)),
    UART_CHAR_SIZE_7BIT = ((1 << UCSZ01) | (0 << UCSZ00)),
    UART_CHAR_SIZE_8BIT = ((1 << UCSZ01) | (1 << UCSZ00)),
    UART_CHAR_SIZE_9BIT = ((1 << UCSZ01) | (1 << UCSZ00))
};

/* =======================
   UART Functions
   ======================= */

void uart_init(void);
void uart_tx(char c);
char uart_rx(void);
__attribute__((signal)) void USART_TX_vect(void);
__attribute__((signal)) void USART_RX_vect(void);
void uart_printstr(const char* str);
void uart_printint(const int nb);
void uart_printhex(const int nb);

#endif