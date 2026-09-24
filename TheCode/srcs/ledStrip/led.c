#include "led.h"
#include "../../includes/main.h"
#define MAX_PULSES 20
t_strip_buffer strip;

t_rgb_color colors[NUM_LEDS];
static const uint16_t TOTAL_BYTES = sizeof(t_strip_buffer);

// Impulsion en virgule fixe 8.8 (plus aucun float !)
// 256 unités = 1 LED. Ex: vitesse de 0.5 LED/frame = 128
typedef struct {
    uint16_t pos_fixed; // Position * 256
    uint16_t speed;     // Vitesse * 256
    uint8_t length;     // Nombre de LEDs
    uint8_t r, g, b;    // Couleur
    bool active;
} t_pulse;

static t_pulse pulses[MAX_PULSES];


// Déclencheur avec vitesse en virgule fixe (ex: speed_fp = 200 pour ~0.78 LED/frame)
void trigger_pulse(uint8_t r, uint8_t g, uint8_t b, uint16_t speed_fp, uint8_t length) {
    for (uint8_t i = 0; i < MAX_PULSES; i++) {
        if (!pulses[i].active) {
            pulses[i].pos_fixed = 0;
            pulses[i].speed = speed_fp;
            pulses[i].length = length;
            pulses[i].r = r;
            pulses[i].g = g;
            pulses[i].b = b;
            pulses[i].active = true;
            break;
        }
    }
}

// Addition saturée 8 bits ultra-rapide
static inline uint8_t qadd8(uint8_t a, uint8_t b) {
    uint16_t res = (uint16_t)a + b;
    return (res > 255) ? 255 : (uint8_t)res;
}

// Envoi brut du buffer complet en SPI
static inline void strip_flush(void) {
    uint8_t *ptr = (uint8_t *)&strip;
    uint16_t count = TOTAL_BYTES;
    
    while (count--) {
        SPDR = *ptr++;
        while (!(SPSR & (1 << SPIF)));
    }
}

// Atténue tout le ruban d'une fraction (ex: on conserve 85% de la lumière à chaque frame)
void fade_strip_to_black(uint8_t fade_factor) { // fade_factor de 0 à 255 (ex: 220 pour ~86%)
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        strip.leds[i].red   = ((uint16_t)strip.leds[i].red   * fade_factor) >> 8;
        strip.leds[i].green = ((uint16_t)strip.leds[i].green * fade_factor) >> 8;
        strip.leds[i].blue  = ((uint16_t)strip.leds[i].blue  * fade_factor) >> 8;
    }
}

// Calcul et affichage
void update_and_render(void) {
    // On estompe légèrement le sillage laissé par les trames précédentes
    fade_strip_to_black(215); // Plus le chiffre est bas, plus la traîne disparaît vite

    // On dessine uniquement la tête ultra-brillante de chaque impulsion
    for (uint8_t i = 0; i < MAX_PULSES; i++) {
        if (!pulses[i].active) continue;

        pulses[i].pos_fixed += pulses[i].speed;
        int16_t head = pulses[i].pos_fixed >> 8;

        if (head >= 0 && head < NUM_LEDS) {
            // Tête de comète brillante déposée sur le ruban
            strip.leds[head].red   = qadd8(strip.leds[head].red,   pulses[i].r);
            strip.leds[head].green = qadd8(strip.leds[head].green, pulses[i].g);
            strip.leds[head].blue  = qadd8(strip.leds[head].blue,  pulses[i].b);
        }

        // Fin de course
        if (head >= NUM_LEDS) {
            pulses[i].active = false;
        }
    }

    strip_flush();
}

/* Converts a color from HSV to RGB.
 * h is hue (color), as a number between 0 and 360.
 * s is the saturation (intensity), as a number between 0 and 255. (0 - 100%)
 * v is the value (luminosity), as a number between 0 and 255. (0 - 100%) */
t_rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
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
    return (t_rgb_color){r, g, b};
}

// Initialisation unique des trames fixes (start, end, et header des LEDs)
void strip_init(uint8_t brightness) {
    for (uint8_t i = 0; i < 4; i++) strip.start[i] = 0x00;
    
    uint8_t hdr = 0xE0 | (brightness & 0x1F);
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        strip.leds[i].header = hdr;
        strip.leds[i].blue = 0;
        strip.leds[i].green = 0;
        strip.leds[i].red = 0;
    }

    for (uint8_t i = 0; i < END_FRAME_BYTES; i++) strip.end[i] = 0x00;
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
        strip_init(BRIGHTNESS); // Luminosité globale à ~50% (15/31)

        // Déclenche une impulsion test :
        // Rouge=255, Vert=200, Bleu=0, Vitesse=128 (0.5 LED/frame), Longueur=10
        trigger_pulse(255, 200, 0, 200, 15);

        initialized = true;
    }

    // 2. Relancer une impulsion régulièrement (ex: toutes les ~1,5 secondes)
    // static uint16_t tick = 0;
    // if (++tick >= 100) { // 75 frames à 20ms = ~1.5 s
    //     tick = 0;
    //     trigger_pulse(0, 150, 255, 250, 15); // Deuxième impulsion bleue plus rapide
    // }

    update_and_render();
}
