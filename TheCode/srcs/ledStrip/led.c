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

void led_spi()
{
    static uint16_t color_cycle = 0; // hue of the first LED, kept between calls
    rgb_color_t colors[NUM_LEDS];

    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        uint16_t hue = (color_cycle + (uint16_t)(180UL * i / NUM_LEDS)) % 180;

        colors[i] = hsvToRgb(hue, 255, 255);
    }
    spi_send_color(colors, NUM_LEDS, BRIGHTNESS);

    color_cycle += LED_COLOR_SPEED;
    if (color_cycle >= 180)
        color_cycle -= 180;

    // pulses effect instead of the wheel:

    // static bool initialized = false;
    // if (!initialized) {
    //     strip_init(BRIGHTNESS);
    //     trigger_pulse(FINGER_COUNT, 255, 255, 255, 255, 14);
    //     initialized = true;
    // }
    // update_and_render_pulses();
}
