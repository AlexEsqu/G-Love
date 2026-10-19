#include "touchscreen.h"
#include "main.h"

// current touchscreen mode, changes logic of the LEDs and info sent / displayed
tft_state_t			g_touchscreen_mode = TFT_STATE_HOMESCREEN;

// timing for animations
uint8_t				g_animation_frame = 0;

const tft_screen_info_t g_screens[5] = {
	[TFT_STATE_HOMESCREEN] = {
		.frames = { NULL },
		.frame_count = 0,
		.get_background_color = get_color_from_homescreen,
		.print_screen_text = print_homescreen_text,
		.on_touch_action = on_touch_on_homescreen,
	},
	[TFT_STATE_COLORSCREEN] = {
		.frames = { NULL },
		.frame_count = 0,
		.get_background_color = get_color_from_colorscreen,
		.print_screen_text = print_colorscreen_text,
		.on_touch_action = on_touch_on_colorscreen
	},
	[TFT_STATE_IMUSCREEN] = {
		.frames = { NULL },
		.frame_count = 0,
		.get_background_color = get_color_from_imuscreen,
		.print_screen_text = print_imuscreen_text,
		.on_touch_action = on_touch_on_imuscreen
	},
	[TFT_STATE_FLEXSCREEN] = {
		.frames = { NULL },
		.frame_count = 0,
		.get_background_color = get_color_from_flexscreen,
		.print_screen_text = print_flexscreen_text,
		.on_touch_action = on_touch_on_flexscreen,
	},
	[TFT_STATE_COMMSCREEN] = {
		.frames = { NULL },
		.frame_count = 0,
		.get_background_color = get_color_from_comscreen,
		.print_screen_text = print_comscreen_text,
		.on_touch_action = on_touch_on_comscreen
	},
};

uint8_t	is_in_mask(uint16_t row, uint16_t col, const very_packed_image_t* image)
{
	if (row >= image->height || col >= image->width)
		return 0;

	uint16_t stride = ((uint16_t)image->width + 7) >> 3;
	uint32_t offset = (uint32_t)row * stride + (col >> 3);
	uint8_t byte = pgm_read_byte_far((uintptr_t)image->pixels + offset);

	return (byte & (uint8_t)(0x80 >> (col & 7))) ? 1 : 0;
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
	uint8_t byte = pgm_read_byte_far((uint_farptr_t)(uintptr_t)image->pixels + (uint32_t)local_row * stride + (local_column >> 1));
	uint8_t nibble = (local_column & 1) ? (byte & 0x0F) : (byte >> 4);

	if (nibble == 0)
		return RGB16_TRANS;
	return (image->palette[nibble]);
}

uint16_t	get_color_from_image_or_background(const uint16_t row, const uint16_t column)
{
	const tft_screen_info_t	*screen = &g_screens[g_touchscreen_mode];
	uint16_t	color;

	if (screen->frame_count && screen->frame_count >= g_animation_frame)
	{
		color = get_image_color(screen->frames[g_animation_frame], row, column);
		if (color != RGB16_TRANS)
			return color;
		else
			return screen->get_background_color(row, column);
	}
	else if (screen->get_background_color)
	{
		return screen->get_background_color(row, column);
	}
	return RGB16_CYAN;
}

// incremented at each full redraw (used by the flex screen to redraw its indicators)
uint8_t	g_screen_redraw_count = 0;

void	display_screen()
{
	g_screen_redraw_count++;
	gc9a01_set_display_size(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);

	gc9a01_send_command(GC9A01_MEM_WRITE);
	gc9a01_start_parameter_sequence();
	if (g_touchscreen_mode == TFT_STATE_COLORSCREEN)
	{
		static uint16_t block_colors[TFT_WIDTH / 4];
		for (uint16_t row = 0; row < TFT_HEIGHT; row += 4)
		{
			for (uint16_t column = 0; column < TFT_WIDTH; column += 4)
			{
				block_colors[column >> 2] = get_color_from_colorscreen_fast(row, column);
			}
			for (uint8_t block_row = 0; block_row < 4; block_row++)
			{
				for (uint16_t column = 0; column < TFT_WIDTH; column += 4)
				{
					uint16_t color = block_colors[column >> 2];
					for (uint8_t block_column = 0; block_column < 4; block_column++)
					{
						spi_send_data(color >> 8);
						spi_send_data(color & 0xFF);
					}
				}
			}
		}
	}
	else
	{
		for (uint16_t row = 0; row < TFT_HEIGHT; row++)
		{
			for (uint16_t column = 0; column < TFT_WIDTH; column++)
			{
				uint16_t color = get_color_from_image_or_background(row, column);
				spi_send_data(color >> 8);
				spi_send_data(color & 0xFF);
			}
		}
	}
	gc9a01_end_parameter_sequence();
}

