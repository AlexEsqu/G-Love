#include "touchscreen.h"
#include "main.h"


static int is_x_spoke(uint16_t row, uint16_t col)
{
	int16_t diff1 = (int16_t)col - (int16_t)row;
	int16_t sum2  = (int16_t)col + (int16_t)row - 240;

	return ((diff1 > -2 && diff1 < 2) || (sum2 > -2 && sum2 < 2));
}

static int is_comframe(uint16_t row, uint16_t col)
{
	int16_t y = (int16_t)col - 120;
	int16_t x = (int16_t)row - 120;
	return is_screen_frame(x, y) || is_x_spoke(row, col);
}

static inline int is_in_top_quarter(uint16_t row, uint16_t col)
{
	return (row < col && (row + col) < 240);
}

static inline int is_in_right_quarter(uint16_t row, uint16_t col)
{
	return (row < col && (row + col) > 240);
}

static inline int is_in_bottom_quarter(uint16_t row, uint16_t col)
{
	return (row > col && (row + col) > 240);
}

static inline int is_in_left_quarter(uint16_t row, uint16_t col)
{
	return (row > col && (row + col) < 240);
}

uint16_t get_color_from_comscreen(uint16_t row, uint16_t col)
{
	uint16_t color = RGB16_BLACK;

	if (is_comframe(row, col))
	{
		color = RGB16_GREEN; // X-spokes and outer border
	}
	else if (is_in_top_quarter(row, col))
	{
		color = RGB16_GOLD;
	}
	else if (is_in_right_quarter(row, col))
	{
		color = RGB16_GRAY;
	}
	else if (is_in_bottom_quarter(row, col))
	{
		color = RGB16_NAVY;
	}
	else if (is_in_left_quarter(row, col))
	{
		color = RGB16_PURPLE; // Fourth region for the left triangle wedge
	}

	return color;
}


void		print_comscreen_text(void)
{
	putstr_screen(20, 80, "Com", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 100, "picker", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 120, "mode", RGB16_BLACK, FONT_SIZE_MID);

	// print_touch_info(&g_last_touch);
	print_nav_info(RGB16_BLACK);
}

void		on_touch_on_comscreen()
{
	// print_color_touch_info(&g_last_touch);
	// gc9a01_send_square(30, 160, g_last_touch.rgb565, 10);
}
