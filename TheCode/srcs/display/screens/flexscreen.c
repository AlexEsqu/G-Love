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

/*
	FSR / FLEX CALIBRATION, done the first time the flex screen is used
	(after each power-up), or again with a tap on "Recalib":
	  step 1: open hand, relaxed               -> value at rest of each sensor
	  step 2: fist closed                      -> flex sensors bent
	  step 3: fingertips pressed on the table  -> FSR pressed
	Then each sensor is scaled 0..255 between these two values: that removes
	the offset at rest and works whether the value goes up or down.
*/
#define FLEX_MIN_RANGE		20		// a sensor that moved less than this is ignored (not connected)
#define FLEX_TAP_COOLDOWN	500		// g_ms_counter units (~1 s): one tap = one step
#define FLEX_REDRAW_STEP	24		// redraw a finger when its bend changed by more than this
#define FSR_REDRAW_STEP		8		// redraw a fingertip when its pressure changed by more than this

typedef enum
{
	FLEX_CAL_REST,		// waiting for step 1: open hand
	FLEX_CAL_FLEX,		// waiting for step 2: fist closed (flex)
	FLEX_CAL_FSR,		// waiting for step 3: fingertips pressed (FSR)
	FLEX_CAL_DONE		// calibrated, sensors displayed
} flex_calibration_step_t;

typedef struct
{
	uint16_t rest[FLEX_FINGER_COUNT];	// value with the hand open
	uint16_t active[FLEX_FINGER_COUNT];	// value bent (flex) / pressed (FSR)
} sensor_range_t;

static flex_calibration_step_t	g_flex_step = FLEX_CAL_REST;
static sensor_range_t			g_fsr_range;
static sensor_range_t			g_flex_range;

// scaled values 0..255, computed once per loop (not for each pixel)
static uint8_t					g_flex_level[FLEX_FINGER_COUNT];
static uint8_t					g_fsr_level[FLEX_FINGER_COUNT];

// fingertip where the pressure (FSR) square is drawn, inside the hand image
static const uint8_t g_tip_row[FLEX_FINGER_COUNT] = { 145, 38, 19, 27, 48 };
static const uint8_t g_tip_col[FLEX_FINGER_COUNT] = { 24, 72, 103, 132, 165 };
#define FSR_SQUARE_SIZE		5		// half width: 11x11 pixels

// instructions on the right of the hand
#define FLEX_TEXT_COL		178
#define FLEX_TEXT_ROW		100

// copy the sensor values, interrupts off: the UART interrupt writes them (comms.c)
static void	read_sensors(uint16_t force[FLEX_FINGER_COUNT], uint16_t flex[FLEX_FINGER_COUNT])
{
	uint8_t sreg_backup = SREG;

	SREG &= ~(1 << SREG_I);
	for (uint8_t finger = 0; finger < FLEX_FINGER_COUNT; finger++)
	{
		force[finger] = g_packet_to_export.sensorForce[finger];
		flex[finger] = g_packet_to_export.sensorFlex[finger];
	}
	SREG = sreg_backup;
}

// value between rest (0) and active (255), whatever the direction of the sensor
static uint8_t	scale_sensor(uint16_t value, uint16_t rest, uint16_t active)
{
	int16_t range = (int16_t)active - (int16_t)rest;
	int16_t delta = (int16_t)value - (int16_t)rest;

	if (range < 0) // sensor whose value goes down when pressed / bent
	{
		range = -range;
		delta = -delta;
	}
	if (range < FLEX_MIN_RANGE || delta <= 0)
		return (0);
	if (delta >= range)
		return (255);
	return ((uint8_t)((int32_t)delta * 255 / range));
}

static void	update_levels(void)
{
	uint16_t force[FLEX_FINGER_COUNT];
	uint16_t flex[FLEX_FINGER_COUNT];

	if (g_flex_step != FLEX_CAL_DONE)
	{
		for (uint8_t finger = 0; finger < FLEX_FINGER_COUNT; finger++)
		{
			g_flex_level[finger] = 0;
			g_fsr_level[finger] = 0;
		}
		return;
	}
	read_sensors(force, flex);
	for (uint8_t finger = 0; finger < FLEX_FINGER_COUNT; finger++)
	{
		g_flex_level[finger] = scale_sensor(flex[finger],
			g_flex_range.rest[finger], g_flex_range.active[finger]);
		g_fsr_level[finger] = scale_sensor(force[finger],
			g_fsr_range.rest[finger], g_fsr_range.active[finger]);
	}
}

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
	uint8_t level = g_flex_level[finger]; // bend of the finger, 0..255
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
		return level;

	uint16_t gradient_length = (length + 2) / 2;
	if (finger != FLEX_THUMB)
	{
		uint16_t red_length = length - gradient_length;
		if (position <= red_length)
			return level;
		position -= red_length;
		return (uint8_t)(((uint16_t)level * (gradient_length - position)) / gradient_length);
	}
	if (position >= gradient_length)
	{
		return level;
	}

	return (uint8_t)(((uint16_t)level * position) / gradient_length);
}

// pressure square: dark yellow (no pressure) -> bright yellow (full pressure)
static uint16_t	fsr_color(uint8_t level)
{
	uint8_t intensity = 40 + (uint16_t)level * 215 / 255;

	return (((uint16_t)(intensity >> 3) << 11) | ((uint16_t)(intensity >> 2) << 5));
}

static void	draw_fsr_square(uint8_t finger)
{
	gc9a01_send_square(g_tip_col[finger], g_tip_row[finger],
		fsr_color(g_fsr_level[finger]), FSR_SQUARE_SIZE);
}

