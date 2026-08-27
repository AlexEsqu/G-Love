#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_ICM20948 icm;

// Définition des broches pour les 9 LED
const int ledGaucheX = 5;
const int ledMilieuX = 6;
const int ledDroiteX = 7;

const int ledGaucheY = 8;
const int ledMilieuY = 9;
const int ledDroiteY = 10;

const int ledGaucheZ = 11;
const int ledMilieuZ = 12;
const int ledDroiteZ = 13;

const float seuilInclinaisonX = 3.0;
const float seuilInclinaisonY = 3.0;
const float seuilInclinaisonZ = 3.0;

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  // Configuration des broches des LED en sortie
  pinMode(ledGaucheX, OUTPUT);
  pinMode(ledMilieuX, OUTPUT);
  pinMode(ledDroiteX, OUTPUT);

  pinMode(ledGaucheY, OUTPUT);
  pinMode(ledMilieuY, OUTPUT);
  pinMode(ledDroiteY, OUTPUT);

  pinMode(ledGaucheZ, OUTPUT);
  pinMode(ledMilieuZ, OUTPUT);
  pinMode(ledDroiteZ, OUTPUT);

  // Test LED
  digitalWrite(ledGaucheX, HIGH);
  digitalWrite(ledMilieuX, HIGH);
  digitalWrite(ledDroiteX, HIGH);
  delay(500);
  digitalWrite(ledGaucheX, LOW);
  digitalWrite(ledMilieuX, LOW);
  digitalWrite(ledDroiteX, LOW);

  digitalWrite(ledGaucheY, HIGH);
  digitalWrite(ledMilieuY, HIGH);
  digitalWrite(ledDroiteY, HIGH);
  delay(500);
  digitalWrite(ledGaucheY, LOW);
  digitalWrite(ledMilieuY, LOW);
  digitalWrite(ledDroiteY, LOW);

  digitalWrite(ledGaucheZ, HIGH);
  digitalWrite(ledMilieuZ, HIGH);
  digitalWrite(ledDroiteZ, HIGH);
  delay(500);
  digitalWrite(ledGaucheZ, LOW);
  digitalWrite(ledMilieuZ, LOW);
  digitalWrite(ledDroiteZ, LOW);

  Serial.println("Initialisation de l'ICM-20948...");

  // Initialisation I2C (adresse 0x68 pour Pimoroni)
  if (!icm.begin_I2C(0x68)) {
    Serial.println("Erreur : Impossible de trouver le capteur !");
    while (1) {
      delay(10);
    }
  }
  Serial.println("Capteur prêt pour le contrôle des LED !");
}

String dataX;
String dataY;
String dataZ;
void loop() {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mag;
  sensors_event_t temp;

  // Lecture des capteurs
  icm.getEvent(&accel, &gyro, &temp, &mag);

  float valeurX = accel.acceleration.x;
  float valeurY = accel.acceleration.y;
  float valeurZ = accel.acceleration.z;

  dataX = "X: " + String(valeurX) + "m/s^2"; 
  //Serial.print("X: ");
  //Serial.print(valeurX);
  //Serial.print(" m/s^2; ");
  //Serial.print(data);

  dataY = " Y: " + String(valeurY) + " m/s^2; ";
  //Serial.print(" Y: ");
  //Serial.print(valeurY);
  //Serial.print(" m/s^2; ");

  dataZ = " Z: " + String(valeurZ) + " m/s^2\n";
  //Serial.print(" Z: ");
  //Serial.print(valeurZ);
  //Serial.print(" m/s^2\n");
  Serial.print(dataX + dataY + dataZ);
  // logique d'allumage des LED

  // AXE X
  if (valeurX > seuilInclinaisonX) {
    digitalWrite(ledGaucheX, HIGH);
    digitalWrite(ledMilieuX, LOW);
    digitalWrite(ledDroiteX, LOW);
  } else if (valeurX < -seuilInclinaisonX) {
    digitalWrite(ledGaucheX, LOW);
    digitalWrite(ledMilieuX, LOW);
    digitalWrite(ledDroiteX, HIGH);
  } else {
    digitalWrite(ledGaucheX, LOW);
    digitalWrite(ledMilieuX, HIGH);
    digitalWrite(ledDroiteX, LOW);
  }

  // AXE Y
  if (valeurY > seuilInclinaisonY) {
    digitalWrite(ledGaucheY, HIGH);
    digitalWrite(ledMilieuY, LOW);
    digitalWrite(ledDroiteY, LOW);
  } else if (valeurY < -seuilInclinaisonY) {
    digitalWrite(ledGaucheY, LOW);
    digitalWrite(ledMilieuY, LOW);
    digitalWrite(ledDroiteY, HIGH);
  } else {
    digitalWrite(ledGaucheY, LOW);
    digitalWrite(ledMilieuY, HIGH);
    digitalWrite(ledDroiteY, LOW);
  }

  // AXE Z
  if (valeurZ > seuilInclinaisonZ) {
    digitalWrite(ledGaucheZ, HIGH);
    digitalWrite(ledMilieuZ, LOW);
    digitalWrite(ledDroiteZ, LOW);
  } else if (valeurZ < -seuilInclinaisonZ) {
    digitalWrite(ledGaucheZ, LOW);
    digitalWrite(ledMilieuZ, LOW);
    digitalWrite(ledDroiteZ, HIGH);
  } else {
    digitalWrite(ledGaucheZ, LOW);
    digitalWrite(ledMilieuZ, HIGH);
    digitalWrite(ledDroiteZ, LOW);
  }

  delay(100);
}