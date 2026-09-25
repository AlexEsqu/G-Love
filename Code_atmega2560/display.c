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


void	gc9a01_send_init_sequence()
{

	// wake up lil driver
	gc9a01_send_command(GC9A01_SLEEP_OUT);
	_delay_ms(120);

	gc9a01_send_command(GC9A01_COLMOD);
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
    // 0x05 = RGB565, 16 bits/pixel

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

	// pullup external pin to "1111" for 4-line serial interface - p.48
	set_pin_up(&SCREEN_PORT, SCREEN_SS);
	set_pin_up(&SCREEN_PORT, SCREEN_RST);
	set_pin_up(&SCREEN_PORT, SCREEN_DC);

	gc9a01_send_init_sequence();
}

void	gc9a01_send_command(GC9A01_cmd_t cmd)
{
	set_pin_low(&SCREEN_PORT, SCREEN_DC);
	set_pin_low(&SCREEN_PORT, SCREEN_SS);

	spi_send_data(cmd);

	set_pin_up(&SCREEN_PORT, SCREEN_SS);
}

void	gc9a01_send_parameter(uint8_t param)
{
	set_pin_up(&SCREEN_PORT, SCREEN_DC);
	set_pin_low(&SCREEN_PORT, SCREEN_SS);

	spi_send_data(param);

	set_pin_up(&SCREEN_PORT, SCREEN_SS);
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


void gc9a01_fill_screen(uint32_t rgb)
{
	gc9a01_set_display_size(0, 0, TFT_WIDTH, TFT_HEIGHT);

	gc9a01_send_command(GC9A01_MEM_WRITE);

	set_pin_up(&SCREEN_PORT, SCREEN_DC);
	set_pin_low(&SCREEN_PORT, SCREEN_SS);

	uint8_t	red = rgb >> 16;
	uint8_t	green = rgb >> 8;
	uint8_t	blue = rgb & 0xFF;

	uint16_t rgb565 =
        ((red   & 0xF8) << 8) |
        ((green & 0xFC) << 3) |
        ((blue  & 0xF8) >> 3);

    uint8_t high = rgb565 >> 8;
    uint8_t low  = rgb565 & 0xFF;

	uint32_t size = TFT_WIDTH * TFT_HEIGHT;

	for (uint32_t i = 0; i < size; i++)
	{
		spi_send_data(high);
		spi_send_data(low);
	}

	set_pin_up(&SCREEN_PORT, SCREEN_SS);

	gc9a01_send_command(GC9A01_DISPLAY_ON);
}
