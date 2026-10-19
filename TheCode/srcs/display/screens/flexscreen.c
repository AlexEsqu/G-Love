#include "touchscreen.h"
#include "main.h"


void		print_nav_info_flex(uint16_t color)
{
	putstr_screen(118, 200, "swipe", color, FONT_SIZE_SMALL);
	putstr_screen(102, 210, "to go back", color, FONT_SIZE_SMALL);
	putstr_screen(112, 220, "to menu", color, FONT_SIZE_SMALL);
}

uint16_t get_color_from_flexscreen(uint16_t row, uint16_t col)
{
	if (is_in_mask(row, col, &flexscreen_image))
		return RGB16_BLUE;
	return RGB16_BLACK;
}

void		print_flexscreen_text(void)
{
	putstr_screen(20, 80, "Flex", RGB16_WHITE, FONT_SIZE_MID);
	putstr_screen(20, 100, "mode", RGB16_WHITE, FONT_SIZE_MID);

	print_nav_info_flex(RGB16_WHITE);
	// print_touch_info(&g_last_touch);
}

void		on_touch_on_flexscreen()
{

}
