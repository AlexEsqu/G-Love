#include "touchscreen.h"
#include "main.h"

// static int is_diagonal_spoke(int16_t x, int16_t y)
// {
//     int16_t diff = x - y;
//     int16_t sum = x + y;
//     return ((diff > -6 && diff < 6) || (sum > -6 && sum < 6));
// }

static int is_center_circle(int16_t x, int16_t y, uint16_t radius)
{
    return (x * x + y * y <= (int32_t)radius * radius);
}

uint16_t get_color_from_imuscreen(uint16_t row, uint16_t col)
{
	int16_t y = (int16_t)col - 120;
	int16_t x = (int16_t)row - 120;

	if (is_center_circle(x, y, 20))
		return RGB16_GOLD;

	if (is_screen_frame(x, y))
		return RGB16_GRAY;

	// if (is_diagonal_spoke(row, col))
	// 	return RGB16_BLACK;

	int16_t abs_x = x < 0 ? -x : x;
	int16_t abs_y = y < 0 ? -y : y;

	if (x < -abs_y)
		return RGB16_GOLD;
	else if (x > abs_y)
		return RGB16_GOLD;
	else if (y > abs_x)
		return RGB16_BLACK;
	else
		return RGB16_BLACK;
}

void		print_imuscreen_text(void)
{
	putstr_screen(20, 90, "IMU", RGB16_WHITE, FONT_SIZE_MID);
	putstr_screen(20, 110, "Fly", RGB16_WHITE, FONT_SIZE_MID);
	putstr_screen(20, 130, "mode", RGB16_WHITE, FONT_SIZE_MID);
}
