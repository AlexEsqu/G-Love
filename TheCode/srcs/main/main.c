#include "main.h"

int main()
{
	// differente flag when compiling with avrdude depending of mcu
	#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
		main_ATM328P();
	#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
		main_ATM2560();
	#endif
}
