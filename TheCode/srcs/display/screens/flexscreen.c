#include "touchscreen.h"
#include "main.h"

enum
{
	FLEX_THUMB,
	FLEX_INDEX,
	FLEX_MIDDLE,
	FLEX_RING,
	FLEX_PINKY,
	FLEX_FINGER_COUNT
};

typedef struct
{
	uint16_t row_min;
	uint16_t row_max;
	uint16_t col_min;
	uint16_t col_max;
} flex_finger_area_t;

static const flex_finger_area_t g_finger_areas[FLEX_FINGER_COUNT] =
{
	[FLEX_THUMB] = { 135, 205, 15, 80 },
	[FLEX_INDEX] = { 30, 90, 65, 95 },
	[FLEX_MIDDLE] = { 10, 90, 95, 122 },
	[FLEX_RING] = { 20, 100, 124, 147 },
	[FLEX_PINKY] = { 40, 107, 147, 170 }
};

// plug into fsr values
static uint8_t g_finger_red[FLEX_FINGER_COUNT] = { 255, 20, 0, 255, 0 };

static int8_t get_finger_index(uint16_t row, uint16_t col)
{
	for (uint8_t finger = 0; finger < FLEX_FINGER_COUNT; finger++)
	{
		const flex_finger_area_t *area = &g_finger_areas[finger];
		if (row >= area->row_min && row <= area->row_max
			&& col >= area->col_min && col <= area->col_max)
			return (int8_t)finger;
	}
	return -1;
}

static uint16_t blend_blue_with_red(uint8_t red_level)
{
	uint8_t red = (uint8_t)(((uint16_t)31 * red_level) / 255);
	uint8_t blue = (uint8_t)(((uint16_t)31 * (255 - red_level)) / 255);

	return ((uint16_t)red << 11) | blue;
}

static uint8_t get_finger_red_level(uint8_t finger, uint16_t row, uint16_t col)
{
	const flex_finger_area_t *area = &g_finger_areas[finger];
	uint16_t position;
	uint16_t length;

	if (finger == FLEX_THUMB)
	{
		position = area->col_max - col;
		length = area->col_max - area->col_min;
	}
	else
	{
		position = row - area->row_min;
		length = area->row_max - area->row_min;
	}

	if (length == 0)
		return g_finger_red[finger];

	uint16_t gradient_length = (length + 2) / 2;
	if (finger != FLEX_THUMB)
	{
		uint16_t red_length = length - gradient_length;
		if (position <= red_length)
			return g_finger_red[finger];
		position -= red_length;
		return (uint8_t)(((uint16_t)g_finger_red[finger]
			* (gradient_length - position)) / gradient_length);
	}
	if (position >= gradient_length)
	{
		return g_finger_red[finger];
	}

	return (uint8_t)(((uint16_t)g_finger_red[finger] * position)
		/ gradient_length);
}


void		print_nav_info_flex(uint16_t color)
{
	putstr_screen(118, 200, "swipe", color, FONT_SIZE_SMALL);
	putstr_screen(102, 210, "to go back", color, FONT_SIZE_SMALL);
	putstr_screen(112, 220, "to menu", color, FONT_SIZE_SMALL);
}

uint16_t get_color_from_flexscreen(uint16_t row, uint16_t col)
{
	if (!is_in_mask(row, col, &flexscreen_image))
		return RGB16_BLACK;

	int8_t finger = get_finger_index(row, col);
	if (finger >= 0)
		return blend_blue_with_red(get_finger_red_level((uint8_t)finger, row, col));

	return RGB16_BLUE;
}

void		print_flexscreen_text(void)
{
	putstr_screen(20, 80, "Flex", RGB16_WHITE, FONT_SIZE_MID);
	putstr_screen(20, 100, "mode", RGB16_WHITE, FONT_SIZE_MID);

	print_nav_info_flex(RGB16_WHITE);
	// print_touch_info(&g_last_touch);
}

void		on_touch_on_flexscreen()
{

}
