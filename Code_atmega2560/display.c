#include "../includes/config_atm2560.h"
#include "../includes/main.h"

// Using the display driver chip GC9A01
// Set up in Serial Interface Protocol (SPI)
// in a 4-line serial interface with:
//	- SCL: Serial Clock Input - SCREEN_SCK
//	- SDA: Serial Data - SCREEN_MOSI
//	- D/C: Data / Command selection input - SCREEN_DC
//	- CS (aka SS): Chip Select / Slave Select - SCREEN_DC
// and a reset pin such that:
//  - RST: Reset the display driver - SCREEN_RST

const gc9a01_sequence_t gc9a01_init_sequence[] =
{
	{
		.cmd = GC9A01_INREGEN2,
		.args = 0x00,
		.args_size = 0,
		.delay_ms = 0,
	},
	{
		.cmd = 0xEB,
		.args = (const uint8_t[]){ 0x14 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_INREGEN1,
		.args = 0x00,
		.args_size = 0,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_INREGEN2,
		.args = 0x00,
		.args_size = 0,
		.delay_ms = 0,
	},
	{
		.cmd = 0xEB,
		.args = (const uint8_t[]){ 0x14 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x84,
		.args = (const uint8_t[]){ 0x40 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x85,
		.args = (const uint8_t[]){ 0xFF },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x86,
		.args = (const uint8_t[]){ 0xFF },
		.args_size = 1,
		.delay_ms = 0,
	},

	{
		.cmd = 0x87,
		.args = (const uint8_t[]){ 0xFF },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x88,
		.args = (const uint8_t[]){ 0x0A },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x89,
		.args = (const uint8_t[]){ 0x21 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x8A,
		.args = (const uint8_t[]){ 0x00 },
		.args_size = 1,
		.delay_ms = 0,
	},

	{
		.cmd = 0x8B,
		.args = (const uint8_t[]){ 0x80 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x8C,
		.args = (const uint8_t[]){ 0x01 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x8D,
		.args = (const uint8_t[]){ 0x01 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x8E,
		.args = (const uint8_t[]){ 0xFF },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x8F,
		.args = (const uint8_t[]){ 0xFF },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0xB6,
		.args = (const uint8_t[]){ 0x00, 0x20 },
		.args_size = 2,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_MEM_ACCESS_CTL,
		.args = (const uint8_t[]){ 0x08 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_SET_PIX_FORMAT,
		.args = (const uint8_t[]){ 0x05 },
		.args_size = 1,
		.delay_ms = 0,
	},

	{
		.cmd = 0x90,
		.args = (const uint8_t[]){
			0x08, 0x08, 0x08, 0x08
		},
		.args_size = 4,
		.delay_ms = 0,
	},
	{
		.cmd = 0xBD,
		.args = (const uint8_t[]){ 0x06 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0xBC,
		.args = (const uint8_t[]){ 0x00 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0xFF,
		.args = (const uint8_t[]){
			0x60, 0x01, 0x04
		},
		.args_size = 3,
		.delay_ms = 0,
	},

	{
		.cmd = GC9A01_POWER2,
		.args = (const uint8_t[]){ 0x13 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_POWER3,
		.args = (const uint8_t[]){ 0x13 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_POWER4,
		.args = (const uint8_t[]){ 0x22 },
		.args_size = 1,
		.delay_ms = 0,
	},

	{
		.cmd = 0xBE,
		.args = (const uint8_t[]){ 0x11 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0xE1,
		.args = (const uint8_t[]){ 0x10, 0x0E },
		.args_size = 2,
		.delay_ms = 0,
	},
	{
		.cmd = 0xDF,
		.args = (const uint8_t[]){
			0x21, 0x0C, 0x02
		},
		.args_size = 3,
		.delay_ms = 0,
	},

	{
		.cmd = GC9A01_GAMMA1,
		.args = (const uint8_t[]){
			0x45, 0x09, 0x08, 0x08, 0x26, 0x2A
		},
		.args_size = 6,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_GAMMA2,
		.args = (const uint8_t[]){
			0x43, 0x70, 0x72, 0x36, 0x37, 0x6F
		},
		.args_size = 6,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_GAMMA3,
		.args = (const uint8_t[]){
			0x45, 0x09, 0x08, 0x08, 0x26, 0x2A
		},
		.args_size = 6,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_GAMMA4,
		.args = (const uint8_t[]){
			0x43, 0x70, 0x72, 0x36, 0x37, 0x6F
		},
		.args_size = 6,
		.delay_ms = 0,
	},
	{
		.cmd = 0xED,
		.args = (const uint8_t[]){ 0x1B, 0x0B },
		.args_size = 2,
		.delay_ms = 0,
	},
	{
		.cmd = 0xAE,
		.args = (const uint8_t[]){ 0x77 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0xCD,
		.args = (const uint8_t[]){ 0x63 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x70,
		.args = (const uint8_t[]){
			0x07, 0x07, 0x04, 0x0E, 0x0F,
			0x09, 0x07, 0x08, 0x03
		},
		.args_size = 9,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_FRAMERATE,
		.args = (const uint8_t[]){ 0x34 },
		.args_size = 1,
		.delay_ms = 0,
	},
	{
		.cmd = 0x62,
		.args = (const uint8_t[]){
			0x18, 0x0D, 0x71, 0xED, 0x70, 0x70,
			0x18, 0x0F, 0x71, 0xEF, 0x70, 0x70
		},
		.args_size = 12,
		.delay_ms = 0,
	},
	{
		.cmd = 0x63,
		.args = (const uint8_t[]){
			0x18, 0x11, 0x71, 0xF1, 0x70, 0x70,
			0x18, 0x13, 0x71, 0xF3, 0x70, 0x70
		},
		.args_size = 12,
		.delay_ms = 0,
	},
	{
		.cmd = 0x64,
		.args = (const uint8_t[]){
			0x28, 0x29, 0xF1, 0x01,
			0xF1, 0x00, 0x07
		},
		.args_size = 7,
		.delay_ms = 0,
	},
	{
		.cmd = 0x66,
		.args = (const uint8_t[]){
			0x3C, 0x00, 0xCD, 0x67, 0x45,
			0x45, 0x10, 0x00, 0x00, 0x00
		},
		.args_size = 10,
		.delay_ms = 0,
	},
	{
		.cmd = 0x67,
		.args = (const uint8_t[]){
			0x00, 0x3C, 0x00, 0x00, 0x00,
			0x01, 0x54, 0x10, 0x32, 0x98
		},
		.args_size = 10,
		.delay_ms = 0,
	},
	{
		.cmd = 0x74,
		.args = (const uint8_t[]){
			0x10, 0x85, 0x80, 0x00,
			0x00, 0x4E, 0x00
		},
		.args_size = 7,
		.delay_ms = 0,
	},
	{
		.cmd = 0x98,
		.args = (const uint8_t[]){ 0x3E, 0x07 },
		.args_size = 2,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_TEAR_ON,
		.args = 0x00,
		.args_size = 0,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_INVERSE_ON,
		.args = 0x00,
		.args_size = 0,
		.delay_ms = 0,
	},
	{
		.cmd = GC9A01_SLEEP_OUT,
		.args = 0x00,
		.args_size = 0,
		.delay_ms = 120,
	},
	{
		.cmd = GC9A01_DISPLAY_ON,
		.args = 0x00,
		.args_size = 0,
		.delay_ms = 20,
	},
};

// Initialize the display driver of the touchscreen
// by setting the correct pins as inputs and/or pullup
void	gc9a01_init()
{
	// SCK and MOSI are initialized by spi_init()

	// Set screen output pins
	set_pin_as_output(&SCREEN_DDR, SCREEN_SS);
	set_pin_as_output(&SCREEN_DDR, SCREEN_DC);
	set_pin_as_output(&SCREEN_DDR, SCREEN_RST);

	set_pin_high(&SCREEN_PORT, SCREEN_SS);
	set_pin_high(&SCREEN_PORT, SCREEN_RST);
	set_pin_high(&SCREEN_PORT, SCREEN_DC);

	// gc9a01_execute_sequence(&gc9a01_init_sequence[0]);
}

void	gc9a01_send_command(gc9a01_cmd_t cmd)
{
	set_pin_low(&SCREEN_PORT, SCREEN_DC);
	set_pin_low(&SCREEN_PORT, SCREEN_SS);

	spi_send_data(cmd);

	set_pin_high(&SCREEN_PORT, SCREEN_SS);
}

void	gc9a01_send_parameter(uint8_t param)
{
	set_pin_high(&SCREEN_PORT, SCREEN_DC);
	set_pin_low(&SCREEN_PORT, SCREEN_SS);

	spi_send_data(param);

	set_pin_high(&SCREEN_PORT, SCREEN_SS);
}

void	gc9a01_execute_sequence(const gc9a01_sequence_t* sequence)
{
	set_pin_low(&SCREEN_PORT, SCREEN_SS);
	set_pin_low(&SCREEN_PORT, SCREEN_DC);
	spi_send_data(sequence->cmd);

	if (sequence->args_size)
	{
		set_pin_high(&SCREEN_PORT, SCREEN_DC);
		for (uint8_t i = 0; i < sequence->args_size; i++)
		{
			spi_send_data(sequence->args[i]);
		}
	}

	set_pin_high(&SCREEN_PORT, SCREEN_SS);
}

void	gc9a01_set_display_size(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
	gc9a01_send_command(GC9A01_SET_COL_ADDR);
	gc9a01_send_parameter(left >> 8);
	gc9a01_send_parameter(left & 0xFF);
	gc9a01_send_parameter(right >> 8);
	gc9a01_send_parameter(right & 0xFF);

	gc9a01_send_command(GC9A01_SET_ROW_ADDR);
	gc9a01_send_parameter(top >> 8);
	gc9a01_send_parameter(top & 0xFF);
	gc9a01_send_parameter(bottom >> 8);
	gc9a01_send_parameter(bottom & 0xFF);
}


void	gc9a01_send_pixel(uint8_t x, uint8_t y, uint16_t rgb256)
{
	gc9a01_set_display_size(x, y, x, y);
	gc9a01_send_command(GC9A01_MEM_WRITE);
	gc9a01_send_parameter(rgb256 >> 8);
	gc9a01_send_parameter(rgb256 & 0xFF);
}

void	gc9a01_send_square(uint8_t x, uint8_t y, uint16_t rgb256, uint16_t width)
{
	gc9a01_set_display_size(x - width, y - width, x + width, y + width);
	gc9a01_send_command(GC9A01_MEM_WRITE);

	set_pin_high(&SCREEN_PORT, SCREEN_DC);
	set_pin_low(&SCREEN_PORT, SCREEN_SS);
	uint8_t high = rgb256 >> 8;
    uint8_t low  = rgb256 & 0xFF;
	uint32_t size = width * width * 4;
	for (uint32_t i = 0; i < size; i++)
	{
		spi_send_data(high);
		spi_send_data(low);
	}
	set_pin_high(&SCREEN_PORT, SCREEN_SS);

	// gc9a01_send_command(GC9A01_DISPLAY_ON);
}

void gc9a01_fill_screen(uint16_t rgb565)
{
	gc9a01_set_display_size(0, 0, TFT_WIDTH, TFT_HEIGHT);

	gc9a01_send_command(GC9A01_MEM_WRITE);

	set_pin_high(&SCREEN_PORT, SCREEN_DC);
	set_pin_low(&SCREEN_PORT, SCREEN_SS);

    uint8_t high = rgb565 >> 8;
    uint8_t low  = rgb565 & 0xFF;

	uint32_t size = TFT_WIDTH * TFT_HEIGHT;

	for (uint32_t i = 0; i < size; i++)
	{
		spi_send_data(high);
		spi_send_data(low);
	}

	set_pin_high(&SCREEN_PORT, SCREEN_SS);

	gc9a01_send_command(GC9A01_DISPLAY_ON);
}



