#include <SparkFunBME280.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>

// Marshall Tylor@sparkfun  https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
BME280 boschBME280; // Objekt Bosch Umweltsensor
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2,13,NEO_GRBW + NEO_KHZ800);
String matrixausgabe_text  = " "; // Ausgabetext als globale Variable

volatile int matrixausgabe_index = 0;// aktuelle Position in Matrix

volatile int matrixausgabe_wait  = 0;// warte bis Anzeige durchgelaufen ist

//--------------------------------------- Charlieplex Matrix
Ticker matrixausgabe;
#include <Wire.h>              // LED-Matrix (CharlieWing-Addon)
#include <Adafruit_GFX.h>      // und Text Ausgabe darauf
#include <Adafruit_IS31FL3731.h>
Adafruit_IS31FL3731_Wing matrix = Adafruit_IS31FL3731_Wing();
void matrixUpdate(){ // Update Charlieplexmatrix über Ticker
  int anzahlPixel=(matrixausgabe_text.length())*6;
  if (anzahlPixel > 15) { // Scrollen 
    if (matrixausgabe_index >= -anzahlPixel) {
      matrix.clear();
      matrix.setCursor(matrixausgabe_index,0);
      matrix.print(matrixausgabe_text);
      matrixausgabe_index--;
    } 
    else {
      matrixausgabe_index = 0;
      matrixausgabe_wait=0;
    }
  } 
  else {// nur 3 Zeichen lang-> kein Scroll 
    matrix.setCursor(0,0);
    matrix.print(matrixausgabe_text);
    matrixausgabe_wait  = 0; // no wait
  }
}
void matrixAnzeige(String text, int wait) { // Setze Ausgabetext
  if (matrixausgabe_text  != text) { // update nur bei neuem Text 
    matrix.clear();
    matrixausgabe_index = 0;
    matrixausgabe_text  = text;
    matrixausgabe_wait  = wait;
    while (matrixausgabe_wait) delay(10); // warte bis Text einmal ausgegeben ist
  }
};


void Temperatur();
void Luftfeuchtigkeit();

void setup(){ // Einmalige Initialisierung
  Wire.begin(); // ---- Initialisiere den I2C-Bus 
  if (Wire.status() != I2C_OK) Serial.println("Something wrong with I2C");

  boschBME280.settings.runMode = 3; // Normal Mode
  boschBME280.settings.tempOverSample  = 4; 
  boschBME280.settings.pressOverSample = 4;
  boschBME280.settings.humidOverSample = 4;
  boschBME280.begin();
  pixels.begin();//-------------- Initialisierung Neopixel
  delay(1);
  pixels.show();
  pixels.setPixelColor(0,0,0,0); // alle aus
  pixels.setPixelColor(1,0,0,0);
  pixels.show();                 // und anzeigen

  Serial.begin(115200);
  Serial.println();
  matrix.begin();// Matrix initialisieren 
  delay(10);
  matrix.clear(); 
  matrix.setTextColor(60); // Helligkeit begrenzen 
  matrixausgabe.attach(0.08, matrixUpdate); // zyklisch aktualisieren

}

void loop() { // Kontinuierliche Wiederholung 
  Luftfeuchtigkeit();
  Temperatur();
}

void Luftfeuchtigkeit()
{
  if (( ( boschBME280.readFloatHumidity() ) > ( 45 ) ))
  {
    pixels.setPixelColor(0,0,30,0);
    pixels.show();
  }
  else
  {
    if (( ( ( boschBME280.readFloatHumidity() ) > ( 40 ) ) || ( ( boschBME280.readFloatHumidity() ) < ( 45 ) ) ))
    {
      pixels.setPixelColor(0,30,30,0);
      pixels.show();
    }
    else
    {
      pixels.setPixelColor(0,40,0,0);
      pixels.show();
    }
  }
  Serial.print("Luftfeuchte:"+String(boschBME280.readFloatHumidity()));
  Serial.println();
}

void Temperatur()
{
  matrixAnzeige(String("Temp:"+String(boschBME280.readTempC())),1);
  Serial.print("Temperatur"+String(boschBME280.readTempC()));
  Serial.println();
}


