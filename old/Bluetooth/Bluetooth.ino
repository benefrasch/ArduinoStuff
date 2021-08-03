#include <SoftwareSerial.h>



SoftwareSerial bt(D1, D2); //RX,TX

void setup() {

  Serial.begin(9600);

  bt.begin(9600);

}

void loop()

{
  if (bt.available())
  {
    Serial.print(bt.read());
  }

  if (Serial.available())
  {
    // Serial.print((char)Serial.read());
    bt.print(Serial.read());
  }



}
