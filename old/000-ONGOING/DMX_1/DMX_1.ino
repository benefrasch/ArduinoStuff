#define rx 2
#define tx 3

char command;
int channel, value;

#include <SoftwareSerial.h>
#include <DmxSimple.h>


SoftwareSerial bt(rx, tx); // RX, TX

void setup() {

  bt.begin(9600);


}

void loop() {

  while ( bt.available() >= 1 ) {
    command = bt.read();

    if (command = "c") {
      for (int e; e < 20; e++) {
        if (bt.available() >= 1) {
          channel = bt.read();
          e = 21;
        } else {
          delay(200);
        }
      }
    }

    if (command = "i") {
      for (int e; e < 20; e++) {
        if (bt.available() >= 1) {
          value = bt.read();
          e = 21;
        } else {
          delay(200);
        }
      }
    }

    if (command = "f"){
      DmxSimple.write(channel, value);
    }



  }


}
