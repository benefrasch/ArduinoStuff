#include <Adafruit_BME680.h>
#include <Wire.h>
#include <Ticker.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <EEPROM.h>

#include "sensirion_common.h"
#include "sgp30.h"

#define LOOP_TIME_INTERVAL_MS  1000
#define BASELINE_IS_STORED_FLAG  (0X55)


#include <ESP8266WiFi.h>

String matrixausgabe_text  = " "; // Ausgabetext als globale Variable

volatile int matrixausgabe_index = 0;// aktuelle Position in Matrix
//#define ARRAY_TO_U32(a)  (a[0]<<24|a[1]<<16|a[2]<<8|a[3])    //MSB first  //Not suitable for 8-bit platform
u16 tvoc_ppb, co2_eq_ppm;

void array_to_u32(u32 *value, u8* array)
{
  (*value) = (*value) | (u32)array[0] << 24;
  (*value) = (*value) | (u32)array[1] << 16;
  (*value) = (*value) | (u32)array[2] << 8;
  (*value) = (*value) | (u32)array[3];
}
void u32_to_array(u32 value, u8* array)
{
  if (!array)
    return;
  array[0] = value >> 24;
  array[1] = value >> 16;
  array[2] = value >> 8;
  array[3] = value;
}

/*
   Reset baseline per hour,store it in EEPROM;
*/
void  store_baseline(void)
{
  static u32 i = 0;
  u32 j = 0;
  u32 iaq_baseline = 0;
  u8 value_array[4] = {0};
  i++;
  Serial.println(i);
  if (i == 3600)
  {
    i = 0;
    if (sgp_get_iaq_baseline(&iaq_baseline) != STATUS_OK)
    {
      Serial.println("get baseline failed!");
    }
    else
    {
      Serial.println(iaq_baseline, HEX);
      Serial.println("get baseline");
      u32_to_array(iaq_baseline, value_array);
      for (j = 0; j < 4; j++) {
        EEPROM.write(j, value_array[j]);
        Serial.print(value_array[j]);
        Serial.println("...");
      }
      EEPROM.write(j, BASELINE_IS_STORED_FLAG);
    }
  }
  delay(LOOP_TIME_INTERVAL_MS);
}

/*Read baseline from EEPROM and set it.If there is no value in EEPROM,retrun .
   Another situation: When the baseline record in EEPROM is older than seven days,Discard it and return!!

*/
void set_baseline(void)
{
  u32 i = 0;
  u8 baseline[5] = {0};
  u32 baseline_value = 0;
  for (i = 0; i < 5; i++)
  {
    baseline[i] = EEPROM.read(i);
    Serial.print(baseline[i], HEX);
    Serial.print("..");
  }
  Serial.println("!!!");
  if (baseline[4] != BASELINE_IS_STORED_FLAG)
  {
    Serial.println("There is no baseline value in EEPROM");
    return;
  }
  /*
    if(baseline record in EEPROM is older than seven days)
    {
    return;
    }
  */
  array_to_u32(&baseline_value, baseline);
  sgp_set_iaq_baseline(baseline_value);
  Serial.println(baseline_value, HEX);
}



