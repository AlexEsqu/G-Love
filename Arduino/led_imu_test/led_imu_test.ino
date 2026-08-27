#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_Sensor.h>
#include <FastLED.h>
#include <Wire.h>

#define NUM_LEDS 20
#define DATA_PIN 8  // Fil DI connecté sur la broche 8
#define CLOCK_PIN 9 // Fil CI connecté sur la broche 9

CRGB leds[NUM_LEDS];

Adafruit_ICM20948 icm;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("Initialisation du ruban LED APA102...");

  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  // FastLED.setBrightness(50); 

  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();
  delay(500);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  Serial.println("Initialisation de l'ICM-20948...");
  // Initialisation de l'IMU à l'adresse 0x68 pour Pimoroni
  if (!icm.begin_I2C(0x68)) {
    Serial.println("Erreur : Capteur Pimoroni introuvable !");
    while (1) {
      delay(10);
    }
  }

  Wire.setClock(100000);

  Serial.println("Système prêt pour le test !");
}

void loop() {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mag;
  sensors_event_t temp;

  // Lecture de l'IMU
  icm.getEvent(&accel, &gyro, &temp, &mag);

  float valeurX = accel.acceleration.x;
  float valeurY = accel.acceleration.y;
  float valeurZ = accel.acceleration.z;

  // Limitation de la valeur X entre -9.81 et +9.81 m/s^2
  if (valeurX < -9.81)
    valeurX = -9.81;
  if (valeurX > 9.81)
    valeurX = 9.81;

  // Limitation de la valeur Y entre -9.81 et +9.81 m/s^2
  if (valeurY < -9.81)
    valeurY = -9.81;
  if (valeurY > 9.81)
    valeurY = 9.81;

  // Limitation de la valeur Z entre -9.81 et +9.81 m/s^2
  if (valeurZ < -9.81)
    valeurZ = -9.81;
  if (valeurZ > 9.81)
    valeurZ = 9.81;

  // map() travaille avec des entiers
  long xCentiemes = valeurX * 100;
  uint8_t hueX = map(xCentiemes, -981, 981, 160, 0);

  long yCentiemes = valeurY * 100;
  uint8_t hueY = map(yCentiemes, -981, 981, 160, 0);

  long zCentiemes = valeurZ * 100;
  uint8_t hueZ = map(zCentiemes, -981, 981, 160, 0);

  // CHSV(Teinte, Saturation, Valeur/Luminosite)
  fill_solid(leds, NUM_LEDS, CHSV(hueX, 128, 64));
  FastLED.show();

  Serial.print("Inclinaison X: ");
  Serial.print(valeurX);
  Serial.print(" m/s^2; ");

  Serial.print(" Y: ");
  Serial.print(valeurY);
  Serial.print(" m/s^2; ");

  Serial.print(" Z: ");
  Serial.print(valeurZ);
  Serial.println(" m/s^2");

  Serial.print(" Teinte LED (Hue): ");
  Serial.print(hueX);
  Serial.print(", ");
  Serial.print(hueY);
  Serial.print(", ");
  Serial.println(hueZ);

  delay(50);
}