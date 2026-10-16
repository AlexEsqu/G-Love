#include "touchscreen.h"
#include "main.h"

void		print_imuscreen_text(void)
{
	putstr_screen(20, 80, "IMU", RGB16_BLUE, FONT_SIZE_MID);
	putstr_screen(20, 100, "Fly", RGB16_BLUE, FONT_SIZE_MID);
	putstr_screen(20, 120, "mode", RGB16_BLUE, FONT_SIZE_MID);
}
