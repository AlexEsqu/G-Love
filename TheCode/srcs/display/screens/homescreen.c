#include "touchscreen.h"
#include "main.h"

static int	is_spoke(int16_t x, int16_t y)
{
	return ((y > -2 && y < 2) || (x > -2 && x < 2));
}

static int	is_homeframe( uint16_t row, uint16_t col)
{
	int16_t	y = (int16_t)col - 120;
	int16_t	x = (int16_t)row - 120;
	return is_screen_frame(x, y) || is_spoke(x, y);
}

uint16_t	get_color_from_homescreen(uint16_t row, uint16_t col)
{
	uint16_t color = RGB16_BLACK;
	if (is_homeframe(row, col))
	{
		color = RGB16_GREEN;
	}
	else if (col < 120 && row < 120)
	{
		color = RGB16_GOLD;
	}
	else if (col >= 120 && row < 120)
	{
		color = RGB16_GRAY;
	}
	else if (col < 120 && row >= 120)
	{
		color = RGB16_NAVY;
	}
	return color;
}

void		print_homescreen_text(void)
{
	putstr_screen(75, 30, "Home", RGB16_WHITE, FONT_SIZE_BIG);

	print_touch_info(&g_last_touch);
}


void		on_touch_on_homescreen()
{

}


