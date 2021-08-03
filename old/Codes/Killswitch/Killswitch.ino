//#include <Blynk.h>

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

int ETan = 0;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "235f8d68859c45b48ce31050d7f79d60";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "benefrasch@gmail.com";
char pass[] = "D@sistM1WLAN";

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin V1
BLYNK_WRITE(V1)
{
  int pinValue1 = param.asInt(); // assigning incoming value from pin V1 to a variable
  
  // process received value
}
BLYNK_WRITE(V2)
{
  int pinValue1 = param.asInt(); // assigning incoming value from pin V1 to a variable

  // process received value
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop(){
  Blynk.run();
  Serial.println(".");
}
