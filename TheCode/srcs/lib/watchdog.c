#include <avr/io.h>
#include <stdint.h>

/*
	WATCHDOG - replaces MCUSR = 0; wdt_disable(); from <avr/wdt.h>
	Datasheet atm328P section 10.9 / atm2561 section 12.5

	The watchdog is a timer that resets the MCU if the program does not
	reset it in time. We do not use it, so we turn it off at boot
	(it can stay on after a watchdog reset or because of the WDTON fuse).

	Registers used:

	SREG   - Status Register
	  bit 7  I     : 1 = interrupts allowed, 0 = interrupts blocked

	MCUSR  - MCU Status Register (why the MCU restarted)
	  bit 3  WDRF  : 1 = last reset caused by the watchdog
	                 while WDRF = 1, the hardware FORCES WDE = 1
	                 -> must be cleared first, or the watchdog stays on
	  bit 2  BORF  : brown-out reset   (kept)
	  bit 1  EXTRF : reset pin         (kept)
	  bit 0  PORF  : power-on          (kept)

	WDTCSR - Watchdog Timer Control Register
	  bit 7  WDIF  : interrupt flag
	  bit 6  WDIE  : interrupt mode
	  bit 4  WDCE  : change enable, unlocks WDE and WDP for 4 clock cycles
	  bit 3  WDE   : reset mode (1 = the watchdog can reset the MCU)
	  bit 5,2-0 WDP: timeout duration

	Protected sequence (datasheet), to avoid turning it off by accident:
	  1. write WDCE = 1 and WDE = 1 at the same time  -> unlocks
	  2. within the next 4 clock cycles, write the new value -> 0 = off
*/

#define WDT_UNLOCK	((1 << WDCE) | (1 << WDE))
#define WDT_OFF		0x00

// optimize("Os"): keeps the two WDTCSR writes back to back (4 cycles max)
// even if the project is one day compiled without optimization (-O0)
__attribute__((optimize("Os")))
void watchdog_disable(void)
{
	// save the interrupt state (bit I of SREG), then block interrupts
	uint8_t sreg_backup = SREG;
	SREG &= ~(1 << SREG_I);

	// restart the watchdog counter, so it cannot expire during the sequence
	__asm__ __volatile__ ("wdr");

	// clear only WDRF, otherwise WDE stays forced to 1
	MCUSR &= ~(1 << WDRF);

	// unlock the watchdog configuration (opens the 4 cycles window)
	WDTCSR = WDT_UNLOCK;

	// turn the watchdog off: WDE = 0, WDIE = 0
	WDTCSR = WDT_OFF;

	// restore the interrupt state as it was before
	SREG = sreg_backup;
}
