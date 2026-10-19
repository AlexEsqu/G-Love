#include "../../includes/main.h"

int main()
{
	MCUSR = 0;
    wdt_disable();
	// watchdog_disable();

	// differente flag when compiling with avrgcc depending of mcu
	#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
		main_ATM328P();
		//uart_init_test();

	#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
		main_ATM2560();
		// uart_init_test();
	#endif
}