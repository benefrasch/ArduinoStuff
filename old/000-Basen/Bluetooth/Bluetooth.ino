#include <SoftwareSerial.h>

int bluetoothTx = 2; // D2 to TXD
int bluetoothRx = 3; // D3 to RXD  (Warning! See Text)
SoftwareSerial bt(bluetoothTx, bluetoothRx);

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