// redraw only the rectangle of one finger (much faster than the whole screen)
static void	redraw_finger(uint8_t finger)
{
	const flex_finger_area_t *area = &g_finger_areas[finger];

	gc9a01_set_display_size(area->col_min, area->row_min, area->col_max, area->row_max);
	gc9a01_send_command(GC9A01_MEM_WRITE);
	gc9a01_start_parameter_sequence();
	for (uint16_t row = area->row_min; row <= area->row_max; row++)
	{
		for (uint16_t col = area->col_min; col <= area->col_max; col++)
		{
			uint16_t color = get_color_from_image_or_background(row, col);
			spi_send_data(color >> 8);
			spi_send_data(color & 0xFF);
		}
	}
	gc9a01_end_parameter_sequence();
}

static uint8_t	level_changed(uint8_t a, uint8_t b, uint8_t step)
{
	return ((a > b ? a - b : b - a) > step);
}

/*
	Live display after calibration: only what changed is redrawn
	(a finger whose bend moved, a fingertip whose pressure moved).
*/
static void	update_sensor_display(void)
{
	static uint8_t	drawn_flex[FLEX_FINGER_COUNT];
	static uint8_t	drawn_fsr[FLEX_FINGER_COUNT];
	static uint8_t	last_redraw_count = 0;
	// display_screen() ran since last time: the fingers are up to date, the squares are gone
	uint8_t			screen_redrawn = (last_redraw_count != g_screen_redraw_count);

	last_redraw_count = g_screen_redraw_count;
	for (uint8_t finger = 0; finger < FLEX_FINGER_COUNT; finger++)
	{
		uint8_t square_erased = screen_redrawn;

		if (screen_redrawn)
			drawn_flex[finger] = g_flex_level[finger];
		else if (level_changed(g_flex_level[finger], drawn_flex[finger], FLEX_REDRAW_STEP))
		{
			redraw_finger(finger);
			drawn_flex[finger] = g_flex_level[finger];
			square_erased = 1; // the square is inside the finger
		}
		if (square_erased || level_changed(g_fsr_level[finger], drawn_fsr[finger], FSR_REDRAW_STEP))
		{
			draw_fsr_square(finger);
			drawn_fsr[finger] = g_fsr_level[finger];
		}
	}
}

void		print_nav_info_flex(uint16_t color)
{
	putstr_screen(118, 200, "swipe", color, FONT_SIZE_SMALL);
	putstr_screen(102, 210, "to go back", color, FONT_SIZE_SMALL);
	putstr_screen(112, 220, "to menu", color, FONT_SIZE_SMALL);
}

// 5 lines of text on the right of the hand, same length so they erase each other
static void	print_flex_lines(const char *l0, const char *l1, const char *l2,
				const char *l3, const char *l4)
{
	const char	*lines[5] = {l0, l1, l2, l3, l4};

	for (uint8_t i = 0; i < 5; i++)
		putstr_screen(FLEX_TEXT_COL, FLEX_TEXT_ROW + 10 * i, lines[i], RGB16_WHITE, FONT_SIZE_SMALL);
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

	if (g_flex_step == FLEX_CAL_REST)
		print_flex_lines("Etape 1/3", "main     ", "ouverte, ", "tape     ", "l'ecran  ");
	else if (g_flex_step == FLEX_CAL_FLEX)
		print_flex_lines("Etape 2/3", "poing    ", "serre,   ", "tape     ", "l'ecran  ");
	else if (g_flex_step == FLEX_CAL_FSR)
		print_flex_lines("Etape 3/3", "appuie   ", "les bouts", "de doigts", "et tape  ");
	else
	{
		print_flex_lines("         ", "Recalib: ", "tape ici ", "         ", "         ");
		update_levels();
		update_sensor_display();
	}

	print_nav_info_flex(RGB16_WHITE);
}

void		on_touch_on_flexscreen()
{
	static uint16_t	last_step = 0;
	uint16_t		force[FLEX_FINGER_COUNT];
	uint16_t		flex[FLEX_FINGER_COUNT];
	uint16_t		col = g_packet_to_export.last_touch.x;

	// the touchscreen reports the finger every few ms: one press = one step
	if ((uint16_t)(g_ms_counter - last_step) <= FLEX_TAP_COOLDOWN)
		return;

	read_sensors(force, flex);
	if (g_flex_step == FLEX_CAL_REST)
	{
		for (uint8_t finger = 0; finger < FLEX_FINGER_COUNT; finger++)
		{
			g_fsr_range.rest[finger] = force[finger];
			g_flex_range.rest[finger] = flex[finger];
		}
		g_flex_step = FLEX_CAL_FLEX;
	}
	else if (g_flex_step == FLEX_CAL_FLEX)
	{
		// fist closed: only the flex sensors (the fingertips may not press anything)
		for (uint8_t finger = 0; finger < FLEX_FINGER_COUNT; finger++)
			g_flex_range.active[finger] = flex[finger];
		g_flex_step = FLEX_CAL_FSR;
	}
	else if (g_flex_step == FLEX_CAL_FSR)
	{
		// fingertips pressed on the table: only the FSR
		for (uint8_t finger = 0; finger < FLEX_FINGER_COUNT; finger++)
			g_fsr_range.active[finger] = force[finger];
		g_flex_step = FLEX_CAL_DONE;
	}
	else if (col >= FLEX_TEXT_COL - 10) // tap on "Recalib"
		g_flex_step = FLEX_CAL_REST;
	else
		return;
	last_step = g_ms_counter;
}
