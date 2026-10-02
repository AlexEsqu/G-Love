#ifndef gc9a01_SEQUENCES_H
#define gc9a01_SEQUENCES_H

#include <avr/io.h>

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

// Making explicit all available commands on the GC9A01 driver
typedef enum {
	GC9A01_COMMANDS(GC9A01_ENUM)
}	gc9a01_cmd_t;

#undef GC9A01_ENUM

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

#endif
