
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

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
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 1){
    digitalWrite(12 , HIGH);
  } else {
    digitalWrite(12, LOW);
  }
  // process received value
}


void setup()
{
  delay(1000);
  // Debug console
  Serial.begin(115200);
  Serial.println("Serial started");
  pinMode(12, OUTPUT);
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Blynk.run();
}