//--------------------------------------- HTTP-Get
int httpGET(String host, String cmd, String &antwort, int Port) {
  WiFiClient client; // Client über WiFi
  String text = "GET http://" + host + cmd + " HTTP/1.1\r\n";
  text = text + "Host:" + host + "\r\n";
  text = text + "Connection:close\r\n\r\n";
  int ok = 1;
  if (ok) { // Netzwerkzugang vorhanden
    ok = client.connect(host.c_str(), Port); // verbinde mit Client
    if (ok) {
      client.print(text);                 // sende an Client
      for (int tout = 1000; tout > 0 && client.available() == 0; tout--)
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

volatile int matrixausgabe_wait  = 0;// warte bis Anzeige durchgelaufen ist

//--------------------------------------- Charlieplex Matrix
Ticker matrixausgabe;
Adafruit_IS31FL3731_Wing matrix = Adafruit_IS31FL3731_Wing();
void matrixUpdate() { // Update Charlieplexmatrix über Ticker
  int anzahlPixel = (matrixausgabe_text.length()) * 6;
  if (anzahlPixel > 15) { // Scrollen
    if (matrixausgabe_index >= -anzahlPixel) {
      matrix.clear();
      matrix.setCursor(matrixausgabe_index, 0);
      matrix.print(matrixausgabe_text);
      matrixausgabe_index--;
    }
    else {
      matrixausgabe_index = 0;
      matrixausgabe_wait = 0;
    }
  }
  else {// nur 3 Zeichen lang-> kein Scroll
    matrix.setCursor(0, 0);
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

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(2, 13, NEO_GRBW + NEO_KHZ800);
int keinwlan = 1;

void setup() { // Einmalige Initialisierung
  Serial.begin(115200);
    //------------------------------------------------------------------------------------------
    //------------ WLAN initialisieren
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    delay(100);
    Serial.print ("\nWLAN connect to:");
    Serial.print("sghGeraete");
    WiFi.begin("sghGeraete", "(3ljD*F-6:q0?");
    while (WiFi.status() != WL_CONNECTED) { // Warte bis Verbindung steht
      delay(500);
      Serial.print(".");
    };
    Serial.println ("\nconnected, meine IP:" + WiFi.localIP().toString());
    matrixausgabe_text = " Meine IP:" + WiFi.localIP().toString();
    matrixausgabe_index = 0;
    keinwlan == 0;
  
  s16 err;
  u16 scaled_ethanol_signal, scaled_h2_signal;

  Serial.println("serial start!!");



  /*Init module,Reset all baseline,The initialization takes up to around 15 seconds, during which
    all APIs measuring IAQ(Indoor air quality ) output will not change.Default value is 400(ppm) for co2,0(ppb) for tvoc*/
  while (sgp_probe() != STATUS_OK) {
    Serial.println("SGP failed");
    while (1);
  }
  /*Read H2 and Ethanol signal in the way of blocking*/
  err = sgp_measure_signals_blocking_read(&scaled_ethanol_signal,
                                          &scaled_h2_signal);
  if (err == STATUS_OK) {
    Serial.println("get ram signal!");
  } else {
    Serial.println("error reading signals");
  }
  // err = sgp_iaq_init();
  set_baseline();
  //

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
  pixels.setPixelColor(0, 0, 0, 0); // alle aus
  pixels.setPixelColor(1, 0, 0, 0);
  pixels.show();                 // und anzeigen

}

void loop() { // Kontinuierliche Wiederholung

  s16 err = 0;

  err = sgp_measure_iaq_blocking_read(&tvoc_ppb, &co2_eq_ppm);
  if (err == STATUS_OK) {


  } else {
    Serial.println("error reading IAQ values\n");
  }
  store_baseline();

  Luftfeuchtigkeit();
  Temperatur();
  co2();

    Serial.println("\nThingspeak update ");
    String cmd = "/update?api_key=" + String("JHF4QG01C66G1UXI");
    String host = "api.thingspeak.com";
    String antwort = " ";
    cmd = cmd + String("&field1=" + String(Temp)
                       + "&field2=" + String(Luftfeuchte)
                       + "&field3=" + String(co2_eq_ppm)) + "\n\r";
    httpGET(host, cmd, antwort, 80); // und absenden

}

void Temperatur()
{
  Temp = ( abs( boschBME680.readTemperature() ) - 4 ) ;
  matrixAnzeige(String(String(Temp)), 1);
  Serial.print("Temperatur: " + String(Temp));
  Serial.print("C");
  Serial.println();
}

void co2()
{
  if (( ( co2_eq_ppm ) < ( 1500 ) ))
  {
    pixels.setPixelColor(1, 0, 30, 0);
    pixels.show();
  }
  else
  {
    if (( ( ( co2_eq_ppm ) < ( 2000 ) ) && ( ( co2_eq_ppm ) > ( 1500 ) ) ))
    {
      pixels.setPixelColor(1, 30, 30, 0);
      pixels.show();
    }
    else
    {
      pixels.setPixelColor(1, 40, 0, 0);
      pixels.show();
    }
  }
  Serial.print("CO2: " + String(( co2_eq_ppm )));
  Serial.print("ppm");
  Serial.println();
}

void Luftfeuchtigkeit()
{
  Luftfeuchte = boschBME680.readHumidity() ;
  if (( ( Luftfeuchte ) > ( 45 ) ))
  {
    pixels.setPixelColor(0, 0, 30, 0);
    pixels.show();
  }
  else
  {
    if (( ( ( Luftfeuchte ) > ( 35 ) ) && ( ( Luftfeuchte ) < ( 45 ) ) ))
    {
      pixels.setPixelColor(0, 30, 30, 0);
      pixels.show();
    }
    else
    {
      pixels.setPixelColor(0, 40, 0, 0);
      pixels.show();
    }
  }
  Serial.print("Luftfeuchte: " + String(Luftfeuchte));
  Serial.print("%");
  Serial.println();
}
