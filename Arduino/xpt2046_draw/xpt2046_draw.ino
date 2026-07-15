#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

// Broches Affichage
#define TFT_CS   10
#define TFT_DC    8
#define TFT_RST   9

// Broche Tactile
#define TOUCH_CS  7

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen ts(TOUCH_CS);

// Valeurs de calibration
#define TS_MINX 590
#define TS_MAXX 3800
#define TS_MINY 700
#define TS_MAXY 3800

void setup() {
  Serial.begin(9600);
  
  tft.begin();
  tft.setRotation(1); // Mode paysage (320x240)
  tft.fillScreen(ILI9341_BLACK);
  
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.println("Appuyez sur l'ecran...");

  // Initialisation du tactile sur le bus SPI matériel
  ts.begin(SPI);
  ts.setRotation(1);
}

void loop() {
  // On vérifie si l'écran détecte une pression physique
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    
    // Convertir les coordonnées brutes de la puce en Pixels de l'écran (320x240)
    int pixelX = map(p.x, TS_MINX, TS_MAXX, 0, 320);
    int pixelY = map(p.y, TS_MINY, TS_MAXY, 0, 240);
    
    // Sécurité pour ne pas dépasser les bords de l'écran
    pixelX = constrain(pixelX, 0, 320);
    pixelY = constrain(pixelY, 0, 240);
    
    // Affichage sur le moniteur série
    Serial.print("Brut X:"); Serial.print(p.x);
    Serial.print(" Y:"); Serial.print(p.y);
    Serial.print("  ->  Pixels X:"); Serial.print(pixelX);
    Serial.print(" Y:"); Serial.println(pixelY);
    
    // Dessiner un petit carré
    tft.fillRect(320 - pixelX, pixelY - 2, 4, 4, ILI9341_WHITE);
    
  }
}