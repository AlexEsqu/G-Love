#ifndef LED_H
# define LED_H

#include <stdint.h>
#include <stdbool.h>

// Définition des broche pour les LED
// LED pin definition
#define CLOCK_PIN 13 // Fil CI connecté sur la broche 13
#define DATA_PIN 11  // Fil DI connecté sur la broche 11

#define BRIGHTNESS 4
#define LED_COLOR_SPEED 4
#define LED_LUMINOSITY_SPEED 1

enum fingers {
    THUMB = 0,
    INDEX,
    MIDDLE,
    RING,
    PINKY,
    FINGER_COUNT
};

typedef struct s_finger_leds {
    uint8_t start;
    uint8_t size;
} finger_leds_t;

#define THUMB_LED_SIZE 14
#define INDEX_LED_SIZE 18
#define MIDDLE_LED_SIZE 20
#define RING_LED_SIZE 18
#define PINKY_LED_SIZE 14

// LED strip configuration
#define NUM_LEDS (THUMB_LED_SIZE + INDEX_LED_SIZE + MIDDLE_LED_SIZE + RING_LED_SIZE + PINKY_LED_SIZE)
#define END_FRAME_BYTES ((NUM_LEDS + 15) / 16)
#define MAX_PULSES 20

typedef struct s_rgb_color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_color_t;

// Structure packée d'une LED SK9822 (4 octets exacts)
typedef struct __attribute__((packed)) {
    uint8_t header; // 0xE0 | brightness
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} sk9822_led_t;

// Buffer LED complet contigu en mémoire statique
typedef struct __attribute__((packed)) {
    uint8_t start[4];
    sk9822_led_t leds[NUM_LEDS];
    uint8_t end[END_FRAME_BYTES];
} strip_buffer_t;

// Pulse in fixed-point 8.8 (no floats!)
// 256 units = 1 LED. e.g., speed 0.5 LED/frame = 128
typedef struct {
    uint8_t start_pos; // Starting position of the pulse
    uint8_t end_pos;   // Ending position of the pulse
    uint16_t pos_fixed; // Position * 256
    uint16_t speed;     // Speed * 256
    uint8_t length;     // Number of LEDs
    uint8_t r, g, b;    // Color
    bool active;
} pulse_t;

extern strip_buffer_t strip;
extern const finger_leds_t g_finger_leds[FINGER_COUNT];

void        led_spi();
void        strip_init(uint8_t brightness);
void        update_and_render_pulses(void);
void        trigger_pulse(uint8_t finger, uint8_t r, uint8_t g, uint8_t b, uint16_t speed_fp, uint8_t length);
void        fade_strip_to_black(uint8_t fade_factor);
void        led_effect(uint8_t finger, uint8_t effect);
void        strip_flush(void);
rgb_color_t hsvToRgb(uint16_t h, uint8_t s, uint8_t v);
uint8_t     qadd8(uint8_t a, uint8_t b);

#endif