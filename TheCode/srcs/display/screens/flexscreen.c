#include "touchscreen.h"
#include "main.h"


uint16_t get_color_from_flexscreen(uint16_t row, uint16_t col)
{
	int16_t y = (int16_t)col - 120;
	int16_t x = (int16_t)row - 120;

	// if (is_center_circle(x, y, 20))
	// 	return RGB16_GOLD;

	// if (is_screen_frame(x, y))
	// 	return RGB16_GRAY;

	// if (is_diagonal_spoke(row, col))
	// 	return RGB16_BLACK;

	int16_t abs_x = x < 0 ? -x : x;
	int16_t abs_y = y < 0 ? -y : y;

	if (x < -abs_y)
		return RGB16_BLUE;
	else if (x > abs_y)
		return RGB16_BLUE;
	else if (y > abs_x)
		return RGB16_BLACK;
	else
		return RGB16_BLACK;
}

void		print_flexscreen_text(void)
{
	putstr_screen(20, 90, "Flex", RGB16_WHITE, FONT_SIZE_MID);
	putstr_screen(20, 110, "Force", RGB16_WHITE, FONT_SIZE_MID);
	putstr_screen(20, 130, "mode", RGB16_WHITE, FONT_SIZE_MID);

	print_nav_info(RGB16_BLACK);
	print_touch_info(&g_last_touch);
}

void		on_touch_on_flexscreen()
{

}
