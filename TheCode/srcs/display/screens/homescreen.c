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
	return is_spoke(x, y);
}

static inline int	is_in_top_left_quarter(uint16_t row, uint16_t col)
{
	return (col < 120 && row < 120);
}

static inline int	is_in_top_right_quarter(uint16_t row, uint16_t col)
{
	return (col >= 120 && row < 120);
}

static inline int	is_in_bottom_right_quarter(uint16_t row, uint16_t col)
{
	return (col < 120 && row >= 120);
}

static inline int	is_in_bottom_left_quarter(uint16_t row, uint16_t col)
{
	return (col >= 120 && row >= 120);
}

uint16_t	get_color_from_homescreen(uint16_t row, uint16_t col)
{
	uint16_t color = RGB16_ALIVE_AMBER;
	if (is_homeframe(row, col))
	{
		color = RGB16_BLACK;
	}
	else if (is_in_top_left_quarter(row, col))
	{
		color = RGB16_PLASMA_BURN;
	}
	else if (is_in_top_right_quarter(row, col))
	{
		color = RGB16_FOREST_FLOOR;
	}
	else if (is_in_bottom_right_quarter(row, col))
	{
		color = RGB16_CYBER_SAPPHIRE;
	}
	return color;
}

void		print_homescreen_text(void)
{
	putstr_screen(40, 105, "Com Mode", RGB16_WHITE, FONT_SIZE_SMALL);
	putstr_screen(150, 105, "IMU Mode", RGB16_WHITE, FONT_SIZE_SMALL);
	putstr_screen(40, 130, "Flex Mode", RGB16_WHITE, FONT_SIZE_SMALL);
	putstr_screen(150, 130, "Color Mode", RGB16_WHITE, FONT_SIZE_SMALL);

	print_touch_info(&g_last_touch);
}


void		on_touch_on_homescreen()
{
	uint16_t	row = g_last_touch.x;
	uint16_t	col = g_last_touch.y;

	if (is_in_top_left_quarter(row, col))
	{
		g_touchscreen_mode = TFT_STATE_COMMSCREEN;
	}
	else if (is_in_top_right_quarter(row, col))
	{
		g_touchscreen_mode = TFT_STATE_FLEXSCREEN;
	}
	else if (is_in_bottom_right_quarter(row, col))
	{
		g_touchscreen_mode = TFT_STATE_IMUSCREEN;
	}
	else if (is_in_bottom_left_quarter(row, col))
	{
		g_touchscreen_mode = TFT_STATE_COLORSCREEN;
	}
	refresh_screen();
}


