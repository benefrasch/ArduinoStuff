#ifndef WifiHelper_h
#define WiFiHelper_h

#include "Arduino.h"
#include <ESP8266WiFi.h>

class WiFiHelper : WiFiClient
{
    public:
        WiFiClient client;
        void WiFiConnect(char SSID[], char password[]);

};
#endif 