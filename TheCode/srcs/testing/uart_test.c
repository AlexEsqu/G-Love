#include "../../includes/main.h"

/*
	UART echo test for the atm2561: every byte received is sent back
	between brackets. Same speed as the firmware (BAUDRATE in atm2561.h),
	so it can be read with "make screen".
*/
int main(void)
{
    watchdog_disable();

    uart0_init();

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
