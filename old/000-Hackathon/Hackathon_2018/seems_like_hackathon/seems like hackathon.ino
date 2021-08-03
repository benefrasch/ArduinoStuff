#include <ESP8266WiFi.h>
#include <Adafruit_BME680.h>
#include <Wire.h>
#include <Ticker.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include <Adafruit_NeoPixel.h>

//--------------------------------------- HTTP-Get
int httpGET(String host, String cmd, String &antwort,int Port) {
  WiFiClient client; // Client über WiFi
  String text = "GET http://"+ host + cmd + " HTTP/1.1\r\n";
  text = text + "Host:" + host + "\r\n";
  text = text + "Connection:close\r\n\r\n";
  int ok = 1;
  if (ok) { // Netzwerkzugang vorhanden 
    ok = client.connect(host.c_str(),Port);// verbinde mit Client
    if (ok) {
      client.print(text);                 // sende an Client 
      for (int tout=1000;tout>0 && client.available()==0; tout--)
        delay(10);                         // und warte auf Antwort
      if (client.available() > 0)         // Anwort gesehen 
        while (client.available())         // und ausgewertet
          antwort = antwort + client.readStringUntil('\r');
      else ok = 0;
      client.stop(); 
      Serial.println(antwort);
    } 
  } 
  if (!ok) Serial.print(" no connection"); // Fehlermeldung
  return ok;
}

int Temp = 0 ;
int Luftfeuchte = 0 ;
int co = 0 ;
// BME680 Lib written by Limor Fried & Kevin Townsend for Adafruit Industries, http://www.adafruit.com/products/3660
Adafruit_BME680 boschBME680; // Objekt Bosch Umweltsensor
String matrixausgabe_text  = " "; // Ausgabetext als globale Variable

volatile int matrixausgabe_index = 0;// aktuelle Position in Matrix

volatile int matrixausgabe_wait  = 0;// warte bis Anzeige durchgelaufen ist

//--------------------------------------- Charlieplex Matrix
Ticker matrixausgabe;
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

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2,13,NEO_GRBW + NEO_KHZ800);


void setup(){ // Einmalige Initialisierung
  Serial.begin(115200);
  Wire.begin(); // ---- Initialisiere den I2C-Bus 
  if (Wire.status() != I2C_OK) Serial.println("Something wrong with I2C");

  if (!boschBME680.begin(118)) { 
    Serial.println("Failed to communicate BME680");
    while (1) {
      delay(1);
    };
  }

  // Set up Bosch BME 680
  boschBME680.setTemperatureOversampling(BME680_OS_8X);
  boschBME680.setHumidityOversampling(BME680_OS_2X);
  boschBME680.setPressureOversampling(BME680_OS_4X);
  boschBME680.setIIRFilterSize(BME680_FILTER_SIZE_3);
  boschBME680.setGasHeater(320, 150); // 320*C for 150 ms

  matrix.begin();// Matrix initialisieren 
  delay(10);
  matrix.clear(); 
  matrix.setTextColor(60); // Helligkeit begrenzen 
  matrixausgabe.attach(0.08, matrixUpdate); // zyklisch aktualisieren

  Serial.println();
  pixels.begin();//-------------- Initialisierung Neopixel
  delay(1);
  pixels.show();
  pixels.setPixelColor(0,0,0,0); // alle aus
  pixels.setPixelColor(1,0,0,0);
  pixels.show();                 // und anzeigen

}

void loop() { // Kontinuierliche Wiederholung 
  Luftfeuchtigkeit();
  Temperatur();
  co2();

  { //Block------------------------------ sende Daten an Thingspeak (mit http GET) 
    Serial.println("\nThingspeak update ");
    String cmd = "/update?api_key="+ String("2H71DFR7CWVZHCUY");
    String host = "thingspeak.com";
    String antwort= " ";
    cmd = cmd +String("&field1="+String(Temp)
      +"&field2="+String(Luftfeuchte)
      +"&field3="+String(co))+ "\n\r";
    httpGET(host,cmd,antwort,80);// und absenden 
  } // Blockende
}

void Temperatur()
{
  Temp = ( abs( boschBME680.readTemperature() ) - 4 ) ;
  matrixAnzeige(String(String(Temp)),1);
  Serial.print("Temperatur"+String(Temp));
  Serial.print("C");
  Serial.println();
}

void co2()
{
  if (( ( co ) < ( 1500 ) ))
  {
    pixels.setPixelColor(1,0,30,0);
    pixels.show();
  }
  else
  {
    if (( ( ( co ) < ( 2000 ) ) && ( ( co ) > ( 1500 ) ) ))
    {
      pixels.setPixelColor(1,30,30,0);
      pixels.show();
    }
    else
    {
      pixels.setPixelColor(1,40,0,0);
      pixels.show();
    }
  }
  Serial.print("CO2"+String(( co / 10000 )));
  Serial.print("%");
  Serial.println();
}

void Luftfeuchtigkeit()
{
  Luftfeuchte = boschBME680.readHumidity() ;
  if (( ( Luftfeuchte ) > ( 45 ) ))
  {
    pixels.setPixelColor(0,0,30,0);
    pixels.show();
  }
  else
  {
    if (( ( ( Luftfeuchte ) > ( 35 ) ) && ( ( Luftfeuchte ) < ( 45 ) ) ))
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
  Serial.print("Luftfeuchte:"+String(Luftfeuchte));
  Serial.print("%");
  Serial.println();
}
