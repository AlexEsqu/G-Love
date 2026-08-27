#include <Wire.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"


#define TFT_CS   10
#define TFT_DC    8
#define TFT_RST   9

#define TFT_MOSI 51
#define TFT_MISO 50
#define TFT_SCK  52

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

//define tactile
#define I2C_ADDR_CST816D 0x15 // addresse du tactile
#define TOUCH_INT 7

String fill = "";
String input = "";

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  tft.begin();
  tft.setRotation(2); // rotate screen(240x240)
  tft.fillScreen(0xF800);
  
  tft.setTextColor(0x780F);
  tft.setTextSize(2);
  tft.setCursor(120, 120);
  tft.println("PROUT123");

  Wire.begin();
  Wire.setClock(400000);
  
  pinMode(TOUCH_INT, INPUT); 

}

void loop() {

  if (digitalRead(TOUCH_INT) == LOW) {
    
    // On demande la lecture des registres de position
    Wire.beginTransmission(I2C_ADDR_CST816D);
    Wire.write(0x02); // On pointe directement sur le registre du nombre de points
    if (Wire.endTransmission() == 0) {
      
      Wire.requestFrom(I2C_ADDR_CST816D, 5); // On lit 5 octets (Nb points, X_high, X_low, Y_high, Y_low)
      
      if (Wire.available() == 5) {
        uint8_t points  = Wire.read(); 
        uint8_t x_high  = Wire.read(); 
        uint8_t x_low   = Wire.read(); 
        uint8_t y_high  = Wire.read(); 
        uint8_t y_low   = Wire.read(); 


        int x = ((x_high & 0x0F) << 8) | x_low;
        int y = ((y_high & 0x0F) << 8) | y_low;

        if (points > 0) { 
          Serial.print("Toucher detecte ! X: ");
          Serial.print(x);
          Serial.print(" | Y: ");
          Serial.println(y);

          tft.fillRect(240-x, 240-y, 4, 4, 0xFFFF);
        }
      }
    }
  }

  if (Serial1.available() > 0)
  {
    char caractereRecu;
    //char caractereRecu = Serial1.read();
    fill = input;
    input = Serial1.readStringUntil('\n');  // Read until newline
    Serial.println(input);
/*
    tft.setTextColor(0xF800);
    tft.setCursor(10, 60);
    tft.print(fill);
      //delay(100);

    tft.setTextColor(0x780F);
    tft.setCursor(10, 60);
    tft.print(input);
*/
  }
}