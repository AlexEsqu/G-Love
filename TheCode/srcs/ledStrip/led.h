#ifndef LED_H
# define LED_H

#include "../../includes/main.h"

// Définition des broche pour les LED
#define NUM_LEDS 144
#define END_FRAME_BYTES ((NUM_LEDS + 15) / 16)
#define CLOCK_PIN 13 // Fil CI connecté sur la broche 13
#define DATA_PIN 11  // Fil DI connecté sur la broche 11

// Structure packée d'une LED SK9822 (4 octets exacts)
typedef struct __attribute__((packed)) {
    uint8_t header; // 0xE0 | brightness
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} t_sk9822_led;

// Buffer LED complet contigu en mémoire statique
typedef struct __attribute__((packed)) {
    uint8_t start[4];
    t_sk9822_led leds[NUM_LEDS];
    uint8_t end[END_FRAME_BYTES];
} t_strip_buffer;

extern t_strip_buffer strip;

#define BRIGHTNESS 4
#define LED_COLOR_SPEED 4
#define LED_LUMINOSITY_SPEED 1

#define THUMB_LED 0
#define INDEX_LED 1
#define MIDDLE_LED 2
#define RING_LED 3
#define PINKY_LED 4

// Définition des LED pour chaque doigt
#define THUMB_LED_NUMBER 14
#define INDEX_LED_NUMBER 18
#define MIDDLE_LED_NUMBER 20
#define RING_LED_NUMBER 18
#define PINKY_LED_NUMBER 14

// Définition de la position de départ de chaque doigt dans le tableau de LED
#define THUMB_LED_START 0
#define INDEX_LED_START (THUMB_LED_START + THUMB_LED_NUMBER)
#define MIDDLE_LED_START (INDEX_LED_START + INDEX_LED_NUMBER)
#define RING_LED_START (MIDDLE_LED_START + MIDDLE_LED_NUMBER)
#define PINKY_LED_START (RING_LED_START + RING_LED_NUMBER)

void        led_spi();
void       strip_init(uint8_t brightness);
void       update_and_render(void);
void       trigger_pulse(uint8_t r, uint8_t g, uint8_t b, uint16_t speed_fp, uint8_t length);
void       fade_strip_to_black(uint8_t fade_factor);
void       led_effect(uint8_t finger, uint8_t effect);

#endif