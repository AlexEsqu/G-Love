#include "touchscreen.h"
#include "main.h"

uint16_t	get_color_from_comscreen(uint16_t row, uint16_t col)
{
	(void)row;
	(void)col;
	return (RGB16_MAGENTA);
}

// void		print_color_touch_info(tft_touch_info_t* touch_info)
// {
// 	uint8_t	pointer = putstr_screen(180, 120, "r: ", RGB16_WHITE, FONT_SIZE_SMALL);
// 	pointer = putstr_screen(pointer, 120, "0x", RGB16_WHITE, FONT_SIZE_SMALL);
// 	pointer = puthex_screen(pointer, 120, (touch_info->rgb888 >> 11) & 0x1F, RGB16_WHITE, FONT_SIZE_SMALL);
// 	pointer = putstr_screen(180, 130, "b: ", RGB16_WHITE, FONT_SIZE_SMALL);
// 	pointer = putstr_screen(pointer, 130, "0x", RGB16_WHITE, FONT_SIZE_SMALL);
// 	pointer = puthex_screen(pointer, 130, (touch_info->rgb888 >> 5) & 0x3F, RGB16_WHITE, FONT_SIZE_SMALL);
// 	pointer = putstr_screen(180, 140, "g: ", RGB16_WHITE, FONT_SIZE_SMALL);
// 	pointer = putstr_screen(pointer, 140, "0x", RGB16_WHITE, FONT_SIZE_SMALL);
// 	pointer = puthex_screen(pointer, 140, (touch_info->rgb888 & 0x1F), RGB16_WHITE, FONT_SIZE_SMALL);
// }

void		print_comscreen_text(void)
{
	putstr_screen(20, 80, "Com", RGB16_BLUE, FONT_SIZE_MID);
	putstr_screen(20, 100, "picker", RGB16_BLUE, FONT_SIZE_MID);
	putstr_screen(20, 120, "mode", RGB16_BLUE, FONT_SIZE_MID);

	print_touch_info(&g_last_touch);
	print_nav_info(RGB16_BLACK);
}

void		on_touch_on_comscreen()
{
	// print_color_touch_info(&g_last_touch);
	// gc9a01_send_square(30, 160, g_last_touch.rgb565, 10);
}
