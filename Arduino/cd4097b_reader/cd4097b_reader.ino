#include <Wire.h>

const uint8_t PIN_SEL_A = A2; // 25 (PC2)
const uint8_t PIN_SEL_B = A1; // 23 (PC0)
const uint8_t PIN_SEL_C = A0; // 24 (PC1)

const uint8_t PIN_ADC_X = A7; // 22 (ADC7)
const uint8_t PIN_ADC_Y = A6; // 19 (ADC6)

void setMuxAddress(uint8_t channel) {
  digitalWrite(PIN_SEL_A, (channel & 0x01) ? HIGH : LOW);
  digitalWrite(PIN_SEL_B, (channel & 0x02) ? HIGH : LOW);
  digitalWrite(PIN_SEL_C, (channel & 0x04) ? HIGH : LOW);
  

  delayMicroseconds(30);
}


uint16_t readMuxChannel(uint8_t channel, uint8_t adcPin) {
  setMuxAddress(channel);

  analogRead(adcPin);
  delayMicroseconds(20);

  return analogRead(adcPin);
}

void setup() {

  Serial.begin(9600);

  pinMode(PIN_SEL_A, OUTPUT);
  pinMode(PIN_SEL_B, OUTPUT);
  pinMode(PIN_SEL_C, OUTPUT);

  // Diviseur ADC regle a 8 (ADPS2=0, ADPS1=1, ADPS0=1) pour 125 kHz a 1 MHz
  ADCSRA = (ADCSRA & 0xF8) | 0x03;

  Serial.println("Demarrage mesure...");
}

void loop() {
  // Broche 8 (Canal X1) sur COM X
  uint16_t valPin8 = readMuxChannel(1, PIN_ADC_X);

  // Broche 7 (Canal X2) sur COM X
  uint16_t valPin7 = readMuxChannel(2, PIN_ADC_X);

  // Broche 22 (Canal Y6) sur COM Y
  uint16_t valPin22 = readMuxChannel(0, PIN_ADC_Y);

  // Broche 23 (Canal Y7) sur COM Y
  uint16_t valPin23 = readMuxChannel(1, PIN_ADC_Y);

  Serial.print("P8 (X1): ");
  Serial.print(valPin8);
  Serial.print(" | P7 (X2): ");
  Serial.print(valPin7);
  Serial.print(" | P22 (Y6): ");
  Serial.print(valPin22);
  Serial.print(" | P23 (Y7): ");
  Serial.println(valPin23);

  delay(100);
}