void	refresh_image(const packed_image_t* image)
{
	gc9a01_set_display_size(
        image->col,
        image->row,
        image->col + image->width - 1,
        image->row + image->height - 1
    );

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
	static uint32_t	last_full_refresh = 0;

	// full refresh once in a while to remove bad pixels accumulating
	if (last_full_refresh < 1000000UL)
		last_full_refresh++;
	else
	{
		last_full_refresh = 0;
		display_screen();
		g_screens[g_touchscreen_mode].print_screen_text();
		return;
	}

	if (last_mode != g_touchscreen_mode)
	{
		display_screen();
		last_mode = g_touchscreen_mode;
	}
	else if (g_screens[g_touchscreen_mode].frame_count > g_animation_frame)
	{
		static uint8_t	last_frame = 0;
		if ((last_frame != g_animation_frame))
		{
			refresh_image(g_screens[g_touchscreen_mode].frames[g_animation_frame]);
			last_frame = g_animation_frame;
		}
	}
	g_screens[g_touchscreen_mode].print_screen_text();
}


uint16_t	putchar_screen(uint16_t x, uint16_t y, char c, uint16_t font_color, tft_font_size_t font_size)
{
	uint16_t	byte_offset = (c - g_default_font.first_char) * g_default_font.width;
	uint8_t		target_width = g_default_font.width * font_size;
	uint8_t		target_height = g_default_font.height * font_size;
	uint8_t		multiplier = font_size == FONT_SIZE_SMALL ? 0
			: font_size == FONT_SIZE_MID ? 1
			: 2;

	gc9a01_set_display_size(x, y, x + target_width - 1, y + target_height - 1);

	gc9a01_send_command(GC9A01_MEM_WRITE);
	gc9a01_start_parameter_sequence();
	for (uint8_t target_row = 0; target_row < target_height; target_row++)
	{
		uint8_t font_row = target_row >> multiplier;

		for (uint8_t target_col = 0; target_col < target_width; target_col++)
		{
			uint8_t font_col = target_col >> multiplier;
			uint8_t line_byte = pgm_read_byte_far((uint_farptr_t)(uintptr_t)g_default_font.bitmap + byte_offset + font_col);
			uint16_t color = (line_byte & (1 << font_row))
				? font_color
				: get_color_from_image_or_background(y + target_row, x + target_col);

			spi_send_data(color >> 8);
			spi_send_data(color & 0xFF);
		}
	}
	gc9a01_end_parameter_sequence();
	x += target_width;

	return (x);
}

uint16_t	putstr_screen(uint16_t x, uint16_t y, const char *str, uint16_t font_color, tft_font_size_t font_size)
{
	while (*str)
	{
		char c = *str++;
		if (c < g_default_font.first_char || c > g_default_font.last_char)
			c = '?';

		x = putchar_screen(x, y, c, font_color, font_size);
	}

	return x;
}

int	is_screen_frame(int16_t x, int16_t y)
{
	return (x * x + y * y) > (int32_t)TFT_FRAME;
}


uint16_t	puthex_screen(uint16_t x, uint16_t y, uint32_t nbr, uint16_t font_color, tft_font_size_t font_size)
{
	const char*	hex_base = "0123456789ABCDEF";

	if (nbr > 15)
	{
		x = puthex_screen(x, y, nbr / 16, font_color, font_size);
	}
	x = putchar_screen(x, y, hex_base[nbr % 16], font_color, font_size);

	return x;
}


uint16_t	putint_screen(uint16_t x, uint16_t y, int32_t nbr, uint16_t font_color, tft_font_size_t font_size)
{
	if (nbr == INT32_MIN)
	{
		return putstr_screen(x, y, "-2147483648", font_color, font_size);
	}

	if (nbr < 0)
	{
		x = putchar_screen(x, y, '-', font_color, font_size);
		nbr = -nbr;
	}

	if (nbr > 9)
	{
		x = putint_screen(x, y, nbr / 10, font_color, font_size);
	}
	x = putchar_screen(x, y, (nbr % 10) + '0', font_color, font_size);

	return x;
}

uint16_t	putfloat_screen(uint16_t x, uint16_t y, float nbr, uint16_t font_color, tft_font_size_t font_size)
{
	if (nbr < 0)
	{
		x = putchar_screen(x, y, '-', font_color, font_size);
		nbr = -nbr;
	}

	uint32_t	integer_part = (uint32_t)nbr;
	x = putint_screen(x, y, integer_part, font_color, font_size);

	x = putchar_screen(x, y, '.', font_color, font_size);

	uint32_t	two_first_decimal = (nbr - integer_part) * 100;
	if (two_first_decimal < 10)
		x = putchar_screen(x, y, '0', font_color, font_size);

	x = putint_screen(x, y, two_first_decimal, font_color, font_size);

	return x;
}

void		print_nav_info(uint16_t color)
{
	putstr_screen(105, 200, "swipe", color, FONT_SIZE_SMALL);
	putstr_screen(90, 210, "to go back", color, FONT_SIZE_SMALL);
	putstr_screen(100, 220, "to menu", color, FONT_SIZE_SMALL);
}
