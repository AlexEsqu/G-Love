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

#define TS_MINX 590
#define TS_MAXX 3800
#define TS_MINY 700
#define TS_MAXY 3800



//define tactile
#define I2C_ADDR_CST816D 0x15 // addressr du tactile
#define TOUCH_INT 7


void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(1); // Mode paysage (320x240)
  tft.fillScreen(0xF800);
  
  tft.setTextColor(0x780F);
  tft.setTextSize(2);
  tft.setCursor(120, 120);
  tft.println("PROUT");

  Wire.begin(); 
  
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

        int pixelX = map(x, TS_MINX, TS_MAXX, 0, 320);
        int pixelY = map(y, TS_MINY, TS_MAXY, 0, 240);
        
        pixelX = constrain(pixelX, 0, 320);
        pixelY = constrain(pixelY, 0, 240);
        if (points > 0) { 
          Serial.print("Toucher detecte ! X: ");
          Serial.print(x);
          Serial.print(" | Y: ");
          Serial.println(y);

          tft.fillRect(320 - pixelX, pixelY - 2, 4, 4, 0xF800);
        }
      }
    }
    delay(20);
  }
}