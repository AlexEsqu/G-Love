#include "touchscreen.h"
#include "main.h"

// static int is_diagonal_spoke(int16_t x, int16_t y)
// {
//     int16_t diff = x - y;
//     int16_t sum = x + y;
//     return ((diff > -6 && diff < 6) || (sum > -6 && sum < 6));
// }

// static int is_center_circle(int16_t x, int16_t y, uint16_t radius)
// {
//     return (x * x + y * y <= (int32_t)radius * radius);
// }

uint16_t get_color_from_imuscreen(uint16_t row, uint16_t col)
{
	int16_t y = (int16_t)col - 120;
	int16_t x = (int16_t)row - 120;

	int16_t abs_x = x < 0 ? -x : x;
	if (y > abs_x + 20 || y < -abs_x - 20)
	{
		return RGB16_GOLD;
	}
	return RGB16_BLACK;
}

void		print_imuscreen_text(void)
{
	putstr_screen(20, 90, "IMU", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 110, "Fly", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 130, "mode", RGB16_BLACK, FONT_SIZE_MID);

	print_touch_info(&g_last_touch);
	print_nav_info(RGB16_GOLD);
}

void		on_touch_on_imuscreen()
{

}
