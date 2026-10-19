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
	uint8_t	pointer = putstr_screen(180, 110, "Gyro: ", RGB16_BLACK, FONT_SIZE_SMALL);
	pointer = putint_screen(pointer, 110, g_packet_to_export.gyro[0], RGB16_BLACK, FONT_SIZE_SMALL);
	pointer = putstr_screen(180, 120, "Accel: ", RGB16_BLACK, FONT_SIZE_SMALL);
	pointer = putint_screen(pointer, 120,  g_packet_to_export.accel[0], RGB16_BLACK, FONT_SIZE_SMALL);
}

void		print_imuscreen_text(void)
{
	putstr_screen(20, 90, "IMU", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 110, "Fly", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 130, "mode", RGB16_BLACK, FONT_SIZE_MID);

	putstr_screen(70, 20, "Click up here to", RGB16_GOLD, FONT_SIZE_SMALL);
	putstr_screen(55, 30, " Calibrate  ", RGB16_GOLD, FONT_SIZE_MID);

	print_imu_info();
	print_nav_info(RGB16_GOLD);
}

void		on_touch_on_imuscreen()
{
	uint16_t	row = g_packet_to_export.last_touch.y;
	uint16_t	col = g_packet_to_export.last_touch.x;

	if (is_top_quarter_of_screen(row, col))
	{
		g_love_directive.request_reset_imu = 1;
		putstr_screen(55, 30, "Calibrating", RGB16_GOLD, FONT_SIZE_MID);
		send_g_love_directive();
		g_love_directive.request_reset_imu = 0;
	}

	print_imu_info();
}
