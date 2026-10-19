#include "main.h"

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

uint8_t		g_touch_debounce = 0;
uint16_t	g_ms_counter = 0;

// 16 MHz / 256 * 0.010
#define TICKS_10MS   625u
// 16 MHz / 256 * 0.5
#define TICKS_500MS  31250u

// Initialize timer1 to ping comA every 10ms and comB every 0.5s
void	touch_timer_init()
{
	// Initialize the timer (by default but in case previous set)
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	OCR1B  = TICKS_10MS;

	// Set wavelength generation mode in two control registers
	TIFR1  = (1 << OCF1A) | (1 << OCF1B);

	// enable B compare interrupts for ms counting
	TIMSK1 = (1 << OCIE1B);

	// Launch Prescaler
	TCCR1B = (1 << CS12);
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
	PCICR |= (1 << PCIF0);

	// set interrupts specifically on pin PCINT 6
	PCMSK0 |= (1 << PCINT6);
}

// Launch fast timer to check the touch press actually was a touch
void	launchTouchDebounce()
{
	disableTouchInterrupt();
	g_touch_debounce = 1;

	// set COMA value to when debounce is over
	// should wrap on overflow
	OCR1A = TCNT1 + TOUCH_DEBOUNCE_TIME;

	// enable COMA interrupt on timer
	TIMSK1 |= (1 << OCIE1A);
}

void	stopTouchDebounce()
{
	g_touch_debounce = 0;

	// remove COMA interrupt on timer
	TIMSK1 &= ~(1 << OCIE1A);

	enableTouchInterrupt();
}


void	concludeTouchDebounce()
{
	onTouch();
	stopTouchDebounce();
}


// ON 10 MILISECOND
// Matching vector #44 per datasheet Table 14-1 p. 101-102
void __attribute__((signal)) TIMER1_COMPB_vect (void)
{
	// should wrap
	OCR4B += TICKS_10MS;

	g_ms_counter += 10;
	g_animation_frame = g_animation_frame ? 0 : 1;
}

// FEW MILISECONDS AFTER TOUCH
// Set on interrupt function of timer2 on COMA
// (set to be no wired interrupt in the current setup)
// Matching vector #43 per datasheet Table 14-1 p. 101-102
void __attribute__((signal)) TIMER1_COMPA_vect (void)
{
	concludeTouchDebounce();
}

void __attribute__((signal)) PCINT0_vect (void)
{
	launchTouchDebounce();
}
