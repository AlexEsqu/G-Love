
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/wdt.h>

#define BAUDRATE      57600UL
#define MYUBRR  ((F_CPU + BAUDRATE * 4) / (BAUDRATE * 8) - 1)

void uart_bridge_init(void) {
    // U2X
    UCSR0A = (1 << U2X0);
    UCSR1A = (1 << U2X1);

    UBRR0 = MYUBRR;
    UBRR1 = MYUBRR;

    // RX et TX
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR1B = (1 << RXEN1) | (1 << TXEN1);

    // 8N1
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

void trigger_reset_328p(void) {
    PORTC &= ~(1 << PORTC0); //port 35 atm2561
    DDRC  |=  (1 << DDC0);
    _delay_ms(20);

    DDRC  &= ~(1 << DDC0);
    _delay_ms(50);
}

int main(void) {
    MCUSR = 0;
    wdt_disable();
    
    uart_bridge_init();
    trigger_reset_328p(); // restart atm328p

    while (1) {
        // PC to ATmega328P UART0 to UART1
        if (UCSR0A & (1 << RXC0))
        {
            uint8_t c = UDR0;
            while (!(UCSR1A & (1 << UDRE1)))
            {
            }
            UDR1 = c;
            
        }

        // ATmega328P to PC UART1 to UART0
        if (UCSR1A & (1 << RXC1))
        {
            uint8_t c = UDR1;
            while (!(UCSR0A & (1 << UDRE0)))
            {
            }
            UDR0 = c;
        }
    }
}