#ifndef CONFIG_ATM2560_H
#define CONFIG_ATM2560_H

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#define BAUDRATE 38400
#define MYUBRR ((F_CPU + BAUDRATE * 4) / (BAUDRATE * 8) - 1)

// define tactile port
#define I2C_ADDR_CST816D 0x15 // addresse du tactile
#define TOUCH_INT 7

#define RX_BUFFER_SIZE 128
#define SENSOR_PACKET_MAGIC2 0xA55A

typedef struct __attribute__((packed)) s_sensor_data_to_pc
{
    uint16_t	magic; //magic number to detect start of packet
    uint16_t	sensorForce[5]; //fsr
    uint16_t	sensorFlex[5]; //fsr
    int16_t 	accel[3]; //imu
    int16_t		gyro[3]; //imu
    uint16_t	screen_x; //touchscreen x cordinate
    uint16_t	screen_y; //touchscreen y cordinate
    uint8_t		screen_action; //touchscreen: action= swipe left, swipe right, press etc...
    uint16_t	crc16; // crc to protect and check data authenticity
} t_sensor_data_to_pc;

extern t_sensor_data_to_pc g_packet;

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
