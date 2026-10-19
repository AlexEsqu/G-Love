#include "touchscreen.h"
#include "main.h"
#include "math.h"

static uint16_t rgb_to_565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((uint16_t)(b & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (r >> 3);
}

static uint16_t hsv_to_565(uint8_t hue, uint8_t sat)
{
	uint8_t section = hue / 40;
	uint8_t rise = (uint8_t)((hue % 40) * 255 / 40);
	uint8_t fall = 255 - rise;

	uint8_t r, g, b;

	switch (section)
	{
		case 0:
			r = 255;
			g = rise;
			b = 0;
			break;
		case 1:
			r = fall;
			g = 255;
			b = 0;
			break;
		case 2:
			r = 0;
			g = 255;
			b = rise;
			break;
		case 3:
			r = 0;
			g = fall;
			b = 255;
			break;
		case 4:
			r = rise;
			g = 0;
			b = 255;
			break;
		default:
			r = 255;
			g = 0;
			b = fall;
			break;
	}

	// blend base hue with white at the center
	r = 255 - (((uint16_t)(255 - r) * sat) / 255);
	g = 255 - (((uint16_t)(255 - g) * sat) / 255);
	b = 255 - (((uint16_t)(255 - b) * sat) / 255);

	return rgb_to_565(r, g, b);
}


// Approximating a square root without floats
static uint8_t fast_isqrt(uint16_t val)
{
	uint16_t result = 0;
	uint16_t bit = 1 << 14;
	while (bit > val) bit >>= 2;
	while (bit != 0)
	{
		if (val >= result + bit)
		{
			val -= result + bit;
			result = (result >> 1) + bit;
		}
		else
		{
			result >>= 1;
		}
		bit >>= 2;
	}

	return (uint8_t)result;
}

// Approximating angles on 240
static uint8_t fast_atan2_to_hue(int8_t y, int8_t x)
{
	if (x == 0 && y == 0)
		return 0;

	int32_t abs_x = x < 0 ? -x : x;
	int32_t abs_y = y < 0 ? -y : y;
	int32_t ratio = (abs_x + abs_y == 0) ? 0
		: (abs_y * 60) / (abs_x + abs_y);

	uint8_t hue = 0;
	if (x >= 0 && y >= 0)
		hue = ratio;
	else if (x < 0 && y >= 0)
		hue = 120 - ratio;
	else if (x < 0 && y < 0)
		hue = 120 + ratio;
	else
		hue = 240 - ratio;

	if (hue >= 240)
		hue -= 240;

	return hue;
}

#define COLOR_WHEEL_CENTER 120
#define COLOR_WHEEL_RADIUS 120

uint16_t	get_color_from_colorscreen(uint16_t row, uint16_t col)
{
	int8_t x = (int8_t)((int16_t)col - COLOR_WHEEL_CENTER);
	int8_t y = (int8_t)((int16_t)row - COLOR_WHEEL_CENTER);

	int16_t dist_sq = (int16_t)(x * x) + (int16_t)(y * y);
	// if (dist_sq >= COLOR_WHEEL_RADIUS * COLOR_WHEEL_RADIUS)
	// 	return RGB16_BLACK;

	uint16_t r = fast_isqrt(dist_sq);
	uint8_t sat = (uint8_t)(((uint16_t)r * 255UL) / COLOR_WHEEL_RADIUS);

	uint8_t hue = fast_atan2_to_hue(y, x);

	return hsv_to_565(hue, sat);
}
uint16_t	get_color_from_colorscreen_fast(uint16_t row, uint16_t col)
{
	uint16_t sample_row = row + 2 < TFT_HEIGHT ? row + 2 : row;
	uint16_t sample_col = col + 2 < TFT_WIDTH ? col + 2 : col;

	return get_color_from_colorscreen(sample_row, sample_col);
}

void		print_color_touch_info(tft_touch_info_t* touch_info)
{
	uint32_t	full_color = rgb565_to_rgb888(touch_info->rgb565);

	putstr_screen(180, 110, "Color", RGB16_WHITE, FONT_SIZE_SMALL);
	uint8_t	pointer = putstr_screen(180, 120, "r: ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(pointer, 120, "0x", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = puthex_screen(pointer, 120, (full_color >> 11) & 0x1F, RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(180, 130, "b: ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(pointer, 130, "0x", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = puthex_screen(pointer, 130, (full_color >> 5) & 0x3F, RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(180, 140, "g: ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(pointer, 140, "0x", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = puthex_screen(pointer, 140, (full_color & 0x1F), RGB16_WHITE, FONT_SIZE_SMALL);
}

void		print_colorscreen_text(void)
{
	putstr_screen(20, 80, "Color", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 100, "picker", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 120, "mode", RGB16_BLACK, FONT_SIZE_MID);

	print_nav_info(RGB16_BLACK);
	print_touch_info(&g_last_touch);
	print_color_touch_info(&g_last_touch);
}

void		on_touch_on_colorscreen()
{
	print_color_touch_info(&g_last_touch);
	gc9a01_send_square(30, 155, RGB16_BLACK, 12);
	gc9a01_send_square(30, 155, g_last_touch.rgb565, 10);
}
