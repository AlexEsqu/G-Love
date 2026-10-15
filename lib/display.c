#include "touchscreen.h"
#include "main.h"

int	is_circle(int16_t x, int16_t y)
{
	return (x * x + y * y) > TFT_FRAME;
}

int	is_spoke(int16_t x, int16_t y)
{
	return ((y > -2 && y < 2) || (x > -2 && x < 2));
}

int	is_frame(int16_t x, int16_t y)
{
	return is_circle(x, y) || is_spoke(x, y);
}

uint16_t	get_color_from_homescreen(uint16_t col, uint16_t row)
{
	uint16_t color = RGB16_BLACK;
	if (is_frame(col - 120, row - 120))
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

uint16_t get_image_color(const packed_image_t *image, uint16_t column, uint16_t row)
{
	if (column < image->col || row < image->row)
		return 0;

	uint16_t local_column = column - image->col;
	uint16_t local_row = row - image->row;
	if (local_column >= image->width || local_row >= image->height)
		return 0;

	uint8_t stride = (image->width + 1) >> 1;
	uint8_t byte = pgm_read_byte_far(image->pixels + (uint32_t)local_row * stride + (local_column >> 1));
	uint8_t nibble = (local_column & 1) ? (byte & 0x0F) : (byte >> 4);

	if (nibble == 0)
		return 0;
	return (image->palette[nibble]);
}

uint16_t	get_color_from_colorscreen(uint16_t col, uint16_t row)
{
	return (hue_to_565((uint8_t)((col + row) >> 1)));
}

const tft_screen_info_t screens[2] = {
	[TFT_STATE_HOMESCREEN]  = { handwide_animation, get_color_from_homescreen },
	[TFT_STATE_COLORSCREEN] = { handwide_animation, get_color_from_colorscreen },
};

uint16_t	get_color_from_image_or_background(const uint16_t column, const uint16_t row)
{
	const tft_screen_info_t	*screen = &screens[g_touchscreen_mode];
	return screen->get_background_color(column, row);
	uint16_t	color;

	if (screen->frames)
	{
		color = get_image_color(screen->frames[g_animation_frame], column, row);
		if (color != RGB16_BLACK)
			return color;
	}
	if (screen->get_background_color)
		return screen->get_background_color(column, row);
	return RGB16_BLACK;
}

void gc9a01_display_homescreen()
{
	gc9a01_set_display_size(0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);

	gc9a01_send_command(GC9A01_MEM_WRITE);
	gc9a01_start_parameter_sequence();
	for (uint16_t row = 0; row < TFT_HEIGHT; row++)
	{
		for (uint16_t column = 0; column < TFT_WIDTH; column++)
		{
			uint16_t color = get_color_from_image_or_background(column, row);
			spi_send_data(color >> 8);
			spi_send_data(color & 0xFF);
		}
	}
	gc9a01_end_parameter_sequence();
}

void	gc9a01_send_image(uint8_t x, uint8_t y, const packed_image_t* image)
{
	const uint8_t	width = image->width;
	const uint8_t	height = image->height;
	const uint8_t	stride = (width + 1) >> 1;
	const uint32_t	total_pixels = image->total_pixels; // could overflow but we're all adult here
	const uint16_t	transparent_color = image->palette[0];
	const uint_farptr_t addr = pgm_get_far_address(hand1_data);

	uart0_printstr("sending image:");
	uart0_print_10bit(width);
	uart0_printstr(" ");
	uart0_print_10bit(height);
	uart0_printstr(" with total pixel: ");
	uart0_print_10bit(total_pixels);
	uart0_printstr("\r\n");

	gc9a01_set_display_size(x, y, x + width - 1, y + height - 1);

	gc9a01_send_command(GC9A01_MEM_WRITE);
	gc9a01_start_parameter_sequence();
	for (uint8_t row = 0; row < height; row++)
	{
		for (uint8_t col = 0; col < width; col++)
		{
			uint8_t byte = pgm_read_byte_far(addr + (uint16_t)row * stride + (col >> 1));
			uint8_t nibble = (col & 1) ? (byte & 0x0F) : (byte >> 4);
			uint16_t color = (nibble == 0) ? get_color_from_colorscreen(row - 120, col - 120) : image->palette[nibble];
			spi_send_data(color >> 8);
			spi_send_data(color & 0xFF);
		}
	}
	gc9a01_end_parameter_sequence();
}



