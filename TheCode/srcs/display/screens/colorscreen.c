#include "touchscreen.h"
#include "main.h"

static uint16_t rgb_to_565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((uint16_t)(b & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (r >> 3);
}

static uint16_t hue_to_565(uint8_t hue)
{
	uint8_t section = hue / 40;
	uint8_t rise = (uint8_t)((hue % 40) * 255 / 40);
	uint8_t fall = 255 - rise;

	switch (section)
	{
		case 0:
			return rgb_to_565(255, rise, 0);
		case 1:
			return rgb_to_565(fall, 255, 0);
		case 2:
			return rgb_to_565(0, 255, rise);
		case 3:
			return rgb_to_565(0, fall, 255);
		case 4:
			return rgb_to_565(rise, 0, 255);
		default:
			return rgb_to_565(255, 0, fall);
	}
}

uint16_t	get_color_from_colorscreen(uint16_t row, uint16_t col)
{
	return (hue_to_565((uint8_t)((col + row) >> 1)));
}

void		print_colorscreen_text(void)
{
	putstr_screen(20, 80, "Color", RGB16_BLUE, FONT_SIZE_MID);
	putstr_screen(20, 100, "picker", RGB16_BLUE, FONT_SIZE_MID);
	putstr_screen(20, 120, "mode", RGB16_BLUE, FONT_SIZE_MID);

	putstr_screen(100, 200, "swipe left", RGB16_BLUE, FONT_SIZE_SMALL);
	putstr_screen(70, 210, "to go back to menu", RGB16_BLUE, FONT_SIZE_SMALL);
}
