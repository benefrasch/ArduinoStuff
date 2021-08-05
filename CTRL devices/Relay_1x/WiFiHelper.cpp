#include "WiFiHelper.h"

void WiFiHelper::WiFiConnect(char SSID[], char password[])
{
  //------------ WLAN initialisieren
  WiFi.disconnect();
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.print("\nWLAN connect: ");
  WiFi.begin(SSID, password);
  int WifiTries = 0;
  while (WiFi.status() != WL_CONNECTED && WifiTries < 10)
  { //will try to connect for 5 secs
    delay(500);
    Serial.print(".");
    ++WifiTries;
  };
  if (WiFi.status() == WL_CONNECTED)
    Serial.println("  ...connected, meine IP: " + WiFi.localIP().toString() + "\n");
  else
    Serial.println("  ...could not connect");
}