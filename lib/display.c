#include "touchscreen.h"
#include "main.h"

int	is_circle(int16_t x, int16_t y)
{
	return (x * x + y * y) > (int32_t)TFT_FRAME;
}

int	is_spoke(int16_t x, int16_t y)
{
	return ((y > -2 && y < 2) || (x > -2 && x < 2));
}

int	is_frame( uint16_t row, uint16_t col)
{
	int16_t	y = (int16_t)col - 120;
	int16_t	x = (int16_t)row - 120;
	return is_circle(x, y) || is_spoke(x, y);
}

uint16_t	get_color_from_homescreen(uint16_t row, uint16_t col)
{
	uint16_t color = RGB16_BLACK;
	if (is_frame(row, col))
	{
		color = RGB16_GREEN;
	}
	else if (col < 120 && row < 120)
	{
		color = RGB16_GOLD;
	}
	else if (col >= 120 && row < 120)
	{
		color = RGB16_GRAY;
	}
	else if (col < 120 && row >= 120)
	{
		color = RGB16_NAVY;
	}
	return color;
}

static uint16_t rgb_to_565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((uint16_t)(b & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (r >> 3);
}

static uint16_t hue_to_565(uint8_t hue)
{
	uint8_t section = hue / 40;
	uint8_t rise = (uint8_t)((hue % 40) * 255 / 40);
	uint8_t fall = 255 - rise;

	switch (section)
	{
		case 0:
			return rgb_to_565(255, rise, 0);
		case 1:
			return rgb_to_565(fall, 255, 0);
		case 2:
			return rgb_to_565(0, 255, rise);
		case 3:
			return rgb_to_565(0, fall, 255);
		case 4:
			return rgb_to_565(rise, 0, 255);
		default:
			return rgb_to_565(255, 0, fall);
	}
}

uint16_t get_image_color(const packed_image_t *image, uint16_t row, uint16_t column)
{
	if (column < image->col || row < image->row)
		return RGB16_TRANS;

	uint16_t local_column = column - image->col;
	uint16_t local_row = row - image->row;
	if (local_column >= image->width || local_row >= image->height)
		return RGB16_TRANS;

	uint8_t stride = (image->width + 1) >> 1;
	uint8_t byte = pgm_read_byte_far(image->pixels + (uint32_t)local_row * stride + (local_column >> 1));
	uint8_t nibble = (local_column & 1) ? (byte & 0x0F) : (byte >> 4);

	if (nibble == 0)
		return RGB16_TRANS;
	return (image->palette[nibble]);
}

uint16_t	get_color_from_colorscreen(uint16_t row, uint16_t col)
{
	return (hue_to_565((uint8_t)((col + row) >> 1)));
}

const tft_screen_info_t g_screens[5] = {
	[TFT_STATE_HOMESCREEN]  = { .frames = { &hand1, &hand2 }, .get_background_color = get_color_from_homescreen },
	[TFT_STATE_COLORSCREEN] = { .frames = { &hand1, &hand2 }, .get_background_color = get_color_from_colorscreen },
	[TFT_STATE_IMUSCREEN] = { .frames = { &hand1, &hand2 }, .get_background_color = get_color_from_colorscreen },
	[TFT_STATE_FLEXSCREEN] = { .frames = { &hand1, &hand2 }, .get_background_color = get_color_from_colorscreen },
	[TFT_STATE_COMMSCREEN] = { .frames = { &hand1, &hand2 }, .get_background_color = get_color_from_colorscreen },
};

uint16_t	get_color_from_image_or_background(const uint16_t row, const uint16_t column)
{
	const tft_screen_info_t	*screen = &g_screens[g_touchscreen_mode];
	uint16_t	color;

	if (screen->frames[0])
	{
		color = get_image_color(screen->frames[g_animation_frame], row, column);
		if (color != RGB16_TRANS)
			return color;
		else if (screen->get_background_color)
			return screen->get_background_color(row, column);
	}
	return RGB16_CYAN;
}

void	display_screen()
{
	gc9a01_set_display_size(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);

	gc9a01_send_command(GC9A01_MEM_WRITE);
	gc9a01_start_parameter_sequence();
	for (uint16_t row = 0; row < TFT_HEIGHT; row++)
	{
		for (uint16_t column = 0; column < TFT_WIDTH; column++)
		{
			uint16_t color = get_color_from_image_or_background(row, column);
			spi_send_data(color >> 8);
			spi_send_data(color & 0xFF);
		}
	}
	gc9a01_end_parameter_sequence();
}

void	refresh_image(const packed_image_t* image)
{
	gc9a01_set_display_size(image->row, image->col,
		image->row + image->width - 1, image->col + image->height - 1);

	gc9a01_send_command(GC9A01_MEM_WRITE);
	gc9a01_start_parameter_sequence();
	for (uint8_t row = 0; row < image->height; row++)
	{
		for (uint8_t col = 0; col < image->width; col++)
		{
			uint16_t color = get_color_from_image_or_background(image->row + row, image->col + col);
			spi_send_data(color >> 8);
			spi_send_data(color & 0xFF);
		}
	}
	gc9a01_end_parameter_sequence();
}

void	refresh_screen()
{
	static uint8_t	last_mode = TFT_STATE_HOMESCREEN;
	if (last_mode != g_touchscreen_mode)
	{
		display_screen();
		last_mode = g_touchscreen_mode;
		return;
	}
	else
	{
		static uint8_t	last_frame = 0;
		if ((last_frame != g_animation_frame) && g_screens[g_touchscreen_mode].frames[0])
		{
			refresh_image(g_screens[g_touchscreen_mode].frames[g_animation_frame]);
			last_frame = g_animation_frame;
		}
	}
}

void	gc9a01_send_image(uint8_t x, uint8_t y, const packed_image_t* image)
{
	const uint8_t	stride = (image->width + 1) >> 1;

	gc9a01_set_display_size(x, y, x + image->width - 1, y + image->height - 1);

	gc9a01_send_command(GC9A01_MEM_WRITE);
	gc9a01_start_parameter_sequence();
	for (uint8_t row = 0; row < image->height; row++)
	{
		for (uint8_t col = 0; col < image->width; col++)
		{
			uint8_t byte = pgm_read_byte_far(image->pixels + (uint16_t)row * stride + (col >> 1));
			uint8_t nibble = (col & 1) ? (byte & 0x0F) : (byte >> 4);
			uint16_t color = (nibble == 0) ? get_color_from_colorscreen(row - 120, col - 120) : image->palette[nibble];
			spi_send_data(color >> 8);
			spi_send_data(color & 0xFF);
		}
	}
	gc9a01_end_parameter_sequence();
}

void putstr_screen(uint16_t x, uint16_t y, const char *str, uint16_t fg_color)
{
	while (*str)
	{
		uart0_printstr(str);
		char c = *str++;
		if (c < g_default_font.first_char || c > g_default_font.last_char)
			c = '?';

		uint16_t byte_offset = (c - g_default_font.first_char) * g_default_font.width;

		gc9a01_set_display_size(x, y, x + g_default_font.width - 1, y + g_default_font.height - 1);

		gc9a01_send_command(GC9A01_MEM_WRITE);
		gc9a01_start_parameter_sequence();
		for (uint8_t row = 0; row < g_default_font.height; row++)
		{
			for (uint8_t col = 0; col < g_default_font.width; col++)
			{
				uint8_t line_byte = pgm_read_byte_far(g_default_font.bitmap + byte_offset + col);
				uint16_t color = (line_byte & (1 << row)) ? fg_color : g_screens[g_touchscreen_mode].get_background_color(x + row, y + col);
				spi_send_data(color >> 8);
				spi_send_data(color & 0xFF);
			}
		}
		gc9a01_end_parameter_sequence();

		x -= g_default_font.width;
	}
}

void putstr_screen_big(uint16_t x, uint16_t y, const char *str, uint16_t fg_color)
{
	// uint16_t	original_x = x;

	uint8_t		target_width = g_default_font.width * 2;
	uint8_t		target_height = g_default_font.height * 2;

	while (*str)
	{
		char c = *str++;
		if (c < g_default_font.first_char || c > g_default_font.last_char)
			c = '?';

		uint16_t	byte_offset = (c - g_default_font.first_char) * g_default_font.width;

		gc9a01_set_display_size(x, y, x + target_width - 1, y + target_height - 1);

		gc9a01_send_command(GC9A01_MEM_WRITE);
		gc9a01_start_parameter_sequence();
		for (uint8_t target_row = 0; target_row < target_height; target_row++)
		{
			uint8_t font_row = target_row >> 1;

			for (uint8_t target_col = 0; target_col < target_width; target_col++)
			{
				uint8_t font_col = target_col >> 1;
				uint8_t line_byte = pgm_read_byte_far(g_default_font.bitmap + byte_offset + font_col);
				uint16_t color = (line_byte & (1 << font_row)) ? fg_color : g_screens[g_touchscreen_mode].get_background_color(y + font_row, x + font_col);

				spi_send_data(color >> 8);
				spi_send_data(color & 0xFF);
			}
		}
		gc9a01_end_parameter_sequence();

		x += target_width;
	}
}


