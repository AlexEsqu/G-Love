// Test UART0 de l'ATmega2561 - horloge interne 8 MHz
// Envoie un message chaque seconde + renvoie (echo) chaque caractère reçu.
// Côté PC : screen /dev/ttyUSB0 38400

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/wdt.h>          // à ajouter avec les autres include
#define BAUD 57600UL

#define UBRR_VAL ((F_CPU + BAUD * 4) / (BAUD * 8) - 1)   // mode U2X -> 25 (erreur 0,2 %)


static void uart_init(void) {
    UBRR0  = UBRR_VAL;
    UCSR0A = _BV(U2X0);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);   // 8N1
}

static void uart_putc(char c) {
    while (!(UCSR0A & _BV(UDRE0)));
    UDR0 = c;
}

static void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

static void uart_putu(uint16_t n) {
    char buf[6]; uint8_t i = 0;
    do { buf[i++] = '0' + n % 10; n /= 10; } while (n);
    while (i) uart_putc(buf[--i]);
}

int main(void) {
    MCUSR = 0;
    wdt_disable();

    uart_init();
    uart_puts("\r\n=== ATmega2561 UART0 OK ===\r\n");

    uint16_t compteur = 0;
    uint16_t ticks = 0;

    for (;;) {
        // Echo : ce que tu tapes revient entre crochets
        if (UCSR0A & _BV(RXC0)) {
            char c = UDR0;
            uart_putc('[');
            uart_putc(c);
            uart_putc(']');
            if (c == '\r') uart_puts("\r\n");
        }

        _delay_ms(1);
        if (++ticks >= 1000) {
            ticks = 0;
            uart_puts("tick ");
            uart_putu(compteur++);
            uart_puts("\r\n");
        }
    }
}