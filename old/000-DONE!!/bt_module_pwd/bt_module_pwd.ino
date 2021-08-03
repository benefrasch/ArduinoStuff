//--------------------------- legt Passwörter fest
char pwd1[] = "Gc3c^z*%|nQjwjuQ;!B6/.23>!S>OysHM~?X%Dxg4mSRYnB=.H3iY$8#YX4TVS:>eqIi.q$W/y2K.wu!JEU<XGb%-W71A=k.D3HP";
char pwd2[] = "Ff0<rRt|cZ,+W;Cff8A?>T3m>SVD~+8s=b^zqzVrirj0cy;JL2b^.,i<|76r`Dezx8Rd5WTawr6#5S3g0vnF$`+!7V*d2=HC!VGk";
char pwd3[] = ">I.6Wtg_7v~NhIaFPN!qAHz/$fv*k2Y*X=9D/VU~Lg<iR-1Kv_BE5SNGusgqf0Bcw<2sCB/~&&;WHVXI~Ft:luwaQ4$,csR:5,F*";
char pwd4[] = "test4";
char pwd5[] = "test5";
char pwd6[] = "test6";

//--------------------------- bindet Bibliotheken ein und startet SoftwareSerial

#include <Keyboard.h>
#include <SoftwareSerial.h>

SoftwareSerial bt(8, 9); // RX;TX

char temp = "";

//---------------------------SETUP

void setup() {

  //--------------------------- Initialisiert Serial

  Serial.begin(9600);
  /*
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Native USB only
  }
  Serial.println("Serial started");
  */

  //---------------------------Initialisiert SoftwareSerial als bt

  bt.begin(9600);
  delay(1000);
  /*
    bt.write("AT+NAMEboringmodule");
    bt.write("AT+PIN032510");
    bt.write("AT+TYPE2");
  */
  //---------------------------Startet die Keyboard Library

  Keyboard.begin();

}

//--------------------------- LOOP

void loop() {

  //--------------------------- Verarbeitet Serial Nachrichten
/*
  if (Serial.available()) {
    bt.write(Serial.read());
  }
*/
  //--------------------------- Verarbeitet bt Nachrichten

  if (bt.available()) {

    temp = bt.read();
    Serial.print("Received Message: ");
    Serial.print(temp);
    Serial.println();

    Serial.print("Puffer:");
    Serial.print( bt.available());
    Serial.println();

    if (temp == '1') {
      Serial.println("pwd1");
      Keyboard.print(pwd1);

    } else if (temp == '2') {
      Serial.println("pwd2");
      Keyboard.print(pwd2);

    } else if (temp == '3') {
      Serial.println("pwd3");
      Keyboard.print(pwd3);

    } else if (temp == '4') {
      Serial.println("pwd4");
      Keyboard.print(pwd4);

    } else if (temp == '5') {
      Serial.println("pwd5");
      Keyboard.print(pwd5);

    } else if (temp == '6') {
      Serial.println("pwd6");
      Keyboard.print(pwd6);

    } /*else {
      Serial.println("no command");
      // Keyboard.print("test");
    } */

    while (bt.available() > 0) {
      temp = bt.read();
      delay (500);
      temp = bt.read();
    }

    temp = "";
  }


}
