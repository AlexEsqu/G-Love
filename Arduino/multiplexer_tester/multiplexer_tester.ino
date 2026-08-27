// Définition des broches de sélection
const int S0 = 2;
const int S1 = 3;
const int S2 = 4;
const int S3 = 5;
const int EN = 7
int last = 0;
// Broche de lecture commune
const int SIG = 14;

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(EN, OUTPUT);

  pinMode(SIG, INPUT);
  digitalWrite(EN, 1);

  Serial.println("Test de truc");
}

// Fonction pour configurer les broches de sélection selon le canal désiré (0 à
// 15)
void selectChannel(int channel) {
  digitalWrite(S0, (channel & 1));
  digitalWrite(S1, (channel & 2) >> 1);
  digitalWrite(S2, (channel & 4) >> 2);
  digitalWrite(S3, (channel & 8) >> 3);
}

void loop() {

  selectChannel(0);

  delayMicroseconds(50);
  int etatC0 = analogRead(SIG);
  delayMicroseconds(50);
  etatC0 = analogRead(SIG);

  selectChannel(1);
  delayMicroseconds(50);
  int etatC1 = analogRead(SIG);
  delayMicroseconds(50);
  etatC1 = analogRead(SIG);

  selectChannel(2);
  delayMicroseconds(50);
  int etatC2 = analogRead(SIG);
  delayMicroseconds(50);
  etatC2 = analogRead(SIG);

  // Affichage résultats
  Serial.print("Canal C0  -> ");
  Serial.println(etatC0);

  Serial.print("Canal C1  -> ");
  Serial.println(etatC1);
  
  Serial.print("Canal C2  -> ");
  Serial.println(etatC2);
  /*
  int current = analogRead(14);
  Serial.print(current);

  Serial.println("------------------------------------");
  //if ((etatC0 >= last && etatC0 - last > 200) || etatC0 >= 900)
  if (etatC0 >= 700)
  {
     digitalWrite(7, 1);
  }
  else
  {
     digitalWrite(7, 0);
     etatC0 = etatC0 * 0.7 + last * 0.3;
  }
  last = etatC0;
*/
  Serial.println("------------------------------------");
  delay(100);
}