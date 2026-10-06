#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <stdint.h>

#define TOUCH_ADDR_CST816D 0x15
#define DISPLAY_ADDR_GC9A91 0x2C
#define TFT_WIDTH 240UL
#define TFT_HEIGHT 240UL
#define TFT_PIXEL_COUNT 57600UL

#define SLIDE_UP 0x01
#define SLIDE_DOWN 0x02
#define SLIDE_LEFT 0x03
#define SLIDE_RIGHT 0x04
#define SINGLE_CLICK 0x05
#define DOUBLE_CLICK 0x0B
#define LONG_PRESS 0x0C

typedef struct touchscreen_data_s {
    uint16_t x;
    uint16_t y;
    uint8_t gesture;
} touchscreen_data_t;

typedef struct gc9a01_sequence gc9a01_sequence_t;

// Command and arguments bytes with minimum delay
struct gc9a01_sequence {
	const uint8_t				cmd;
	const uint8_t*				args;
	const uint8_t				args_size;
	const uint32_t				delay_ms;
};

typedef struct gc9a01_pixel {
	uint8_t		x;
	uint8_t		y;
	uint16_t	rgb256;
} gc9a01_pixel_t;

enum {
	TFT_TAP,
	TFT_LEFT,
	TFT_UP,
	TFT_RIGHT,
	TFT_DOWN,
};

typedef enum {

	RGB565_BLACK	= 0x0000,
	RGB565_WHITE	= 0xFFFF,

	RGB565_RED		= 0xF800,
	RGB565_GREEN	= 0x07E0,
	RGB565_BLUE		= 0x001F,

	RGB565_YELLOW	= 0xFFE0,
	RGB565_CYAN		= 0x07FF,
	RGB565_MAGENTA	= 0xF81F,

	RGB565_ORANGE	= 0xFD20,
	RGB565_PURPLE	= 0x780F,
	RGB565_PINK		= 0xF81F,
	RGB565_BROWN	= 0xA145,
	RGB565_GRAY		= 0x8410,
	RGB565_GOLD		= 0xFEA0,
	RGB565_NAVY		= 0x000F,

} gc9a01_color_t;

extern int g_touchscreen_mode;

// see https://www.buydisplay.com/download/ic/GC9A01A.pdf p.90 - 94
#define GC9A01_COMMANDS(X)						\
	X(GC9A01_NULL,					0x00)		\
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
	/* -- IDENTIFICATION -- */					\
	X(GC9A01_RD_ID1,				0xDA)		\
	X(GC9A01_RD_ID2,				0xDB)		\
	X(GC9A01_RD_ID3,				0xDC)		\
												\
	/* ---- TIMING -------- */					\
	X(GC9A01_FRAMERATE,				0xE8)		\
	X(GC9A01_SPI2DATA,				0xE9)		\
												\
	/* ---- WEIRD -------- */					\
	X(GC9A01_INREGEN2,				0xEF)		\
	X(GC9A01_GAMMA1,				0xF0)		\
	X(GC9A01_GAMMA2,				0xF1)		\
	X(GC9A01_GAMMA3,				0xF2)		\
	X(GC9A01_GAMMA4,				0xF3)		\
	X(GC9A01_IFACE,					0xF6)		\
	X(GC9A01_INREGEN1,				0xFE)		\

#define GC9A01_ENUM(name, hexcode) name = hexcode,

// Making explicit all available commands on the GC9A01 driver
typedef enum {
	GC9A01_COMMANDS(GC9A01_ENUM)
}	gc9a01_cmd_t;

#undef GC9A01_ENUM



void	gc9a01_init();
void	gc9a01_send_command(gc9a01_cmd_t cmd);
void	gc9a01_send_parameter(uint8_t param);
void	gc9a01_fill_screen(uint16_t rgb565);
void	gc9a01_execute_sequence(const gc9a01_sequence_t* sequence);
void	gc9a01_send_pixel(uint8_t x, uint8_t y, uint16_t rgb256);
void	gc9a01_send_square(uint8_t x, uint8_t y, uint16_t rgb256, uint16_t width);

void	cst816d_init();
int		received_data_from_cst816d();
void	onTouch();

void	touchscreen_read(touchscreen_data_t *data);

void	set_pin_as_input(volatile uint8_t* reg, uint8_t pin);
void	set_pin_as_output(volatile uint8_t* reg, uint8_t pin);
void	set_pin_high(volatile uint8_t* reg, uint8_t pin);
void	set_pin_low(volatile uint8_t* reg, uint8_t pin);

void	touch_timer_init();
void	enableTouchInterrupt();





#endif
