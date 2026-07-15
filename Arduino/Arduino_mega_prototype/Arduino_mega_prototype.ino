#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

#define TFT_CS   10
#define TFT_DC    8
#define TFT_RST   9

#define TFT_MOSI 51
#define TFT_MISO 50
#define TFT_SCK  52

// Broche Tactile
#define TOUCH_CS  7

// Initialisation
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, TFT_MISO);

// Valeurs de calibration pout le tactile
#define TS_MINX 590
#define TS_MAXX 3800
#define TS_MINY 700
#define TS_MAXY 3800

void setup() {

  Serial.begin(115200);  
  
  // Liaison UART avec l'Arduino Uno (Broche 19 = RX1)
  Serial1.begin(115200); 

  Serial.println("Démarrage du test de l'écran...");
  
  // Initialisation de l'écran
  tft.begin();
  
 
  tft.setRotation(3); 
  

  tft.fillScreen(ILI9341_BLACK); 
  
  // Affichage d'un texte de test pour vérifier que l'écran fonctionne
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("ECRAN EN LIGNE !");
  
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(10, 40);
  tft.println("Attente de l'UART...");

}

String fill = "";
String input = "";

void loop() {

  if (Serial1.available() > 0)
  {
    char caractereRecu;
    //char caractereRecu = Serial1.read();
    fill = input;
    input = Serial1.readStringUntil('\n');  // Read until newline
    Serial.println(input);

    tft.setTextColor(ILI9341_BLACK);
    tft.setCursor(10, 60);
    tft.print(fill);
      //delay(100);

    tft.setTextColor(ILI9341_WHITE);
    tft.setCursor(10, 60);
    tft.print(input);
  }

  delay(5);
}