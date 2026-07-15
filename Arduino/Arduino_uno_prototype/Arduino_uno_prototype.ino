#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_Sensor.h>
#include <FastLED.h>
#include <Wire.h>

// Définition des broche pour les LED
#define NUM_LEDS 20
#define DATA_PIN 8  // Fil DI connecté sur la broche 8
#define CLOCK_PIN 9 // Fil CI connecté sur la broche 9
CRGB leds[NUM_LEDS];

// Définition des broches de sélection
const int S0 = 2;
const int S1 = 3;
const int S2 = 4;
const int S3 = 5;
const int EN = 7;
// Broche de lecture commune
const int SIG = 14;

const float seuilInclinaisonX = 3.0;
const float seuilInclinaisonY = 3.0;
const float seuilInclinaisonZ = 3.0;

Adafruit_ICM20948 icm;

// Fonction pour configurer les broches de sélection selon le canal désiré (0 à
// 15)
void selectChannel(int channel) {
  digitalWrite(S0, (channel & 1));
  digitalWrite(S1, (channel & 2) >> 1);
  digitalWrite(S2, (channel & 4) >> 2);
  digitalWrite(S3, (channel & 8) >> 3);
}

int readChannel(int channel) {
  selectChannel(channel);
  delayMicroseconds(50);
  int etat = analogRead(SIG);
  delayMicroseconds(50);
  etat = analogRead(SIG);

  // Affichage résultats
  Serial.print("Canal C" + String(channel) + "  -> ");
  Serial.println(etat);
  return etat;
}

void readAllChannels(int max) {
  for (int i = 0; i < max && i < 16; i++) {
    readChannel(i);
  }
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("Init ruban LED APA102");
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
  // FastLED.setBrightness(50);

  fill_solid(leds, NUM_LEDS, CHSV(CRGB::White, 128, 8));
  FastLED.show();
  delay(500);
  fill_solid(leds, NUM_LEDS, CHSV(CRGB::Black, 128, 8));
  FastLED.show();

  Wire.setClock(100000);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(SIG, INPUT);
  digitalWrite(EN, 1);

  Serial.println("Init ICM-20948");
  // Initialisation I2C (adresse 0x68 pour Pimoroni)
  if (!icm.begin_I2C(0x68)) {
    Serial.println("Erreur: no sensor found");
    while (1) {
      delay(10);
    }
  }
  Serial.println("ICM-20948 ready");
}

String dataX;
String dataY;
String dataZ;

void loop() {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mag;
  sensors_event_t temp;

  // Lecture des capteurs de l'ICM20948
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

  dataX = "X: " + String(valeurX) + "m/s^2";
  dataY = " Y: " + String(valeurY) + " m/s^2; ";
  dataZ = " Z: " + String(valeurZ) + " m/s^2\n";
  Serial.print(dataX + dataY + dataZ);

  // Formatage des valeurs pour les LED
  long xCentiemes = valeurX * 100;
  uint8_t hueX = map(xCentiemes, -981, 981, 160, 0);

  long yCentiemes = valeurY * 100;
  uint8_t hueY = map(yCentiemes, -981, 981, 160, 0);

  long zCentiemes = valeurZ * 100;
  uint8_t hueZ = map(zCentiemes, -981, 981, 160, 0);

  // logique d'allumage des LED
  // CHSV(Teinte, Saturation, Valeur/Luminosite)
  fill_solid(leds, NUM_LEDS, CHSV(hueX, 128, 8));
  FastLED.show();

  readAllChannels(9);
  delay(200);
}