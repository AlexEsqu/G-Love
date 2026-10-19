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
    SREG |= (1 << SREG_I); // the UART works with interrupts

    while(1)
    {
        uint8_t c;

        if (uart0_read(&c)) {
            uart0_tx('[');
            uart0_tx(c);
            uart0_tx(']');
            if (c == '\r')
                uart0_printstr("\r\n");
        }

        _delay_ms(1);
    }
}
