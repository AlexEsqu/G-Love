void setup() {
  Serial.begin(115200); 
  Serial1.begin(115200);
  Serial.println("Mega ready...");
}

char incomingByte = 0;

void loop() {
  // Si la Mega reçoit un signal de la Uno sur Serial1...
  if (Serial1.available() > 0) {
    //char caractereRecu = Serial1.read();
    
    //Serial.print(caractereRecu);

    String donneeRecue = Serial1.readStringUntil('\n');
    Serial.println(donneeRecue);

  }

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);


  }

}