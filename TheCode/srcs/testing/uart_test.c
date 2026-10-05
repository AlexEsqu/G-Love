#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/wdt.h> 
#include "../../includes/main.h"

#define BAUDRATE 57600UL
#define MYUBRR ((F_CPU + BAUDRATE * 4) / (BAUDRATE * 8) - 1)   

void uart_init(void)
{
    UBRR0  = MYUBRR;
    UCSR0A |= (1 << U2X0);
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
}

int main(void)
{
    MCUSR = 0;
    wdt_disable();

    uart_init();

    while(1)
    {
        if (UCSR0A & (1 << RXC0)) {
            char c = UDR0;
            uart0_tx('[');
            uart0_tx(c);
            uart0_tx(']');
            if (c == '\r')  
                uart0_printstr("\r\n");
        }

        _delay_ms(1);
    }
}