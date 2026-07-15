#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <APA102.h>

// Définition des broche pour les LED
#define NUM_LEDS 144
#define DATA_PIN 8  // Fil DI connecté sur la broche 8
#define CLOCK_PIN 9 // Fil CI connecté sur la broche 9
rgb_color colors[NUM_LEDS];
uint8_t brightness = 1;
int sensorState[16];

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
// Create an object for writing to the LED strip.
APA102<DATA_PIN, CLOCK_PIN> ledStrip;

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
  sensorState[channel] = analogRead(SIG);
  delayMicroseconds(50);
  sensorState[channel] = analogRead(SIG);

  // Affichage résultats
  Serial.print("Canal C" + String(channel) + "  -> ");
  Serial.println(sensorState[channel]);
  return sensorState[channel];
}

void readAllChannels(int max) {
  for (int i = 0; i < max && i < 16; i++) {
    readChannel(i);
  }
}

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

  Serial.println("Init ruban LED APA102");
 

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

  uint8_t color_rgb;
  for(uint16_t i = 0; i < NUM_LEDS; i++)
  {
    if (i < NUM_LEDS / 3)
      color_rgb = hueX;
    else if (i > NUM_LEDS / 3 && i < 2*NUM_LEDS / 3)
      color_rgb = hueY;
    else
      color_rgb = hueZ;
    colors[i] = hsvToRgb(color_rgb, 255, 255);
  }

  brightness = 1;
  uint8_t BIGGESTb = 1;
  if (sensorState[0] >= 100)
    brightness = 3;
  if (sensorState[0] >= 200)
    brightness = 5;
  if (sensorState[0] >= 400)
    brightness = 7;
  if (sensorState[0] >= 600)
    brightness = 9;
  if (brightness > BIGGESTb)
    BIGGESTb = brightness;

  if (sensorState[2] >= 100)
    brightness = 3;
  if (sensorState[2] >= 200)
    brightness = 5;
   if (sensorState[2] >= 400)
    brightness = 7;
  if (sensorState[2] >= 600)
    brightness = 9;
     if (brightness > BIGGESTb)
    BIGGESTb = brightness;
/*
  if (sensorState[3] >= 100)
    brightness = 3;
  if (sensorState[3] >= 200)
    brightness = 5;
   if (sensorState[3] >= 400)
    brightness = 7;
  if (sensorState[3] >= 600)
    brightness = 9;
   if (brightness > BIGGESTb)
    BIGGESTb = brightness;
  */
  if (sensorState[4] >= 100)
    brightness = 3;
  if (sensorState[4] >= 200)
    brightness = 5;
   if (sensorState[4] >= 400)
    brightness = 7;
  if (sensorState[4] >= 600)
    brightness = 9;
   if (brightness > BIGGESTb)
    BIGGESTb = brightness;
  ledStrip.write(colors, NUM_LEDS, BIGGESTb);

  readAllChannels(6);
  delay(200);
}