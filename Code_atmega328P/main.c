#include "../includes/config_atm328p.h"
#include "../includes/main.h"


// Définition des broche pour les LED
#define NUM_LEDS 144
#define DATA_PIN 8  // Fil DI connecté sur la broche 8
#define CLOCK_PIN 9 // Fil CI connecté sur la broche 9
t_rgb_color colors[NUM_LEDS];
uint8_t brightness = 1;
int sensorState[16];

// define multiplexer selector pins
#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define EN 7
// define multiplxer reading pin
#define SIG 14

const float seuilInclinaisonX = 3.0;
const float seuilInclinaisonY = 3.0;
const float seuilInclinaisonZ = 3.0;

void selectChannel(int channel) {
	if (channel & 1)
		PORTD |= (1 << S0);
	else
		PORTD &= ~(1 << S0);

	if (channel & 2)
		PORTD |= (1 << S1);
	else
		PORTD &= ~(1 << S1);

	if (channel & 4)
		PORTD |= (1 << S2);
	else	
		PORTD &= ~(1 << S2);

	if (channel & 8)
		PORTD |= (1 << S3);
	else
		PORTD &= ~(1 << S3);
}

int readChannel(int channel) {
	selectChannel(channel);
	_delay_ms(50);
	sensorState[channel] = ft_adc_read_10bit();

	uart0_printstr("channel:");
	uart0_print_10bit(channel);
	uart0_printstr(" value:");
	uart0_print_10bit(sensorState[channel]);
	uart0_printstr("\n");
	return sensorState[channel];
}

void readAllChannels(int max) {
	for (int i = 0; i < max && i < 16; i++)
	{
		readChannel(i);
	}
}

/* Converts a color from HSV to RGB.
 * h is hue, as a number between 0 and 360.
 * s is the saturation, as a number between 0 and 255.
 * v is the value, as a number between 0 and 255. */
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


void setup()
{
	i2c_init();
	adc_init();
	spi_init();
	uart0_init(BAUDRATE);

	// init selector pins
	DDRD |= (1 << S0);
	DDRD |= (1 << S1);
	DDRD |= (1 << S2);
	DDRD |= (1 << S3);

	DDRD |= (1 << EN);
	PORTD &= ~(1 << EN);

	//sig is our read pin (A0 on Uno corresponds to PC0 / Channel 0)
	DDRC &= ~(1 << 0);
	PORTC &= ~(1 << 0);

}

int main()
{

	setup();

	while (1) {
		
		// read adc pin with multiplexer
		readAllChannels(2);

		//led strip
		for (int i = 0; i < NUM_LEDS; i++)
		{
			colors[i] = hsvToRgb(128, 255, 255); 
		}
		spi_send_color(colors, NUM_LEDS, brightness);

		_delay_ms(200);
	}
}