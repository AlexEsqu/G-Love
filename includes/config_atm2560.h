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
				  │			 │
CST816D SCL ──────┤ PD0 / SCL   │
CST816D SDA ──────┤ PD1 / SDA   │
				  │			 │
CST816D INT ──────┤ PB6		 │
CST816D RST ──────┤ PB7		 │
				  │			 │
				  │			 │
GC9A01A SCK  ─────┤ PB1 / SCK   │
GC9A01A MOSI ─────┤ PB2 / MOSI  │
GC9A01A CS   ─────┤ PB0		 │
GC9A01A DC   ─────┤ PB4		 │
GC9A01A RST  ─────┤ PB5		 │
				  └─────────────┘
*/

/************************/
/*		ADDRESSES		*/
/************************/

#define I2C_ADDR_CST816D 0x15 // tactile chip driver on the touchscreen
#define SPI_ADDR_GC9A91 0x2C // display chip driver on the touchscreen

/********************************/
/*		DISPLAY COMMANDS		*/
/********************************/

// see https://www.buydisplay.com/download/ic/GC9A01A.pdf p.90 - 94
#define GC9A01_COMMANDS(X)						\
	X(GC9A01_SOFTWARE_RESET,		0x01)		\
	X(GC9A01_READ_DISPLAY_ID,		0x04)		\
	X(GC9A01_READ_DISPLAY_STATUS,	0x09)		\
	X(GC9A01_SLEEP_ON,				0x10)		\
	X(GC9A01_SLEEP_OUT,				0x11)		\
	X(GC9A01_PARTIAL_ON,			0x12)		\
	X(GC9A01_NORMAL_ON,				0x13)		\
	X(GC9A01_INVERSE_OFF,			0x20)		\
	X(GC9A01_INVERSE_ON,			0x21)		\
	X(GC9A01_DISPLAY_OFF,			0x28)		\
	X(GC9A01_DISPLAY_ON,			0x29)		\
												\
	/* ---- RAW WRITE ---- */					\
	X(GC9A01_SET_COL_ADDR,			0x2A)		\
	X(GC9A01_SET_ROW_ADDR,			0x2B)		\
	X(GC9A01_MEM_WRITE,				0x2C)		\
	X(GC9A01_MEM_WRITE_C,			0x3C)		\
												\
	/* ---- EFFECTS ---- */						\
	X(GC9A01_PARTIAL_AREA,			0x30)		\
	X(GC9A01_VERT_SCROLL_DEF,		0x33)		\
	X(GC9A01_TEAR_OFF,				0x34)		\
	X(GC9A01_TEAR_ON,				0x35)		\
	X(GC9A01_SET_TEAR,				0x44)		\
	X(GC9A01_GET_LINE,				0x45)		\
	X(GC9A01_SET_BRIGHT,			0x51)		\
	X(GC9A01_SET_CTRL,				0x53)		\
												\
	/* ---- FORMAT ---- */						\
	X(GC9A01_MEM_ACCESS_CTL,		0x36)		\
	X(GC9A01_VERT_ADDR_START,		0x37)		\
	X(GC9A01_IDLE_OFF,				0x38)		\
	X(GC9A01_IDLE_ON,				0x39)		\
	X(GC9A01_SET_PIX_FORMAT,		0x3A)		\
												\
	/* ---- POWER ---- */						\
	X(GC9A01_POWER7,				0xA7)		\
	X(GC9A01_TEWC,					0xBA)		\
	X(GC9A01_POWER1,				0xC1)		\
	X(GC9A01_POWER2,				0xC3)		\
	X(GC9A01_POWER3,				0xC4)		\
	X(GC9A01_POWER4,				0xC9)		\
												\
	/* ---- IDENTIFICATION ---- */				\
	X(GC9A01_RD_ID1,				0xDA)		\
	X(GC9A01_RD_ID2,				0xDB)		\
	X(GC9A01_RD_ID3,				0xDC)		\
												\
	/* ---- TIMING ---- */						\
	X(GC9A01_FRAMERATE,				0xE8)		\
	X(GC9A01_SPI2DATA,				0xE9)		\
												\
	/* ---- WEIRD ---- */						\
	X(GC9A01_INREGEN2,				0xEF)		\
	X(GC9A01_GAMMA1,				0xF0)		\
	X(GC9A01_GAMMA2,				0xF1)		\
	X(GC9A01_GAMMA3,				0xF2)		\
	X(GC9A01_GAMMA4,				0xF3)		\
	X(GC9A01_IFACE,					0xF6)		\
	X(GC9A01_INREGEN1,				0xFE)		\
												\
	/* ---- PROPRIETARY MYSTERY ---- */			\
	X(GC9A01_MAGIC_70,				0x70)		\
    X(GC9A01_MAGIC_74,				0x74)		\
    X(GC9A01_MAGIC_84,				0x84)		\
    X(GC9A01_MAGIC_85,				0x85)		\
    X(GC9A01_MAGIC_86,				0x86)		\
    X(GC9A01_MAGIC_87,				0x87)		\
    X(GC9A01_MAGIC_88,				0x88)		\
    X(GC9A01_MAGIC_89,				0x89)		\
    X(GC9A01_MAGIC_8A,				0x8A)		\
    X(GC9A01_MAGIC_8B,				0x8B)		\
    X(GC9A01_MAGIC_8C,				0x8C)		\
    X(GC9A01_MAGIC_8D,				0x8D)		\
    X(GC9A01_MAGIC_8E,				0x8E)		\
    X(GC9A01_MAGIC_8F,				0x8F)		\
    X(GC9A01_MAGIC_90,				0x90)		\
    X(GC9A01_MAGIC_98,				0x98)		\
    X(GC9A01_MAGIC_EB,				0xEB)		\
    X(GC9A01_MAGIC_ED,				0xED)		\
    X(GC9A01_MAGIC_FF,				0xFF)

#define GC9A01_ENUM(name, hexcode) name = hexcode,

typedef enum {
	GC9A01_COMMANDS(GC9A01_ENUM)
}	GC9A01_cmd_t;

#undef GC9A01_ENUM


#define TFT_WIDTH 239UL
#define TFT_HEIGHT 239UL

enum {
	TFT_TAP,
	TFT_LEFT,
	TFT_UP,
	TFT_RIGHT,
	TFT_DOWN,
};


void	gc9a01_init();
void	gc9a01_send_command(GC9A01_cmd_t cmd);
void	gc9a01_send_parameter(uint8_t param);
void	gc9a01_fill_screen(uint32_t rgba);

void	cst816d_init();
int		received_data_from_cst816d();

void	set_pin_as_input(volatile uint8_t* reg, uint8_t pin);
void	set_pin_as_output(volatile uint8_t* reg, uint8_t pin);
void	set_pin_high(volatile uint8_t* reg, uint8_t pin);
void	set_pin_low(volatile uint8_t* reg, uint8_t pin);

#endif
