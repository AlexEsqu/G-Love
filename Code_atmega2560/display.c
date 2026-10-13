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

void	gc9a01_send_init_sequence()
{

	// wake up lil driver
	gc9a01_send_command(GC9A01_SLEEP_OUT);
	_delay_ms(120);

	gc9a01_send_command(GC9A01_SET_PIX_FORMAT);
	gc9a01_send_parameter(0x05);

	gc9a01_send_command(GC9A01_DISPLAY_ON);
	_delay_ms(20);


	// Inter-register enable
    gc9a01_send_command(GC9A01_INREGEN1);

    gc9a01_send_command(0xEB);
    gc9a01_send_parameter(0x14);

    gc9a01_send_command(GC9A01_INREGEN1);

    gc9a01_send_command(GC9A01_INREGEN2);

    gc9a01_send_command(0xEB);
    gc9a01_send_parameter(0x14);


    // Power / internal registers

    gc9a01_send_command(0x84);
    gc9a01_send_parameter(0x40);

    gc9a01_send_command(0x85);
    gc9a01_send_parameter(0xFF);

    gc9a01_send_command(0x86);
    gc9a01_send_parameter(0xFF);

    gc9a01_send_command(0x87);
    gc9a01_send_parameter(0xFF);

    gc9a01_send_command(0x88);
    gc9a01_send_parameter(0x0A);

    gc9a01_send_command(0x89);
    gc9a01_send_parameter(0x21);

    gc9a01_send_command(0x8A);
    gc9a01_send_parameter(0x00);

    gc9a01_send_command(0x8B);
    gc9a01_send_parameter(0x80);

    gc9a01_send_command(0x8C);
    gc9a01_send_parameter(0x01);

    gc9a01_send_command(0x8D);
    gc9a01_send_parameter(0x01);

    gc9a01_send_command(0x8E);
    gc9a01_send_parameter(0xFF);

    gc9a01_send_command(0x8F);
    gc9a01_send_parameter(0xFF);


    // Display function control

    gc9a01_send_command(0xB6);

    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0x20);


    // Memory access control
    // 0x08 = RGB ordering, vertical scan

    gc9a01_send_command(0x36);
    gc9a01_send_parameter(0x08);


    // Pixel format
    // 0x05 = RGB 16 bits/pixel

    gc9a01_send_command(0x3A);
    gc9a01_send_parameter(0x05);


    // Internal register

    gc9a01_send_command(0x90);

    gc9a01_send_parameter(0x08);
    gc9a01_send_parameter(0x08);
    gc9a01_send_parameter(0x08);
    gc9a01_send_parameter(0x08);


    gc9a01_send_command(0xBD);
    gc9a01_send_parameter(0x06);

    gc9a01_send_command(0xBC);
    gc9a01_send_parameter(0x00);


    // Internal register

    gc9a01_send_command(0xFF);

    gc9a01_send_parameter(0x60);
    gc9a01_send_parameter(0x01);
    gc9a01_send_parameter(0x04);


    // Power control

    gc9a01_send_command(0xC3);
    gc9a01_send_parameter(0x13);

    gc9a01_send_command(0xC4);
    gc9a01_send_parameter(0x13);

    gc9a01_send_command(0xC9);
    gc9a01_send_parameter(0x22);

    gc9a01_send_command(0xBE);
    gc9a01_send_parameter(0x11);


    // Internal timing

    gc9a01_send_command(0xE1);

    gc9a01_send_parameter(0x10);
    gc9a01_send_parameter(0x0E);


    gc9a01_send_command(0xDF);

    gc9a01_send_parameter(0x21);
    gc9a01_send_parameter(0x0C);
    gc9a01_send_parameter(0x02);


    // Positive gamma

    gc9a01_send_command(0xF0);

    gc9a01_send_parameter(0x45);
    gc9a01_send_parameter(0x09);
    gc9a01_send_parameter(0x08);
    gc9a01_send_parameter(0x08);
    gc9a01_send_parameter(0x26);
    gc9a01_send_parameter(0x2A);


    // Negative gamma

    gc9a01_send_command(0xF1);

    gc9a01_send_parameter(0x43);
    gc9a01_send_parameter(0x70);
    gc9a01_send_parameter(0x72);
    gc9a01_send_parameter(0x36);
    gc9a01_send_parameter(0x37);
    gc9a01_send_parameter(0x6F);


    // Positive gamma 2

    gc9a01_send_command(0xF2);

    gc9a01_send_parameter(0x45);
    gc9a01_send_parameter(0x09);
    gc9a01_send_parameter(0x08);
    gc9a01_send_parameter(0x08);
    gc9a01_send_parameter(0x26);
    gc9a01_send_parameter(0x2A);


    // Negative gamma 2

    gc9a01_send_command(0xF3);

    gc9a01_send_parameter(0x43);
    gc9a01_send_parameter(0x70);
    gc9a01_send_parameter(0x72);
    gc9a01_send_parameter(0x36);
    gc9a01_send_parameter(0x37);
    gc9a01_send_parameter(0x6F);


    // Internal registers

    gc9a01_send_command(0xED);

    gc9a01_send_parameter(0x1B);
    gc9a01_send_parameter(0x0B);


    gc9a01_send_command(0xAE);
    gc9a01_send_parameter(0x77);

    gc9a01_send_command(0xCD);
    gc9a01_send_parameter(0x63);


    // Internal register

    gc9a01_send_command(0x70);

    gc9a01_send_parameter(0x07);
    gc9a01_send_parameter(0x07);
    gc9a01_send_parameter(0x04);
    gc9a01_send_parameter(0x0E);
    gc9a01_send_parameter(0x0F);
    gc9a01_send_parameter(0x09);
    gc9a01_send_parameter(0x07);
    gc9a01_send_parameter(0x08);
    gc9a01_send_parameter(0x03);


    // Frame rate

    gc9a01_send_command(0xE8);
    gc9a01_send_parameter(0x34);


    // Internal register 0x62

    gc9a01_send_command(0x62);

    gc9a01_send_parameter(0x18);
    gc9a01_send_parameter(0x0D);
    gc9a01_send_parameter(0x71);
    gc9a01_send_parameter(0xED);
    gc9a01_send_parameter(0x70);
    gc9a01_send_parameter(0x70);
    gc9a01_send_parameter(0x18);
    gc9a01_send_parameter(0x0F);
    gc9a01_send_parameter(0x71);
    gc9a01_send_parameter(0xEF);
    gc9a01_send_parameter(0x70);
    gc9a01_send_parameter(0x70);


    // Internal register 0x63

    gc9a01_send_command(0x63);

    gc9a01_send_parameter(0x18);
    gc9a01_send_parameter(0x11);
    gc9a01_send_parameter(0x71);
    gc9a01_send_parameter(0xF1);
    gc9a01_send_parameter(0x70);
    gc9a01_send_parameter(0x70);
    gc9a01_send_parameter(0x18);
    gc9a01_send_parameter(0x13);
    gc9a01_send_parameter(0x71);
    gc9a01_send_parameter(0xF3);
    gc9a01_send_parameter(0x70);
    gc9a01_send_parameter(0x70);


    // Internal register 0x64

    gc9a01_send_command(0x64);

    gc9a01_send_parameter(0x28);
    gc9a01_send_parameter(0x29);
    gc9a01_send_parameter(0xF1);
    gc9a01_send_parameter(0x01);
    gc9a01_send_parameter(0xF1);
    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0x07);


    // Internal register 0x66

    gc9a01_send_command(0x66);

    gc9a01_send_parameter(0x3C);
    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0xCD);
    gc9a01_send_parameter(0x67);
    gc9a01_send_parameter(0x45);
    gc9a01_send_parameter(0x45);
    gc9a01_send_parameter(0x10);
    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0x00);


    // Internal register 0x67

    gc9a01_send_command(0x67);

    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0x3C);
    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0x01);
    gc9a01_send_parameter(0x54);
    gc9a01_send_parameter(0x10);
    gc9a01_send_parameter(0x32);
    gc9a01_send_parameter(0x98);


    // Internal register 0x74

    gc9a01_send_command(0x74);

    gc9a01_send_parameter(0x10);
    gc9a01_send_parameter(0x85);
    gc9a01_send_parameter(0x80);
    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0x00);
    gc9a01_send_parameter(0x4E);
    gc9a01_send_parameter(0x00);


    // Internal register 0x98

    gc9a01_send_command(0x98);

    gc9a01_send_parameter(0x3E);
    gc9a01_send_parameter(0x07);


    // Tearing effect ON

    gc9a01_send_command(0x35);


    // Inversion ON

    gc9a01_send_command(0x21);

    // Sleep Out

    gc9a01_send_command(0x11);
    _delay_ms(120);

    // Display ON

    gc9a01_send_command(0x29);
    _delay_ms(20);
}

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

	gc9a01_send_init_sequence();
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
	gc9a01_set_display_size(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);

	gc9a01_send_command(GC9A01_MEM_WRITE);

	set_pin_high(&SCREEN_PORT, SCREEN_DC);
	set_pin_low(&SCREEN_PORT, SCREEN_SS);

	uint32_t size = TFT_PIXEL_COUNT;

	for (uint32_t i = 0; i < size; i++)
	{
		spi_send_data(rgb565 >> 8);
		spi_send_data(rgb565 & 0xFF);
	}

	set_pin_high(&SCREEN_PORT, SCREEN_SS);

	gc9a01_send_command(GC9A01_DISPLAY_ON);
}

void gc9a01_display_homescreen()
{
	gc9a01_set_display_size(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);

	gc9a01_send_command(GC9A01_MEM_WRITE);

	set_pin_high(&SCREEN_PORT, SCREEN_DC);
	set_pin_low(&SCREEN_PORT, SCREEN_SS);

	// uint32_t row = 0;
	// uint32_t columm = 0;

	for (uint16_t row = 0; row < TFT_HEIGHT; row++)
	{
		for (uint16_t column = 0; column < TFT_WIDTH; column++)
		{
			int16_t x = column - 120;
			int16_t y = row - 120;

			if (x > -10 && x < 10 && y > -10 && y < 10)
			{
				spi_send_data(RGB16_RED >> 8);
				spi_send_data(RGB16_RED & 0xFF);
			}
			else
			{
				spi_send_data(RGB16_BLUE >> 8);
				spi_send_data(RGB16_BLUE & 0xFF);
			}
		}
	}

	set_pin_high(&SCREEN_PORT, SCREEN_SS);

	gc9a01_send_command(GC9A01_DISPLAY_ON);
}



