#include "led.h"
#include "main.h"

static pulse_t pulses[MAX_PULSES];

// Static array of finger LED ranges
const finger_leds_t finger_leds_map[FINGER_COUNT + 1] = {
    [THUMB] = { .start = 0, .size = THUMB_LED_SIZE },
    [INDEX] = { .start = THUMB_LED_SIZE, .size = INDEX_LED_SIZE },
    [MIDDLE] = { .start = THUMB_LED_SIZE + INDEX_LED_SIZE, .size = MIDDLE_LED_SIZE },
    [RING] = { .start = THUMB_LED_SIZE + INDEX_LED_SIZE + MIDDLE_LED_SIZE, .size = RING_LED_SIZE },
    [PINKY] = { .start = THUMB_LED_SIZE + INDEX_LED_SIZE + MIDDLE_LED_SIZE + RING_LED_SIZE, .size = PINKY_LED_SIZE },
    [FINGER_COUNT] = { .start = 0, .size = NUM_LEDS } // All leds for init/testing purposes
};

// Convert a distance from the center to the physical strip index on the left.
static uint8_t left_pulse_led(uint8_t finger, uint8_t distance) {
    finger_leds_t range = finger_leds_map[finger];
    uint8_t half = range.size / 2;

    return range.start + half - 1 - distance;
}

// Convert a distance from the center to the physical strip index on the right.
static uint8_t right_pulse_led(uint8_t finger, uint8_t distance) {
    finger_leds_t range = finger_leds_map[finger];
    uint8_t half = range.size / 2;

    return range.start + half + distance;
}

static void render_pulse_led(uint8_t led,
                             uint8_t r, uint8_t g, uint8_t b) {
    strip.leds[led].red = qadd8(strip.leds[led].red, r);
    strip.leds[led].green = qadd8(strip.leds[led].green, g);
    strip.leds[led].blue = qadd8(strip.leds[led].blue, b);
}

// Trigger with fixed-point speed (speed_fp = speed * 256) and length in number of LEDs
void trigger_pulse(uint8_t finger, uint8_t r, uint8_t g, uint8_t b, uint16_t speed_fp, uint8_t length) {
    if (finger > FINGER_COUNT) {
        return;
    }

    finger_leds_t range = finger_leds_map[finger];
    uint8_t bounded_length = (length > range.size) ? range.size : length;

    for (uint8_t i = 0; i < MAX_PULSES; i++) {
        if (!pulses[i].active) {
            pulses[i].start_pos = 0;
            pulses[i].end_pos = range.size / 2;
            pulses[i].pos_fixed = 0;
            pulses[i].finger = finger;
            pulses[i].speed = speed_fp;
            pulses[i].length = bounded_length;
            pulses[i].r = r;
            pulses[i].g = g;
            pulses[i].b = b;
            pulses[i].active = true;
            break;
        }
    }
}

// Dim the entire strip by a fraction (e.g., keep 85% brightness each frame)
void fade_strip_to_black(uint8_t fade_factor) { // fade_factor from 0 to 255 (e.g., 220 for ~86%)
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        strip.leds[i].red   = ((uint16_t)strip.leds[i].red   * fade_factor) >> 8;
        strip.leds[i].green = ((uint16_t)strip.leds[i].green * fade_factor) >> 8;
        strip.leds[i].blue  = ((uint16_t)strip.leds[i].blue  * fade_factor) >> 8;
    }
}

// Calculation and rendering
void update_and_render_pulses(void) {
    if (strip_is_busy()) {
        return;
    }

    // Slightly fade the trail left by previous frames
    fade_strip_to_black(215); // The lower the number, the faster the trail disappears

    // Draw only the ultra-bright head of each pulse
    for (uint8_t i = 0; i < MAX_PULSES; i++) {
        if (!pulses[i].active) continue;

        pulses[i].pos_fixed += pulses[i].speed;
        int16_t head = pulses[i].pos_fixed >> 8;

        if (head >= pulses[i].start_pos && head < pulses[i].end_pos) {

            // Two synchronized heads move away from the finger center.
            render_pulse_led(left_pulse_led(pulses[i].finger, head),
                             pulses[i].r, pulses[i].g, pulses[i].b);
            render_pulse_led(right_pulse_led(pulses[i].finger, head),
                             pulses[i].r, pulses[i].g, pulses[i].b);
        }

        // End of run
        if (head >= pulses[i].end_pos) {
            pulses[i].active = false;
        }
    }

    strip_flush();
}

