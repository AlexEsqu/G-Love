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
        case 0: r = 255; g = rise; b = 0;   break;
        case 1: r = fall; g = 255; b = 0;   break;
        case 2: r = 0;   g = 255; b = rise; break;
        case 3: r = 0;   g = fall; b = 255; break;
        case 4: r = rise; g = 0;   b = 255; break;
        default:r = 255; g = 0;   b = fall; break;
    }

    // Blend base hue with white at the center
    r = 255 - (((uint16_t)(255 - r) * sat) / 255);
    g = 255 - (((uint16_t)(255 - g) * sat) / 255);
    b = 255 - (((uint16_t)(255 - b) * sat) / 255);

    return rgb_to_565(r, g, b);
}

// static uint16_t hue_to_565(uint8_t hue)
// {
// 	uint8_t section = hue / 40;
// 	uint8_t rise = (uint8_t)((hue % 40) * 255 / 40);
// 	uint8_t fall = 255 - rise;

// 	switch (section)
// 	{
// 		case 0:
// 			return rgb_to_565(255, rise, 0);
// 		case 1:
// 			return rgb_to_565(fall, 255, 0);
// 		case 2:
// 			return rgb_to_565(0, 255, rise);
// 		case 3:
// 			return rgb_to_565(0, fall, 255);
// 		case 4:
// 			return rgb_to_565(rise, 0, 255);
// 		default:
// 			return rgb_to_565(255, 0, fall);
// 	}
// }

uint16_t	get_color_from_colorscreen(uint16_t row, uint16_t col)
{
    int dx = (int)col - 120;
    int dy = (int)row - 120;

    int dist_sq = (dx * dx) + (dy * dy);

    if (dist_sq > (120 * 120)) {
        return 0x0000;
    }

    float r = sqrtf((float)dist_sq);
    uint8_t sat = (uint8_t)(r * 255.0f / 120.0f);

    float angle = atan2f((float)dy, (float)dx);
    if (angle < 0.0f) {
        angle += 2.0f * 3.14159265f;
    }

    uint8_t hue = (uint8_t)(angle * (240.0f / (2.0f * 3.14159265f)));

    return hsv_to_565(hue, sat);
}

void		print_color_touch_info(tft_touch_info_t* touch_info)
{
	uint8_t	pointer = putstr_screen(180, 120, "r: ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(pointer, 120, "0x", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = puthex_screen(pointer, 120, (touch_info->rgb888 >> 11) & 0x1F, RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(180, 130, "b: ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(pointer, 130, "0x", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = puthex_screen(pointer, 130, (touch_info->rgb888 >> 5) & 0x3F, RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(180, 140, "g: ", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = putstr_screen(pointer, 140, "0x", RGB16_WHITE, FONT_SIZE_SMALL);
	pointer = puthex_screen(pointer, 140, (touch_info->rgb888 & 0x1F), RGB16_WHITE, FONT_SIZE_SMALL);
}

void		print_colorscreen_text(void)
{
	putstr_screen(20, 80, "Color", RGB16_BLUE, FONT_SIZE_MID);
	putstr_screen(20, 100, "picker", RGB16_BLUE, FONT_SIZE_MID);
	putstr_screen(20, 120, "mode", RGB16_BLUE, FONT_SIZE_MID);

	putstr_screen(100, 200, "swipe left", RGB16_WHITE, FONT_SIZE_SMALL);
	putstr_screen(70, 210, "to go back to menu", RGB16_WHITE, FONT_SIZE_SMALL);

	print_touch_info(&g_last_touch);
	print_color_touch_info(&g_last_touch);
}

void		on_touch_on_colorscreen()
{
	print_color_touch_info(&g_last_touch);
	gc9a01_send_square(30, 155, g_last_touch.rgb565, 10);
}
