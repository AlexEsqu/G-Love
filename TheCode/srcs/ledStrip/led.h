#ifndef LED_H
# define LED_H

#include "../../includes/main.h"

// Définition des broche pour les LED
#define NUM_LEDS 144
#define CLOCK_PIN 13 // Fil CI connecté sur la broche 13
#define DATA_PIN 11  // Fil DI connecté sur la broche 11

extern t_rgb_color colors[NUM_LEDS];
#define BRIGHTNESS 1


t_rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v);
void        led_spi();

#endif