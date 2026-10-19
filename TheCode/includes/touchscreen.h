#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <stdint.h>
#include <avr/pgmspace.h>

#define TOUCH_ADDR_CST816D 0x15
#define DISPLAY_ADDR_GC9A91 0x2C
#define TFT_WIDTH 240UL
#define TFT_HEIGHT 240UL
#define TFT_PIXEL_COUNT 57600UL
#define TFT_CHUNK_COUNT 14400UL
#define TFT_FRAME 12500UL

typedef enum {
	TFT_ACTION_TAP = 0x00,
	TFT_ACTION_UP = 0x01,
	TFT_ACTION_DOWN = 0x02,
	TFT_ACTION_LEFT = 0x03,
	TFT_ACTION_RIGHT = 0x04,
	TFT_ACTION_SINGLE_TAP = 0x05,
	TFT_ACTION_DOUBLE_TAP = 0x0B,
	TFT_ACTION_LONG_TAP = 0x0C,
} tft_action_t ;

typedef struct {
	uint16_t		x;
	uint16_t		y;
	tft_action_t	action;
	tft_action_t	last_action;
	uint16_t		rgb565;
	uint8_t			is_pending;
} tft_touch_info_t;

typedef struct gc9a01_sequence gc9a01_sequence_t;

struct gc9a01_sequence {
	const uint8_t				cmd;
	const uint8_t*				args;
	const uint8_t				args_size;
	const uint32_t				delay_ms;
};

typedef struct {
	uint8_t	row;
	uint8_t col;
	uint8_t width;
	uint8_t height;
	uint32_t total_pixels;
	uint16_t palette[16];
	const uint8_t* pixels;
} packed_image_t;

typedef struct {
	uint16_t width;
	uint16_t height;
	uint32_t total_pixels;
	const uint8_t* pixels;
} very_packed_image_t;

typedef struct {
	const packed_image_t*	frames[2];
	const uint8_t			frame_count;
	uint16_t				(*get_background_color)(uint16_t, uint16_t);
	void					(*print_screen_text)(void);
	void					(*on_touch_action)(void);
} tft_screen_info_t;

#include "images/hand.h"
#include "images/font.h"
#include "images/flexscreen.h"
#include "images/homescreen.h"
#include "images/comscreen.h"

typedef enum {
	RGB16_TRANS		= 0x0001,
	RGB16_BLACK		= 0x0000,
	RGB16_WHITE		= 0xFFFF,

	RGB16_RED		= 0xF800,
	RGB16_GREEN		= 0x07E0,
	RGB16_BLUE		= 0x001F,
	RGB16_YELLOW	= 0xFFE0,
	RGB16_CYAN		= 0x07FF,
	RGB16_MAGENTA	= 0xF81F,

	RGB16_ORANGE	= 0xFD20,
	RGB16_PURPLE	= 0x780F,
	RGB16_PINK		= 0xF81F,
	RGB16_BROWN		= 0xA145,
	RGB16_GRAY		= 0x8410,
	RGB16_GOLD		= 0xFEA0,
	RGB16_NAVY		= 0x000F,

	// greens
	RGB16_FOREST_FLOOR = 0x00A0,
	RGB16_MATRIX_DARK  = 0x0140,
	RGB16_CODE_GREEN   = 0x05E0,
	RGB16_NEON_GREEN   = 0x37E6,
	RGB16_PHOSPHOR     = 0x77A0,
	RGB16_TOXIC_SLIME  = 0x9FE0,

	// yellows
	RGB16_DARK_AMBER   = 0x9200,
	RGB16_VINTAGE_GOLD = 0xCDA0,
	RGB16_GOLDEN_OCHRE = 0xB560,
	RGB16_CYBER_GOLD   = 0xFE20,
	RGB16_ALIVE_AMBER  = 0xFCE0,
	RGB16_LIGHT_YELLOW = 0xFCE3,
	RGB16_SOFT_YELLOW  = 0xFFE8,

	// oranges
	RGB16_TANGERINE   = 0xFC00,
	RGB16_SUNSET_ORANGE= 0xF320,
	RGB16_PUMPKIN	  = 0xFBA3,
	RGB16_CARROT	  = 0xEC84,
	RGB16_BURNT_ORANGE = 0xCAA0,
	RGB16_RUST		 = 0xB202,
	RGB16_MANDARIN	 = 0xF405,
	RGB16_CORAL_ORANGE = 0xFB6C,
	RGB16_APRICOT	  = 0xFE54,

	// reds
	RGB16_EMBER_GLOW   = 0xA000,
	RGB16_FLAME_CORE   = 0xF900,
	RGB16_PLASMA_BURN  = 0xFC00,
	RGB16_THERMAL_WARN = 0xFA40,

	// purples
	RGB16_VOID_PURPLE  = 0x2004,
	RGB16_DUSK_VIOLET  = 0x3808,
	RGB16_NEO_VIOLET   = 0x4810,
	RGB16_CYBER_PULSE  = 0x8013,
	RGB16_LASER_ORCHID = 0xB81C,

	// Blues
	RGB16_LAPIS_SHADOW = 0x0045,
	RGB16_LAPIS_CORE   = 0x0110,
	RGB16_CYBER_SAPPHIRE=0x031B,
	RGB16_ULTRAMARINE  = 0x04DF,
	RGB16_ABYSS_SHADOW = 0x0008,
	RGB16_OCEAN_BEACON = 0x043A,
	RGB16_COBALT_DARK  = 0x00CD,
	RGB16_COBALT_FLARE = 0x247F,
	RGB16_DEEP_CYAN    = 0x018D,
	RGB16_BIO_GLOW     = 0x57FF,
	RGB16_GLACIER_DEEP = 0x0A50,
	RGB16_FROST_LIGHT  = 0xCE7F,
} gc9a01_color_t;

