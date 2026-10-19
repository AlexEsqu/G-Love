#include "touchscreen.h"
#include "main.h"

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

void		print_imu_info()
{
	putstr_screen(180, 100, "Position", RGB16_BLACK, FONT_SIZE_SMALL);
	uint8_t	pointer = putstr_screen(180, 110, "x: ", RGB16_BLACK, FONT_SIZE_SMALL);
	pointer = putint_screen(pointer, 110, 15, RGB16_BLACK, FONT_SIZE_SMALL);
	pointer = putstr_screen(180, 120, "y: ", RGB16_BLACK, FONT_SIZE_SMALL);
	pointer = putint_screen(pointer, 120, 130, RGB16_BLACK, FONT_SIZE_SMALL);
}

void		print_imuscreen_text(void)
{
	putstr_screen(20, 90, "IMU", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 110, "Fly", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 130, "mode", RGB16_BLACK, FONT_SIZE_MID);

	putstr_screen(70, 20, "Click up here to", RGB16_GOLD, FONT_SIZE_SMALL);
	putstr_screen(67, 30, "Calibrate", RGB16_GOLD, FONT_SIZE_MID);

	print_imu_info();
	print_nav_info(RGB16_GOLD);
}

void		on_touch_on_imuscreen()
{
	uint16_t	row = g_last_touch.y;
	uint16_t	col = g_last_touch.x;

	if (is_top_quarter_of_screen(row, col))
	{
		; // TO-DO: plug cmd here for calibration
		// gc9a01_send_square(col, row, RGB16_BIO_GLOW, 4); // for sanity, to remove
	}

	print_imu_info();
}
