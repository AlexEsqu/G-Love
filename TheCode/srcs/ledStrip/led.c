#include "led.h"
#include "../../includes/main.h"

static pulse_t pulses[MAX_PULSES];

// Static array of finger LED ranges
const finger_leds_t finger_leds_map[FINGER_COUNT + 1] = {
    [THUMB] = { .start = 0, .size = THUMB_LED_SIZE },
    [INDEX] = { .start = THUMB_LED_SIZE, .size = INDEX_LED_SIZE },
    [MIDDLE] = { .start = THUMB_LED_SIZE + INDEX_LED_SIZE, .size = MIDDLE_LED_SIZE },
    [RING] = { .start = THUMB_LED_SIZE + INDEX_LED_SIZE + MIDDLE_LED_SIZE, .size = RING_LED_SIZE },
    [PINKY] = { .start = THUMB_LED_SIZE + INDEX_LED_SIZE + MIDDLE_LED_SIZE + RING_LED_SIZE, .size = PINKY_LED_SIZE },
    [FINGER_COUNT] = { .start = 0, .size = NUM_LEDS } // Optional: Represents the entire hand
};

// Trigger with fixed-point speed (e.g., speed_fp = 200 for ~0.78 LED/frame)
void trigger_pulse(uint8_t finger, uint8_t r, uint8_t g, uint8_t b, uint16_t speed_fp, uint8_t length) {
    if (finger > FINGER_COUNT) {
        return;
    }

    finger_leds_t range = finger_leds_map[finger];
    uint8_t bounded_length = (length > range.size) ? range.size : length;

    for (uint8_t i = 0; i < MAX_PULSES; i++) {
        if (!pulses[i].active) {
            pulses[i].start_pos = range.start;
            pulses[i].end_pos = range.start + range.size;
            pulses[i].pos_fixed = (uint16_t)range.start << 8;
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
    // Slightly fade the trail left by previous frames
    fade_strip_to_black(215); // The lower the number, the faster the trail disappears

    // Draw only the ultra-bright head of each pulse
    for (uint8_t i = 0; i < MAX_PULSES; i++) {
        if (!pulses[i].active) continue;

        pulses[i].pos_fixed += pulses[i].speed;
        int16_t head = pulses[i].pos_fixed >> 8;

        if (head >= pulses[i].start_pos && head < pulses[i].end_pos) {
            // Bright comet head deposited onto the strip
            strip.leds[head].red   = qadd8(strip.leds[head].red,   pulses[i].r);
            strip.leds[head].green = qadd8(strip.leds[head].green, pulses[i].g);
            strip.leds[head].blue  = qadd8(strip.leds[head].blue,  pulses[i].b);
        }

        // End of run
        if (head >= pulses[i].end_pos) {
            pulses[i].active = false;
        }
    }

    strip_flush();
}

void led_effect(uint8_t finger, uint8_t effect)
{
    if (finger >= FINGER_COUNT) {
        return;
    }

    finger_leds_t range = g_finger_leds[finger];

    switch (finger)
    {
        case THUMB:
            // Apply effect to thumb LEDs
            break;
        case INDEX:
            // Apply effect to index LEDs
            break;
        case MIDDLE:
            // Apply effect to middle LEDs
            break;
        case RING:
            // Apply effect to ring LEDs
            break;
        case PINKY:
            // Apply effect to pinky LEDs
            break;
        default:
            return;
            break;
    }

    (void)range;
    (void)effect;
}

void led_spi()
{
    // static uint16_t color_cycle = 0;
    // static uint16_t luminosity_cycle = 0;

    // for (uint16_t i = 0; i < NUM_LEDS; i++)
	// {
	// 	colors[i] = hsvToRgb(color_cycle + 360 * i / NUM_LEDS,
    //                             255,
    //                                 (luminosity_cycle < 180) ? luminosity_cycle * 255 / 200 : (360 - luminosity_cycle) * 255 / 200); 
	// }
    
	// spi_send_color(colors, NUM_LEDS, 1);

    // color_cycle += LED_COLOR_SPEED;
    // if (color_cycle >= 360) {
    //     color_cycle = 0;
    // }

    // luminosity_cycle += LED_LUMINOSITY_SPEED;
    // if (luminosity_cycle >= 360) {
    //     luminosity_cycle = 0;
    // }
    static bool initialized = false;
    if (!initialized) {
        strip_init(BRIGHTNESS); // Global brightness ~50% (15/31)

        // Trigger a test pulse on thumb LEDs.
        trigger_pulse(FINGER_COUNT, 255, 255, 255, 255, 14);

        initialized = true;
    }

    update_and_render_pulses();
}