typedef enum {
	TFT_STATE_HOMESCREEN,	// no mode selected
	TFT_STATE_COLORSCREEN,	// Select LEDs color on touchscreen
	TFT_STATE_IMUSCREEN,	// IMU calibration and data read
	TFT_STATE_FLEXSCREEN,	// Flex sensor affect LEDs
	TFT_STATE_COMMSCREEN,	// Select output mode
}	tft_state_t;

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

typedef enum {
	FONT_SIZE_SMALL = 1,
	FONT_SIZE_MID = 2,
	FONT_SIZE_BIG = 4,
} tft_font_size_t ;


void		gc9a01_init();
void		gc9a01_send_command(gc9a01_cmd_t cmd);
void		gc9a01_send_parameter(uint8_t param);
void		gc9a01_fill_screen(uint16_t rgb565);
void		gc9a01_execute_sequence(const gc9a01_sequence_t* sequence);
void		gc9a01_start_parameter_sequence();
void		gc9a01_end_parameter_sequence();
void		gc9a01_send_init_sequence();
void		gc9a01_send_pixel(uint8_t x, uint8_t y, uint16_t rgb256);
void		gc9a01_send_square(uint8_t x, uint8_t y, uint16_t rgb256, uint16_t width);
void		gc9a01_send_image(uint8_t x, uint8_t y, const packed_image_t* image);
void		gc9a01_set_display_size(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);

uint32_t	rgb565_to_rgb888(uint16_t rgb565);

void		display_screen();
void		refresh_screen();
int			is_screen_frame(int16_t x, int16_t y);
uint8_t		is_in_mask(uint16_t row, uint16_t col, const very_packed_image_t* image);
uint16_t	putchar_screen(uint16_t x, uint16_t y, char c, uint16_t font_color, tft_font_size_t font_size);
uint16_t	putstr_screen(uint16_t x, uint16_t y, const char *str, uint16_t font_color, tft_font_size_t font_size);
uint16_t	putfloat_screen(uint16_t x, uint16_t y, float nbr, uint16_t font_color, tft_font_size_t font_size);
uint16_t	putint_screen(uint16_t x, uint16_t y, int32_t nbr, uint16_t font_color, tft_font_size_t font_size);
uint16_t	puthex_screen(uint16_t x, uint16_t y, uint32_t nbr, uint16_t font_color, tft_font_size_t font_size);
void		print_touch_info(tft_touch_info_t* touch_info);

void	cst816d_init();
int		received_data_from_cst816d();
void	onTouch();

uint8_t	is_top_quarter_of_screen(uint16_t row, uint16_t col);
uint8_t is_right_quarter_of_screen(uint16_t row, uint16_t col);
uint8_t is_bottom_quarter_of_screen(uint16_t row, uint16_t col);
uint8_t is_left_quarter_of_screen(uint16_t row, uint16_t col);
uint8_t	is_in_top_left_of_screen(uint16_t row, uint16_t col);
uint8_t	is_in_top_right_of_screen(uint16_t row, uint16_t col);
uint8_t	is_in_bottom_left_of_screen(uint16_t row, uint16_t col);
uint8_t	is_in_bottom_right_of_screen(uint16_t row, uint16_t col);

void	touch_timer_init();
void	enableTouchInterrupt();

// SCREENS
extern const tft_screen_info_t g_screens[5];

uint16_t	get_color_from_colorscreen(uint16_t row, uint16_t col);
uint16_t	get_color_from_colorscreen_fast(uint16_t row, uint16_t col);
void		print_colorscreen_text(void);
void		on_touch_on_colorscreen();
uint16_t	get_color_from_homescreen(uint16_t row, uint16_t col);
void		print_homescreen_text(void);
void		on_touch_on_homescreen(void);
uint16_t	get_color_from_imuscreen(uint16_t row, uint16_t col);
void		print_imuscreen_text(void);
void		on_touch_on_imuscreen();
uint16_t	get_color_from_flexscreen(uint16_t row, uint16_t col);
void		print_flexscreen_text(void);
void		on_touch_on_flexscreen();
uint16_t	get_color_from_comscreen(uint16_t row, uint16_t col);
void		print_comscreen_text(void);
void		on_touch_on_comscreen();

void		print_nav_info(uint16_t color);

#define GC9A01_INIT_SEQUENCE_LEN 51

extern const gc9a01_sequence_t gc9a01_init_sequence[GC9A01_INIT_SEQUENCE_LEN];

#endif
