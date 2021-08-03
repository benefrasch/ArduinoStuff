#ifndef DIY_h
 #define DIY_h
 #include <Arduino.h>
 #include <ESP8266WiFi.h>
 #include <Adafruit_Sensor.h>
 #include <Adafruit_TSL2561_U.h>
 #include <Adafruit_NeoPixel.h>
 
/*--------------------------------------------------
  Photometer Programm
  ein LED Photometer mit dem Lichtsensor TSL2561,
  einer dreifarbigen LED (RGB-LED) und
  mit eigenem WIFI AccessPoint und kleinem Webserver

  HAW Hamburg - Labor BPA - Ulrich Scheffler
  Version 0.30, 02.11.2017
  modified for iot-werkstatt by iot-werkstatt umwelt-campus Birkenfeld oct 2019
*/
void DIY_SpectroOctiSetup(int, int);
void DIY_SpectroOctiLoop();

extern Adafruit_NeoPixel pixels ;
extern Adafruit_TSL2561_Unified tsl ;

#endif
