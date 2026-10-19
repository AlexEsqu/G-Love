#ifndef CONFIG_ATM2561_H
#define CONFIG_ATM2561_H

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#define BAUDRATE 57600UL
#define MYUBRR ((F_CPU + BAUDRATE * 4) / (BAUDRATE * 8) - 1)

// Globals
extern uint16_t				g_ms_counter;
extern uint8_t				g_touch_debounce;

// To convert from Arduino Mega to ATMEGA 2560, see:
// https://devboards.info/boards/arduino-mega2560-rev3

/********************/
/*		PORTS		*/
/********************/

// SPI - Screen display on GC9A01A chip

// Pins
#define TFT_CS			10
#define TFT_SCK			11
#define TFT_MOSI		12
#define TFT_MISO		13
#define TFT_DC			14
#define TFT_RST			15

// Insude ATMEGA 2561
#define SCREEN_DDR		DDRB
#define SCREEN_PORT		PORTB
#define SCREEN_PIN		PINB
#define SCREEN_SS		PB0
#define SCREEN_SCK		PB1
#define SCREEN_MOSI		PB2
#define SCREEN_DC		PB4
#define SCREEN_RST		PB5

// I2C - tactile input on CST816D chip

// Pins
#define TFT_SCL			25
#define TFT_SDA			26
#define TFT_TOUCH_RST	17
#define TFT_INT			16

// Inside the ATMEGA 2560
#define TOUCH_I2C_DDR		DDRD
#define TOUCH_I2C_PINS		PIND
#define TOUCH_I2C_PORT		PORTD
#define TOUCH_SCL_PIN		PD0
#define TOUCH_SDA_PIN		PD1

#define TOUCH_GPIO_DDR		DDRB
#define TOUCH_GPIO_PINS		PINB
#define TOUCH_GPIO_PORT		PORTB
#define TOUCH_RST_PIN		PB7
#define TOUCH_INT_PIN		PB6

/*

					ATmega2561
					┌─────────────┐
					│			  │
CST816D SCL	  ──────┤	PD0	/ SCL │
CST816D SDA	  ──────┤	PD1	/ SDA │
					│			  │
CST816D INT	  ──────┤	PB6		  │
CST816D RST	  ──────┤	PB7		  │
					│			  │
					│			  │
GC9A01A SCK	   ─────┤ PB1 / SCK   │
GC9A01A MOSI   ─────┤ PB2 / MOSI  │
GC9A01A CS	   ─────┤ PB0		  │
GC9A01A DC	   ─────┤ PB4		  │
GC9A01A RST	   ─────┤ PB5		  │
					└─────────────┘
*/

#endif
