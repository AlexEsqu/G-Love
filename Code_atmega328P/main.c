#include "../includes/main.h"
#include "../includes/config_atm328p.h"
#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_Sensor.h>
#include <APA102.h>

// Définition des broche pour les LED
#define NUM_LEDS 144
#define DATA_PIN 8  // Fil DI connecté sur la broche 8
#define CLOCK_PIN 9 // Fil CI connecté sur la broche 9
rgb_color colors[NUM_LEDS];
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

void setup()
{
	i2c_init();

	uart0_init(BAUDRATE);

	// init selector pins
	DDRD |= (1 << S0);
	DDRD |= (1 << S1);
	DDRD |= (1 << S2);
	DDRD |= (1 << S3);
	DDRD |= (1 << EN);

}

int main()
{

	setup();

	while (1) {
	}
}