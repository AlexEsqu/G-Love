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
                  │             │
CST816D SCL ──────┤ PD0 / SCL   │
CST816D SDA ──────┤ PD1 / SDA   │
                  │             │
CST816D INT ──────┤ PB6         │
CST816D RST ──────┤ PB7         │
                  │             │
                  │             │
GC9A01A SCK  ─────┤ PB1 / SCK   │
GC9A01A MOSI ─────┤ PB2 / MOSI  │
GC9A01A CS   ─────┤ PB0         │
GC9A01A DC   ─────┤ PB4         │
GC9A01A RST  ─────┤ PB5         │
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

typedef enum
{
	GC9A01_SOFTWARE_RESET		= 0x01,
	GC9A01_READ_DISPLAY_ID		= 0x04,
	GC9A01_READ_DISPLAY_STATUS	= 0x09,
	GC9A01_SLEEP_ON				= 0x10,
	GC9A01_SLEEP_OUT			= 0x11,
	GC9A01_PARTIAL_ON			= 0x12,
	GC9A01_NORMAL_ON			= 0x13,
	GC9A01_INVERSE_OFF			= 0x20,
	GC9A01_INVERSE_ON			= 0x21,
	GC9A01_DISPLAY_OFF			= 0x28,
	GC9A01_DISPLAY_ON			= 0x29,

	// ---- Address / memory ----

	GC9A01_SET_COL_ADDR		= 0x2A, // Column Address Set
	GC9A01_SET_ROW_ADDR		= 0x2B, // Row Address Set
	GC9A01_MEM_WRITE		= 0x2C, // Memory Write

	// ---- Partial / scrolling ----

	GC9A01_PTLAR		= 0x30, // Partial Area
	GC9A01_VSCRDEF		= 0x33, // Vertical Scrolling Definition

	// ---- Tearing effect ----

	GC9A01_TEOFF		= 0x34, // Tearing Effect Line OFF
	GC9A01_TEON		 = 0x35, // Tearing Effect Line ON

	// ---- Memory access / pixel format ----

	GC9A01_MADCTL		= 0x36, // Memory Access Control
	GC9A01_VSCRSADD	 = 0x37, // Vertical Scrolling Start Address
	GC9A01_IDLEOFF		= 0x38, // Idle Mode OFF
	GC9A01_IDLEON		= 0x39, // Idle Mode ON
	GC9A01_COLMOD		= 0x3A, // Pixel Format Set

	// ---- Memory write continuation ----

	GC9A01_MEM_WRITEC		= 0x3C, // Write Memory Continue

	// ---- Tearing / scanline ----

	GC9A01_TEARSET		= 0x44, // Set Tear Scanline
	GC9A01_GETLINE		= 0x45, // Get Scanline

	// ---- Display brightness / control ----

	GC9A01_SETBRIGHT	= 0x51, // Write Display Brightness
	GC9A01_SETCTRL		= 0x53, // Write CTRL Display

	// ---- Power control ----

	GC9A01_POWER7		= 0xA7, // Power Control 7

	GC9A01_TEWC		 = 0xBA, // Tearing Effect Width Control

	GC9A01_POWER1		= 0xC1, // Power Control 1
	GC9A01_POWER2		= 0xC3, // Power Control 2
	GC9A01_POWER3		= 0xC4, // Power Control 3
	GC9A01_POWER4		= 0xC9, // Power Control 4

	// ---- Display identification ----

	GC9A01_RDID1		= 0xDA, // Read ID 1
	GC9A01_RDID2		= 0xDB, // Read ID 2
	GC9A01_RDID3		= 0xDC, // Read ID 3

	// ---- Timing / SPI ----

	GC9A01_FRAMERATE	= 0xE8, // Frame Rate Control
	GC9A01_SPI2DATA		= 0xE9, // SPI 2DATA Control

	// ---- Extended / manufacturer commands ----

	GC9A01_INREGEN2		= 0xEF, // Inter-Register Enable 2

	GC9A01_GAMMA1		= 0xF0, // Gamma 1
	GC9A01_GAMMA2		= 0xF1, // Gamma 2
	GC9A01_GAMMA3		= 0xF2, // Gamma 3
	GC9A01_GAMMA4		= 0xF3, // Gamma 4

	GC9A01_IFACE		= 0xF6, // Interface Control

	GC9A01_INREGEN1		= 0xFE	// Inter-Register Enable 1

} GC9A01_cmd_t;


#define TFT_WIDTH 239UL
#define TFT_HEIGHT 239UL


void	gc9a01_init();
void	gc9a01_send_command(GC9A01_cmd_t cmd);
void	gc9a01_send_parameter(uint8_t param);
void	gc9a01_fill_screen(uint32_t rgba);

void	cst816d_init();
int		received_data_from_cst816d();

void	set_pin_as_input(volatile uint8_t* reg, uint8_t pin);
void	set_pin_as_output(volatile uint8_t* reg, uint8_t pin);
void	set_pin_up(volatile uint8_t* reg, uint8_t pin);
void	set_pin_low(volatile uint8_t* reg, uint8_t pin);

#endif
