#include "touchscreen.h"
#include "main.h"


static int is_x_spoke(uint16_t row, uint16_t col)
{
	int16_t diff1 = (int16_t)col - (int16_t)row;
	int16_t sum2  = (int16_t)col + (int16_t)row - 240;

	return ((diff1 > -2 && diff1 < 2) || (sum2 > -2 && sum2 < 2));
}

static int is_comframe(uint16_t row, uint16_t col)
{
	return is_x_spoke(row, col);
}

uint16_t get_color_from_comscreen(uint16_t row, uint16_t col)
{
	uint16_t color = RGB16_BLACK;

	if (is_comframe(row, col))
	{
		color = RGB16_BLACK;
	}
	else if (is_top_quarter_of_screen(row, col))
	{
		return(is_in_mask(row, col, &comscreen_image) ? RGB16_LAPIS_CORE : RGB16_LAPIS_SHADOW);
	}
	else if (is_right_quarter_of_screen(row, col))
	{
		return(is_in_mask(row, col, &comscreen_image) ? RGB16_DEEP_CYAN : RGB16_ABYSS_SHADOW);
	}
	else if (is_bottom_quarter_of_screen(row, col))
	{
		return(is_in_mask(row, col, &comscreen_image) ? RGB16_GLACIER_DEEP : RGB16_COBALT_DARK);
	}
	else if (is_left_quarter_of_screen(row, col))
	{
		return(is_in_mask(row, col, &comscreen_image) ? RGB16_FROST_LIGHT : RGB16_GLACIER_DEEP);
	}

	return color;
}


void		print_comscreen_text(void)
{
	putstr_screen(20, 80, "Com", RGB16_BLACK, FONT_SIZE_MID);
	putstr_screen(20, 100, "mode", RGB16_BLACK, FONT_SIZE_MID);

	putstr_screen(20, 130, "Currently:", RGB16_BLACK, FONT_SIZE_SMALL);
	// gotta plug it here for debug
	putstr_screen(20, 140, "Debug", RGB16_BLACK, FONT_SIZE_SMALL);

	putstr_screen(85, 15, "To a", RGB16_LAPIS_CORE, FONT_SIZE_SMALL);
	putstr_screen(75, 25, "computer", RGB16_LAPIS_CORE, FONT_SIZE_SMALL);

	putstr_screen(205, 145, "To", RGB16_DEEP_CYAN, FONT_SIZE_SMALL);
	putstr_screen(180, 155, "another", RGB16_DEEP_CYAN, FONT_SIZE_SMALL);
	putstr_screen(185, 165, "G-Love", RGB16_DEEP_CYAN, FONT_SIZE_SMALL);

	putstr_screen(140, 190, "To the", RGB16_GLACIER_DEEP, FONT_SIZE_SMALL);
	putstr_screen(142, 200, "STAGE", RGB16_GLACIER_DEEP, FONT_SIZE_SMALL);
}

void		on_touch_on_comscreen()
{
	uint16_t	row = g_packet_to_export.last_touch.x;
	uint16_t	col = g_packet_to_export.last_touch.y;
	uint8_t		old_output = g_current_uart_output;

	if (is_top_quarter_of_screen(row, col))
	{
		g_current_uart_output = OUTPUT_UART_TO_SERIAL;
	}
	else if (is_right_quarter_of_screen(row, col))
	{
		g_current_uart_output = OUTPUT_UART_TO_JST;
	}
	else if (is_bottom_quarter_of_screen(row, col))
	{
		g_current_uart_output = OUTPUT_TO_BMX;
	}
	if (g_current_uart_output != old_output)
		display_screen();
}
