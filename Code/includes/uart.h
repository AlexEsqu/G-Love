#ifndef UART_H
#define UART_H

#include <avr/io.h>

#define myUBBR (F_CPU / (8UL * BAUD) - 1UL) // Formula p.182

/* =======================
   UART MODE FLAGS
   ======================= */

#define UART_TX_ENABLE      (1 << TXEN0)
#define UART_RX_ENABLE      (1 << RXEN0)

#define UART_RX_INTERRUPT   (1 << RXCIE0)
#define UART_TX_INTERRUPT   (1 << UDRIE0)

/* =======================
   UART config structure (bitmask)
   ======================= */

#define UART_MODE_TX        UART_TX_ENABLE
#define UART_MODE_RX        UART_RX_ENABLE

#define UART_MODE_TX_RX     (UART_TX_ENABLE | UART_RX_ENABLE)

#define UART_MODE_RX_IT     (UART_RX_ENABLE | UART_RX_INTERRUPT)
#define UART_MODE_TX_IT     (UART_TX_ENABLE | UART_TX_INTERRUPT)

/* =======================
   UART UCSR0C CONFIG BITS
   ======================= */

/* Mode USART (UMSEL01:0) */
#define UART_MODE_ASYNC        (0 << UMSEL01) | (0 << UMSEL00)
#define UART_MODE_SPI          (1 << UMSEL01) | (0 << UMSEL00)

/* Parité (UPM01:0) */
#define UART_PARITY_DISABLED   (0 << UPM01) | (0 << UPM00)
#define UART_PARITY_EVEN       (1 << UPM01) | (0 << UPM00)
#define UART_PARITY_ODD        (1 << UPM01) | (1 << UPM00)

/* Stop bits (USBS0) */
#define UART_STOP_1BIT         (0 << USBS0)
#define UART_STOP_2BIT         (1 << USBS0)

/* Taille de caractère (UCSZ01:0 dans UCSR0C + UCSZ02 dans UCSR0B si besoin) */
#define UART_CHAR_SIZE_5BIT    (0 << UCSZ01) | (0 << UCSZ00)
#define UART_CHAR_SIZE_6BIT    (0 << UCSZ01) | (1 << UCSZ00)
#define UART_CHAR_SIZE_7BIT    (1 << UCSZ01) | (0 << UCSZ00)
#define UART_CHAR_SIZE_8BIT    (1 << UCSZ01) | (1 << UCSZ00)
#define UART_CHAR_SIZE_9BIT    (1 << UCSZ01) | (1 << UCSZ00)  // nécessite UCSZ02 dans UCSR0B

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

#endif