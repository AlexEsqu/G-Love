#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_ICM20948 icm;

const int ledGauche = 5;
const int ledMilieu = 6;
const int ledDroite = 7;


const float seuilInclinaison = 3.0; 

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }


  pinMode(ledGauche, OUTPUT);
  pinMode(ledMilieu, OUTPUT);
  pinMode(ledDroite, OUTPUT);

  // Test LED
  digitalWrite(ledGauche, HIGH);
  digitalWrite(ledMilieu, HIGH);
  digitalWrite(ledDroite, HIGH);
  delay(500);
  digitalWrite(ledGauche, LOW);
  digitalWrite(ledMilieu, LOW);
  digitalWrite(ledDroite, LOW);

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

void loop() {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mag;
  sensors_event_t temp;
  
  // Lecture des capteurs
  icm.getEvent(&accel, &gyro, &temp, &mag);

  float valeurX = accel.acceleration.x;

  Serial.print("Inclinaison X: ");
  Serial.print(valeurX);
  Serial.println(" m/s^2");

  // Logique d'allumage des LED

  if (valeurX > seuilInclinaison) {
    // Incliné vers la Gauche
    digitalWrite(ledGauche, HIGH);
    digitalWrite(ledMilieu, LOW);
    digitalWrite(ledDroite, LOW);
  } 
  else if (valeurX < -seuilInclinaison) {
    // Incliné vers la Droite
    digitalWrite(ledGauche, LOW);
    digitalWrite(ledMilieu, LOW);
    digitalWrite(ledDroite, HIGH);
  } 
  else {
    // À plat (au centre)
    digitalWrite(ledGauche, LOW);
    digitalWrite(ledMilieu, HIGH);
    digitalWrite(ledDroite, LOW);
  }

  delay(100);
}