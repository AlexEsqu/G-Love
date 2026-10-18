
#include <Wire.h>
#include <APA102.h>

// Définition des broche pour les LED
#define NUM_LEDS 20
#define DATA_PIN 8  // Fil DI connecté sur la broche 8
#define CLOCK_PIN 9 // Fil CI connecté sur la broche 9
rgb_color colors[NUM_LEDS];
uint8_t brightness = 1;

// Create an object for writing to the LED strip.
APA102<DATA_PIN, CLOCK_PIN> ledStrip;

/* Converts a color from HSV to RGB.
 * h is hue, as a number between 0 and 360.
 * s is the saturation, as a number between 0 and 255.
 * v is the value, as a number between 0 and 255. */
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return rgb_color(r, g, b);
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
}

void loop() {
  uint16_t timeOffset = millis() / 10;

  for(uint16_t i = 0; i < NUM_LEDS; i++)
  {
    uint16_t hue = (timeOffset + (i * 360 / NUM_LEDS)) % 360;
    colors[i] = hsvToRgb(hue, 255, 255);
  }

  ledStrip.write(colors, NUM_LEDS, brightness);
  delay(20);
}