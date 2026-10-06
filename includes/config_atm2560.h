#ifndef CONFIG_ATM2560_H
#define CONFIG_ATM2560_H

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define BAUDRATE 115200

// To convert from Arduino Mega to ATMEGA 2560, see:
// https://devboards.info/boards/arduino-mega2560-rev3

/********************/
/*		PORTS		*/
/********************/

// SPI - Screen display on GC9A01A chip

// On Arduino Mega
#define TFT_CS			53
#define TFT_SCK			52
#define TFT_MOSI		51
#define TFT_MISO		50
#define TFT_RST			11
#define TFT_DC			10

// On ATMEGA 2560
#define SCREEN_DDR		DDRB
#define SCREEN_PORT		PORTB
#define SCREEN_PIN		PINB
#define SCREEN_SS		PB0
#define SCREEN_SCK		PB1
#define SCREEN_MOSI		PB2
#define SCREEN_DC		PB4
#define SCREEN_RST		PB5

// I2C - tactile input on CST816D chip

// On Arduino Mega
#define TFT_SCL			21
#define TFT_SDA			20
#define TFT_TOUCH_RST	13
#define TFT_INT			12

// On ATMEGA 2560
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

Annoyingly, CST chip's INT and RST pins are on another register

					ATmega2560
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
