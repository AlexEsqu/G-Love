#include "../includes/config_atm2560.h"

// ATmega 2560/2561 datasheet p.154 & 156

// View of the timer registers for:
//  normal mode,
//  Toggle COMA on match,
//  with prescaler 1024

// TCCR4A :
// COM4A1	COM4A0	COM4B1	COM4B0	COM4C1	COM4C0	WGM11	WGM10
//		0		1		0		0		0		0		0		0

// TCCR4B :
// ICNC4	ICES4	-----	WGM43	WGM42	CS42	CS41	CS40
//		0		0		0		0		0		1		0		0

#define TOUCH_DEBOUNCE_TIME 50

// Initialize timer1 to debounce tactile
void	touch_timer_init()
{
    // Initialize the timer (by default but in case previous set)
    TCNT4 = 0;
    TCCR4A = 0;
    TCCR4B = 0;

    // Set wavelength generation mode in two control registers
    TCCR4A = 0b01000000;
    TCCR4B = 0b00000100;
}

// disallow interrupts on pin PCINT 6
void	disableTouchInterrupt()
{
	PCMSK0 &= ~(1 << PCINT6);
}

void	enableTouchInterrupt()
{
	// allow interrupts in general
	SREG |= (1 << 7);

	// allow interrupts on pins PCINT 7 to 0
	PCIFR |= (1 << PCIF0);

	// set interrupts specifically on pin PCINT 6
	PCMSK0 |= (1 << PCINT6);
}

// Launch fast timer to check the touch press actually was a touch
void	launchTouchDebounce()
{
	uart0_printstr("registred");

	disableTouchInterrupt();

	// set COMA value to when debounce is over
	// should wrap on overflow
	OCR4A = TCNT4 + TOUCH_DEBOUNCE_TIME;

	// enable COMA interrupt on timer
	TIMSK4 |= (1 << OCIE4A);
}

void	stopTouchDebounce()
{
	enableTouchInterrupt();

	// remove COMA interrupt on timer
	TIMSK4 &= ~(1 << OCIE4A);
}


void	concludeTouchDebounce()
{
	// if button is not still pressed, was probably faulty
	// if (!received_data_from_cst816d())
	// {
	// 	stopTouchDebounce();
	// 	return;
	// }

	onTouch();
	stopTouchDebounce();
}


// ON TOUCH
// Set on interrupt function of PCINT6 / OC1B on pin D12
// (set to be the touch interrupt in the current wiring)
// Matching vector #19 per datasheet Table 14-1 p. 101-102
void __attribute__((signal)) TIMER1_COMPB_vect (void)
{
	launchTouchDebounce();
}

// FEW MILISECONDS AFTER TOUCH
// Set on interrupt function of timer2 on COMA
// (set to be no wired interrupt in the current setup)
// Matching vector #43 per datasheet Table 14-1 p. 101-102
void __attribute__((signal)) TIMER4_COMPA_vect (void)
{
	concludeTouchDebounce();
}