// Trigger a pulse on a specific finger with given color
void led_effect(uint8_t finger, uint8_t color[3])
{
    if (finger >= FINGER_COUNT) {
        return;
    }

    switch (finger)
    {
        case THUMB:
            trigger_pulse(THUMB, color[0], color[1], color[2], 255, 10);
            break;
        case INDEX:
            trigger_pulse(INDEX, color[0], color[1], color[2], 255, 10);
            break;
        case MIDDLE:
            trigger_pulse(MIDDLE, color[0], color[1], color[2], 255, 10);
            break;
        case RING:
            trigger_pulse(RING, color[0], color[1], color[2], 255, 10);
            break;
        case PINKY:
            trigger_pulse(PINKY, color[0], color[1], color[2], 255, 10);
            break;
        default:
            return;
            break;
    }
}

// color of the pulses: the one chosen in Color mode, LED_DEFAULT_COLOR until then
// (kept when leaving Color mode, until another color is chosen)
static rgb_color_t g_chosen_color = {
    (LED_DEFAULT_COLOR >> 16) & 0xFF, (LED_DEFAULT_COLOR >> 8) & 0xFF, LED_DEFAULT_COLOR & 0xFF
};

// set the color of the whole strip (RGB888: 0xRRGGBB, from the touchscreen)
void led_set_color(uint32_t rgb888)
{
    g_chosen_color.red = (rgb888 >> 16) & 0xFF;
    g_chosen_color.green = (rgb888 >> 8) & 0xFF;
    g_chosen_color.blue = rgb888 & 0xFF;
}

// multiplexer channel of each finger (led.h), same channel for its FSR and its flex sensor
static const uint8_t g_finger_channel[FINGER_COUNT] = {
    FSR_CHANNEL_THUMB, FSR_CHANNEL_INDEX, FSR_CHANNEL_MIDDLE, FSR_CHANNEL_RING, FSR_CHANNEL_PINKY
};


// one sensor that can start pulses
typedef struct s_sensor_trigger
{
    uint16_t baseline;  // value at rest, measured at boot
    uint16_t smoothed;  // value without noise
    bool     armed;     // ready for a new pulse (went back under the OFF threshold)
    uint8_t  repeat;    // frames left before the next pulse while still pressed
} sensor_trigger_t;

static sensor_trigger_t g_fsr_trigger[FINGER_COUNT];
static sensor_trigger_t g_flex_trigger[FINGER_COUNT];

/*
    Returns true when a pulse must start:
      - the sensor moved away from its rest value by more than "on" -> pulse
      - still more than "on" -> a new pulse every PULSE_REPEAT_FRAMES
      - back under "off"     -> ready for the next press (hysteresis: no pulse
                                storm when the value hesitates around "on")
    The difference is taken both ways: works whether the value goes up or down.
*/
static bool sensor_trigger_update(sensor_trigger_t *trigger, uint16_t raw,
                                  uint16_t on, uint16_t off, bool measuring_rest)
{
    // smoothing: 3/4 old value + 1/4 new one
    trigger->smoothed = (trigger->smoothed * 3 + raw) / 4;

    if (measuring_rest)
    {
        trigger->baseline = trigger->smoothed;
        trigger->armed = true;
        return (false);
    }

    uint16_t delta = (trigger->smoothed > trigger->baseline)
        ? trigger->smoothed - trigger->baseline
        : trigger->baseline - trigger->smoothed;

    if (delta >= on)
    {
        if (trigger->armed || --trigger->repeat == 0)
        {
            trigger->armed = false;
            trigger->repeat = PULSE_REPEAT_FRAMES;
            return (true);
        }
    }
    else if (delta < off)
        trigger->armed = true;
    return (false);
}

/*
    LEDs: pulses started by the sensors of each finger.
    FSR pressed or finger bent -> pulse in the color chosen in Color mode
    (LED_DEFAULT_COLOR until a color is chosen). The pulses spread from the middle of the
    finger to its ends (update_and_render_pulses), the strip is sent by
    the SPI interrupt (strip_flush) without blocking the loop.
*/
void led_spi()
{
    static bool    initialized = false;
    static uint8_t frames = 0;  // frames since boot, to measure the rest values

    if (!initialized)
    {
        strip_init(BRIGHTNESS);
        initialized = true;
    }

    bool measuring_rest = (frames < BASELINE_FRAMES);
    if (measuring_rest)
        frames++;

    for (uint8_t finger = 0; finger < FINGER_COUNT; finger++)
    {
        uint8_t channel = g_finger_channel[finger];
        rgb_color_t color = g_chosen_color;

        // FSR pressed or finger bent: a pulse in the chosen color
        if (sensor_trigger_update(&g_fsr_trigger[finger], g_sensor.sensorForce[channel],
                FSR_PULSE_ON, FSR_PULSE_OFF, measuring_rest))
            trigger_pulse(finger, color.red, color.green, color.blue, PULSE_SPEED, 0);

        if (sensor_trigger_update(&g_flex_trigger[finger], g_sensor.sensorFlex[channel],
                FLEX_PULSE_ON, FLEX_PULSE_OFF, measuring_rest))
            trigger_pulse(finger, color.red, color.green, color.blue, PULSE_SPEED, 0);
    }

    update_and_render_pulses();
}

